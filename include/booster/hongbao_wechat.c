
#include "hongbao_wechat.h"

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/file.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <linux/tcp.h>
#include <linux/version.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/inet_sock.h>
#include <securec.h>

#include <linux/highmem.h>

#include <huawei_platform/log/hw_log.h>
#undef HWLOG_TAG
#define HWLOG_TAG hongbao_wechat
HWLOG_REGIST();

#if (KERNEL_VERSION(4, 14, 0) <= LINUX_VERSION_CODE)
#include <linux/sched/task.h>
#endif
#include <hwnet/booster/chr_manager.h>

#include "hw_booster_common.h"

#define MSG_LEN 4

struct app_info_stru g_app_info[MAX_APP_NUMBER];
static notify_event *notifier = NULL;
static notify_event *g_chr_notifier = NULL;

/*
 * hognbao_init() - initialize the structure of the fast
 *                 preemption technology
 *
 * initialize the global struct.
 */
void hongbao_init(void)
{
	uint16_t i;
	uint16_t j;

	for (i = 0; i < MAX_APP_NUMBER; i++) {
		g_app_info[i].l_uid = APP_INVALID_UID;
		g_app_info[i].us_version = INVALID_PARAMETER;
		g_app_info[i].us_index = i;
		spin_lock_init(&g_app_info[i].st_locker);
		for (j = 0; j < MAX_KW_NUMBER; j++)
			g_app_info[i].pst_kws[j] = NULL;
	}
}

/*
 * proc_wx_sock() - Processing WeChat Socket Status Changes
 *
 * @pstSock: the object of sock
 * @state: the state of new socket
 *
 * process the wechat socket connection states.
 */
static void proc_wx_sock(struct sock *pst_sock, int state)
{
	/* If new socket is built, we think it to "waiting" state */
	if (state == TCP_ESTABLISHED) {
		pst_sock->fg_spec = WECHAT_WAIT_HONGBAO;
		hwlog_info("Set To WAIT_HONGBAO, PTR=%p", pst_sock);
	} else if (state == TCP_CLOSING) {
		/*
		 * Only out put a log. But now wx socket is using "RST"
		 * but "Close", so this
		 * can't be printed;
		 */
		if (pst_sock->fg_spec == WECHAT_HONGBAO)
			hwlog_info("Hongbao_socket is Removed");
	} else {
		return;
	}
}

/*
 * proc_wx_match_keyword_dl() - process the matched dl keyword
 *
 * @pstKwdIns: the key words that needed to matched.
 * @pData: the received Downlink data(from skbuff)
 * @ulLength: Maximum valid data length
 *
 * This function is used to process WeChat socket downstream hook
 * packets. This parameter is valid only for push channels.
 *
 * Return: true - matche.
 *         false - not match.
 */
static bool proc_wx_match_keyword_dl(
	struct keyword_stru *pst_kwd_ins,
	uint8_t *p_data, uint32_t ul_length)
{
	uint64_t us_tot_len;

	if (pst_kwd_ins == NULL)
		return false;

	/*
	 * probe log, get the packet head
	 * match the length of XML to dl packet
	 */
	if ((ul_length > pst_kwd_ins->st_len_porp.us_max) ||
		(ul_length < pst_kwd_ins->st_len_porp.us_min))
		return false;

	/* match keywords */
	us_tot_len = pst_kwd_ins->st_key_word.us_tot_len;
	if (memcmp(&pst_kwd_ins->st_key_word.auc_data[0],
		p_data, us_tot_len) == 0)
		return true;
	return false;
}

/*
 * is_satisfied_keyword_version() - is satisfied keyword version
 *
 * @version: the key words that needed to matched.
 * @min_version: the received Downlink data(from skbuff)
 * @max_version: Maximum valid data length
 *
 * This function is used to process WeChat socket downstream hook
 * packets. This parameter is valid only for push channels.
 *
 * Return: true - matche.
 *         false - not match.
 */
static bool is_satisfied_keyword_version(
	int16_t version, int16_t min_version, int16_t max_version)
{
	if (version == INVALID_PARAMETER)
		return false;

	if ((min_version == INVALID_PARAMETER) && (max_version == INVALID_PARAMETER)) {
		return true;
	} else if (min_version == INVALID_PARAMETER) {
		if (version <= max_version)
			return true;
	} else if (max_version == INVALID_PARAMETER) {
		if (min_version <= version)
			return true;
	} else {
		if (min_version <= version && version <= max_version)
			return true;
	}
	return false;
}

/*
 * proc_wx_packet_dl() - handle dl wx packet.
 *
 * @pstSock: the object of sock
 * @pData: the received Downlink data(from skbuff)
 * @ulLength: Maximum valid data length
 *
 * This function is used to process WeChat socket
 * downstream hook packets. This parameter is valid
 * only for push channels.
 */
static uint8_t proc_wx_packet_dl(struct sock *pst_sock,
	uint8_t *p_data, uint32_t ul_length)
{
	struct keyword_stru *pst_kwd_ins = NULL;
	struct keyword_stru *pst_kwd_ins_new = NULL;
	bool b_found = false;
	int16_t version;

	if (pst_sock->fg_spec != WECHAT_PUSH)
		return R_ERROR;

	/* Set the "PUSH"( 0 0 4 ) SIP-Command to be compared object. */
	pst_kwd_ins = get_app_ins(IDX_WECHAT)->pst_kws[FLAG_WECHAT_PUSH];
	version = get_app_ins(IDX_WECHAT)->us_version;
	if (pst_kwd_ins == NULL)
		return 0;
	if (is_satisfied_keyword_version(version, pst_kwd_ins->st_key_word.us_min_ver, pst_kwd_ins->st_key_word.us_max_ver))
		b_found = proc_wx_match_keyword_dl(pst_kwd_ins, p_data, ul_length);
	if (b_found)
		return R_OK;
	pst_kwd_ins_new = get_app_ins(IDX_WECHAT)->pst_kws[FLAG_WECHAT_PUSH_NEW];
	if (pst_kwd_ins_new == NULL)
		return 0;
	if (is_satisfied_keyword_version(version, pst_kwd_ins_new->st_key_word.us_min_ver, pst_kwd_ins_new->st_key_word.us_max_ver))
		b_found = proc_wx_match_keyword_dl(pst_kwd_ins_new, p_data, ul_length);
	if (b_found)
		return R_OK;
	return R_ERROR;
}

/*
 * proc_send_rpacket_priority() - Determine whether WeChat
 *     data is processed based on WeChat data.
 *
 * @pstSock: the object of sock
 * @pData: the received Downlink data(forced conversion msg,
 *          iov user space data)
 * @ulLength: Maximum valid data length
 *
 * this function was invoked in tcp_output.c/tcp_write_xmit.
 *
 * Return: 0 - No key data information
 *         1 - has key data information
 *
 */
uint8_t proc_send_rpacket_priority(struct sock *pst_sock)
{
	if (!pst_sock)
		return R_ERROR;
	if (pst_sock->fg_spec == WECHAT_HONGBAO) {
		hwlog_info("Hongbao packet");
		return R_OK;
	}
	return R_ERROR;
}

static bool judge_the_data_length(uint32_t ul_length,
	struct keyword_stru *pst_kwd_ins,
	struct sock *pst_sock)
{
	if ((ul_length < pst_kwd_ins->st_len_porp.us_copy) ||
		(pst_kwd_ins->st_len_porp.us_copy > MAX_US_COPY_NUM) ||
		(pst_kwd_ins->st_len_porp.us_copy == 0)) {
		hwlog_info("pstKwdIns->stLenPorp.usCopy is invalid");
		set_sock_special(pst_sock, NO_SPEC);
		return false;
	}
	return true;
}

/*
 * proc_wx_packet_ul() - Processing WeChat Upload hook Packets
 *
 * @pstSock: the object of sock
 * @pData: the received Downlink data(forced conversion msg,
 *         iov user space data)
 * @ulLength: Maximum valid data length
 *
 * copy the date from user space to kernel,process the wx ul packet.
 *
 * Return: 0 - No key data information
 *         1 - has key data information
 *
 */
static uint8_t proc_wx_packet_ul(
	struct sock *pst_sock,
	uint8_t *p_data,
	uint32_t ul_length)
{
	struct keyword_stru *pst_kwd_ins = NULL;
	char *pc_found = NULL;
	char *p_usr_data = NULL;
	struct msghdr *p_usr_msg_hdr = NULL;

	/*
	 * If this sock has been matched tob HONGBAO-Server connection,
	 * return R_OK direction to let HP data sending.
	 */
	if (pst_sock->fg_spec == WECHAT_HONGBAO) {
		hwlog_info("Hongbao_socket is sending");
		return R_OK;
	}

	/*
	 * If this sock is "WAITING" to be matched state,
	 * here will Match the first sending packet
	 * of this sock to find "hongbao"-URL message.
	 * ATENTION: This function only execute matching one time per sock.
	 */
	if (pst_sock->fg_spec == WECHAT_WAIT_HONGBAO) {
		/* Set the "hongbao"-URL to be compared object. */
		pst_kwd_ins = get_app_ins(IDX_WECHAT)
			->pst_kws[FLAG_WECHAT_GET];
		if (pst_kwd_ins == NULL)
			return R_ERROR;

		/* too short. */
		if (!judge_the_data_length(ul_length, pst_kwd_ins, pst_sock))
			return R_ERROR;

		/* Think it to be a common sock firstly. */
		pst_sock->fg_spec = NO_SPEC;
		p_usr_data = kzalloc(pst_kwd_ins->st_len_porp.us_copy,
			GFP_ATOMIC);
		if (p_usr_data == NULL)
			return R_ERROR;

		/*
		 * Copy data from usr space.
		 * Set the last byte to '0' for strstr input.
		 */
		p_usr_msg_hdr = (struct msghdr *)p_data;
		if (copy_from_user(p_usr_data,
			p_usr_msg_hdr->msg_iter.iov->iov_base,
			pst_kwd_ins->st_len_porp.us_copy)) {
			kfree(p_usr_data);
			return R_ERROR;
		}
		p_usr_data[pst_kwd_ins->st_len_porp.us_copy - 1] = 0;
		pc_found = strstr((const char *)p_usr_data,
			(const char *)&pst_kwd_ins->st_key_word.auc_data[0]);
		kfree(p_usr_data);
		if (pc_found == NULL) {
			pst_sock->fg_spec = NO_SPEC;
			return R_ERROR;
		}
		set_sock_special(pst_sock, WECHAT_HONGBAO);
		hwlog_info("Find New Hongbao_socket");
		return R_OK;
	}
	return R_ERROR;
}

/*
 * proc_wx_packet() - Processing WeChat hook Packages
 *
 * @pstSock: the object of sock
 * @pData: the received data.
 * @ulLength: Maximum valid data length
 * @ulRole: TX/RX direction
 *
 * invoke the dl or ul process method by the ul_role.
 *
 * Return: 0 - No key data information
 *         1 - has key data information
 */
static uint8_t proc_wx_packet(struct sock *pst_sock,
	uint8_t *p_data, uint32_t ul_length,
	uint32_t ul_role)
{
	uint8_t uc_rtn = 0;

	/* Call UL/DL packet proc, according to ROLE */
	if (ul_role == ROLE_RCVER)
		uc_rtn = proc_wx_packet_dl(pst_sock, p_data, ul_length);

	else if (ul_role == ROLE_SNDER)
		uc_rtn = proc_wx_packet_ul(pst_sock, p_data, ul_length);

	return uc_rtn;
}

void hongbao_regist_msg_fun(notify_event *notify)
{
	if (notify == NULL) {
		hwlog_info("notify null");
		return;
	}
	g_chr_notifier = notify;
}

static void notify_hongbao_chr(void)
{
	struct res_msg_head msg;

	if (g_chr_notifier == NULL) {
		hwlog_info("g_chr_notifier null");
		return;
	}
	msg.type = FASTGRAB_CHR_RPT;
	msg.len = MSG_LEN;
	g_chr_notifier(&msg);
}

static void send_data_to_booster(int type_id, uid_t uid)
{
	char event[HONGBAO_STAT_EVT_LEN];
	char *p = event;

	if (!notifier)
		return;

	// type
	assign_short(p, type_id);
	skip_byte(p, sizeof(s16));

	// len(2B type + 2B len + 4B uid)
	assign_short(p, HONGBAO_STAT_EVT_LEN);
	skip_byte(p, sizeof(s16));
	// uid
	assign_int(p, uid);
	skip_byte(p, sizeof(int));

	notifier((struct res_msg_head *)event);
}

/*
 * hook_dl_stub() - hook the downlink data packets,
 *
 * @sk: the object of sock
 * @skb: the object of sk_buff.
 * @hrd_len: head length
 *
 * hook the downlink data packets
 *
 * Return:NA
 */
void hook_dl_stub(struct sock *sk, struct sk_buff *skb, uint16_t hrd_len)
{
	uint8_t *p_data = NULL;
	u8 *vaddr = NULL;
	const skb_frag_t *frag = NULL;
	struct page *page = NULL;
	uint32_t ul_length = 0;

	if (sk == NULL || skb == NULL)
		return;

	if (sk->fg_spec != NO_SPEC && skb->len > hrd_len) {
		/* 0:nonlinear has data */
		if ((skb_headlen(skb) == hrd_len) && (skb_shinfo(skb)->nr_frags > 0)) {
			frag = &skb_shinfo(skb)->frags[0]; /* 0:nonlinear first data */
			if (frag == NULL)
				return;
			page = skb_frag_page(frag);
			if (page == NULL)
				return;
			vaddr = kmap_atomic(page);
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)
			p_data = vaddr + frag->page_offset;
#else
			p_data = vaddr + skb_frag_off(frag);
#endif
			ul_length = skb_frag_size(frag);
		} else {
			p_data = skb->data + hrd_len;
			if (skb_headlen(skb) > hrd_len)
				ul_length = skb_headlen(skb) - hrd_len;
		}

		if (p_data == NULL || ul_length <= 0) {
			hw_unmap_vaddr(vaddr);
			return;
		}
		hook_packet(sk, p_data, ul_length, ROLE_RCVER);
		hw_unmap_vaddr(vaddr);
	}
}

/*
 * hook_packet() - external interface,
 * Processing the packets sent and received
 *                       by the external socket
 *
 * @pstSock: the object of sock
 * @pData: the received data.
 * @ulLength: Maximum valid data length
 * @ulRole: TX/RX direction
 *
 * hook the wx packet by us_idx,if the scenario is grab
 * hongbao, will start rrc alive timer.
 *
 * Return: 0 - No key data information
 *         1 - has key data information
 */
uint8_t hook_packet(
	struct sock *pst_sock,
	uint8_t *p_data,
	uint32_t ul_length,
	uint32_t ul_role)
{
	uid_t l_sock_uid;
	uint8_t uc_rtn = 0;
	uint16_t us_idx;

	if (IS_ERR_OR_NULL(pst_sock)) {
		hwlog_info("invalid parameter");
		return R_ERROR;
	}
	/* Get and find the uid in fast-grab message information list. */
	l_sock_uid = sock_i_uid(pst_sock).val;
	for (us_idx = 0; us_idx < MAX_APP_NUMBER; us_idx++) {
		if (l_sock_uid == g_app_info[us_idx].l_uid)
			break;
	}
	if (!is_app_idx_valid(us_idx))
		return R_ERROR;

	hwlog_info("Hook Length=%u, Role=%u, sk_state=%d",
		ul_length, ul_role, pst_sock->fg_spec);

	/* Call different packet proc according the Application Name(Index). */
	switch (us_idx) {
	case IDX_WECHAT:
		uc_rtn = proc_wx_packet(pst_sock, p_data, ul_length, ul_role);
		if (uc_rtn) {
			hwlog_info("identification sucess");
			send_data_to_booster(HONGBAO_WECHAT_RPT, l_sock_uid);
#ifdef CONFIG_CHR_NETLINK_MODULE
			notify_hongbao_chr();
#endif
		}
		break;
	default:
		break;
	}
	return uc_rtn;
}

static struct file *hongb_fget_by_pid(unsigned int fd, pid_t pid)
{
	struct file *file = NULL;
	struct task_struct *task = NULL;
	struct files_struct *files = NULL;

	rcu_read_lock();
	task = find_task_by_vpid(pid);
	if (!task) {
		rcu_read_unlock();
		return NULL;
	}
	get_task_struct(task);
	rcu_read_unlock();
	files = task->files;

	/* process is removed, task isn't null, but files is null */
	if (files == NULL) {
		put_task_struct(task);
		return NULL;
	}
	put_task_struct(task);
	rcu_read_lock();
	file = fcheck_files(files, fd);
	if (file) {
		/* File object ref couldn't be taken */
		if ((file->f_mode & FMODE_PATH) ||
			!atomic_long_inc_not_zero(&file->f_count))
			file = NULL;
	}
	rcu_read_unlock();

	return file;
}

struct socket *hongb_sockfd_lookup_by_fd_pid(int fd, pid_t pid, int *err)
{
	struct file *file = NULL;
	struct socket *sock = NULL;

	file = hongb_fget_by_pid(fd, pid);
	if (!file) {
		*err = -EBADF;
		return NULL;
	}

	sock = sock_from_file(file, err);
	if (!sock)
		fput(file);

	return sock;
}

/*
 * get_sock_by_fd_pid() - get the sock.
 * @fd: file descriptor.
 * @pid: process id.
 *
 * invoke the sockfd_lookup_by_fd_pid() to get the sock.
 *
 * Return: sock, success.
 * NULL means fail.
 */
struct sock *hongb_get_sock_by_fd_pid(int fd, pid_t pid)
{
	int err = 0;
	struct socket *sock = NULL;
	struct sock *sk = NULL;

	sock = hongb_sockfd_lookup_by_fd_pid(fd, pid, &err);
	if (sock == NULL)
		return NULL;

	sk = sock->sk;
	if (sk != NULL)
		sock_hold(sk);

	if (sock->file != NULL)
		sockfd_put(sock);

	return sk;
}

static struct sock *hongbao_get_sock_by_fd_pid(int fd, int32_t pid)
{
	struct inet_sock *isk = NULL;
	struct sock *sk = NULL;

	sk = hongb_get_sock_by_fd_pid(fd, pid);
	if (sk == NULL)
		return NULL;

	if (sk->sk_state != TCP_ESTABLISHED) {
		sock_put(sk);
		return NULL;
	}

	if (sk->sk_socket && sk->sk_socket->type != SOCK_STREAM) {
		sock_put(sk);
		return NULL;
	}

	isk = inet_sk(sk);
	if (isk == NULL) {
		sock_put(sk);
		return NULL;
	}

	if ((isk->inet_daddr == INVALID_INET_ADDR) &&
		(isk->inet_dport == INVALID_INET_ADDR)) {
		sock_put(sk);
		return NULL;
	}
	return sk;
}

static bool process_sock_find_result(struct sock **pst_sock_reg,
	uint16_t *us_found_port, int *l_found_fd, int fd,
	struct sock **pst_sock, struct inet_sock *pst_inet)
{
	/* If there no sock be found, set the ptr to tmp-ptr. */
	if (*pst_sock_reg == NULL) {
		*l_found_fd = fd;
		*us_found_port = pst_inet->inet_sport;
		*pst_sock_reg = *pst_sock;
	} else {
		/*
		 * If there is a sock has been recorded,we will
		 * check fd/port to judge if it is the same one.
		 * If it's another one, it means that there is
		 * not only one sock in this uid, unsuccessful.
		 */
		if ((fd == *l_found_fd) && (*us_found_port ==
			pst_inet->inet_sport)) {
			sock_put(*pst_sock);
		} else {
			sock_put(*pst_sock);
			sock_put(*pst_sock_reg);
			hwlog_info("More than One Push Socket Found");
			return false;
		}
	}
	return true;
}

static struct files_struct *find_file_sys_head_according_to_task(
	struct task_struct *pst_task)
{
	struct files_struct *pst_files = NULL;

	if (pst_task == NULL) {
		hwlog_info("pstTask error");
		rcu_read_unlock();
		return NULL;
	}
	get_task_struct(pst_task);
	rcu_read_unlock();

	/* Find File sys head according to task */
	pst_files = pst_task->files;
	if (pst_files == NULL) {
		put_task_struct(pst_task);
		hwlog_info("pstFiles error");
		return NULL;
	}
	return pst_files;
}

static int set_hongbao_push_sock(int32_t tid_num, int32_t *tids)
{
	struct task_struct *pst_task = NULL;
	struct files_struct *pst_files = NULL;
	struct fdtable *pst_fdt = NULL;
	struct sock *pst_sock = NULL;
	struct sock *pst_sock_reg = NULL;
	struct inet_sock *pst_inet = NULL;
	uint16_t us_looper;
	int fd;
	uint16_t us_found_port = 0;
	int l_found_fd = -1;

	/* use pid to proc. */
	for (us_looper = 0; us_looper < tid_num; us_looper++) {
		/* Find task message head */
		rcu_read_lock();
		pst_task = find_task_by_vpid(tids[us_looper]);
		pst_files = find_file_sys_head_according_to_task(pst_task);
		if (pst_files == NULL)
			return -EFAULT;

		put_task_struct(pst_task);
		/* list the fd table */
		pst_fdt = files_fdtable(pst_files);
		for (fd = 0; fd < pst_fdt->max_fds; fd++) {
			/* check the state, ip-addr, port-num of this sock */
			pst_sock = hongbao_get_sock_by_fd_pid(fd, tids[us_looper]);
			if (!pst_sock)
				continue;

			pst_inet = inet_sk(pst_sock);
			if (!process_sock_find_result(&pst_sock_reg,
				&us_found_port, &l_found_fd, fd, &pst_sock,
				pst_inet)) {
				return R_ERROR;
			}
		}
	}
	if (pst_sock_reg != NULL) {
		/* record the PUSH special flag to this sock */
		set_sock_special(pst_sock_reg, WECHAT_PUSH);
		sock_put(pst_sock_reg);
	}
	return R_ERROR;
}

/*
 * hook_ul_stub() - hook the uplink data packets.
 *
 * @*pstSock: the object of socket
 * @*msg: struct msghdr, the struct of message that was sent.
 *
 * if socket has matched keyword and if uid equals current acc uid
 * hook the uplink data packets.
 *
 * Return: true - hook the ul stub
 *         false - not matched.
 */
bool hook_ul_stub(struct sock *pst_sock, struct msghdr *msg)
{
	if ((pst_sock == NULL) || (msg == NULL))
		return false;

	/*
	 * if the socket has matched keyword, keep acc always
	 * until socket destruct
	 */
	if (pst_sock->fg_spec == WECHAT_HONGBAO)
		return true;

	/* if matched keyword, accelerate socket */
	if (pst_sock->fg_spec != NO_SPEC) {
		if (hook_packet(pst_sock, (uint8_t *)msg,
			(uint32_t)(msg->msg_iter.iov->iov_len),
			ROLE_SNDER)) {
			return true;
		}
	}

	return false;
}

/*
 * check_sock_uid() - check sock uid.
 *
 * @pstSock: sock struct
 * @state: the state of socket
 *
 * Processes the socket status change information related to uid.
 */
void check_sock_uid(struct sock *pst_sock, int state)
{
	uid_t l_uid;
	uint16_t us_idx;

	if (IS_ERR_OR_NULL(pst_sock)) {
		hwlog_info("%s:invalid parameter\n", __func__);
		return;
	}

	/* Judge the state if it's should be cared. */
	if (!is_cared_sk_state(state))
		return;

	/* Reset the sock fg special flag to inited state. */
	init_sock_spec(pst_sock);
	l_uid = sock_i_uid(pst_sock).val;
	if (l_uid <= APP_INVALID_UID)
		return;

	/* Find the application name(index) according to uid. */
	for (us_idx = 0; us_idx < MAX_APP_NUMBER; us_idx++) {
		if (l_uid == g_app_info[us_idx].l_uid) {
			hwlog_info("Cared Uid Found, uid=%u", l_uid);
			send_data_to_booster(HONGBAO_WECHAT_UID_SOCK_CHG, l_uid);
			break;
		}
	}
	if (!is_app_idx_valid(us_idx))
		return;

	/* Call branched function of different application. */
	switch (us_idx) {
	case IDX_WECHAT:
		proc_wx_sock(pst_sock, state);
		break;
	default:
		break;
	}
}

static void notify_update_wechat_uid_event(const char *msg, int len)
{
	int uid;
	int usindex;
	int16_t version;
	const char *p = msg;
	struct app_info_stru *pst_app_ins = NULL;

	if (len < sizeof(int) || msg == NULL) //lint !e574
		return;
	uid = *(int *)p;

	if (uid < APP_INVALID_UID)
		return;
	len -= sizeof(int);
	p += sizeof(int);
	if (len < sizeof(int)) //lint !e574
		return;

	usindex = *(int *)p;

	len -= sizeof(int);
	p += sizeof(int);
	if (len < sizeof(int)) //lint !e574
		return;

	version = (int16_t) *(int *)p;

	pst_app_ins = get_app_ins(usindex);
	if (!pst_app_ins)
		return;

	spin_lock_bh(&pst_app_ins->st_locker);
	pst_app_ins->l_uid = (uid_t)uid;
	pst_app_ins->us_version = version;
	spin_unlock_bh(&pst_app_ins->st_locker);
}

int hex_to_dec(const char *s)
{
	int i, t;
	int sum = 0;

	if (s == NULL)
		return INVALID_PARAMETER;

	for (i = 0; s[i]; i++) {
		if (s[i] >= '0' && s[i] <= '9') /* 0: 9: the Digital Part of hexadecimal */
			t = s[i] - '0'; /* 0: 9: the Digital Part of hexadecimal */
		else if (s[i] >= 'a' && s[i] <= 'f') /* a: z: the character Part of hexadecimal */
			t = s[i] - 'a' + 10; /* a: z:  change the character Part of hexadecimal to digital */
		else if (s[i] >= 'A' && s[i] <= 'F') /* A: Z: the character Part of hexadecimal */
			t = s[i] - 'A' + 10; /* A: 10: change the character Part of hexadecimal to digital */
		else
			return INVALID_PARAMETER;
		sum = sum * 16 + t; /* 16: hexadecimal to Decimal */
	}
	return sum;
}

static int get_data(const char *auc_str, int us_loop)
{
	char tmp[3] = {0, 0, 0}; /* 0: init data */
	int m;

	/* 2: for changing string len to content */
	tmp[0] = auc_str[us_loop * 2];
	/* 2: for changing string len to content */
	tmp[1] = auc_str[us_loop * 2 + 1];
	tmp[2] = 0; /* dx 2 use for end flag */

	m = hex_to_dec(tmp);
	return m;
}

static uint16_t get_keyword(const char *p, struct keyword_stru *pst_kwd)
{
	int format = -1;

	if (p == NULL || pst_kwd == NULL)
		return format;
	p += sizeof(int);
	pst_kwd->st_len_porp.us_copy = (uint16_t) *(int *)p;
	p += sizeof(int);

	pst_kwd->st_len_porp.us_min = (uint16_t) *(int *)p;
	p += sizeof(int);

	pst_kwd->st_len_porp.us_max = (uint16_t) *(int *)p;
	p += sizeof(int);

	pst_kwd->st_key_word.us_ofst = (uint16_t) *(int *)p;
	p += sizeof(int);

	pst_kwd->us_role = (uint16_t) *(int *)p;
	p += sizeof(int);

	format = (uint16_t) *(int *)p;
	p += sizeof(int);

	pst_kwd->st_key_word.us_min_ver = (int16_t) *(int *)p;
	p += sizeof(int);

	pst_kwd->st_key_word.us_max_ver = (int16_t) *(int *)p;
	p += sizeof(int);
	return format;
}

static void notify_update_wechat_keyword_event(const char *msg, int len)
{
	int index;
	int tot_len;
	int key_idx;
	int looper;
	int format = -1;
	struct app_info_stru *pst_app_ins = NULL;
	struct keyword_stru *pst_kwd_dst = NULL;
	const char *p = msg;

	if (msg == NULL || len < (sizeof(int) + (DATA_NUM * (sizeof(int)) + KW_DATA_LEN) * MAX_KW_NUMBER))
		return;

	index = *(int *)p;
	if ((index >= MAX_APP_NUMBER) || (index < MIN_INDEX))
		return;

	p += sizeof(int);
	pst_app_ins = get_app_ins(index);
	if (pst_app_ins == NULL)
		return;

	spin_lock_bh(&pst_app_ins->st_locker);
	for (key_idx = 0; key_idx < MAX_KW_NUMBER; key_idx++) {
		tot_len = (*(int *)p);
		if ((tot_len <= KW_MIN_LEN) || (tot_len >= KW_DATA_LEN))
			break;

		pst_kwd_dst = NULL;
		pst_kwd_dst = kzalloc(sizeof(struct keyword_stru) +
			sizeof(char) * tot_len, GFP_ATOMIC);
		if (pst_kwd_dst == NULL)
			break;

		pst_kwd_dst->st_key_word.us_tot_len = (uint16_t)tot_len;
		format = get_keyword(p, pst_kwd_dst);
		p += DATA_NUM * (sizeof(int));
		if (format == TYPE_FORM_ASC) {
			/* 1: change the length to the index */
			int ret = memcpy_s(pst_kwd_dst->st_key_word.auc_data, tot_len - 1, p, tot_len - 1);

			if (ret != 0)
				break;

			/* the upstream keyword is set to 0 */
			pst_kwd_dst->st_key_word.auc_data[tot_len-1] = (uint8_t) 0;
		} else if (format == TYPE_FORM_HEX) {
			for (looper = 0; looper < tot_len; looper++)
				pst_kwd_dst->st_key_word.auc_data[looper] = (uint8_t)(get_data(p, looper));
		} else {
			break;
		}
		p += KW_DATA_LEN;
		pst_app_ins->pst_kws[key_idx] = pst_kwd_dst;
	}
	spin_unlock_bh(&pst_app_ins->st_locker);
}

static void notify_update_wechat_pid_event(const char *msg, int len)
{
	int index = 0;
	int pid_num = 0;
	int looper;
	int32_t *pl_tids = NULL;
	const char *p = msg;

	if (len < sizeof(int)) //lint !e574
		return;
	index = *(int *)p;
	len -= sizeof(int);
	p += sizeof(int);
	if (len < sizeof(int)) //lint !e574
		return;
	pid_num = *(int *)p;

	hwlog_info("notify update wechat pid event  pid_num    %d\n", pid_num);
	pl_tids = kzalloc(pid_num * sizeof(int32_t), GFP_ATOMIC);
	if (pl_tids == NULL)
		return;

	for (looper = 0; looper < pid_num; looper++) {
		len -= sizeof(int);
		p += sizeof(int);
		if (len < sizeof(int)) //lint !e574
			return;
		pl_tids[looper] = (int32_t) *(int *)p;
		if (pl_tids[looper] < INVALID_PARAMETER)
			continue;
	}
	p += sizeof(int);
	spin_lock_bh(&g_app_info[index].st_locker);

	if (g_app_info[index].l_uid <= APP_INVALID_UID) {
		spin_unlock_bh(&g_app_info[index].st_locker);
		return;
	}
	switch (index) {
	case IDX_WECHAT:
		/* If wechat, find the push socket according the pid message */
		set_hongbao_push_sock(pid_num, pl_tids);
		break;
	default:
		break;
	}

	spin_unlock_bh(&g_app_info[index].st_locker);
}

// 即调用nl_notify_event（）方法发送数据
static void cmd_process(struct req_msg_head *msg)
{
	if (!msg) {
		pr_err("msg is null\n");
		return;
	}
	hwlog_info("cmd process  msg->type %d", msg->type);
	if (msg->type == UPDAT_WECHAT_UID)
		notify_update_wechat_uid_event((char *)msg +
			sizeof(struct req_msg_head), msg->len - sizeof(struct req_msg_head));
	else if (msg->type == UPDAT_WECHAT_KEYWORD)
		notify_update_wechat_keyword_event((char *)msg +
			sizeof(struct req_msg_head), msg->len - sizeof(struct req_msg_head));
	else if (msg->type == UPDAT_WECHAT_PID)
		notify_update_wechat_pid_event((char *)msg +
			sizeof(struct req_msg_head), msg->len - sizeof(struct req_msg_head));
}

msg_process * __init update_wechat_init(notify_event *notify)
{
	hongbao_init();
	hwlog_info("update wechat init");

	if (notify == NULL) {
		pr_err("%s: notify parameter is null\n", __func__);
		return NULL;
	}
	notifier = notify;
	return cmd_process;
}

