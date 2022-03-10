/*
 * rt4801h.c
 *
 * rt4801h bias driver
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

#include "rt4801h.h"
#include "lcd_kit_common.h"
#include "lcd_kit_bias.h"
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <platform/mt_i2c.h>

#define RT4801H_SLAV_ADDR 0x73
#define RT4801H_I2C_SPEED 100
#define RT4801H_CMD_LEN_ONE 1
#define RT4801H_CMD_LEN_TWO 2
#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1

static unsigned int i2c_bus_id;
static unsigned int check_status;
static struct rt4801h_voltage voltage_table[] = {
	{ LCD_BIAS_VOL_40, RT4801H_VOL_40_V },
	{ LCD_BIAS_VOL_41, RT4801H_VOL_41_V },
	{ LCD_BIAS_VOL_42, RT4801H_VOL_42_V },
	{ LCD_BIAS_VOL_43, RT4801H_VOL_43_V },
	{ LCD_BIAS_VOL_44, RT4801H_VOL_44_V },
	{ LCD_BIAS_VOL_45, RT4801H_VOL_45_V },
	{ LCD_BIAS_VOL_46, RT4801H_VOL_46_V },
	{ LCD_BIAS_VOL_47, RT4801H_VOL_47_V },
	{ LCD_BIAS_VOL_48, RT4801H_VOL_48_V },
	{ LCD_BIAS_VOL_49, RT4801H_VOL_49_V },
	{ LCD_BIAS_VOL_50, RT4801H_VOL_50_V },
	{ LCD_BIAS_VOL_51, RT4801H_VOL_51_V },
	{ LCD_BIAS_VOL_52, RT4801H_VOL_52_V },
	{ LCD_BIAS_VOL_53, RT4801H_VOL_53_V },
	{ LCD_BIAS_VOL_54, RT4801H_VOL_54_V },
	{ LCD_BIAS_VOL_55, RT4801H_VOL_55_V },
	{ LCD_BIAS_VOL_56, RT4801H_VOL_56_V },
	{ LCD_BIAS_VOL_57, RT4801H_VOL_57_V },
	{ LCD_BIAS_VOL_58, RT4801H_VOL_58_V },
	{ LCD_BIAS_VOL_59, RT4801H_VOL_59_V },
	{ LCD_BIAS_VOL_60, RT4801H_VOL_60_V }
};

static int rt4801h_i2c_read_u8(unsigned char addr, unsigned char *databuffer)
{
	int ret;
	unsigned char len;
	struct mt_i2c_t rt4801h_i2c = {0};

	*databuffer = addr;
	rt4801h_i2c.id = i2c_bus_id;
	rt4801h_i2c.addr = RT4801H_SLAV_ADDR;
	rt4801h_i2c.mode = ST_MODE;
	rt4801h_i2c.speed = RT4801H_I2C_SPEED;
	rt4801h_i2c.st_rs = I2C_TRANS_REPEATED_START;
	len = RT4801H_CMD_LEN_ONE;

	ret = i2c_write_read(&rt4801h_i2c, databuffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read  failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);
	return ret;
}

static int rt4801h_i2c_write_u8(unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[RT4801H_CMD_LEN_TWO] = {0};
	unsigned char len;
	struct mt_i2c_t rt4801h_i2c = {0};

	write_data[0] = addr;
	write_data[1] = value;

	rt4801h_i2c.id = i2c_bus_id;
	rt4801h_i2c.addr = RT4801H_SLAV_ADDR;
	rt4801h_i2c.mode = ST_MODE;
	rt4801h_i2c.speed = RT4801H_I2C_SPEED;
	len = RT4801H_CMD_LEN_TWO;

	ret = i2c_write(&rt4801h_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write  failed, reg is  0x%x ret: %d\n",
			__func__, addr, ret);
	return ret;
}

static int rt4801h_device_verify(void)
{
	int ret;
	unsigned char app_dis = 0;

	ret = rt4801h_i2c_read_u8(RT4801H_REG_APP_DIS, &app_dis);
	if (ret < 0) {
		LCD_KIT_ERR("no rt4801h device, read app_dis failed\n");
		return LCD_KIT_FAIL;
	}
	LCD_KIT_INFO("rt4801h verify ok, app_dis = 0x%x\n", app_dis);

	return LCD_KIT_OK;
}

static int rt4801h_reg_init(unsigned char vpos_cmd, unsigned char vneg_cmd)
{
	unsigned char vpos = 0;
	unsigned char vneg = 0;
	unsigned char app_dis = 0;
	int ret;

	ret = rt4801h_i2c_read_u8(RT4801H_REG_VPOS, &vpos);
	if (ret != 0) {
		LCD_KIT_ERR("read vpos voltage failed\n");
		return ret;
	}

	ret = rt4801h_i2c_read_u8(RT4801H_REG_VNEG, &vneg);
	if (ret != 0) {
		LCD_KIT_ERR("read vneg voltage failed\n");
		return ret;
	}

	ret = rt4801h_i2c_read_u8(RT4801H_REG_APP_DIS, &app_dis);
	if (ret != 0) {
		LCD_KIT_ERR("read app_dis failed\n");
		return ret;
	}

	vpos = (vpos & (~RT4801H_REG_VOL_MASK)) | vpos_cmd;
	vneg = (vneg & (~RT4801H_REG_VOL_MASK)) | vneg_cmd;

	app_dis = app_dis | RT4801H_APPS_BIT | RT4801H_DISP_BIT |
		RT4801H_DISN_BIT;

	ret = rt4801h_i2c_write_u8(RT4801H_REG_VPOS, vpos);
	if (ret != 0) {
		LCD_KIT_ERR("write vpos failed\n");
		return ret;
	}

	ret = rt4801h_i2c_write_u8(RT4801H_REG_VNEG, vneg);
	if (ret != 0) {
		LCD_KIT_ERR("write vneg failed\n");
		return ret;
	}

	ret = rt4801h_i2c_write_u8(RT4801H_REG_APP_DIS, app_dis);
	if (ret != 0)
		LCD_KIT_ERR("write app_dis failed\n");

	return ret;
}

void rt4801h_set_voltage(unsigned char vpos, unsigned char vneg)
{
	int ret;

	if (vpos >= RT4801H_VOL_MAX_V) {
		LCD_KIT_ERR("set vpos error, vpos = %d is out of range\n", vpos);
		return;
	}

	if (vneg >= RT4801H_VOL_MAX_V) {
		LCD_KIT_ERR("set vneg error, vneg = %d is out of range\n", vneg);
		return;
	}

	ret = rt4801h_reg_init(vpos, vneg);
	if (ret)
		LCD_KIT_ERR("rt4801h reg init not success\n");
	LCD_KIT_ERR("rt4801h set voltage succeed\n");
}

static int rt4801h_set_bias(int vpos, int vneg)
{
	int i;
	unsigned char vpos_value;
	unsigned char vneg_value;
	int vol_size = ARRAY_SIZE(voltage_table);

	for (i = 0; i < vol_size; i++) {
		if (voltage_table[i].voltage == (unsigned int)vpos) {
			LCD_KIT_INFO("rt4801h vsp voltage:0x%x\n",
				voltage_table[i].value);
			vpos_value = voltage_table[i].value;
			break;
		}
	}
	if (i >= vol_size) {
		LCD_KIT_ERR("not found vsp voltage, use default voltage\n");
		vpos_value = RT4801H_VOL_55_V;
	}
	for (i = 0; i < vol_size; i++) {
		if (voltage_table[i].voltage == (unsigned int)vneg) {
			LCD_KIT_INFO("rt4801h vsn voltage:0x%x\n",
				voltage_table[i].value);
			vneg_value = voltage_table[i].value;
			break;
		}
	}

	if (i >= vol_size) {
		LCD_KIT_ERR("not found vsn voltage, use default voltage\n");
		vneg_value = RT4801H_VOL_55_V;
	}
	LCD_KIT_INFO("vpos = 0x%x, vneg = 0x%x\n", vpos_value, vneg_value);
	rt4801h_set_voltage(vpos_value, vneg_value);
	return LCD_KIT_OK;
}

void rt4801h_set_bias_status(void)
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

	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_RT4801H);
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
		LCD_KIT_ERR("can not update dts status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("rt4801h set bias status OK!\n");
}

static struct lcd_kit_bias_ops bias_ops = {
	.set_bias_voltage = rt4801h_set_bias,
};

int rt4801h_bias_recognize(void)
{
	int ret;

	ret = rt4801h_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("rt4801h is not right bias\n");
	} else {
		lcd_kit_bias_register(&bias_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("rt4801h is right bias\n");
	}
	return ret;
}

int rt4801h_init(void)
{
	int ret;
	unsigned int support = 0;

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_RT4801H, RT4801H_SUPPORT,
		&support, 0);
	if ((ret < 0) || (!support)) {
		LCD_KIT_ERR("not support rt4801h\n");
		return LCD_KIT_OK;
	}

	/* register bias ops */
	lcd_kit_bias_recognize_register(rt4801h_bias_recognize);
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_RT4801H,
		RT4801H_I2C_BUS_ID, &i2c_bus_id, 0);
	if (ret < 0) {
		i2c_bus_id = 0;
		return LCD_KIT_OK;
	}
	LCD_KIT_INFO("rt4801h is support\n");
	return LCD_KIT_OK;
}
