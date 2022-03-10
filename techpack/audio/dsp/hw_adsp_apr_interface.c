// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
//#include <linux/ion_kernel.h>
#include <linux/msm_ion.h>
#include <linux/mutex.h>
#include <dsp/msm_audio_ion.h>
#include <dsp/audio_cal_utils.h>
#include <dsp/q6afe-v2.h>
#include <dsp/q6audio-v2.h>
#include <dsp/q6common.h>
#include <dsp/q6core.h>
#include <sound/adsp_misc_interface.h>
#include <dsp/hw_adsp_apr_interface.h>

struct hw_afe_ctl {
	struct rtac_cal_block_data hw_smartpa_cal;
	atomic_t hw_smartpa_state;
	struct hw_q6afe_interface_dev *q6afe_dev;
	struct smartpa_misc_interface *misc_intf;
	struct mutex apr_ops_lock;
};

static struct hw_afe_ctl g_hw_afe;

static bool check_q6afe_ops_status(void)
{
	if (IS_ERR_OR_NULL(g_hw_afe.q6afe_dev)) {
		pr_err("%s: q6afe_dev ptr is err\n", __func__);
		return false;
	}

	if (IS_ERR_OR_NULL(g_hw_afe.q6afe_dev->wait) ||
		IS_ERR_OR_NULL(g_hw_afe.q6afe_dev->state) ||
		IS_ERR_OR_NULL(g_hw_afe.q6afe_dev->status)) {
		pr_err("%s: q6afe_dev ptr is err\n", __func__);
		return false;
	}

	if (IS_ERR_OR_NULL(g_hw_afe.q6afe_dev->ops.q6afe_set_params_v2) ||
		IS_ERR_OR_NULL(g_hw_afe.q6afe_dev->ops.q6afe_set_params_v3) ||
		IS_ERR_OR_NULL(g_hw_afe.q6afe_dev->ops.q6afe_pack_and_set_param_in_band) ||
		IS_ERR_OR_NULL(g_hw_afe.q6afe_dev->ops.q6afe_get_params_v2) ||
		IS_ERR_OR_NULL(g_hw_afe.q6afe_dev->ops.q6afe_get_params_v3)) {
		pr_err("%s: q6afe_dev ops ptr err\n", __func__);
		return false;
	}
	return true;
}

bool hw_smartpa_make_afe_callback(uint32_t token, uint32_t *payload,
	uint32_t payload_size)
{
	if (!check_q6afe_ops_status())
		return false;

	if (token >= AFE_MAX_PORTS) {
		pr_err("%s: token:%d is too large\n", __func__, token);
			return false;
	}

	pr_info("%s:this_afe.hw_smartpa_state = %d, payload_size = %d\n",
		__func__, atomic_read(&g_hw_afe.hw_smartpa_state),
		payload_size);

	if (atomic_read(&g_hw_afe.hw_smartpa_state) == 1 &&
		payload_size == sizeof(uint32_t)) {
		atomic_set(g_hw_afe.q6afe_dev->status, payload[0]);
		if (payload[0])
			atomic_set(g_hw_afe.q6afe_dev->state, -1);
		else
			atomic_set(g_hw_afe.q6afe_dev->state, 0);

		atomic_set(&g_hw_afe.hw_smartpa_state, 0);
		wake_up(&(g_hw_afe.q6afe_dev->wait[token]));
		return true;
	}

	return false;
}
EXPORT_SYMBOL_GPL(hw_smartpa_make_afe_callback);

static void hw_cal_unmap_memory(void)
{
	int result = 0;

	mutex_lock(&g_hw_afe.apr_ops_lock);
	if (g_hw_afe.hw_smartpa_cal.map_data.map_handle) {
		result = afe_unmap_rtac_block(&g_hw_afe.hw_smartpa_cal.map_data.map_handle);

		/*Force to remap after unmap failed*/
		if (result)
			g_hw_afe.hw_smartpa_cal.map_data.map_handle = 0;
	}

	if (g_hw_afe.hw_smartpa_cal.map_data.dma_buf) {
		msm_audio_ion_free(g_hw_afe.hw_smartpa_cal.map_data.dma_buf);
		g_hw_afe.hw_smartpa_cal.map_data.dma_buf = NULL;
	}
	mutex_unlock(&g_hw_afe.apr_ops_lock);
}

static int hw_send_cal_in_band(struct smartpa_common_param common_param,
	int dst_port, void *cmd, size_t cmd_size)
{
	union afe_spkr_prot_config afe_spk_config;

	struct param_hdr_v3 param_info;
	int ret = -EINVAL;

	if (!check_q6afe_ops_status())
		return -EINVAL;

	if ((cmd_size > sizeof(afe_spk_config)) || (cmd == NULL))
		return -EINVAL;

	mutex_lock(&g_hw_afe.apr_ops_lock);
	memset(&afe_spk_config, 0, sizeof(afe_spk_config));
	memcpy(&afe_spk_config, cmd, cmd_size);
	memset(&param_info, 0, sizeof(param_info));

	ret = q6audio_validate_port(common_param.port_id);
	if (ret < 0) {
		pr_err("%s: Invalid src port 0x%x ret %d", __func__,
			common_param.port_id, ret);
		ret = -EINVAL;
		goto hw_send_cal_in_band_fail_cmd;
	}
	ret = q6audio_validate_port(dst_port);
	if (ret < 0) {
		pr_err("%s: Invalid dst port 0x%x ret %d", __func__,
				dst_port, ret);
		ret = -EINVAL;
		goto hw_send_cal_in_band_fail_cmd;
	}

	param_info.instance_id = INSTANCE_ID_0;
	param_info.param_id = common_param.param_id;
	param_info.param_size = cmd_size;
	param_info.module_id = common_param.module_id;

	ret = g_hw_afe.q6afe_dev->ops.q6afe_pack_and_set_param_in_band(
		common_param.port_id,
		q6audio_get_port_index(common_param.port_id),
		param_info, (u8 *)(&afe_spk_config));
	if (ret)
		pr_err("%s: port = 0x%x param = 0x%x failed %d\n", __func__,
			common_param.port_id, common_param.param_id, ret);

hw_send_cal_in_band_fail_cmd:
	pr_debug("%s: config.pdata.param_id 0x%x status %d 0x%x\n", __func__,
		param_info.param_id, ret, common_param.port_id);
	mutex_unlock(&g_hw_afe.apr_ops_lock);
	return ret;
}

static int send_afe_cal_apr_data_init(struct rtac_cal_block_data *cal)
{
	int ret;
	size_t len;

	if (cal->map_data.dma_buf == NULL) {
		/* Minimal chunk size is 4K */
		cal->map_data.map_size = SZ_16K;
		ret = msm_audio_ion_alloc(&(cal->map_data.dma_buf),
			cal->map_data.map_size,
			&(cal->cal_data.paddr),
			&len,
			&(cal->cal_data.kvaddr));
		if (ret < 0) {
			pr_err("%s: allocate buffer failed! ret = %d\n",
				__func__, ret);
			goto cal_apr_data_init_err2;
		}
		pr_debug("%s: paddr 0x%pK, kvaddr 0x%pK, map_size 0x%x\n",
			__func__, &cal->cal_data.paddr,
		cal->cal_data.kvaddr, cal->map_data.map_size);
	}

	if (cal->map_data.map_handle == 0) {
		ret = afe_map_rtac_block(cal);
		if (ret < 0) {
			pr_err("%s: map buffer failed! ret = %d\n",
				__func__, ret);
			goto cal_apr_data_init_err1;
		}
	}

	// if map_data had been inited, we don't need init again, just return 0
	return 0;

cal_apr_data_init_err1:
	msm_audio_ion_free(cal->map_data.dma_buf);
	cal->map_data.dma_buf = NULL;
cal_apr_data_init_err2:
	return ret;
}

static int hw_send_cal_apr_set_param(struct rtac_cal_block_data *cal,
	struct param_hdr_v3 *param_hdr, void *buf, uint32_t port_id)
{
	int ret;
	struct mem_mapping_hdr mem_hdr;
	uint32_t payload_size = 0;

	memset(&mem_hdr, 0x00, sizeof(mem_hdr));
	/* Send/Get package to/from ADSP */
	mem_hdr.data_payload_addr_lsw =
		lower_32_bits(cal->cal_data.paddr);
	mem_hdr.data_payload_addr_msw =
		msm_audio_populate_upper_32_bits(cal->cal_data.paddr);
	mem_hdr.mem_map_handle =
		cal->map_data.map_handle;

	pr_info("%s: Send port 0x%x, cal size %zd, cal addr 0x%pK\n",
		__func__, port_id, cal->cal_data.size, &cal->cal_data.paddr);

	ret = q6common_pack_pp_params(cal->cal_data.kvaddr,
			param_hdr,
			buf,
			&payload_size);
	if (ret) {
		pr_err("%s:pack pp params failed %d\n", __func__, ret);
		return ret;
	}
	cal->cal_data.size = payload_size;

	if (q6common_is_instance_id_supported())
		ret = g_hw_afe.q6afe_dev->ops.q6afe_set_params_v3(
				port_id, q6audio_get_port_index(port_id),
				&mem_hdr, NULL, payload_size);
	else
		ret = g_hw_afe.q6afe_dev->ops.q6afe_set_params_v2(
				port_id, q6audio_get_port_index(port_id),
				&mem_hdr, NULL, payload_size);

	if (ret) {
		pr_err("%s:get param  failed %d\n", __func__, ret);
		return ret;
	}

	return ret;
}

static int hw_send_cal_apr_get_param(struct rtac_cal_block_data *cal,
	struct param_hdr_v3 *param_hdr, void *buf, int buf_size, uint32_t port_id)
{
	int8_t *resp = (int8_t *)cal->cal_data.kvaddr;
	int ret;
	struct mem_mapping_hdr mem_hdr;

	cal->cal_data.size = buf_size + sizeof(struct param_hdr_v3);
	memset(&mem_hdr, 0x00, sizeof(mem_hdr));
	/*Send/Get package to/from ADSP*/
	mem_hdr.data_payload_addr_lsw =
		lower_32_bits(cal->cal_data.paddr);
	mem_hdr.data_payload_addr_msw =
		msm_audio_populate_upper_32_bits(cal->cal_data.paddr);
	mem_hdr.mem_map_handle =
		cal->map_data.map_handle;

	pr_info("%s: Send port 0x%x, cal size %zd, cal addr 0x%pK\n",
		__func__, port_id, cal->cal_data.size, &cal->cal_data.paddr);

	atomic_set(&g_hw_afe.hw_smartpa_state, 1);
	if (q6common_is_instance_id_supported()) {
		ret = g_hw_afe.q6afe_dev->ops.q6afe_get_params_v3(
				port_id, q6audio_get_port_index(port_id),
				&mem_hdr, param_hdr);
		resp += sizeof(struct param_hdr_v3);
	} else {
		ret = g_hw_afe.q6afe_dev->ops.q6afe_get_params_v2(
				port_id, q6audio_get_port_index(port_id),
				&mem_hdr, param_hdr);
		resp += sizeof(struct param_hdr_v1);
	}

	if (ret)
		pr_err("%s: get response from port 0x%x failed %d\n",
			__func__, port_id, ret);
	else
		/* Copy response data to command buffer */
		memcpy(buf, resp, buf_size);

	return ret;
}

static int hw_send_afe_cal_apr(struct smartpa_common_param common_param,
	void *buf, size_t buf_size, unsigned int write)
{
	int32_t ret;
	uint32_t port_id = common_param.port_id;
	int32_t  module_id = common_param.module_id;
	uint32_t port_index = 0;
	struct rtac_cal_block_data *cal = &(g_hw_afe.hw_smartpa_cal);
	struct param_hdr_v3 param_hdr;

	if (!check_q6afe_ops_status())
		return -EINVAL;

	mutex_lock(&g_hw_afe.apr_ops_lock);
	memset(&param_hdr, 0x00, sizeof(param_hdr));
	ret = send_afe_cal_apr_data_init(cal);
	if (ret < 0) {
		pr_err("%s: map data is invalid", __func__);
		goto send_afe_cal_apr_err;
	}

	port_index = q6audio_get_port_index(port_id);
	if (port_index >= AFE_MAX_PORTS) {
		pr_err("%s: Invalid AFE port = 0x%x\n", __func__, port_id);
		goto send_afe_cal_apr_err;
	}

	if (buf_size > (SZ_16K - sizeof(struct param_hdr_v3))) {
		pr_err("%s: Invalid payload size = %d\n", __func__, buf_size);
		ret = -EINVAL;
		goto send_afe_cal_apr_err;
	}

	ret = afe_q6_interface_prepare();
	if (ret != 0) {
		pr_err("%s:Q6 interface prepare failed %d\n", __func__, ret);
		goto send_afe_cal_apr_err;
	}

	/* Pack message header with data */
	param_hdr.module_id = module_id;
	param_hdr.instance_id = INSTANCE_ID_0;
	param_hdr.param_size = buf_size;
	param_hdr.param_id = common_param.param_id;
	if (write)
		ret = hw_send_cal_apr_set_param(cal, &param_hdr, buf, port_id);
	else
		ret = hw_send_cal_apr_get_param(cal, &param_hdr, buf, buf_size, port_id);

send_afe_cal_apr_err:
	mutex_unlock(&g_hw_afe.apr_ops_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(hw_send_afe_cal_apr);

int hw_register_q6afe_ops(struct hw_q6afe_interface_dev *dev)
{
	if (dev == NULL) {
		pr_err("%s: dev is NULL \n", __func__);
		return -EINVAL;
	}
	g_hw_afe.q6afe_dev = dev;

	return 0;
}
EXPORT_SYMBOL_GPL(hw_register_q6afe_ops);

struct smartpa_afe_interface hw_afe_interface = {
	.afe_tisa_get_set = NULL,
	.send_smartpakit_cal_apr = hw_send_afe_cal_apr,
	.send_cal_in_band = hw_send_cal_in_band,
	.cal_unmap_memory = hw_cal_unmap_memory,
};

void refresh_smartpa_algo_status(int port_id)
{
	if (IS_ERR_OR_NULL(g_hw_afe.misc_intf) ||
		IS_ERR_OR_NULL(g_hw_afe.misc_intf->refresh_smartpa_algo_status)) {
		pr_err("%s: it is err ptr\n", __func__);
		return;
	}


	g_hw_afe.misc_intf->refresh_smartpa_algo_status(port_id);
}
EXPORT_SYMBOL_GPL(refresh_smartpa_algo_status);

bool hw_get_smartpa_num(unsigned int *pa_num)
{
	bool ret = false;

	if (IS_ERR_OR_NULL(g_hw_afe.misc_intf) ||
		IS_ERR_OR_NULL(g_hw_afe.misc_intf->get_smartpa_num)) {
		pr_err("%s: get_smartpa_num err ptr\n", __func__);
		return ret;
	}

	ret = g_hw_afe.misc_intf->get_smartpa_num(pa_num);
	return ret;
}
EXPORT_SYMBOL_GPL(hw_get_smartpa_num);

bool hw_get_smartpa_rx_port_id(unsigned int *port_id)
{
	bool ret = false;

	if (IS_ERR_OR_NULL(g_hw_afe.misc_intf) ||
		IS_ERR_OR_NULL(g_hw_afe.misc_intf->get_smartpa_rx_port_id)) {
		pr_err("%s: get_smartpa_rx_port_id err ptr\n", __func__);
		return ret;
	}

	ret = g_hw_afe.misc_intf->get_smartpa_rx_port_id(port_id);
	return ret;
}
EXPORT_SYMBOL_GPL(hw_get_smartpa_rx_port_id);

bool hw_get_smartpa_tx_port_id(unsigned int *port_id)
{
	bool ret = false;

	if (IS_ERR_OR_NULL(g_hw_afe.misc_intf) ||
		IS_ERR_OR_NULL(g_hw_afe.misc_intf->get_smartpa_tx_port_id)) {
		pr_err("%s: get_smartpa_rx_port_id err ptr\n", __func__);
		return ret;
	}

	ret = g_hw_afe.misc_intf->get_smartpa_tx_port_id(port_id);
	return ret;
}
EXPORT_SYMBOL_GPL(hw_get_smartpa_tx_port_id);

int __init hw_adsp_apr_interface_init(void)
{
	int ret = register_adsp_intf(&hw_afe_interface);
	if (ret < 0) {
		pr_err("%s: register_adsp_intf failed %d \n", __func__, ret);
		goto init_complete;
	}

	ret = register_smartpa_misc_intf(&g_hw_afe.misc_intf);
	if (ret < 0) {
		pr_err("%s: register misc intf failed %d \n", __func__, ret);
		goto init_complete;
	}
	mutex_init(&g_hw_afe.apr_ops_lock);
	pr_info("%s: register intf succed \n", __func__);

init_complete:
	return ret;
}

void __exit hw_adsp_apr_interface_exit(void)
{
	hw_cal_unmap_memory();
	mutex_destroy(&g_hw_afe.apr_ops_lock);
}
