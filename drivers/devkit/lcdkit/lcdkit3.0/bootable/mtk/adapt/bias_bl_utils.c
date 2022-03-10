/*
 * bias_bl_utils.c
 *
 * bias_bl_utils driver for hisi paltform i2c and dts
 * operation function
 *
 * Copyright (c) 2019-2019 Huawei Technologies Co., Ltd.
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
#include "bias_bl_utils.h"
#include "bias_bl_common.h"

#define MTK_I2C_STD_SPEED 100
#define I2C_DATA_LEN_ONE 1
#define I2C_DATA_LEN_TWO 2
#define DTS_START_OFFSET (-1)
#define DTS_COMP_LCD_PANEL_TYPE "huawei,lcd_panel_type"
#define DTS_LCD_PANEL_TYPE "/huawei,lcd_panel"

static unsigned int get_i2c_number(unsigned char i2c_number)
{
	unsigned int i2c_num;

	switch (i2c_number) {
	case I2C_BUS_0:
		i2c_num = I2C0;
		break;
	case I2C_BUS_1:
		i2c_num = I2C1;
		break;
	case I2C_BUS_2:
		i2c_num = I2C2;
		break;
	case I2C_BUS_3:
		i2c_num = I2C3;
		break;
	case I2C_BUS_4:
		i2c_num = I2C4;
		break;
	case I2C_BUS_5:
		i2c_num = I2C5;
		break;
	case I2C_BUS_6:
		i2c_num = I2C6;
		break;
	case I2C_BUS_7:
		i2c_num = I2C7;
		break;
	case I2C_BUS_8:
		i2c_num = I2C8;
		break;
	case I2C_BUS_9:
		i2c_num = I2C9;
		break;
	default:
		i2c_num = INVALID_VALUE;
		break;
	}
	return i2c_num;
}

static int bias_bl_i2c_read_u8(unsigned char i2c_num, unsigned char chip_addr,
	unsigned char reg, unsigned char *val)
{
	int ret;
	unsigned char len;
	struct mt_i2c_t i2c_info = {0};
	unsigned int i2c_chip_num;

	if (val == NULL) {
		FASTBOOT_ERR("NULL pointer!\n");
		return LCD_RET_FAIL;
	}

	*val = reg;
	i2c_chip_num = get_i2c_number(i2c_num);
	if (i2c_chip_num == INVALID_VALUE) {
		FASTBOOT_ERR("i2c bus number is invalid\n");
		return LCD_RET_FAIL;
	}

	i2c_info.id = i2c_chip_num;
	i2c_info.addr = chip_addr;
	i2c_info.mode = ST_MODE;
	i2c_info.speed = MTK_I2C_STD_SPEED;
	i2c_info.st_rs = I2C_TRANS_REPEATED_START;
	len = I2C_DATA_LEN_ONE;

	ret = i2c_write_read(&i2c_info, val, len, len);
	if (ret != 0)
		FASTBOOT_INFO("i2c_read  failed! reg is 0x%x ret: %d\n",
			reg, ret);
	FASTBOOT_INFO("lcd i2c read addr = 0x%x, reg = 0x%x, val = 0x%x\n",
		chip_addr, reg, *val);
	return ret;
}

static int bias_bl_i2c_write_u8(unsigned char i2c_num, unsigned char chip_addr,
	unsigned char reg, unsigned char val)
{
	int ret;
	unsigned char write_data[I2C_DATA_LEN_TWO] = {0};
	unsigned char len;
	struct mt_i2c_t i2c_info = {0};
	unsigned int i2c_chip_num;

	i2c_chip_num = get_i2c_number(i2c_num);
	if (i2c_chip_num == INVALID_VALUE) {
		FASTBOOT_ERR("i2c bus number is invalid\n");
		return LCD_RET_FAIL;
	}

	write_data[0] = reg;
	write_data[1] = val;

	i2c_info.id = i2c_chip_num;
	i2c_info.addr = chip_addr;
	i2c_info.mode = ST_MODE;
	i2c_info.speed = MTK_I2C_STD_SPEED;
	len = I2C_DATA_LEN_TWO;

	ret = i2c_write(&i2c_info, write_data, len);
	if (ret != 0)
		FASTBOOT_INFO("i2c_write  failed! reg is  0x%x ret: %d\n",
			reg, ret);
	FASTBOOT_INFO("lcd i2c write addr = 0x%x, reg = 0x%x, val = 0x%x\n",
		chip_addr, reg, val);
	return ret;
}

static int bias_bl_i2c_update_u8(unsigned char i2c_num, unsigned char chip_addr,
	unsigned char reg, unsigned char mask, unsigned char val)
{
	int ret;
	unsigned char orig_val;
	unsigned char value;

	ret = bias_bl_i2c_read_u8(i2c_num, chip_addr, reg, &orig_val);
	if (ret < 0) {
		FASTBOOT_INFO("i2c read error chip_addr = 0x%x, reg = 0x%x\n",
			chip_addr, reg);
		return ret;
	}

	value = orig_val & (~mask);
	value |= (val & mask);

	if (value != orig_val) {
		ret = bias_bl_i2c_write_u8(i2c_num, chip_addr, reg, value);
		if (ret < 0) {
			FASTBOOT_INFO("error addr = 0x%x, reg = 0x%x\n",
				chip_addr, reg);
			return ret;
		}
	}
	FASTBOOT_INFO("i2c update  addr = 0x%x, reg = 0x%x, value = 0x%x\n",
		chip_addr, reg, val);
	return ret;
}

static int dts_get_data_by_property(const char *compatible,
	const char *prop_name, int *data, unsigned int *len)
{
	int offset;
	int prop_len = 0;
	const void *prop_data = NULL;
	void *g_fdt = NULL;

	if (compatible == NULL || prop_name == NULL ||
		data == NULL || len == NULL) {
		FASTBOOT_ERR("input parameter is NULL\n");
		return LCD_RET_FAIL;
	}

	g_fdt = get_lk_overlayed_dtb();
	if (g_fdt == NULL) {
		FASTBOOT_ERR("failed to get fdt addr!\n");
		return LCD_RET_FAIL;
	}

	offset = fdt_node_offset_by_compatible(g_fdt, DTS_START_OFFSET, compatible);
	if (offset < 0) {
		FASTBOOT_INFO("can not find %s node by compatible\n",
			compatible);
		return LCD_RET_FAIL;
	}

	prop_data = fdt_getprop(g_fdt, offset, prop_name, &prop_len);
	if (prop_len > 0 && prop_data)
		*data = fdt32_to_cpu(*(unsigned int *)prop_data);
	else
		*data = 0;

	return LCD_RET_OK;
}

static void dts_set_ic_name(const char *prop_name, const char *name)
{
	int ret;
	int offset;
	void *g_fdt = NULL;

	if ((prop_name == NULL) || (name == NULL)) {
		FASTBOOT_ERR("input parameter is NULL\n");
		return;
	}

#ifdef DEVICE_TREE_SUPPORT
	g_fdt = get_kernel_fdt();
#endif
	if (g_fdt == NULL) {
		FASTBOOT_ERR("failed to get fdt addr!\n");
		return;
	}

	offset = fdt_node_offset_by_compatible(g_fdt, DTS_START_OFFSET,
		DTS_COMP_LCD_PANEL_TYPE);
	if (offset < 0) {
		FASTBOOT_INFO("can not find %s node by compatible\n",
			DTS_COMP_LCD_PANEL_TYPE);
		return;
	}

	ret = fdt_setprop_string(g_fdt, offset, prop_name,
		(const void *)name);
	if (ret) {
		FASTBOOT_INFO("set prop %s faile errno=%d\n",
			prop_name, ret);
		return;
	}

	FASTBOOT_INFO("set prop %s to %s OK!\n", prop_name, name);
}

static int dts_get_ic_num(const char *prop_name)
{
	int num = 0;
	int offset;
	int prop_len = 0;
	const void *prop_data = NULL;
	void *g_fdt = NULL;

	if (prop_name == NULL) {
		FASTBOOT_ERR("prop_name is NULL!\n");
		return num;
	}

	g_fdt = get_lk_overlayed_dtb();
	if (g_fdt == NULL) {
		FASTBOOT_ERR("failed to get fdt addr!\n");
		return num;
	}

	offset = fdt_node_offset_by_compatible(g_fdt, DTS_START_OFFSET,
		DTS_COMP_LCD_PANEL_TYPE);
	if (offset < 0) {
		FASTBOOT_INFO("can not find %s node by compatible\n",
			DTS_COMP_LCD_PANEL_TYPE);
		return num;
	}

	prop_data = fdt_getprop(g_fdt, offset, prop_name, &prop_len);
	if (prop_len > 0 && prop_data)
		num = fdt32_to_cpu(*(unsigned int *)prop_data);
	else
		num = 0;

	FASTBOOT_INFO("ic num is %d\n", num);
	return num;
}

static int dts_get_ic_name_index(const char *prop_name, int idx,
	char *name_buf, int buf_len)
{
	int offset;
	int length = 0;
	const char *list = NULL;
	void *g_fdt = NULL;

	if ((prop_name == NULL) || (name_buf == NULL) || (buf_len == 0)) {
		FASTBOOT_ERR("name or buf is NULL or buf_len is 0\n");
		return LCD_RET_FAIL;
	}

	g_fdt = get_lk_overlayed_dtb();
	if (g_fdt == NULL) {
		FASTBOOT_ERR("failed to get fdt addr!\n");
		return LCD_RET_FAIL;
	}

	offset = fdt_node_offset_by_compatible(g_fdt, DTS_START_OFFSET,
		DTS_COMP_LCD_PANEL_TYPE);
	if (offset < 0) {
		FASTBOOT_INFO("can not find %s node by compatible\n",
			DTS_COMP_LCD_PANEL_TYPE);
		return LCD_RET_FAIL;
	}

	list = fdt_stringlist_get(g_fdt, offset, prop_name, idx, &length);
	if ((list == NULL) || (length <= 0)) {
		FASTBOOT_INFO("parse %s fail\n", prop_name);
		return LCD_RET_FAIL;
	}

	if (length > buf_len)
		length = buf_len - 1;

	memcpy(name_buf, list, length);
	return LCD_RET_OK;
}

static int dts_set_status_by_compatible(const char *name, const char *status)
{
	int ret;
	int offset;
	void *g_fdt = NULL;

	if ((name == NULL) || (status == NULL)) {
		FASTBOOT_ERR("input parameter is null!\n");
		return LCD_RET_FAIL;
	}

#ifdef DEVICE_TREE_SUPPORT
	g_fdt = get_kernel_fdt();
#endif
	if (g_fdt == NULL) {
		FASTBOOT_ERR("failed to get fdt addr!\n");
		return LCD_RET_FAIL;
	}

	offset = fdt_node_offset_by_compatible(g_fdt, DTS_START_OFFSET, name);
	if (offset < 0) {
		FASTBOOT_INFO("not find %s node by compatible\n",
			name);
		return LCD_RET_FAIL;
	}

	ret = fdt_setprop_string(g_fdt, offset, (const char *)"status", status);
	if (ret) {
		FASTBOOT_INFO("set prop %s faile errno=%d\n",
			status, ret);
		return LCD_RET_FAIL;
	}

	return LCD_RET_OK;
}

static struct bias_bl_common_ops g_bias_bl_common_ops = {
	.i2c_read_u8 = bias_bl_i2c_read_u8,
	.i2c_write_u8 = bias_bl_i2c_write_u8,
	.i2c_update_u8 = bias_bl_i2c_update_u8,
	.dts_get_data_by_property = dts_get_data_by_property,
	.dts_set_ic_name = dts_set_ic_name,
	.dts_get_ic_num = dts_get_ic_num,
	.dts_get_ic_name_index = dts_get_ic_name_index,
	.dts_set_status_by_compatible = dts_set_status_by_compatible,
};

void bias_bl_ops_init(void)
{
	bias_bl_adapt_register(&g_bias_bl_common_ops);
}
