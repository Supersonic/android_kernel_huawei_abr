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
#include "hisi_fb_panel.h"

#define DTS_COMP_LCD_PANEL_TYPE "huawei,lcd_panel_type"
#define DTS_LCD_PANEL_TYPE "/huawei,lcd_panel"
#define I2C_DATA_LEN_ONE 1
#define I2C_DATA_LEN_TWO 2

static unsigned int get_i2c_number(unsigned char i2c_number)
{
	unsigned int i2c_num;

	switch (i2c_number) {
	case I2C_BUS_0:
		i2c_num = REG_BASE_I2C0;
		break;
	case I2C_BUS_1:
		i2c_num = REG_BASE_I2C1;
		break;
	case I2C_BUS_2:
		i2c_num = REG_BASE_I2C2;
		break;
	case I2C_BUS_3:
		i2c_num = REG_BASE_I2C3;
		break;
	case I2C_BUS_4:
		i2c_num = REG_BASE_I2C4;
		break;
#if !defined FASTBOOT_DISPLAY_LOGO_KIRIN980 && !defined FASTBOOT_DISPLAY_LOGO_KIRIN990
	case I2C_BUS_5:
		i2c_num = REG_BASE_I2C5;
		break;
#endif
	case I2C_BUS_6:
		i2c_num = REG_BASE_I2C6;
		break;
	case I2C_BUS_7:
		i2c_num = REG_BASE_I2C7;
		break;
	default:
		i2c_num = INVALID_VALUE;
		break;
	}
	return i2c_num;
}

static int bias_bl_i2c_read_u8(unsigned char i2c_num,
	unsigned char chip_addr, unsigned char reg, unsigned char *val)
{
	int ret;
	unsigned char addr_readdata[I2C_DATA_LEN_ONE] = { reg };
	struct i2c_operators *i2c_ops = get_operators(I2C_MODULE_NAME_STR);
	unsigned int i2c_chip_num;

	if (val == NULL) {
		FASTBOOT_ERR("NULL pointer!\n");
		return LCD_RET_FAIL;
	}

	if (i2c_ops == NULL ||
		i2c_ops->i2c_read == NULL ||
		i2c_ops->i2c_init == NULL ||
		i2c_ops->i2c_exit == NULL) {
		FASTBOOT_ERR("bad i2c_ops\n");
		return LCD_RET_FAIL;
	}

	i2c_chip_num = get_i2c_number(i2c_num);
	if (i2c_chip_num == INVALID_VALUE) {
		FASTBOOT_ERR("i2c bus number is invalid\n");
		return LCD_RET_FAIL;
	}

	i2c_ops->i2c_init(i2c_chip_num);
	ret = i2c_ops->i2c_read(i2c_chip_num,
		(unsigned char *)addr_readdata,
		sizeof(addr_readdata), chip_addr);
	if (ret < 0)
		FASTBOOT_INFO("i2c read error, reg = 0x%x\n",
			reg);

	i2c_ops->i2c_exit(i2c_chip_num);

	*val = addr_readdata[0];
	FASTBOOT_INFO("addr = 0x%x, reg = 0x%x, val = 0x%x\n",
		chip_addr, reg, addr_readdata[0]);
	return ret;
}

static int bias_bl_i2c_write_u8(unsigned char i2c_num,
	unsigned char chip_addr, unsigned char reg, unsigned char val)
{
	int ret;
	unsigned char I2C_write[I2C_DATA_LEN_TWO] = { reg, val };
	struct i2c_operators *i2c_ops = NULL;
	unsigned int i2c_chip_num;

	i2c_ops = get_operators(I2C_MODULE_NAME_STR);
	if (i2c_ops == NULL) {
		FASTBOOT_ERR("can not get i2c_ops!\n");
		return LCD_RET_FAIL;
	}

	if (i2c_ops->i2c_write == NULL ||
		i2c_ops->i2c_init == NULL ||
		i2c_ops->i2c_exit == NULL) {
		FASTBOOT_ERR("bad i2c_ops\n");
		return LCD_RET_FAIL;
	}

	i2c_chip_num = get_i2c_number(i2c_num);
	if (i2c_chip_num == INVALID_VALUE) {
		FASTBOOT_ERR("i2c bus number is invalid\n");
		return LCD_RET_FAIL;
	}

	i2c_ops->i2c_init(i2c_chip_num);
	ret = i2c_ops->i2c_write(i2c_chip_num, (unsigned char *)I2C_write,
		sizeof(I2C_write), chip_addr);
	if (ret < 0)
		FASTBOOT_INFO("i2c write error chip_addr = 0x%x, reg = 0x%x\n",
			chip_addr, reg);

	i2c_ops->i2c_exit(i2c_chip_num);

	FASTBOOT_INFO("lcd bias ic write addr = 0x%x, reg = 0x%x, val = 0x%x\n",
		chip_addr, reg, val);
	return ret;
}

static int bias_bl_i2c_update_u8(unsigned char i2c_num, unsigned char chip_addr,
	unsigned char reg, unsigned char mask, unsigned char val)
{
	int ret;
	unsigned char orig_val = 0;
	unsigned char value;

	ret = bias_bl_i2c_read_u8(i2c_num, chip_addr, reg, &orig_val);
	if (ret < 0) {
		FASTBOOT_INFO("i2c read error addr = 0x%x, reg = 0x%x\n",
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
	FASTBOOT_INFO("addr = 0x%x, reg = 0x%x, value = 0x%x\n",
		chip_addr, reg, val);
	return ret;
}

static int dts_get_data_by_property(const char *compatible,
	const char *prop_name, int *data, unsigned int *len)
{
#if !defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) && !defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	void *fdt = NULL;
	struct fdt_operators *fdt_ops = NULL;
	struct fdt_property *property = NULL;
	int offset;
	int ret_tmp;

	if (compatible == NULL || prop_name == NULL ||
		data == NULL || len == NULL) {
		FASTBOOT_ERR("input parameter is NULL\n");
		return LCD_RET_FAIL;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		FASTBOOT_ERR("can not get fdt_ops!\n");
		return LCD_RET_FAIL;
	}
	fdt = fdt_ops->get_dt_handle(FW_DTB_TYPE);
	offset = fdt_ops->fdt_node_offset_by_compatible(fdt, 0, compatible);
	if (offset < 0) {
		FASTBOOT_INFO("can not find %s node by compatible\n",
			compatible);
		return LCD_RET_FAIL;
	}
	property = (struct fdt_property *)fdt_ops->fdt_get_property(
		fdt, offset, prop_name, (int *)len);
	if (!property) {
		FASTBOOT_INFO("can not find %s\n", prop_name);
		return LCD_RET_FAIL;
	}
	ret_tmp = *((int *)property->data);
	*data = (int)fdt32_to_cpu((uint32_t)ret_tmp);
#endif
	return LCD_RET_OK;
}

static void dts_set_ic_name(const char *prop_name, const char *name)
{
#if !defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) && !defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	struct fdt_operators *fdt_ops = NULL;
	void *fdt = NULL;
	int ret;
	int offset;

	if ((prop_name == NULL) || (name == NULL))
		return;
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		FASTBOOT_ERR("can not get fdt_ops!\n");
		return;
	}
	fdt = fdt_ops->get_dt_handle(FW_DTB_TYPE);
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		FASTBOOT_INFO("fdt_open_into failed!\n");
		return;
	}
	ret = fdt_ops->fdt_path_offset(fdt, DTS_LCD_PANEL_TYPE);
	if (ret < 0) {
		FASTBOOT_INFO("not find panel node, change fb dts fail\n");
		return;
	}
	offset = ret;
	ret = fdt_ops->fdt_setprop_string(fdt, offset, prop_name, name);
	if (ret)
		FASTBOOT_INFO("Cannot update lcd panel type errno=%d\n",
			ret);
#endif
}

static int dts_get_ic_num(const char *prop_name)
{
#if !defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) && !defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	int num = 0;
	int ret;
	void *fdt_addr = NULL;
	struct fdt_operators *fdt_ops = NULL;

	if (prop_name == NULL) {
		FASTBOOT_ERR("prop_name is NULL!\n");
		return num;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (fdt_ops == NULL) {
		FASTBOOT_ERR("failed to get fdt operators!\n");
		return num;
	}
	fdt_addr = fdt_ops->get_dt_handle(DTS_NO_DISTINCT_TYPE);
	if (!fdt_addr) {
		FASTBOOT_ERR("fdt_addr is null!\n");
		return num;
	}
	if (fdt_ops->get_u32_by_compatible == NULL) {
		FASTBOOT_ERR("ops function is NULL\n");
		return num;
	}
	ret = fdt_ops->get_u32_by_compatible(fdt_addr, DTS_COMP_LCD_PANEL_TYPE,
		prop_name, &num);
	if (ret < 0) {
		FASTBOOT_INFO("get %s fail\n", prop_name);
		num = 0;
	}
	FASTBOOT_INFO("ic num is %d\n", num);
	return num;
#endif
	return LCD_RET_OK;
}

static int dts_get_ic_name_index(const char *prop_name, int idx,
	char *name_buf, int buf_len)
{
	int ret;

	if ((prop_name == NULL) || (name_buf == NULL) || (buf_len == 0)) {
		FASTBOOT_ERR("name or buf is NULL or buf_len is 0!\n");
		return LCD_RET_FAIL;
	}

	ret = get_dts_string_index(DTS_COMP_LCD_PANEL_TYPE,
		prop_name, idx, name_buf);
	if (ret < 0) {
		FASTBOOT_INFO("get %s fail\n", prop_name);
		return LCD_RET_FAIL;
	}

	return ret;
}

static int dts_set_status_by_compatible(const char *name, const char *status)
{
#if !defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) && !defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	struct fdt_operators *fdt_ops = NULL;
	void *fdt = NULL;
	int ret;
	int offset;

	if (!name) {
		FASTBOOT_ERR("name is null!\n");
		return LCD_RET_FAIL;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		FASTBOOT_ERR("can not get fdt_ops!\n");
		return LCD_RET_FAIL;
	}
	fdt = fdt_ops->get_dt_handle(FW_DTB_TYPE);
	if (!fdt) {
		FASTBOOT_ERR("failed to get fdt addr!\n");
		return LCD_RET_FAIL;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		FASTBOOT_INFO("fdt_open_into failed!\n");
		return LCD_RET_FAIL;
	}
	ret = fdt_ops->fdt_node_offset_by_compatible(fdt, 0, name);
	if (ret < 0) {
		FASTBOOT_INFO("can not find node, change dts status failed\n");
		return LCD_RET_FAIL;
	}
	offset = ret;
	ret = fdt_ops->fdt_setprop_string(fdt, offset, (const char *)"status",
		(const void *)status);
	if (ret)
		FASTBOOT_INFO("can not update dts status errno=%d\n",
			ret);
	return ret;
#endif
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
