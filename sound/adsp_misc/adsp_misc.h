/*
 * adsp_misc.h
 *
 * adsp misc header
 *
 * Copyright (c) 2017-2019 Huawei Technologies Co., Ltd.
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

#ifndef __ADSP_MISC_DEFS_H__
#define __ADSP_MISC_DEFS_H__

#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/ioctl.h>

#define SMARTPAKIT_NAME_MAX    64
#define RESERVED_NUM           200
#define MAX_EPCOEFF_NUM        4
#define MAX_PA_PARAS_SIZE      (16 * 1024) // 16K

enum smartpa_cmd {
	GET_CURRENT_R0 = 0,
	GET_CURRENT_TEMPRATURE,      /* 1 */
	GET_CURRENT_F0,              /* 2 */
	GET_CURRENT_Q,               /* 3 */
	GET_PARAMETERS,              /* 4 */
	GET_CURRENT_POWER,           /* 5 */
	GET_CMD_NUM,                 /* 6 */

	SET_ALGO_SECENE,             /* 7 */
	SET_CALIBRATION_VALUE,       /* 8 */
	CALIBRATE_MODE_START,        /* 9 */
	CALIBRATE_MODE_STOP,         /* 10 */
	SET_F0_VALUE,                /* 11 */

	SET_PARAMETERS,              /* 12 */
	SET_VOICE_VOLUME,            /* 13 */
	SET_LOW_POWER_MODE,          /* 14 */
	SET_SAFETY_STRATEGY,         /* 15 */
	SET_FADE_CONFIG,             /* 16 */
	SET_SCREEN_ANGLE,            /* 17 */
	SMARTPA_ALGO_ENABLE,         /* 18 */
	SMARTPA_ALGO_DISABLE,        /* 19 */
	SET_FADE_IN_CONFIG,          /* 20 */
	CMD_NUM,
	// new
	SMARTPA_PRINT_MCPS,
	SMARTPA_DEBUG,
	SMARTPA_DSP_ENABLE,
	SMARTPA_DSP_DISABLE,
	SMARTPA_SET_BATTERY,
	HOOK_CHANNEL_CMD,
	SMARTPA_DEBUG_ERROR_CODE,
};

// chip provider, which must be the same definition with smartpa kernel
enum smartpakit_chip_vendor {
	CHIP_VENDOR_MAXIM = 0, // max98925
	CHIP_VENDOR_NXP,       // tfa9871, tfa9872, tfa9874, tfa9895
	CHIP_VENDOR_TI,        // tas2560, tas2562
	CHIP_VENDOR_CS,        // cs
	CHIP_VENDOR_CUSTOMIZE, // huawei customize
	CHIP_VENDOR_RT,        // richtek
	CHIP_VENDOR_AWINIC,    // awinic
	CHIP_VENDOR_FOURSEMI,  // foursemi

	CHIP_VENDOR_MAX,
};

enum smartpa_num {
	NO_PA = 0,
	SINGLE_PA,
	DOUBLE_PA,
	THREE_PA,
	FOUR_PA,
	FIVE_PA,
	SIX_PA,
	SEVEN_PA,
	EIGHT_PA,
	MAX_PA_NUM = 8
};

enum smartpakit_pa_id {
	SMARTPAKIT_PA_ID_BEGIN = 0,
	SMARTPAKIT_PA_ID_PRIL  = SMARTPAKIT_PA_ID_BEGIN,
	SMARTPAKIT_PA_ID_PRIR,
	SMARTPAKIT_PA_ID_SECL,
	SMARTPAKIT_PA_ID_SECR,
	SMARTPAKIT_PA_ID_THIRDL,
	SMARTPAKIT_PA_ID_THIRDR,
	SMARTPAKIT_PA_ID_FOURTHL,
	SMARTPAKIT_PA_ID_FOURTHR,
	SMARTPAKIT_PA_ID_MAX,
	SMARTPAKIT_PA_ID_ALL   = 0xFF,
};

enum dsp_read_write_status {
	DSP_READ = 0,
	DSP_WRITE = 1,
};

enum smartpa_algo_status {
	PA_ALGO_DISABLE = 0,
	PA_ALGO_ENABLE = 1,
};

/* For hismartpa */
struct hismartpa_coeff {
	unsigned short kv1;
	unsigned short kv2;
	unsigned short kr1;
	unsigned short kr2;
};

struct ep_coeff {
	short point0;
	short point1;
	short point2;
	short point3;
	short point4;
	short point5;
};

struct calib_oeminfo_common {
	unsigned char chip_vendor[MAX_PA_NUM];
	unsigned char box_vendor[MAX_PA_NUM];
	unsigned int calibration_value[MAX_PA_NUM];
	struct hismartpa_coeff otp[MAX_PA_NUM];
	char chip_model[SMARTPAKIT_NAME_MAX];
	struct ep_coeff ep_coeff[MAX_EPCOEFF_NUM];
};

// smartpakit_info, which must be the same definition with smartpa hal
struct smartpakit_info {
	/* common info */
	unsigned int soc_platform;
	unsigned int algo_in;
	unsigned int out_device;
	unsigned int pa_num;
	unsigned int param_version;
	char special_name_config[SMARTPAKIT_NAME_MAX];

	/* smartpa chip info */
	unsigned int algo_delay_time;
	unsigned int chip_vendor;
	char chip_model[SMARTPAKIT_NAME_MAX];
	char chip_model_list[SMARTPAKIT_PA_ID_MAX][SMARTPAKIT_NAME_MAX];
	char cust[SMARTPAKIT_NAME_MAX];
	char product_name[SMARTPAKIT_NAME_MAX];
	struct hismartpa_coeff otp[SMARTPAKIT_PA_ID_MAX];
	unsigned int cali_data_update_mode;

	// reserved field
	char reserved[RESERVED_NUM];
};

struct adsp_misc_priv {
	struct smartpakit_info pa_info;
	struct miscdevice misc_dev;
	struct smartpa_afe_interface *intf;
	struct mutex adsp_misc_mutex;
	unsigned int calibration_value[MAX_PA_NUM];
	struct calib_oeminfo_common calib_oeminfo;
	uint32_t rx_port_id;
	uint32_t tx_port_id;
	uint32_t cali_max_limit;
	uint32_t cali_min_limit;
	uint32_t smartpa_num;
	bool cali_start;
	bool algo_enable;
};

struct smartpa_capability_intf {
	int (*cali_start)(struct adsp_misc_priv *pdata);
	int (*cali_stop)(struct adsp_misc_priv *pdata);
	int (*cali_re_update)(struct adsp_misc_priv *pdata, void *data,
			size_t len);
	int (*get_current_r0)(struct adsp_misc_priv *pdata, void *data,
			size_t len);
	int (*get_current_f0)(struct adsp_misc_priv *pdata, void *data,
			size_t len);
	int (*get_current_te)(struct adsp_misc_priv *pdata, void *data,
			size_t len);
	int (*get_current_q)(struct adsp_misc_priv *pdata, void *data,
			size_t len);
	int (*algo_control)(struct adsp_misc_priv *pdata, uint32_t enable);
	int (*smartpa_cal_write)(struct adsp_misc_priv *pdata, void *data,
			size_t len);
	int (*smartpa_cal_read)(struct adsp_misc_priv *pdata, void *data,
			size_t len);
	int (*set_algo_scene)(struct adsp_misc_priv *pdata, void *data,
			size_t len);
	int (*set_algo_battery)(struct adsp_misc_priv *pdata, void *data,
			size_t len);
};

struct misc_io_async_param {
	unsigned int  para_length;
	unsigned char *param;
};

struct misc_io_sync_param {
	unsigned int  in_len;
	unsigned char *in_param;
	unsigned int  out_len;
	unsigned char *out_param;
};

struct adsp_misc_ctl_info {
	struct smartpakit_info pa_info;
	unsigned int      param_size;
	unsigned short    cmd;
	unsigned short    size;
	unsigned char     data[0];
};

struct adsp_misc_ultrasonic_info {
	unsigned int      param_size;
	unsigned short    cmd;
	unsigned short    size;
	unsigned char     data[0];
};

struct adsp_misc_karaoke_info {
	unsigned int      param_size;
	unsigned short    dataType;
	unsigned short    value;
	unsigned short    size;
	unsigned char     data[0];
};

struct misc_io_async_param_compat {
	unsigned int para_length;
	unsigned int param;
};

struct misc_io_sync_param_compat {
	unsigned int in_len;
	unsigned int in_param;
	unsigned int out_len;
	unsigned int out_param;
};

struct adsp_misc_data_pkg {
	unsigned short cmd;
	unsigned short size;
	unsigned char  data[0];
};

struct adsp_misc_karaoke_data_pkg {
	unsigned short dataType;
	unsigned short value;
	unsigned short size;
	unsigned char  data[0];
};

struct adsp_ctl_param {
	unsigned int in_len;
	void __user  *param_in;
	unsigned int out_len;
	void __user  *param_out;
};

struct battery_param {
	unsigned int cap;
	unsigned int temp;
	unsigned int cur;
	unsigned int vol;
	unsigned int volm;
	unsigned int rbat;
};

#define MIN_PARAM_IN     sizeof(struct adsp_misc_ctl_info)
#define MIN_PARAM_OUT    sizeof(struct adsp_misc_data_pkg)

// The following is ioctrol command sent from AP to ADSP misc device,
// ADSP misc side need response these commands.
#define ADSP_MISC_IOCTL_ASYNCMSG \
	_IOWR('A', 0x0, struct misc_io_async_param) // AP send async msg to ADSP
#define ADSP_MISC_IOCTL_SYNCMSG \
	_IOW('A', 0x1, struct misc_io_sync_param) // AP send sync msg to ADSP
#define ADSP_MISC_IOCTL_ULTRASONICMSG \
	_IOW('A', 0x2, struct misc_io_sync_param) // AP send ultrasonic msg to ADSP
#define ADSP_MISC_IOCTL_KARAOKEMSG \
	_IOW('A', 0x3, struct misc_io_sync_param) // AP send karaoke msg to ADSP

#define ADSP_MISC_IOCTL_ASYNCMSG_COMPAT \
	_IOWR('A', 0x0, struct misc_io_async_param_compat)
#define ADSP_MISC_IOCTL_SYNCMSG_COMPAT \
	_IOW('A', 0x1, struct misc_io_sync_param_compat)
#define ADSP_MISC_IOCTL_ULTRASONICMSG_COMPAT \
	_IOW('A', 0x2, struct misc_io_sync_param_compat)
#define ADSP_MISC_IOCTL_KARAOKEMSG_COMPAT \
	_IOW('A', 0x3, struct misc_io_sync_param_compat)


int tas_smartpa_cali_start(struct adsp_misc_priv *pdata);
int tas_smartpa_cali_stop(struct adsp_misc_priv *pdata);
int tas_cali_re_update(struct adsp_misc_priv *pdata, void *data, size_t len);
int tas_get_current_r0(struct adsp_misc_priv *pdata, void *data, size_t len);
int tas_get_current_f0(struct adsp_misc_priv *pdata, void *data, size_t len);
int tas_get_current_te(struct adsp_misc_priv *pdata, void *data, size_t len);
int tas_get_current_q(struct adsp_misc_priv *pdata, void *data, size_t len);
int tas_algo_control(struct adsp_misc_priv *pdata, uint32_t enable);
int tas_smartpa_cal_write(struct adsp_misc_priv *pdata, void *data, size_t len);
int tas_smartpa_cal_read(struct adsp_misc_priv *pdata, void *data, size_t len);

int awinic_smartpa_cali_start(struct adsp_misc_priv *pdata);
int awinic_smartpa_cali_stop(struct adsp_misc_priv *pdata);
int awinic_cali_re_update(struct adsp_misc_priv *pdata, void *data, size_t len);
int awinic_get_current_r0(struct adsp_misc_priv *pdata, void *data, size_t len);
int awinic_get_current_f0(struct adsp_misc_priv *pdata, void *data, size_t len);
int awinic_get_current_te(struct adsp_misc_priv *pdata, void *data, size_t len);
int awinic_get_current_q(struct adsp_misc_priv *pdata, void *data, size_t len);
int awinic_algo_control(struct adsp_misc_priv *pdata, uint32_t enable);
int awinic_smartpa_cal_write(struct adsp_misc_priv *pdata, void *data,
	size_t len);
int awinic_smartpa_cal_read(struct adsp_misc_priv *pdata, void *data,
	size_t len);

int tfa_smartpa_cali_start(struct adsp_misc_priv *pdata);
int tfa_smartpa_cali_stop(struct adsp_misc_priv *pdata);
int tfa_cali_re_update(struct adsp_misc_priv *pdata, void *data, size_t len);
int tfa_get_current_r0(struct adsp_misc_priv *pdata, void *data, size_t len);
int tfa_get_current_f0(struct adsp_misc_priv *pdata, void *data, size_t len);
int tfa_get_current_te(struct adsp_misc_priv *pdata, void *data, size_t len);
int tfa_get_current_q(struct adsp_misc_priv *pdata, void *data, size_t len);
int tfa_algo_control(struct adsp_misc_priv *pdata, uint32_t enable);
int tfa_smartpa_cal_write(struct adsp_misc_priv *pdata, void *data, size_t len);
int tfa_smartpa_cal_read(struct adsp_misc_priv *pdata, void *data, size_t len);

int cust_smartpa_cali_start(struct adsp_misc_priv *pdata);
int cust_smartpa_cali_stop(struct adsp_misc_priv *pdata);
int cust_cali_re_update(struct adsp_misc_priv *pdata, void *data, size_t len);
int cust_get_current_r0(struct adsp_misc_priv *pdata, void *data, size_t len);
int cust_get_current_f0(struct adsp_misc_priv *pdata, void *data, size_t len);
int cust_get_current_te(struct adsp_misc_priv *pdata, void *data, size_t len);
int cust_get_current_q(struct adsp_misc_priv *pdata, void *data, size_t len);
int cust_algo_control(struct adsp_misc_priv *pdata, uint32_t enable);
int cust_smartpa_cal_write(struct adsp_misc_priv *pdata, void *data,
	size_t len);
int cust_smartpa_cal_read(struct adsp_misc_priv *pdata, void *data,
	size_t len);
int cust_smartpa_set_algo_scene(struct adsp_misc_priv *pdata, void *data,
	size_t len);
int cust_smartpa_set_algo_battery(struct adsp_misc_priv *pdata, void *data,
	size_t len);
int cust_ultrasonic_set_param(struct adsp_misc_priv *pdata, void *data,
	size_t len, unsigned short cmd);
int cust_ultrasonic_get_param(struct adsp_misc_priv *pdata, void *data,
	size_t len, unsigned short cmd);
int cust_karaoke_set_param(struct adsp_misc_priv *pdata, void *data,
	size_t len, unsigned short dataType, unsigned short value);
#endif // __ADSP_MISC_DEFS_H__

