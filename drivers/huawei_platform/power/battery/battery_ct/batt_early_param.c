/*
 * battery_early_param.c
 *
 * battery early param.
 *
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
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

#include "batt_early_param.h"
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <huawei_platform/power/power_mesg_srv.h>

static struct batt_ic_para g_ic_para[MAX_IC_COUNT];
static struct batt_info_para g_info_para[MAX_IC_COUNT];
static struct batt_cert_para g_cert_para[MAX_IC_COUNT];
static struct batt_group_sn_para g_group_sn_para[MAX_IC_COUNT];
static char g_nv_sn[MAX_SN_LEN];

#define BATT_IC_CMDLINE_TAG "battery_ic"
#define BATT_INFO_CMDLINE_TAG "battery_info"
#define BATT_NV_SN_CMDLINE_TAG "battery_nv_sn"
#define BATT_CERT_CMDLINE_TAG "battery_cert"
#define BATT_GROUP_SN_CMDLINE_TAG "battery_group_sn"
#define BATT_IC_CMDLINE_TAG_1 "battery_ic_aux"
#define BATT_INFO_CMDLINE_TAG_1 "battery_info_aux"
#define BATT_CERT_CMDLINE_TAG_1 "battery_cert_aux"
#define BATT_GROUP_SN_CMDLINE_TAG_1 "battery_group_sn_aux"

static char *g_batt_ic_cmdline_tags[MAX_IC_COUNT] = {
	BATT_IC_CMDLINE_TAG,
	BATT_IC_CMDLINE_TAG_1,
};

static char *g_batt_info_cmdline_tags[MAX_IC_COUNT] = {
	BATT_INFO_CMDLINE_TAG,
	BATT_INFO_CMDLINE_TAG_1,
};

static char *g_batt_cert_cmdline_tags[MAX_IC_COUNT] = {
	BATT_CERT_CMDLINE_TAG,
	BATT_CERT_CMDLINE_TAG_1,
};

static char *g_batt_group_sn_cmdline_tags[MAX_IC_COUNT] = {
	BATT_GROUP_SN_CMDLINE_TAG,
	BATT_GROUP_SN_CMDLINE_TAG_1,
};

static int batt_get_ic_index_by_batt_ic(const char *str)
{
	unsigned int i;

	for (i = 0; i < MAX_IC_COUNT; ++i) {
		if (!strcmp(str, g_batt_ic_cmdline_tags[i]))
			return i;
	}
	return -1;
}

static int batt_get_ic_index_by_batt_info(const char *str)
{
	unsigned int i;

	for (i = 0; i < MAX_IC_COUNT; ++i) {
		if (!strcmp(str, g_batt_info_cmdline_tags[i]))
			return i;
	}
	return -1;
}

static int batt_get_ic_index_by_batt_cert(const char *str)
{
	unsigned int i;

	for (i = 0; i < MAX_IC_COUNT; ++i) {
		if (!strcmp(str, g_batt_cert_cmdline_tags[i]))
			return i;
	}
	return -1;
}

static int batt_get_ic_index_by_group_sn(const char *str)
{
	unsigned int i;

	for (i = 0; i < MAX_IC_COUNT; ++i) {
		if (!strcmp(str, g_batt_group_sn_cmdline_tags[i]))
			return i;
	}
	return -1;
}

static int __init batt_parse_nv_sn_cmdline(char *p)
{
	int strp_len;

	if (!p)
		return -1;

	strp_len = strnlen(p, MAX_SN_LEN - 1);
	memcpy(g_nv_sn, p, strp_len);
	g_nv_sn[strp_len] = 0;
	return 0;
}
early_param("battery_nv_sn", batt_parse_nv_sn_cmdline);

static int batt_hexstr_to_array(const char *p, char *hex, int hex_len)
{
	int hex_index = 0;
	int p_pos = 0;
	int p_len = strlen(p);
	char temp[BATT_HEX_STR_LEN] = { 0 };

	if (((p_len % (BATT_HEX_STR_LEN - 1)) != 0) ||
		((p_len / (BATT_HEX_STR_LEN - 1)) > hex_len))
		return -1;

	while (p_pos < p_len) {
		memcpy(temp, p + p_pos, BATT_HEX_STR_LEN - 1);
		temp[BATT_HEX_STR_LEN - 1] = 0;

		if (kstrtou8(temp, POWER_BASE_HEX, &hex[hex_index]) != 0)
			return -1;

		p_pos += BATT_HEX_STR_LEN - 1;
		hex_index++;
	}

	return hex_index;
}

static int batt_parse_ic_info_cmdline_by_index(char *p, int ic_index)
{
	char *token = NULL;

	if (!p || (ic_index < 0) || (ic_index >= MAX_IC_COUNT))
		return -1;

	memset(&g_ic_para[ic_index], 0, sizeof(struct batt_ic_para));
	token = strsep(&p, ",");
	if (!token)
		goto parse_fail;

	if (kstrtoint(token, POWER_BASE_HEX, &g_ic_para[ic_index].ic_type) != 0)
		goto parse_fail;

	g_ic_para[ic_index].uid_len = batt_hexstr_to_array(p,
		g_ic_para[ic_index].uid, BATT_MAX_UID_LEN);
	if (g_ic_para[ic_index].uid_len == 0)
		goto parse_fail;
	return 0;

parse_fail:
	g_ic_para[ic_index].ic_type = LOCAL_IC_TYPE;
	return -1;
}

static int __init batt_parse_ic_info_cmdline(char *p)
{
	return batt_parse_ic_info_cmdline_by_index(p,
		batt_get_ic_index_by_batt_ic(BATT_IC_CMDLINE_TAG));
}
early_param(BATT_IC_CMDLINE_TAG, batt_parse_ic_info_cmdline);

static int __init batt_parse_ic_info_cmdline_1(char *p)
{
	return batt_parse_ic_info_cmdline_by_index(p,
		batt_get_ic_index_by_batt_ic(BATT_IC_CMDLINE_TAG_1));
}
early_param(BATT_IC_CMDLINE_TAG_1, batt_parse_ic_info_cmdline_1);

static int batt_parse_info_cmdline_by_index(char *p, int ic_index)
{
	int value;
	char *token = NULL;

	if (!p || (ic_index < 0) || (ic_index >= MAX_IC_COUNT))
		return -1;

	memset(&g_info_para[ic_index], 0, sizeof(struct batt_info_para));
	token = strsep(&p, ",");
	if (!token)
		return -1;
	memcpy(g_info_para[ic_index].type, token,
		strnlen(token, BATTERY_TYPE_BUFF_SIZE - 1));

	token = strsep(&p, ",");
	if (!token)
		return -1;
	if (kstrtoint(token, POWER_BASE_HEX, &value) != 0)
		return -1;
	g_info_para[ic_index].source = value;
	memcpy(g_info_para[ic_index].sn, p, strnlen(p, MAX_SN_LEN - 1));
	return 0;
}

static int __init batt_parse_info_cmdline(char *p)
{
	return batt_parse_info_cmdline_by_index(p,
		batt_get_ic_index_by_batt_info(BATT_INFO_CMDLINE_TAG));
}
early_param(BATT_INFO_CMDLINE_TAG, batt_parse_info_cmdline);

static int __init batt_parse_info_cmdline_1(char *p)
{
	return batt_parse_info_cmdline_by_index(p,
		batt_get_ic_index_by_batt_info(BATT_INFO_CMDLINE_TAG_1));
}
early_param(BATT_INFO_CMDLINE_TAG_1, batt_parse_info_cmdline_1);

static int batt_parse_cert_cmdline_by_index(char *p, int ic_index)
{
	int value;
	char *token = NULL;

	if (!p || (ic_index < 0) || (ic_index >= MAX_IC_COUNT))
		return -1;

	memset(&g_cert_para[ic_index], 0, sizeof(struct batt_cert_para));
	token = strsep(&p, ",");
	if (!token)
		return -1;

	if (kstrtoint(token, POWER_BASE_HEX, &value) != 0)
		return -1;
	g_cert_para[ic_index].key_result = value;

	g_cert_para[ic_index].sign_len = batt_hexstr_to_array(p,
		g_cert_para[ic_index].signature, BATT_MAX_SIGN_LEN);
	g_cert_para[ic_index].valid_sign = 1;
	return 0;
}

static int __init batt_parse_cert_cmdline(char *p)
{
	return batt_parse_cert_cmdline_by_index(p,
		batt_get_ic_index_by_batt_cert(BATT_CERT_CMDLINE_TAG));
}
early_param(BATT_CERT_CMDLINE_TAG, batt_parse_cert_cmdline);

static int __init batt_parse_cert_cmdline_1(char *p)
{
	return batt_parse_cert_cmdline_by_index(p,
		batt_get_ic_index_by_batt_cert(BATT_CERT_CMDLINE_TAG_1));
}
early_param(BATT_CERT_CMDLINE_TAG_1, batt_parse_cert_cmdline_1);

static bool batt_check_all_zero(char *sn, int sn_length)
{
	int i;

	for (i = 0; i < sn_length; ++i) {
		if ((sn[i] != '0') && (sn[i] != 0))
			return false;
	}
	return true;
}

static int batt_parse_group_sn_cmdline_by_index(char *p, int ic_index)
{
	if (!p || (ic_index < 0) || (ic_index >= MAX_IC_COUNT))
		return -1;

	memset(&g_group_sn_para[ic_index], 0, sizeof(struct batt_group_sn_para));
	if (batt_check_all_zero(p, strnlen(p, MAX_SN_LEN - 1)))
		return -1;

	g_group_sn_para[ic_index].group_sn_len = strnlen(p, MAX_SN_LEN - 1);
	memcpy(g_group_sn_para[ic_index].group_sn, p,
		g_group_sn_para[ic_index].group_sn_len);
	g_group_sn_para[ic_index].valid_sign = 1;
	return 0;
}

static int __init batt_parse_group_sn_cmdline(char *p)
{
	return batt_parse_group_sn_cmdline_by_index(p,
		batt_get_ic_index_by_group_sn(BATT_GROUP_SN_CMDLINE_TAG));
}
early_param(BATT_GROUP_SN_CMDLINE_TAG, batt_parse_group_sn_cmdline);

static int __init batt_parse_group_sn_cmdline_1(char *p)
{
	return batt_parse_group_sn_cmdline_by_index(p,
		batt_get_ic_index_by_group_sn(BATT_GROUP_SN_CMDLINE_TAG_1));
}
early_param(BATT_GROUP_SN_CMDLINE_TAG_1, batt_parse_group_sn_cmdline_1);

struct batt_ic_para *batt_get_ic_para(uint32_t ic_index)
{
	if ((ic_index >= MAX_IC_COUNT) ||
		(g_ic_para[ic_index].ic_type == LOCAL_IC_TYPE))
		return NULL;

	return &g_ic_para[ic_index];
}

struct batt_info_para *batt_get_info_para(uint32_t ic_index)
{
	if ((ic_index >= MAX_IC_COUNT) ||
		(g_ic_para[ic_index].ic_type == LOCAL_IC_TYPE))
		return NULL;

	return &g_info_para[ic_index];
}

struct batt_cert_para *batt_get_cert_para(uint32_t ic_index)
{
	if ((ic_index >= MAX_IC_COUNT) || !g_cert_para[ic_index].valid_sign)
		return NULL;

	return &g_cert_para[ic_index];
}

struct batt_group_sn_para *batt_get_group_sn_para(uint32_t ic_index)
{
	if ((ic_index >= MAX_IC_COUNT) || !g_group_sn_para[ic_index].valid_sign)
		return NULL;

	return &g_group_sn_para[ic_index];
}

char *batt_get_nv_sn(void)
{
	if (strnlen(g_nv_sn, MAX_SN_LEN - 1) == 0)
		return NULL;

	return g_nv_sn;
}
