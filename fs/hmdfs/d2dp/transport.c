// SPDX-License-Identifier: GPL-2.0
/*
 * D2D transport implementation
 */

#define pr_fmt(fmt) "[D2D]: " fmt

#include <linux/atomic.h>
#include <linux/compiler.h>
#include <linux/errno.h>
#include <linux/if_ether.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/net.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/spinlock.h>
#include <linux/spinlock_types.h>
#include <linux/wait.h>

#include "d2d.h"
#include "buffer.h"
#include "headers.h"
#include "params.h"
#include "protocol.h"
#include "timers.h"
#include "transport.h"
#include "trace.h"
#include "wrappers.h"

static const struct cred *d2d_override_creds(struct d2d_protocol *proto)
{
	if (proto->security && proto->security->cred)
		return override_creds(proto->security->cred);

	return NULL;
}

static void d2d_revert_creds(const struct cred *creds)
{
	if (creds)
		revert_creds(creds);
}

/**
 * finish_transport_thread - close the transport thread in safe manner
 * @proto: D2DP instance
 * @code: exit code to set to the protocol instance
 *
 * The function performs safe thread termination. It tries to set exit code
 * atomically, then wakes up all waiters, zeroize the corresponding task under
 * the lock and finally push the completion. Otherwise, if the threads return
 * early, the TX and RX task pointers become garbage and the protocol will
 * crash.
 */
static void finish_transport_thread(struct d2d_protocol *proto, int code,
				    struct d2dp_thread_state *self)
{
	struct d2dp_transport_threads *transport = &proto->transport;

	atomic_cmpxchg(&proto->exitcode, 0, code);
	d2dp_wake_up_transport(proto);

	mutex_lock(&transport->exit_completion_lock);
	self->task = NULL;
	mutex_unlock(&transport->exit_completion_lock);

	complete(&self->exit);
}

static void finish_tx_transport_thread(struct d2d_protocol *proto, int code)
{
	finish_transport_thread(proto, code, &proto->transport.tx);
}

static void finish_rx_transport_thread(struct d2d_protocol *proto, int code)
{
	finish_transport_thread(proto, code, &proto->transport.rx);
}

static bool rx_buf_check_limit_exceeded(const struct rx_buffer *rxb)
{
	size_t fill_percentage = rx_buf_get_fill_percentage(rxb);

	return fill_percentage > D2D_BUFFER_ALLOWED_FILL_PERCENTAGE;
}

static void prepare_ack(struct d2d_protocol *proto)
{
	struct tx_handler *tx = &proto->tx;
	struct rx_handler *rx = &proto->rx;
	struct d2d_header *ackhdr = NULL;
	struct ack_data *ack_data = NULL;
	struct sack_pairs pairs = { 0 };
	struct tx_ack_state *ack_state = &tx->ack_state;
	size_t sack_max     = 0;
	bool limit_exceeded = false;
	int force_ack       = 0;

	/*
	 * Take two locks here - to protect the ack packet from data races and
	 * to serialize access to the RX buffer.
	 */
	spin_lock(&tx->lock);

	/* Set pointer to ack that will be filled */
	ack_data = ack_state->ack_to_fill;
	ackhdr = &ack_data->ack_header;

	pairs.pairs = ackhdr->sack_pairs;
	pairs.len = 0;
	sack_max = sizeof(ackhdr->sack_pairs) / SACK_PAIR_SIZE;

	spin_lock(&rx->lock);
	/* Get ACK id and SACK pairs */
	ackhdr->seq_id = rx_buf_get_ack(rx->buffer);
	rx_buf_generate_sack_pairs(rx->buffer, &pairs, sack_max);
	ackhdr->data_len = pairs.len;

	/* Check RX buffer limit exceeded */
	limit_exceeded = rx_buf_check_limit_exceeded(rx->buffer);
	spin_unlock(&rx->lock);

	force_ack = atomic_cmpxchg(&ack_state->force_ack, 1, 0);
	/*
	 * Do not set ack_ready flag if there are no SACK pairs, ack_id equals
	 * to previous one and flowcontrol flag was not changed.
	 */
	if (d2d_wrap_eq(ackhdr->seq_id, ack_state->last_acked) &&
	    limit_exceeded == ack_state->last_limit_exceeded &&
	    pairs.len == 0 && !force_ack) {
		goto not_ready;
	}
	ack_state->last_acked = ackhdr->seq_id;
	ack_state->last_limit_exceeded = limit_exceeded;

	/* Set the flags */
	ackhdr->flags = D2D_FLAG_ACK;
	/* Set flowcontrol flag if RX buffer limit was exceeded */
	ackhdr->flags |= limit_exceeded ? D2D_FLAG_SUSPEND : 0;

	/* Don't forget to update `ready` flag! */
	ack_data->ack_ready = true;
not_ready:
	spin_unlock(&tx->lock);
}

bool ack_timer_action(void *arg)
{
	struct d2d_protocol *proto = arg;
	struct tx_handler *tx = &proto->tx;

	trace_d2d_ack_timer(1);
	prepare_ack(proto);
	trace_d2d_ack_timer(0);

	/* Wake up TX thread to send this ack packet */
	wake_up_interruptible_all(&tx->wq_data);

	/* ACK timer should restart itself */
	return true;
}

bool rto_timer_action(void *arg)
{
	struct d2d_protocol *proto = arg;
	struct tx_handler *tx = &proto->tx;
	struct d2d_timer *self = &proto->timers.rto_timer;
	bool restart = true;

	if (atomic_read_acquire(&proto->exitcode) ||
	    atomic_read_acquire(&proto->closing))
		return false;

	trace_d2d_rto_timer(1);

	spin_lock(&tx->lock);
	tx_buf_mark_as_need_resent(tx->buffer);
	spin_unlock(&tx->lock);

	self->retry_num++;

	/* double delay time if number of retries exceeds the threshold */
	if (self->retry_num > D2D_RETRY_LIMIT_MIN)
		self->delay_usecs *= 2;

	if (self->retry_num > D2D_RETRY_LIMIT_MAX) {
		/* RTO timer fired too many times - link failure detected */
		atomic_cmpxchg(&proto->exitcode, 0, -EIO);
		restart = false;
	}

	/* Wake up TX thread to resend marked packets */
	wake_up_interruptible_all(&tx->wq_data);

	trace_d2d_rto_timer(0);
	return restart;
}

static int sock_recv_packet(struct socket *sock, char *buffer, size_t buflen)
{
	struct msghdr msg = { 0 };
	struct kvec iov[] = {
		{
			.iov_base = buffer,
			.iov_len  = buflen,
		},
	};

	/* set MSG_TRUNC here to catch oversized datagrams */
	return kernel_recvmsg(sock, &msg, iov, ARRAY_SIZE(iov),
			      buflen, MSG_TRUNC);
}

static int sock_send_packet(struct socket *sock, char *buffer, size_t buflen)
{
	struct msghdr msg = { 0 };
	struct kvec iov[] = {
		{
			.iov_base = buffer,
			.iov_len  = buflen,
		},
	};

	return kernel_sendmsg(sock, &msg, iov, ARRAY_SIZE(iov), buflen);
}

static bool tx_wait_cond_lock(struct d2d_protocol *proto, int *reason)
{
	bool wakeup = false;
	struct tx_handler *tx = &proto->tx;

	if (unlikely(atomic_read_acquire(&proto->exitcode))) {
		*reason = TX_WAKE_REASON_PROTO_ERROR;
		return true;
	}

	if (unlikely(atomic_read_acquire(&proto->closing))) {
		*reason = TX_WAKE_REASON_CLOSING;
		return true;
	}

	spin_lock(&tx->lock);

	wakeup |= tx_buf_has_packets_to_send(tx->buffer);
	wakeup |= tx->ack_state.ack_to_send->ack_ready;
	if (wakeup) {
		/*
		 * Do not release the lock if wakeup reason is OK. We do that to
		 * reduce the TX lock pressure and improve the performance
		 */
		*reason = TX_WAKE_REASON_OK;
		return true;
	}

	/* if no need to wakeup, release the lock */
	spin_unlock(&tx->lock);
	return wakeup;
}

static int tx_wait_data_lock(struct d2d_protocol *proto)
{
	int ret = 0;
	int reason = TX_WAKE_REASON_OK;

	ret = wait_event_interruptible(proto->tx.wq_data,
				       tx_wait_cond_lock(proto, &reason));
	if (ret == -ERESTARTSYS && reason == TX_WAKE_REASON_OK)
		reason = TX_WAKE_REASON_ERESTARTSYS;

	return reason;
}

static int tx_transport_encrypt(struct d2d_security *security,
				struct net_frames *frames,
				size_t length)
{
	int ret = length;

	if (security) {
		mutex_lock(&frames->crypto_tx_mtx);
		ret = security->encrypt(security, frames->tx_frame, length,
					frames->tx_encr_frame, frames->len);
		mutex_unlock(&frames->crypto_tx_mtx);
	}

	return ret;
}

static int tx_transport_send_ack(struct d2d_protocol *proto,
				 struct d2d_header *ackhdr)
{
	size_t hdrlen = 0;
	int to_send   = 0;
	void *sendbuf = NULL;
	struct net_frames *frames = &proto->frames;

	/* set packet number field */
	proto->tx_packet_id++;
	ackhdr->packet_id = proto->tx_packet_id;

	hdrlen = d2d_header_write(frames->tx_frame, frames->len, ackhdr);

	to_send = tx_transport_encrypt(proto->security, frames, hdrlen);
	if (to_send < 0)
		return to_send;

	sendbuf = (proto->security) ? frames->tx_encr_frame : frames->tx_frame;

	return sock_send_packet(proto->sock, sendbuf, to_send);
}

static int tx_transport_send_data(struct d2d_protocol *proto,
				  const struct d2d_data_packet *packet)
{
	size_t hdrlen = 0;
	size_t pktlen = 0;
	int to_send   = 0;
	void *sendbuf = NULL;
	struct tx_handler *tx     = &proto->tx;
	struct d2d_header *hdr    = &tx->tx_header;
	struct net_frames *frames = &proto->frames;

	/* Prepare the packet header first */
	hdr->flags    = D2D_FLAG_NONE;
	hdr->data_len = packet->len;
	hdr->seq_id   = packet->seq_id;

	/* set packet number field */
	proto->tx_packet_id++;
	hdr->packet_id = proto->tx_packet_id;

	/* serialize the header to bytes */
	hdrlen = d2d_header_write(frames->tx_frame, frames->len, hdr);

	/* copy the data to frame */
	pktlen = packet->len;
	memcpy(frames->tx_frame + hdrlen, packet->data, pktlen);

	to_send = tx_transport_encrypt(proto->security,
				       frames, hdrlen + pktlen);
	if (to_send < 0)
		return to_send;

	sendbuf = (proto->security) ? frames->tx_encr_frame : frames->tx_frame;

	return sock_send_packet(proto->sock, sendbuf, to_send);
}

static void tx_switch_ack_pointer(struct tx_ack_state *ack_state,
				  struct ack_data **ack_to_switch)
{
	*ack_to_switch = (*ack_to_switch == &ack_state->acks[0]) ?
		&ack_state->acks[1] :
		&ack_state->acks[0];
}

static void tx_switch_ack_to_fill(struct tx_ack_state *ack_state)
{
	tx_switch_ack_pointer(ack_state, &ack_state->ack_to_fill);
}

static void tx_switch_ack_to_send(struct tx_ack_state *ack_state)
{
	tx_switch_ack_pointer(ack_state, &ack_state->ack_to_send);
}

/**
 * tx_transport_try_ack_locked - try to send ACK packet if ready
 * @proto: D2DP instance
 *
 * This function is called with TX lock being held. The function itself
 * temporary releases the lock to send the ACK packet (if it is ready), but then
 * it re-aquires it to restore invariant and flush TX ack state. Note that the
 * lock is released when the sending operation fails.
 *
 * Returns:
 * 0               - when no acknowledgement has been sent (ACK is not ready)
 * positive number - number of bytes sent (ACK was ready)
 * negative error  - socket operation failed (lock is released in this case)
 */
static int tx_transport_try_ack_locked(struct d2d_protocol *proto)
{
	int ret = 0;
	struct tx_handler *tx = &proto->tx;
	struct tx_ack_state *ack_state = &tx->ack_state;
	struct d2d_header *ackhdr = NULL;

	if (ack_state->ack_to_send->ack_ready) {
		/*
		 * Move ack_to_fill pointer to fill another ack while this one
		 * is in sending state
		 */
		tx_switch_ack_to_fill(ack_state);
		ackhdr = &ack_state->ack_to_send->ack_header;

		/*
		 * Send the acknowledgement without holding the lock. Our header
		 * is protected because ack_to_fill pointer has been switched.
		 */
		spin_unlock(&tx->lock);

		ret = tx_transport_send_ack(proto, ackhdr);
		if (ret < 0)
			return ret;

		trace_d2d_ack_sent(ackhdr);

		/* take the lock back to hold invariant and flush ack state */
		spin_lock(&tx->lock);
		ack_state->ack_to_send->ack_ready = false;
		tx_switch_ack_to_send(ack_state);
	}

	return ret;
}

/**
 * tx_transport_run_workcycle_unlock - perform the TX transport activity
 * @proto: D2DP instance
 *
 * This function is called with TX lock being held. It tries to send all the
 * UNSENT/NEED_RESEND packets from TX buffer and send ACKs when it is ready.
 * This function finally releases the TX lock, whatever it has succeed or
 * failed.
 *
 * Returns: amount of DATA (not ACK) packets sent or negative error.
 */
static int tx_transport_run_workcycle_unlock(struct d2d_protocol *proto)
{
	int ret = 0;
	unsigned int count = 0;
	struct tx_handler *tx = &proto->tx;

	while (true) {
		/* try ACK first to guarantee at least one ACK in whole call */
		ret = tx_transport_try_ack_locked(proto);
		if (ret < 0)
			return ret;

		/* check the packet here to peek it in `peek_packet` */
		if (tx_buf_has_packets_to_send(tx->buffer)) {
			struct d2d_data_packet packet = { 0 };

			/* peek a packet and send it without holding the lock */
			tx_buf_peek_packet(tx->buffer, &packet);
			d2d_timer_disable(&proto->timers.rto_timer);
			spin_unlock(&tx->lock);

			ret = tx_transport_send_data(proto, &packet);
			if (ret < 0)
				return ret;

			spin_lock(&tx->lock);
			ret = tx_buf_peek_return(tx->buffer, &packet);
			if (ret < 0)
				goto err_paranoid;

			count++;
		} else {
			/* if no packets, exit the loop and try another time */
			break;
		}
	}

	if (count)
		d2d_timer_enable(&proto->timers.rto_timer);

	spin_unlock(&tx->lock);
	return count;

err_paranoid:
	spin_unlock(&tx->lock);
	return ret;
}

static int tx_transport_loop(struct d2d_protocol *proto)
{
	int ret = 0;

	while (true) {
		trace_d2d_tx_status(0);
		ret = tx_wait_data_lock(proto);
		trace_d2d_tx_status(1);

		switch (ret) {
		case TX_WAKE_REASON_OK:
			break;
		case TX_WAKE_REASON_PROTO_ERROR:
			return atomic_read_acquire(&proto->exitcode);
		case TX_WAKE_REASON_ERESTARTSYS:
			return -ERESTARTSYS;
		case TX_WAKE_REASON_CLOSING:
			return 0;
		default:
			WARN(true, "Invalid tx wake reason: %d\n", ret);
			return -EINVAL;
		}

		ret = tx_transport_run_workcycle_unlock(proto);
		if (ret < 0)
			return ret;
	}
}

int tx_transport_thread(void *arg)
{
	int err = 0;
	struct d2d_protocol *proto = arg;
	const struct cred *creds = d2d_override_creds(proto);

	err = tx_transport_loop(proto);

	finish_tx_transport_thread(proto, err);
	d2d_revert_creds(creds);
	return 0;
}

static int rx_process_acknowledgement(struct d2d_protocol *proto,
				       struct d2d_header *header)
{
	size_t freed = 0;
	struct tx_handler *tx = &proto->tx;
	bool flowcontrol_mode = false;
	bool key_update_acked = false;
	u32 ack_id = header->seq_id;
	const struct sack_pairs sacks = {
		.pairs = header->sack_pairs,
		.len   = header->data_len,
	};

	trace_d2d_ack_received(header);

	/*
	 * TODO: check that holding lock all the time does not cause performance
	 * problems. Maybe we should reduce the amortization effect here.
	 */
	spin_lock(&tx->lock);
	freed = tx_buf_process_ack(tx->buffer, ack_id, &sacks);
	if (tx_buf_is_empty(tx->buffer)) {
		d2d_timer_reset(&proto->timers.rto_timer);
		d2d_timer_disable(&proto->timers.rto_timer);
	} else if (freed) {
		d2d_timer_reset(&proto->timers.rto_timer);
	}

	/* Check flowcontrol flag and set TX buffer mode */
	flowcontrol_mode = header->flags & D2D_FLAG_SUSPEND;
	tx_buf_set_flowcontrol_mode(tx->buffer, flowcontrol_mode);

	/* check whether TX thread waits for KEY_UPDATE acknowledgement */
	if (atomic_read_acquire(&tx->key_update)) {
		if (d2d_wrap_ge(ack_id, tx->key_update_id)) {
			atomic_set_release(&tx->key_update, 0);
			key_update_acked = true;
		}
	}
	spin_unlock(&tx->lock);

	if (freed)
		wake_up_interruptible_all(&tx->wq_free);
	if (key_update_acked)
		wake_up_interruptible_all(&tx->wq_data);

	return 0;
}

static int rx_process_data(struct d2d_protocol *proto,
			    const struct d2d_header *header,
			    const void *data)
{
	enum rx_buf_put_result res = RX_BUF_PUT_OK;
	const struct d2d_data_packet packet = {
		.data = (void *)data, /* compiler wants this cast to be here */
		.len = header->data_len,
		.seq_id = header->seq_id,
	};
	struct rx_handler *rx = &proto->rx;
	struct tx_handler *tx = &proto->tx;
	struct tx_ack_state *ack_state = &tx->ack_state;

	/* try to put packet to rx buffer */
	spin_lock(&rx->lock);
	res = rx_buf_put(rx->buffer, &packet);
	spin_unlock(&rx->lock);

	switch (res) {
	case RX_BUF_PUT_OK:
		/* successfully put the packet - wake up the reader */
		wake_up_interruptible_all(&rx->wq_data);
		break;
	case RX_BUF_PUT_CONSUMED:
	case RX_BUF_PUT_DUPLICATE:
		/* set flag to force ACK sending if duplicate received */
		atomic_cmpxchg(&ack_state->force_ack, 0, 1);
		break;
	case RX_BUF_PUT_OVERFLOW:
		/* no free space in the RX buffer - drop the packet */
		break;
	default:
		/* buggy or malicious packet - better safe than sorry */
		WARN(true, "bad D2DP packet, err=%d: len=%zu, seq=%u\n",
		     res, packet.len, packet.seq_id);
		return -EINVAL;
	}

	/* activate ACK timer */
	d2d_timer_enable(&proto->timers.ack_timer);

	return 0;
}

static int rx_process_packet(struct d2d_protocol *proto,
			      struct d2d_header *header,
			      const void *data)
{
	if (header->flags & D2D_FLAG_ACK)
		return rx_process_acknowledgement(proto, header);
	else
		return rx_process_data(proto, header, data);
}

static int rx_transport_recv(struct d2d_protocol *proto,
			     int *result)
{
	int ret = 0;
	void *recvbuf = NULL;
	struct net_frames *frames = &proto->frames;

	/* choose the right buffer to store input data from the wire */
	recvbuf = (proto->security) ? frames->rx_encr_frame : frames->rx_frame;

	ret = sock_recv_packet(proto->sock, recvbuf, frames->len);
	if (unlikely(ret < 0)) {
		if (ret == -ERESTARTSYS) {
			if (atomic_read_acquire(&proto->closing))
				/* we are closing */
				return RX_WAKE_REASON_CLOSING;
			else
				/* someone wants to kill us */
				return RX_WAKE_REASON_ERESTARTSYS;
		}

		/* other protocol errors */
		*result = ret;
		return RX_WAKE_REASON_PROTO_ERROR;
	}

	if (unlikely(atomic_read_acquire(&proto->closing)))
		/* check this before `ret`, otherwise we may recv forever */
		return RX_WAKE_REASON_CLOSING;

	if (unlikely(ret > frames->len))
		/* we don't deal with ill-formatted packets */
		return RX_WAKE_REASON_TRUNCATED_PACKET;

	if (unlikely(!ret))
		/* empty UDP datagrams are not used in D2DP */
		return RX_WAKE_REASON_EMPTY_PACKET;

	*result = ret;
	return RX_WAKE_REASON_OK;
}

static int rx_transport_decrypt(struct d2d_security *security,
				struct net_frames *frames,
				size_t pktlen)
{
	int ret = pktlen;

	if (security) {
		mutex_lock(&frames->crypto_rx_mtx);
		ret = security->decrypt(security,
					frames->rx_encr_frame, pktlen,
					frames->rx_frame, frames->len);
		mutex_unlock(&frames->crypto_rx_mtx);
	}

	return ret;
}

static int rx_transport_do_work(struct d2d_protocol *proto, int data_len)
{
	int ret = 0;
	size_t hdrlen = 0;
	size_t pktlen = 0;
	struct rx_handler *rx = &proto->rx;
	struct net_frames *frames = &proto->frames;

	ret = rx_transport_decrypt(proto->security, frames, data_len);
	if (ret < 0) {
		/* force ack to not deadlock on update keys */
		atomic_cmpxchg(&proto->tx.ack_state.force_ack, 0, 1);
		return 0;
	}

	pktlen = ret;
	hdrlen = d2d_header_read(frames->rx_frame, pktlen, &rx->rx_header);
	if (!hdrlen)
		return 0;

	/*
	 * Replay attack protection. Ignore outdated packets as they may
	 * be re-sent by malicious users by eavesdropping the air.
	 */
	if (rx->rx_header.packet_id <= proto->rx_packet_id)
		return 0;

	proto->rx_packet_id = rx->rx_header.packet_id;
	return rx_process_packet(proto, &rx->rx_header,
				 frames->rx_frame + hdrlen);
}

static int rx_transport_loop(struct d2d_protocol *proto)
{
	int reason = 0;
	int result = 0;

	while (true) {
		reason = rx_transport_recv(proto, &result);
		switch (reason) {
		case RX_WAKE_REASON_OK:
			break;
		case RX_WAKE_REASON_EMPTY_PACKET:
		case RX_WAKE_REASON_TRUNCATED_PACKET:
			continue;
		case RX_WAKE_REASON_PROTO_ERROR:
			/* the error code is stored in result */
			return result;
		case RX_WAKE_REASON_CLOSING:
			return 0;
		case RX_WAKE_REASON_ERESTARTSYS:
			return -ERESTARTSYS;
		default:
			WARN(true, "Invalid rx wake reason: %d\n", reason);
			return -EINVAL;
		}

		result = rx_transport_do_work(proto, result);
		if (result < 0)
			return result;
	}

	return 0;
}

int rx_transport_thread(void *arg)
{
	int err = 0;
	struct d2d_protocol *proto = arg;
	const struct cred *creds = d2d_override_creds(proto);

	allow_signal(SIGTERM);

	err = rx_transport_loop(proto);

	finish_rx_transport_thread(proto, err);
	d2d_revert_creds(creds);
	return 0;
}
