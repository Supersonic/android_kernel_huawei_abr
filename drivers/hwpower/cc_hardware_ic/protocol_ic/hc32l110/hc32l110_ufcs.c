// SPDX-License-Identifier: GPL-2.0
/*
 * hc32l110_ufcs.c
 *
 * hc32l110 ufcs driver
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

#include "hc32l110_ufcs.h"
#include "hc32l110_i2c.h"
#include <chipset_common/hwpower/common_module/power_delay.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/protocol/adapter_protocol.h>
#include <chipset_common/hwpower/protocol/adapter_protocol_ufcs.h>

#define HWLOG_TAG hc32l110
HWLOG_REGIST();

static int hc32l110_ufcs_wait(struct hc32l110_device_info *di, u8 flag)
{
	u8 reg_val = 0;
	int ret, i;

	for (i = 0; i < HC32L110_UFCS_WAIT_RETRY_CYCLE; i++) {
		power_usleep(DT_USLEEP_1MS);
		ret = hc32l110_read_byte(di, HC32L110_UFCS_ISR1_REG, &reg_val);
		if (ret) {
			hwlog_err("read isr reg[%x] fail\n", HC32L110_UFCS_ISR1_REG);
			continue;
		}

		if (reg_val == 0)
			continue;

		/* isr1[3] must be 0 */
		if ((flag & HC32L110_UFCS_ISR1_CRC_ERROR_MASK) &&
			(reg_val & HC32L110_UFCS_ISR1_CRC_ERROR_MASK)) {
			hwlog_err("crc error\n");
			continue;
		}

		/* isr1[2] must be 1 */
		if ((flag & HC32L110_UFCS_ISR1_SEND_PACKET_COMPLETE_MASK) &&
			!(reg_val & HC32L110_UFCS_ISR1_SEND_PACKET_COMPLETE_MASK)) {
			hwlog_err("sent packet not complete or fail\n");
			continue;
		}

		/* isr1[1] must be 1 */
		if ((flag & HC32L110_UFCS_ISR1_DATA_READY_MASK) &&
			!(reg_val & HC32L110_UFCS_ISR1_DATA_READY_MASK)) {
			hwlog_err("receive data not ready\n");
			continue;
		}

		hwlog_info("wait succ\n");
		return 0;
	}

	hwlog_err("wait fail\n");
	return -EINVAL;
}

static int hc32l110_ufcs_detect_adapter(void *dev_data)
{
	struct hc32l110_device_info *di = dev_data;
	u8 reg_val = 0;
	int ret, i;

	if (!di) {
		hwlog_err("di is null\n");
		return HWUFCS_DETECT_OTHER;
	}

	mutex_lock(&di->ufcs_detect_lock);

	ret = hc32l110_write_mask(di, HC32L110_UFCS_CTL1_REG,
		HC32L110_UFCS_CTL1_EN_UFCS_HANDSHAKE_MASK,
		HC32L110_UFCS_CTL1_EN_UFCS_HANDSHAKE_SHIFT, 1);
	(void)power_usleep(DT_USLEEP_20MS);
	/* waiting for handshake */
	for (i = 0; i < HC32L110_UFCS_HANDSHARK_RETRY_CYCLE; i++) {
		(void)power_usleep(DT_USLEEP_2MS);
		ret = hc32l110_read_byte(di, HC32L110_UFCS_ISR1_REG, &reg_val);
		if (ret) {
			hwlog_err("read isr reg[%x] fail\n", HC32L110_UFCS_ISR1_REG);
			continue;
		}

		if (reg_val & HC32L110_UFCS_ISR1_HANDSHAKE_FLAG_MASK)
			break;
	}

	mutex_unlock(&di->ufcs_detect_lock);

	if (i == HC32L110_UFCS_HANDSHARK_RETRY_CYCLE) {
		hwlog_err("handshake fail\n");
		return HWUFCS_DETECT_FAIL;
	}

	hwlog_info("handshake succ\n");
	return HWUFCS_DETECT_SUCC;
}

static int hc32l110_ufcs_send_msg_head(void *dev_data, u8 *data, u8 len)
{
	struct hc32l110_device_info *di = dev_data;

	if (!di || !data) {
		hwlog_err("di or data is null\n");
		return -ENODEV;
	}

	if (len != sizeof(struct hwufcs_header_data)) {
		hwlog_err("invalid length=%u\n", len);
		return -EINVAL;
	}

	return hc32l110_write_block(di, HC32L110_UFCS_TX_HEADER_H_REG,
		data, len);
}

static int hc32l110_ufcs_send_msg_body(void *dev_data, u8 *data, u8 len)
{
	struct hc32l110_device_info *di = dev_data;

	if (!di || !data) {
		hwlog_err("di or data is null\n");
		return -ENODEV;
	}

	if (len > HC32L110_UFCS_TX_BUFFER_SIZE) {
		hwlog_err("invalid length=%u\n", len);
		return -EINVAL;
	}

	return hc32l110_write_block(di, HC32L110_UFCS_TX_BUFFER_0_REG,
		data, len);
}

static int hc32l110_ufcs_end_send_msg(void *dev_data, u8 flag)
{
	struct hc32l110_device_info *di = dev_data;
	u8 tmp_flag = 0;
	int ret;

	if (!di) {
		hwlog_err("di is null\n");
		return -ENODEV;
	}

	if (flag & HWUFCS_WAIT_CRC_ERROR)
		tmp_flag |= HC32L110_UFCS_ISR1_CRC_ERROR_MASK;
	if (flag & HWUFCS_WAIT_SEND_PACKET_COMPLETE)
		tmp_flag |= HC32L110_UFCS_ISR1_SEND_PACKET_COMPLETE_MASK;
	if (flag & HWUFCS_WAIT_DATA_READY)
		tmp_flag |= HC32L110_UFCS_ISR1_DATA_READY_MASK;

	ret = hc32l110_write_byte(di, HC32L110_UFCS_ISR1_REG, 0);
	ret += hc32l110_write_mask(di, HC32L110_UFCS_CTL1_REG,
		HC32L110_UFCS_CTL1_SEND_MASK, HC32L110_UFCS_CTL1_SEND_SHIFT, 1);
	ret += hc32l110_ufcs_wait(di, tmp_flag);
	return ret;
}

static int hc32l110_ufcs_wait_msg_ready(void *dev_data, u8 flag)
{
	struct hc32l110_device_info *di = dev_data;
	u8 tmp_flag = 0;

	if (!di) {
		hwlog_err("di is null\n");
		return -ENODEV;
	}

	if (flag & HWUFCS_WAIT_CRC_ERROR)
		tmp_flag |= HC32L110_UFCS_ISR1_CRC_ERROR_MASK;
	if (flag & HWUFCS_WAIT_SEND_PACKET_COMPLETE)
		tmp_flag |= HC32L110_UFCS_ISR1_SEND_PACKET_COMPLETE_MASK;
	if (flag & HWUFCS_WAIT_DATA_READY)
		tmp_flag |= HC32L110_UFCS_ISR1_DATA_READY_MASK;

	return hc32l110_ufcs_wait(di, tmp_flag);
}

static int hc32l110_ufcs_receive_msg_head(void *dev_data, u8 *data, u8 len)
{
	struct hc32l110_device_info *di = dev_data;
	u8 tmp_len = sizeof(struct hwufcs_header_data);

	if (!di || !data) {
		hwlog_err("di or data is null\n");
		return -ENODEV;
	}

	if (len != tmp_len) {
		hwlog_err("invalid length=%u,%u\n", len, tmp_len);
		return -EINVAL;
	}

	return hc32l110_read_block(di, HC32L110_UFCS_RX_HEADER_H_REG,
		data, len);
}

static int hc32l110_ufcs_receive_msg_body(void *dev_data, u8 *data, u8 len)
{
	struct hc32l110_device_info *di = dev_data;
	u8 tmp_len = HC32L110_UFCS_RX_BUFFER_SIZE;

	if (!di || !data) {
		hwlog_err("di or data is null\n");
		return -ENODEV;
	}

	if (len > tmp_len) {
		hwlog_err("invalid length=%u,%u\n", len, tmp_len);
		return -EINVAL;
	}

	return hc32l110_read_block(di, HC32L110_UFCS_RX_BUFFER_0_REG,
		data, len);
}

static int hc32l110_ufcs_end_receive_msg(void *dev_data)
{
	struct hc32l110_device_info *di = dev_data;

	if (!di) {
		hwlog_err("di is null\n");
		return -ENODEV;
	}

	return hc32l110_write_byte(di, HC32L110_UFCS_ISR1_REG, 0);
}

static int hc32l110_ufcs_soft_reset_master(void *dev_data)
{
	struct hc32l110_device_info *di = dev_data;

	if (!di) {
		hwlog_err("di is null\n");
		return -ENODEV;
	}

	return hc32l110_write_mask(di, HC32L110_UFCS_CTL1_REG,
		HC32L110_UFCS_CTL1_SOFT_RESET_MASK,
		HC32L110_UFCS_CTL1_SOFT_RESET_SHIFT, 1);
}

static struct hwufcs_ops hc32l110_hwufcs_ops = {
	.chip_name = "hc32l110",
	.detect_adapter = hc32l110_ufcs_detect_adapter,
	.send_msg_header = hc32l110_ufcs_send_msg_head,
	.send_msg_body = hc32l110_ufcs_send_msg_body,
	.end_send_msg = hc32l110_ufcs_end_send_msg,
	.wait_msg_ready = hc32l110_ufcs_wait_msg_ready,
	.receive_msg_header = hc32l110_ufcs_receive_msg_head,
	.receive_msg_body = hc32l110_ufcs_receive_msg_body,
	.end_receive_msg = hc32l110_ufcs_end_receive_msg,
	.soft_reset_master = hc32l110_ufcs_soft_reset_master,
};

int hc32l110_hwufcs_register(struct hc32l110_device_info *di)
{
	hc32l110_hwufcs_ops.dev_data = (void *)di;
	return hwufcs_ops_register(&hc32l110_hwufcs_ops);
}
