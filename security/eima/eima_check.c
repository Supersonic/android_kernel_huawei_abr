/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: the eima_check.c for check eima hash
 * Create: 2021-03-22
 */

#include "eima_check.h"
#include "eima_utils.h"

#define FILE_NAME_LEN 100
#define EIMA_CHECK_SAFE 0
#define EIMA_CHECK_RISK 1

static struct m_list_msg g_eima_baselines;
static struct m_list_msg g_eima_measurements;
static int g_eima_status;
static char g_eima_hash_error_file_path[EIMA_NAME_STRING_LEN + 1];

#ifdef CONFIG_HUAWEI_ENG_EIMA

#define HEX_NUM 16

enum dump_hex_member {
	EIMA_DUMP_IDX_0 = 0,
	EIMA_DUMP_IDX_1,
	EIMA_DUMP_IDX_2,
	EIMA_DUMP_IDX_3,
	EIMA_DUMP_IDX_4,
	EIMA_DUMP_IDX_5,
	EIMA_DUMP_IDX_6,
	EIMA_DUMP_IDX_7,
	EIMA_DUMP_IDX_8,
	EIMA_DUMP_IDX_9,
	EIMA_DUMP_IDX_10,
	EIMA_DUMP_IDX_11,
	EIMA_DUMP_IDX_12,
	EIMA_DUMP_IDX_13,
	EIMA_DUMP_IDX_14,
	EIMA_DUMP_IDX_15,
	EIMA_DUMP_IDX_MAX,
};

static void dump_hex(const char *tag, const uint8_t *hex, uint32_t len)
{
	uint32_t i;

	eima_debug("%s:", tag);
	for (i = 0; i < (len - len % HEX_NUM); i += HEX_NUM)
		eima_debug("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x"
			" %02x %02x %02x %02x %02x %02x",
			hex[i + EIMA_DUMP_IDX_0], hex[i + EIMA_DUMP_IDX_1],
			hex[i + EIMA_DUMP_IDX_2], hex[i + EIMA_DUMP_IDX_3],
			hex[i + EIMA_DUMP_IDX_4], hex[i + EIMA_DUMP_IDX_5],
			hex[i + EIMA_DUMP_IDX_6], hex[i + EIMA_DUMP_IDX_7],
			hex[i + EIMA_DUMP_IDX_8], hex[i + EIMA_DUMP_IDX_9],
			hex[i + EIMA_DUMP_IDX_10], hex[i + EIMA_DUMP_IDX_11],
			hex[i + EIMA_DUMP_IDX_12], hex[i + EIMA_DUMP_IDX_13],
			hex[i + EIMA_DUMP_IDX_14], hex[i + EIMA_DUMP_IDX_15]);

	for (i = (len - len % HEX_NUM); i < len; i++)
		eima_debug("hex[%d] = %02x", i, hex[i]);
}
#else
static void dump_hex(const char *tag, const uint8_t *hex, uint32_t len)
{
	(void)tag;
	(void)hex;
	(void)len;
}
#endif

static int eima_set_list(const struct m_list_msg *src_list,
	struct m_list_msg *dst_list)
{
	int ret, i;

	if (dst_list == NULL) {
		eima_error("eima destination list is null");
		return ERROR_CODE;
	}

	ret = strcpy_s(dst_list->usecase, EIMA_NAME_STRING_LEN + 1,
		src_list->usecase);
	if (ret != EOK) {
		eima_error("eima usecase name copy failed");
		return ERROR_CODE;
	}

	for (i = 0; i < src_list->num; i++) {
		if (src_list->m_list[i].fn_len == 0)
			continue;

		ret = strcpy_s(dst_list->m_list[i].fn,
			EIMA_NAME_STRING_LEN + 1, src_list->m_list[i].fn);
		if (ret != EOK)
			return ERROR_CODE;

		dst_list->m_list[i].fn_len = src_list->m_list[i].fn_len;
		dst_list->m_list[i].type = src_list->m_list[i].type;
	}
	dst_list->num = src_list->num;

	return 0;
}

static int eima_set_hash(const struct m_list_msg *src_list,
	struct m_list_msg *dst_list, int type)
{
	int ret;
	int src_index = 0;
	int dst_index = 0;

	while (src_index < src_list->num && dst_index < dst_list->num) {
		struct m_entry src_entry = src_list->m_list[src_index];
		struct m_entry *dst_entry = &(dst_list->m_list[dst_index]);

		if (strcmp(dst_entry->fn, src_entry.fn) ||
			(dst_entry->type != src_entry.type)) {
			dst_index++;
			continue;
		}

		if ((type == EIMA_MSG_BASELINE && dst_entry->hash_len == 0) ||
			(type == EIMA_MSG_RUNTIME_INFO)) {
			ret = memcpy_s(dst_entry->hash, EIMA_HASH_DIGEST_SIZE,
				src_entry.hash, src_entry.hash_len);
			if (ret != EOK)
				return ERROR_CODE;

			dst_entry->hash_len = src_entry.hash_len;
		}
		src_index++;
		dst_index++;
	}

	return 0;
}

static int eima_set_hash_error_path(const struct m_entry entry)
{
	int ret;
	char error_path[FILE_NAME_LEN] = {0};

	ret = sprintf_s(error_path, FILE_NAME_LEN, "[%s]%s:",
		entry.type == EIMA_DYNAMIC ? "dynamic" : "static", entry.fn);
	if (ret == ERROR_CODE) {
		eima_error("eima sprintf_s error_path %s error", entry.fn);
		return ret;
	}
	if (strstr(g_eima_hash_error_file_path, error_path) == NULL) {
		eima_debug("eima add hash error file path: %s", error_path);
		ret = strcat_s(g_eima_hash_error_file_path,
			EIMA_NAME_STRING_LEN + 1, error_path);
		if (ret != EOK) {
			eima_error("eima strcat_s %s to %s error",
				error_path, g_eima_hash_error_file_path);
			return ERROR_CODE;
		}
	}

	return 0;
}

static int eima_check_hash(const struct m_list_msg *baselines,
	const struct m_list_msg *measurements)
{
	int i;
	int status = EIMA_CHECK_SAFE;

	for (i = 0; i < baselines->num; i++) {
		if (measurements->m_list[i].hash_len == 0)
			continue;

		if (memcmp(baselines->m_list[i].hash,
			measurements->m_list[i].hash,
			baselines->m_list[i].hash_len)) {
			if (eima_set_hash_error_path(baselines->m_list[i]) != 0)
				return ERROR_CODE;

			eima_info("eima %s measure hash mismatch, file path: %s",
				baselines->m_list[i].type == EIMA_DYNAMIC ?
				"dynamic" : "static", baselines->m_list[i].fn);
			dump_hex("baseline hash", baselines->m_list[i].hash,
				baselines->m_list[i].hash_len);
			dump_hex("measurement hash",
				measurements->m_list[i].hash,
				measurements->m_list[i].hash_len);
			status = EIMA_CHECK_RISK;
		}
	}
	return status;
}

static int eima_set_measure_list(const struct m_list_msg *mlist_msg)
{
	int ret;

	ret = eima_set_list(mlist_msg, &g_eima_baselines);
	if (ret != 0) {
		eima_error("eima set baselines list failed");
		return ret;
	}

	ret = eima_set_list(mlist_msg, &g_eima_measurements);
	if (ret != 0)
		eima_error("eima set measurements list failed");

	return ret;
}

static int eima_set_baseline(const struct m_list_msg *mlist_msg)
{
	int ret;

	ret = eima_set_hash(mlist_msg, &g_eima_baselines, EIMA_MSG_BASELINE);
	if (ret != 0)
		eima_error("eima set baselines hash failed");

	return ret;
}

static int eima_set_runtime(const struct m_list_msg *mlist_msg)
{
	int ret;

	// when baseline has not been set,
	// the first measure result will be set as baseline
	ret = eima_set_hash(mlist_msg, &g_eima_baselines, EIMA_MSG_BASELINE);
	if (ret != 0) {
		eima_error("eima set uninitialized baselines hash during boot failed");
		return ret;
	}

	ret = eima_set_hash(mlist_msg, &g_eima_measurements,
		EIMA_MSG_RUNTIME_INFO);
	if (ret != 0) {
		eima_error("eima set measurements hash failed");
		return ret;
	}

	ret = eima_check_hash(&g_eima_baselines, &g_eima_measurements);
	if (ret == ERROR_CODE) {
		eima_error("eima check hash failed");
		return ret;
	}

	if (g_eima_status == EIMA_CHECK_SAFE)
		g_eima_status = ret;

	return 0;
}

int send_eima_data(int type, const struct m_list_msg *mlist_msg)
{
	switch (type) {
	case EIMA_MSG_TRUSTLIST:
		return eima_set_measure_list(mlist_msg);
	case EIMA_MSG_BASELINE:
		return eima_set_baseline(mlist_msg);
	case EIMA_MSG_RUNTIME_INFO:
		return eima_set_runtime(mlist_msg);
	default:
		eima_error("invalid message type, type is %d", type);
		return ERROR_CODE;
	}
}

int get_eima_status()
{
	return g_eima_status;
}

char *get_hash_error_file_path()
{
	return g_eima_hash_error_file_path;
}
