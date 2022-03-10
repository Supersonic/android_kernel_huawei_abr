/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor dw9781 OIS driver
 */

#include <linux/module.h>
#include <linux/firmware.h>
#include <linux/dma-contiguous.h>
#include <cam_sensor_cmn_header.h>
#include <cam_ois_core.h>
#include <cam_ois_soc.h>
#include <cam_sensor_util.h>
#include <cam_debug_util.h>
#include <cam_res_mgr_api.h>
#include <cam_common_util.h>
#include <cam_packet_util.h>

#include "vendor_ois_core.h"
#include "ois_dw9781_workaround.h"
#include "ois_dw9781_params.h"

static void dw_ois_reset(struct cam_ois_ctrl_t *o_ctrl)
{
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xD002, 0x0001); /* Logic reset */
	msleep(4); /* wait time */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xD001, 0x0001); /* Active mode (DSP ON) */
	msleep(25); /* ST gyro - over wait 25ms, default Servo On */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xEBF1, 0x56FA); /* User protection release */
	CAM_INFO(CAM_OIS, "dw9781 ois reset finish");
}

static int32_t dw_calibration_save(struct cam_ois_ctrl_t *o_ctrl)
{
	CAM_INFO(CAM_OIS, "dw9781 calibration save starting");
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7011, 0x00AA); /* select mode */
	msleep(10);
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7010, 0x8000); /* start mode */
	msleep(100); /* wait time */
	dw_ois_reset(o_ctrl);
	CAM_INFO(CAM_OIS, "dw9781 calibration save finish");
	return 0;
}

static void owen_flash_check(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data, uint16_t *rewrite_flag)
{
	int32_t i, r_index;
	uint16_t read_data = 0;
	uint16_t user_flash;

	for (i = 0; i < OWEN_RECOVERY_SIZE; i++) {
		r_index = g_recovery_otp_index_owen[i];
		dw9781b_ois_read_addr16_value16(o_ctrl, g_recovery_addr_owen[i], &read_data);
		user_flash = (uint16_t)(ois_otp_data[r_index] << SHIFT8) +
			ois_otp_data[r_index + 1];
		if (read_data != user_flash) {
			CAM_INFO(CAM_OIS, "cal_memory ng, ADDR: %04X R_DATA: %04X, U_DATA: %04X",
				g_recovery_addr_owen[i], read_data, user_flash);
			*rewrite_flag = 1;
			break;
		}
	}
}

static void owen_rewrite(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data, uint16_t rewrite_flag,
	unsigned char act_type)
{
	int32_t i, r_index;
	if (rewrite_flag == 0) {
		CAM_INFO(CAM_OIS, "INI memory check pass");
		return;
	}
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7015, 0x0002); /* Servo Off */
	for (i = 0; i < OWEN_RECOVERY_SIZE; i++) {
		r_index = g_recovery_otp_index_owen[i];
		dw9781b_ois_write_addr16_value16(o_ctrl, g_recovery_addr_owen[i],
			(uint16_t)(ois_otp_data[r_index] << SHIFT8) +
				ois_otp_data[r_index + 1]);
	}
	if (act_type == 0x01) { /* TDK */
		for (i = 0; i < OWEN_PARAM_SIZE; i++)
			dw9781b_ois_write_addr16_value16(o_ctrl, g_read_addr_owen[i], g_ref_tdk_owen[i]);
	} else if (act_type == 0x02) { /* MTM */
		for (i = 0; i < OWEN_PARAM_SIZE; i++)
			dw9781b_ois_write_addr16_value16(o_ctrl, g_read_addr_owen[i], g_ref_mtm_owen[i]);
	}
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7070,
		(uint16_t)(ois_otp_data[11] << SHIFT8) + ois_otp_data[15]);
	dw_calibration_save(o_ctrl);
	CAM_INFO(CAM_OIS, "owen recovery succ");
}

static void owen_recovery(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data,
	int32_t ois_otp_num, unsigned char act_type)
{
	int32_t i;
	uint16_t rewrite_flag = 0;
	uint16_t check_mtm = 0;
	uint16_t check_v3 = 0;
	uint16_t check_v4 = 0;
	uint16_t check_tdk = 0;
	uint16_t read_pid[OWEN_PID_SIZE] = {};
	uint16_t read_dat[OWEN_PARAM_SIZE] = {};

	if (ois_otp_num < OWEN_RECOVERY_OTP_SIZE)
		return;

	owen_flash_check(ois_otp_data, &rewrite_flag);
	/* MTM actuator PID check, v0201 */
	for (i = 0; i < OWEN_PID_SIZE; i++) {
		dw9781b_ois_read_addr16_value16(o_ctrl, g_pid_addr_owen[i], read_pid + i);
		if (read_pid[i] == g_mtm_pid_owen[i])
			check_mtm += 1;
		if (read_pid[i] == g_mtm_v3_pid_owen[i])
			check_v3 += 1;
		if (read_pid[i] == g_mtm_v4_pid_owen[i])
			check_v4 += 1;
		if (read_pid[i] == g_tdk_pid_owen[i])
			check_tdk += 1;
	}
	/* calibration fixed data check */
	for (i = 0; i < OWEN_PARAM_SIZE; i++)
		dw9781b_ois_read_addr16_value16(o_ctrl, g_read_addr_owen[i], read_dat + i);
	g_ref_tdk_owen[4] = (uint16_t)(ois_otp_data[11] << 8) + ois_otp_data[15];
	g_ref_mtm_owen[4] = (uint16_t)(ois_otp_data[11] << 8) + ois_otp_data[15];
	if (act_type == 0x01) { /* TDK */
		if (check_tdk < 8)
			rewrite_flag = 1;
		for (i = 0; i < OWEN_PARAM_SIZE; i++) {
			if (g_ref_tdk_owen[i] != read_dat[i]) {
				CAM_INFO(CAM_OIS, "cal_memory ng ADDR: %04X R_DATA: %04X, REF_DATA: %04X",
					g_read_addr_owen[i], read_dat[i], g_ref_tdk_owen[i]);
				rewrite_flag = 1;
				break;
			}
		}
	} else if (act_type == 0x02) { /* MTM */
		if (check_v3 <= 2 && check_v4 <= 2 && check_mtm < 8) /* Error limit */
			rewrite_flag = 1;
		for (i = 0; i < OWEN_PARAM_SIZE; i++) {
			if ((g_ref_mtm_owen[i] != read_dat[i]) && (check_mtm > 2)) { /* Error limit */
				CAM_INFO(CAM_OIS, "cal_memory ng ADDR: %04X R_DATA: %04X, REF_DATA: %04X",
					g_read_addr_owen[i], read_dat[i], g_ref_mtm_owen[i]);
				rewrite_flag = 1;
				break;
			}
		}
	}
	owen_rewrite(o_ctrl, ois_otp_data, rewrite_flag, act_type);
}

static void nolan_flash_check(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data, uint16_t *rewrite_flag)
{
	int32_t i, r_index;
	uint16_t read_data = 0;
	uint16_t user_flash;

	for (i = 0; i < NOLAN_RECOVERY_SIZE; i++) {
		r_index = g_recovery_otp_index_nolan[i];
		dw9781b_ois_read_addr16_value16(o_ctrl, g_recovery_addr_nolan[i], &read_data);
		user_flash = (uint16_t)(ois_otp_data[r_index] << SHIFT8) +
			ois_otp_data[r_index + 1];
		if ((g_recovery_addr_nolan[i] != 0x70F8) &&
			(g_recovery_addr_nolan[i] != 0x70F9) &&
			(g_recovery_addr_nolan[i] != 0x7075) &&
			(g_recovery_addr_nolan[i] != 0x7076) &&
			(read_data != user_flash)) {
			CAM_INFO(CAM_OIS, "cal_memory ng, ADDR: %04X R_DATA: %04X, U_DATA: %04X",
				g_recovery_addr_nolan[i], read_data, user_flash);
			*rewrite_flag = 1;
			break;
		}
	}
}

static void nolan_rewrite(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data,
	uint16_t rewrite_flag, unsigned char act_type)
{
	int32_t i, r_index;
	if (rewrite_flag == 0) {
		CAM_INFO(CAM_OIS, "INI memory check pass");
		return;
	}
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7015, 0x0002); /* Servo Off */

	for (i = 0; i < NOLAN_RECOVERY_SIZE; i++) {
		r_index = g_recovery_otp_index_nolan[i];
		dw9781b_ois_write_addr16_value16(o_ctrl, g_recovery_addr_nolan[i],
			(uint16_t)(ois_otp_data[r_index] << SHIFT8) +
			ois_otp_data[r_index + 1]);
	}
	/* compensation status */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7384, 0x8000); /* Linearity_status */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x73A3, 0x8000); /* crosstalk_status */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x73C2, 0x0000); /* posture_status */
	/* posture */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x73C3, 0x4000); /* POSTURE_1G */
	/* gyro gain */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x70FA,
		(uint16_t)(ois_otp_data[44] << 8) + ois_otp_data[45]); /* X_Gyro_Gain */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x70FB,
		(uint16_t)(ois_otp_data[46] << 8) + ois_otp_data[47]); /* Y_Gyro_Gain */
	if (ois_otp_data[1] == 0x01) { /* TDK */
		if (act_type == 0x0F)
			/* TDK Module Info */
			dw9781b_ois_write_addr16_value16(o_ctrl, 0x7004, 0x0000); /* 0x4600: 0x0F */
		else if (act_type == 0x0E)
			dw9781b_ois_write_addr16_value16(o_ctrl, 0x7004, 0x0100); /* 0x4606: 0x0E */
		for (i = 1; i < NOLAN_PARAM_SIZE; i++)
			dw9781b_ois_write_addr16_value16(o_ctrl, g_read_addr_nolan[i],
				g_ref_tdk_nolan[i]);
	} else if (ois_otp_data[1] == 0x02) { /* MTM */
		/* MTM Module Info */
		for (i = 0; i < NOLAN_PARAM_SIZE; i++)
			dw9781b_ois_write_addr16_value16(o_ctrl, g_read_addr_nolan[i],
				g_ref_mtm_nolan[i]);
	}
	dw_calibration_save(o_ctrl);
	CAM_INFO(CAM_OIS, "nolan recovery succ");
}

static void nolan_recovery(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data,
	int32_t ois_otp_num, unsigned char act_type)
{
	int32_t i;
	uint16_t rewrite_flag = 0;
	uint16_t read_dat[NOLAN_PARAM_SIZE] = {};

	if (ois_otp_num < NOLAN_RECOVERY_OTP_SIZE)
		return;

	nolan_flash_check(ois_otp_data, &rewrite_flag);
	/* calibration fixed data check */
	for (i = 0; i < NOLAN_PARAM_SIZE; i++)
		dw9781b_ois_read_addr16_value16(o_ctrl, g_read_addr_nolan[i], read_dat + i);

	if (ois_otp_data[1] == 0x01) { /* TDK */
		if (read_dat[0] != 0x0000 && read_dat[0] != 0x0100) {
			CAM_INFO(CAM_OIS, "cal_memory ng actuator type NG ADDR: %04X R_DATA: %04X",
				g_read_addr_nolan[0], read_dat[0]);
			rewrite_flag = 1;
		}
		for (i = 1; i < NOLAN_PARAM_SIZE; i++) {
			if (read_dat[i] != g_ref_tdk_nolan[i]) {
				CAM_INFO(CAM_OIS, "dw9781 ADDR: %04X R_DATA: %04X, REF_DATA: %04X",
					g_read_addr_nolan[i], read_dat[i], g_ref_tdk_nolan[i]);
				rewrite_flag = 1;
				break;
			}
		}
	} else if (ois_otp_data[1] == 0x02) { /* MTM */
		if (read_dat[0] != 0x0001 && read_dat[0] != 0x0101) {
			CAM_INFO(CAM_OIS, "dw9781 actuator type NG ADDR: %04X R_DATA: %04X",
				g_read_addr_nolan[0], read_dat[0]);
			rewrite_flag = 1;
		}
		for (i = 1; i < NOLAN_PARAM_SIZE; i++) {
			if (read_dat[i] != g_ref_mtm_nolan[i]) {
				CAM_INFO(CAM_OIS, "dw9781 ADDR: %04X R_DATA: %04X, REF_DATA: %04X",
					g_read_addr_nolan[i], read_dat[i], g_ref_mtm_nolan[i]);
				rewrite_flag = 1;
				break;
			}
		}
	}
	nolan_rewrite(o_ctrl, ois_otp_data, rewrite_flag, act_type);
}

static void meigui_flash_check(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data, uint16_t *rewrite_flag)
{
	int32_t i, r_index;
	uint16_t read_data = 0;
	uint16_t user_flash;

	for (i = 0; i < MEIGUI_RECOVERY_SIZE; i++) {
		r_index = g_recovery_otp_index_meigui[i];
		dw9781b_ois_read_addr16_value16(o_ctrl, g_recovery_addr_meigui[i], &read_data);
		user_flash = (uint16_t)(ois_otp_data[r_index] << SHIFT8) +
			ois_otp_data[r_index + 1];
		if ((g_recovery_addr_meigui[i] != 0x70F8) && /* gyro offset x */
			(g_recovery_addr_meigui[i] != 0x70F9) && /* gyro offset y */
			(g_recovery_addr_meigui[i] != 0x7075) && /* lens offset x */
			(g_recovery_addr_meigui[i] != 0x7076) && /* lens offset y */
			(read_data != user_flash)) {
			CAM_INFO(CAM_OIS, "cal_memory ng, ADDR: %04X R_DATA: %04X, U_DATA: %04X",
				g_recovery_addr_meigui[i], read_data, user_flash);
			*rewrite_flag = 1;
			break;
		}
	}
}

static void meigui_rewrite(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data, uint16_t rewrite_flag)
{
	int32_t i, r_index;
	if (rewrite_flag == 0) {
		CAM_INFO(CAM_OIS, "INI memory check pass");
		return;
	}
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7015, 0x0002); /* Servo Off */

	for (i = 0; i < MEIGUI_RECOVERY_SIZE; i++) {
		r_index = g_recovery_otp_index_meigui[i];
		dw9781b_ois_write_addr16_value16(o_ctrl, g_recovery_addr_meigui[i],
			(uint16_t)(ois_otp_data[r_index] << SHIFT8) +
				ois_otp_data[r_index + 1]);
	}
	/* compensation status */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7384, 0x8000); /* Linearity_status */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x73A3, 0x8000); /* crosstalk_status */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x73C2, 0x0000); /* posture_status */
	/* posture */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x73C3, 0x4000); /* POSTURE_1G */
	/* gyro gain */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x70FA,
		(uint16_t)(ois_otp_data[44] << 8) + ois_otp_data[45]); /* X_Gyro_Gain */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x70FB,
		(uint16_t)(ois_otp_data[46] << 8) + ois_otp_data[47]); /* Y_Gyro_Gain */
	for (i = 0; i < MEIGUI_PARAM_SIZE; i++)
		dw9781b_ois_write_addr16_value16(o_ctrl, g_read_addr_meigui[i],
			g_ref_tdk_meigui[i]);
	dw_calibration_save(o_ctrl,);
	CAM_INFO(CAM_OIS, "meigui recovery succ");
}

static void meigui_recovery(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data,
	int32_t ois_otp_num, unsigned char supplier_type)
{
	int32_t i;
	uint16_t rewrite_flag = 0;
	uint16_t read_dat[MEIGUI_PARAM_SIZE] = {};
	unsigned char ois_otp_data_real[MEIGUI_RECOVERY_OTP_SIZE];
	memset_s(ois_otp_data_real, sizeof(ois_otp_data_real), 0, sizeof(ois_otp_data_real));

	if (ois_otp_num < MEIGUI_RECOVERY_OTP_SIZE)
		return;

	loge_if_ret_void(memcpy_s(ois_otp_data_real, sizeof(ois_otp_data_real),
		ois_otp_data, sizeof(ois_otp_data_real)));

	if (supplier_type == 0x01) { /* sunny */
		CAM_INFO(CAM_OIS, "suuny ois, need to revert some bytes");
		for (i = MEIGUI_SUNNY_LIN_START_ADDR;
			i < MEIGUI_SUNNY_LIN_END_ADDR; i += 4) {
			ois_otp_data_real[i] = ois_otp_data[i + 2];
			ois_otp_data_real[i + 1] = ois_otp_data[i + 3];
			ois_otp_data_real[i + 2] = ois_otp_data[i];
			ois_otp_data_real[i + 3] = ois_otp_data[i + 1];
		}
	}

	meigui_flash_check(ois_otp_data_real, &rewrite_flag);
	/* calibration fixed data check */
	for (i = 0; i < MEIGUI_PARAM_SIZE; i++)
		dw9781b_ois_read_addr16_value16(o_ctrl, g_read_addr_meigui[i], read_dat + i);

	for (i = 0; i < MEIGUI_PARAM_SIZE; i++) {
		if (read_dat[i] != g_ref_tdk_meigui[i]) {
			CAM_INFO(CAM_OIS, "dw9781 ADDR: %04X R_DATA: %04X, REF_DATA: %04X",
				g_read_addr_meigui[i], read_dat[i], g_ref_tdk_meigui[i]);
			rewrite_flag = 1;
			break;
		}
	}
	meigui_rewrite(o_ctrl, ois_otp_data_real, rewrite_flag);
}

static int32_t check_fw_update_item(struct cam_ois_ctrl_t *o_ctrl, unsigned char fw_project)
{
	uint16_t r_reg = 0;
	unsigned char actuator_id;
	unsigned char actuator_version;
	uint16_t r_dat = 0;
	int32_t rewrite_flag = 0;
	int32_t i;

	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7015, 0x0002); /* servo off */
	dw9781b_ois_read_addr16_value16(o_ctrl, 0x7004, &r_reg);
	actuator_id = (unsigned char)(r_reg & 0xFF); /* Actuator ID */
	/* Actuator Version */
	actuator_version = (unsigned char)((r_reg >> SHIFT8) & 0xFF);
	dw_act_version_update(fw_project, actuator_version);

	if (fw_project < (sizeof(g_update_table) / sizeof(g_update_table[0])) &&
		actuator_id < ACTUATOR_NUM_MAX && fw_project < PSHUIXIANHUA) {
		for (i = 0; i < UPDATE_ITEM_LENGTH; i++) {
			dw9781b_ois_read_addr16_value16(o_ctrl, g_update_item_addr[i], &r_dat);
			if (r_dat != g_update_item[g_update_table[fw_project][actuator_id]][i]) {
				rewrite_flag = 1;
				CAM_INFO(CAM_OIS, "check_fw_update_item ADDR: %04X R_DATA: %04X, U_DATA: %04X",
					g_update_item_addr[i], r_dat,
					g_update_item[g_update_table[fw_project][actuator_id]][i]);
				break;
			}
		}
		if (rewrite_flag) {
			for (i = 0; i < UPDATE_ITEM_LENGTH; i++)
				dw9781b_ois_write_addr16_value16(o_ctrl, g_update_item_addr[i],
					g_update_item[g_update_table[fw_project][actuator_id]][i]);
			CAM_INFO(CAM_OIS, "dw9781 g_update_item memory abnormality re-setting succ");
		} else {
			CAM_INFO(CAM_OIS, "dw9781 check fw update item pass");
		}
	}
	return rewrite_flag;
}

static int32_t check_set_gyro_select(struct cam_ois_ctrl_t *o_ctrl,
	unsigned char fw_project, unsigned char gyro_info)
{
	uint16_t dat = 0;
	int32_t rewrite_flag = 0;
	int32_t i;

	dw9781b_ois_read_addr16_value16(o_ctrl, 0x7012, &dat); /* add gyro select */
	if (dat != gyro_info)
		rewrite_flag = 1;

	if (fw_project == PNOLAN) {
		for (i = 0; i < GYRO_BIQAUD_LENGTH; i++) {
			dw9781b_ois_read_addr16_value16(o_ctrl, g_gyro_biqaud_addr[i], &dat);
			if (dat != g_gyro_biqaud[g_gyro_biqaud_table[fw_project][gyro_info]][i]) {
				rewrite_flag = 1;
				CAM_INFO(CAM_OIS, "gyro_selection ADDR: %04X R_DATA: %04X, U_DATA: %04X",
					g_gyro_biqaud_addr[i], dat,
					g_gyro_biqaud[g_gyro_biqaud_table[fw_project][gyro_info]][i]);
				break;
			}
		}
	}
	if (rewrite_flag) {
		dw9781b_ois_write_addr16_value16(o_ctrl, 0x7012, gyro_info); /* add gyro select */
		if (fw_project == PNOLAN) {
			for (i = 0; i < GYRO_BIQAUD_LENGTH; i++)
				dw9781b_ois_write_addr16_value16(o_ctrl, g_gyro_biqaud_addr[i],
					g_gyro_biqaud[g_gyro_biqaud_table[fw_project][gyro_info]][i]);
		}
		CAM_INFO(CAM_OIS, "dw9781 gyro selection memory abnormality re-setting succ");
	} else {
		CAM_INFO(CAM_OIS, "dw9781 check gyro select pass");
	}
	return rewrite_flag;
}

void dw_gyro_direction_setting(struct cam_ois_ctrl_t *o_ctrl,
	int32_t gyro_arrangement, int32_t gyro_degree)
{
	uint16_t reg_arrangement = 0; /* 0x710E, gyro position (0, 1) */
	/* 0x710F gyro rotation degree (0, 90, 180, 270) */
	uint16_t reg_gyro_deg = 0;
	int32_t upd_flag = 0;

	CAM_INFO(CAM_OIS, "dw9781 gyro direction setting starting %d %d",
		gyro_arrangement, gyro_degree);

	dw9781b_ois_read_addr16_value16(o_ctrl, 0x710E, &reg_arrangement);
	dw9781b_ois_read_addr16_value16(o_ctrl, 0x710F, &reg_gyro_deg);

	if ((gyro_arrangement != reg_arrangement) ||
		(gyro_degree != reg_gyro_deg)) {
		upd_flag = 1;
		/* gyro position  set */
		dw9781b_ois_write_addr16_value16(o_ctrl, 0x710E, gyro_arrangement);
		/* gyro angle rotation set */
		dw9781b_ois_write_addr16_value16(o_ctrl, 0x710F, gyro_degree);
		CAM_INFO(CAM_OIS, "dw9781 Start changing the gyro direction");
	} else {
		upd_flag = 0;
		CAM_INFO(CAM_OIS, "dw9781 Gyro direction is already set and not needed");
	}
	if (upd_flag == 1)
		gyro_update_setting(gyro_arrangement, gyro_degree);
	CAM_INFO(CAM_OIS, "dw9781 gyro direction setting finish");
}

static void check_gyro_pol(struct cam_ois_ctrl_t *o_ctrl,
	int32_t gyro_arrangement, int32_t *rewrite_flag)
{
	uint16_t ref_gyro_pol_x = 0;
	uint16_t ref_gyro_pol_y = 0;
	uint16_t read_gyro_pol_x;
	uint16_t read_gyro_pol_y;

	if (gyro_arrangement == GYRO_FRONT_LAYOUT) {
		/* Gyro Polarity - Front Side Layout */
		ref_gyro_pol_x = 0xFFFF;
		ref_gyro_pol_y = 0xFFFF;
	} else if (gyro_arrangement == GYRO_BACK_LAYOUT) {
		/* Gyro Polarity - Back Side Layout */
		ref_gyro_pol_x = 0x0001;
		ref_gyro_pol_y = 0xFFFF;
	}

	/* check gyro polarity */
	dw9781b_ois_read_addr16_value16(o_ctrl, 0x71DA, &read_gyro_pol_x);
	dw9781b_ois_read_addr16_value16(o_ctrl, 0x71DB, &read_gyro_pol_y);
	if (read_gyro_pol_x != ref_gyro_pol_x || read_gyro_pol_y != ref_gyro_pol_y)
		*rewrite_flag = 1;
}

static int32_t check_gyro_direction_setting(struct cam_ois_ctrl_t *o_ctrl,
	unsigned char fw_project,
	int32_t gyro_arrangement, int32_t gyro_degree)
{
	unsigned char actuator_id;
	uint16_t gyro_id = 0;
	uint16_t reg_arrangement = 0; /* 0x710E, gyro position (0, 1) */
	uint16_t reg_gyro_deg = 0; /* gyro rotation degree 0, 90, 180, 270 */
	int32_t rewrite_flag;
	int32_t i, cnt;
	uint16_t ref_gyro_mat[GYRO_SIZE] = {};
	uint16_t read_gyro_mat[GYRO_SIZE];
	uint16_t gyro_degree_origin = gyro_degree;

	dw9781b_ois_read_addr16_value16(o_ctrl, 0x710E, &reg_arrangement);
	dw9781b_ois_read_addr16_value16(o_ctrl, 0x710F, &reg_gyro_deg);
	if ((reg_arrangement != gyro_arrangement) || (reg_gyro_deg != gyro_degree))
		rewrite_flag = 1;
	else
		rewrite_flag = 0;
	check_gyro_pol(o_ctrl, gyro_arrangement, &rewrite_flag);
	dw9781b_ois_read_addr16_value16(o_ctrl, 0x7012, &gyro_id); /* Gyro ID */
	actuator_id = get_actuator_id(fw_project);

	if (gyro_id == 6) /* Bosch Gryo */
		gyro_degree = (gyro_degree + GYRO_DEGREE_90) % GYRO_DEGREE_360;
	if (gyro_arrangement == GYRO_FRONT_LAYOUT && actuator_id == ACT_MTM)
		gyro_degree = (gyro_degree + GYRO_DEGREE_90) % GYRO_DEGREE_360;
	CAM_INFO(CAM_OIS, "dw9781 bosch gyro_degree %d", gyro_degree);
	cnt = (gyro_degree / GYRO_DEGREE_90) % GYRO_DEGREE_NUM;
	if (actuator_id == ACT_TDK) {
		for (i = 0; i < GYRO_SIZE; i++)
			ref_gyro_mat[i] = g_gyro_degree_tdk[cnt][i];
	} else if (actuator_id == ACT_MTM) {
		for (i = 0; i < GYRO_SIZE; i++)
			ref_gyro_mat[i] = g_gyro_degree_tdk[cnt][i];
	}
	/* check gyro direction */
	for (i = 0; i < GYRO_SIZE; i++) {
		dw9781b_ois_read_addr16_value16(o_ctrl, 0x7084 + i, read_gyro_mat + i);
		if (ref_gyro_mat[i] != read_gyro_mat[i])
			rewrite_flag = 1;
	}
	if (rewrite_flag) {
		/* Gyro position init 360 */
		dw9781b_ois_write_addr16_value16(o_ctrl, 0x710E, 0x0168);
		/* Gyro anlge rotation init 360 */
		dw9781b_ois_write_addr16_value16(o_ctrl, 0x710F, 0x0168);
		dw_gyro_direction_setting(o_ctrl, gyro_arrangement, gyro_degree_origin);
		CAM_INFO(CAM_OIS, "dw9781 gyro direction memory abnormality re-setting succ");
	} else {
		CAM_INFO(CAM_OIS, "dw9781 check gyro direction pass");
	}
	return rewrite_flag;
}

void dw9781_ois_recovery_memory(struct cam_ois_ctrl_t *o_ctrl,
	const unsigned char *ois_otp_data, int32_t ois_otp_num,
	unsigned char act_type, unsigned char supplier_type,
	unsigned char gyro_select, int32_t gyro_arrangement, int32_t gyro_degree)
{
	uint16_t r_reg = 0;
	unsigned char fw_project;
	int32_t check_ret = 0;
	dw9781b_ois_read_addr16_value16(o_ctrl, 0x7001, &r_reg); /* Project Info */
	fw_project = (unsigned char)((r_reg >> SHIFT8) & 0xFF);

	if (!ois_otp_data) {
		CAM_INFO(CAM_OIS, "ois_otp_data is NULL");
		return;
	}

	CAM_INFO(CAM_OIS, "dw9781 fw_project is 0x%x", fw_project);
	if (fw_project == POWEN) { /* owen */
		act_type = ois_otp_data[1];
		owen_recovery(o_ctrl, ois_otp_data, ois_otp_num, act_type);
	} else if (fw_project == PNOLAN) { /* nolan */
		nolan_recovery(o_ctrl, ois_otp_data, ois_otp_num, act_type);
	} else if (fw_project == PMEIGUI) { /* meigui */
		meigui_recovery(o_ctrl, ois_otp_data, ois_otp_num, supplier_type);
	} else { /* others */
		CAM_INFO(CAM_OIS, "not need recovery memory");
		return;
	}

	if (fw_project == POWEN || fw_project == PNOLAN) {
		check_ret += check_fw_update_item(o_ctrl, fw_project);
		check_ret += check_set_gyro_select(o_ctrl, fw_project, gyro_select);
	}
	check_ret += check_gyro_direction_setting(o_ctrl, fw_project,
		gyro_arrangement, gyro_degree);
	if (check_ret) {
		dw_calibration_save(o_ctrl);
		CAM_INFO(CAM_OIS, "dw9781 recovery succ");
		return;
	}
	CAM_INFO(CAM_OIS, "dw9781 recovery check pass");
}
