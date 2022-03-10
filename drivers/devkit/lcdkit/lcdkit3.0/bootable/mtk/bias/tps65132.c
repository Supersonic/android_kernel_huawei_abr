/*
 * tps65132.c
 *
 * tps65132 bias driver
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

#include "tps65132.h"
#include "lcd_kit_common.h"
#include "lcd_kit_bias.h"
#include "lcd_kit_utils.h"
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <platform/mt_i2c.h>

#define TPS65132_SLAV_ADDR	0x3E
#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1

static unsigned int i2c_bus_id;
static unsigned int check_status;
static struct tps65132_voltage voltage_table[] = {
	{ 4000000, TPS65132_VOL_40_V },
	{ 4100000, TPS65132_VOL_41_V },
	{ 4200000, TPS65132_VOL_42_V },
	{ 4300000, TPS65132_VOL_43_V },
	{ 4400000, TPS65132_VOL_44_V },
	{ 4500000, TPS65132_VOL_45_V },
	{ 4600000, TPS65132_VOL_46_V },
	{ 4700000, TPS65132_VOL_47_V },
	{ 4800000, TPS65132_VOL_48_V },
	{ 4900000, TPS65132_VOL_49_V },
	{ 5000000, TPS65132_VOL_50_V },
	{ 5100000, TPS65132_VOL_51_V },
	{ 5200000, TPS65132_VOL_52_V },
	{ 5300000, TPS65132_VOL_53_V },
	{ 5400000, TPS65132_VOL_54_V },
	{ 5500000, TPS65132_VOL_55_V },
	{ 5600000, TPS65132_VOL_56_V },
	{ 5700000, TPS65132_VOL_57_V },
	{ 5800000, TPS65132_VOL_58_V },
	{ 5900000, TPS65132_VOL_59_V },
	{ 6000000, TPS65132_VOL_60_V },
};
static int tps65132_i2c_read_u8(unsigned char addr, unsigned char *dataBuffer)
{
	int ret;
	unsigned char len;
	struct mt_i2c_t tps65132_i2c = {0};
	*dataBuffer = addr;

	tps65132_i2c.id = i2c_bus_id;
	tps65132_i2c.addr = TPS65132_SLAV_ADDR;
	tps65132_i2c.mode = ST_MODE;
	tps65132_i2c.speed = 100;
	tps65132_i2c.st_rs = I2C_TRANS_REPEATED_START;
	len = 1;

	ret = i2c_write_read(&tps65132_i2c, dataBuffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read  failed! reg is 0x%x ret: %d\n",
			__func__, addr, ret);
	return ret;
}

static int tps65132_i2c_write_u8(unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[2] = {0};
	unsigned char len;
	struct mt_i2c_t tps65132_i2c = {0};

	write_data[0] = addr;
	write_data[1] = value;

	tps65132_i2c.id = i2c_bus_id;
	tps65132_i2c.addr = TPS65132_SLAV_ADDR;
	tps65132_i2c.mode = ST_MODE;
	tps65132_i2c.speed = 100;
	len = 2;

	ret = i2c_write(&tps65132_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write  failed! reg is  0x%x ret: %d\n",
			__func__, addr, ret);
	return ret;
}

static int tps65132_reg_inited(unsigned char vpos_target_cmd,
	unsigned char vneg_target_cmd)
{
	unsigned char vpos = 0;
	unsigned char vneg = 0;
	int ret;

	ret = tps65132_i2c_read_u8(TPS65132_REG_VPOS, &vpos);
	if (ret != 0) {
		LCD_KIT_ERR("[%s]:read vpos voltage failed\n", __func__);
		goto exit;
	}

	ret = tps65132_i2c_read_u8(TPS65132_REG_VNEG, &vneg);
	if (ret != 0) {
		LCD_KIT_ERR("[%s]:read vneg voltage failed\n", __func__);
		goto exit;
	}

	LCD_KIT_INFO("vpos : 0x%x, vneg: 0x%x\n", vpos, vpos);

	if (((vpos & TPS65132_REG_VOL_MASK) == vpos_target_cmd)
		&& ((vneg & TPS65132_REG_VOL_MASK) == vneg_target_cmd))
		ret = 1;
	else
		ret = 0;
exit:
	return ret;
}

static int tps65132_reg_init(unsigned char vpos_cmd, unsigned char vneg_cmd)
{
	unsigned char vpos = 0;
	unsigned char vneg = 0;
	unsigned char app_dis = 0;
	unsigned char ctl = 0;
	int ret;

	ret = tps65132_i2c_read_u8(TPS65132_REG_VPOS, &vpos);
	if (ret != 0) {
		LCD_KIT_ERR("[%s]:read vpos voltage failed\n", __func__);
		goto exit;
	}

	ret = tps65132_i2c_read_u8(TPS65132_REG_VNEG, &vneg);
	if (ret != 0) {
		LCD_KIT_ERR("[%s]:read vneg voltage failed\n", __func__);
		goto exit;
	}

	ret = tps65132_i2c_read_u8(TPS65132_REG_APP_DIS, &app_dis);
	if (ret != 0) {
		LCD_KIT_ERR("[%s]:read app_dis failed\n", __func__);
		goto exit;
	}

	ret = tps65132_i2c_read_u8(TPS65132_REG_CTL,  &ctl);
	if (ret != 0) {
		LCD_KIT_ERR("[%s]:read ctl failed\n", __func__);
		goto exit;
	}

	vpos = (vpos & (~TPS65132_REG_VOL_MASK)) | vpos_cmd;
	vneg = (vneg & (~TPS65132_REG_VOL_MASK)) | vneg_cmd;

	app_dis = app_dis | TPS65312_APPS_BIT | TPS65132_DISP_BIT |
		TPS65132_DISN_BIT;
	ctl = ctl | TPS65132_WED_BIT;

	ret = tps65132_i2c_write_u8(TPS65132_REG_VPOS, vpos);
	if (ret != 0) {
		LCD_KIT_ERR("[%s]:write vpos failed\n", __func__);
		goto exit;
	}

	ret = tps65132_i2c_write_u8(TPS65132_REG_VNEG, vneg);
	if (ret != 0) {
		LCD_KIT_ERR("[%s]:write vneg failed\n", __func__);
		goto exit;
	}

	ret = tps65132_i2c_write_u8(TPS65132_REG_APP_DIS, app_dis);
	if (ret != 0) {
		LCD_KIT_ERR("%s write app_dis failed\n", __func__);
		goto exit;
	}

	ret = tps65132_i2c_write_u8(TPS65132_REG_CTL, ctl);
	if (ret != 0) {
		LCD_KIT_ERR("%s write ctl failed\n", __func__);
		goto exit;
	}

	mdelay(60);

exit:
	return ret;
}

static int tps65132_device_verify(void)
{
	int ret;
	unsigned char app_dis = 0;

	ret = tps65132_i2c_read_u8(TPS65132_REG_APP_DIS, &app_dis);
	if (ret < 0) {
		LCD_KIT_ERR("no tps65132 device, read app_dis failed\n");
		return LCD_KIT_FAIL;
	}
	LCD_KIT_INFO("tps65132 verify ok, app_dis = 0x%x\n", app_dis);

	return LCD_KIT_OK;
}

static void tps65132_get_target_voltage(int *vpos_target, int *vneg_target)

{
	*vpos_target = TPS65132_VOL_55_V;
	*vneg_target = TPS65132_VOL_55_V;
}

void tps65132_set_voltage(int vpos, int vneg)
{
	int ret;

	if ((vpos < TPS65132_VOL_40_V) || (vpos >= TPS65132_VOL_MAX_V)) {
		LCD_KIT_ERR("set vpos error, vpos = %d is out of range", vpos);
		return;
	}

	if ((vneg < TPS65132_VOL_40_V) || (vneg >= TPS65132_VOL_MAX_V)) {
		LCD_KIT_ERR("set vneg error, vneg = %d is out of range", vneg);
		return;
	}

	ret = tps65132_reg_inited(vpos, vneg);
	if (ret > 0) {
		LCD_KIT_ERR("tps65132 inited needn't reset value\n");
	} else if (ret < 0) {
		LCD_KIT_ERR("tps65132 I2C read not success\n");
	} else {
		ret = tps65132_reg_init(vpos, vneg);
		if (ret)
			LCD_KIT_ERR("tps65132_reg_init not success\n");
		LCD_KIT_ERR("tps65132 inited succeed\n");
	}
}

static int tps65132_set_bias(int vpos, int vneg)
{
	int i;
	int vol_size = ARRAY_SIZE(voltage_table);

	for (i = 0; i < vol_size; i++) {
		if (voltage_table[i].voltage == vpos) {
			LCD_KIT_INFO("tps65132 vsp voltage:0x%x\n",
				voltage_table[i].value);
			vpos = voltage_table[i].value;
			break;
		}
	}
	if (i >= vol_size) {
		LCD_KIT_ERR("not found vsp voltage, use default voltage\n");
		vpos = TPS65132_VOL_55_V;
	}
	for (i = 0; i < vol_size; i++) {
		if (voltage_table[i].voltage == vneg) {
			LCD_KIT_INFO("tps65132 vsn voltage:0x%x\n",
				voltage_table[i].value);
			vneg = voltage_table[i].value;
			break;
		}
	}

	if (i >= vol_size) {
		LCD_KIT_ERR("not found vsn voltage, use default voltage\n");
		vneg = TPS65132_VOL_55_V;
	}
	LCD_KIT_INFO("vpos = 0x%x, vneg = 0x%x\n", vpos, vneg);
	tps65132_set_voltage(vpos, vneg);
	return LCD_KIT_OK;
}

void tps65132_set_bias_status(void)
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

	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_TPS65132);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find tps65132 node, change dts failed\n");
		return;
	}
	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update tps65132 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("tps65132_set_bias_status OK!\n");
}

static struct lcd_kit_bias_ops bias_ops = {
	.set_bias_voltage = tps65132_set_bias,
};

int tps65132_bias_recognize(void)
{
	int ret;

	ret = tps65132_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("tps65132 is not right bias\n");
		return ret;
	} else {
		lcd_kit_bias_register(&bias_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("tps65132 is right bias\n");
		return ret;
	}
}

int tps65132_init(void)
{
	int ret;
	unsigned int support = 0;

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_TPS65132, TPS65132_SUPPORT,
		&support, 0);
	if ((ret < 0) || (!support)) {
		LCD_KIT_ERR("not support tps65132!\n");
		return LCD_KIT_OK;
	}

	/* register bias ops */
	lcd_kit_bias_recognize_register(tps65132_bias_recognize);
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_TPS65132,
		TPS65132_I2C_BUS_ID, &i2c_bus_id, 0);
	if (ret < 0) {
		i2c_bus_id = 0;
		return LCD_KIT_OK;
	}
	LCD_KIT_INFO("[%s]:tps65132 is support!\n", __func__);
	return LCD_KIT_OK;
}

