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
#include "ois_dw9781.h"
#include "ois_dw9781_workaround.h"
#include "ois_dw9781_params.h"
#include "hwcam_hiview.h"

#define DW9781_CHIP_ID_ADDRESS            0x7000

// #define INDEX_DEGREE                      0
// #define INDEX_ARRAGEMENT                  1
// #define MODULE_VERSION                    6
// #define SUPPLIER_VERSION                  5

#define RETRY_TIME                        2
#define SECTOR_NUM                        5

#define MTP_START_ADDRESS                 0x8000
#define VERIFY_OK                         0
#define VERIFY_ERROR                      1

#define DW9781_FW_SIZE                    0x2800
#define DW9781_VERSION_OFFSET             5
#define DW9781_CHECKSUM_OFFSET            6
#define DW9781_CHIPID_OFFSET              3
#define DATPKT_SIZE                       32

struct fw_desc_t {
	uint32_t fw_size;
	uint32_t checksum;
	uint16_t version;
	uint16_t chip_id;
};

struct vendor_ois_ctrl_t {
	bool already_download;
	struct fw_desc_t desc;
};

/****************this should be only global define!!!*****************/
static struct vendor_ois_ctrl_t g_ctrl;
/****************this should be only global define!!!*****************/

static int dw9781b_i2c_read_write_fail_handle(struct cam_ois_ctrl_t *o_ctrl)
{
	int ret;

	o_ctrl->io_master_info.cci_client->sid = DW9781B_LOGIC_SLAVE_ID;
	ret = ois_write_addr16_value16(o_ctrl,
		DW9781B_LOGIC_RESET_ADDRESS, DW9781B_LOGIC_RESET_VAL);
	if (ret < 0)
		CAM_ERR(CAM_OIS, "dw9781b ois logic reset failed");

	o_ctrl->io_master_info.cci_client->sid = DW9781B_SLAVE_ID;
	msleep(10); /* delay 10ms for dw9781b reset */

	return ret;
}

int dw9781b_ois_write_addr16_value16(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint16_t value)
{
	int32_t rc = 0;
	rc = ois_write_addr16_value16(o_ctrl, reg, value);
	if (rc < 0 && o_ctrl->io_master_info.cci_client->sid == DW9781B_SLAVE_ID) {
		CAM_ERR(CAM_OIS, "dw9781b i2c write:%x val:%x fail, set logic reset", reg, value);
		dw9781b_i2c_read_write_fail_handle(o_ctrl);
		rc = ois_write_addr16_value16(o_ctrl, reg, value);
		if (rc < 0) {
			CAM_ERR(CAM_OIS, "dw9781b ois after logic reset write fail");
			cam_i2c_error_hiview_handle(&(o_ctrl->io_master_info), "dw9781b i2c write fail");
		}
	}

	return rc;
}

int dw9781b_ois_read_addr16_value16(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint16_t *value)
{
	int32_t rc = 0;
	rc = ois_read_addr16_value16(o_ctrl, reg, value);
	if (rc < 0 && o_ctrl->io_master_info.cci_client->sid == DW9781B_SLAVE_ID) {
		CAM_ERR(CAM_OIS, "dw9781 i2c read:%x fail, set logic reset", reg);
		dw9781b_i2c_read_write_fail_handle(o_ctrl);
		rc = ois_read_addr16_value16(o_ctrl, reg, value);
		if (rc < 0) {
			CAM_ERR(CAM_OIS, "dw9781 ois after logic reset read fail");
			cam_i2c_error_hiview_handle(&(o_ctrl->io_master_info), "dw9781b i2c read fail");
		}
	}

	return rc;
}

static void dw9781_ois_reset(struct cam_ois_ctrl_t *o_ctrl)
{
	/* Logic reset */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xD002, 0x0001);
	msleep(4); /* wait time */
	/* Active mode (DSP ON) */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xD001, 0x0001);
	msleep(25); /* ST gyro - over wait 25ms, default Servo On */
	/* User protection release */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xEBF1, 0x56FA);
	CAM_INFO(CAM_OIS, "ois reset finish");
}

static int32_t dw9781_ready_check(struct cam_ois_ctrl_t *o_ctrl)
{
	uint16_t flag_t = 0;
	uint16_t flag_l = 0;

	dw9781b_ois_write_addr16_value16(o_ctrl, 0xd000, 0x0001); /* active mode */
	msleep(4); /* wait time */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xd001, 0x0000); /* dsp mode */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xFAFA, 0x98AC); /* All protection(1) */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xF053, 0x70BD); /* All protection(2) */
	dw9781b_ois_read_addr16_value16(o_ctrl, 0x9FF9, &flag_t); /* T PRJ checksum flag */
	dw9781b_ois_read_addr16_value16(o_ctrl, 0xA7F9, &flag_l); /* L PRJ checksum flag */

	if (flag_t == 0xCC33 || flag_l == 0xCC33) {
		dw9781_ois_reset(o_ctrl); /* ois reset */
		return 0;
	} else {
		dw9781b_ois_write_addr16_value16(o_ctrl, 0xD002, 0x0001); /* logic reset */
		msleep(4); /* reset delay time */
		CAM_ERR(CAM_OIS, "previous fw download fail");
		return -1;
	}
}

static void dw9781_set_download_mode(struct cam_ois_ctrl_t *o_ctrl)
{
	/* release all protection */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xFAFA, 0x98AC);
	msleep(1);
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xF053, 0x70BD);
	msleep(1);
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xD041, 0x000E); /* set spi SSB1 */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xD043, 0x000E); /* set spi SCLK */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xD044, 0x000E); /* set spi SDAT */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xDD02, 0x01C8); /* set spi input mode */
	CAM_INFO(CAM_OIS, "set download mode");
}

static void dw9781_erase_mtp(struct cam_ois_ctrl_t *o_ctrl)
{
	int32_t i;
	uint16_t sector[SECTOR_NUM] = {
		0x0000, 0x0008, 0x0010, 0x0018, 0x0020
	};

	/* Erase each 4k Sector */
	for (i = 0; i < SECTOR_NUM; i++) {
		dw9781b_ois_write_addr16_value16(o_ctrl, 0xDE03, sector[i]);
		/* 4k Sector Erase */
		dw9781b_ois_write_addr16_value16(o_ctrl, 0xDE04, 0x0002);
		msleep(10);
	}
	CAM_INFO(CAM_OIS, "erase mtp finish");
}

static void dw9781_erase_for_rewritefw(struct cam_ois_ctrl_t *o_ctrl)
{
	/* last 512byte select */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xDE03, 0x0027);
	/* last 512byte Erase */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xDE04, 0x0008);
	msleep(10); /* wait time */
}

static int32_t dw9781_download_fw(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw)
{
	int32_t ret = 0;
	uint32_t i;
	uint16_t addr;
	uint16_t *buf_r = NULL;
	uint16_t *ptr = (uint16_t *)fw->data;
	uint32_t fw_size = fw->size / 2;
	uint16_t write_cnt = fw_size / DATPKT_SIZE;
	uint16_t write_index;

	buf_r = (uint16_t *)vmalloc(fw_size * sizeof(uint16_t) + 2);
	if (!buf_r) {
		CAM_ERR(CAM_OIS, "malloc failed");
		return -EIO;
	}
	memset(buf_r, 0, fw_size * sizeof(uint16_t) + 2);

	/* step 1: MTP setup */
	dw9781_set_download_mode(o_ctrl);
	CAM_INFO(CAM_OIS, "flash ready,fw_size:%x", fw->size);

	/* step 2: MTP Erase and DSP Disable for FW 0x8000 write */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0xd001, 0x0000);
	dw9781_erase_mtp(o_ctrl);

	/* step 3: Seq Write All Flash */
	for (write_index = 0; write_index < write_cnt; write_index++) {
		addr = MTP_START_ADDRESS + write_index * DATPKT_SIZE;
		/* 0: MSM_CCI_I2C_WRITE_SEQ */
		ois_i2c_block_write_reg(o_ctrl, addr,
			(uint16_t *)(ptr + write_index * DATPKT_SIZE), DATPKT_SIZE, 0);
	}
	CAM_INFO(CAM_OIS, "flash write complete");

	/* step 4: Firmware memory read */
	for (i = 0; i < fw_size; i = i + DATPKT_SIZE) {
		addr = MTP_START_ADDRESS + i;
		ois_i2c_block_read_reg(o_ctrl, addr, (uint32_t *)(buf_r + i), DATPKT_SIZE * 2,
			CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
	}
	CAM_INFO(CAM_OIS, "flash read complete");

	/* step 5: Verify */
	for (i = 0; i < fw_size; i++) {
		if (ptr[i] != buf_r[i]) {
			ret = -EIO;
			CAM_ERR(CAM_OIS, "fw verify error, addr:%4xh--fw:%4xh--read:%4xh",
				MTP_START_ADDRESS + i, ptr[i], buf_r[i]);
			break;
		}
	}

	if (ret != 0) {
		dw9781_erase_for_rewritefw(o_ctrl);
		/* Shut download mode */
		dw9781b_ois_write_addr16_value16(o_ctrl, 0xd000, 0x0000);
		CAM_INFO(CAM_OIS, "FW Download NG!!!");
		return ret;
	}

	CAM_INFO(CAM_OIS, "download_FW finished :%d", ret);
	vfree(buf_r);
	return ret;
}

static uint16_t dw9781_fw_checksum_verify(struct cam_ois_ctrl_t *o_ctrl)
{
	uint16_t data = 0;
	/* FW checksum command */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7011, 0x2000);
	/* command  start */
	dw9781b_ois_write_addr16_value16(o_ctrl, 0x7010, 0x8000);
	msleep(10);
	/* calc the checksum to write the 0x7005 */
	dw9781b_ois_read_addr16_value16(o_ctrl, 0x7005, &data);
	return data;
}

static uint16_t dw9781_fw_checksum(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw)
{
	uint16_t fw_checksum_current;
	uint16_t fw_checksum_result;
	uint32_t checksum_retry = 0;

	while (checksum_retry < RETRY_TIME) {
		/* read fw checksum value from IC register: 0x7005 */
		fw_checksum_current = dw9781_fw_checksum_verify(o_ctrl);
		fw_checksum_result = (g_ctrl.desc.checksum ==
			fw_checksum_current) ? VERIFY_OK : VERIFY_ERROR;
		CAM_INFO(CAM_OIS, "fw_checksum:0x%x, checksum:0x%x, result:0x%x",
			g_ctrl.desc.checksum, fw_checksum_current, fw_checksum_result);
		dw9781_ois_reset(o_ctrl); /* must ois reset after fw checksum */
		if (fw_checksum_result == VERIFY_ERROR)
			checksum_retry++;
		else
			return VERIFY_OK;
	}

	return VERIFY_ERROR;
}

static int32_t dw9781_download_ois_fw(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw)
{
	int32_t ret;
	uint16_t fw_version_latest;
	uint16_t fw_version_current = 0;
	uint16_t chip_id = 0;
	uint16_t second_id = 0;
	uint16_t fw_checksum_result = VERIFY_OK;

	/* 2. check if loaded */
	if (g_ctrl.already_download) {
		CAM_INFO(CAM_OIS, "fw already downloaded");
		dw9781b_ois_write_addr16_value16(o_ctrl, 0xd000, 0x0001); /* active mode */
		msleep(4); /* wait time */
		return 0;
	}
	dw9781_ready_check(o_ctrl); /* check dw9781 checksum flag */
	/* 6. ready to download */
	dw9781b_ois_read_addr16_value16(o_ctrl, DW9781_CHIP_ID_ADDRESS, &chip_id);
	CAM_INFO(CAM_OIS, "chipid_value = 0x%x", chip_id);

	/* 6.1 if error occured last time, chip id maybe tampered */
	if (chip_id != g_ctrl.desc.chip_id) {
		dw9781_set_download_mode(o_ctrl);
		/* second_info: 0x0020 */
		dw9781b_ois_read_addr16_value16(o_ctrl, 0xd060, &second_id);
		if (second_id == 0x0020) {
			/* Need to forced update OIS FW again. */
			ret = dw9781_download_fw(o_ctrl, fw);
			CAM_INFO(CAM_OIS, "finish flash download");
			if (ret) {
				/* Shut download mode */
				dw9781b_ois_write_addr16_value16(o_ctrl, 0xd000, 0x0000);
				CAM_ERR(CAM_OIS, "select download error, ret = 0x%x", ret);
				return -EIO;
			}
		} else {
			/* Shut download mode */
			dw9781b_ois_write_addr16_value16(o_ctrl, 0xd000, 0x0000);
			CAM_ERR(CAM_OIS, "second info check fail");
			return -EIO;
		}
	} else {
		dw9781b_ois_read_addr16_value16(o_ctrl, 0x7001, &fw_version_current);
		fw_version_latest = g_ctrl.desc.version;

		CAM_INFO(CAM_OIS, "fw version_current:0x%x, version_latest:0x%x",
			fw_version_current, fw_version_latest);

		if ((fw_version_current & 0xFF) == (fw_version_latest & 0xFF))
			fw_checksum_result = dw9781_fw_checksum(o_ctrl, fw);

		/* download fw, check if need update, download fw to flash */
		if (((fw_version_current & 0xFF) != (fw_version_latest & 0xFF)) ||
			fw_checksum_result == VERIFY_ERROR) {
			ret = dw9781_download_fw(o_ctrl, fw);
			if (ret) {
				/* Shut download mode */
				dw9781b_ois_write_addr16_value16(o_ctrl, 0xd000, 0x0000);
				CAM_ERR(CAM_OIS, "select download error, ret = 0x%x", ret);
				return -EIO;
			}
		} else {
			CAM_INFO(CAM_OIS, "ois fw version is updated, skip download");
		}
	}

	/* check if INI and OTP is same, if INI is error, recovery INI */
	//dw9781_ois_recovery_memory(g_ois_otp.ois, MAX_OIS_OTP_SIZE,
	//	g_module_info.data[MODULE_VERSION], g_module_info.data[SUPPLIER_VERSION],
	//	g_gyro_select_value, g_gyro_pos_map[g_s_cfg_params.gyro_position][INDEX_ARRAGEMENT],
	//	g_gyro_pos_map[g_s_cfg_params.gyro_position][INDEX_DEGREE]);

	g_ctrl.already_download = true;
	return 0;
}

static void dw9781_fw_analysis(const struct firmware *fw)
{
	uint16_t *ptr = (uint16_t *)fw->data;
	uint32_t size = fw->size / 2; /* size in 2bytes */

	/* analysis info*/
	g_ctrl.desc.chip_id = ptr[size - DW9781_CHIPID_OFFSET];
	g_ctrl.desc.version = ptr[size - DW9781_VERSION_OFFSET];
	g_ctrl.desc.checksum = ptr[size - DW9781_CHECKSUM_OFFSET];
	g_ctrl.desc.fw_size = DW9781_FW_SIZE;

	CAM_INFO(CAM_OIS, "checksum:%x,fw_size:%x,size:%x",
		g_ctrl.desc.checksum, g_ctrl.desc.fw_size, size);
	CAM_INFO(CAM_OIS, "chip_id:%x,ver:%x",
		g_ctrl.desc.chip_id, g_ctrl.desc.checksum);
}

int32_t dw9781_fw_download(struct cam_ois_ctrl_t *o_ctrl)
{
	int32_t                            rc;
	const struct firmware             *fw = NULL;
	const char                        *fw_name = NULL;
	char                               name_buf[32] = {0};
	struct device                     *dev = &(o_ctrl->pdev->dev);

	if (!o_ctrl) {
		CAM_ERR(CAM_OIS, "Invalid Args");
		return -EINVAL;
	}

	if (strcmp(o_ctrl->opcode.fw_ver_name, "") == 0 ||
		strcmp(o_ctrl->opcode.fw_ver_name, "-") == 0)
			snprintf(name_buf, 32, "%s.prog", o_ctrl->ois_name);
	else
		snprintf(name_buf, 32, "%s_%s.prog", o_ctrl->ois_name,
			o_ctrl->opcode.fw_ver_name);

	fw_name = name_buf;
	CAM_INFO(CAM_OIS, "ois_name:%s, fw_ver_name:%s, fw:%s",
		o_ctrl->ois_name, o_ctrl->opcode.fw_ver_name, fw_name);
	/* Load FW */
	rc = request_firmware(&fw, fw_name, dev);
	if (rc) {
		CAM_ERR(CAM_OIS, "Failed to locate %s", fw_name);
		return rc;
	}
	dw9781_fw_analysis(fw);
	rc = dw9781_download_ois_fw(o_ctrl, fw);
	if (rc)
		CAM_ERR(CAM_OIS, "download_ois_fw fail");
	release_firmware(fw);
	return rc;
}