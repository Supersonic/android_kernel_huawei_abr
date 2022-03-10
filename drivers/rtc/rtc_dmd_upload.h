/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * FileName: dmd_upload
 * Description: Define some macros and some structures
 * Revision history:2020-10-14 zhangxun NVE
 */
#ifndef _RTC_DMD_UPLOAD_H_
#define _RTC_DMD_UPLOAD_H_

#include <linux/rtc.h>

#define RTC_MONTH_INIT 1
#define RTC_YEAR_VALID 75
#define DSM_RTC_PMU_READCOUNT_ERROR_NUM 925005000
#define DSM_RTC_SET_RTC_TMIE_WARNING_NUM 925005001

void dsm_rtc_valid(const struct rtc_time *tm);
void dsm_rtc_check_lasttime(unsigned long time);

#endif
