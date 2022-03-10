/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2021. All rights reserved.
 * Description: Process the event for hongbao fastgrab in the kernel
 * Author: zhuweichen@huawei.com
 * Create: 2015-09-30
 */

#ifndef _HONGBAO_WECHAT_H
#define _HONGBAO_WECHAT_H

#include <net/sock.h>

#include <linux/netfilter.h>

#include "netlink_handle.h"

#define NOTIFY_BUF_LEN 512

msg_process *update_wechat_init(notify_event *fn);

#define HONGBAO_STAT_EVT_LEN 8

#define MAX_APP_NUMBER 0x01U
#define MAX_KW_LEN 0xFFU /* Current Max keyword lenght is 16 */
#define MAX_KW_NUMBER 0x04U
#define MAX_PID_NUMBER 0x200U
#define MAX_US_COPY_NUM 4096
#define ROLE_SNDER 0x01U
#define ROLE_RCVER 0x02U

#define IDX_WECHAT 0x00U
#define FLAG_APP_INFO 0x01U

#define FLAG_WECHAT_PUSH 0x00U
#define FLAG_WECHAT_RCVD 0x01U
#define FLAG_WECHAT_GET 0x02U
#define FLAG_WECHAT_PUSH_NEW 0x03U

#define APP_INVALID_UID 0
#define MIN_INDEX 0
#define INVALID_PARAMETER 0
#define INVALID_INET_ADDR 0
#define R_ERROR 0
#define R_OK 1
#define KW_DATA_LEN 40
#define KW_MIN_LEN 0
#define DATA_NUM 9
#define TYPE_FORM_HEX 0
#define TYPE_FORM_ASC 1

#define FLAG_WECHAT_VALID \
	((FLAG_APP_INFO) | \
	(FLAG_WECHAT_RCVD) | \
	(FLAG_WECHAT_GET) | \
	(FLAG_WECHAT_VALID))

enum hognbao_sock_prop {
	NO_SPEC = 0,
	WECHAT_PUSH,
	WECHAT_WAIT_HONGBAO,
	WECHAT_HONGBAO,
	SOCK_BUTT
};

#define is_app_idx_valid(idx) ((idx) < (MAX_APP_NUMBER))
#define is_kwd_idx_valid(idx) ((idx) < (MAX_KW_NUMBER))
#define get_app_ins(idx) (&g_app_info[idx])
#define is_cared_sk_state(state) \
	((TCP_CLOSING == (state)) || (TCP_ESTABLISHED == (state)) || \
	(TCP_FIN_WAIT1 == (state)) || (TCP_CLOSE_WAIT == (state)))

#define init_sock_spec(pSock) do { \
	(pSock)->fg_spec = NO_SPEC; } while (0)

#define set_sock_special(sk, spec) do { bh_lock_sock(sk); \
	(sk)->fg_spec = (spec); \
	bh_unlock_sock(sk); } while (0)

#define assign_short(p, val) (*(s16 *)(p) = (val))
#define assign_int(p, val) (*(s32 *)(p) = (val))
#define assign_uint(p, val) (*(u32 *)(p) = (val))
#define assign_long(p, val) (*(s64 *)(p) = (val))
#define skip_byte(p, num) ((p) = (p) + (num))

typedef void(*PKT_PROC_T)(struct sock *, uint8_t*, uint32_t uint32_t);

/*
 * struct bst_fg_packet_len_stru - define the packet length.
 * @us_copy:The length should be copied from Packet.
 * @us_min:The min length of this key packet.
 * @us_max:The max length of this key packet.
 *
 * used to record the packet length information.
 */
struct packet_len_stru {
	uint16_t us_copy;
	uint16_t us_min;
	uint16_t us_max;
};

/*
 * struct bst_fg_key_words_stru - key words stru.
 * @us_ofst:The offset address of the keyworks.
 * @us_tot_len:The totle length of this keyworks.
 * @auc_data[0]:The content.
 *
 * used to record the key words structer informations.
 */
struct key_words_stru {
	uint16_t us_ofst;
	uint16_t us_tot_len;
	int16_t us_min_ver;
	int16_t us_max_ver;
	uint8_t auc_data[0];
};

/*
 * struct bst_fg_keyword_stru - key words context in the packet.
 * @us_role:The comm role of this packet.
 * @us_index:The Keyword Index Name, co. with Kernel.
 * @bst_fg_packet_len_stru:Packet length message.
 * @bst_fg_key_words_stru:The keywords of this one.
 *
 * used to record the key words context informations within packets.
 */
struct keyword_stru {
	uint16_t us_role;
	uint16_t us_index;
	struct packet_len_stru st_len_porp;
	struct key_words_stru st_key_word;
};

/*
 * struct bst_fg_app_info_stru - record app info.
 * @l_uid:The uid of Wechat Application.
 * @us_index:The App Index Name, co. with Kernel.
 * @*pst_kws[BST_FG_MAX_KW_NUMBER]:Keywords arry.
 * @st_locker:The Guard Lock of this unit.
 * @pf_proc:The Packet Process Function.
 *
 * used to record the app key information and keywords array.
 */
struct app_info_stru {
	uid_t l_uid;
	uint16_t us_index;
	int16_t us_version;
	spinlock_t st_locker;
	PKT_PROC_T pf_proc;
	struct keyword_stru *pst_kws[MAX_KW_NUMBER];
};

struct socket *hongb_sockfd_lookup_by_fd_pid(int fd, pid_t pid, int *err);
struct sock *hongb_get_sock_by_fd_pid(int fd, pid_t pid);
void hongbao_init(void);
void hognbao_io_ctrl(unsigned long arg);
void check_sock_uid(struct sock *pst_sock, int state);
uint8_t hook_packet(struct sock *pst_sock, uint8_t *p_data,
	uint32_t ul_length, uint32_t ul_role);
bool hook_ul_stub(struct sock *pst_sock, struct msghdr *msg);
void hook_dl_stub(struct sock *sk, struct sk_buff *skb, uint16_t hrd_len);
void hongbao_regist_msg_fun(notify_event *notify);
#endif
