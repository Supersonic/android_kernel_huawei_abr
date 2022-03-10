/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: 用于大数据打点时HAL侧数据打点的头文件
 */
#ifndef __AUDIO_BIGDATA_H__
#define __AUDIO_BIGDATA_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SEC_IN_MIN                  60
#define SEC_IN_HOUR                 (60 * SEC_IN_MIN)
#define SEC_IN_DAY                  (24 * SEC_IN_HOUR)
#define HIFI_AP_MESG_CNT            127
#define VOICE_BIGDATA_NOISESIZE     16
#define VOICE_BIGDATA_VOICESIZE     16
#define VOICE_BIGDATA_CHARACTSIZE   32
#define VOICE_BIGDATA_MIC_CHANGE    20
#define MIC_CHANGE_LV17             17
#define MIC_CHANGE_LV18             32
#define MIC_CHANGE_LV19             64
#define MIC_CHANGE_LV20             128
#define BIGDATA_VOICE_HSEVENTID     916200001   // HANDSET
#define BIGDATA_VOICE_HFEVENTID     916200002   // HANDFREE
#define BIGDATA_VOICE_HESEVENTID    916200003   // HEADSET
#define BIGDATA_VOICE_BTEVENTID     916200004   // BLUETOOTH
#define BIGDATA_AUXHEAR_EVENTID     916200009

#define AUDIO_CODEC                 0
#define RET_TRUE                    0
#define RET_FALSE                   1

enum BIGDATA_REPORT_ID {
    REPORT_HANDSET = 0,
    REPORT_HANDFREE,
    REPORT_HEADSET,
    REPORT_BLUETOOTH,
    REPORT_MAX
};

enum MLIB_DEVICE {
    MLIB_DEVICE_HANDSET = 0,
    MLIB_DEVICE_HANDFREE,
    MLIB_DEVICE_CARFREE,
    MLIB_DEVICE_HEADSET,
    MLIB_DEVICE_BLUETOOTH,
    MLIB_DEVICE_PCVOICE,
    MLIB_DEVICE_HEADPHONE,
    MLIB_DEVICE_USBVOICE,
    MLIB_DEVICE_USBHEADSET,
    MLIB_DEVICE_USBHEADPHONE,
    MLIB_DEVICE_BUTT
};

enum BIGDATA_CHARACTER {
    BIGDATA_CHARACTER_WHISPER,
    BIGDATA_CHARACTER_AVE,
    BIGDATA_CHARACTER_BWE,
    BIGDATA_CHARACTER_LVM,
    BIGDATA_CHARACTER_WND,
    BIGDATA_CHARACTER_ANGLE,
    BIGDATA_CHARACTER_SVM,
    BIGDATA_CHARACTER_SVMOUT,
    BIGDATA_CHARACTER_RVB,
    BIGDATA_CHARACTER_MAX
};

enum hifi_err_log_alarm_id_enum {
    HIFI_ERR_LOG_VOICE_FTD_ERR = 0x1000,
    HIFI_ERR_LOG_VOICE_CONTROL_FAULT,
    HIFI_ERR_LOG_VOICE_SUSPEND_SLOW,
    HIFI_ERR_LOG_VOICE_BSD_ALARM,
    HIFI_ERR_LOG_VOICE_BSD_ALARM_SUB,
    HIFI_ERR_LOG_VOICE_BSD_WRR_NOISE,
    HIFI_ERR_LOG_VOICE_BSD_VQI_ALARM,
    HIFI_ERR_LOG_VOICE_BSD_DIAGNOSE_REPORT,
    HIFI_ERR_LOG_VOICE_BSD_ECHODET_ERR,
    HIFI_ERR_LOG_VOICE_BSD_COMMON_MSG,
    HIFI_ERR_LOG_VOICE_BSD_VOLTE_MSG,
    HIFI_LOG_LOG_CALL_QUALITY_VOICE_PARA_MSG = 0x100d,
    HIFI_ERR_LOG_VOICE_TRANSMIT_CHK_ERR = 0x100e,
    HIFI_ERR_LOG_VOICE_DB_CMR_MSG = 0x1020,
    HIFI_ERR_LOG_VOICE_DB_VQI_MSG = 0x1021,
    HIFI_ERR_LOG_VOICE_MIC_MICBLK_MSG = 0x1022,
    HIFI_ERR_LOG_VOICE_SIO_RESET = 0x1023,
    HIFI_ERR_LOG_VOICE_BIDATA = 0x1024,
    HIFI_ERR_LOG_VOICE_VQM_DBG = 0x1025,
    HIFI_ERR_LOG_VOICE_3A_DMD = 0x1100,
    HIFI_ERR_LOG_ALARM_ID_BUTT,
};


typedef struct {
    unsigned int mic_change[VOICE_BIGDATA_MIC_CHANGE]; // 1-20 mic change level
    unsigned int noise[VOICE_BIGDATA_NOISESIZE];       // 0-15 noise level
    unsigned int voice[VOICE_BIGDATA_VOICESIZE];       // 0-15 voice level
    unsigned int charact[VOICE_BIGDATA_CHARACTSIZE];   // 32 voice charact, such as whisper, ave and so on */
}voice_bigdata_rpt_t;

typedef struct {
    char imedia_auxhear_mode;
    short imedia_auxhear_error;
    unsigned int imedia_auxhear_device;
}imedia_auxhear_bigdata_t;

typedef struct {
    unsigned char imedia_voice_bigdata_device : 4;
    unsigned char imedia_voice_bigdata_flag : 4;
    unsigned char imedia_voice_bigdata_noise : 4;
    unsigned char imedia_voice_bigdata_voice : 4;
    unsigned char imedia_voice_bigdata_miccheck;
    unsigned char imedia_voice_bigdata_reserve2;
    unsigned int  imedia_voice_bigdata_charact;
} imedia_voice_bigdata_t;

typedef struct {
    unsigned int msg_type;
    unsigned short msg;
    unsigned short nvid;
    unsigned short data1;
    unsigned short data2;
} voice_3a_om_dmd_t;


extern void voice_bigdata_proc(void *bigdata);
extern void voice_3a_dmd_report_to_imonitor(    voice_3a_om_dmd_t *dmddata);
extern int audio_dsm_report_info(int device_type, int error_no, char *fmt, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

