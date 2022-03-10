/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor dw9787 OIS driver
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
#include "ois_dw9787.h"

#define MCS_START_ADDRESS              0x8000
#define IF_START_ADDRESS               0x0000
#define MCS_PKT_SIZE                   32    /* 32 word */
#define IF_PKT_SIZE                    32    /* 32 word */
#define MCS_VALUE                      0x0002
#define IF_VALUE                       0x0001
#define MCS_FW_OFFSET_W                0x4000 /* MCS fw offset with 2bytes */

struct mem_desc_t {
	uint32_t type;
	uint16_t addr;
	uint32_t num_pages;
	uint32_t section_size;
	uint16_t section_value;
	uint32_t pkt_size;
	uint32_t mem_size;
};

struct fw_desc_t {
	uint32_t fw_size;
	uint32_t mcs_checksum;
	uint32_t if_checksum;
	uint16_t ver_major;
	uint16_t ver_minor;
	uint16_t ver_patch;
	uint16_t chip_id;
};

struct vendor_ois_ctrl_t {
	bool already_download;
	bool force_download;
	struct fw_desc_t desc;
};

enum mem_type {
	CODE_SECTION = 0,  /* MCS_SEC */
	DATA_SECTION,      /* IF_SEC */
};

static struct mem_desc_t g_mem_desc[] = {
	{ CODE_SECTION,  MCS_START_ADDRESS, 16, 0x800, MCS_VALUE, MCS_PKT_SIZE, 0x8000 },
	{ DATA_SECTION,  IF_START_ADDRESS,  4,  0x200, IF_VALUE,  IF_PKT_SIZE,  0x0800 }
};

/****************this should be only global define!!!*****************/
static struct vendor_ois_ctrl_t g_ctrl;
/****************this should be only global define!!!*****************/

static void dw9787_flash_access(struct cam_ois_ctrl_t *o_ctrl, uint16_t type)
{
	/* chip enable */
	ois_write_addr16_value16(o_ctrl, 0x0020, 0x0001);
	/* stanby mode(MCU off) */
	ois_write_addr16_value16(o_ctrl, 0x0024, 0x0000);
	/* code protection */
	ois_write_addr16_value16(o_ctrl, 0x0220, 0xC0D4);
	/* select program flash */
	ois_write_addr16_value16(o_ctrl, 0x3000, type);
	mdelay(1); /* flash_access delay 1ms */
}

static uint32_t dw9787_checksum_read(struct cam_ois_ctrl_t *o_ctrl, uint16_t type)
{
	uint16_t addr, size;
	uint16_t csh = 0;
	uint16_t csl = 0;
	uint32_t checksum;

	dw9787_flash_access(o_ctrl, type);
	/* Set the checksum area */
	if (type == CODE_SECTION) {
		addr = MCS_START_ADDRESS;
		size = 0x2000;
		CAM_INFO(CAM_OIS, "MCS Select");
	} else if (type == DATA_SECTION) {
		addr = IF_START_ADDRESS;
		size = 0x0200;
		CAM_INFO(CAM_OIS, "IF Select");
	} else {
		CAM_ERR(CAM_OIS, "not MCS or IF");
		return 0;
	}
	ois_write_addr16_value16(o_ctrl, 0x3048, addr); /* write addr */
	ois_write_addr16_value16(o_ctrl, 0x304C, size); /* 32bit(4byte) * 8192 */
	ois_write_addr16_value16(o_ctrl, 0x3050, 0x0001); /* write cmd read checksum */
	msleep(10); /* checksum_read delay 10ms, error when delay 1ms */
	ois_read_addr16_value16(o_ctrl, 0x3054, &csh); /* read csh */
	ois_read_addr16_value16(o_ctrl, 0x3058, &csl); /* read csl */
	checksum = ((uint32_t)(csh << 16)) | csl;
	CAM_INFO(CAM_OIS, "checksum calculated value: 0x%08x", checksum);
	/* code protection on */
	ois_write_addr16_value16(o_ctrl, 0x0220, 0x0000);

	return checksum;
}

static int32_t dw9787_internel_download(struct cam_ois_ctrl_t *o_ctrl,
	 const struct firmware *fw, enum mem_type type)
{
	int32_t i;
	uint16_t addr;
	uint32_t checksum;
	uint32_t fw_checksum;
	uint16_t *ptr = NULL;
	struct mem_desc_t *desc = &g_mem_desc[type];

	CAM_INFO(CAM_OIS, "fw size:%d, mem_size:%d", fw->size, desc->mem_size);
	if (type == CODE_SECTION) {
		fw_checksum = g_ctrl.desc.mcs_checksum;
		ptr = (uint16_t *)fw->data;
	} else {
		fw_checksum = g_ctrl.desc.if_checksum;
		ptr = (uint16_t *)fw->data + MCS_FW_OFFSET_W;
	}

	dw9787_flash_access(o_ctrl, desc->type);

	addr = desc->addr;
	/* erase MCS:32kBytes = 2K*16, IF: 2kBytes = 512B*4*/
	for (i = 0; i < desc->num_pages; i++) {
		/* erase address */
		ois_write_addr16_value16(o_ctrl, 0x3008, addr); /* Set sddress */
		/* erase sector MCS:2kbyte, IF:512byte */
		ois_write_addr16_value16(o_ctrl, 0x300C, desc->section_value);
		addr += desc->section_size; /* 2kbyte */
		msleep(5); /* Sector Erase delay 5ms */
	}
	/* Set mem write start Address */
	ois_write_addr16_value16(o_ctrl, 0x3028, desc->addr);

	/* flash fw write */
	for (i = 0; i < desc->mem_size / 2; i += desc->pkt_size)
		/* program sequential write 2K byte */
		ois_i2c_block_write_reg(o_ctrl, 0x302C,
			(uint16_t *)(ptr + i), desc->pkt_size, 1);

	/* Set the checksum area */
	checksum = dw9787_checksum_read(o_ctrl, desc->type);
	if (fw_checksum != checksum) {
		CAM_ERR(CAM_OIS, "checksum fail, bin: 0x%08x, read: 0x%08x",
			fw_checksum, checksum);
		/* chip disable */
		ois_write_addr16_value16(o_ctrl, 0x0220, 0x0000);
		return -EIO;
	}

	return 0;
}

static int32_t dw9787_auto_read_check(struct cam_ois_ctrl_t *o_ctrl)
{
	uint16_t autord_rv = 0;

	/* Check if flash data is normally auto read */
	ois_read_addr16_value16(o_ctrl, 0x305C, &autord_rv);
	if (autord_rv != 0) {
		CAM_ERR(CAM_OIS, "auto_read fail 0x%04X", autord_rv);
	} else {
		ois_write_addr16_value16(o_ctrl, 0x0018, 0x0001); /* Logic reset */
		msleep(4); /* reset reg 0x0018 delay 4ms */
		ois_write_addr16_value16(o_ctrl, 0x0024, 0x0001); /* Idle mode(MCU ON) */
		msleep(20); /* after MCU ON delay 20ms */
	}
	return autord_rv;
}

static int32_t dw9787_chipid_check(struct cam_ois_ctrl_t *o_ctrl)
{
	uint16_t chip_id = 0;

	ois_read_addr16_value16(o_ctrl, 0x0004, &chip_id); /* ID read */
	/* Check the chip_id of OIS ic */
	if (chip_id != g_ctrl.desc.chip_id) {
		CAM_ERR(CAM_OIS, "The module's OIS IC is not right 0x%04X", chip_id);
		return -ENODEV;
	}
	return 0;
}

int32_t dw9787_mem_download(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw)
{
	uint16_t fw_ver_major = 0;
	uint16_t fw_ver_minor = 0;
	uint16_t fw_ver_patch = 0;
	uint32_t checksum;

	/* check if loaded */
	if (g_ctrl.already_download) {
		CAM_INFO(CAM_OIS, "fw already downloaded");
		return 0;
	}

	if (dw9787_auto_read_check(o_ctrl))
		g_ctrl.force_download = true;

	/* Check the chip_id of OIS ic */
	if (dw9787_chipid_check(o_ctrl))
		return -ENODEV;

	/* Check the firmware version */
	ois_read_addr16_value16(o_ctrl, 0x9B08, &fw_ver_major); /* fw_ver_major */
	ois_read_addr16_value16(o_ctrl, 0x9B0A, &fw_ver_minor); /* fw_ver_minor */
	ois_read_addr16_value16(o_ctrl, 0x9B0C, &fw_ver_patch); /* fw_ver_patch */
	CAM_INFO(CAM_OIS, "fw version: %u.%u.%u",
		fw_ver_major, fw_ver_minor, fw_ver_patch);

	if ((fw_ver_major != g_ctrl.desc.ver_major) ||
		(fw_ver_minor != g_ctrl.desc.ver_minor) ||
		(fw_ver_patch != g_ctrl.desc.ver_patch) ||
		g_ctrl.force_download) {
		CAM_INFO(CAM_OIS, "another version is checked and download it");
		/* firmware download function */
		if (dw9787_internel_download(o_ctrl, fw, CODE_SECTION))
			return -EIO;
		if (dw9787_internel_download(o_ctrl, fw, DATA_SECTION))
			return -EIO;
	} else {
		CAM_INFO(CAM_OIS, "the ois firmware is lastest");
		checksum = dw9787_checksum_read(o_ctrl, CODE_SECTION);
		if (g_ctrl.desc.mcs_checksum != checksum) {
			CAM_ERR(CAM_OIS, "checksum error and download again");
			if (dw9787_internel_download(o_ctrl, fw, CODE_SECTION))
				return -EIO;
		}
		checksum = dw9787_checksum_read(o_ctrl, DATA_SECTION);
		if (g_ctrl.desc.if_checksum != checksum) {
			CAM_ERR(CAM_OIS, "checksum error and download again");
			if (dw9787_internel_download(o_ctrl, fw, DATA_SECTION))
				return -EIO;
		}
	}
	g_ctrl.already_download = true;
	CAM_INFO(CAM_OIS, "download_fw success");

	return 0;
}

static void dw9787_fw_analysis(const struct firmware *fw)
{
	uint16_t *ptr = (uint16_t *)fw->data;
	uint32_t index = fw->size / 2 - 1;

	/* analysis info*/
	index -= 2; /* reserved 2 bytes */
	g_ctrl.desc.chip_id = ptr[index--];
	g_ctrl.desc.ver_patch = ptr[index--];
	g_ctrl.desc.ver_minor = ptr[index--];
	g_ctrl.desc.ver_major = ptr[index--];
	g_ctrl.desc.if_checksum = ptr[index] + (ptr[index - 1] << 16);
	index -= 2; /* 2bytes for mcs checksum*/
	g_ctrl.desc.mcs_checksum = ptr[index] + (ptr[index - 1] << 16);
	index -= 2; /* 2bytes for if checksum*/
	g_ctrl.desc.fw_size = ptr[index] + (ptr[index - 1] << 16);
	CAM_INFO(CAM_OIS, "checksum:mcs:%x,if:%x,fw_size:%x", g_ctrl.desc.mcs_checksum,
		g_ctrl.desc.if_checksum, g_ctrl.desc.fw_size);
	CAM_INFO(CAM_OIS, "chip_id:%x,ver:%d.%d.%d",
		g_ctrl.desc.chip_id, g_ctrl.desc.ver_major,
		g_ctrl.desc.ver_minor, g_ctrl.desc.ver_patch);
}

int32_t dw9787_fw_download(struct cam_ois_ctrl_t *o_ctrl)
{
	int32_t                            rc = 0;
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
	dw9787_fw_analysis(fw);
	rc = dw9787_mem_download(o_ctrl, fw);
	if (rc) {
		CAM_ERR(CAM_OIS, "dw9787_mem_download fail : %d", rc);
		return rc;
	}
	release_firmware(fw);

	return rc;
}
