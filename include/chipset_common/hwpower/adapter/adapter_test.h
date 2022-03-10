/* SPDX-License-Identifier: GPL-2.0 */
/*
 * adapter_test.h
 *
 * test of adapter driver
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

#ifndef _ADAPTER_TEST_H_
#define _ADAPTER_TEST_H_

#define AT_START_FLAG         1
#define AT_MIN_FLAG           0
#define AT_MAX_FLAG           5
#define AT_BUF_LEN            10
#define AT_SUFFIX_LEN         3

enum adapter_test_type {
	AT_TYPE_LVC,
	AT_TYPE_FCP,
	AT_TYPE_PD,
	AT_TYPE_SC,
	AT_TYPE_SC4,
	AT_TYPE_OTHER,
};

enum adapter_test_state {
	AT_DETECT_FAIL,
	AT_DETECT_SUCC,
	AT_PROTOCOL_FINISH_SUCC,
	AT_DETECT_OTHER,
};

enum adapter_test_result {
	AT_RESULT_SUCC,
	AT_RESULT_FAIL,
	AT_RESULT_NOT_SUPPORT,
};

struct adapter_test_attr {
	unsigned int type;
	const char *type_name;
	unsigned int result;
};

#ifdef CONFIG_ADAPTER_PROTOCOL
void adapter_test_clear_result(void);
void adapter_test_set_result(unsigned int type, unsigned int result);
int adapter_test_get_result(char *buf, unsigned int buf_len);
int adapter_test_check_protocol_type(unsigned int prot_type);
int adapter_test_check_support_mode(unsigned int prot_type);
int adapter_test_match_adp_type(unsigned int prot_type);
#else
static inline void adapter_test_clear_result(void)
{
}

static inline void adapter_test_set_result(unsigned int type, unsigned int result)
{
}

static inline int adapter_test_get_result(char *buf, unsigned int buf_len)
{
	return -EINVAL;
}

static inline int adapter_test_check_protocol_type(unsigned int prot_type)
{
	return -EINVAL;
}

static inline int adapter_test_check_support_mode(unsigned int prot_type)
{
	return -EINVAL;
}

static inline int adapter_test_match_adp_type(unsigned int prot_type)
{
	return AT_RESULT_NOT_SUPPORT;
}
#endif /* CONFIG_ADAPTER_PROTOCOL */

#endif /* _ADAPTER_TEST_H_ */
