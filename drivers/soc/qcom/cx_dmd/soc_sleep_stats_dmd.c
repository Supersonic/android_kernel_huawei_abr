/*
 * soc_sleep_stats_dmd.c
 *
 * cx none idle dmd upload
 *
 * Copyright (C) 2017-2021 Huawei Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <log/hiview_hievent.h>
#include <securec.h>

#define CXSD_NOT_IDLE_DMD 925004501
#define REPORT_NONEIDLE_DMD_TIME (60 * 60 * 1000) /* 60min */

static __le64 cx_last_acc_dur;
static s64 cx_last_time;
static bool cx_first = true;

static void soc_sleep_stats_dmd_report(int domain, const char* context)
{
	int dmd_code, ret;
	struct hiview_hievent *hi_event = NULL;

	dmd_code = domain;

	hi_event = hiview_hievent_create(dmd_code);
	if (!hi_event) {
		pr_err("create hievent fail\n");
		return;
	}

	ret = hiview_hievent_put_string(hi_event, "CONTENT", context);
	if (ret < 0)
		pr_err("hievent put string failed\n");

	ret = hiview_hievent_report(hi_event);
	if (ret < 0)
		pr_err("report hievent failed\n");

	hiview_hievent_destroy(hi_event);
}

static void check_count(const __le64 cur_acc_duration, const s64 now, bool *isfirst, s64 *last_time,
    __le64 *last_val, const int domain)
{
	s64 none_idle_time;

	if (*isfirst) {
		*last_time = now;
		*last_val = cur_acc_duration;
		*isfirst = false;
		return;
	}

	if (cur_acc_duration != *last_val) {
		*last_val = cur_acc_duration;
		*last_time = now;
	} else {
		none_idle_time = now - *last_time;
		if (none_idle_time >= REPORT_NONEIDLE_DMD_TIME) {
			soc_sleep_stats_dmd_report(domain, "cx none idle");
			*last_time = now;
		}
	}
}

static void check_soc_idle_count(const struct stats_entry *data)
{
	char stat_type[5] = {0};
	__le64 cur_acc_dur;
	s64 cur_time;

	memcpy_s(stat_type, sizeof(stat_type), &data->entry.stat_type, sizeof(u32));

	cur_acc_dur = data->entry.accumulated;
	cur_time = ktime_to_ms(ktime_get_real());

	if (!strcmp(stat_type, "cxsd"))
		check_count(cur_acc_dur, cur_time, &cx_first, &cx_last_time, &cx_last_acc_dur, CXSD_NOT_IDLE_DMD);
}
