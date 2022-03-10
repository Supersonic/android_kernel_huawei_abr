/*
 * bias_ic_common.c
 *
 * bias_ic_common driver for lcd bias ic
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
#include "bias_ic_common.h"
#include "bias_bl_common.h"
#include "bias_bl_utils.h"
#include "lcd_kit_bias.h"

#define DTS_BIAS_IC_NUM "lcd_bias_ic_num"
#define DTS_BIAS_IC_LIST "lcd_bias_ic_list"
#define DTS_BIAS_IC_NAME "lcd_bias_ic_name"

#define DTS_BIAS_IC_I2C_ID "bias_ic_i2c_bus_id"
#define DTS_BIAS_IC_I2C_ADDR "reg"
#define DTS_BIAS_IC_TYPE "bias_ic_type"
#define DTS_BIAS_IC_CHECK_REG "bias_ic_check_reg"
#define DTS_BIAS_IC_CHECK_VAL "bias_ic_check_val"
#define DTS_BIAS_IC_CHECK_MASK "bias_ic_check_mask"
#define DTS_BIAS_IC_POS_REG "bias_ic_pos_reg"
#define DTS_BIAS_IC_POS_MASK "bias_ic_pos_mask"
#define DTS_BIAS_IC_NEG_REG "bias_ic_neg_reg"
#define DTS_BIAS_IC_NEG_MASK "bias_ic_neg_mask"
#define DTS_BIAS_IC_STATE_REG "bias_ic_state_reg"
#define DTS_BIAS_IC_STATE_VAL "bias_ic_state_val"
#define DTS_BIAS_IC_STATE_MASK "bias_ic_state_mask"
#define DTS_BIAS_IC_WRITE_REG "bias_ic_write_reg"
#define DTS_BIAS_IC_WRITE_VAL "bias_ic_write_val"
#define DTS_BIAS_IC_WRITE_MASK "bias_ic_write_mask"
#define DTS_BIAS_IC_WRITE_DELAY "bias_ic_write_delay"

#define DEFAULT_VOL_VAL 0x0F
/* bias ic: tps65132 */
static struct bias_ic_voltage tps65132_vol_table[] = {
	{ LCD_BAIC_VOL_40, TPS65132_VOL_40 },
	{ LCD_BAIC_VOL_41, TPS65132_VOL_41 },
	{ LCD_BAIC_VOL_42, TPS65132_VOL_42 },
	{ LCD_BAIC_VOL_43, TPS65132_VOL_43 },
	{ LCD_BAIC_VOL_44, TPS65132_VOL_44 },
	{ LCD_BAIC_VOL_45, TPS65132_VOL_45 },
	{ LCD_BAIC_VOL_46, TPS65132_VOL_46 },
	{ LCD_BAIC_VOL_47, TPS65132_VOL_47 },
	{ LCD_BAIC_VOL_48, TPS65132_VOL_48 },
	{ LCD_BAIC_VOL_49, TPS65132_VOL_49 },
	{ LCD_BAIC_VOL_50, TPS65132_VOL_50 },
	{ LCD_BAIC_VOL_51, TPS65132_VOL_51 },
	{ LCD_BAIC_VOL_52, TPS65132_VOL_52 },
	{ LCD_BAIC_VOL_53, TPS65132_VOL_53 },
	{ LCD_BAIC_VOL_54, TPS65132_VOL_54 },
	{ LCD_BAIC_VOL_55, TPS65132_VOL_55 },
	{ LCD_BAIC_VOL_56, TPS65132_VOL_56 },
	{ LCD_BAIC_VOL_57, TPS65132_VOL_57 },
	{ LCD_BAIC_VOL_58, TPS65132_VOL_58 },
	{ LCD_BAIC_VOL_59, TPS65132_VOL_59 },
	{ LCD_BAIC_VOL_60, TPS65132_VOL_60 }
};

/* bias ic: rt4801h */
static struct bias_ic_voltage rt4801h_vol_table[] = {
	{ LCD_BAIC_VOL_40, RT4801H_VOL_40 },
	{ LCD_BAIC_VOL_41, RT4801H_VOL_41 },
	{ LCD_BAIC_VOL_42, RT4801H_VOL_42 },
	{ LCD_BAIC_VOL_43, RT4801H_VOL_43 },
	{ LCD_BAIC_VOL_44, RT4801H_VOL_44 },
	{ LCD_BAIC_VOL_45, RT4801H_VOL_45 },
	{ LCD_BAIC_VOL_46, RT4801H_VOL_46 },
	{ LCD_BAIC_VOL_47, RT4801H_VOL_47 },
	{ LCD_BAIC_VOL_48, RT4801H_VOL_48 },
	{ LCD_BAIC_VOL_49, RT4801H_VOL_49 },
	{ LCD_BAIC_VOL_50, RT4801H_VOL_50 },
	{ LCD_BAIC_VOL_51, RT4801H_VOL_51 },
	{ LCD_BAIC_VOL_52, RT4801H_VOL_52 },
	{ LCD_BAIC_VOL_53, RT4801H_VOL_53 },
	{ LCD_BAIC_VOL_54, RT4801H_VOL_54 },
	{ LCD_BAIC_VOL_55, RT4801H_VOL_55 },
	{ LCD_BAIC_VOL_56, RT4801H_VOL_56 },
	{ LCD_BAIC_VOL_57, RT4801H_VOL_57 },
	{ LCD_BAIC_VOL_58, RT4801H_VOL_58 },
	{ LCD_BAIC_VOL_59, RT4801H_VOL_59 },
	{ LCD_BAIC_VOL_60, RT4801H_VOL_60 }
};

/* bias ic: lv52134a */
static struct bias_ic_voltage lv52134a_vol_table[] = {
	{ LCD_BAIC_VOL_41, LV52134A_VOL_41 },
	{ LCD_BAIC_VOL_42, LV52134A_VOL_42 },
	{ LCD_BAIC_VOL_43, LV52134A_VOL_43 },
	{ LCD_BAIC_VOL_44, LV52134A_VOL_44 },
	{ LCD_BAIC_VOL_45, LV52134A_VOL_45 },
	{ LCD_BAIC_VOL_46, LV52134A_VOL_46 },
	{ LCD_BAIC_VOL_47, LV52134A_VOL_47 },
	{ LCD_BAIC_VOL_48, LV52134A_VOL_48 },
	{ LCD_BAIC_VOL_49, LV52134A_VOL_49 },
	{ LCD_BAIC_VOL_50, LV52134A_VOL_50 },
	{ LCD_BAIC_VOL_51, LV52134A_VOL_51 },
	{ LCD_BAIC_VOL_52, LV52134A_VOL_52 },
	{ LCD_BAIC_VOL_53, LV52134A_VOL_53 },
	{ LCD_BAIC_VOL_54, LV52134A_VOL_54 },
	{ LCD_BAIC_VOL_55, LV52134A_VOL_55 },
	{ LCD_BAIC_VOL_56, LV52134A_VOL_56 },
	{ LCD_BAIC_VOL_57, LV52134A_VOL_57 }
};

static struct bias_ic_config bias_config[] = {
	{ "huawei,hw_tps65132",
	sizeof(tps65132_vol_table) / sizeof(tps65132_vol_table[0]),
	&tps65132_vol_table[0] },
	{ "huawei,hw_rt4801h",
	sizeof(rt4801h_vol_table) / sizeof(rt4801h_vol_table[0]),
	&rt4801h_vol_table[0] },
	{ "huawei,hw_lv52134a",
	sizeof(lv52134a_vol_table) / sizeof(lv52134a_vol_table[0]),
	&lv52134a_vol_table[0] }
};

static struct lcd_kit_bias_ic_info g_current_info;
static int bias_ic_num;

static int bias_ic_select(void)
{
	int ret;
	unsigned char val;
	struct bias_bl_common_ops *bias_ops = NULL;

	bias_ops = bias_bl_get_ops();
	if (bias_ops == NULL) {
		FASTBOOT_ERR("can not get bias_ops!!\n");
		return LCD_RET_FAIL;
	}

	if (!bias_ops->i2c_write_u8 || !bias_ops->i2c_read_u8) {
		FASTBOOT_ERR("can not get bias_ops function!\n");
		return LCD_RET_FAIL;
	}

	if (g_current_info.ic_type & BIAS_IC_READ_INHIBITION) {
		ret = bias_ops->i2c_write_u8(g_current_info.i2c_num,
			g_current_info.i2c_addr,
			g_current_info.check_reg,
			g_current_info.check_val);
		if (ret < 0) {
			FASTBOOT_INFO("bias ic select write fail\n");
			return LCD_RET_FAIL;
		}
	} else {
		if (g_current_info.check_mask != 0x00) {
			ret = bias_ops->i2c_read_u8(g_current_info.i2c_num,
				g_current_info.i2c_addr,
				g_current_info.check_reg, &val);
			if (ret < 0) {
				FASTBOOT_INFO("bias ic select read fail\n");
				return LCD_RET_FAIL;
			}
			if ((val & g_current_info.check_mask) !=
				g_current_info.check_val) {
				FASTBOOT_INFO("val 0x%x not expect 0x%x!\n",
					val, g_current_info.check_val);
				return LCD_RET_FAIL;
			}
		} else {
			ret = bias_ops->i2c_read_u8(g_current_info.i2c_num,
				g_current_info.i2c_addr,
				g_current_info.check_reg, &val);
			if (ret < 0) {
				FASTBOOT_INFO("failed!\n");
				return LCD_RET_FAIL;
			}
		}
	}

	return LCD_RET_OK;
}

static int bias_ic_check(const char *ic_name)
{
	int ret;
	int cfg_data;
	unsigned int cfg_len;
	struct bias_bl_common_ops *bias_ops = NULL;

	if (ic_name == NULL)
		return LCD_RET_FAIL;

	bias_ops = bias_bl_get_ops();
	if (bias_ops == NULL) {
		FASTBOOT_ERR("can not get bias_ops!!\n");
		return LCD_RET_FAIL;
	}
	if (!bias_ops->dts_get_data_by_property) {
		FASTBOOT_ERR("can not get bias_ops function!\n");
		return LCD_RET_FAIL;
	}
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_TYPE,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.ic_type = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_CHECK_REG,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.check_reg = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_CHECK_VAL,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.check_val = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_CHECK_MASK,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.check_mask = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_I2C_ID,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.i2c_num = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_I2C_ADDR,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.i2c_addr = (unsigned char)cfg_data;

	ret = bias_ic_select();

	return ret;
}

static int bias_parse_config(const char *ic_name, int str_len)
{
	int ret;
	int cfg_data;
	unsigned int cfg_len;
	struct bias_bl_common_ops *bias_ops = NULL;

	if (ic_name == NULL)
		return LCD_RET_FAIL;

	bias_ops = bias_bl_get_ops();
	if (bias_ops == NULL) {
		FASTBOOT_ERR("can not get bias_ops!!\n");
		return LCD_RET_FAIL;
	}

	if (!bias_ops->dts_get_data_by_property) {
		FASTBOOT_ERR("can not get bias_ops function!\n");
		return LCD_RET_FAIL;
	}

	if (str_len >= LCD_BIAS_IC_NAME_LEN)
		str_len = LCD_BIAS_IC_NAME_LEN - 1;

	memcpy(&g_current_info.name[0], ic_name, str_len);
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_POS_REG,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.vpos_reg = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_POS_MASK,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.vpos_mask = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_NEG_REG,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.vneg_reg = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_NEG_MASK,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.vneg_mask = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_STATE_REG,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.state_reg = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_STATE_VAL,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.state_val = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_STATE_MASK,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.state_mask = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_WRITE_REG,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.write_reg = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_WRITE_VAL,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.write_val = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_WRITE_MASK,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;
	g_current_info.write_mask = (unsigned char)cfg_data;
	ret = bias_ops->dts_get_data_by_property(ic_name,
		DTS_BIAS_IC_WRITE_DELAY,
		&cfg_data, &cfg_len);
	if (ret < 0)
		return LCD_RET_FAIL;

	g_current_info.delay = (unsigned char)cfg_data;
	FASTBOOT_INFO("vpos_reg is 0x%x!\n", g_current_info.vpos_reg);
	FASTBOOT_INFO("vpos_mask is 0x%x!\n", g_current_info.vpos_mask);
	FASTBOOT_INFO("vneg_reg is 0x%x!\n", g_current_info.vneg_reg);
	FASTBOOT_INFO("vneg_mask is 0x%x!\n", g_current_info.vneg_mask);
	FASTBOOT_INFO("state_reg is 0x%x!\n", g_current_info.state_reg);
	FASTBOOT_INFO("state_val is 0x%x!\n", g_current_info.state_val);
	FASTBOOT_INFO("state_mask is 0x%x!\n", g_current_info.state_mask);
	FASTBOOT_INFO("write_reg is 0x%x!\n", g_current_info.write_reg);
	FASTBOOT_INFO("write_val is 0x%x!\n", g_current_info.write_val);
	FASTBOOT_INFO("write_mask is 0x%x!\n", g_current_info.write_mask);
	FASTBOOT_INFO("delay is %d!\n", g_current_info.delay);

	return LCD_RET_OK;
}

static int bias_ic_verify(void)
{
	int i;
	int ret;
	char name[LCD_BIAS_IC_NAME_LEN] = {0};
	struct bias_bl_common_ops *bias_ops = NULL;

	bias_ops = bias_bl_get_ops();
	if (bias_ops == NULL) {
		FASTBOOT_ERR("can not get bias_ops!!\n");
		return LCD_RET_FAIL;
	}

	if (!bias_ops->dts_get_ic_name_index) {
		FASTBOOT_ERR("can not get bias_ops function!\n");
		return LCD_RET_FAIL;
	}

	if (bias_ic_num == 0)
		return LCD_RET_FAIL;

	for (i = 0; i < bias_ic_num; i++) {
		ret = bias_ops->dts_get_ic_name_index(DTS_BIAS_IC_LIST,
			i, name, LCD_BIAS_IC_NAME_LEN);
		if (ret < 0) {
			FASTBOOT_INFO("get lcd bias ic name list fail\n");
			return LCD_RET_FAIL;
		}
		FASTBOOT_INFO("get lcd bias ic name is %s\n", name);
		ret = bias_ic_check(name);
		if (ret == LCD_RET_OK) {
			ret = bias_parse_config(name, LCD_BIAS_IC_NAME_LEN);
			if (ret < 0) {
				FASTBOOT_INFO("parse dts fail, use default\n");
				ret = LCD_RET_OK;
			}
			break;
		}
		memset(name, 0, LCD_BIAS_IC_NAME_LEN);
	}
	if (i == bias_ic_num) {
		FASTBOOT_ERR("no bias ic is right\n");
		ret = LCD_RET_FAIL;
	}
	return ret;
}

static void bias_ic_get_value(int pos_votage, int neg_votage)
{
	unsigned int i;
	unsigned int j;

	for (i = 0; i < sizeof(bias_config) / sizeof(struct bias_ic_config);
		i++) {
		if (strcmp(g_current_info.name, bias_config[i].name))
			continue;
		for (j = 0; j < bias_config[i].len; j++) {
			if (bias_config[i].vol_table[j].voltage ==
				(unsigned int)pos_votage) {
				g_current_info.vpos_val =
					bias_config[i].vol_table[j].value;
				break;
			}
		}
		if (j == bias_config[i].len) {
			FASTBOOT_ERR("not found vsp, use default\n");
			g_current_info.vpos_val = DEFAULT_VOL_VAL;
		}
		FASTBOOT_INFO("vsp voltage is 0x%x  j is %d\n",
			g_current_info.vpos_val, j);
		for (j = 0; j < bias_config[i].len; j++) {
			if (bias_config[i].vol_table[j].voltage ==
				(unsigned int)neg_votage) {
				g_current_info.vneg_val =
					bias_config[i].vol_table[j].value;
				break;
			}
		}
		FASTBOOT_INFO("neg voltage is 0x%x  j is %d\n",
			g_current_info.vneg_val, j);
		if (j == bias_config[i].len) {
			FASTBOOT_INFO("not found vneg, use default\n");
			g_current_info.vneg_val = DEFAULT_VOL_VAL;
		}
		break;
	}

	if (i == sizeof(bias_config) / sizeof(struct bias_ic_config)) {
		FASTBOOT_ERR("not found right voltage, use default\n");
		g_current_info.vpos_val = DEFAULT_VOL_VAL;
		g_current_info.vneg_val = DEFAULT_VOL_VAL;
	}
}

static int bias_voltage_inited(unsigned char vpos_target_cmd,
	unsigned char vneg_target_cmd)
{
	unsigned char v_pos;
	unsigned char v_neg;
	int ret;
	unsigned char state_val = 0;
	struct bias_bl_common_ops *bias_ops = NULL;

	bias_ops = bias_bl_get_ops();
	if (bias_ops == NULL) {
		FASTBOOT_ERR("can not get bias_ops!!\n");
		ret = LCD_RET_FAIL;
		goto exit;
	}

	if (!bias_ops->i2c_read_u8) {
		FASTBOOT_ERR("can not get bias_ops function!\n");
		ret = LCD_RET_FAIL;
		goto exit;
	}

	ret = bias_ops->i2c_read_u8(g_current_info.i2c_num,
		g_current_info.i2c_addr, g_current_info.vpos_reg, &v_pos);
	if (ret < 0) {
		FASTBOOT_INFO("read vpos voltage failed\n");
		ret = LCD_RET_FAIL;
		goto exit;
	}

	ret = bias_ops->i2c_read_u8(g_current_info.i2c_num,
		g_current_info.i2c_addr, g_current_info.vneg_reg, &v_neg);
	if (ret < 0) {
		FASTBOOT_INFO("read vneg voltage fail\n");
		ret = LCD_RET_FAIL;
		goto exit;
	}

	if (g_current_info.state_mask != 0x00) {
		ret = bias_ops->i2c_read_u8(g_current_info.i2c_num,
			g_current_info.i2c_addr, g_current_info.state_reg,
			&state_val);
		if (ret < 0) {
			FASTBOOT_INFO("read vpos voltage fail\n");
			ret = LCD_RET_FAIL;
			goto exit;
		}
		if (((v_pos & g_current_info.vpos_mask) == vpos_target_cmd) &&
			((v_neg & g_current_info.vneg_mask) == vneg_target_cmd) &&
			((state_val & g_current_info.state_mask) ==
			g_current_info.state_val))
			ret = LCD_RET_OK;
		else
			ret = LCD_RET_FAIL;
	} else {
		if (((v_pos & g_current_info.vpos_mask) == vpos_target_cmd) &&
			((v_neg & g_current_info.vneg_mask) ==
			vneg_target_cmd))
			ret = LCD_RET_OK;
		else
			ret = LCD_RET_FAIL;
	}
exit:
	return ret;
}

static int bias_voltage_init(void)
{
	int ret;
	struct bias_bl_common_ops *bias_ops = NULL;

	bias_ops = bias_bl_get_ops();
	if (bias_ops == NULL) {
		FASTBOOT_ERR("can not get bias_ops!!\n");
		return LCD_RET_FAIL;
	}

	if (!bias_ops->i2c_write_u8 || !bias_ops->i2c_update_u8) {
		FASTBOOT_ERR("can not get bias_ops function!\n");
		return LCD_RET_FAIL;
	}

	ret = bias_ops->i2c_write_u8(g_current_info.i2c_num,
		g_current_info.i2c_addr, g_current_info.vpos_reg,
		g_current_info.vpos_val);
	if (ret < 0) {
		FASTBOOT_INFO("write vpos fail\n");
		return LCD_RET_FAIL;
	}

	ret = bias_ops->i2c_write_u8(g_current_info.i2c_num,
		g_current_info.i2c_addr, g_current_info.vneg_reg,
		g_current_info.vneg_val);
	if (ret < 0) {
		FASTBOOT_INFO("write vneg fail\n");
		return LCD_RET_FAIL;
	}

	if (g_current_info.state_mask != 0x00) {
		ret = bias_ops->i2c_update_u8(g_current_info.i2c_num,
			g_current_info.i2c_addr, g_current_info.state_reg,
			g_current_info.state_mask, g_current_info.state_val);
		if (ret < 0) {
			FASTBOOT_INFO("update addr = 0x%x fail\n",
				g_current_info.state_reg);
			return LCD_RET_FAIL;
		}
	}

	if (g_current_info.ic_type & BIAS_IC_HAVE_E2PROM) {
		if (g_current_info.write_mask != 0x00) {
			ret = bias_ops->i2c_update_u8(g_current_info.i2c_num,
				g_current_info.i2c_addr,
				g_current_info.write_reg,
				g_current_info.write_mask,
				g_current_info.write_val);
			if (ret < 0) {
				FASTBOOT_INFO("update addr=0x%x fail\n",
					g_current_info.write_reg);
				return LCD_RET_FAIL;
			}
		}
	}

	if (g_current_info.delay)
		mdelay(g_current_info.delay);

	return LCD_RET_OK;
}

static int bias_ic_set_voltage(int vpos, int vneg)
{
	int ret;

	bias_ic_get_value(vpos, vneg);
	if (g_current_info.ic_type & BIAS_IC_HAVE_E2PROM) {
		ret = bias_voltage_inited(g_current_info.vpos_val,
			g_current_info.vneg_val);
		if (ret == LCD_RET_OK) {
			FASTBOOT_INFO("bias ic is inited!\n");
			return ret;
		}
	}
	ret = bias_voltage_init();
	if (ret < 0)
		FASTBOOT_ERR("bias ic voltage init fail\n");

	return ret;
}

static struct lcd_kit_bias_ops bias_ic_ops = {
	.set_bias_voltage = bias_ic_set_voltage,
};

static int bias_ic_recognize(void)
{
	int ret;
	struct bias_bl_common_ops *bias_ops = NULL;

	bias_ops = bias_bl_get_ops();
	if (bias_ops == NULL) {
		FASTBOOT_ERR("can not get bias_ops!!\n");
		return LCD_RET_FAIL;
	}

	if (!bias_ops->dts_set_ic_name ||
		!bias_ops->dts_set_status_by_compatible) {
		FASTBOOT_ERR("can not get bias_ops function!\n");
		return LCD_RET_FAIL;
	}

	ret = bias_ic_verify();
	if (ret < 0) {
		FASTBOOT_ERR("not verify right bias ic\n");
	} else {
		lcd_kit_bias_register(&bias_ic_ops);
		bias_ops->dts_set_status_by_compatible(g_current_info.name,
			"ok");
		bias_ops->dts_set_ic_name(DTS_BIAS_IC_NAME,
			g_current_info.name);
		FASTBOOT_ERR("get right bias ic %s\n",
			g_current_info.name);
	}
	return ret;
}

int bias_ic_init(void)
{
	struct bias_bl_common_ops *bias_ops = NULL;
	char name[LCD_BIAS_IC_NAME_LEN] = {0};
	int i;
	int ret;

	bias_ops = bias_bl_get_ops();
	if (bias_ops == NULL) {
		FASTBOOT_ERR("can not get bias_ops!!\n");
		return LCD_RET_FAIL;
	}

	if (!bias_ops->dts_get_ic_num ||
		!bias_ops->dts_set_ic_name ||
		!bias_ops->dts_get_ic_name_index ||
		!bias_ops->dts_set_status_by_compatible) {
		FASTBOOT_ERR("can not get bias_ops function!\n");
		return LCD_RET_FAIL;
	}

	bias_ops->dts_set_ic_name(DTS_BIAS_IC_NAME,
		"default");

	bias_ic_num = bias_ops->dts_get_ic_num(DTS_BIAS_IC_NUM);
	if (bias_ic_num == 0) {
		FASTBOOT_INFO("not support lcd_kit_bias_ic!\n");
		return LCD_RET_FAIL;
	}

	for (i = 0; i < bias_ic_num; i++) {
		ret = bias_ops->dts_get_ic_name_index(DTS_BIAS_IC_LIST,
			i, name, LCD_BIAS_IC_NAME_LEN);
		if (ret < 0) {
			FASTBOOT_INFO("get lcd bias ic name list fail\n");
		} else {
			ret = bias_ops->dts_set_status_by_compatible(name,
				"disabled");
			if (ret < 0)
				FASTBOOT_INFO("set %s disabled faile\n",
					name);
		}
		memset(name, 0, LCD_BIAS_IC_NAME_LEN);
	}

	lcd_kit_bias_recognize_register(bias_ic_recognize);
	FASTBOOT_INFO("bias ic init ok !\n");

	return LCD_RET_OK;
}
