/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * FileName: dmd_upload
 * Description: Define some macros and some structures
 * Revision history:2020-10-14 zhangxun NVE
 */
#include "rtc_dmd_upload.h"
#ifdef CONFIG_HUAWEI_DSM
#include <dsm/dsm_pub.h>
#endif /* CONFIG_HUAWEI_DSM */
#include <log/hiview_hievent.h>
#include <log/hw_log.h>
#include <log/log_exception.h>
#include <securec.h>

#ifdef CONFIG_HUAWEI_DSM

#define DSM_RTC_BUF_SIZE 256
#define RTC_PMU_BULK_INIT 0xff
#define RTC_YEAR_LINUX_INIT 1900
#define RTC_MONTH_INIT 1
#define RTC_YEAR_VALID 75
#define PMU_RTC_SET_DELAY 200

unsigned long g_last_time;

static void rtc_dsm_report(int err_code, const char *err_msg)
{
	int ret;
	struct hiview_hievent *hi_event =
		hiview_hievent_create(err_code);

	if (!hi_event) {
		pr_err("create hievent fail\n");
		return;
	}

	ret = hiview_hievent_put_string(hi_event, "CONTENT", err_msg);
	if (ret < 0)
		pr_err("hievent put string failed\n");

	ret = hiview_hievent_report(hi_event);
	if (ret < 0)
		pr_err("report hievent failed\n");

	hiview_hievent_destroy(hi_event);
}

void dsm_rtc_valid(const struct rtc_time *tm)
{
	int err = 0;
	char buf[DSM_RTC_BUF_SIZE] = {0};
	if (tm->tm_year < RTC_YEAR_VALID)
		err = -EINVAL;
	// hour not bigger than 23, second & minutes not bigger than 59
	if (tm->tm_hour > 23 || tm->tm_min > 59 || tm->tm_sec > 59)
		err = -EINVAL;
	if (err) {
		/* DSM RTC SET TIME ERROR */
		if (snprintf_s(buf, sizeof(buf), DSM_RTC_BUF_SIZE,
			"Soc rtc set time error, time : %d-%d-%d-%d-%d-%d\n",
			(tm->tm_year + RTC_YEAR_LINUX_INIT), (tm->tm_mon +
			RTC_MONTH_INIT), tm->tm_mday, tm->tm_hour,
			tm->tm_min, tm->tm_sec) == -1)
			pr_err("snprintf failed!");
		rtc_dsm_report(DSM_RTC_SET_RTC_TMIE_WARNING_NUM, buf);
	}
}

void dsm_rtc_check_lasttime(unsigned long time)
{
	char buf[DSM_RTC_BUF_SIZE] = {0};
	if (g_last_time > time) {
		/* DMD RTC PRINT INFO */
		if (snprintf_s(buf, sizeof(buf), DSM_RTC_BUF_SIZE,
			"Pmu rtc readcount error, time : %lu, lasttime : %lu\n",
			time, g_last_time) == -1)
			pr_err("snprintf failed!");
		rtc_dsm_report(DSM_RTC_PMU_READCOUNT_ERROR_NUM, buf);
	}
	g_last_time = time;
}

#else
void dsm_rtc_valid(const struct rtc_time *tm)
{
	pr_info("RTC_DSM %s: not support dsm\n", __func__);
}

void dsm_rtc_check_lasttime(unsigned long time)
{
	pr_info("RTC_DSM %s: not support dsm\n", __func__);
}
#endif
