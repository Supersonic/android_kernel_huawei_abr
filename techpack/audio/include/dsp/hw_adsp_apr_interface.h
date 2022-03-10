/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 */
#ifndef __HW_ADSP_APR_INTERFACE_H__
#define __HW_ADSP_APR_INTERFACE_H__

#include <dsp/apr_audio-v2.h>
#include <linux/types.h>

struct hw_q6afe_ops {
	int (*q6afe_set_params_v3)(u16 port_id, int index,
			struct mem_mapping_hdr *mem_hdr,
			u8 *packed_param_data,
			u32 packed_data_size);
	int (*q6afe_set_params_v2)(u16 port_id, int index,
			struct mem_mapping_hdr *mem_hdr,
			u8 *packed_param_data,
			u32 packed_data_size);

	int (*q6afe_pack_and_set_param_in_band)(u16 port_id, int index,
			struct param_hdr_v3 param_hdr,
			u8 *param_data);

	int (*q6afe_get_params_v3)(u16 port_id, int index,
			struct mem_mapping_hdr *mem_hdr,
			struct param_hdr_v3 *param_hdr);

	int (*q6afe_get_params_v2)(u16 port_id, int index,
			struct mem_mapping_hdr *mem_hdr,
			struct param_hdr_v3 *param_hdr);
};

struct hw_q6afe_interface_dev {
	atomic_t *state;
	atomic_t *status;
	wait_queue_head_t *wait;
	struct hw_q6afe_ops ops;
};

#ifdef CONFIG_HUAWEI_QCOM_ADSP_MISC
int hw_register_q6afe_ops(struct hw_q6afe_interface_dev *dev);
bool hw_smartpa_make_afe_callback(uint32_t token, uint32_t *payload,
		uint32_t payload_size);
void refresh_smartpa_algo_status(int port_id);
bool hw_get_smartpa_num(unsigned int *pa_num);
bool hw_get_smartpa_rx_port_id(unsigned int *port_id);
bool hw_get_smartpa_tx_port_id(unsigned int *port_id);
#else
static inline int hw_register_q6afe_ops(struct hw_q6afe_interface_dev *dev)
{
	return 0;
}

static inline bool hw_smartpa_make_afe_callback(uint32_t token,
	uint32_t *payload, uint32_t payload_size)
{
	return false;
}

static inline void refresh_smartpa_algo_status(int port_id)
{
}

static inline bool hw_get_smartpa_num(unsigned int *pa_num)
{
	return false;
}

static inline bool hw_get_smartpa_rx_port_id(unsigned int *port_id)
{
	return false;
}

static inline bool hw_get_smartpa_tx_port_id(unsigned int *port_id)
{
	return false;
}
#endif

#endif /* __HW_ADSP_APR_INTERFACE_H__ */
