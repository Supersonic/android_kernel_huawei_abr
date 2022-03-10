/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: the eima_utils.h for print debug.
 * Create: 2017-12-20
 */

#ifndef _EIMA_UTILS_H_
#define _EIMA_UTILS_H_

#include <linux/printk.h>
#include <linux/string.h>
#include <linux/version.h>

#define EIMA_TAG "EIMA"
#define ERROR_CODE (-1)

#ifdef CONFIG_HUAWEI_ENG_EIMA

#define eima_error(fmt, args...) pr_err("%s:ERROR[%s: %d]: " fmt "\n", \
	EIMA_TAG, __func__, __LINE__, ## args)

#define eima_warning(fmt, args...) pr_warn("%s:WARN[%s: %d]: " fmt "\n", \
	EIMA_TAG, __func__, __LINE__, ## args)

#define eima_info(fmt, args...) pr_info("%s:INFO[%s: %d]: " fmt "\n", \
	EIMA_TAG, __func__, __LINE__, ## args)

#define eima_debug(fmt, args...) pr_info("%s:DEBUG[%s:%d]: " fmt "\n", \
	EIMA_TAG, __func__, __LINE__, ## args)

#else /* CONFIG_HUAWEI_ENG_EIMA */

#define eima_error(fmt, args...) pr_err("%s:ERROR %d " fmt "\n", \
	EIMA_TAG, __LINE__, ## args)

#define eima_warning(fmt, args...) pr_warn("%s:WARN %d " fmt "\n", \
	EIMA_TAG, __LINE__, ## args)

#define eima_info(fmt, args...) pr_info("%s:INFO %d " fmt "\n", \
	EIMA_TAG, __LINE__, ## args)

#define eima_debug(fmt, ...)

#endif /* CONFIG_HUAWEI_ENG_EIMA */

struct eima_async_send_work {
	struct m_list_msg *msg;
	int type;
	struct work_struct work;
};

#endif /* _EIMA_UTILS_H_ */
