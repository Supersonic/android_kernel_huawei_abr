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
#include "ois_lc898129.h"
#include "ois_lc898129_fun.h"

#define OIS129_READ_STATUS_INI          0x01000000
#define OIS129_CNT050MS                 676
#define OIS129_CMD_READ_STATUS          0xF100
/* for oisdata protocol2.0 */
#define EIS_VERSION_MASK                0xff
#define EIS_VERSION_MASK_SHIFT          16
#define FLASH_BLOCKS                    14 /* 1[Block] = 4[KByte] (14*4 = 56[KByte]) */
#define USER_RESERVE                    0 /* Reserved for customer data blocks */
#define ERASE_BLOCKS                    (FLASH_BLOCKS - USER_RESERVE)
#define EIS_DATA_VERSION                0x8004
/* checksum size 4bytes + checksum 4bytes + fw version 4bytes */
#define FW_OFFSET                       12

struct fw_desc_t {
	uint32_t fw_checksum_size;
	uint32_t fw_checksum;
	uint32_t fw_version;
};

struct vendor_ois_ctrl_t {
	bool already_download;
	bool force_download;
	struct fw_desc_t desc;
};

/****************this should be only global define!!!*****************/
static struct vendor_ois_ctrl_t g_ctrl;
/****************this should be only global define!!!*****************/

static int32_t user_mat_write129(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw)
{
	int32_t from_code_size = fw->size-FW_OFFSET;
	int32_t write_num1 = from_code_size / 64; /* 64 bytes written at a time */
	int32_t write_num2 = from_code_size % 64;
	int32_t write_num3 = write_num2 * 4;
	int32_t write_num4 = write_num2 / 4;

	uint8_t data[67]; /* 4*16 + 3 = 67 bytes */
	uint32_t n;
	const uint8_t *pd = (uint8_t *)fw->data;
	uint32_t read_val;
	uint8_t uc_snd_dat = SUCCESS; /* 0x01 = NG  0x00 = OK */
	int32_t i, j;
	if (unlock_code_set129(o_ctrl) == SUCCESS) { /* Unlock Code Set */
		hs_write_permission129(o_ctrl); /* Write Permission */
		addtional_unlock_code_set129(o_ctrl); /* Additional Unlock Code Set */
		ois_write_addr16_value32(o_ctrl, 0xF007, 0x00000000); /* Update FlashAccess Command Table */
		ois_write_addr16_value32(o_ctrl, 0xF00A, 0x00000000); /* FromCmd Addr Configuration */
		/* Transfer every 64byte */
		for (i = 0; i < write_num1; i++) {
			n = 0;
			data[n++] = 0xF0; /* CmdH */
			data[n++] = 0x08; /* CmdL */
			data[n++] = 0x00; /* FromCmd address of the bufferA */
			for (j = 0; j < 16; j++) {
				data[n++] = *pd++; /* 1byte */
				data[n++] = *pd++; /* 2byte */
				data[n++] = *pd++; /* 3byte */
				data[n++] = *pd++; /* 4byte */
			}
			hs_cnt_write(o_ctrl, data, 67); /* 4*16 + 3 = 67 bytes */

			ois_write_addr16_value32(o_ctrl, 0xF00B, 0x00000010); /* FromCmd Configuration Example */

			ois_write_addr16_value32(o_ctrl, 0xF00C, 0x00000004); /* Setting FromCmd.Control (Write) */
			do /* Write completion judgment */
				ois_read_addr16_value32(o_ctrl, 0xF00C, &read_val); /* Read from Cmd.Control */
			while (read_val != 0);
		}
		/* Transfer 64byte or less */
		if (write_num2 != 0) {
			n = 0;
			data[n++] = 0xF0; /* CmdH */
			data[n++] = 0x08; /* CmdL */
			data[n++] = 0x00; /* FromCmd address of the bufferA */
			for (j = 0; j < write_num4; j++) {
				data[n++] = *pd++; /* 1byte */
				data[n++] = *pd++; /* 2byte */
				data[n++] = *pd++; /* 3byte */
				data[n++] = *pd++; /* 4byte */
			}
			hs_cnt_write(o_ctrl, data, write_num3 + 3);
			ois_write_addr16_value32(o_ctrl, 0xF00B, write_num4); /* FromCmd Configuration Example */
			ois_write_addr16_value32(o_ctrl, 0xF00C, 0x00000004); /* FromCmd Control Settings (Write) */
			do { /* write end judgment */
				ois_read_addr16_value32(o_ctrl, 0xF00C, &read_val); /* FromCmd readout of Control */
				msleep(1);
			} while (read_val != 0);
		}

		if (unlock_code_clear129(o_ctrl) == FAILURE) /* Unlock Code Clear */
			uc_snd_dat = FAILURE;
	} else {
		uc_snd_dat = FAILURE;
	}
	return uc_snd_dat;
}

/*********************************************************************************
	Function Name     : updata_code_write129
	Retun Value       : NON
	Argment Value     : NON
	Explanation       : Write the UpData Code of the LC898129 to the Pmem.
	History           : First edition                     2019.03.11
*********************************************************************************/
static void updata_code_write129(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw_updata)
{
	/* Calculate the number of elements in the Pmem data. */
	int32_t write_num1 = UP_DATA_CODE_SIZE * 5 / 50;
	int32_t write_num2 = UP_DATA_CODE_SIZE * 5 % 50;
	int32_t write_num3 = write_num2 + 2;
	uint32_t i, num;
	uint8_t data[52] = {0}; /* 5*10+2=52bytes */
	uint32_t n;
	const uint8_t *pd = (uint8_t *)fw_updata->data;

	/* Pmem address set */
	data[0] = 0x30; /* CmdH */
	data[1] = 0x00; /* CmdL */
	data[2] = 0x00; /* DataH */
	data[3] = 0x08; /* DataMH */
	data[4] = 0x00; /* DataML */
	data[5] = 0x00; /* DataL */

	hs_cnt_write(o_ctrl, data, 6);
	/* Pmem data write */
	data[0] = 0x40; /* CmdH */
	data[1] = 0x00; /* CmdL */
	for (num = 0; num < write_num1; num++) {
		n = 2;
		for (i = 0; i < 10; i++) {
			data[n++] = *pd++;
			data[n++] = *pd++;
			data[n++] = *pd++;
			data[n++] = *pd++;
			data[n++] = *pd++;
		}
		hs_cnt_write(o_ctrl, data, 52); /* 5*10+2=52bytes */
	}
	if (write_num2 != 0) {
		n = 2;
		for (i = 0; i < write_num2; i++)
			data[n++] = *pd++;
		hs_cnt_write(o_ctrl, data, write_num3);
	}
}

static uint8_t updata_code_read129(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw_updata)
{
	uint8_t data[6];
	uint8_t read_data[5];
	uint8_t ng_flg = SUCCESS; /* 0x01 = NG  0x00 = OK */
	uint32_t i;
	uint8_t j;
	const uint8_t *pd = (uint8_t *)fw_updata->data;

	/* PMEM Address Set */
	data[0] = 0x30; /* CmdH */
	data[1] = 0x00; /* CmdL */
	data[2] = 0x00; /* DataH */
	data[3] = 0x08; /* DataMH */
	data[4] = 0x00; /* DataML */
	data[5] = 0x00; /* DataL */
	hs_cnt_write(o_ctrl, data, 6);

	/* Pmem Data Read & Verify */
	for (i = 0; i < UP_DATA_CODE_SIZE; i++) {
		ois_i2c_block_read_reg(o_ctrl, 0x4000, (uint32_t *)read_data, 5,
			CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
		for (j = 0; j < 5; j++) {
			if (read_data[j] != *pd++)
				ng_flg = FAILURE;
		}
	}
	return ng_flg; /* Verify Result */
}

/*********************************************************************************
	Function Name     : updata_code_checksum129
	Retun Value       : NON
	Argment Value     : NON
	Explanation       : LC898129 Checksum Execution and Judgment of Command PMEM Area
	History           : First edition         2018.05.07
*********************************************************************************/
static uint8_t updata_code_checksum129(struct cam_ois_ctrl_t *o_ctrl)
{
	uint8_t data[6];
	uint8_t read_data[8];
	uint32_t ulcnt;
	uint32_t read_val;
	uint8_t uc_snd_dat = SUCCESS; /* 0x01 = NG  0x00 = OK */
	int32_t size = UP_DATA_CODE_SIZE;
	int64_t checksum_code = UP_DATA_CODE_CHECKSUM;
	uint8_t *p = (uint8_t *)&checksum_code;
	uint8_t i;

	/* Launching the CheckSum of Program RAM */
	data[0] = 0xF0; /* CmdID */
	data[1] = 0x0E; /* CmdID */
	data[2] = (uint8_t)((size >> 8) & 0x000000FF); /* Write Data (MSB) */
	data[3] = (uint8_t)(size & 0x000000FF); /* Write Data */
	data[4] = 0x00; /* Write Data */
	data[5] = 0x00; /* Write Data(LSB) */
	hs_cnt_write(o_ctrl, data, 6);

	/* Checksum termination judgment */
	ulcnt = 0;
	do {
		if (ulcnt++ > 100) {
			uc_snd_dat = FAILURE;
			break;
		}
		ois_read_addr16_value32(o_ctrl, 0x0088, &read_val); /* Reading PMCheck.ExecFlag */
	} while (read_val != 0);

	/* Read CheckSum Values */
	if (uc_snd_dat == SUCCESS) {
		ois_i2c_block_read_reg(o_ctrl, 0xF00E, (uint32_t *)read_data, 8,
			CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);

		/* Determine the CheckSum value (The expected value is defined in the Header.) */
		for (i = 0; i < 8; i++) {
			if (read_data[7 - i] != *p++) /* CheckSum code judgment */
				uc_snd_dat = FAILURE;
		}
	}
	return uc_snd_dat;
}

static uint8_t user_mat_erase129(struct cam_ois_ctrl_t *o_ctrl)
{
	uint32_t addr;
	uint32_t ulcnt;
	uint32_t read_val;
	uint8_t uc_snd_dat = SUCCESS; /* 0x01 = NG  0x00 = OK */
	uint32_t i;

	if (unlock_code_set129(o_ctrl) == SUCCESS) { /* Unlock Code Set */
		hs_write_permission129(o_ctrl); /* Write Permission */
		addtional_unlock_code_set129(o_ctrl); /* Additional Unlock Code Set */
		/* Update FlashAccess Command Table */
		ois_write_addr16_value32(o_ctrl, 0xF007, 0x00000000);

		/* Clear User Mat Block  */
		for (i = 0; i < ERASE_BLOCKS; i++) {
			addr = i << 10;
			/* FromCmd Addr Configuration */
			ois_write_addr16_value32(o_ctrl, 0xF00A, addr);
			/* FromCmd control settings (clear blocks)) */
			ois_write_addr16_value32(o_ctrl, 0xF00C, 0x00000020);

			/* Block Erase End Judgment */
			ulcnt = 0;
			do {
				if (ulcnt++ > 100) {
					uc_snd_dat = -3; /* FAILURE; */
					break;
				}
				/* FromCmd readout of Control */
				ois_read_addr16_value32(o_ctrl, 0xF00C, &read_val);
				msleep(1);
			} while (read_val != 0);
			if (uc_snd_dat == -3)
				break;
		}

		if (unlock_code_clear129(o_ctrl) == FAILURE) /* Unlock Code Clear */
			uc_snd_dat = -2; /* FAILURE; */
	} else {
		uc_snd_dat = -1; /* FAILURE; */
	}

	return uc_snd_dat;
}

/*********************************************************************************
	Function Name : user_mat_checksum129
	Retun Value   : NON
	Argment Value : NON
	Explanation   :LC898129 Run the CheckSum command in the Command User Mat area.
********************************************************************************/
static uint8_t user_mat_checksum129(struct cam_ois_ctrl_t *o_ctrl, const struct firmware *fw)
{
	uint32_t read_val;
	uint8_t uc_snd_dat = SUCCESS; /* 0x01 = NG  0x00 = OK */
	ois_write_addr16_value32(o_ctrl, 0xF00A, 0x00000000); /* FromCmd Addr Configuration */
	/* ptr->SizeFromCodeValid   Set the CheckSum size */
	ois_write_addr16_value32(o_ctrl, 0xF00D, g_ctrl.desc.fw_checksum_size);
	/* FromCmd Set Control (Execute CheckSum) */
	ois_write_addr16_value32(o_ctrl, 0xF00C, 0x00000100);
	do { /* write end judgment */
		ois_read_addr16_value32(o_ctrl, 0xF00C, &read_val); /* FromCmd readout of Control */
		msleep(1);
	} while (read_val != 0);

	ois_read_addr16_value32(o_ctrl, 0xF00D, &read_val); /* Read CheckSum Values */
	if (read_val != (uint32_t)g_ctrl.desc.fw_checksum) /* ptr->SizeFromCodeCksm */
		uc_snd_dat = FAILURE;
	return uc_snd_dat;
}

static int32_t hs_flash_updata129(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw, const struct firmware *fw_updata)
{
	uint32_t uc_snd_dat = SUCCESS; /* 0x01 = FAILURE, 0x00 = SUCCESS */
	uint32_t ul_data_val;
	/* Adjusting OSC, LDO, and SMA Measurement Currents */
	uc_snd_dat = hs_drv_off_adj(o_ctrl);
	if (uc_snd_dat != 0 && uc_snd_dat != 1)
		return uc_snd_dat;
	/* Go to Boot Mode */
	hs_boot_mode(o_ctrl);

	/* UpData Execution */
	hs_io_read32(o_ctrl, 0xD000AC, &ul_data_val);
	if ((ul_data_val & 0x00000001) == 0) {
		hs_io_write32(o_ctrl, 0xD000AC, 0x00000000); /* Turn off MC_IGNORE2 */
		updata_code_write129(o_ctrl, fw_updata); /* Write Updateta Code to Program RAM */
	} else {
		return -1; /* Boot Mode Migration Failed */
	}
	if (updata_code_read129(o_ctrl, fw_updata) == FAILURE)
		return -2; /* The Read Verify of the UpData code is incorrect */
	if (updata_code_checksum129(o_ctrl) == FAILURE)
		return -3; /* UpData The value of CheckSum in the code is NG */
	hs_io_write32(o_ctrl, 0xE0701C, 0x00000000); /* FLASH Standby Disable */
	if (user_mat_erase129(o_ctrl) == FAILURE)
		return -4; /* Flash Memory Error */
	if (user_mat_write129(o_ctrl, fw) == FAILURE)
		return -5; /* Flash Memory Write Failed */
	if (user_mat_checksum129(o_ctrl, fw) == FAILURE)
		return -6; /* The value of CheckSum in the Flash Memory is NG */
	hs_io_write32(o_ctrl, 0xD000AC, 0x00001000); /* CORE_RST ON */
	msleep(30); /* 30 [msec] Waiting */
	hs_io_read32(o_ctrl, 0xD000AC, &ul_data_val);
	if (ul_data_val != 0x000000A1)
		return -7; /* ReMap Failed */
	return SUCCESS;
}

static uint8_t lc898129_read_status(struct cam_ois_ctrl_t *o_ctrl, uint32_t bit_check)
{
	uint32_t read_val;
	ois_read_addr16_value32(o_ctrl, OIS129_CMD_READ_STATUS, &read_val);
	if (bit_check)
		read_val &= OIS129_READ_STATUS_INI;
	if (!read_val)
		return 0;
	else
		return 1;
}

static void lc898129_set_active_mode(struct cam_ois_ctrl_t *o_ctrl)
{
	uint8_t st_rd = 1;
	uint32_t st_cnt = 0;
	hs_io_write32(o_ctrl, 0xD01008, 0x00000090);
	ois_write_addr16_value32(o_ctrl, 0xF019, 0x00000000);

	while (st_rd && (st_cnt++ < OIS129_CNT050MS))
		st_rd = lc898129_read_status(o_ctrl, 1);
}

static void update_ois_data_protocol(struct cam_ois_ctrl_t *o_ctrl)
{
	uint32_t tmp = 0;
	static uint16_t g_ois_data_protocol = 0;
	ois_read_addr16_value32(o_ctrl, EIS_DATA_VERSION, (uint32_t *)&tmp);
	g_ois_data_protocol = (tmp & (EIS_VERSION_MASK << EIS_VERSION_MASK_SHIFT))
		>> EIS_VERSION_MASK_SHIFT;
}

static int32_t lc898129_download_ois_fw(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw, const struct firmware *fw_updata)
{
	uint32_t fw_version_current = 0;
	int32_t result = 0;

	ois_read_addr16_value32(o_ctrl, FW_VER_ADDR, (uint32_t *)&fw_version_current);
	CAM_INFO(CAM_OIS, "lc898129 fw_version = 0x%x", fw_version_current);
	if (fw_version_current == g_ctrl.desc.fw_version) {
		CAM_INFO(CAM_OIS, "Latest version");
		return 0;
	}
	lc898129_set_active_mode(o_ctrl);
	result = hs_flash_updata129(o_ctrl, fw, fw_updata);
	hs_io_write32(o_ctrl, 0xE0701C, 0x00000002); /* FLASH Standby Enable */
	if (result == SUCCESS)
		CAM_INFO(CAM_OIS, "fw download success");
	else
		CAM_INFO(CAM_OIS, "fw download failed ,result = %d", result);
	update_ois_data_protocol(o_ctrl);
	return 0;
}

static void lc898129_fw_analysis(const struct firmware *fw)
{
	uint32_t index = fw->size;
	const uint8_t *pd = (uint8_t *)fw->data - 1;
	g_ctrl.desc.fw_version = pd[index] + (pd[index - 1] << 8) + (pd[index - 2] << 16) + (pd[index - 3] << 24);
	index -= 4; /* fw_version 4bytes = 32 bits */
	g_ctrl.desc.fw_checksum = pd[index] + (pd[index - 1] << 8) + (pd[index - 2] << 16) + (pd[index - 3] << 24);
	index -= 4; /* fw_checksum 4bytes = 32 bits */
	g_ctrl.desc.fw_checksum_size = pd[index] + (pd[index - 1] << 8) + (pd[index - 2] << 16) + (pd[index - 3] << 24);
	CAM_INFO(CAM_OIS, "lc898129 fw_version = 0x%x, fw_chesum = 0x%x, fw_checksum_size = 0x%x",
		g_ctrl.desc.fw_version, g_ctrl.desc.fw_checksum, g_ctrl.desc.fw_checksum_size);
}

int32_t lc898129_fw_download(struct cam_ois_ctrl_t *o_ctrl)
{
	int32_t rc;
	const struct firmware *fw = NULL;
	const struct firmware *fw_updata = NULL;
	const char *fw_name = NULL;
	const char *fw_updata_name = "lc898129_updatacode.prog";
	char name_buf[32] = {0};
	struct device *dev = &(o_ctrl->pdev->dev);
	if (!o_ctrl) {
		CAM_ERR(CAM_OIS, "Invalid Args");
		return -EINVAL;
	}

	if (strcmp(o_ctrl->opcode.fw_ver_name, "") == 0 ||
		strcmp(o_ctrl->opcode.fw_ver_name, "-") == 0) {
		snprintf(name_buf, 32, "%s.prog", o_ctrl->ois_name);
	} else {
		snprintf(name_buf, 32, "%s_%s.prog", o_ctrl->ois_name, o_ctrl->opcode.fw_ver_name);
	}
	fw_name = name_buf;
	CAM_INFO(CAM_OIS, "ois_name:%s, fw_ver_name:%s, fw:%s",
		o_ctrl->ois_name, o_ctrl->opcode.fw_ver_name, fw_name);
	/* Load FW */
	rc = request_firmware(&fw, fw_name, dev);
	if (rc) {
		CAM_ERR(CAM_OIS, "Failed to locate %s", fw_name);
		return rc;
	}
	/* Load updatacode FW */
	rc = request_firmware(&fw_updata, fw_updata_name, dev);
	if (rc) {
		CAM_ERR(CAM_OIS, "Failed to locate %s", fw_updata_name);
		return rc;
	}
	lc898129_fw_analysis(fw);
	rc = lc898129_download_ois_fw(o_ctrl, fw, fw_updata);
	if (rc)
		CAM_ERR(CAM_OIS, "download_ois_fw fail");
	release_firmware(fw);
	release_firmware(fw_updata);
	return rc;
}

