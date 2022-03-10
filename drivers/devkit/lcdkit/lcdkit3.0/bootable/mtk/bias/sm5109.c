/*
 * sm5109.c
 *
 * sm5109 bias driver
 *
 * Copyright (c) 2020-2020 Huawei Technologies Co., Ltd.
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

#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <platform/mt_i2c.h>
#include "lcd_kit_common.h"
#include "lcd_kit_bias.h"
#include "sm5109.h"

#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1

static unsigned int i2c_bus_id;
static unsigned int check_status;

static struct sm5109_voltage voltage_table[] = {
	{ 4000000, SM5109_VOL_40 },
	{ 4100000, SM5109_VOL_41 },
	{ 4200000, SM5109_VOL_42 },
	{ 4300000, SM5109_VOL_43 },
	{ 4400000, SM5109_VOL_44 },
	{ 4500000, SM5109_VOL_45 },
	{ 4600000, SM5109_VOL_46 },
	{ 4700000, SM5109_VOL_47 },
	{ 4800000, SM5109_VOL_48 },
	{ 4900000, SM5109_VOL_49 },
	{ 5000000, SM5109_VOL_50 },
	{ 5100000, SM5109_VOL_51 },
	{ 5200000, SM5109_VOL_52 },
	{ 5300000, SM5109_VOL_53 },
	{ 5400000, SM5109_VOL_54 },
	{ 5500000, SM5109_VOL_55 },
	{ 5600000, SM5109_VOL_56 },
	{ 5700000, SM5109_VOL_57 },
	{ 5800000, SM5109_VOL_58 },
	{ 5900000, SM5109_VOL_59 },
	{ 6000000, SM5109_VOL_60 },
	{ 6100000, SM5109_VOL_61 },
	{ 6200000, SM5109_VOL_62 },
	{ 6300000, SM5109_VOL_63 },
	{ 6400000, SM5109_VOL_64 },
	{ 6500000, SM5109_VOL_65 }
};

static int sm5109_i2c_read_u8(unsigned char addr, unsigned char *databuffer)
{
	int ret;
	struct mt_i2c_t sm5109_i2c = {0};

	*databuffer = addr;
	sm5109_i2c.id = i2c_bus_id;
	sm5109_i2c.addr = SM5109_SLAV_ADDR;
	sm5109_i2c.mode = ST_MODE;
	sm5109_i2c.speed = SM5109_I2C_SPEED;
	sm5109_i2c.st_rs = I2C_TRANS_REPEATED_START;

	ret = i2c_write_read(&sm5109_i2c, databuffer,
		SM5109_CMD_LEN_ONE, SM5109_CMD_LEN_ONE);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read failed, ret: %d\n",
			__func__, ret);
	return ret;
}

static int sm5109_i2c_write_u8(unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[SM5109_CMD_LEN_TWO] = {0};
	struct mt_i2c_t sm5109_i2c = {0};

	write_data[0] = addr;
	write_data[1] = value;

	sm5109_i2c.id = i2c_bus_id;
	sm5109_i2c.addr = SM5109_SLAV_ADDR;
	sm5109_i2c.mode = ST_MODE;
	sm5109_i2c.speed = SM5109_I2C_SPEED;

	ret = i2c_write(&sm5109_i2c, write_data, SM5109_CMD_LEN_TWO);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);
	return ret;
}

static int sm5109_device_verify(void)
{
	int ret;
	unsigned char app_dis = 0;

	ret = sm5109_i2c_read_u8(SM5109_REG_APP_DIS, &app_dis);
	if (ret < 0) {
		LCD_KIT_ERR("no sm5109 device, read app_dis failed\n");
		return LCD_KIT_FAIL;
	}
	LCD_KIT_INFO("sm5109 verify ok, app_dis = 0x%x\n", app_dis);

	return LCD_KIT_OK;
}

static int sm5109_reg_init(unsigned char vpos_cmd, unsigned char vneg_cmd)
{
	unsigned char vpos = 0;
	unsigned char vneg = 0;
	unsigned char app_dis = 0;
	unsigned char vol_save_value = 0;
	int ret;

	ret = sm5109_i2c_read_u8(SM5109_REG_VPOS, &vpos);
	if (ret != 0) {
		LCD_KIT_ERR("read vpos voltage failed\n");
		return ret;
	}

	ret = sm5109_i2c_read_u8(SM5109_REG_VNEG, &vneg);
	if (ret != 0) {
		LCD_KIT_ERR("read vneg voltage failed\n");
		return ret;
	}

	ret = sm5109_i2c_read_u8(SM5109_REG_APP_DIS, &app_dis);
	if (ret != 0) {
		LCD_KIT_ERR("read app_dis failed\n");
		return ret;
	}

	vpos = (vpos & (~SM5109_REG_VOL_MASK)) | vpos_cmd;
	vneg = (vneg & (~SM5109_REG_VOL_MASK)) | vneg_cmd;
	app_dis = app_dis | SM5109_DISP_BIT | SM5109_DISN_BIT;

	ret = sm5109_i2c_write_u8(SM5109_REG_VPOS, vpos);
	if (ret != 0) {
		LCD_KIT_ERR("write vpos failed\n");
		return ret;
	}

	ret = sm5109_i2c_write_u8(SM5109_REG_VNEG, vneg);
	if (ret != 0) {
		LCD_KIT_ERR("write vneg failed\n");
		return ret;
	}

	ret = sm5109_i2c_write_u8(SM5109_REG_APP_DIS, app_dis);
	if (ret != 0) {
		LCD_KIT_ERR("write app_dis failed\n");
		return ret;
	}

	LCD_KIT_INFO("vpos = 0x%x, vneg = 0x%x, app_dis = 0x%x\n",
		vpos, vneg, app_dis);

	ret = sm5109_i2c_read_u8(SM5109_REG_VOL_SAVE, &vol_save_value);
	if (ret != 0)
		LCD_KIT_ERR("read vol_save_value failed,vol_save_value = 0x%x\n",vol_save_value);

	vol_save_value = vol_save_value | SM5109_VOL_SAVE_BIT;

	ret = sm5109_i2c_write_u8(SM5109_REG_VOL_SAVE, vol_save_value);
	if (ret != 0)
		LCD_KIT_ERR("save vol value failed, only apply to ocp2131, if ic is sm5109, err ingnore\n");

	LCD_KIT_INFO("vol_save_value = 0x%x\n",vol_save_value);

	return LCD_KIT_OK;
}

void sm5109_set_voltage(unsigned char vpos, unsigned char vneg)
{
	int ret;

	if (vpos >= SM5109_VOL_MAX) {
		LCD_KIT_ERR("set vpos error, vpos = %d is out of range\n", vpos);
		return;
	}

	if (vneg >= SM5109_VOL_MAX) {
		LCD_KIT_ERR("set vneg error, vneg = %d is out of range\n", vneg);
		return;
	}

	ret = sm5109_reg_init(vpos, vneg);
	if (ret)
		LCD_KIT_ERR("sm5109 reg init not success\n");
	else
		LCD_KIT_INFO("sm5109 set voltage succeed\n");
}

static int sm5109_set_bias(int vpos, int vneg)
{
	int i;
	unsigned char vpos_value;
	unsigned char vneg_value;
	int vol_size = ARRAY_SIZE(voltage_table);

	for (i = 0; i < vol_size; i++) {
		if (voltage_table[i].voltage == (unsigned int)vpos) {
			LCD_KIT_INFO("sm5109 vsp voltage:0x%x\n",
				voltage_table[i].value);
			vpos_value = voltage_table[i].value;
			break;
		}
	}

	if (i >= vol_size) {
		LCD_KIT_ERR("not found vsp voltage, use default voltage\n");
		vpos_value = SM5109_VOL_55;
	}

	for (i = 0; i < vol_size; i++) {
		if (voltage_table[i].voltage == (unsigned int)vneg) {
			LCD_KIT_INFO("sm5109 vsn voltage:0x%x\n",
				voltage_table[i].value);
			vneg_value = voltage_table[i].value;
			break;
		}
	}

	if (i >= vol_size) {
		LCD_KIT_ERR("not found vsn voltage, use default voltage\n");
		vneg_value = SM5109_VOL_55;
	}

	sm5109_set_voltage(vpos_value, vneg_value);
	return LCD_KIT_OK;
}

void sm5109_set_bias_status(void)
{
	int ret;
	int offset;
	void *kernel_fdt = NULL;

#ifdef DEVICE_TREE_SUPPORT
	kernel_fdt = get_kernel_fdt();
#endif
	if (kernel_fdt == NULL) {
		LCD_KIT_ERR("kernel_fdt is NULL\n");
		return;
	}

	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_SM5109);
	if (offset < 0) {
		LCD_KIT_ERR("can not find node, change status failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("can not update dts status errno = %d\n", ret);
		return;
	}

	LCD_KIT_INFO("sm5109 set bias status OK!\n");
}

static struct lcd_kit_bias_ops bias_ops = {
	.set_bias_voltage = sm5109_set_bias,
};

int sm5109_bias_recognize(void)
{
	int ret;

	ret = sm5109_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("sm5109 is not right bias\n");
	} else {
		lcd_kit_bias_register(&bias_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("sm5109 is right bias\n");
	}
	return ret;
}

int sm5109_init(void)
{
	int ret;
	unsigned int support = 0;

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_SM5109, SM5109_SUPPORT,
		&support, 0);
	if ((ret < 0) || (!support)) {
		LCD_KIT_ERR("not support sm5109\n");
		return LCD_KIT_OK;
	}

	/* register bias ops */
	lcd_kit_bias_recognize_register(sm5109_bias_recognize);
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_SM5109,
		SM5109_I2C_BUS_ID, &i2c_bus_id, 0);
	if (ret < 0) {
		i2c_bus_id = 0;
		return LCD_KIT_OK;
	}
	LCD_KIT_INFO("sm5109 is support\n");
	return LCD_KIT_OK;
}
