/*
 * lcd_kit_power.c
 *
 * lcdkit power function for lcd driver
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

#include "lcd_kit_power.h"
#include "lcd_kit_disp.h"
#include "lcm_drv.h"

uint32_t g_lcd_kit_gpio;
#define LDO_BEGIN 1
#define LDO_MAX 5

static struct str_ldo_desc g_ldo_enable[] = {
	/* ldo num, volt_value, enable_addr, enable_data, enable_mask, enable_shift */
#ifdef MTK_MT6360_LDO_SUPPORT
	/* ldo1 enable */
	{ 1, 3000000, 0x17, 0x1, 0x1, 0x6 },
	{ 1, 3100000, 0x17, 0x1, 0x1, 0x6 },
#endif
#ifdef MTK_MT6358_LDO_SUPPORT
	/* ldo1 enable */
	{ 28, 3000000, 0x17, 0x1, 0x1, 0x6 },
#endif
#if defined(CONFIG_MACH_MT6885) && defined(MTK_MT6360_PMIC_SUPPORT)
	/* ldo6 enable */
	{ 6, 1800000, 0x37, 0x1, 0x1, 0x6 },
#endif
};

static struct str_ldo_desc g_ldo_volt[] = {
	/* ldo num, volt_value, volt_addr, volt_data, volt_mask, volt_shift */
#ifdef MTK_MT6358_LDO_SUPPORT
	/* ldo1 3v */
	{ 28, 3000000, 0x1B, 0xb, 0xf, 0x4 },
#endif
#ifdef MTK_MT6360_LDO_SUPPORT
	/* ldo1 3v */
	{ 1, 3000000, 0x1B, 0xb, 0xf, 0x4 },
	/* ldo1 3v1 */
	{ 1, 3100000, 0x1B, 0xc, 0xf, 0x4 },
#endif
#if defined(CONFIG_MACH_MT6885) && defined(MTK_MT6360_PMIC_SUPPORT)
	/* ldo6 1v8 */
	{ 6, 1800000, 0x3B, 0xd0, 0xff, 0x0 },
#endif
};
/*
 * power type
 */
static struct regulate_bias_desc vsp_param = { 5500000, 5500000, 0, 0 };
static struct regulate_bias_desc vsn_param = { 5500000, 5500000, 0, 0 };

static struct gpio_desc gpio_req_cmds[] = {
	{
		DTYPE_GPIO_REQUEST, WAIT_TYPE_US, 0,
		GPIO_NAME, &g_lcd_kit_gpio, 0
	},
};

static struct gpio_desc gpio_free_cmds[] = {
	{
		DTYPE_GPIO_FREE, WAIT_TYPE_US, 0,
		GPIO_NAME, &g_lcd_kit_gpio, 0
	},
};

static struct gpio_desc gpio_high_cmds[] = {
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_US, 0,
		GPIO_NAME, &g_lcd_kit_gpio, 1
	},
};

static struct gpio_desc gpio_low_cmds[] = {
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_US, 0,
		GPIO_NAME, &g_lcd_kit_gpio, 0
	},
};

struct gpio_power_arra gpio_power[] = {
	{ GPIO_REQ, ARRAY_SIZE(gpio_req_cmds), gpio_req_cmds },
	{ GPIO_HIGH, ARRAY_SIZE(gpio_high_cmds), gpio_high_cmds },
	{ GPIO_LOW, ARRAY_SIZE(gpio_low_cmds), gpio_low_cmds },
	{ GPIO_RELEASE, ARRAY_SIZE(gpio_free_cmds), gpio_free_cmds },
};

static struct lcd_kit_mtk_regulate_ops *g_regulate_ops;

struct lcd_kit_mtk_regulate_ops *lcd_kit_mtk_get_regulate_ops(void)
{
	return g_regulate_ops;
}

int gpio_cmds_tx(struct gpio_desc *cmds, int cnt)
{
	int ret = 0;
	struct gpio_desc *cm = NULL;
	int i;

	if (!cmds) {
		LCD_KIT_ERR("cmds is NULL!\n");
		return LCD_KIT_FAIL;
	}

	cm = cmds;

	for (i = 0; i < cnt; i++) {
		if ((cm == NULL) || (cm->label == NULL)) {
			LCD_KIT_ERR("cm or cm->label is null! index=%d, cnt=%d\n",
				i, cnt);
			ret = LCD_KIT_FAIL;
			goto error;
		} else if (cm->dtype == DTYPE_GPIO_OUTPUT) {
			mt_set_gpio_mode(*(cm->gpio), GPIO_MODE_00);
			mt_set_gpio_dir(*(cm->gpio), GPIO_DIR_OUT);
			mt_set_gpio_out(*(cm->gpio), cm->value);
		} else if (cm->dtype == DTYPE_GPIO_REQUEST) {
			;
		} else if (cm->dtype == DTYPE_GPIO_FREE) {
			;
		} else {
			ret = LCD_KIT_FAIL;
			goto error;
		}

		if (cm->wait) {
			if (cm->waittype == WAIT_TYPE_US)
				udelay(cm->wait);
			else if (cm->waittype == WAIT_TYPE_MS)
				mdelay(cm->wait);
			else
				/* 1000 means seconds */
				mdelay(cm->wait * 1000);
		}

		cm++;
	}

	return 0;

error:
	return ret;
}
void lcd_kit_gpio_tx(uint32_t type, uint32_t op)
{
	unsigned int i;
	struct gpio_power_arra *gpio_cm = NULL;

	switch (type) {
	case LCD_KIT_VCI:
		g_lcd_kit_gpio = power_hdl->lcd_vci.buf[POWER_NUMBER];
		break;
	case LCD_KIT_IOVCC:
		g_lcd_kit_gpio = power_hdl->lcd_iovcc.buf[POWER_NUMBER];
		break;
	case LCD_KIT_VSP:
		g_lcd_kit_gpio = power_hdl->lcd_vsp.buf[POWER_NUMBER];
		break;
	case LCD_KIT_VSN:
		g_lcd_kit_gpio = power_hdl->lcd_vsn.buf[POWER_NUMBER];
		break;
	case LCD_KIT_RST:
		g_lcd_kit_gpio = power_hdl->lcd_rst.buf[POWER_NUMBER];
		break;
	case LCD_KIT_BL:
		g_lcd_kit_gpio = power_hdl->lcd_backlight.buf[POWER_NUMBER];
		break;
	case LCD_KIT_TP_RST:
		g_lcd_kit_gpio = power_hdl->tp_rst.buf[POWER_NUMBER];
		break;
	case LCD_KIT_VDD:
		g_lcd_kit_gpio = power_hdl->lcd_vdd.buf[POWER_NUMBER];
		break;
	default:
		LCD_KIT_ERR("not support type:%d\n", type);
		break;
	}

	for (i = 0; i < ARRAY_SIZE(gpio_power); i++) {
		if (gpio_power[i].oper == op) {
			gpio_cm = &gpio_power[i];
			break;
		}
	}
	if (i >= ARRAY_SIZE(gpio_power)) {
		LCD_KIT_ERR("not found cm from gpio_power\n");
		return;
	}
	gpio_cmds_tx(gpio_cm->cm, gpio_cm->num);
}

static uint32_t lcd_kit_ldo_index(uint32_t power_num, uint32_t power_volt)
{
	uint32_t i;

	for (i = 0; i < ARRAY_SIZE(g_ldo_enable); i++) {
		if (power_num == g_ldo_enable[i].ldo_num &&
			power_volt == g_ldo_enable[i].ldo_value)
			return i;
	}

	return LCD_KIT_FAIL;
}

static int lcd_kit_get_pmu_info(uint32_t type, uint32_t *power_num, uint32_t *power_volt)
{
	switch (type) {
	case LCD_KIT_VCI:
		*power_num = power_hdl->lcd_vci.buf[POWER_NUMBER];
		*power_volt = power_hdl->lcd_vci.buf[POWER_VOLTAGE];
		break;
	case LCD_KIT_IOVCC:
		*power_num = power_hdl->lcd_iovcc.buf[POWER_NUMBER];
		*power_volt = power_hdl->lcd_iovcc.buf[POWER_VOLTAGE];
		break;
	case LCD_KIT_VDD:
		*power_num = power_hdl->lcd_vdd.buf[POWER_NUMBER];
		*power_volt = power_hdl->lcd_vdd.buf[POWER_VOLTAGE];
		break;
	default:
		LCD_KIT_ERR("not support type:%d\n", type);
		return LCD_KIT_FAIL;
	}
	LCD_KIT_INFO("type = %d, *power_num = %d, *power_volt = %d\n",
			type, *power_num, *power_volt);
	return LCD_KIT_OK;
}

int lcd_kit_pmu_ctrl(uint32_t type, uint32_t enable)
{
	uint32_t power_num = 0;
	uint32_t power_volt = 0;
	uint32_t array_index;

	if (lcd_kit_get_pmu_info(type, &power_num, &power_volt))
		return LCD_KIT_FAIL;

	array_index = lcd_kit_ldo_index(power_num, power_volt);
	if (array_index > LDO_MAX)
		return LCD_KIT_FAIL;
	g_ldo_enable[array_index].ldo_data = enable;

	if (power_num >= LDO_BEGIN && power_num <= LDO_MAX) {
#ifdef MTK_MT6360_LDO_SUPPORT
		if (mt6360_ldo_config_interface(g_ldo_volt[array_index].ldo_addr,
			g_ldo_volt[array_index].ldo_data,
			g_ldo_volt[array_index].ldo_mask,
			g_ldo_volt[array_index].ldo_shift)) {
			LCD_KIT_ERR("ldo set volt fail\n");
			return LCD_KIT_FAIL;
		}
#endif
		LCD_KIT_INFO("ldo%d set vol\n", power_num);
	} else {
#if defined(CONFIG_MACH_MT6885) && defined(MTK_MT6360_PMIC_SUPPORT)
		if (mt6360_pmic_config_interface(g_ldo_volt[array_index].ldo_addr,
			g_ldo_volt[array_index].ldo_data,
			g_ldo_volt[array_index].ldo_mask,
			g_ldo_volt[array_index].ldo_shift)) {
			LCD_KIT_ERR("ldo set volt fail\n");
			return LCD_KIT_FAIL;
		}
#endif
#ifdef MTK_MT6358_LDO_SUPPORT
		if (pmic_config_interface(PMIC_RG_LDO_VLDO28_EN_0_ADDR,
			0x01,
			PMIC_RG_LDO_VLDO28_EN_0_MASK,
			PMIC_RG_LDO_VLDO28_EN_0_SHIFT)) {
			LCD_KIT_ERR("ldo enable fail\n");
			return LCD_KIT_FAIL;
		}
#endif
		LCD_KIT_INFO("ldo%d set vol\n", power_num);
	}

	LCD_KIT_INFO("[vol]ldo_addr = 0x%x,ldo_data = 0x%x,ldo_mask = 0x%x, ldo_shift = 0x%x\n",
			g_ldo_volt[array_index].ldo_addr,
			g_ldo_volt[array_index].ldo_data,
			g_ldo_volt[array_index].ldo_mask,
			g_ldo_volt[array_index].ldo_shift);

	if (power_num >= LDO_BEGIN && power_num <= LDO_MAX) {
#ifdef MTK_MT6360_LDO_SUPPORT
		if (mt6360_ldo_config_interface(g_ldo_enable[array_index].ldo_addr,
			g_ldo_enable[array_index].ldo_data,
			g_ldo_enable[array_index].ldo_mask,
			g_ldo_enable[array_index].ldo_shift)) {
			LCD_KIT_ERR("ldo enable fail\n");
			return LCD_KIT_FAIL;
		}
#endif
		LCD_KIT_INFO("ldo%d config\n", power_num);
	} else {
#if defined(CONFIG_MACH_MT6885) && defined(MTK_MT6360_PMIC_SUPPORT)
		if (mt6360_pmic_config_interface(g_ldo_enable[array_index].ldo_addr,
			g_ldo_enable[array_index].ldo_data,
			g_ldo_enable[array_index].ldo_mask,
			g_ldo_enable[array_index].ldo_shift)) {
			LCD_KIT_ERR("ldo enable fail\n");
			return LCD_KIT_FAIL;
		}
#endif
#ifdef MTK_MT6358_LDO_SUPPORT
		if (pmic_config_interface(PMIC_RG_VLDO28_VOSEL_ADDR,
			0x03,
			PMIC_RG_VLDO28_VOSEL_MASK,
			PMIC_RG_VLDO28_VOSEL_SHIFT)) {
			LCD_KIT_ERR("ldo enable fail\n");
			return LCD_KIT_FAIL;
		}
#endif
		LCD_KIT_INFO("ldo%d config\n", power_num);
	}
	LCD_KIT_INFO("[enable]ldo_addr = 0x%x,ldo_data = 0x%x,ldo_mask = 0x%x, ldo_shift = 0x%x\n",
			g_ldo_enable[array_index].ldo_addr,
			g_ldo_enable[array_index].ldo_data,
			g_ldo_enable[array_index].ldo_mask,
			g_ldo_enable[array_index].ldo_shift);

	return LCD_KIT_OK;
}

static void lcd_kit_charger_vsp_ctrl(uint32_t enable)
{
	int ret;

	if (enable) {
		if (g_regulate_ops->reguate_vsp_enable) {
			ret = g_regulate_ops->reguate_vsp_enable(vsp_param);
			if (ret != LCD_KIT_OK)
				LCD_KIT_ERR("reguate_vsp_enable failed\n");
		} else {
			LCD_KIT_ERR("reguate_vsp_enable is NULL\n");
		}
	} else {
		if (g_regulate_ops->reguate_vsp_disable) {
			ret = g_regulate_ops->reguate_vsp_disable(vsp_param);
			if (ret != LCD_KIT_OK)
				LCD_KIT_ERR("reguate_vsp_disable failed\n");
		} else {
				LCD_KIT_ERR("reguate_vsp_disable is NULL\n");
		}
	}
}

static void lcd_kit_charger_vsn_ctrl(uint32_t enable)
{
	int ret;

	if (enable) {
		if (g_regulate_ops->reguate_vsn_enable) {
			ret = g_regulate_ops->reguate_vsn_enable(vsp_param);
			if (ret != LCD_KIT_OK)
				LCD_KIT_ERR("reguate_vsn_enable failed\n");
		} else {
			LCD_KIT_ERR("reguate_vsn_enable is NULL\n");
		}
	} else {
		if (g_regulate_ops->reguate_vsn_disable) {
			ret = g_regulate_ops->reguate_vsn_disable(vsp_param);
			if (ret != LCD_KIT_OK)
				LCD_KIT_ERR("reguate_vsn_disable failed\n");
		} else {
			LCD_KIT_ERR("reguate_vsn_disable is NULL\n");
		}
	}
}

int lcd_kit_charger_ctrl(uint32_t type, uint32_t enable)
{
	switch (type) {
	case LCD_KIT_VSP:
		if (!g_regulate_ops)
			LCD_KIT_ERR("g_regulate_ops is NULL\n");
		lcd_kit_charger_vsp_ctrl(enable);
		break;
	case LCD_KIT_VSN:
		if (!g_regulate_ops)
			LCD_KIT_ERR("g_regulate_ops is NULL\n");
		lcd_kit_charger_vsn_ctrl(enable);
		break;
	default:
		LCD_KIT_ERR("error type\n");
		break;
	}
	return LCD_KIT_OK;
}

static void lcd_kit_power_regulate_vsp_set(struct regulate_bias_desc *cmds)
{
	if (!cmds) {
		LCD_KIT_ERR("cmds is null point!\n");
		return;
	}
	cmds->min_uV = power_hdl->lcd_vsp.buf[POWER_VOLTAGE];
	cmds->max_uV = power_hdl->lcd_vsp.buf[POWER_VOLTAGE];
	cmds->wait = power_hdl->lcd_vsp.buf[POWER_NUMBER];
}

static void lcd_kit_power_regulate_vsn_set(struct regulate_bias_desc *cmds)
{
	if (!cmds) {
		LCD_KIT_ERR("cmds is null point!\n");
		return;
	}
	cmds->min_uV = power_hdl->lcd_vsn.buf[POWER_VOLTAGE];
	cmds->max_uV = power_hdl->lcd_vsn.buf[POWER_VOLTAGE];
	cmds->wait = power_hdl->lcd_vsn.buf[POWER_NUMBER];
}
#if  defined(MTK_RT5081_PMU_CHARGER_SUPPORT) || defined(MTK_MT6370_PMU)
extern struct lcd_kit_mtk_regulate_ops *lcd_kit_mtk_regulate_register(void);
#endif
int lcd_kit_power_init(void)
{
#if  defined(MTK_RT5081_PMU_CHARGER_SUPPORT) || defined(MTK_MT6370_PMU)
	g_regulate_ops = lcd_kit_mtk_regulate_register();
#endif
	/* vsp */
	if (power_hdl->lcd_vsp.buf) {
		if (power_hdl->lcd_vsp.buf[POWER_NUMBER] == REGULATOR_MODE) {
			LCD_KIT_INFO("LCD vsp type is regulate!\n");
			lcd_kit_power_regulate_vsp_set(&vsp_param);
		}
	}
	/* vsn */
	if (power_hdl->lcd_vsn.buf) {
		if (power_hdl->lcd_vsn.buf[POWER_NUMBER] == REGULATOR_MODE) {
			LCD_KIT_INFO("LCD vsn type is regulate!\n");
			lcd_kit_power_regulate_vsn_set(&vsn_param);
		}
	}
	return LCD_KIT_OK;
}

