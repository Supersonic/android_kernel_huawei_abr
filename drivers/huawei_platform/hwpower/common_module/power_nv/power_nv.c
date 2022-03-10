// SPDX-License-Identifier: GPL-2.0
/*
 * power_nv.c
 *
 * nv(non-volatile) interface for power module
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

#include <chipset_common/hwpower/common_module/power_nv.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#ifdef CONFIG_HW_NVE
#include <linux/mtd/hw_nve_interface.h>
#endif
#include <chipset_common/hwpower/common_module/power_printk.h>

#define HWLOG_TAG power_nv
HWLOG_REGIST();

static struct power_nv_data_info g_power_nv_data[] = {
	{ POWER_NV_SCCALCUR, 402, "SCCAL" },
	{ POWER_NV_CUROFFSET, 450, "CURCALI" },
	{ POWER_NV_BLIMSW, 388, "BLIMSW" },
	{ POWER_NV_BBINFO, 389, "BBINFO" },
	{ POWER_NV_HWCOUL, 392, "HWCOUL" },
	{ POWER_NV_DEVCOLR, 330, "DEVCOLR" },
	{ POWER_NV_BATHEAT, 426, "BATHEAT" },
};

static struct power_nv_data_info *power_nv_get_data(enum power_nv_type type)
{
	int i;
	struct power_nv_data_info *p_data = g_power_nv_data;
	int size = ARRAY_SIZE(g_power_nv_data);

	if ((type < POWER_NV_TYPE_BEGIN) || (type >= POWER_NV_TYPE_END)) {
		hwlog_err("nv_type %d check fail\n", type);
		return NULL;
	}

	for (i = 0; i < size; i++) {
		if (type == p_data[i].type)
			break;
	}

	if (i >= size) {
		hwlog_err("nv_type %d find fail\n", type);
		return NULL;
	}

	hwlog_info("nv [%d]=%d,%u,%s\n",
		i, p_data[i].type, p_data[i].id, p_data[i].name);
	return &p_data[i];
}

#ifdef CONFIG_HW_NVE
static int power_nv_qcom_write(uint32_t nv_number, const char *nv_name,
	const void *data, uint32_t data_len)
{
	struct hw_nve_info_user *nv_info = NULL;

	if (!nv_name || !data) {
		hwlog_err("nv_name or data is null\n");
		return -EINVAL;
	}

	nv_info = kzalloc(sizeof(*nv_info), GFP_KERNEL);
	if (!nv_info)
		return -EINVAL;

	nv_info->nv_operation = NV_WRITE;
	nv_info->nv_number  = nv_number;
	nv_info->valid_size = data_len;
	memcpy(nv_info->nv_data, data,
		(sizeof(nv_info->nv_data) < nv_info->valid_size) ?
		sizeof(nv_info->nv_data) : nv_info->valid_size);

	if (hw_nve_direct_access(nv_info)) {
		hwlog_err("nv %s write fail\n", nv_name);
		kfree(nv_info);
		return -EINVAL;
	}
	kfree(nv_info);

	hwlog_info("nv %s,%u write succ\n", nv_name, data_len);
	return 0;
}

static int power_nv_qcom_read(uint32_t nv_number, const char *nv_name,
	void *data, uint32_t data_len)
{
	struct hw_nve_info_user *nv_info = NULL;

	if (!nv_name || !data) {
		hwlog_err("nv_name or data is null\n");
		return -EINVAL;
	}

	nv_info = kzalloc(sizeof(*nv_info), GFP_KERNEL);
	if (!nv_info)
		return -EINVAL;

	nv_info->nv_operation = NV_READ;
	nv_info->nv_number = nv_number;
	nv_info->valid_size = data_len;

	if (hw_nve_direct_access(nv_info)) {
		hwlog_err("nv %s read fail\n", nv_name);
		kfree(nv_info);
		return -EINVAL;
	}
	memcpy(data, nv_info->nv_data,
		(sizeof(nv_info->nv_data) < nv_info->valid_size) ?
		sizeof(nv_info->nv_data) : nv_info->valid_size);
	kfree(nv_info);

	hwlog_info("nv %s,%u read succ\n", nv_name, data_len);
	return 0;
}
#else
static int power_nv_qcom_write(uint32_t nv_number, const char *nv_name,
	const void *data, uint32_t data_len)
{
	return 0;
}

static int power_nv_qcom_read(uint32_t nv_number, const char *nv_name,
	void *data, uint32_t data_len)
{
	return 0;
}
#endif /* CONFIG_HW_NVE */

int power_nv_write(enum power_nv_type type, const void *data, uint32_t data_len)
{
	struct power_nv_data_info *p_data = power_nv_get_data(type);

	if (!p_data)
		return -EINVAL;

	return power_nv_qcom_write(p_data->id, p_data->name, data, data_len);
}

int power_nv_read(enum power_nv_type type, void *data, uint32_t data_len)
{
	struct power_nv_data_info *p_data = power_nv_get_data(type);

	if (!p_data)
		return -EINVAL;

	return power_nv_qcom_read(p_data->id, p_data->name, data, data_len);
}
