/*
 * process for himntn function
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

#include "himntn_kernel.h"
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/err.h>

#ifdef HIMNTN_FACTORY_MODE
static unsigned long long g_himntn_data = HIMNTN_DEFAULT_FACTORY;
#else
#ifdef CONFIG_FINAL_RELEASE
static unsigned long long g_himntn_data = HIMNTN_DEDAULT_COMMERCIAL;
#else
static unsigned long long g_himntn_data = HIMNTN_DEFAULT_BETA;
#endif
#endif

// if item read data before himntn init, return false
static unsigned int g_himntn_data_inited;

unsigned long long get_global_himntn_data(void)
{
	return g_himntn_data;
}

void set_global_himntn_data(unsigned long long value)
{
	g_himntn_data = value;
}

static int __init second_reason_print(char *p)
{
	if (p == NULL) {
		HIMNTN_ERR("%s: input null\n", __func__);
		return -EINVAL;
	}
	HIMNTN_INFO("%s: sreason=%s\n", __func__, p);

	return 0;
}
early_param("sreason", second_reason_print);

static int __init main_reason_print(char *p)
{
	if (p == NULL) {
		HIMNTN_ERR("%s: input null\n", __func__);
		return -EINVAL;
	}
	HIMNTN_INFO("%s: reboot_reason=%s\n", __func__, p);

	return 0;
}
early_param("reboot_reason", main_reason_print);

static int __init get_himntn_from_cmdline(char *arg)
{
	unsigned long long value = 0;
	char *buf = arg;

	if (buf == NULL) {
		HIMNTN_ERR("cmdline input is null\n");
		return -EINVAL;
	}

	/* from hex string to unsigned long long */
	if (kstrtoull(buf, 16, &value) < 0) {
		HIMNTN_ERR("get himntn from cmdline fail\n");
		return -EINVAL;
	}
	HIMNTN_INFO("cmdline convert to number 0x%llx\n", value);

	set_global_himntn_data(value);
	g_himntn_data_inited = 1;

	return 0;
}
early_param("HIMNTN", get_himntn_from_cmdline);

// Description: get item index by >> base value, then & global
bool cmd_himntn_item_switch(unsigned int index)
{
	unsigned long long tmp_value;
	unsigned long long tmp_global;

	if (index < HIMNTN_ID_HEAD || index >= HIMNTN_ID_BOTTOM) {
		HIMNTN_ERR("himntn item name invalid\n");
		return false;
	}

	if (!g_himntn_data_inited) {
		HIMNTN_ERR("%d: get fail, himntn not init", index);
		return false;
	}

	tmp_value = (BASE_VALUE_GET_SWITCH >> index);
	tmp_global = get_global_himntn_data();

	if ((tmp_global & tmp_value) == 0) {
		HIMNTN_INFO("%d: status is close", index);
		return false;
	}

	HIMNTN_INFO("%d: status is open", index);
	return true;
}
