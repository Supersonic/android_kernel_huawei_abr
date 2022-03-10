/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: booster common interface.
 * Author: fanxiaoyu3@huawei.com
 * Create: 2020-03-16
 */

#include "hw_booster_common.h"

#include <linux/highmem.h>
#include <linux/highuid.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/version.h>

static notify_event *notifier = NULL;

/* when modify mobile_ifaces array, plz modify the IFACE_NUM simultaneously */
#define IFACE_NUM 15
static const char mobile_ifaces[IFACE_NUM][IFNAMSIZ] __attribute__((aligned(sizeof(long)))) = {
	/* hisi, sort by frequency of use:rmnet0 > rmnet3 > rmnet1 > ... */
	"rmnet0", "rmnet3", "rmnet1", "rmnet4", "rmnet2", "rmnet5", "rmnet6",
	/* qcom */
	"rmnet_data0", "rmnet_data1", "rmnet_data2", "rmnet_data3",
	/* mtk */
	"ccmni0", "ccmni1", "ccmni2", "ccmni3",
};

static struct interface_info g_interface_info[] = {
	{ DS_NET_ID, "" },
	{ DS_NET_SLAVE_ID, "" }
};

/* reference to ifname_compare_aligned in x_tables.h */
static bool ifname_equal(const char *if1, size_t if1_len,
	const char *if2, size_t if2_len)
{
	const unsigned long *p = (const unsigned long *)if1;
	const unsigned long *q = (const unsigned long *)if2;
	unsigned long ret;

	if (if1_len != if2_len)
		return false;
	if (if1_len == 0)
		return false;

	ret = (p[ARRAY_INDEX_0] ^ q[ARRAY_INDEX_0]);
	if (ret != 0)
		return false;
	if (if1_len > sizeof(unsigned long))
		ret = (p[ARRAY_INDEX_1] ^ q[ARRAY_INDEX_1]);
	if (ret != 0)
		return false;
	if (if1_len > IFNAME_LEN_DOUBLE * sizeof(unsigned long))
		ret = (p[ARRAY_INDEX_2] ^ q[ARRAY_INDEX_2]);
	if (ret)
		return false;
	if (if1_len > IFNAME_LEN_TRIPLE * sizeof(unsigned long))
		ret = (p[ARRAY_INDEX_3] ^ q[ARRAY_INDEX_3]);
	return (ret == 0L);
}

bool is_ds_rnic(const struct net_device *dev)
{
	int i;

	if (!dev)
		return false;
	for (i = 0; i < IFACE_NUM; ++i) {
		if (ifname_equal(dev->name, sizeof(dev->name),
			mobile_ifaces[i], sizeof(mobile_ifaces[i])))
			return true;
	}
	return false;
}

uid_t hw_get_sock_uid(struct sock *sk)
{
	kuid_t kuid;

	if (!sk || !sk_fullsock(sk))
		return overflowuid;
	kuid = sock_net_uid(sock_net(sk), sk);
	return from_kuid_munged(sock_net(sk)->user_ns, kuid);
}

static u32 hw_get_transport_hdr_len(const struct sk_buff *skb, u8 proto)
{
	if (proto == IPPROTO_UDP)
		return sizeof(struct udphdr);

	return tcp_hdrlen(skb);
}

static u32 hw_get_parse_len(const struct sk_buff *skb, u8 proto)
{
	u32 parse_len = 0;
	u32 hdr_len = hw_get_transport_hdr_len(skb, proto);
	if (skb->len > skb_transport_offset(skb) + hdr_len)
		parse_len = skb->len - skb_transport_offset(skb) - hdr_len;
	return parse_len;
}

static bool hw_is_zero_lineare_room(const struct sk_buff *skb, u8 proto)
{
	if (skb_is_nonlinear(skb) &&
		(skb_headlen(skb) == skb_transport_offset(skb) + hw_get_transport_hdr_len(skb, proto)))
		return true;

	return false;
}

u8 *hw_get_payload_addr(const struct sk_buff *skb,
	u8 proto, u8 **vaddr, u32 *parse_len)
{
	const skb_frag_t *frag = NULL;
	struct page *page = NULL;
	u8 *payload = NULL;
	if (skb == NULL || vaddr == NULL || parse_len == NULL)
		return payload;

	if (hw_is_zero_lineare_room(skb, proto)) {
		frag = &skb_shinfo(skb)->frags[0];
		page = skb_frag_page(frag);
		*vaddr = kmap_atomic(page);
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)
		payload = *vaddr + frag->page_offset;
#else
		payload = *vaddr + skb_frag_off(frag);
#endif
		*parse_len = skb_frag_size(frag);
	} else {
		payload = skb_transport_header(skb) + hw_get_transport_hdr_len(skb, proto);
		*parse_len = hw_get_parse_len(skb, proto);
	}
	return payload;
}

void hw_unmap_vaddr(u8 *vaddr)
{
	if (vaddr)
		kunmap_atomic(vaddr);
}

bool is_cellular_interface(const struct net_device *dev)
{
	int i;

	if (!dev)
		return false;
	for (i = 0; i <= DS_NET_SLAVE_ID; ++i) {
		if (ifname_equal(dev->name, sizeof(dev->name),
			g_interface_info[i].name, IFNAMSIZ))
			return true;
	}
	return false;
}

static void process_interface_info_update(struct interface_info_msg *msg)
{
	struct interface_info *info = &msg->info;
	u32 modem_id = info->modem_id;

	if (modem_id != DS_NET_ID && modem_id != DS_NET_SLAVE_ID)
		return;

	if (msg->len < sizeof(struct interface_info_msg))
		return;

	g_interface_info[modem_id].modem_id = modem_id;
	memcpy(g_interface_info[modem_id].name, info->name, IFNAMSIZ);
	g_interface_info[modem_id].name[IFNAMSIZ - 1] = '\0';

	pr_info("[booster_comm]modem_id:%d interface name:%s\n", modem_id, g_interface_info[modem_id].name);
}

static void cmd_process(struct req_msg_head *msg)
{
	if (msg->len > MAX_REQ_DATA_LEN)
		return;

	if (msg->type == UPDATE_INTERFACE_INFO)
		process_interface_info_update((struct interface_info_msg *)msg);
	else
		pr_info("[booster_comm]map msg error\n");
}

msg_process *hw_booster_common_init(notify_event *notify)
{
	if (notify == NULL) {
		pr_err("%s: notify parameter is null\n", __func__);
		return NULL;
	}
	notifier = notify;
	return cmd_process;
}
