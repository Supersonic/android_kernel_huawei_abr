/*
 * lcd_kit_effect.c
 *
 * lcdkit display effect for lcd driver
 *
 * Copyright (c) 2018-2019 Huawei Technologies Co., Ltd.
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

#include "lcd_kit_effect.h"
#include "lcd_kit_common.h"
#include "lcd_kit_disp.h"
#include <stdint.h>

#include "oeminfo_api.h"
#ifdef CFG_USBLOADER_SECURITY
#include "sec_api_ops.h"
#include "sec_api.h"
#include "sec_api_internal.h"
#endif

#define MAX_READ_SIZE 20

static struct lcdbrightnesscoloroeminfo lcd_color_oeminfo = {0};
static struct panelid lcd_panelid = {0};
static unsigned char sncode_cur[LCD_KIT_SNCODE_SIZE] = {0};
static unsigned char sncode_oem[LCD_KIT_SNCODE_SIZE] = {0};
static struct lcd_kit_tplcd_info tplcd = {
	.sn_check = '0',
	.sn_hash = {0},
};
static int gamma_len;
/*
 * 1542 equals degamma_r + degamma_g + degamma_b
 * equals (257 + 257 +257) * sizeof(u16)
 */
static uint16_t gamma_lut[GM_LUT_LEN] = {0};

static int lcd_kit_get_cur_sn(struct hisi_fb_data_type *hisifd)
{
	int ret;
	char read_value[LCD_KIT_SNCODE_SIZE + 1] = {0};

	if (!hisifd) {
		LCD_KIT_ERR("hisifd is null\n");
		return LCD_KIT_FAIL;
	}
	ret = lcd_kit_dsi_cmds_rx(hisifd, (uint8_t *)read_value,
		LCD_KIT_SNCODE_SIZE,
		&disp_info->sn_code.cmds);
	if (ret != 0) {
		LCD_KIT_ERR("get sn_code error!\n");
		return LCD_KIT_FAIL;
	}

	ret = memcpy_s(sncode_cur, LCD_KIT_SNCODE_SIZE, read_value, LCD_KIT_SNCODE_SIZE);
	if (ret != 0) {
		LCD_KIT_ERR("memcpy_s error\n");
		return LCD_KIT_FAIL;
	}

	return LCD_KIT_OK;
}

static int lcd_kit_get_oem_sn(void)
{
#ifdef CFG_USBLOADER_SECURITY
	if (common_reused_oeminfo_read(OEMINFO_REUSED_ID_204_8K_VALID_SIZE_64_BYTE,
		(unsigned int)OEMINFO_SNCODE, (char *)sncode_oem, LCD_KIT_SNCODE_SIZE)) {
		LCD_KIT_ERR("get sn from oeminfo error\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
#else
	return LCD_KIT_FAIL;
#endif
}

static int lcd_kit_get_hash_code(void)
{
	uint8_t sha256_sn[HASH_LEN + 1] = {0};
#ifndef FASTBOOT_DDR_TEST
#ifdef CFG_USBLOADER_SECURITY
	struct data_materials sn;
	sn.data = (uint8_t *)sncode_cur;
	sn.data_len = strlen((char *)sncode_cur);
	if (data_SHA256(&sn, 0, sha256_sn, HASH_LEN) != 0) {
		LCD_KIT_ERR("data_SHA256 fail\n");
		return LCD_KIT_FAIL;
	}
#endif
#endif
	int i;
	char ch[3] = {0}; // store a hex character
	for (i = 0; i < HASH_LEN; i++) {
		if (snprintf_s(ch, sizeof(ch), sizeof(ch) - 1, "%02x", sha256_sn[i] & 0xff) < 0) {
			LCD_KIT_ERR("snprintf_s fail\n");
			return LCD_KIT_FAIL;
		}
		tplcd.sn_hash[2 * i] = ch[0]; // 1 hex character expressed by 2 bytes
		tplcd.sn_hash[2 * i + 1] = ch[1];
		if (memset_s(ch, sizeof(ch), 0, sizeof(ch)) != EOK) {
			LCD_KIT_ERR("memset_s fail\n");
			return LCD_KIT_FAIL;
		}
	}
	tplcd.sn_hash[HASH_LEN * 2] = '\0';
	return LCD_KIT_OK;
}

static int lcd_kit_get_tplcd_info(struct hisi_fb_data_type *hisifd)
{
	int i;
	tplcd.sn_check = '0';
	if (lcd_kit_get_cur_sn(hisifd) != 0) {
		for (i = 0; i < (HASH_LEN * 2); i++)
			tplcd.sn_hash[i] = '0';
		LCD_KIT_INFO("cannot get cur sn code\n");
		return LCD_KIT_OK;
	}
	if (lcd_kit_get_oem_sn() != 0) {
		for (i = 0; i < (HASH_LEN * 2); i++)
			tplcd.sn_hash[i] = '1';
		LCD_KIT_INFO("cannot get oem sn code\n");
		return LCD_KIT_OK;
	}

	if (strncmp((char *)sncode_oem, (char *)sncode_cur, LCD_KIT_SNCODE_SIZE) == 0) {
		tplcd.sn_check = '1';
		LCD_KIT_INFO("tplcd.sn_check ok!\n");
		return LCD_KIT_OK;
	}
	LCD_KIT_INFO("sn code check not right, need report\n");
	if (lcd_kit_get_hash_code() != 0) {
		LCD_KIT_ERR("get hash code fail\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

static unsigned long lcd_kit_get_sn_mem_addr(void)
{
	return (HISI_SUB_RESERVED_BRIGHTNESS_CHROMA_MEM_PHYMEM_BASE +
		HISI_SUB_RESERVED_BRIGHTNESS_CHROMA_MEM_PHYMEM_SIZE -
		sizeof(struct lcd_kit_tplcd_info));
}

int lcd_kit_write_tplcd_info_to_mem(struct hisi_fb_data_type *hisifd)
{
	int ret;
	unsigned long sn_mem_addr = lcd_kit_get_sn_mem_addr();
	unsigned long sn_mem_size = sizeof(struct lcd_kit_tplcd_info);

	if (lcd_kit_get_tplcd_info(hisifd) != 0) {
		LCD_KIT_ERR("get tplcd info error\n");
		return LCD_KIT_FAIL;
	}

	ret = memcpy_s((void *)(uintptr_t)sn_mem_addr, sn_mem_size, &tplcd, sizeof(tplcd));
	if (ret != 0) {
		LCD_KIT_ERR("memcpy_s error\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

int lcd_kit_get_bright_rgbw_id_from_oeminfo(void)
{
	struct oeminfo_operators *oeminfo_ops = NULL;

	oeminfo_ops = get_operators(OEMINFO_MODULE_NAME_STR);
	if (!oeminfo_ops) {
		HISI_FB_ERR("get_operators error\n");
		return LCD_KIT_FAIL;
	}

	if (oeminfo_ops->get_oeminfo(OEMINFO_LCD_BRIGHT_COLOR_CALIBRATION,
		sizeof(struct lcdbrightnesscoloroeminfo),
		(char *)&lcd_color_oeminfo)) {
		HISI_FB_ERR("get bright color oeminfo error\n");
		return LCD_KIT_FAIL;
	}

	return LCD_KIT_OK;
}

int lcd_kit_set_bright_rgbw_id_to_oeminfo(void)
{
	struct oeminfo_operators *oeminfo_ops = NULL;

	oeminfo_ops = get_operators(OEMINFO_MODULE_NAME_STR);
	if (!oeminfo_ops) {
		HISI_FB_ERR("get_operators error\n");
		return LCD_KIT_FAIL;
	}

	if (oeminfo_ops->set_oeminfo(OEMINFO_LCD_BRIGHT_COLOR_CALIBRATION,
		sizeof(struct lcdbrightnesscoloroeminfo),
		(char *)&lcd_color_oeminfo)) {
		HISI_FB_ERR("set bright color oeminfo error\n");
		return LCD_KIT_FAIL;
	}

	return LCD_KIT_OK;
}

int lcd_kit_get_panel_id_info(struct hisi_fb_data_type *hisifd)
{
	int ret;
	char read_value[MAX_READ_SIZE] = {0};

	ret = lcd_kit_dsi_cmds_rx((void *)hisifd, (uint8_t *)read_value,
		MAX_READ_SIZE,
		&disp_info->brightness_color_uniform.modulesn_cmds);
	if (ret != 0)
		LCD_KIT_ERR("rx failed\n");
	/* 24 means 24bits 16 means 16bits 8 means 8bits */
	lcd_panelid.modulesn = (((uint32_t)read_value[0] & 0xFF) << 24) |
			       (((uint32_t)read_value[1] & 0xFF) << 16) |
			       (((uint32_t)read_value[2] & 0xFF) << 8)  |
			       ((uint32_t)read_value[3] & 0xFF);

	ret = memset_s(read_value, MAX_READ_SIZE, 0, MAX_READ_SIZE);
	if (ret != 0) {
		LCD_KIT_ERR("memset_s error\n");
		return LCD_KIT_FAIL;
	}
	ret = lcd_kit_dsi_cmds_rx((void *)hisifd, (uint8_t *)read_value,
		MAX_READ_SIZE,
		&disp_info->brightness_color_uniform.equipid_cmds);
	if (ret < 0) {
		HISI_FB_ERR("check_cmds fail\n");
		return ret;
	}
	lcd_panelid.equipid = (uint32_t)read_value[0];
	ret = memset_s(read_value, MAX_READ_SIZE, 0, MAX_READ_SIZE);
	if (ret != 0) {
		LCD_KIT_ERR("memset_s error\n");
		return LCD_KIT_FAIL;
	}
	ret = lcd_kit_dsi_cmds_rx((void *)hisifd, (uint8_t *)read_value,
		MAX_READ_SIZE,
		&disp_info->brightness_color_uniform.modulemanufact_cmds);
	if (ret < 0) {
		HISI_FB_ERR("check_cmds fail\n");
		return ret;
	}
	/* 16 means 16 bits 8 means 8 bits */
	lcd_panelid.modulemanufactdate = (((uint32_t)read_value[0] & 0xFF) << 16) |
					 (((uint32_t)read_value[1] & 0xFF) << 8)  |
					 ((uint32_t)read_value[2] & 0xFF);

	lcd_panelid.vendorid = 0xFFFF;
	LCD_KIT_INFO("lcd_panelid.modulesn = 0x%x\n", lcd_panelid.modulesn);
	LCD_KIT_INFO("lcd_panelid.equipid = 0x%x\n", lcd_panelid.equipid);
	LCD_KIT_INFO("lcd_panelid.modulemanufactdate = 0x%x\n",
		lcd_panelid.modulemanufactdate);
	LCD_KIT_INFO("lcd_panelid.vendorid = 0x%x\n", lcd_panelid.vendorid);
	return LCD_KIT_OK;
}

unsigned long lcd_kit_get_color_mem_addr(void)
{
	return HISI_SUB_RESERVED_BRIGHTNESS_CHROMA_MEM_PHYMEM_BASE;
}

unsigned long lcd_kit_get_color_mem_size(void)
{
	return HISI_SUB_RESERVED_BRIGHTNESS_CHROMA_MEM_PHYMEM_SIZE;
}

void lcd_kit_write_oeminfo_to_mem(void)
{
	int ret;
	unsigned long mem_addr;
	unsigned long mem_size;

	mem_addr = lcd_kit_get_color_mem_addr();
	mem_size = lcd_kit_get_color_mem_size();
	ret = memcpy_s((void *)(uintptr_t)mem_addr, mem_size, &lcd_color_oeminfo,
		sizeof(lcd_color_oeminfo));
	if (ret != 0)
		LCD_KIT_ERR("memcpy_s error\n");
}

void lcd_kit_bright_rgbw_id_from_oeminfo(struct hisi_fb_data_type *hisifd)
{
	int ret;

	if (!hisifd) {
		HISI_FB_ERR("pointer is NULL\n");
		return;
	}
	ret = lcd_kit_get_panel_id_info(hisifd);
	if (ret != LCD_KIT_OK)
		HISI_FB_ERR("read panel id from lcd failed!\n");

	ret = lcd_kit_get_bright_rgbw_id_from_oeminfo();
	if (ret != LCD_KIT_OK)
		HISI_FB_ERR("read oeminfo failed!\n");

	if (lcd_color_oeminfo.id_flag == 0) {
		HISI_FB_INFO("panel id_flag is 0\n");
		lcd_color_oeminfo.panel_id = lcd_panelid;
		lcd_color_oeminfo.id_flag = 1;
		ret = lcd_kit_set_bright_rgbw_id_to_oeminfo();
		if (ret != LCD_KIT_OK)
			HISI_FB_ERR("write oeminfo failed!\n");
		lcd_kit_write_oeminfo_to_mem();
		return;
	}
	if (!memcmp(&lcd_color_oeminfo.panel_id, &lcd_panelid,
		sizeof(lcd_color_oeminfo.panel_id))) {
		HISI_FB_INFO("panel id is same\n");
		lcd_kit_write_oeminfo_to_mem();
		return;
	}
	lcd_color_oeminfo.panel_id = lcd_panelid;
	if ((lcd_color_oeminfo.panel_id.modulesn == lcd_panelid.modulesn) &&
		(lcd_color_oeminfo.panel_id.equipid == lcd_panelid.equipid) &&
		(lcd_color_oeminfo.panel_id.modulemanufactdate == lcd_panelid.modulemanufactdate)) {
		HISI_FB_INFO("panle id and oeminfo panel id is same\n");
		if (lcd_color_oeminfo.tc_flag == 0) {
			HISI_FB_INFO("panle id and oeminfo panel id is same, set tc_flag to true\n");
			lcd_color_oeminfo.tc_flag = 1;
		}
	} else {
		HISI_FB_INFO("panle id and oeminfo panel id is not same, set tc_flag to false\n");
		lcd_color_oeminfo.tc_flag = 0;
	}
	ret = lcd_kit_set_bright_rgbw_id_to_oeminfo();
	if (ret != LCD_KIT_OK)
		HISI_FB_ERR("write panel id wight and color info to oeminfo failed!\n");
	lcd_kit_write_oeminfo_to_mem();
	return;
}

static int lcd_kit_read_gamma_from_oeminfo(void)
{

	struct oeminfo_operators *oeminfo_ops = NULL;

	oeminfo_ops = get_operators(OEMINFO_MODULE_NAME_STR);
	if (!oeminfo_ops) {
		HISI_FB_ERR("get_operators error\n");
		return LCD_KIT_FAIL;
	}

	if (oeminfo_ops->get_oeminfo(OEMINFO_GAMMA_INDEX, OEMINFO_GAMMA_LEN,
		(char *)&gamma_len)) {
		HISI_FB_ERR("get gamma oeminfo len error, gamma len = %d!\n",
			gamma_len);
		return LCD_KIT_FAIL;
	}
	if (gamma_len != GM_IGM_LEN) {
		HISI_FB_ERR("gamma oeminfo len is not correct\n");
		return LCD_KIT_FAIL;
	}

	if (oeminfo_ops->get_oeminfo(OEMINFO_GAMMA_DATA, GM_IGM_LEN,
		(char *)gamma_lut)) {
		HISI_FB_ERR("get gamma oeminfo data error\n");
		return LCD_KIT_FAIL;
	}

	return LCD_KIT_OK;
}

int lcd_kit_write_gm_to_reserved_mem(void)
{
	int ret;
	unsigned long gm_addr = HISI_SUB_RESERVED_LCD_GAMMA_MEM_PHYMEM_BASE;
	unsigned long gm_size = HISI_SUB_RESERVED_LCD_GAMMA_MEM_PHYMEM_SIZE;

	if (gm_size < GM_IGM_LEN + OEMINFO_GAMMA_LEN) {
		HISI_FB_ERR("gamma mem size is not enough !\n");
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_read_gamma_from_oeminfo();
	if (ret) {
		writel(0, gm_addr);
		HISI_FB_ERR("can not get oeminfo gamma data!\n");
		return LCD_KIT_FAIL;
	}

	writel((unsigned int)GM_IGM_LEN, gm_addr);
	gm_addr += OEMINFO_GAMMA_LEN;
	ret = memcpy_s((void *)(uintptr_t)gm_addr, gm_size, gamma_lut, GM_IGM_LEN);
	if (ret != 0) {
		LCD_KIT_ERR("memcpy_s error\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}
