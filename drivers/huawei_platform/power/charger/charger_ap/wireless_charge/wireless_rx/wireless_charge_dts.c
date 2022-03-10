/*
 * wireless_charge_dts.c
 *
 * wireless charge driver, function as parsing dts
 *
 * Copyright (c) 2020-2020 Huawei Technologies Co., Ltd.
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

#include <linux/of.h>
#include <linux/slab.h>
#include <chipset_common/hwpower/common_module/power_cmdline.h>
#include <chipset_common/hwpower/common_module/power_dts.h>
#include <chipset_common/hwpower/wireless_charge/wireless_rx_ic_intf.h>
#include <huawei_platform/log/hw_log.h>
#include <huawei_platform/power/wireless/wireless_charger.h>

#define HWLOG_TAG wireless_dts
HWLOG_REGIST();

static int wlc_parse_u32_array(struct device_node *np, const char *prop,
	u32 row, u32 col, u32 *data)
{
	int ret;
	int len;

	if (!np || !prop || !data) {
		hwlog_err("parse_u32_array: para null\n");
		return -EINVAL;
	}

	len = of_property_count_u32_elems(np, prop);
	if ((len <= 0) || (len % col) || (len > row * col)) {
		hwlog_err("parse_u32_array: %s invalid\n", prop);
		return -EINVAL;
	}

	ret = of_property_read_u32_array(np, prop, data, len);
	if (ret) {
		hwlog_err("parse_u32_array: get %s fail\n", prop);
		return -EINVAL;
	}

	return len;
}

static void wlc_parse_iout_ctrl_para(struct device_node *np, struct wlrx_dev_info *di)
{
	int i;
	int para_id;
	int arr_len;
	u32 tmp_para[WLC_ICTRL_TOTAL * WLC_ICTRL_PARA_LEVEL] = { 0 };
	struct wireless_iout_ctrl_para *para = NULL;

	arr_len = wlc_parse_u32_array(np, "rx_iout_ctrl_para",
		WLC_ICTRL_PARA_LEVEL, WLC_ICTRL_TOTAL, tmp_para);
	if (arr_len <= 0) {
		di->iout_ctrl_data.ictrl_para_level = 0;
		return;
	}

	di->iout_ctrl_data.ictrl_para =
		kzalloc(sizeof(u32) * arr_len, GFP_KERNEL);
	if (!di->iout_ctrl_data.ictrl_para) {
		di->iout_ctrl_data.ictrl_para_level = 0;
		hwlog_err("parse_iout_ctrl_para: alloc ictrl_para failed\n");
		return;
	}

	di->iout_ctrl_data.ictrl_para_level = arr_len / WLC_ICTRL_TOTAL;
	for (i = 0; i < di->iout_ctrl_data.ictrl_para_level; i++) {
		para = &di->iout_ctrl_data.ictrl_para[i];
		para_id = WLC_ICTRL_BEGIN + WLC_ICTRL_TOTAL * i;
		para->iout_min = (int)tmp_para[para_id];
		para->iout_max = (int)tmp_para[++para_id];
		para->iout_set = (int)tmp_para[++para_id];
		hwlog_info("ictrl_para[%d], imin: %-4d imax: %-4d iset: %-4d\n",
			i, di->iout_ctrl_data.ictrl_para[i].iout_min,
			di->iout_ctrl_data.ictrl_para[i].iout_max,
			di->iout_ctrl_data.ictrl_para[i].iout_set);
	}
}

int wlc_parse_dts(struct device_node *np, struct wlrx_dev_info *di)
{
	if (!np || !di)
		return -EINVAL;

	wlc_parse_iout_ctrl_para(np, di);

	return 0;
}
