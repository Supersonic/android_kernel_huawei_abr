/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:lc898129 OIS driver
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
#include "ois_lc898129_fun.h"
#include "ois_lc898129.h"

void hs_io_write32(struct cam_ois_ctrl_t *o_ctrl, uint32_t io_adrs, uint32_t io_data)
{
	ois_write_addr16_value32(o_ctrl, CMD_IO_ADR_ACCESS, io_adrs);
	ois_write_addr16_value32(o_ctrl, CMD_IO_DAT_ACCESS, io_data);
}

void hs_io_read32(struct cam_ois_ctrl_t *o_ctrl, uint32_t io_adrs, uint32_t *io_data)
{
	ois_write_addr16_value32(o_ctrl, CMD_IO_ADR_ACCESS, io_adrs);
	ois_read_addr16_value32(o_ctrl, CMD_IO_DAT_ACCESS, io_data);
}

/********************************************************************************
	Function Name     : hs_get_osc_tgt_count
	Retun Value       : NON
	Argment Value     : NON
	Explanation       : Get Osc Target Count
********************************************************************************/
uint32_t hs_get_osc_tgt_count(struct cam_ois_ctrl_t *o_ctrl, uint16_t time_us)
{
	uint32_t ul_osc_tgt_count;
	/* 80MHz 7-Frequency Divider */
	ul_osc_tgt_count = (uint32_t)80 * (uint32_t)time_us / (uint32_t)7;
	CAM_INFO(CAM_OIS, "ul_osc_tgt_count = %08x \n", ul_osc_tgt_count);

	return (ul_osc_tgt_count); /* Not done */
}

/*********************************************************************************
	Function Name   : OscAdj
	Retun Value     : NON
	Argment Value   : NON
	Explanation     : OSC Clock adjustment
*********************************************************************************/
#if I2C_TIME_MODE == 1
#define I2C_CLOCK         370 /* I2C 370kbps */
/* #define I2C_CLOCK       400 I2C 400kbps */
/* I2C 400kbps : 16.25usec * 80MHz = 1300count. */
#define OSC_TGT_COUNT     (80000 * 6 / I2C_CLOCK)
#define DIFF_THRESHOLD    (OSC_TGT_COUNT * 625 / 100000) /* 7.5 --> 7 */
#else /* I2C_TIME_MODE */
#define OSC_ADJMES_TIME    4000 /* 4[msec] */
#endif /* I2C_TIME_MODE */

static uint8_t hs_osc_adj(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t *osc_value)
{
	int8_t i;
	uint32_t ul_trim = 0;
	uint32_t ul_trim_bak = 0;
	uint32_t ul_trim_bak2 = 0;
	uint32_t ul_ck_cnt, ul_ck_cnt2;
	uint32_t ul_dspdiv_bak;
	uint32_t ul_frq_diff, ul_frq_diff2;
	uint8_t uc_result = 0;
	uint32_t ulrominfo;
#if I2C_TIME_MODE == 1
#else /* I2C_TIME_MODE */
	uint32_t OSC_TGT_COUNT;
	uint32_t DIFF_THRESHOLD;
#endif /* I2C_TIME_MODE */

	/* Boot Sequence confirms */
	hs_io_read32(o_ctrl, ROMINFO, &ulrominfo); /* Vendor Reserved Status. */
	if ((ulrominfo != 0x0000000C) && (ulrominfo != 0x0000000B)) { /* 0xC? 0xB? */
		uc_result = 0x01; /* Error */
		return(uc_result);
	}
	hs_io_read32(o_ctrl, SYSDSP_DSPDIV, &ul_dspdiv_bak); /* DSP Clock Divide Setting. */

#if I2C_TIME_MODE == 1
	hs_io_write32(o_ctrl, SYSDSP_DSPDIV, 0x00000000); /* 80MHz(129)1/1 */
#else /* I2C_TIME_MODE */
	/* hs_io_write32(SYSDSP_DSPDIV, 0x00000004); 80MHz(129)1/4 */
	hs_io_write32(o_ctrl, SYSDSP_DSPDIV, 0x00000007); /* 80MHz(129)1/7 */
#endif /* I2C_TIME_MODE */

	/* Measurement loop starting */
	/* Soft Reset Setting : OSC counter = Normal */
	hs_io_write32(o_ctrl, SYSDSP_SOFTRES, 0x0003037F);
	/* Peripheral clock On/Off setting : OSC counter = on. */
	hs_io_write32(o_ctrl, PERICLKON, 0x0000005C);
	CAM_INFO(CAM_OIS, "Adj1 \n");
	/* OIS Measure Trim_SAR (NoTest) */
#if I2C_TIME_MODE == 1
	hs_io_write32(o_ctrl, OSCCNT, 0x00000021); /* I2C TIME MODE Enable */
#endif /* I2C_TIME_MODE */

	for (i = 8; i >= 0; i--) {
		ul_trim |= (uint32_t)(1 << i);
		CAM_INFO(CAM_OIS, "i = %d   FRQTRM = %08xh ", i, (int)ul_trim);
		hs_io_write32(o_ctrl, FRQTRM, ~ul_trim & 0x000001FF); /* ~trim */

#if I2C_TIME_MODE == 1
		/* Readout of measurement results */
		hs_io_read32(o_ctrl, OSCCKCNT, &ul_ck_cnt); /* OSC clock count. */
#else /* I2C_TIME_MODE */
		hsUcOscAdjFlg = MEASSTR; /* Start trigger ON */
		/* Readout of measurement results */
		hs_io_read32(o_ctrl, OSCCKCNT, &ul_ck_cnt); /* OSC clock count. */
		/* Calculate target count. */
		OSC_TGT_COUNT = hs_get_osc_tgt_count(o_ctrl, OSC_ADJMES_TIME);
#endif /* I2C_TIME_MODE */

		if (ul_ck_cnt >= OSC_TGT_COUNT)
			ul_trim -= (uint32_t)(1 << i);
		CAM_INFO(CAM_OIS, "cnt = %d \n", (int)ul_ck_cnt);
	}

	/* OSC Adj Trim_I (NoTest) */
	CAM_INFO(CAM_OIS, " Adj2 \n");
	ul_trim_bak = ul_trim;
	ul_trim_bak2 = (((ul_trim_bak & 0x00000070) >> 1) | (ul_trim_bak & 0x00000007));

	if (ul_trim_bak2 & 0x00000001) {
		if ((ul_trim_bak2 & 0x0000003F) != 0x0000003F) {
			ul_trim_bak2++;
			ul_trim = (ul_trim_bak & 0x00000180) | ((ul_trim_bak2 & 0x00000038) << 1)
				| (ul_trim_bak & 0x00000008) | (ul_trim_bak2 & 0x00000007);
			CAM_INFO(CAM_OIS, "1 : \n");
		} else {
			ul_trim = ul_trim_bak;
			CAM_INFO(CAM_OIS, "2 : \n");
		}
	} else {
		ul_trim = ul_trim_bak;
		ul_trim_bak++;
		CAM_INFO(CAM_OIS, "3 : \n");
	}

	hs_io_write32(o_ctrl, FRQTRM, ~ul_trim & 0x000001FF); /* ~trim */

#if I2C_TIME_MODE == 1
	/* Readout of measurement results */
	hs_io_read32(o_ctrl, OSCCKCNT, &ul_ck_cnt2); /* OSC clock count. */
#else /* I2C_TIME_MODE */
	hsUcOscAdjFlg = MEASSTR; /* Start trigger ON */
	/* Readout of measurement results */
	hs_io_read32(o_ctrl, OSCCKCNT, &ul_ck_cnt2); /* OSC clock count. */

	/* Calculate target count. */
	OSC_TGT_COUNT = hs_get_osc_tgt_count(o_ctrl, OSC_ADJMES_TIME);
#endif /* I2C_TIME_MODE */

	CAM_INFO(CAM_OIS, "Adj2-3 : \n");
	CAM_INFO(CAM_OIS, "cnt = %d \n", (int)ul_ck_cnt2);
	ul_frq_diff = abs(ul_ck_cnt - OSC_TGT_COUNT);
	ul_frq_diff2 = abs(ul_ck_cnt2 - OSC_TGT_COUNT);
	if (ul_frq_diff < ul_frq_diff2) {
		ul_trim = ul_trim_bak;
		CAM_INFO(CAM_OIS, " 4 : \n");
	}
	CAM_INFO(CAM_OIS, "ul_trim = %08xh \n", (int)ul_trim);
	hs_io_write32(o_ctrl, FRQTRM, ~ul_trim & 0x000001FF); /* ~trim */
	/* OSC Adj Verigy (NoTest) */
#if I2C_TIME_MODE == 1
	/* Readout of measurement results */
	hs_io_read32(o_ctrl, OSCCKCNT, &ul_ck_cnt); /* OSC clock count. */
#else /* I2C_TIME_MODE */
	hsUcOscAdjFlg = MEASSTR; /* Start trigger ON */
	/* Calculate target count. */
	OSC_TGT_COUNT = hs_get_osc_tgt_count(o_ctrl, OSC_ADJMES_TIME);

	/* Readout of measurement results */
	hs_io_read32(o_ctrl, OSCCKCNT, &ul_ck_cnt); /* OSC clock count. */
#endif /* I2C_TIME_MODE */

#if I2C_TIME_MODE == 1
	hs_io_write32(o_ctrl, OSCCNT, 0x00000000); /* I2C TIME MODE Disable */
#endif /* I2C_TIME_MODE */
	ul_frq_diff = abs(ul_ck_cnt - OSC_TGT_COUNT);
#if I2C_TIME_MODE == 1
#else /* I2C_TIME_MODE */
	DIFF_THRESHOLD = OSC_TGT_COUNT * (uint32_t)5 / (uint32_t)800; /* 80MHz ± 0.5MHz */
	CAM_INFO(CAM_OIS, " DIFF_THRESHOLD = %08x \n", DIFF_THRESHOLD);
#endif /* I2C_TIME_MODE */
	if (ul_frq_diff <= DIFF_THRESHOLD)
		uc_result = 0x00; /* OK */
	else
		uc_result = 0x02; /* NG */
	hs_io_write32(o_ctrl, SYSDSP_DSPDIV, ul_dspdiv_bak); /* DSP Clock Divide Setting. */
	*osc_value = (uint16_t)ul_trim;
	return(uc_result);
}

uint8_t unlock_code_set129(struct cam_ois_ctrl_t *o_ctrl)
{
	uint32_t read_val = 0;
	uint32_t ulcnt = 0;

	do {
		hs_io_write32(o_ctrl, 0xE07554, 0xAAAAAAAA); /* UNLK_CODE1(E0_7554h) = AAAA_AAAAh */
		hs_io_write32(o_ctrl, 0xE07AA8, 0x55555555); /* UNLK_CODE2(E0_7AA8h) = 5555_5555h */
		hs_io_read32(o_ctrl, 0xE07014, &read_val);
		if ((read_val & 0x00000080) != 0)
			return (SUCCESS); /* Check UNLOCK(E0_7014h[7]) ? */
		msleep(1);
	} while (ulcnt++ < 10);
	return (FAILURE);
}

void hs_write_permission129(struct cam_ois_ctrl_t *o_ctrl)
{
	hs_io_write32(o_ctrl, 0xE074CC, 0x00000001); /* RSTB_FLA_WR(E0_74CCh[0])= 1 */
	hs_io_write32(o_ctrl, 0xE07664, 0x00000010); /* FLA_WR_ON(E0_7664h[4])= 1 */
}

void addtional_unlock_code_set129(struct cam_ois_ctrl_t *o_ctrl)
{
	hs_io_write32(o_ctrl, 0xE07CCC, 0x0000ACD5); /* UNLK_CODE3(E0_7CCCh) = 0000_ACD5h */
}

void hs_cnt_write(struct cam_ois_ctrl_t *o_ctrl, uint8_t *data, int16_t len)
{
	uint16_t reg;

	if (!data || len == 0)
		return;

	reg = (data[0] << SHIFT_8BIT) | data[1];
	ois_i2c_block_write_reg_value8(o_ctrl, reg, &data[2], len - 2, 1);
}

uint8_t unlock_code_clear129(struct cam_ois_ctrl_t *o_ctrl)
{
	uint32_t ul_data_val = 0;
	uint32_t ulcnt = 0;

	do {
		hs_io_write32(o_ctrl, 0xE07014, 0x00000010); /* UNLK_CODE3(E0_7014h[4]) = 1 */
		hs_io_read32(o_ctrl, 0xE07014, &ul_data_val);
		if ((ul_data_val & 0x00000080) == 0)
			return (SUCCESS); /* Check UNLOCK(E0_7014h[7]) ? */
		msleep(1);
	} while (ulcnt++ < 10);
	return (FAILURE);
}

void hs_boot_mode(struct cam_ois_ctrl_t *o_ctrl)
{
	uint32_t read_val;
	hs_io_read32(o_ctrl, SYSDSP_REMAP, &read_val);
	read_val = (read_val & 0x1) | 0x00001400;
	hs_io_write32(o_ctrl, SYSDSP_REMAP, read_val); /* CORE_RST[12], MC_IGNORE2[10] = 1 */
	msleep(15); /* Wait 15ms */
}

static uint32_t hs_trim_adj(struct cam_ois_ctrl_t *o_ctrl, uint32_t src_trim)
{
	uint8_t byt_trim[4];
	uint8_t byt_sum = 0;
	uint32_t adj_trim = 0;

	byt_trim[0] = (uint8_t)(src_trim >> 24); /* MSB */
	byt_trim[1] = (uint8_t)(src_trim >> 16);
	byt_trim[2] = (uint8_t)(src_trim >> 8);
	byt_trim[3] = (uint8_t)(src_trim >> 0); /* LSB */

	byt_sum = (byt_trim[3] & 0x0F) + (byt_trim[3] >> 4)
		+ (byt_trim[2] & 0x0F) + (byt_trim[2] >> 4)
		+ (byt_trim[1] & 0x08) + (byt_trim[1] >> 4);
	byt_sum &= 0x0F;

	if (byt_trim[0] == 0x13) {
		/* correct trimed */
		adj_trim = src_trim;
	} else if ((byt_sum == (byt_trim[0] >> 4)) && (src_trim != 0x00000000)) {
		adj_trim = (uint32_t)(0x13000000)
			| ((uint32_t)byt_trim[1] << 16)
			| ((uint32_t)byt_trim[2] << 8)
			| ((uint32_t)byt_trim[3] << 0);
	} else {
		adj_trim = 0x13040000;
	}
	CAM_INFO(CAM_OIS, "src_trim = %08X, Checksum = %02X, adj_trim = %08X\n",
		src_trim, byt_sum, adj_trim);
	return (adj_trim);
}

static uint8_t hs_check_checkcode(struct cam_ois_ctrl_t *o_ctrl, uint16_t us_cver)
{
	uint32_t ul_check_code1;
	uint32_t ul_check_code2;
	uint32_t ul_trim_reg_val;
	uint32_t ul_adj_trim;

	hs_io_write32(o_ctrl, 0xE07008, 0x00000001); /* ACSCNT 0x01 + 1 = 2Word */

	if (us_cver == 0x0141) /* LC898129    ES1 */
		hs_io_write32(o_ctrl, 0xE0700C, 0x0004002E); /* FLA_ADR    Information Mat 2 Page 2 */
	else if ((us_cver == 0x0144) || (us_cver == 0x0145)) /* LC898129    ES2 or ES3 */
		hs_io_write32(o_ctrl, 0xE0700C, 0x0010001E); /* FLA_ADR    Trimming Mat */
	else
		return (0x02); /* Error */

	hs_io_write32(o_ctrl, 0xE07010, 0x00000001); /* CMD */
	hs_io_read32(o_ctrl, 0xE07000, &ul_check_code1); /* FLA_RDAT */
	hs_io_read32(o_ctrl, 0xE07000, &ul_check_code2); /* FLA_RDAT */

	hs_io_write32(o_ctrl, 0xE07008, 0x00000000); /* 1 word read */
	hs_io_write32(o_ctrl, 0xE074CC, 0x00000080); /* RSTB_FLA   Bit7 Don not change */
	hs_io_write32(o_ctrl, 0xE0700C, 0x00200000); /* FLA_ADR    User Trimming Mat */
	hs_io_write32(o_ctrl, 0xE07010, 0x00000001); /* CMD    Read */
	hs_io_read32(o_ctrl, 0xE07000, &ul_trim_reg_val); /* FLA_RDAT */
	ul_adj_trim = hs_trim_adj(o_ctrl, ul_trim_reg_val);
	if (ul_check_code1 == 0x99756768 && ul_check_code2
		== 0x01AC29AC && ul_trim_reg_val == ul_adj_trim)
		return (0x01); /* Done */

	return (0x00); /* Not done */
}

/*********************************************************************************
	Function Name     : hs_protect_release
	Retun Value       : NON
	Argment Value     : NON
	Explanation       : Protect Release
********************************************************************************/
static uint8_t hs_protect_release(struct cam_ois_ctrl_t *o_ctrl, uint8_t uc_mat)
{
	uint32_t ul_read_val;

	/* Protect Release */
	hs_io_write32(o_ctrl, 0xE07554, 0xAAAAAAAA); /* UNLK_CODE1 */
	hs_io_write32(o_ctrl, 0xE07AA8, 0x55555555); /* UNLK_CODE2 */
	hs_io_write32(o_ctrl, 0xE074CC, 0x00000001); /* RSTB_FLA */
	hs_io_write32(o_ctrl, 0xE07664, 0x00000010); /* CLK_FLAON */
	if ((uc_mat == INF_MAT0) || (uc_mat == INF_MAT1)
		|| (uc_mat == INF_MAT2)) { /* Information Mat0,1,2 */
		hs_io_write32(o_ctrl, 0xE07CCC, 0x0000C5AD); /* UNLK_CODE3 */
	} else if (uc_mat == TRIM_MAT) { /* Trimming Mat */
		hs_io_write32(o_ctrl, 0xE07CCC, 0x0000C5AD); /* UNLK_CODE3 */
		hs_io_write32(o_ctrl, 0xE07CCC, 0x00005B29); /* UNLK_CODE3 */
	} else {
		return (0x02); /* Error */
	}
	hs_io_write32(o_ctrl, 0xE07CCC, 0x0000ACD5); /* UNLK_CODE3 */
	hs_io_read32(o_ctrl, 0xE07014, &ul_read_val); /* FLAWP */
	if (((ul_read_val == 0x1185) && ((uc_mat == INF_MAT0)
		|| (uc_mat == INF_MAT1) || (uc_mat == INF_MAT2)))
		|| ((ul_read_val == 0x1187) && ((uc_mat == TRIM_MAT))))
		return (0x00); /* Success */
	else
		return (0x01); /* Error */
}

/*********************************************************************************
	Function Name     : hs_flash_information_mat_erase
	Retun Value       : NON
	Argment Value     : NON
	Explanation       : Flash Information Mat Erase
*********************************************************************************/
static uint8_t hs_flash_information_mat_erase(struct cam_ois_ctrl_t *o_ctrl, uint8_t uc_mat)
{
	uint32_t ul_read_val;
	uint32_t ul_cnt = 0;

	/* Flash Information Mat erase */
	switch (uc_mat) {
	case INF_MAT0:
		hs_io_write32(o_ctrl, 0xE0700C, 0x00010000); /* FLA_ADR Information Mat 0 */
		break;
	case INF_MAT1:
		hs_io_write32(o_ctrl, 0xE0700C, 0x00020000); /* FLA_ADR Information Mat 1 */
		break;
	case INF_MAT2:
		hs_io_write32(o_ctrl, 0xE0700C, 0x00040000); /* FLA_ADR Information Mat 2 */
		break;
	case TRIM_MAT:
		hs_io_write32(o_ctrl, 0xE0700C, 0x00100000); /* FLA_ADR Trimming Mat */
		break;
	default:
		return (0x01); /* Error */
		break;
	}

	msleep(1);
	hs_io_write32(o_ctrl, 0xE07010, 0x00000004); /* CMD */
	msleep(8);
	do {
		hs_io_read32(o_ctrl, 0xE07018, &ul_read_val); /* FLAINT */
		if ((ul_read_val & 0x80) == 0x00)
			return (0x00); /* Success */
		msleep(1);
	} while (ul_cnt++ < 10);
	return (0x01); /* Error */
}

/*********************************************************************************
	Function Name     : hs_flash_information_mat_program
	Retun Value       : NON
	Argment Value     : NON
	Explanation       : Flash Information Mat Program
*********************************************************************************/
static uint8_t hs_flash_information_mat_program(struct cam_ois_ctrl_t *o_ctrl,
	uint32_t *ul_write_val, uint32_t ul_fla_adr)
{
	uint32_t ul_read_val;
	uint32_t ul_cnt = 0;
	int16_t i;

	/* Flash Information Mat 2 Page 2 Program */
	hs_io_write32(o_ctrl, 0xE0700C, ul_fla_adr);
	/* FLA_ADR      Information Mat 2 Page 2 */
	/* 0x00040000 : Information Mat 2 Page 0 */
	/* 0x00040010 : Information Mat 2 Page 1 */
	/* 0x00040020 : Information Mat 2 Page 2 */
	/* 0x00040030 : Information Mat 2 Page 3 */
	/* 0x00100000 : Trimming Mat      Page 0 */
	/* 0x00100010 : Trimming Mat      Page 1 */
	hs_io_write32(o_ctrl, 0xE07010, 0x00000002); /* CMD */
	for (i = 0; i < 16; i++)
		hs_io_write32(o_ctrl, 0xE07004, ul_write_val[i]); /* FLA_WDAT */
	do {
		hs_io_read32(o_ctrl, 0xE07018, &ul_read_val); /* FLAINT */
		if ((ul_read_val & 0x80) == 0)
			break; /* Success */
		msleep(1);
	} while (ul_cnt++ < 10);

	hs_io_write32(o_ctrl, 0xE07010, 0x00000008); /* CMD  PROGRAM (Page Program) */

	msleep(3);
	ul_cnt = 0;
	do {
		hs_io_read32(o_ctrl, 0xE07018, &ul_read_val); /* FLAINT */
		CAM_INFO(CAM_OIS, "hs_flash_information_mat_program : ul_read_val = %02x \n",
			ul_read_val);
		if ((ul_read_val & 0x80) == 0)
			return (0x00); /* Success */
		msleep(1);
	} while (ul_cnt++ < 10);

	return (1); /* Error */
}
/*********************************************************************************
	Function Name      : Protect
	Retun Value        : NON
	Argment Value      : NON
	Explanation        : Protect
********************************************************************************/
static uint8_t hs_protect(struct cam_ois_ctrl_t *o_ctrl)
{
	uint32_t ul_read_val;

	/* Protect */
	hs_io_write32(o_ctrl, 0xE07014, 0x00000010); /* FLAWP */
	hs_io_read32(o_ctrl, 0xE07014, &ul_read_val); /* FLAWP */
	if (ul_read_val == 0x0000)
		return (0x00); /* Success */
	else
		return (0x01); /* Error */
}

/*********************************************************************************
	Function Name     : hs_verify_read
	Retun Value       : NON
	Argment Value     : NON
	Explanation       : Verify Read
********************************************************************************/
static uint8_t hs_verify_read(struct cam_ois_ctrl_t *o_ctrl,
	uint32_t *ul_write_val, uint32_t ul_fla_adr)
{
	int16_t i;
	uint32_t ul_read_val;

	hs_io_write32(o_ctrl, 0xE07008, 0x0000000F); /* ACSCNT    0x0F + 1 = 16Word */

	/* hs_io_write32(0xE0700C, 0x00040020);  FLA_ADR  Information Mat 2 Page 2 */
	hs_io_write32(o_ctrl, 0xE0700C, ul_fla_adr); /* FLA_ADR */

	hs_io_write32(o_ctrl, 0xE07010, 0x00000001); /* CMD */

	for (i = 0; i < 16; i++) {
		hs_io_read32(o_ctrl, 0xE07000, &ul_read_val); /* FLA_RDAT */
		if (ul_read_val != ul_write_val[i])
			return (0x01); /* Error */
	}
	return (0x00); /* Success */
}

/*********************************************************************************
	Function Name     : hs_write_osc_adj_val
	Retun Value       : NON
	Argment Value     : NON
	Explanation       : Write Osc adjustment value
********************************************************************************/
#define GAIN_XP        0x0B /* TRM.12h[15:12]    INF2.22h[15:12] */
#define GAIN_XM        0x0B /* TRM.12h[31:28]    INF2.22h[31:28] */
#define GAIN_YP        0x0B /* TRM.13h[15:12]    INF2.23h[15:12] */
#define GAIN_YM        0x0B /* TRM.13h[31:28]    INF2.23h[31:28] */
#define GAIN_AFP       0x09 /* TRM.14h[15:12]    INF2.24h[15:12] */
#define GAIN_AFM       0x09 /* TRM.14h[31:28]    INF2.24h[31:28] */
#define GAIN_VAL       0x0B

#define OFST_XP        0x40
#define OFST_XM        0x40
#define OFST_YP        0x40
#define OFST_YM        0x40
#define OFST_AFP       0x0100
#define OFST_AFM       0x0100
#define OFST_VAL       0x40

static uint8_t hs_write_osc_adj_val(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t us_osc_val, uint16_t us_cver)
{
	uint16_t us_result;
	uint32_t ul_write_val0[16];
	uint32_t ul_write_val1[16];
	uint32_t ul_checksum_val;
	uint16_t i;
	uint32_t ul_trim_reg_val;
	uint32_t ul_adj_trim;

	hs_io_write32(o_ctrl, SYSDSP_DSPDIV, 0x00000001); /* 80MHz(129)1/1 */

	if ((us_cver == 0x0144) || (us_cver == 0x0145)) { /* LC898129 ES2 */
		hs_io_write32(o_ctrl, 0xE07008, 0x00000000); /* 1 word read */
		hs_io_write32(o_ctrl, 0xE074CC, 0x00000080); /* RSTB_FLA    Bit7 Don not change */
		hs_io_write32(o_ctrl, 0xE0700C, 0x00200000); /* FLA_ADR    User Trimming Mat */
		msleep(1); /* 20usec */
		hs_io_write32(o_ctrl, 0xE07010, 0x00000001); /* CMD Read */
		hs_io_read32(o_ctrl, 0xE07000, &ul_trim_reg_val); /* FLA_RDAT */

		us_result = hs_protect_release(o_ctrl, TRIM_MAT); /* TRIM_MAT */
		if (us_result != 0x00)
			return (0x01); /* Error */

		/* Adjustment trimming data */
		ul_adj_trim = hs_trim_adj(o_ctrl, ul_trim_reg_val);
		hs_io_write32(o_ctrl, 0xE074CC, 0x00000081);
		hs_io_write32(o_ctrl, 0xE0701C, 0x00000000);
		hs_io_write32(o_ctrl, 0xE0700C, 0x00200000);
		hs_io_write32(o_ctrl, 0xE07010, 0x00000002);
		hs_io_write32(o_ctrl, 0xE07004, ul_adj_trim);
		hs_io_write32(o_ctrl, 0xE07010, 0x00000001);
		hs_io_read32(o_ctrl, 0xE07000, &ul_trim_reg_val);
		CAM_INFO(CAM_OIS, "ul_trim_reg_val= %08X\n", ul_trim_reg_val);

		/* Info Mat0 Erase */
		us_result = hs_flash_information_mat_erase(o_ctrl, INF_MAT0);
		if (us_result != 0x00)
			return (0x02); /* Error */

		/* Info Mat1 Erase */
		us_result = hs_flash_information_mat_erase(o_ctrl, INF_MAT1);
		if (us_result != 0x00)
			return (0x02); /* Error */

		/* Info Mat2 Erase */
		us_result = hs_flash_information_mat_erase(o_ctrl, INF_MAT2);
		if (us_result != 0x00)
			return (0x02); /* Error */

		/* Flash Trimming Mat Erase */
		us_result = hs_flash_information_mat_erase(o_ctrl, TRIM_MAT);
		if (us_result != 0x00)
			return (0x02); /* Error */

		/* Data */
		ul_write_val0[0] = ul_trim_reg_val;
		ul_write_val0[1] = ~ul_trim_reg_val;
		for (i = 2; i <= 15; i++) /* 14 times */
			ul_write_val0[i] = 0xFFFFFFFF;

		/* Flash Trimming Mat Page 0 Program */
		us_result = hs_flash_information_mat_program(o_ctrl, ul_write_val0, 0x00100000);
		if (us_result != 0x00)
			return (0x03); /* Error */
		/* Data */
		ul_write_val1[0] = 0x55FFFFFF;
		/* { 0000 000b, OscAdjVal[8], OscAdjVal[7:0], 8F00h }    ~ul_trim & 0x000001FF */
		ul_write_val1[1] = 0x0000AF00 | ((uint32_t)(~us_osc_val & 0x01FF) << 16);
		ul_write_val1[2] = 0x0F000F00 | ((uint32_t)GAIN_XM  << 28) | ((uint32_t)OFST_XM  << 16)
			| ((uint32_t)GAIN_XP  << 12) | ((uint32_t)0x3F); /* { GAIN_VAL[3:0], FFFh, GAIN_VAL[3:0], FFFh } */
		ul_write_val1[3] = 0x0F000F00 | ((uint32_t)GAIN_YM  << 28) | ((uint32_t)OFST_YM  << 16)
			| ((uint32_t)GAIN_YP  << 12) | ((uint32_t)0x3F); /* { GAIN_VAL[3:0], FFFh, GAIN_VAL[3:0], FFFh } */
		ul_write_val1[4] = 0x00000000 | ((uint32_t)GAIN_AFM << 28) | ((uint32_t)OFST_AFM << 16)
			| ((uint32_t)GAIN_AFP << 12) | ((uint32_t)OFST_AFP); /* { GAIN_VAL[3:0], FFFh, GAIN_VAL[3:0], FFFh } */
		ul_write_val1[5] = 0x0F000F00 | ((uint32_t)GAIN_VAL << 28) | ((uint32_t)OFST_VAL << 16)
			| ((uint32_t)GAIN_VAL << 12) | ((uint32_t)OFST_VAL); /* { GAIN_VAL[3:0], FFFh, GAIN_VAL[3:0], FFFh } */
		ul_write_val1[6] = 0x0F000F00 | ((uint32_t)GAIN_VAL << 28) | ((uint32_t)OFST_VAL << 16)
			| ((uint32_t)GAIN_VAL << 12) | ((uint32_t)OFST_VAL); /* { GAIN_VAL[3:0], FFFh, GAIN_VAL[3:0], FFFh } */
		ul_write_val1[11] = ul_write_val1[10] = ul_write_val1[9] = ul_write_val1[8] = ul_write_val1[7] = 0xFFFFFFFF;
		ul_checksum_val = 0;
		for (i = 0; i < 12; i++) /* 12 times */
			ul_checksum_val += ul_write_val1[i];

		ul_write_val1[12] = ul_checksum_val;
		ul_write_val1[13] = 0xFFFF0000 | MAKER_CODE; /* { FFFFh, CCCCh } */
		ul_write_val1[14] = 0x99756768;
		ul_write_val1[15] = 0x01AC29AC;

		/* Flash Trimming Mat Page 1 Program */
		us_result = hs_flash_information_mat_program(o_ctrl, ul_write_val1, 0x00100010);
		if (us_result != 0x00)
			return (0x04); /* Error */

		/* Protect */
		us_result = hs_protect(o_ctrl);
		if (us_result != 0x00)
			return (0x05); /* Error */

		/* Verify Read (Flash Trimming Mat Page 0) */
		us_result = hs_verify_read(o_ctrl, ul_write_val0, 0x00100000);
		if (us_result != 0x00)
			return (0x06); /* Error */

		/* Verify Read (Flash Trimming Mat Page 1) */
		us_result = hs_verify_read(o_ctrl, ul_write_val1, 0x00100010);
		if (us_result != 0x00)
			return (0x07); /* Error */
	}
	return (0x00);
}

/*********************************************************************************
	Function Name     : hs_osc_adj_main
	Value            : NON
	Argment Value     : NON
	Explanation       : hs_osc_adj_main
********************************************************************************/
static uint8_t hs_osc_adj_main(struct cam_ois_ctrl_t *o_ctrl)
{
	uint32_t ul_cver;
	uint8_t uc_result = 0x00;
	uint16_t us_osc_val = 0x0000;
	uint8_t uc_err_code;

#if I2C_TIME_MODE == 1
#else /* I2C_TIME_MODE */
	uint16_t us_osc_can[3];
	uint16_t us_mid_osc;
	uint8_t uc_osc_sts = 0x00;
#endif /* I2C_TIME_MODE */

	CAM_INFO(CAM_OIS, "hs_osc_adj_main \n");
	hs_io_write32(o_ctrl, 0xE0701C, 0x00000000);

	hs_io_read32(o_ctrl, SYSDSP_CVER, &ul_cver); /* SYSDSP_CVER */

	if (hs_check_checkcode(o_ctrl, (uint16_t)ul_cver) == 0x00) { /* Check CHECKCODE */
		CAM_INFO(CAM_OIS, "OSC TSET \n");
#if I2C_TIME_MODE == 1
		if (hs_osc_adj(o_ctrl, &us_osc_val) == 0x00) { /* OSC Clock adjustment */
#else /* I2C_TIME_MODE */
		uc_osc_sts |= hs_osc_adj(o_ctrl, &us_osc_val);
		us_osc_can[0] = us_osc_val;

		uc_osc_sts |= hs_osc_adj(o_ctrl, &us_osc_val);
		us_osc_can[1] = us_osc_val;

		uc_osc_sts |= hs_osc_adj(o_ctrl, &us_osc_val);
		us_osc_can[2] = us_osc_val;

		if ((us_osc_can[0] > us_osc_can[1]) && (us_osc_can[0] > us_osc_can[2]))
			us_mid_osc = (us_osc_can[1] > us_osc_can[2]) ? us_osc_can[1] : us_osc_can[2];
		else if ((us_osc_can[1] > us_osc_can[0]) && (us_osc_can[1] > us_osc_can[2]))
			us_mid_osc = (us_osc_can[0] > us_osc_can[2]) ? us_osc_can[0] : us_osc_can[2];
		else
			us_mid_osc = (us_osc_can[0] > us_osc_can[1]) ? us_osc_can[0] : us_osc_can[1];

		CAM_INFO(CAM_OIS, "%02X, %02X, %02X, Mid %02X \n",
			us_osc_can[0], us_osc_can[1], us_osc_can[2], us_mid_osc);
		us_osc_val = us_mid_osc;

		if (uc_osc_sts == 0x00) { /* OSC Clock adjustment */
#endif /* I2C_TIME_MODE */
			uc_err_code = hs_write_osc_adj_val(o_ctrl, us_osc_val, ul_cver);
			if (uc_err_code == 0x00) { /* Write Osc adjustment value */
				uc_result = 0x00; /* OK */
			} else {
				CAM_INFO(CAM_OIS, "hs_write_osc_adj_val() : NG (%02x)\n", uc_err_code);
				uc_result = 0x03;
			}
			hs_boot_mode(o_ctrl);
		} else {
			CAM_INFO(CAM_OIS, "hs_osc_adj() : NG \n");
			uc_result = 0x02;
		}
	} else {
		CAM_INFO(CAM_OIS, "hs_check_checkcode() : Done or Error \n");
		uc_result = 0x01;
	}
	hs_io_write32(o_ctrl, 0xE0701C, 0x00000002);
	return(uc_result);
}

uint8_t hs_drv_off_adj(struct cam_ois_ctrl_t *o_ctrl)
{
	uint8_t ans = 0;

	hs_boot_mode(o_ctrl);

	/* OSC adjustment if no adjustment */
	ans = hs_osc_adj_main(o_ctrl);

	CAM_INFO(CAM_OIS, "[%02x]hs_osc_adj_main \n", ans);
	if (ans != 0 && ans != 1) /* Not OK & Not Done */
		return(ans); /* error */

	return(ans);
}