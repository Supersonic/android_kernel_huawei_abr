/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: 用于大数据打点时HIFI侧数据打点的文件
 */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include "voice_bigdata.h"
#include <huawei_platform/log/imonitor.h>
#include <huawei_platform/log/imonitor_keys.h>
#include <linux/timekeeping.h>
#include <dsm/dsm_pub.h>

static const char * const voice_param[VOICE_BIGDATA_VOICESIZE] = {
	"VoiceCnt0",  "VoiceCnt1",	"VoiceCnt2",  "VoiceCnt3",
	"VoiceCnt4",  "VoiceCnt5",	"VoiceCnt6",  "VoiceCnt7",
	"VoiceCnt8",  "VoiceCnt9",	"VoiceCnt10", "VoiceCnt11",
	"VoiceCnt12", "VoiceCnt13", "VoiceCnt14", "VoiceCnt15"
};

static const char * const noise_param[VOICE_BIGDATA_NOISESIZE] = {
	"NoiseCnt0",  "NoiseCnt1",	"NoiseCnt2",  "NoiseCnt3",
	"NoiseCnt4",  "NoiseCnt5",	"NoiseCnt6",  "NoiseCnt7",
	"NoiseCnt8",  "NoiseCnt9",	"NoiseCnt10", "NoiseCnt11",
	"NoiseCnt12", "NoiseCnt13", "NoiseCnt14", "NoiseCnt15"
};

static const char * const mic_param[VOICE_BIGDATA_MIC_CHANGE] = {
	"BlockmicCnt0",  "BlockmicCnt1",  "BlockmicCnt2",  "BlockmicCnt3",
	"BlockmicCnt4",  "BlockmicCnt5",  "BlockmicCnt6",  "BlockmicCnt7",
	"BlockmicCnt8",  "BlockmicCnt9",  "BlockmicCnt10", "BlockmicCnt11",
	"BlockmicCnt12", "BlockmicCnt13", "BlockmicCnt14", "BlockmicCnt15",
	"BlockmicCnt16", "BlockmicCnt17", "BlockmicCnt18", "BlockmicCnt19"
};

static const char * const charact_param[BIGDATA_CHARACTER_RVB] = {
	"WhisperCnt", "AveCnt", "BweCnt", "AutolvmCnt", "WindCnt",
	"AngleCnt", "SvmCnt", "SvmoutCnt"
};

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "VOICE_BIGDATA"

static void bigdata_report_voice_level(
    voice_bigdata_rpt_t *bigdata_rpt,
    struct imonitor_eventobj *obj)
{
    int i;

    for (i = 0; i < VOICE_BIGDATA_VOICESIZE; i++) {
        imonitor_set_param_integer_v2(obj, voice_param[i], bigdata_rpt->voice[i]);
    }
}
static void bigdata_report_noise_level(
    voice_bigdata_rpt_t *bigdata_rpt,
    struct imonitor_eventobj *obj)
{
    int i;

    for (i = 0; i < VOICE_BIGDATA_NOISESIZE; i++) {
        imonitor_set_param_integer_v2(obj, noise_param[i], bigdata_rpt->noise[i]);
    }
}

static void bigdata_report_mic_change(
    voice_bigdata_rpt_t *bigdata_rpt,
    struct imonitor_eventobj *obj,
    int rpt_id)
{
    int i;

    // 只上报手持模式和免提模式
    if (rpt_id >= REPORT_HEADSET) {
        return;
    }

    for (i = 0; i < VOICE_BIGDATA_MIC_CHANGE; i++) {
        imonitor_set_param_integer_v2(obj, mic_param[i], bigdata_rpt->mic_change[i]);
    }
}

static void bigdata_report_character(
    voice_bigdata_rpt_t *bigdata_rpt,
    struct imonitor_eventobj *obj,
    int rpt_id)
{
    int i;
    const int size = BIGDATA_CHARACTER_RVB;
    // 手持模式上报8个特性
    // 其它模式上报前5个特性

    for (i = 0; i < size; i++) {
        imonitor_set_param_integer_v2(obj, charact_param[i], bigdata_rpt->charact[i]);
        if (rpt_id != REPORT_HANDSET &&     // 非手持模式
            i >= BIGDATA_CHARACTER_WND) {   // 上报5个特性
            break;
        }
    }
}

// carry data from HAL to imonitor
static void voice_bigdata_report_to_imonitor(unsigned int event_id,
    voice_bigdata_rpt_t *bigdata_rpt)
{
    struct imonitor_eventobj *obj = NULL;
    int rpt_id;

    // creat imonitor obj
    obj = imonitor_create_eventobj(event_id);
    if (!obj) {
        pr_err("%s(), imonitor obj create fail", __FUNCTION__);
        return;
    }

    switch (event_id) {
        case BIGDATA_VOICE_HSEVENTID:
            rpt_id = REPORT_HANDSET;
            break;
        case BIGDATA_VOICE_HFEVENTID:
            rpt_id = REPORT_HANDFREE;
            break;
        case BIGDATA_VOICE_HESEVENTID:
            rpt_id = REPORT_HEADSET;
            break;
        case BIGDATA_VOICE_BTEVENTID:
            rpt_id = REPORT_BLUETOOTH;
            break;
        default:
            pr_err("%s(), invalid event_id=%d", __FUNCTION__, event_id);
            return;
    }

    // report nosie and voice level to imonitor
    bigdata_report_voice_level(&bigdata_rpt[rpt_id], obj);
    bigdata_report_noise_level(&bigdata_rpt[rpt_id], obj);

    // report mic change to imonitor
    bigdata_report_mic_change(&bigdata_rpt[rpt_id], obj, rpt_id);

    // report character to imonitor
    bigdata_report_character(&bigdata_rpt[rpt_id], obj, rpt_id);

    imonitor_send_event(obj);
    imonitor_destroy_eventobj(obj);
}

static void mic_change_stats(
    voice_bigdata_rpt_t *bigdata_rpt,
    imedia_voice_bigdata_t *bigdata)
{
    // 设置mic切换level, 次数转换成level
    /* level    mic_change_num
       0        1
       1        2
       2        3
       3        4
       4        5
       5        6
       6        7
       7        8
       8        9
       9        10
       10       11
       11       12
       12       13
       13       14
       14       15
       15       16
       16       [17-32)
       17       [32-64)
       18       [64-128)
       19       >=128
    */
    int num = bigdata->imedia_voice_bigdata_miccheck;

    if (num == 0) {
        return;
    } else if (num < MIC_CHANGE_LV17) {
        bigdata_rpt->mic_change[num - 1]++;
    } else if ((num >= MIC_CHANGE_LV17) && (num < MIC_CHANGE_LV18)) {
        const int level_index = 16;
        bigdata_rpt->mic_change[level_index]++;
    } else if ((num >= MIC_CHANGE_LV18) && (num < MIC_CHANGE_LV19)) {
        const int level_index = 17;
        bigdata_rpt->mic_change[level_index]++;
    } else if ((num >= MIC_CHANGE_LV19) && (num < MIC_CHANGE_LV20)) {
        const int level_index = 18;
        bigdata_rpt->mic_change[level_index]++;
    } else if (num >= MIC_CHANGE_LV20) {
        const int level_index = 19;
        bigdata_rpt->mic_change[level_index]++;
    }
}

static void character_stats(
    voice_bigdata_rpt_t *bigdata_rpt,
    imedia_voice_bigdata_t *bigdata)
{
    unsigned int character = bigdata->imedia_voice_bigdata_charact;
    int i;

    for (i = 0; i < BIGDATA_CHARACTER_MAX; i++) {
        // 取出每个特性对应的bit位，累计每个特性的次数
        if ((1 << i) & character) {
            bigdata_rpt->charact[i]++;
        }
    }
}

static void voice_bigdata_stats(
    voice_bigdata_rpt_t *bigdata_rpt,
    imedia_voice_bigdata_t *bigdata)
{
    int rpt_id;

    switch (bigdata->imedia_voice_bigdata_device) {
        case MLIB_DEVICE_HANDSET:
        case MLIB_DEVICE_HANDFREE:
            // 只统计HANDSET和HANDFREE模式下的MIC切换
            rpt_id = bigdata->imedia_voice_bigdata_device;
            mic_change_stats(&bigdata_rpt[rpt_id], bigdata);
            break;
        case MLIB_DEVICE_USBHEADSET:
        case MLIB_DEVICE_HEADSET:
            // USBHEADSET和HEADSET 一起统计为HEADSET
            rpt_id = REPORT_HEADSET;
            break;
        case MLIB_DEVICE_BLUETOOTH:
            // BLUETOOTH统计为BLUETOOTH
            rpt_id = REPORT_BLUETOOTH;
            break;
        default:
            // 其它模式不统计
            return;
    }

    // 统计noise/voice每个level的次数
    bigdata_rpt[rpt_id].noise[bigdata->imedia_voice_bigdata_noise]++;
    bigdata_rpt[rpt_id].voice[bigdata->imedia_voice_bigdata_voice]++;
    // 统计每个特性的次数
    character_stats(&bigdata_rpt[rpt_id], bigdata);
}

static void voice_bigdata_report(voice_bigdata_rpt_t *bigdata_rpt)
{
    int event_id;
	pr_err("report a bigdata message!\n");

    // report data to imonitor
    for (event_id = BIGDATA_VOICE_HSEVENTID; event_id <= BIGDATA_VOICE_BTEVENTID; event_id++) {
        voice_bigdata_report_to_imonitor(event_id, bigdata_rpt);
    }
}
void voice_bigdata_proc(void *data)
{
    static voice_bigdata_rpt_t bigdata_rpt[REPORT_MAX];
    static struct timespec bigdata_time1 = {0, 0};
    static struct timespec bigdata_time2;
    static int bigdata_count = 0;
    imedia_voice_bigdata_t *bigdata = (imedia_voice_bigdata_t*)data;
    struct timespec64 now;

    if (bigdata == NULL) {
        pr_err("%s(), bigdata NULL", __FUNCTION__);
        return;
    }

    pr_err("%s() line:%d received VOICE_BIG_DATA, dev:%d flag:%d noise:%d voice:%d mic:%d rsv2:%d charact:0x%x\n",
        __FUNCTION__, __LINE__,
        bigdata->imedia_voice_bigdata_device,
        bigdata->imedia_voice_bigdata_flag,
        bigdata->imedia_voice_bigdata_noise,
        bigdata->imedia_voice_bigdata_voice,
        bigdata->imedia_voice_bigdata_miccheck,
        bigdata->imedia_voice_bigdata_reserve2,
        bigdata->imedia_voice_bigdata_charact);

    bigdata_count++;
    if (bigdata_count < HIFI_AP_MESG_CNT) {
        // 大数据统计信息累计
        voice_bigdata_stats(bigdata_rpt, bigdata);
    }
    ktime_get_real_ts64(&now);
    bigdata_time2.tv_sec = now.tv_sec;

    // 现在距离上次上报大于24小时，上报到imonitor, 或者开机后收到第一条消息，上报imonitor
    if ((bigdata_time2.tv_sec - bigdata_time1.tv_sec) > SEC_IN_DAY ||
        bigdata_time1.tv_sec == 0) {
        voice_bigdata_report(bigdata_rpt);
        memset(bigdata_rpt, 0, sizeof(voice_bigdata_rpt_t));

        ktime_get_real_ts64(&now);
        bigdata_time1.tv_sec = now.tv_sec;
        bigdata_count = 0;
    }
}

// carry data from HAL to imonitor
void voice_3a_dmd_report_to_imonitor(    voice_3a_om_dmd_t *dmddata)
{
	if (dmddata != NULL) {
		audio_dsm_report_info(AUDIO_CODEC, DSM_SOC_HIFI_3A_ERROR,  "3a error type:%d error:%d, nvid:%d, data1:%d, data2:%d\n",
			dmddata->msg_type, dmddata->msg, dmddata->nvid, dmddata->data1, dmddata->data2);
	}
}



