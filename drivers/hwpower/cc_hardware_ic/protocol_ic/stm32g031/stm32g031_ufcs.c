// SPDX-License-Identifier: GPL-2.0
/*
 * stm32g031_ufcs.c
 *
 * stm32g031 ufcs driver
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

#include "stm32g031_ufcs.h"
#include "stm32g031_i2c.h"
#include "stm32g031_fw.h"
#include <chipset_common/hwpower/common_module/power_delay.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/protocol/adapter_protocol.h>
#include <chipset_common/hwpower/protocol/adapter_protocol_ufcs.h>

#define HWLOG_TAG stm32g031
HWLOG_REGIST();

static int stm32g031_ufcs_wait(struct stm32g031_device_info *di, u8 flag)
{
	u8 reg_val = 0;
	int ret, i;

	for (i = 0; i < STM32G031_UFCS_WAIT_RETRY_CYCLE; i++) {
		power_usleep(DT_USLEEP_1MS);
		ret = stm32g031_read_byte(di, STM32G031_UFCS_ISR1_REG, &reg_val);
		if (ret) {
			hwlog_err("read isr reg[%x] fail\n", STM32G031_UFCS_ISR1_REG);
			continue;
		}

		if (reg_val == 0)
			continue;

		/* isr1[3] must be 0 */
		if ((flag & STM32G031_UFCS_ISR1_CRC_ERROR_MASK) &&
			(reg_val & STM32G031_UFCS_ISR1_CRC_ERROR_MASK)) {
			hwlog_err("crc error\n");
			continue;
		}

		/* isr1[2] must be 1 */
		if ((flag & STM32G031_UFCS_ISR1_SEND_PACKET_COMPLETE_MASK) &&
			!(reg_val & STM32G031_UFCS_ISR1_SEND_PACKET_COMPLETE_MASK)) {
			hwlog_err("sent packet not complete or fail\n");
			continue;
		}

		/* isr1[1] must be 1 */
		if ((flag & STM32G031_UFCS_ISR1_DATA_READY_MASK) &&
			!(reg_val & STM32G031_UFCS_ISR1_DATA_READY_MASK)) {
			hwlog_err("receive data not ready\n");
			continue;
		}

		hwlog_info("wait succ\n");
		return 0;
	}

	hwlog_err("wait fail\n");
	return -EINVAL;
}

static int stm32g031_ufcs_detect_adapter(void *dev_data)
{
	struct stm32g031_device_info *di = dev_data;
	u8 reg_val = 0;
	int ret, i;

	if (!di) {
		hwlog_err("di is null\n");
		return HWUFCS_DETECT_OTHER;
	}

	mutex_lock(&di->ufcs_detect_lock);

	ret = stm32g031_fw_set_hw_config(di);
	ret += stm32g031_fw_get_hw_config(di);
	if (ret) {
		hwlog_err("set hw config fail\n");
		mutex_unlock(&di->ufcs_detect_lock);
		return HWUFCS_DETECT_FAIL;
	}

	ret = stm32g031_write_mask(di, STM32G031_UFCS_CTL1_REG,
		STM32G031_UFCS_CTL1_EN_UFCS_HANDSHAKE_MASK,
		STM32G031_UFCS_CTL1_EN_UFCS_HANDSHAKE_SHIFT, 1);
	(void)power_usleep(DT_USLEEP_20MS);
	/* waiting for handshake */
	for (i = 0; i < STM32G031_UFCS_HANDSHARK_RETRY_CYCLE; i++) {
		(void)power_usleep(DT_USLEEP_2MS);
		ret = stm32g031_read_byte(di, STM32G031_UFCS_ISR1_REG, &reg_val);
		if (ret) {
			hwlog_err("read isr reg[%x] fail\n", STM32G031_UFCS_ISR1_REG);
			continue;
		}

		if (reg_val & STM32G031_UFCS_ISR1_HANDSHAKE_FLAG_MASK)
			break;
	}

	mutex_unlock(&di->ufcs_detect_lock);

	if (i == STM32G031_UFCS_HANDSHARK_RETRY_CYCLE) {
		hwlog_err("handshake fail\n");
		return HWUFCS_DETECT_FAIL;
	}

	hwlog_info("handshake succ\n");
	return HWUFCS_DETECT_SUCC;
}

static int stm32g031_ufcs_send_msg_head(void *dev_data, u8 *data, u8 len)
{
	struct stm32g031_device_info *di = dev_data;

	if (!di || !data) {
		hwlog_err("di or data is null\n");
		return -ENODEV;
	}

	if (len != sizeof(struct hwufcs_header_data)) {
		hwlog_err("invalid length=%u\n", len);
		return -EINVAL;
	}

	return stm32g031_write_block(di, STM32G031_UFCS_TX_HEADER_H_REG,
		data, len);
}

static int stm32g031_ufcs_send_msg_body(void *dev_data, u8 *data, u8 len)
{
	struct stm32g031_device_info *di = dev_data;

	if (!di || !data) {
		hwlog_err("di or data is null\n");
		return -ENODEV;
	}

	if (len > STM32G031_UFCS_TX_BUFFER_SIZE) {
		hwlog_err("invalid length=%u\n", len);
		return -EINVAL;
	}

	return stm32g031_write_block(di, STM32G031_UFCS_TX_BUFFER_0_REG,
		data, len);
}

static int stm32g031_ufcs_end_send_msg(void *dev_data, u8 flag)
{
	struct stm32g031_device_info *di = dev_data;
	u8 tmp_flag = 0;
	int ret;

	if (!di) {
		hwlog_err("di is null\n");
		return -ENODEV;
	}

	if (flag & HWUFCS_WAIT_CRC_ERROR)
		tmp_flag |= STM32G031_UFCS_ISR1_CRC_ERROR_MASK;
	if (flag & HWUFCS_WAIT_SEND_PACKET_COMPLETE)
		tmp_flag |= STM32G031_UFCS_ISR1_SEND_PACKET_COMPLETE_MASK;
	if (flag & HWUFCS_WAIT_DATA_READY)
		tmp_flag |= STM32G031_UFCS_ISR1_DATA_READY_MASK;

	ret = stm32g031_write_byte(di, STM32G031_UFCS_ISR1_REG, 0);
	ret += stm32g031_write_mask(di, STM32G031_UFCS_CTL1_REG,
		STM32G031_UFCS_CTL1_SEND_MASK, STM32G031_UFCS_CTL1_SEND_SHIFT, 1);
	ret += stm32g031_ufcs_wait(di, tmp_flag);
	return ret;
}

static int stm32g031_ufcs_wait_msg_ready(void *dev_data, u8 flag)
{
	struct stm32g031_device_info *di = dev_data;
	u8 tmp_flag = 0;

	if (!di) {
		hwlog_err("di is null\n");
		return -ENODEV;
	}

	if (flag & HWUFCS_WAIT_CRC_ERROR)
		tmp_flag |= STM32G031_UFCS_ISR1_CRC_ERROR_MASK;
	if (flag & HWUFCS_WAIT_SEND_PACKET_COMPLETE)
		tmp_flag |= STM32G031_UFCS_ISR1_SEND_PACKET_COMPLETE_MASK;
	if (flag & HWUFCS_WAIT_DATA_READY)
		tmp_flag |= STM32G031_UFCS_ISR1_DATA_READY_MASK;

	return stm32g031_ufcs_wait(di, tmp_flag);
}

static int stm32g031_ufcs_receive_msg_head(void *dev_data, u8 *data, u8 len)
{
	struct stm32g031_device_info *di = dev_data;
	u8 tmp_len = sizeof(struct hwufcs_header_data);

	if (!di || !data) {
		hwlog_err("di or data is null\n");
		return -ENODEV;
	}

	if (len != tmp_len) {
		hwlog_err("invalid length=%u,%u\n", len, tmp_len);
		return -EINVAL;
	}

	return stm32g031_read_block(di, STM32G031_UFCS_RX_HEADER_H_REG,
		data, len);
}

static int stm32g031_ufcs_receive_msg_body(void *dev_data, u8 *data, u8 len)
{
	struct stm32g031_device_info *di = dev_data;
	u8 tmp_len = STM32G031_UFCS_RX_BUFFER_SIZE;

	if (!di || !data) {
		hwlog_err("di or data is null\n");
		return -ENODEV;
	}

	if (len > tmp_len) {
		hwlog_err("invalid length=%u,%u\n", len, tmp_len);
		return -EINVAL;
	}

	return stm32g031_read_block(di, STM32G031_UFCS_RX_BUFFER_0_REG,
		data, len);
}

static int stm32g031_ufcs_end_receive_msg(void *dev_data)
{
	struct stm32g031_device_info *di = dev_data;

	if (!di) {
		hwlog_err("di is null\n");
		return -ENODEV;
	}

	return stm32g031_write_byte(di, STM32G031_UFCS_ISR1_REG, 0);
}

static int stm32g031_ufcs_soft_reset_master(void *dev_data)
{
	struct stm32g031_device_info *di = dev_data;

	if (!di) {
		hwlog_err("di is null\n");
		return -ENODEV;
	}

	return stm32g031_write_mask(di, STM32G031_UFCS_CTL1_REG,
		STM32G031_UFCS_CTL1_SOFT_RESET_MASK,
		STM32G031_UFCS_CTL1_SOFT_RESET_SHIFT, 1);
}

static struct hwufcs_ops stm32g031_hwufcs_ops = {
	.chip_name = "stm32g031",
	.detect_adapter = stm32g031_ufcs_detect_adapter,
	.send_msg_header = stm32g031_ufcs_send_msg_head,
	.send_msg_body = stm32g031_ufcs_send_msg_body,
	.end_send_msg = stm32g031_ufcs_end_send_msg,
	.wait_msg_ready = stm32g031_ufcs_wait_msg_ready,
	.receive_msg_header = stm32g031_ufcs_receive_msg_head,
	.receive_msg_body = stm32g031_ufcs_receive_msg_body,
	.end_receive_msg = stm32g031_ufcs_end_receive_msg,
	.soft_reset_master = stm32g031_ufcs_soft_reset_master,
};

int stm32g031_hwufcs_register(struct stm32g031_device_info *di)
{
	stm32g031_hwufcs_ops.dev_data = (void *)di;
	return hwufcs_ops_register(&stm32g031_hwufcs_ops);
}
