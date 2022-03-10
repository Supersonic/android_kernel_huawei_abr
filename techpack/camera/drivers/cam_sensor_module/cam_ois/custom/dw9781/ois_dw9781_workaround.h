/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor dw9781 OIS driver
 */

#ifndef _OIS_DW9781_WORKAROUND_H_
#define _OIS_DW9781_WORKAROUND_H_

#include <linux/cma.h>
#include <cam_ois_dev.h>

/* Actuator Company */
#define ACT_TDK                         0x00
#define ACT_MTM                         0x01
#define ACT_ZAHWA                       0x03
#define ACT_SEMCO                       0x04

#define GYRO_FRONT_LAYOUT               0
#define GYRO_BACK_LAYOUT                1
#define GYRO_DEGREE_0                   0
#define GYRO_DEGREE_90                  90
#define GYRO_DEGREE_180                 180
#define GYRO_DEGREE_270                 270
#define GYRO_DEGREE_360                 360
#define GYRO_DEGREE_NUM                 4

#define GYRO_DEGREE_SIZE                4
#define SHIFT8                          8
#define GYRO_SIZE                       4

#define PRODUCT_NUM_MAX                 0x10
#define ACTUATOR_NUM_MAX                0x02
#define GYRO_INIT_POSITION              360
#define GYRO_INIT_ANGLE_ROTATION        360

#define OWEN_RECOVERY_OTP_SIZE          78
#define OWEN_PID_SIZE                   8
#define OWEN_PARAM_SIZE                 11
#define OWEN_RECOVERY_SIZE              12

#define NOLAN_RECOVERY_OTP_SIZE         164
#define NOLAN_PARAM_SIZE                10
#define NOLAN_RECOVERY_SIZE             71

#define MEIGUI_RECOVERY_OTP_SIZE        186
#define MEIGUI_PARAM_SIZE               4
#define MEIGUI_RECOVERY_SIZE            73
#define MEIGUI_SUNNY_LIN_START_ADDR     44
#define MEIGUI_SUNNY_LIN_END_ADDR       148

static uint16_t g_gyro_degree_tdk[][4] = {
	{0x7FFF, 0x7FFF, 0x0000, 0x0000}, /* gyro degree 0 */
	{0x0000, 0x0000, 0x8000, 0x7FFF}, /* gyro degree 90 */
	{0x8000, 0x8000, 0x0000, 0x0000}, /* gyro degree 180 */
	{0x0000, 0x0000, 0x7FFF, 0x8000}, /* gyro degree 270 */
};

static uint16_t g_gyro_degree_mtm[][4] = {
	{0x5A82, 0x5A82, 0x5A82, 0xA57E}, /* gyro degree 0 */
	{0x5A82, 0x5A82, 0xA57E, 0x5A82}, /* gyro degree 90 */
	{0xA57E, 0xA57E, 0xA57E, 0x5A82}, /* gyro degree 180 */
	{0xA57E, 0xA57E, 0x5A82, 0xA57E}, /* gyro degree 270 */
};

/* PID */
static uint16_t g_pid_addr_owen[] = {
	0x7260, 0x7261, 0x7262, 0x7263, 0x7264, 0x7265, 0x7266, 0x7267
};

/* PID d0506 INI */
static uint16_t g_tdk_pid_owen[] = {
	0x012C, 0x0104, 0x07D0, 0x1770, 0x012C, 0x0104, 0x07D0, 0x1770
};

/* PID d0601 INI */
static uint16_t g_mtm_pid_owen[] = {
	0x012C, 0x011D, 0x0A44, 0x0000, 0x012C, 0x011D, 0x0A44, 0x0000
};

/* PID */
static uint16_t g_mtm_v3_pid_owen[] = {
	0x0078, 0x0154, 0x0DAC, 0x0000, 0x0078, 0x0154, 0x0DAC, 0x0000
};

/* PID */
static uint16_t g_mtm_v4_pid_owen[] = {
	0x0119, 0x0109, 0x08B4, 0x0000, 0x0119, 0x0109, 0x08B4, 0x0000
};

/* Actuator, HALL_POL, DAC_POL, DAC_IMAX, HALL_BIAS, SAC_CFG,
 * SAC_TVIB, SAC_CC, PANTILT_DEGREE, DD_CNT_LIMIT, DD_VAL_LIMIT
 */
static uint16_t g_read_addr_owen[] = {
	0x7004, 0x707A, 0x707B, 0x7016, 0x7070,
	0x701D, 0x701E, 0x701F, 0x725C, 0x703D,
	0x703E
};

static uint16_t g_ref_tdk_owen[] = {
	0x0000, 0x0011, 0x0111, 0x0233, 0x0000,
	0x0015, 0x1ECA, 0x03FF, 0x0352, 0x0578,
	0x012C
};
static uint16_t g_ref_mtm_owen[] = {
	0x0201, 0x0000, 0x0100, 0x0211, 0x0000,
	0x0014, 0x23D4, 0x03FF, 0x0352, 0x0578,
	0x012C
};

static uint16_t g_recovery_addr_owen[] = {
	0x7092, 0x70A4, 0x7071, 0x7072, 0x7075, 0x7076,
	0x7293, 0x72A3, 0x7094, 0x7093, 0x70A6, 0x70A5
};

static uint16_t g_recovery_otp_index_owen[] = {
	4, 8, 12, 16, 36, 40, 44, 48, 50, 52, 54, 56
};

/* Actuator, HALL_POL, DAC_POL, X_TargetStep, Y_TargetStep,
 * CROSSTALK_WEIGHT_X, CROSSTALK_WEIGHT_Y, I_MAX, Hall bias,
 * PANTILT_DEGREE, DD_CNT_LIMIT, DD_VAL_LIMIT
 */
static uint16_t g_read_addr_nolan[] = {
	0x7004, 0x707A, 0x707B, 0x73A4, 0x73A5,
	0x7387, 0x7388, 0x725C, 0x703D, 0x703E
};

static uint16_t g_ref_tdk_nolan[] = {
	0x0000, 0x0011, 0x0111, 0x01C2, 0x012C,
	0x0308, 0x048D, 0x02EE, 0x0578, 0x012C
};

static uint16_t g_ref_mtm_nolan[] = {
	0x0001, 0x0000, 0x0100, 0x0190, 0x012C,
	0x0369, 0x048D, 0x02EE, 0x0578, 0x012C
};

static uint16_t g_read_addr_meigui[] = {
	0x707A, 0x707B, 0x703D, 0x703E
};

static uint16_t g_ref_tdk_meigui[] = {
	0x0011, 0x0111, 0x0578, 0x012C
};

static uint16_t g_recovery_addr_nolan[] = {
	0x7070, 0x7071, 0x7072, 0x7092, 0x70A4, 0x7093, 0x70A5, 0x7094,
	0x70A6, 0x7293, 0x72A3, 0x70F8, 0x70F9, 0x7075, 0x7076, 0x73A8,
	0x73A9, 0x73AA, 0x73AB, 0x73AC, 0x73AD, 0x73AE, 0x73AF, 0x73B0,
	0x73B1, 0x73B2, 0x73B3, 0x73B4, 0x73B5, 0x73B6, 0x73B7, 0x73B8,
	0x73B9, 0x73BA, 0x73BB, 0x73BC, 0x73BD, 0x73BE, 0x73BF, 0x73C0,
	0x73C1, 0x7389, 0x738A, 0x738B, 0x738C, 0x738D, 0x738E, 0x738F,
	0x7390, 0x7391, 0x7392, 0x7393, 0x7394, 0x7395, 0x7396, 0x7397,
	0x7398, 0x7399, 0x739A, 0x739B, 0x739C, 0x739D, 0x739E, 0x739F,
	0x73A0, 0x73A1, 0x73A2, 0x73C4, 0x73C5, 0x73C6, 0x73C7
};

static uint16_t g_recovery_otp_index_nolan[] = {
	6, 8, 10, 20, 22, 24, 26, 28, 30, 32,
	34, 36, 38, 40, 42, 50, 52, 54, 56, 58,
	60, 62, 64, 66, 68, 70, 72, 74, 76, 78,
	80, 82, 84, 86, 88, 90, 92, 94, 96, 98,
	100, 102, 104, 106, 108, 110, 112, 114, 116, 118,
	120, 122, 124, 126, 128, 130, 132, 134, 136, 138,
	140, 142, 144, 146, 148, 150, 152, 156, 158, 160,
	162
};

static uint16_t g_recovery_addr_meigui[] = {
	0x7070, 0x7071, 0x7072, 0x7092, 0x70A4, 0x7093, 0x70A5, 0x7094,
	0x70A6, 0x7293, 0x72A3, 0x70F8, 0x70F9, 0x7075, 0x7076, 0x73A8,
	0x73A9, 0x73AA, 0x73AB, 0x73AC, 0x73AD, 0x73AE, 0x73AF, 0x73B0,
	0x73B1, 0x73B2, 0x73B3, 0x73B4, 0x73B5, 0x73B6, 0x73B7, 0x73B8,
	0x73B9, 0x73BA, 0x73BB, 0x73BC, 0x73BD, 0x73BE, 0x73BF, 0x73C0,
	0x73C1, 0x7389, 0x738A, 0x738B, 0x738C, 0x738D, 0x738E, 0x738F,
	0x7390, 0x7391, 0x7392, 0x7393, 0x7394, 0x7395, 0x7396, 0x7397,
	0x7398, 0x7399, 0x739A, 0x739B, 0x739C, 0x739D, 0x739E, 0x739F,
	0x73A0, 0x73A1, 0x73A2, 0x73C4, 0x73C5, 0x73C6, 0x73C7, 0x73A4,
	0x73A5
};

static uint16_t g_recovery_otp_index_meigui[] = {
	0, 2, 4, 14, 16, 18, 20, 22, 24, 26,
	28, 30, 32, 34, 36, 44, 46, 48, 50, 52,
	54, 56, 58, 60, 62, 64, 66, 68, 70, 72,
	74, 76, 78, 80, 82, 84, 86, 88, 90, 92,
	94, 96, 98, 100, 102, 104, 106, 108, 110, 112,
	114, 116, 118, 120, 122, 124, 126, 128, 130, 132,
	134, 136, 138, 140, 142, 144, 146, 150, 152, 154,
	156, 182, 184
};

void dw9781_ois_recovery_memory(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data, int32_t ois_otp_num,
	unsigned char act_type, unsigned char supplier_type,
	unsigned char gyro_select, int32_t gyro_arrangement, int32_t gyro_degree);

#endif
/* _OIS_DW9781_WORKAROUND_H_ */
