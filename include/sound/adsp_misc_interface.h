/*
 * adsp_misc_interface.h
 *
 * adsp misc interface header
 *
 * Copyright (c) 2019-2019 Huawei Technologies Co., Ltd.
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

#ifndef __ADSP_MISC_INTERFACE_DEFS_H__
#define __ADSP_MISC_INTERFACE_DEFS_H__

struct smartpa_common_param {
	uint32_t module_id;
	uint32_t param_id;
	uint32_t port_id;
};

struct smartpa_afe_interface {
	int (*afe_tisa_get_set)(u8 *user_data, uint32_t param_id,
		uint8_t get_set, uint32_t length, uint32_t module_id);
	int (*send_smartpakit_cal_apr)(struct smartpa_common_param common_param,
		void *buf, size_t buf_size, unsigned int write);
	int (*send_cal_in_band)(struct smartpa_common_param common_param,
		int dst_port, void *cmd, size_t cmd_size);
	void (*cal_unmap_memory)(void);
};

struct smartpa_misc_interface {
	void (*refresh_smartpa_algo_status)(int port_id);
	bool (*get_smartpa_num)(unsigned int *pa_num);
	bool (*get_smartpa_rx_port_id)(unsigned int *port_id);
	bool (*get_smartpa_tx_port_id)(unsigned int *port_id);
};

#ifdef CONFIG_HUAWEI_QCOM_ADSP_MISC
int register_adsp_intf(struct smartpa_afe_interface *intf);
int register_smartpa_misc_intf(struct smartpa_misc_interface **intf);
#else
static inline int register_adsp_intf(struct smartpa_afe_interface *intf)
{
	return 0;
}

static inline int register_smartpa_misc_intf(struct smartpa_misc_interface **intf)
{
	return 0;
}
#endif
#endif // __ADSP_MISC_INTERFACE_DEFS_H__

