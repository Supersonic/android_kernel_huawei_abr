// SPDX-License-Identifier: GPL-2.0
/*
 * adapter_protocol_ufcs.c
 *
 * ufcs protocol driver
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/delay.h>
#include <linux/byteorder/generic.h>
#include <chipset_common/hwpower/protocol/adapter_protocol.h>
#include <chipset_common/hwpower/protocol/adapter_protocol_ufcs.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <chipset_common/hwpower/common_module/power_delay.h>
#include <chipset_common/hwpower/common_module/power_printk.h>

#define HWLOG_TAG ufcs_protocol
HWLOG_REGIST();

static struct hwufcs_dev *g_hwufcs_dev;

static const struct adapter_protocol_device_data g_hwufcs_dev_data[] = {
	{ PROTOCOL_DEVICE_ID_STM32G031, "stm32g031" },
};

static const char * const g_hwufcs_ctl_msg_table[HWUFCS_CTL_MSG_END] = {
	[HWUFCS_CTL_MSG_PING] = "ping",
	[HWUFCS_CTL_MSG_ACK] = "ack",
	[HWUFCS_CTL_MSG_NCK] = "nck",
	[HWUFCS_CTL_MSG_ACCEPT] = "accept",
	[HWUFCS_CTL_MSG_SOFT_RESET] = "soft_reset",
	[HWUFCS_CTL_MSG_POWER_READY] = "power_ready",
	[HWUFCS_CTL_MSG_GET_OUTPUT_CAPABILITIES] = "get_output_capabilities",
	[HWUFCS_CTL_MSG_GET_SOURCE_INFO] = "get_source_info",
	[HWUFCS_CTL_MSG_GET_SINK_INFO] = "get_sink_info",
	[HWUFCS_CTL_MSG_GET_CABLE_INFO] = "get_cable_info",
	[HWUFCS_CTL_MSG_GET_DEVICE_INFO] = "get_device_info",
	[HWUFCS_CTL_MSG_GET_ERROR_INFO] = "get_error_info",
	[HWUFCS_CTL_MSG_DETECT_CABLE_INFO] = "detect_cable_info",
	[HWUFCS_CTL_MSG_START_CABLE_DETECT] = "start_cable_detect",
	[HWUFCS_CTL_MSG_END_CABLE_DETECT] = "end_cable_detect",
	[HWUFCS_CTL_MSG_EXIT_UFCS_MODE] = "exit_ufcs_mode",
};

static const char * const g_hwufcs_data_msg_table[HWUFCS_DATA_MSG_END] = {
	[HWUFCS_DATA_MSG_OUTPUT_CAPABILITIES] = "output_capabilities",
	[HWUFCS_DATA_MSG_REQUEST] = "request",
	[HWUFCS_DATA_MSG_SOURCE_INFO] = "source_info",
	[HWUFCS_DATA_MSG_SINK_INFO] = "sink_info",
	[HWUFCS_DATA_MSG_CABLE_INFO] = "cable_info",
	[HWUFCS_DATA_MSG_DEVICE_INFO] = "device_info",
	[HWUFCS_DATA_MSG_ERROR_INFO] = "error_info",
	[HWUFCS_DATA_MSG_CONFIG_WATCHDOG] = "config_watchdog",
	[HWUFCS_DATA_MSG_REFUSE] = "refuse",
	[HWUFCS_DATA_MSG_VERIFY_REQUEST] = "verify_request",
	[HWUFCS_DATA_MSG_VERIFY_RESPONSE] = "verify_response",
	[HWUFCS_DATA_MSG_TEST_REQUEST] = "test_request",
};

static int hwufcs_get_device_id(const char *str)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(g_hwufcs_dev_data); i++) {
		if (!strncmp(str, g_hwufcs_dev_data[i].name, strlen(str)))
			return g_hwufcs_dev_data[i].id;
	}

	return -EPERM;
}

static const char *hwufcs_get_ctl_msg_name(unsigned int msg)
{
	if ((msg >= HWUFCS_CTL_MSG_BEGIN) && (msg < HWUFCS_CTL_MSG_END))
		return g_hwufcs_ctl_msg_table[msg];

	return "illegal ctl_msg";
}

static const char *hwufcs_get_data_msg_name(unsigned int msg)
{
	if ((msg >= HWUFCS_DATA_MSG_BEGIN) && (msg < HWUFCS_DATA_MSG_END))
		return g_hwufcs_data_msg_table[msg];

	return "illegal data_msg";
}

static struct hwufcs_dev *hwufcs_get_dev(void)
{
	if (!g_hwufcs_dev) {
		hwlog_err("g_hwufcs_dev is null\n");
		return NULL;
	}

	return g_hwufcs_dev;
}

static struct hwufcs_ops *hwufcs_get_ops(void)
{
	if (!g_hwufcs_dev || !g_hwufcs_dev->p_ops) {
		hwlog_err("g_hwufcs_dev or p_ops is null\n");
		return NULL;
	}

	return g_hwufcs_dev->p_ops;
}

int hwufcs_ops_register(struct hwufcs_ops *ops)
{
	int dev_id;

	if (!g_hwufcs_dev || !ops || !ops->chip_name) {
		hwlog_err("g_hwufcs_dev or ops or chip_name is null\n");
		return -EPERM;
	}

	dev_id = hwufcs_get_device_id(ops->chip_name);
	if (dev_id < 0) {
		hwlog_err("%s ops register fail\n", ops->chip_name);
		return -EPERM;
	}

	g_hwufcs_dev->p_ops = ops;
	g_hwufcs_dev->dev_id = dev_id;

	hwlog_info("%d:%s ops register ok\n", dev_id, ops->chip_name);
	return 0;
}

static u8 hwufcs_get_msg_number(void)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();
	u8 msg_number;

	if (!l_dev)
		return 0;

	mutex_lock(&l_dev->msg_number_lock);
	msg_number = l_dev->info.msg_number;
	mutex_unlock(&l_dev->msg_number_lock);
	return msg_number;
}

static void hwufcs_set_msg_number(u8 msg_number)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();

	if (!l_dev)
		return;

	mutex_lock(&l_dev->msg_number_lock);
	l_dev->info.msg_number = (msg_number % HWUFCS_MSG_MAX_COUNTER);
	mutex_unlock(&l_dev->msg_number_lock);
	hwlog_info("old_msg_number=%u new_msg_number=%u\n",
		msg_number, l_dev->info.msg_number);
}

static void hwufcs_packet_tx_header(struct hwufcs_package_data *pkt,
	struct hwufcs_header_data *hdr)
{
	u16 data = 0;

	/* packet tx header data */
	data |= ((pkt->msg_type & HWUFCS_HDR_MASK_MSG_TYPE) <<
		HWUFCS_HDR_SHIFT_MSG_TYPE);
	data |= ((pkt->prot_version & HWUFCS_HDR_MASK_PROT_VERSION) <<
		HWUFCS_HDR_SHIFT_PROT_VERSION);
	data |= ((pkt->msg_number & HWUFCS_HDR_MASK_MSG_NUMBER) <<
		HWUFCS_HDR_SHIFT_MSG_NUMBER);
	data |= ((pkt->dev_address & HWUFCS_HDR_MASK_DEV_ADDRESS) <<
		HWUFCS_HDR_SHIFT_DEV_ADDRESS);

	/* fill tx header data */
	hdr->header_h = (data >> POWER_BITS_PER_BYTE) & POWER_MASK_BYTE;
	hdr->header_l = (data >> 0) & POWER_MASK_BYTE;
	hdr->cmd = pkt->cmd;
	hdr->length = pkt->length;
}

static void hwufcs_unpack_rx_header(struct hwufcs_package_data *pkt,
	struct hwufcs_header_data *hdr)
{
	u16 data;

	/* unpacket rx header data */
	data = (hdr->header_h << POWER_BITS_PER_BYTE) | hdr->header_l;
	pkt->msg_type = (data >> HWUFCS_HDR_SHIFT_MSG_TYPE) &
		HWUFCS_HDR_MASK_MSG_TYPE;
	pkt->prot_version = (data >> HWUFCS_HDR_SHIFT_PROT_VERSION) &
		HWUFCS_HDR_MASK_PROT_VERSION;
	pkt->msg_number = (data >> HWUFCS_HDR_SHIFT_MSG_NUMBER) &
		HWUFCS_HDR_MASK_MSG_NUMBER;
	pkt->dev_address = (data >> HWUFCS_HDR_SHIFT_DEV_ADDRESS) &
		HWUFCS_HDR_MASK_DEV_ADDRESS;
	pkt->cmd = hdr->cmd;
	pkt->length = hdr->length;
}

static int hwufcs_detect_adapter(void)
{
	struct hwufcs_ops *l_ops = hwufcs_get_ops();

	if (!l_ops)
		return -EPERM;

	if (!l_ops->detect_adapter) {
		hwlog_err("detect_adapter is null\n");
		return -EPERM;
	}

	hwlog_info("detect_adapter\n");

	return l_ops->detect_adapter(l_ops->dev_data);
}

static int hwufcs_soft_reset_master(void)
{
	struct hwufcs_ops *l_ops = hwufcs_get_ops();

	if (!l_ops)
		return -EPERM;

	if (!l_ops->soft_reset_master) {
		hwlog_err("soft_reset_master is null\n");
		return -EPERM;
	}

	hwlog_info("soft_reset_master\n");

	return l_ops->soft_reset_master(l_ops->dev_data);
}

static int hwufcs_send_msg_header(struct hwufcs_package_data *pkt,
	struct hwufcs_header_data *hdr)
{
	struct hwufcs_ops *l_ops = hwufcs_get_ops();
	u8 data[HWUFCS_HDR_HEADER_LEN] = { 0 };

	if (!l_ops)
		return -EPERM;

	if (!l_ops->send_msg_header) {
		hwlog_err("send_msg_header is null\n");
		return -EPERM;
	}

	hwufcs_packet_tx_header(pkt, hdr);
	data[HWUFCS_HDR_HEADER_H_OFFSET] = hdr->header_h;
	data[HWUFCS_HDR_HEADER_L_OFFSET] = hdr->header_l;
	data[HWUFCS_HDR_CMD_OFFSET] = hdr->cmd;
	data[HWUFCS_HDR_LENGTH_OFFSET] = hdr->length;

	if (pkt->msg_type == HWUFCS_MSG_TYPE_CONTROL)
		hwlog_info("tx_ctl_msg=%s type=%u ver=%u num=%u addr=%u cmd=%u len=%u\n",
			hwufcs_get_ctl_msg_name(pkt->cmd),
			pkt->msg_type, pkt->prot_version, pkt->msg_number,
			pkt->dev_address, pkt->cmd, pkt->length);
	if (pkt->msg_type == HWUFCS_MSG_TYPE_DATA)
		hwlog_info("tx_data_cmd=%s type=%u ver=%u num=%u addr=%u cmd=%u len=%u\n",
			hwufcs_get_data_msg_name(pkt->cmd),
			pkt->msg_type, pkt->prot_version, pkt->msg_number,
			pkt->dev_address, pkt->cmd, pkt->length);

	return l_ops->send_msg_header(l_ops->dev_data, data, HWUFCS_HDR_HEADER_LEN);
}

static int hwufcs_send_msg_body(u8 *data, u8 len)
{
	struct hwufcs_ops *l_ops = hwufcs_get_ops();

	if (!l_ops)
		return -EPERM;

	if (!l_ops->send_msg_body) {
		hwlog_err("send_msg_body is null\n");
		return -EPERM;
	}

	return l_ops->send_msg_body(l_ops->dev_data, data, len);
}

static int hwufcs_end_send_msg(u8 flag)
{
	struct hwufcs_ops *l_ops = hwufcs_get_ops();

	if (!l_ops)
		return -EPERM;

	if (!l_ops->end_send_msg) {
		hwlog_err("end_send_msg is null\n");
		return -EPERM;
	}

	hwlog_info("end_send_msg: flag=%02x\n", flag);

	return l_ops->end_send_msg(l_ops->dev_data, flag);
}

static int hwufcs_wait_msg_ready(u8 flag)
{
	struct hwufcs_ops *l_ops = hwufcs_get_ops();

	if (!l_ops)
		return -EPERM;

	if (!l_ops->wait_msg_ready) {
		hwlog_err("wait_msg_ready is null\n");
		return -EPERM;
	}

	hwlog_info("wait_msg_ready: flag=%02x\n", flag);

	return l_ops->wait_msg_ready(l_ops->dev_data, flag);
}

static int hwufcs_receive_msg_header(struct hwufcs_package_data *pkt,
	struct hwufcs_header_data *hdr)
{
	struct hwufcs_ops *l_ops = hwufcs_get_ops();
	u8 data[HWUFCS_HDR_HEADER_LEN] = { 0 };
	int ret;

	if (!l_ops)
		return -EPERM;

	if (!l_ops->receive_msg_header) {
		hwlog_err("receive_msg_header is null\n");
		return -EPERM;
	}

	ret = l_ops->receive_msg_header(l_ops->dev_data, data, HWUFCS_HDR_HEADER_LEN);

	hdr->header_h = data[HWUFCS_HDR_HEADER_H_OFFSET];
	hdr->header_l = data[HWUFCS_HDR_HEADER_L_OFFSET];
	hdr->cmd = data[HWUFCS_HDR_CMD_OFFSET];
	hdr->length = data[HWUFCS_HDR_LENGTH_OFFSET];
	hwufcs_unpack_rx_header(pkt, hdr);

	if (pkt->msg_type == HWUFCS_MSG_TYPE_CONTROL)
		hwlog_info("rx_ctl_msg=%s type=%u ver=%u num=%u addr=%u cmd=%u len=%u\n",
			hwufcs_get_ctl_msg_name(pkt->cmd),
			pkt->msg_type, pkt->prot_version, pkt->msg_number,
			pkt->dev_address, pkt->cmd, pkt->length);
	if (pkt->msg_type == HWUFCS_MSG_TYPE_DATA)
		hwlog_info("rx_data_msg=%s type=%u ver=%u num=%u addr=%u cmd=%u len=%u\n",
			hwufcs_get_data_msg_name(pkt->cmd),
			pkt->msg_type, pkt->prot_version, pkt->msg_number,
			pkt->dev_address, pkt->cmd, pkt->length);

	return ret;
}

static int hwufcs_receive_msg_body(u8 *data, u8 len)
{
	struct hwufcs_ops *l_ops = hwufcs_get_ops();

	if (!l_ops)
		return -EPERM;

	if (!l_ops->receive_msg_body) {
		hwlog_err("receive_msg_body is null\n");
		return -EPERM;
	}

	hwlog_info("receive_msg_body\n");

	return l_ops->receive_msg_body(l_ops->dev_data, data, len);
}

static int hwufcs_end_receive_msg(void)
{
	struct hwufcs_ops *l_ops = hwufcs_get_ops();

	if (!l_ops)
		return -EPERM;

	if (!l_ops->end_receive_msg) {
		hwlog_err("end_receive_msg is null\n");
		return -EPERM;
	}

	hwlog_info("end_receive_msg\n");

	return l_ops->end_receive_msg(l_ops->dev_data);
}

static int hwufcs_send_control_cmd(u8 msg_number, u8 cmd)
{
	struct hwufcs_package_data pkt = { 0 };
	struct hwufcs_header_data hdr = { 0 };
	u8 flag = HWUFCS_WAIT_SEND_PACKET_COMPLETE;
	int ret;

	pkt.msg_type = HWUFCS_MSG_TYPE_CONTROL;
	pkt.prot_version = HWUFCS_MSG_PROT_VERSION;
	pkt.msg_number = msg_number;
	pkt.dev_address = HWUFCS_DEV_ADDRESS_SOURCE;
	pkt.cmd = cmd;
	pkt.length = 0;

	ret = hwufcs_send_msg_header(&pkt, &hdr);
	ret += hwufcs_end_send_msg(flag);
	if (ret) {
		hwlog_err("send_control_cmd msg_number=%u cmd=%u fail\n",
			msg_number, cmd);
		return -EPERM;
	}

	return 0;
}

static int hwufcs_send_data_cmd(u8 msg_number, u8 cmd, u8 *data, u8 len)
{
	struct hwufcs_package_data pkt = { 0 };
	struct hwufcs_header_data hdr = { 0 };
	u8 flag = HWUFCS_WAIT_SEND_PACKET_COMPLETE;
	int ret;

	pkt.msg_type = HWUFCS_MSG_TYPE_DATA;
	pkt.prot_version = HWUFCS_MSG_PROT_VERSION;
	pkt.msg_number = msg_number;
	pkt.dev_address = HWUFCS_DEV_ADDRESS_SOURCE;
	pkt.cmd = cmd;
	pkt.length = len;

	ret = hwufcs_send_msg_header(&pkt, &hdr);
	ret += hwufcs_send_msg_body(data, len);
	ret += hwufcs_end_send_msg(flag);
	if (ret) {
		hwlog_err("send_data_msg msg_number=%u cmd=%u fail\n",
			msg_number, cmd);
		return -EPERM;
	}

	return 0;
}

static int hwufcs_send_control_msg(u8 cmd, bool ack)
{
	struct hwufcs_package_data pkt = { 0 };
	struct hwufcs_header_data hdr = { 0 };
	u8 msg_number = hwufcs_get_msg_number();
	u8 flag = HWUFCS_WAIT_CRC_ERROR | HWUFCS_WAIT_DATA_READY;
	u8 retry = 0;
	int ret;

begin:
	/* send control cmd to tx */
	ret = hwufcs_send_control_cmd(msg_number, cmd);
	if (ret)
		return -EPERM;

	if (!ack)
		goto end;

	/* wait msg from tx */
	ret = hwufcs_wait_msg_ready(flag);
	if (ret) {
		if (retry++ >= HWUFCS_SEND_MSG_MAX_RETRY)
			return -EPERM;
		hwlog_err("wait for ack time out, retry=%u\n", retry);
		goto begin;
	}

	/* receive msg header from tx */
	ret = hwufcs_receive_msg_header(&pkt, &hdr);
	if (ret)
		return -EPERM;

	/* end */
	ret = hwufcs_end_receive_msg();
	if (ret)
		return -EPERM;

	/* retry send cmd when receive nck cmd */
	if (pkt.cmd == HWUFCS_CTL_MSG_NCK) {
		if (retry++ >= HWUFCS_SEND_MSG_MAX_RETRY)
			return -EPERM;
		hwlog_err("send_control_msg cmd=nck retry=%u\n", retry);
		goto begin;
	}

	/* check ack control msg header */
	if (pkt.msg_type != HWUFCS_MSG_TYPE_CONTROL) {
		hwlog_err("send_control_msg msg_type=%u,%u invalid\n",
			pkt.msg_type, HWUFCS_MSG_TYPE_CONTROL);
		return -EPERM;
	}
	if (pkt.msg_number != msg_number) {
		hwlog_err("send_control_msg msg_number=%u,%u invalid\n",
			pkt.msg_number, msg_number);
		return -EPERM;
	}
	if (pkt.cmd != HWUFCS_CTL_MSG_ACK) {
		hwlog_err("send_control_msg cmd=%u,%u invalid\n",
			pkt.cmd, HWUFCS_CTL_MSG_ACK);
		return -EPERM;
	}

end:
	hwufcs_set_msg_number(++msg_number);
	return 0;
}

static int hwufcs_send_data_msg(u8 cmd, u8 *data, u8 len, bool ack)
{
	struct hwufcs_package_data pkt = { 0 };
	struct hwufcs_header_data hdr = { 0 };
	u8 msg_number = hwufcs_get_msg_number();
	u8 flag = HWUFCS_WAIT_CRC_ERROR | HWUFCS_WAIT_DATA_READY;
	u8 retry = 0;
	int ret;

begin:
	/* send data cmd to tx */
	ret = hwufcs_send_data_cmd(msg_number, cmd, data, len);
	if (ret)
		return -EPERM;

	if (!ack)
		goto end;

	/* wait msg from tx */
	ret = hwufcs_wait_msg_ready(flag);
	if (ret) {
		if (retry++ >= HWUFCS_SEND_MSG_MAX_RETRY)
			return -EPERM;
		hwlog_err("wait for ack time out, retry=%u\n", retry);
		goto begin;
	}

	/* receive msg header from tx */
	ret = hwufcs_receive_msg_header(&pkt, &hdr);
	if (ret)
		return -EPERM;

	/* end */
	ret = hwufcs_end_receive_msg();
	if (ret)
		return -EPERM;

	/* retry send cmd when receive nck cmd */
	if (pkt.cmd == HWUFCS_CTL_MSG_NCK) {
		hwlog_err("send_data_msg cmd=nck retry=%u\n", retry);
		if (retry++ >= HWUFCS_SEND_MSG_MAX_RETRY)
			return -EPERM;
		goto begin;
	}

	/* check ack control msg header */
	if (pkt.msg_type != HWUFCS_MSG_TYPE_CONTROL) {
		hwlog_err("send_data_msg msg_type=%u,%u invalid\n",
			pkt.msg_type, HWUFCS_MSG_TYPE_CONTROL);
		return -EPERM;
	}
	if (pkt.msg_number != msg_number) {
		hwlog_err("send_data_msg msg_number=%u,%u invalid\n",
			pkt.msg_number, msg_number);
		return -EPERM;
	}
	if (pkt.cmd != HWUFCS_CTL_MSG_ACK) {
		hwlog_err("send_data_msg cmd=%u,%u invalid\n",
			pkt.cmd, HWUFCS_CTL_MSG_ACK);
		return -EPERM;
	}

end:
	hwufcs_set_msg_number(++msg_number);
	return 0;
}

static int hwufcs_receive_control_msg(u8 cmd)
{
	struct hwufcs_package_data pkt = { 0 };
	struct hwufcs_header_data hdr = { 0 };
	u8 flag = HWUFCS_WAIT_CRC_ERROR | HWUFCS_WAIT_DATA_READY;
	int ret;

	/* wait msg from tx */
	ret = hwufcs_wait_msg_ready(flag);
	if (ret)
		return -EPERM;

	/* receive msg header from tx */
	ret = hwufcs_receive_msg_header(&pkt, &hdr);
	if (ret)
		return -EPERM;

	/* end */
	ret = hwufcs_end_receive_msg();
	if (ret)
		return -EPERM;

	/* check control msg header */
	if (pkt.msg_type != HWUFCS_MSG_TYPE_CONTROL) {
		hwlog_err("receive_control_msg msg_type=%u,%u invalid\n",
			pkt.msg_type, HWUFCS_MSG_TYPE_CONTROL);
		return -HWUFCS_MSG_NOT_MATCH;
	}

	/* check control msg header */
	if (pkt.cmd != cmd) {
		hwlog_err("receive_control_msg cmd=%u,%u invalid\n",
			pkt.cmd, cmd);
		return -EPERM;
	}

	return 0;
}

static int hwufcs_receive_data_msg(u8 cmd, u8 *data, u8 len, u8 *ret_len)
{
	struct hwufcs_package_data pkt = { 0 };
	struct hwufcs_header_data hdr = { 0 };
	u8 flag = HWUFCS_WAIT_CRC_ERROR | HWUFCS_WAIT_DATA_READY;
	int ret;

	/* wait msg from tx */
	ret = hwufcs_wait_msg_ready(flag);
	if (ret)
		return -EPERM;

	/* receive msg header from tx */
	ret = hwufcs_receive_msg_header(&pkt, &hdr);
	if (ret)
		return -EPERM;

	/* check data msg header */
	if (pkt.msg_type != HWUFCS_MSG_TYPE_DATA) {
		hwlog_err("receive_data_msg msg_type=%u,%u invalid\n",
			pkt.msg_type, HWUFCS_MSG_TYPE_DATA);
		return -HWUFCS_MSG_NOT_MATCH;
	}

	/* check data msg header */
	if (pkt.cmd != cmd) {
		hwlog_err("receive_data_msg cmd=%u,%u invalid\n",
			pkt.cmd, cmd);
		return -EPERM;
	}
	if (pkt.length > len) {
		hwlog_err("receive_data_msg length=%u,%u invalid\n",
			pkt.length, len);
		return -EPERM;
	}

	/* receive msg body from tx */
	ret = hwufcs_receive_msg_body(data, pkt.length);
	if (ret)
		return -EPERM;

	*ret_len = pkt.length;

	/* end */
	return hwufcs_end_receive_msg();
}

/* control message from sink to source: 0x00 ping */
static int hwufcs_send_ping_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_PING, true);
}

#ifdef POWER_MODULE_DEBUG_FUNCTION
/* control message from sink to source: 0x01 ack */
static int hwufcs_send_ack_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_ACK, false);
}

/* control message from sink to source: 0x02 nck */
static int hwufcs_send_nck_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_NCK, false);
}

/* control message from sink to source: 0x03 accept */
static int hwufcs_send_accept_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_ACCEPT, true);
}

/* control message from sink to source: 0x04 soft_reset */
static int hwufcs_send_soft_reset_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_SOFT_RESET, true);
}

/* control message from sink to source: 0x05 power_ready */
static int hwufcs_send_power_ready_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_POWER_READY, true);
}
#endif /* POWER_MODULE_DEBUG_FUNCTION */

/* control message from sink to source: 0x06 get_output_capabilities */
static int hwufcs_send_get_output_capabilities_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_GET_OUTPUT_CAPABILITIES, true);
}

/* control message from sink to source: 0x07 get_source_info */
static int hwufcs_send_get_source_info_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_GET_SOURCE_INFO, true);
}

#ifdef POWER_MODULE_DEBUG_FUNCTION
/* control message from sink to source: 0x08 get_sink_info */
static int hwufcs_send_get_sink_info_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_GET_SINK_INFO, true);
}

/* control message from sink to source: 0x09 get_cable_info */
static int hwufcs_send_get_cable_info_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_GET_CABLE_INFO, true);
}
#endif /* POWER_MODULE_DEBUG_FUNCTION */

/* control message from sink to source: 0x0a get_device_info */
static int hwufcs_send_get_dev_info_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_GET_DEVICE_INFO, true);
}

#ifdef POWER_MODULE_DEBUG_FUNCTION
/* control message from sink to source: 0x0b get_error_info */
static int hwufcs_send_get_error_info_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_GET_ERROR_INFO, true);
}

/* control message from sink to source: 0x0c detect_cable_info */
static int hwufcs_send_detect_cable_info_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_DETECT_CABLE_INFO, true);
}

/* control message from sink to source: 0x0d start_cable_detect */
static int hwufcs_send_start_cable_detect_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_START_CABLE_DETECT, true);
}

/* control message from sink to source: 0x0e end_cable_detect */
static int hwufcs_send_end_cable_detect_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_END_CABLE_DETECT, true);
}

/* control message from sink to source: 0x0f exit_ufcs_mode */
static int hwufcs_send_exit_ufcs_mode_ctrl_msg(void)
{
	return hwufcs_send_control_msg(HWUFCS_CTL_MSG_EXIT_HWUFCS_MODE, true);
}
#endif /* POWER_MODULE_DEBUG_FUNCTION */

/* control message from source to sink: 0x03 accept */
static int hwufcs_receive_accept_ctrl_msg(void)
{
	return hwufcs_receive_control_msg(HWUFCS_CTL_MSG_ACCEPT);
}

/* control message from source to sink: 0x05 power_ready */
static int hwufcs_receive_power_ready_ctrl_msg(void)
{
	return hwufcs_receive_control_msg(HWUFCS_CTL_MSG_POWER_READY);
}

/* data message from sink to source: 0x02 request */
static int hwufcs_send_request_data_msg(struct hwufcs_request_data *p)
{
	u64 data = 0;
	u64 tmp_data;
	u8 len = sizeof(u64);

	data |= ((u64)((p->output_curr / HWUFCS_REQ_UNIT_OUTPUT_CURR) &
		HWUFCS_REQ_MASK_OUTPUT_CURR) << HWUFCS_REQ_SHIFT_OUTPUT_CURR);
	data |= ((u64)((p->output_volt / HWUFCS_REQ_UNIT_OUTPUT_VOLT) &
		HWUFCS_REQ_MASK_OUTPUT_VOLT) << HWUFCS_REQ_SHIFT_OUTPUT_VOLT);
	data |= ((u64)(p->output_mode & HWUFCS_REQ_MASK_OUTPUT_MODE) <<
		HWUFCS_REQ_SHIFT_OUTPUT_MODE);
	tmp_data = be64_to_cpu(data);

	return hwufcs_send_data_msg(HWUFCS_DATA_MSG_REQUEST,
		(u8 *)&tmp_data, len, true);
}

#ifdef POWER_MODULE_DEBUG_FUNCTION
/* data message from sink to source: 0x04 sink_information */
static int hwufcs_send_sink_information_data_msg(struct hwufcs_sink_info_data *p)
{
	u64 data = 0;
	u64 tmp_data;
	u8 len = sizeof(u64);

	data |= ((u64)((p->bat_curr / HWUFCS_SINK_INFO_UNIT_CURR) &
		HWUFCS_SINK_INFO_MASK_BAT_CURR) << HWUFCS_SINK_INFO_SHIFT_BAT_CURR);
	data |= ((u64)((p->bat_volt / HWUFCS_SINK_INFO_UNIT_VOLT) &
		HWUFCS_SINK_INFO_MASK_BAT_VOLT) << HWUFCS_SINK_INFO_SHIFT_BAT_VOLT);
	data |= ((u64)((p->usb_temp + HWUFCS_SINK_INFO_TEMP_BASE) &
		HWUFCS_SINK_INFO_MASK_USB_TEMP) << HWUFCS_SINK_INFO_SHIFT_USB_TEMP);
	data |= ((u64)((p->bat_temp + HWUFCS_SINK_INFO_TEMP_BASE) &
		HWUFCS_SINK_INFO_MASK_BAT_TEMP) << HWUFCS_SINK_INFO_SHIFT_BAT_TEMP);
	tmp_data = be64_to_cpu(data);

	return hwufcs_send_data_msg(HWUFCS_DATA_MSG_SINK_INFO,
		(u8 *)&tmp_data, len, true);
}

/* data message from sink to source: 0x05 cable_information */
static int hwufcs_send_cable_information_data_msg(struct hwufcs_cable_info_data *p)
{
	u64 data = 0;
	u64 tmp_data;
	u8 len = sizeof(u64);

	data |= ((u64)((p->max_curr / HWUFCS_CABLE_INFO_UNIT_CURR) &
		HWUFCS_CABLE_INFO_MASK_MAX_CURR) << HWUFCS_CABLE_INFO_SHIFT_MAX_CURR);
	data |= ((u64)((p->max_volt / HWUFCS_CABLE_INFO_UNIT_VOLT) &
		HWUFCS_CABLE_INFO_MASK_MAX_VOLT) << HWUFCS_CABLE_INFO_SHIFT_MAX_VOLT);
	data |= ((p->resistance & HWUFCS_CABLE_INFO_MASK_RESISTANCE) <<
		HWUFCS_CABLE_INFO_SHIFT_RESISTANCE);
	data |= ((u64)(p->elable_vid & HWUFCS_CABLE_INFO_MASK_ELABLE_VID) <<
		HWUFCS_CABLE_INFO_SHIFT_ELABLE_VID);
	data |= ((u64)(p->vid & HWUFCS_CABLE_INFO_MASK_VID) <<
		HWUFCS_CABLE_INFO_SHIFT_VID);
	tmp_data = be64_to_cpu(data);

	return hwufcs_send_data_msg(HWUFCS_DATA_MSG_CABLE_INFO,
		(u8 *)&tmp_data, len, true);
}

/* data message from sink to source: 0x06 device_information */
static int hwufcs_send_device_information_data_msg(struct hwufcs_dev_info_data *p)
{
	u64 data = 0;
	u64 tmp_data;
	u8 len = sizeof(u64);

	data |= ((u64)(p->sw_ver & HWUFCS_DEV_INFO_MASK_SW_VER) <<
		HWUFCS_DEV_INFO_SHIFT_SW_VER);
	data |= ((u64)(p->hw_ver & HWUFCS_DEV_INFO_MASK_HW_VER) <<
		HWUFCS_DEV_INFO_SHIFT_HW_VER);
	data |= ((u64)(p->chip_vid & HWUFCS_DEV_INFO_MASK_CHIP_VID) <<
		HWUFCS_DEV_INFO_SHIFT_CHIP_VID);
	data |= ((u64)(p->manu_vid & HWUFCS_DEV_INFO_MASK_MANU_VID) <<
		HWUFCS_DEV_INFO_SHIFT_MANU_VID);
	tmp_data = be64_to_cpu(data);

	return hwufcs_send_data_msg(HWUFCS_DATA_MSG_DEVICE_INFO,
		(u8 *)&tmp_data, len, true);
}

/* data message from sink to source: 0x07 error_information */
static int hwufcs_send_error_information_data_msg(struct hwufcs_error_info_data *p)
{
	u32 data = 0;
	u32 tmp_data;
	u8 len = sizeof(u32);

	data |= ((u32)(p->output_ovp & HWUFCS_ERROR_INFO_MASK_OUTPUT_OVP) <<
		HWUFCS_ERROR_INFO_SHIFT_OUTPUT_OVP);
	data |= ((u32)(p->output_uvp & HWUFCS_ERROR_INFO_MASK_OUTPUT_UVP) <<
		HWUFCS_ERROR_INFO_SHIFT_OUTPUT_UVP);
	data |= ((u32)(p->output_ocp & HWUFCS_ERROR_INFO_MASK_OUTPUT_OCP) <<
		HWUFCS_ERROR_INFO_SHIFT_OUTPUT_OCP);
	data |= ((u32)(p->output_scp & HWUFCS_ERROR_INFO_MASK_OUTPUT_SCP) <<
		HWUFCS_ERROR_INFO_SHIFT_OUTPUT_SCP);
	data |= ((u32)(p->usb_otp & HWUFCS_ERROR_INFO_MASK_USB_OTP) <<
		HWUFCS_ERROR_INFO_SHIFT_USB_OTP);
	data |= ((u32)(p->device_otp & HWUFCS_ERROR_INFO_MASK_DEVICE_OTP) <<
		HWUFCS_ERROR_INFO_SHIFT_DEVICE_OTP);
	data |= ((u32)(p->cc_ovp & HWUFCS_ERROR_INFO_MASK_CC_OVP) <<
		HWUFCS_ERROR_INFO_SHIFT_CC_OVP);
	data |= ((u32)(p->dminus_ovp & HWUFCS_ERROR_INFO_MASK_DMINUS_OVP) <<
		HWUFCS_ERROR_INFO_SHIFT_DMINUS_OVP);
	data |= ((u32)(p->dplus_ovp & HWUFCS_ERROR_INFO_MASK_DPLUS_OVP) <<
		HWUFCS_ERROR_INFO_SHIFT_DPLUS_OVP);
	data |= ((u32)(p->input_ovp & HWUFCS_ERROR_INFO_MASK_INPUT_OVP) <<
		HWUFCS_ERROR_INFO_SHIFT_INPUT_OVP);
	data |= ((u32)(p->input_uvp & HWUFCS_ERROR_INFO_MASK_INPUT_UVP) <<
		HWUFCS_ERROR_INFO_SHIFT_INPUT_UVP);
	data |= ((u32)(p->over_leakage & HWUFCS_ERROR_INFO_MASK_OVER_LEAKAGE) <<
		HWUFCS_ERROR_INFO_SHIFT_OVER_LEAKAGE);
	data |= ((u32)(p->input_drop & HWUFCS_ERROR_INFO_MASK_INPUT_DROP) <<
		HWUFCS_ERROR_INFO_SHIFT_INPUT_DROP);
	data |= ((u32)(p->crc_error & HWUFCS_ERROR_INFO_MASK_CRC_ERROR) <<
		HWUFCS_ERROR_INFO_SHIFT_CRC_ERROR);
	data |= ((u32)(p->wtg_overflow & HWUFCS_ERROR_INFO_MASK_WTG_OVERFLOW) <<
		HWUFCS_ERROR_INFO_SHIFT_WTG_OVERFLOW);
	tmp_data = be32_to_cpu(data);

	return hwufcs_send_data_msg(HWUFCS_DATA_MSG_ERROR_INFO,
		(u8 *)&tmp_data, len, true);
}
#endif /* POWER_MODULE_DEBUG_FUNCTION */

/* data message from sink to source: 0x08 config_watchdog */
static int hwufcs_send_config_watchdog_data_msg(struct hwufcs_wtg_data *p)
{
	u16 data = 0;
	u16 tmp_data;
	u8 len = sizeof(u16);

	data |= ((u16)(p->time & HWUFCS_WTG_MASK_TIME) <<
		HWUFCS_WTG_SHIFT_TIME);
	tmp_data = be16_to_cpu(data);

	return hwufcs_send_data_msg(HWUFCS_DATA_MSG_CONFIG_WATCHDOG,
		(u8 *)&tmp_data, len, true);
}

#ifdef POWER_MODULE_DEBUG_FUNCTION
/* data message from sink to source: 0x09 refuse */
static int hwufcs_send_refuse_data_msg(struct hwufcs_refuse_data *p)
{
	u32 data = 0;
	u32 tmp_data;
	u8 len = sizeof(u32);

	data |= ((u32)(p->reason & HWUFCS_REFUSE_MASK_REASON) <<
		HWUFCS_REFUSE_SHIFT_REASON);
	data |= ((u32)(p->cmd_number & HWUFCS_REFUSE_MASK_CMD_NUMBER) <<
		HWUFCS_REFUSE_SHIFT_CMD_NUMBER);
	data |= ((u32)(p->msg_type & HWUFCS_REFUSE_MASK_MSG_TYPE) <<
		HWUFCS_REFUSE_SHIFT_MSG_TYPE);
	data |= ((u32)(p->msg_number & HWUFCS_REFUSE_MASK_MSG_NUMBER) <<
		HWUFCS_REFUSE_SHIFT_MSG_NUMBER);
	tmp_data = be32_to_cpu(data);

	return hwufcs_send_data_msg(HWUFCS_DATA_MSG_REFUSE,
		(u8 *)&tmp_data, len, true);
}

/* data message from sink to source: 0x0a verify_request */
static int hwufcs_send_verify_request_data_msg(struct hwufcs_verify_request_data *p)
{
	u8 len = sizeof(struct hwufcs_verify_request_data);

	return hwufcs_send_data_msg(HWUFCS_DATA_MSG_VERIFY_REQUEST,
		(u8 *)p, len, true);
}

/* data message from sink to source: 0x0b verify_response */
static int hwufcs_send_verify_response_data_msg(struct hwufcs_verify_response_data *p)
{
	u8 len = sizeof(struct hwufcs_verify_response_data);

	return hwufcs_send_data_msg(HWUFCS_DATA_MSG_VERIFY_RESPONSE,
		(u8 *)p, len, true);
}

/* data message from sink to source: 0xff test request */
static int hwufcs_send_test_request_data_msg(struct hwufcs_test_request_data *p)
{
	u16 data = 0;
	u16 tmp_data;
	u8 len = sizeof(u16);

	data |= ((u16)(p->msg_number & HWUFCS_TEST_REQ_MASK_MSG_NUMBER) <<
		HWUFCS_TEST_REQ_SHIFT_MSG_NUMBER);
	data |= ((u16)(p->msg_type & HWUFCS_TEST_REQ_MASK_MSG_TYPE) <<
		HWUFCS_TEST_REQ_SHIFT_MSG_TYPE);
	data |= ((u16)(p->dev_address & HWUFCS_TEST_REQ_MASK_DEV_ADDRESS) <<
		HWUFCS_TEST_REQ_SHIFT_DEV_ADDRESS);
	data |= ((u16)(p->volt_test_mode & HWUFCS_TEST_REQ_MASK_VOLT_TEST_MODE) <<
		HWUFCS_TEST_REQ_SHIFT_VOLT_TEST_MODE);
	tmp_data = be16_to_cpu(data);

	return hwufcs_send_data_msg(HWUFCS_DATA_MSG_TEST_REQUEST,
		(u8 *)&tmp_data, len, true);
}
#endif /* POWER_MODULE_DEBUG_FUNCTION */

/* data message from source to sink: 0x01 output_capabilities */
static int hwufcs_receive_output_capabilities_data_msg(struct hwufcs_capabilities_data *p,
	u8 *ret_len)
{
	u64 data[HWUFCS_CAP_MAX_OUTPUT_MODE] = { 0 };
	u64 tmp_data;
	u8 len = HWUFCS_CAP_MAX_OUTPUT_MODE * sizeof(u64);
	u8 tmp_len = 0;
	int ret;
	u8 i;

	ret = hwufcs_receive_data_msg(HWUFCS_DATA_MSG_OUTPUT_CAPABILITIES,
		(u8 *)&data, len, &tmp_len);
	if (ret)
		return -EPERM;

	if (tmp_len % sizeof(u64) != 0) {
		hwlog_err("output_capabilities length=%u,%u invalid\n", len, tmp_len);
		return -EPERM;
	}

	*ret_len = tmp_len / sizeof(u64);
	for (i = 0; i < *ret_len; i++) {
		tmp_data = cpu_to_be64(data[i]);
		/* min output current */
		p[i].min_curr = (tmp_data >> HWUFCS_CAP_SHIFT_MIN_CURR) &
			HWUFCS_CAP_MASK_MIN_CURR;
		p[i].min_curr *= HWUFCS_CAP_UNIT_CURR;
		/* max output current */
		p[i].max_curr = (tmp_data >> HWUFCS_CAP_SHIFT_MAX_CURR) &
			HWUFCS_CAP_MASK_MAX_CURR;
		p[i].max_curr *= HWUFCS_CAP_UNIT_CURR;
		/* min output voltage */
		p[i].min_volt = (tmp_data >> HWUFCS_CAP_SHIFT_MIN_VOLT) &
			HWUFCS_CAP_MASK_MIN_VOLT;
		p[i].min_volt *= HWUFCS_CAP_UNIT_VOLT;
		/* max output voltage */
		p[i].max_volt = (tmp_data >> HWUFCS_CAP_SHIFT_MAX_VOLT) &
			HWUFCS_CAP_MASK_MAX_VOLT;
		p[i].max_volt *= HWUFCS_CAP_UNIT_VOLT;
		/* voltage adjust step */
		p[i].volt_step = (tmp_data >> HWUFCS_CAP_SHIFT_VOLT_STEP) &
			HWUFCS_CAP_MASK_VOLT_STEP;
		p[i].volt_step++;
		p[i].volt_step *= HWUFCS_CAP_UNIT_VOLT;
		/* current adjust step */
		p[i].curr_step = (tmp_data >> HWUFCS_CAP_SHIFT_CURR_STEP) &
			HWUFCS_CAP_MASK_CURR_STEP;
		p[i].curr_step++;
		p[i].curr_step *= HWUFCS_CAP_UNIT_CURR;
		/* output mode */
		p[i].output_mode = (tmp_data >> HWUFCS_CAP_SHIFT_OUTPUT_MODE) &
			HWUFCS_CAP_MASK_OUTPUT_MODE;
	}

	for (i = 0; i < *ret_len; i++)
		hwlog_info("cap[%u]=%lx %umA %umA %umV %umV %umV %umA %u\n", i, tmp_data,
			p[i].min_curr, p[i].max_curr,
			p[i].min_volt, p[i].max_volt,
			p[i].volt_step, p[i].curr_step,
			p[i].output_mode);
	return 0;
}

/* data message from source to sink: 0x03 source_information */
static int hwufcs_receive_source_info_data_msg(struct hwufcs_source_info_data *p)
{
	u64 data = 0;
	u64 tmp_data;
	u8 len = sizeof(u64);
	u8 tmp_len = 0;
	int ret;

	ret = hwufcs_receive_data_msg(HWUFCS_DATA_MSG_SOURCE_INFO,
		(u8 *)&data, len, &tmp_len);
	if (ret)
		return -EPERM;

	if (len != tmp_len) {
		hwlog_err("source_info length=%u,%u invalid\n", len, tmp_len);
		return -EPERM;
	}

	tmp_data = cpu_to_be64(data);
	/* current output current */
	p->output_curr = (tmp_data >> HWUFCS_SOURCE_INFO_SHIFT_OUTPUT_CURR) &
		HWUFCS_SOURCE_INFO_MASK_OUTPUT_CURR;
	p->output_curr *= HWUFCS_SOURCE_INFO_UNIT_CURR;
	/* current output voltage */
	p->output_volt = (tmp_data >> HWUFCS_SOURCE_INFO_SHIFT_OUTPUT_VOLT) &
		HWUFCS_SOURCE_INFO_MASK_OUTPUT_VOLT;
	p->output_volt *= HWUFCS_SOURCE_INFO_UNIT_VOLT;
	/* current usb port temp */
	p->port_temp = (tmp_data >> HWUFCS_SOURCE_INFO_SHIFT_PORT_TEMP) &
		HWUFCS_SOURCE_INFO_MASK_PORT_TEMP;
	p->port_temp -= HWUFCS_SOURCE_INFO_TEMP_BASE;
	/* current device temp */
	p->dev_temp = (tmp_data >> HWUFCS_SOURCE_INFO_SHIFT_DEV_TEMP) &
		HWUFCS_SOURCE_INFO_MASK_DEV_TEMP;
	p->dev_temp -= HWUFCS_SOURCE_INFO_TEMP_BASE;

	hwlog_info("source_info=%lx %umA %umV %dC %dC\n", tmp_data,
		p->output_curr, p->output_volt, p->port_temp, p->dev_temp);
	return 0;
}

#ifdef POWER_MODULE_DEBUG_FUNCTION
/* data message from source to sink: 0x05 cable_information */
static int hwufcs_receive_cable_info_data_msg(struct hwufcs_cable_info_data *p)
{
	u64 data = 0;
	u64 tmp_data;
	u8 len = sizeof(u64);
	u8 tmp_len = 0;
	int ret;

	ret = hwufcs_receive_data_msg(HWUFCS_DATA_MSG_CABLE_INFO,
		(u8 *)&data, len, &tmp_len);
	if (ret)
		return -EPERM;

	if (len != tmp_len) {
		hwlog_err("cable_info length=%u,%u invalid\n", len, tmp_len);
		return -EPERM;
	}

	tmp_data = cpu_to_be64(data);
	/* max loading current */
	p->max_curr = (tmp_data >> HWUFCS_CABLE_INFO_SHIFT_MAX_CURR) &
		HWUFCS_CABLE_INFO_MASK_MAX_CURR;
	p->max_curr *= HWUFCS_CABLE_INFO_UNIT_CURR;
	/* max loading voltage */
	p->max_volt = (tmp_data >> HWUFCS_CABLE_INFO_SHIFT_MAX_VOLT) &
		HWUFCS_CABLE_INFO_MASK_MAX_VOLT;
	p->max_volt *= HWUFCS_CABLE_INFO_UNIT_VOLT;
	/* cable resistance */
	p->resistance = (tmp_data >> HWUFCS_CABLE_INFO_SHIFT_RESISTANCE) &
		HWUFCS_CABLE_INFO_MASK_RESISTANCE;
	p->resistance *= HWUFCS_CABLE_INFO_UNIT_RESISTANCE;
	/* cable electronic lable vendor id */
	p->elable_vid = (tmp_data >> HWUFCS_CABLE_INFO_SHIFT_ELABLE_VID) &
		HWUFCS_CABLE_INFO_MASK_ELABLE_VID;
	/* cable vendor id */
	p->vid = (tmp_data >> HWUFCS_CABLE_INFO_SHIFT_VID) &
		HWUFCS_CABLE_INFO_MASK_VID;

	hwlog_info("cable_info=%lx %xmA %xmV %xmO %x\n", tmp_data,
		p->max_curr, p->max_volt, p->resistance, p->elable_vid, p->vid);
	return 0;
}
#endif /* POWER_MODULE_DEBUG_FUNCTION */

/* data message from source to sink: 0x06 device_information */
static int hwufcs_receive_dev_info_data_msg(struct hwufcs_dev_info_data *p)
{
	u64 data = 0;
	u64 tmp_data;
	u8 len = sizeof(u64);
	u8 tmp_len = 0;
	int ret;

	ret = hwufcs_receive_data_msg(HWUFCS_DATA_MSG_DEVICE_INFO,
		(u8 *)&data, len, &tmp_len);
	if (ret)
		return -EPERM;

	if (len != tmp_len) {
		hwlog_err("dev_info length=%u,%u invalid\n", len, tmp_len);
		return -EPERM;
	}

	tmp_data = cpu_to_be64(data);
	/* software version */
	p->sw_ver = (tmp_data >> HWUFCS_DEV_INFO_SHIFT_SW_VER) &
		HWUFCS_DEV_INFO_MASK_SW_VER;
	/* hardware version */
	p->hw_ver = (tmp_data >> HWUFCS_DEV_INFO_SHIFT_HW_VER) &
		HWUFCS_DEV_INFO_MASK_HW_VER;
	/* protocol chip vendor id */
	p->chip_vid = (tmp_data >> HWUFCS_DEV_INFO_SHIFT_CHIP_VID) &
		HWUFCS_DEV_INFO_MASK_CHIP_VID;
	/* manufacture vendor id */
	p->manu_vid = (tmp_data >> HWUFCS_DEV_INFO_SHIFT_MANU_VID) &
		HWUFCS_DEV_INFO_MASK_MANU_VID;

	hwlog_info("dev_info=%lx %x %x %x %x\n", tmp_data,
		p->sw_ver, p->hw_ver, p->chip_vid, p->manu_vid);
	return 0;
}

#ifdef POWER_MODULE_DEBUG_FUNCTION
/* data message from source to sink: 0x07 error_information */
static int hwufcs_receive_error_info_data_msg(struct hwufcs_error_info_data *p)
{
	u64 data = 0;
	u64 tmp_data;
	u8 len = sizeof(u64);
	u8 tmp_len = 0;
	int ret;

	ret = hwufcs_receive_data_msg(HWUFCS_DATA_MSG_ERROR_INFO,
		(u8 *)&data, len, &tmp_len);
	if (ret)
		return -EPERM;

	if (len != tmp_len) {
		hwlog_err("error_info length=%u,%u invalid\n", len, tmp_len);
		return -EPERM;
	}

	tmp_data = cpu_to_be64(data);
	/* output ovp */
	p->output_ovp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_OUTPUT_OVP) &
		HWUFCS_ERROR_INFO_MASK_OUTPUT_OVP;
	/* output uvp */
	p->output_uvp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_OUTPUT_UVP) &
		HWUFCS_ERROR_INFO_MASK_OUTPUT_UVP;
	/* output ocp */
	p->output_ocp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_OUTPUT_OCP) &
		HWUFCS_ERROR_INFO_MASK_OUTPUT_OCP;
	/* output scp */
	p->output_scp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_OUTPUT_SCP) &
		HWUFCS_ERROR_INFO_MASK_OUTPUT_SCP;
	/* usb otp */
	p->usb_otp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_USB_OTP) &
		HWUFCS_ERROR_INFO_MASK_USB_OTP;
	/* device otp */
	p->device_otp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_DEVICE_OTP) &
		HWUFCS_ERROR_INFO_MASK_DEVICE_OTP;
	/* cc ovp */
	p->cc_ovp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_CC_OVP) &
		HWUFCS_ERROR_INFO_MASK_CC_OVP;
	/* d- ovp */
	p->dminus_ovp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_DMINUS_OVP) &
		HWUFCS_ERROR_INFO_MASK_DMINUS_OVP;
	/* d+ ovp */
	p->dplus_ovp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_DPLUS_OVP) &
		HWUFCS_ERROR_INFO_MASK_DPLUS_OVP;
	/* input ovp */
	p->input_ovp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_INPUT_OVP) &
		HWUFCS_ERROR_INFO_MASK_INPUT_OVP;
	/* input uvp */
	p->input_uvp = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_INPUT_UVP) &
		HWUFCS_ERROR_INFO_MASK_INPUT_UVP;
	/* over leakage current */
	p->over_leakage = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_OVER_LEAKAGE) &
		HWUFCS_ERROR_INFO_MASK_OVER_LEAKAGE;
	/* input drop */
	p->input_drop = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_INPUT_DROP) &
		HWUFCS_ERROR_INFO_MASK_INPUT_DROP;
	/* crc error */
	p->crc_error = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_CRC_ERROR) &
		HWUFCS_ERROR_INFO_MASK_CRC_ERROR;
	/* watchdog overflow */
	p->wtg_overflow = (tmp_data >> HWUFCS_ERROR_INFO_SHIFT_WTG_OVERFLOW) &
		HWUFCS_ERROR_INFO_MASK_WTG_OVERFLOW;

	hwlog_info("error_info=%lx\n", tmp_data);
	return 0;
}
#endif /* POWER_MODULE_DEBUG_FUNCTION */

/* data message from source to sink: 0x09 refuse */
static int hwufcs_receive_refuse_data_msg(struct hwufcs_refuse_data *p)
{
	u32 data = 0;
	u32 tmp_data;
	u8 len = sizeof(u32);
	u8 tmp_len = 0;
	int ret;

	ret = hwufcs_receive_data_msg(HWUFCS_DATA_MSG_REFUSE,
		(u8 *)&data, len, &tmp_len);
	if (ret)
		return -EPERM;

	if (len != tmp_len) {
		hwlog_err("refuse length=%u,%u invalid\n", len, tmp_len);
		return -EPERM;
	}

	tmp_data = cpu_to_be32(data);
	/* reason */
	p->reason = (tmp_data >> HWUFCS_REFUSE_SHIFT_REASON) &
		HWUFCS_REFUSE_MASK_REASON;
	/* command number */
	p->cmd_number = (tmp_data >> HWUFCS_REFUSE_SHIFT_CMD_NUMBER) &
		HWUFCS_REFUSE_MASK_CMD_NUMBER;
	/* message type */
	p->msg_type = (tmp_data >> HWUFCS_REFUSE_SHIFT_MSG_TYPE) &
		HWUFCS_REFUSE_MASK_MSG_TYPE;
	/* message number */
	p->msg_number = (tmp_data >> HWUFCS_REFUSE_SHIFT_MSG_NUMBER) &
		HWUFCS_REFUSE_MASK_MSG_NUMBER;

	hwlog_info("refuse=%lx %x %x %x %x\n", tmp_data,
		p->reason, p->cmd_number, p->msg_type, p->msg_number);
	return 0;
}

#ifdef POWER_MODULE_DEBUG_FUNCTION
/* data message from source to sink: 0x0b verify_response */
static int hwufcs_receive_verify_response_data_msg(struct hwufcs_verify_response_data *p)
{
	u8 len = HWUFCS_VERIFY_RSP_ENCRYPT_SIZE + HWUFCS_VERIFY_RSP_RANDOM_SIZE;
	u8 tmp_len = 0;
	int ret, i;

	ret = hwufcs_receive_data_msg(HWUFCS_DATA_MSG_VERIFY_RESPONSE,
		(u8 *)p, len, &tmp_len);
	if (ret)
		return -EPERM;

	if (len != tmp_len) {
		hwlog_err("verify_response length=%u,%u invalid\n", len, tmp_len);
		return -EPERM;
	}

	for (i = 0; i < HWUFCS_VERIFY_RSP_ENCRYPT_SIZE; i++)
		hwlog_info("encrypt[%u]=%02x\n", i, p->encrypt[i]);
	for (i = 0; i < HWUFCS_VERIFY_RSP_RANDOM_SIZE; i++)
		hwlog_info("random[%u]=%02x\n", i, p->encrypt[i]);
	return 0;
}
#endif /* POWER_MODULE_DEBUG_FUNCTION */

static int hwufcs_get_output_capabilities(void)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();
	int ret;

	if (!l_dev)
		return -EPERM;

	if (l_dev->info.outout_capabilities_rd_flag == HAS_READ_FLAG)
		return 0;

	ret = hwufcs_send_get_output_capabilities_ctrl_msg();
	if (ret)
		return -EPERM;

	l_dev->info.output_mode = 0;
	ret = hwufcs_receive_output_capabilities_data_msg(
		&l_dev->info.cap[0], &l_dev->info.output_mode);
	if (ret)
		return -EPERM;

	l_dev->info.outout_capabilities_rd_flag = HAS_READ_FLAG;

	/* init output_mode & volt & curr */
	l_dev->info.output_mode = 1;
	l_dev->info.output_volt = l_dev->info.cap[0].max_volt;
	l_dev->info.output_curr = l_dev->info.cap[0].max_curr;

	return 0;
}

static int hwufcs_get_source_info(struct hwufcs_source_info_data *p)
{
	int ret;

	/* delay 20ms to avoid send msg densely */
	power_usleep(DT_USLEEP_20MS);

	ret = hwufcs_send_get_source_info_ctrl_msg();
	if (ret)
		return -EPERM;

	ret = hwufcs_receive_source_info_data_msg(p);
	if (ret)
		return -EPERM;

	return 0;
}

static int hwufcs_get_dev_info(void)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();
	int ret;

	if (!l_dev)
		return -EPERM;

	if (l_dev->info.dev_info_rd_flag == HAS_READ_FLAG)
		return 0;

	ret = hwufcs_send_get_dev_info_ctrl_msg();
	if (ret)
		return -EPERM;

	ret = hwufcs_receive_dev_info_data_msg(&l_dev->info.dev_info);
	if (ret)
		return -EPERM;

	l_dev->info.dev_info_rd_flag = HAS_READ_FLAG;

	return 0;
}

static int hwufcs_config_watchdog(struct hwufcs_wtg_data *p)
{
	struct hwufcs_refuse_data refuse;
	int ret;

	ret = hwufcs_send_config_watchdog_data_msg(p);
	if (ret)
		return -EPERM;

	ret = hwufcs_receive_accept_ctrl_msg();
	if (!ret)
		return 0;

	if (ret == -HWUFCS_MSG_NOT_MATCH)
		return hwufcs_receive_refuse_data_msg(&refuse);

	return -EPERM;
}

static int hwufcs_set_default_state(void)
{
	return 0;
}

static int hwufcs_set_default_param(void)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();

	if (!l_dev)
		return -EPERM;

	memset(&l_dev->info, 0, sizeof(l_dev->info));
	return 0;
}

static int hwufcs_detect_adapter_support_mode(int *mode)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();
	int ret;

	if (!l_dev || !mode)
		return ADAPTER_DETECT_FAIL;

	/* set all parameter to default state */
	hwufcs_set_default_param();
	l_dev->info.detect_finish_flag = 1; /* has detect adapter */
	l_dev->info.support_mode = ADAPTER_SUPPORT_UNDEFINED;

	/* protocol handshark: detect ufcs adapter */
	ret = hwufcs_detect_adapter();
	if (ret == HWUFCS_DETECT_FAIL) {
		hwlog_err("ufcs adapter detect fail\n");
		return ADAPTER_DETECT_FAIL;
	}

	ret = hwufcs_send_ping_ctrl_msg();
	if (ret)
		return ADAPTER_DETECT_FAIL;

	hwlog_info("ufcs adapter ping succ\n");
	*mode = ADAPTER_SUPPORT_SC;
	l_dev->info.support_mode = ADAPTER_SUPPORT_SC;
	return ADAPTER_DETECT_SUCC;
}

static int hwufcs_get_support_mode(int *mode)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();

	if (!l_dev || !mode)
		return -EPERM;

	if (l_dev->info.detect_finish_flag)
		*mode = l_dev->info.support_mode;
	else
		hwufcs_detect_adapter_support_mode(mode);

	hwlog_info("support_mode: %d\n", *mode);
	return 0;
}

static int hwufcs_set_init_data(struct adapter_init_data *data)
{
	struct hwufcs_wtg_data wtg;

	wtg.time = data->watchdog_timer * HWUFCS_WTG_UNIT_TIME;
	if (hwufcs_config_watchdog(&wtg))
		return -EPERM;

	hwlog_info("set_init_data\n");
	return 0;
}

static int hwufcs_soft_reset_slave(void)
{
	hwlog_info("soft_reset_slave\n");
	return 0;
}

static int hwufcs_get_inside_temp(int *temp)
{
	struct hwufcs_source_info_data data;

	if (!temp)
		return -EPERM;

	if (hwufcs_get_source_info(&data))
		return -EPERM;

	*temp = data.dev_temp;

	hwlog_info("get_inside_temp: %d\n", *temp);
	return 0;
}

static int hwufcs_get_port_temp(int *temp)
{
	struct hwufcs_source_info_data data;

	if (!temp)
		return -EPERM;

	if (hwufcs_get_source_info(&data))
		return -EPERM;

	*temp = data.port_temp;

	hwlog_info("get_port_temp: %d\n", *temp);
	return 0;
}

static int hwufcs_get_chip_vendor_id(int *id)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();

	if (!l_dev || !id)
		return -EPERM;

	if (hwufcs_get_dev_info())
		return -EPERM;

	*id = l_dev->info.dev_info.chip_vid;

	hwlog_info("get_chip_vendor_id: %d\n", *id);
	return 0;
}

static int hwufcs_get_chip_id(int *id)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();

	if (!l_dev || !id)
		return -EPERM;

	if (hwufcs_get_dev_info())
		return -EPERM;

	*id = l_dev->info.dev_info.manu_vid;

	hwlog_info("get_chip_id_f: 0x%x\n", *id);
	return 0;
}

static int hwufcs_get_hw_version_id(int *id)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();

	if (!l_dev || !id)
		return -EPERM;

	if (hwufcs_get_dev_info())
		return -EPERM;

	*id = l_dev->info.dev_info.hw_ver;

	hwlog_info("get_hw_version_id_f: 0x%x\n", *id);
	return 0;
}

static int hwufcs_get_sw_version_id(int *id)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();

	if (!l_dev || !id)
		return -EPERM;

	if (hwufcs_get_dev_info())
		return -EPERM;

	*id = l_dev->info.dev_info.sw_ver;

	hwlog_info("get_sw_version_id_f: 0x%x\n", *id);
	return 0;
}

static int hwufcs_get_adp_type(int *type)
{
	if (!type)
		return -EPERM;

	*type = ADAPTER_TYPE_UNKNOWN;
	return 0;
}

static int hwufcs_check_output_mode(u8 mode)
{
	if ((mode < HWUFCS_REQ_MIN_OUTPUT_MODE) ||
		(mode > HWUFCS_REQ_MAX_OUTPUT_MODE)) {
		hwlog_err("output_mode=%u invalid\n", mode);
		return -EPERM;
	}

	return 0;
}

static int hwufcs_set_output_voltage(int volt)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();
	struct hwufcs_request_data req;
	struct hwufcs_refuse_data refuse;
	u8 cap_index;
	int ret, i;

	if (!l_dev)
		return -EPERM;

	ret = hwufcs_check_output_mode(l_dev->info.output_mode);
	if (ret)
		return -EPERM;

	/* save current output voltage */
	l_dev->info.output_volt = volt;

	cap_index = l_dev->info.output_mode - HWUFCS_REQ_BASE_OUTPUT_MODE;
	req.output_mode = l_dev->info.output_mode;
	req.output_curr = l_dev->info.output_curr;
	req.output_volt = volt;

	/* delay 20ms to avoid send msg densely */
	power_usleep(DT_USLEEP_20MS);

	ret = hwufcs_send_request_data_msg(&req);
	if (ret)
		return -EPERM;

	ret = hwufcs_receive_accept_ctrl_msg();
	if (!ret) {
		for (i = 0; i < HWUFCS_POWER_READY_RETRY; i++) {
			ret = hwufcs_receive_power_ready_ctrl_msg();
			if (!ret) {
				hwlog_info("set_output_voltage: %d\n", volt);
				break;
			}
		}
		return ret;
	}

	if (ret == -HWUFCS_MSG_NOT_MATCH)
		return hwufcs_receive_refuse_data_msg(&refuse);

	return -EPERM;
}

static int hwufcs_set_output_current(int cur)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();
	struct hwufcs_request_data req;
	struct hwufcs_refuse_data refuse;
	u8 cap_index;
	int ret, i;

	if (!l_dev)
		return -EPERM;

	ret = hwufcs_check_output_mode(l_dev->info.output_mode);
	if (ret)
		return -EPERM;

	/* save current output current */
	l_dev->info.output_curr = cur;

	cap_index = l_dev->info.output_mode - HWUFCS_REQ_BASE_OUTPUT_MODE;
	req.output_mode = l_dev->info.output_mode;
	req.output_curr = cur;
	req.output_volt = l_dev->info.output_volt;

	/* delay 20ms to avoid send msg densely */
	power_usleep(DT_USLEEP_20MS);

	ret = hwufcs_send_request_data_msg(&req);
	if (ret)
		return -EPERM;

	ret = hwufcs_receive_accept_ctrl_msg();
	if (!ret) {
		for (i = 0; i < HWUFCS_POWER_READY_RETRY; i++) {
			ret = hwufcs_receive_power_ready_ctrl_msg();
			if (!ret) {
				hwlog_info("set_output_current: %d\n", cur);
				break;
			}
		}
		return ret;
	}

	if (ret == -HWUFCS_MSG_NOT_MATCH)
		return hwufcs_receive_refuse_data_msg(&refuse);

	return -EPERM;
}

static int hwufcs_get_output_voltage(int *volt)
{
	struct hwufcs_source_info_data data;

	if (!volt)
		return -EPERM;

	if (hwufcs_get_source_info(&data))
		return -EPERM;

	*volt = data.output_volt;

	hwlog_info("get_output_voltage: %d\n", *volt);
	return 0;
}

static int hwufcs_get_output_current(int *cur)
{
	struct hwufcs_source_info_data data;

	if (!cur)
		return -EPERM;

	if (hwufcs_get_source_info(&data))
		return -EPERM;

	*cur = data.output_curr;

	hwlog_info("get_output_current: %d\n", *cur);
	return 0;
}

static int hwufcs_get_output_current_set(int *cur)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();

	if (!l_dev || !cur)
		return -EPERM;

	*cur = l_dev->info.output_curr;

	hwlog_info("get_output_current_set: %d\n", *cur);
	return 0;
}

static int hwufcs_get_min_voltage(int *volt)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();
	u8 cap_index;

	if (!l_dev || !volt)
		return -EPERM;

	if (hwufcs_get_output_capabilities())
		return -EPERM;

	if (hwufcs_check_output_mode(l_dev->info.output_mode))
		return -EPERM;

	cap_index = l_dev->info.output_mode - HWUFCS_REQ_BASE_OUTPUT_MODE;
	*volt = l_dev->info.cap[cap_index].min_volt;

	hwlog_info("get_min_voltage: %u,%d\n", cap_index, *volt);
	return 0;
}

static int hwufcs_get_max_voltage(int *volt)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();
	u8 cap_index;

	if (!l_dev || !volt)
		return -EPERM;

	if (hwufcs_get_output_capabilities())
		return -EPERM;

	if (hwufcs_check_output_mode(l_dev->info.output_mode))
		return -EPERM;

	cap_index = l_dev->info.output_mode - HWUFCS_REQ_BASE_OUTPUT_MODE;
	*volt = l_dev->info.cap[cap_index].max_volt;

	hwlog_info("get_max_voltage: %u,%d\n", cap_index, *volt);
	return 0;
}

static int hwufcs_get_min_current(int *cur)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();
	u8 cap_index;

	if (!l_dev || !cur)
		return -EPERM;

	if (hwufcs_get_output_capabilities())
		return -EPERM;

	if (hwufcs_check_output_mode(l_dev->info.output_mode))
		return -EPERM;

	cap_index = l_dev->info.output_mode - HWUFCS_REQ_BASE_OUTPUT_MODE;
	*cur = l_dev->info.cap[cap_index].min_curr;

	hwlog_info("get_min_current: %u,%d\n", cap_index, *cur);
	return 0;
}

static int hwufcs_get_max_current(int *cur)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();
	u8 cap_index;

	if (!l_dev || !cur)
		return -EPERM;

	if (hwufcs_get_output_capabilities())
		return -EPERM;

	if (hwufcs_check_output_mode(l_dev->info.output_mode))
		return -EPERM;

	cap_index = l_dev->info.output_mode - HWUFCS_REQ_BASE_OUTPUT_MODE;
	*cur = l_dev->info.cap[cap_index].max_curr;

	hwlog_info("get_max_current: %u,%d\n", cap_index, *cur);
	return 0;
}

static int hwufcs_get_power_drop_current(int *cur)
{
	return 0;
}

static int hwufcs_auth_encrypt_start(int key)
{
	hwlog_info("auth_encrypt_start\n");
	return 0;
}

static int hwufcs_get_device_info(struct adapter_device_info *info)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();

	if (!l_dev || !info)
		return -EPERM;

	if (hwufcs_get_chip_id(&info->chip_id))
		return -EPERM;

	if (hwufcs_get_hw_version_id(&info->hwver))
		return -EPERM;

	if (hwufcs_get_sw_version_id(&info->fwver))
		return -EPERM;

	if (hwufcs_get_min_voltage(&info->min_volt))
		return -EPERM;

	if (hwufcs_get_max_voltage(&info->max_volt))
		return -EPERM;

	if (hwufcs_get_min_current(&info->min_cur))
		return -EPERM;

	if (hwufcs_get_max_current(&info->max_cur))
		return -EPERM;

	info->volt_step = l_dev->info.cap[0].volt_step;
	info->curr_step = l_dev->info.cap[0].curr_step;
	info->output_mode = l_dev->info.cap[0].output_mode;

	hwlog_info("get_device_info\n");
	return 0;
}

static int hwufcs_get_protocol_register_state(void)
{
	struct hwufcs_dev *l_dev = hwufcs_get_dev();

	if (!l_dev)
		return -EPERM;

	if ((l_dev->dev_id >= PROTOCOL_DEVICE_ID_BEGIN) &&
		(l_dev->dev_id < PROTOCOL_DEVICE_ID_END))
		return 0;

	hwlog_info("get_protocol_register_state fail\n");
	return -EPERM;
}

static struct adapter_protocol_ops adapter_protocol_hwufcs_ops = {
	.type_name = "hw_ufcs",
	.detect_adapter_support_mode = hwufcs_detect_adapter_support_mode,
	.get_support_mode = hwufcs_get_support_mode,
	.set_default_state = hwufcs_set_default_state,
	.set_default_param = hwufcs_set_default_param,
	.set_init_data = hwufcs_set_init_data,
	.soft_reset_master = hwufcs_soft_reset_master,
	.soft_reset_slave = hwufcs_soft_reset_slave,
	.get_chip_vendor_id = hwufcs_get_chip_vendor_id,
	.get_adp_type = hwufcs_get_adp_type,
	.get_inside_temp = hwufcs_get_inside_temp,
	.get_port_temp = hwufcs_get_port_temp,
	.set_output_voltage = hwufcs_set_output_voltage,
	.get_output_voltage = hwufcs_get_output_voltage,
	.set_output_current = hwufcs_set_output_current,
	.get_output_current = hwufcs_get_output_current,
	.get_output_current_set = hwufcs_get_output_current_set,
	.get_min_voltage = hwufcs_get_min_voltage,
	.get_max_voltage = hwufcs_get_max_voltage,
	.get_min_current = hwufcs_get_min_current,
	.get_max_current = hwufcs_get_max_current,
	.get_power_drop_current = hwufcs_get_power_drop_current,
	.auth_encrypt_start = hwufcs_auth_encrypt_start,
	.get_device_info = hwufcs_get_device_info,
	.get_protocol_register_state = hwufcs_get_protocol_register_state,
};

static int __init hwufcs_init(void)
{
	int ret;
	struct hwufcs_dev *l_dev = NULL;

	l_dev = kzalloc(sizeof(*l_dev), GFP_KERNEL);
	if (!l_dev)
		return -ENOMEM;

	g_hwufcs_dev = l_dev;
	l_dev->dev_id = PROTOCOL_DEVICE_ID_END;

	ret = adapter_protocol_ops_register(&adapter_protocol_hwufcs_ops);
	if (ret)
		goto fail_register_ops;

	mutex_init(&l_dev->msg_number_lock);
	return 0;

fail_register_ops:
	kfree(l_dev);
	g_hwufcs_dev = NULL;
	return ret;
}

static void __exit hwufcs_exit(void)
{
	if (!g_hwufcs_dev)
		return;

	mutex_destroy(&g_hwufcs_dev->msg_number_lock);
	kfree(g_hwufcs_dev);
	g_hwufcs_dev = NULL;
}

subsys_initcall_sync(hwufcs_init);
module_exit(hwufcs_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("ufcs protocol driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
