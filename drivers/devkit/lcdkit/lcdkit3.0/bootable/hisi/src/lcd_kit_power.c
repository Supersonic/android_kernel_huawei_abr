/*
 * lcd_kit_power.c
 *
 * lcdkit display power for lcd driver
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

uint32_t g_lcd_kit_gpio;
uint32_t g_lcd_kit_vci;
uint32_t g_lcd_kit_vdd;
/*
 * power type
 */
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

static struct lcd_kit_vci_type lcd_kit_ldo_map[] = {
	{ LDO_17, DEVICE_LCDANALOG },
	{ LDO_9, DEVICE_SDIO },
#if defined(FASTBOOT_PMU_HI6421V700)
	{ LDO_25, DEVICE_LDO25 },
#elif defined(FASTBOOT_PMU_HI6421V800)
	{ LDO_24, DEVICE_LDO24 },
	{ LDO_29, DEVICE_LDO29 },
	{ LDO_12, DEVICE_LDO12 },
	{ LDO_32, DEVICE_LDO32 },
#elif defined(FASTBOOT_PMU_HI6555V300)
	{ LDO_12, DEVICE_LDO12 },
	{ LDO_32, DEVICE_LDO32 },
#elif defined(FASTBOOT_PMU_HI6555V500)
	{ LDO_43, DEVICE_LCDDIC },
	{ LDO_12, DEVICE_LDO12 },
#elif defined(FASTBOOT_PMU_HI6421V900)
	{ LDO_12, DEVICE_DDR_WARPAGE_DET },
	{ LDO_24, DEVICE_LCDANALOG },
	{ LDO_29, DEVICE_LCDDIG },
#endif
};

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
	case LCD_KIT_AOD:
		g_lcd_kit_gpio = power_hdl->lcd_aod.buf[POWER_NUMBER];
		break;
	case LCD_KIT_POWERIC:
		g_lcd_kit_gpio = power_hdl->lcd_poweric.buf[POWER_NUMBER];
		break;
	case LCD_KIT_MIPI_SWITCH:
		g_lcd_kit_gpio = power_hdl->lcd_mipi_switch.buf[POWER_NUMBER];
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
	if (gpio_cm->num > 0) {
		gpio_cmds_tx(gpio_cm->cm, gpio_cm->num);
		return;
	} else {
		LCD_KIT_INFO("gpio is not bigger than 0\n");
		return;
	}
}

int lcd_kit_get_gpio_value(uint32_t gpio_num, uint32_t *gpio_val)
{
	struct gpio_operators *gpio_ops = NULL;

	gpio_ops = get_operators(GPIO_MODULE_NAME_STR);
	if (!gpio_ops) {
		LCD_KIT_ERR("gpio_ops is null!\n");
		return LCD_KIT_FAIL;
	}
	gpio_ops->set_direction_input(gpio_num);
	*gpio_val = gpio_ops->get_value(gpio_num);
	LCD_KIT_INFO("gpio_value = %u\n", *gpio_val);
	return LCD_KIT_OK;
}

int lcd_kit_pmu_ctrl(uint32_t type, uint32_t enable)
{
	struct pmu_operators *pmu_ops = NULL;

	pmu_ops = get_operators(PMU_MODULE_NAME_STR);
	if (!pmu_ops) {
		LCD_KIT_ERR("can not get pmu_ops!\n");
		return LCD_KIT_FAIL;
	}
	switch (type) {
	case LCD_KIT_VCI:
		if (enable)
			pmu_ops->pmu_enable(g_lcd_kit_vci);
		else
			pmu_ops->pmu_disable(g_lcd_kit_vci);
		break;
	case LCD_KIT_IOVCC:
		if (enable)
			pmu_ops->pmu_enable(DEVICE_LCDIO);
		else
			pmu_ops->pmu_disable(DEVICE_LCDIO);
		break;
	case LCD_KIT_VDD:
		if (enable)
			pmu_ops->pmu_enable(g_lcd_kit_vdd);
		else
			pmu_ops->pmu_disable(g_lcd_kit_vdd);
		break;
	default:
		break;
	}
	return LCD_KIT_OK;
}

int lcd_kit_charger_ctrl(uint32_t type, uint32_t enable)
{
	struct charger_power_operators *charger_ops = NULL;

	charger_ops = get_operators(CHARGER_POWER_MODULE_NAME_STR);
	if (!charger_ops) {
		LCD_KIT_ERR("can not get charger_power_ops!\n");
		return LCD_KIT_FAIL;
	}
	switch (type) {
	case LCD_KIT_VSP:
		if (enable) {
			charger_ops->charger_power_enable(DEVICE_LCD_BIAS);
			charger_ops->charger_power_enable(DEVICE_LCD_VSP);
		} else {
			charger_ops->charger_power_disable(DEVICE_LCD_VSP);
			charger_ops->charger_power_disable(DEVICE_LCD_BIAS);
		}
		break;
	case LCD_KIT_VSN:
		if (enable)
			charger_ops->charger_power_enable(DEVICE_LCD_VSN);
		else
			charger_ops->charger_power_disable(DEVICE_LCD_VSN);
		break;
	case LCD_KIT_BL:
		if (enable)
			charger_ops->charger_power_enable(DEVICE_WLED);
		else
			charger_ops->charger_power_disable(DEVICE_WLED);
		break;
	default:
		break;
	}
	return LCD_KIT_OK;
}

static uint32_t lcd_ldo_sel(uint32_t ldo_num)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(lcd_kit_ldo_map); i++) {
		if (lcd_kit_ldo_map[i].ldo_num == ldo_num) {
			LCD_KIT_INFO("find ldo%d in lcd_kit_ldo_map\n",
				lcd_kit_ldo_map[i].ldo_num);
			return lcd_kit_ldo_map[i].pmu_type;
		}
	}

	LCD_KIT_INFO("[%s] can not find ldo%d, use default\n",
		__func__, ldo_num);
	return DEVICE_LCDANALOG;
}

static void lcd_kit_bias_init(struct charger_power_operators *charger_ops)
{
	if ((power_hdl->lcd_vsp.buf) &&
		(power_hdl->lcd_vsp.buf[POWER_MODE] == REGULATOR_MODE)) {
		charger_ops = get_operators(CHARGER_POWER_MODULE_NAME_STR);
		if (!charger_ops) {
			LCD_KIT_ERR("can not get charger_power_ops!\n");
			return;
		}
		charger_ops->charger_power_set_voltage(DEVICE_LCD_BIAS,
			power_hdl->lcd_vsp.buf[POWER_VOLTAGE]);
		charger_ops->charger_power_set_voltage(DEVICE_LCD_VSP,
			power_hdl->lcd_vsp.buf[POWER_VOLTAGE]);
		charger_ops->charger_power_set_voltage(DEVICE_LCD_VSN,
			power_hdl->lcd_vsn.buf[POWER_VOLTAGE]);
	}
}

static void lcd_kit_vci_init(struct pmu_operators *pmu_ops)
{
	if ((power_hdl->lcd_vci.buf) &&
		(power_hdl->lcd_vci.buf[POWER_MODE] == REGULATOR_MODE)) {
		/* judge vci is valid */
		if (power_hdl->lcd_vci.buf[POWER_NUMBER] > 0) {
			g_lcd_kit_vci = lcd_ldo_sel(
				power_hdl->lcd_vci.buf[POWER_NUMBER]);
			pmu_ops = get_operators(PMU_MODULE_NAME_STR);
			if (!pmu_ops) {
				LCD_KIT_ERR("can not get pmu_ops!\n");
				return;
			}
			pmu_ops->pmu_set_voltage(g_lcd_kit_vci,
				power_hdl->lcd_vci.buf[POWER_VOLTAGE]);
		}
	}
}

static void lcd_kit_iovcc_init(struct pmu_operators *pmu_ops)
{
	if ((power_hdl->lcd_iovcc.buf) &&
		(power_hdl->lcd_iovcc.buf[POWER_MODE] == REGULATOR_MODE)) {
		/* judge iovcc is valid */
		if (power_hdl->lcd_iovcc.buf[POWER_NUMBER] > 0) {
			pmu_ops = get_operators(PMU_MODULE_NAME_STR);
			if (!pmu_ops) {
				LCD_KIT_ERR("can not get pmu_ops!\n");
				return;
			}
			pmu_ops->pmu_set_voltage(DEVICE_LCDIO,
				power_hdl->lcd_iovcc.buf[POWER_VOLTAGE]);
		}
	}
}

static void lcd_kit_vdd_init(struct pmu_operators *pmu_ops)
{
	if ((power_hdl->lcd_vdd.buf) &&
		(power_hdl->lcd_vdd.buf[POWER_MODE] == REGULATOR_MODE)) {
		/* judge vdd is valid */
		if (power_hdl->lcd_vdd.buf[POWER_NUMBER] > 0) {
			g_lcd_kit_vdd = lcd_ldo_sel(
				power_hdl->lcd_vdd.buf[POWER_NUMBER]);
			pmu_ops = get_operators(PMU_MODULE_NAME_STR);
			if (!pmu_ops) {
				LCD_KIT_ERR("can not get pmu_ops!\n");
				return;
			}
			pmu_ops->pmu_set_voltage(g_lcd_kit_vdd,
				power_hdl->lcd_vdd.buf[POWER_VOLTAGE]);
		}
	}
}

static void lcd_kit_poweric_init(void)
{
	uint32_t gpio_vaule;

	if ((power_hdl->lcd_poweric.buf) &&
		(power_hdl->lcd_poweric.buf[POWER_MODE] == GPIO_MODE)) {
		if (power_hdl->lcd_poweric.buf[POWER_NUMBER] > 0) {
			gpio_vaule = power_hdl->lcd_poweric.buf[POWER_VOLTAGE];
			lcd_kit_gpio_tx(LCD_KIT_POWERIC, GPIO_REQ);
			lcd_kit_gpio_tx(LCD_KIT_POWERIC, gpio_vaule);
			lcd_kit_gpio_tx(LCD_KIT_POWERIC, GPIO_RELEASE);
			LCD_KIT_INFO("gpio vaule is [%d]!\n", gpio_vaule);
			return;
		}
	}
}

int lcd_kit_power_init(void)
{
	struct charger_power_operators *charger_ops = NULL;
	struct pmu_operators *pmu_ops = NULL;

	lcd_kit_poweric_init();
	lcd_kit_bias_init(charger_ops);
	lcd_kit_vci_init(pmu_ops);
	lcd_kit_iovcc_init(pmu_ops);
	lcd_kit_vdd_init(pmu_ops);
	return LCD_KIT_OK;
}
