/*
 * lcd_kit_common.c
 *
 * lcdkit common function for lcd driver
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

#include "lcd_kit_common.h"
#include "lcd_kit_bias.h"
#include "lcd_kit_bl.h"
#ifdef MTK_LCD_KIT_SUPPORT
#define LCD_ACTIVE_PANEL_BUTT 1
#define LCD_ACTIVE_PANEL 0
#else
#include "lcd_kit_ext_disp.h"
#endif
#include "lcd_kit_utils.h"

/* log level */
u32 g_lcd_kit_msg_level = MSG_LEVEL_INFO;
/* hw adapt ops */
static struct lcd_kit_adapt_ops *g_adapt_pops;
/* power sequence */
static struct lcd_kit_power_seq_desc g_lcd_kit_power_sequence[LCD_ACTIVE_PANEL_BUTT];
/* power handle */
static struct lcd_kit_power_desc g_lcd_kit_power_handle[LCD_ACTIVE_PANEL_BUTT];
/* common info */
static struct lcd_kit_common_desc g_lcd_kit_common_info[LCD_ACTIVE_PANEL_BUTT];
struct lcd_kit_common_desc *lcd_kit_get_common_info(void)
{
	return &g_lcd_kit_common_info[LCD_ACTIVE_PANEL];
}

struct lcd_kit_power_seq_desc *lcd_kit_get_power_seq(void)
{
	return &g_lcd_kit_power_sequence[LCD_ACTIVE_PANEL];
}

struct lcd_kit_power_desc *lcd_kit_get_power_handle(void)
{
	return &g_lcd_kit_power_handle[LCD_ACTIVE_PANEL];
}

static void lcd_kit_gpio_power_ctrl(int enable,
	enum lcd_kit_power_type power_type,
	struct lcd_kit_adapt_ops *adapt_ops)
{
	if (enable) {
		if (adapt_ops->gpio_enable)
			adapt_ops->gpio_enable(power_type);
	} else {
		if (adapt_ops->gpio_disable)
			adapt_ops->gpio_disable(power_type);
	}
}

static void lcd_kit_regulator_power_ctrl(int enable,
	enum lcd_kit_power_type power_type,
	struct lcd_kit_adapt_ops *adapt_ops)
{
	if (enable) {
		if (adapt_ops->regulator_enable)
			adapt_ops->regulator_enable(power_type);
	} else {
		if (adapt_ops->regulator_disable)
			adapt_ops->regulator_disable(power_type);
	}
}

int lcd_kit_vci_power_ctrl(int enable)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (power_hdl->lcd_vci.buf == NULL) {
		LCD_KIT_ERR("can not get lcd vci!\n");
		return LCD_KIT_FAIL;
	}
	switch (power_hdl->lcd_vci.buf[0]) {
	case GPIO_MODE:
		lcd_kit_gpio_power_ctrl(enable, LCD_KIT_VCI, adapt_ops);
		break;
	case REGULATOR_MODE:
		lcd_kit_regulator_power_ctrl(enable, LCD_KIT_VCI, adapt_ops);
		break;
	case NONE_MODE:
		LCD_KIT_DEBUG("lcd vci mode is none mode\n");
		break;
	default:
		LCD_KIT_ERR("lcd vci mode is not normal\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

int lcd_kit_aod_power_ctrl(int enable)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (power_hdl->lcd_aod.buf == NULL) {
		LCD_KIT_ERR("can not get lcd lcd_aod!\n");
		return LCD_KIT_FAIL;
	}
	switch (power_hdl->lcd_aod.buf[0]) {
	case GPIO_MODE:
		lcd_kit_gpio_power_ctrl(enable, LCD_KIT_AOD, adapt_ops);
		break;
	case REGULATOR_MODE:
		lcd_kit_regulator_power_ctrl(enable, LCD_KIT_AOD, adapt_ops);
		break;
	case NONE_MODE:
		LCD_KIT_DEBUG("lcd lcd_aod mode is none mode\n");
		break;
	default:
		LCD_KIT_ERR("lcd lcd_aod mode is not normal\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}
int lcd_kit_set_mipi_switch_ctrl(int enable)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (power_hdl->lcd_mipi_switch.buf == NULL) {
		LCD_KIT_ERR("can not get lcd mipi switch!\n");
		return LCD_KIT_FAIL;
	}
	switch (power_hdl->lcd_mipi_switch.buf[0]) {
	case GPIO_MODE:
		if (enable) {
			if (adapt_ops->gpio_enable_nolock)
				adapt_ops->gpio_enable_nolock(LCD_KIT_MIPI_SWITCH);
		} else {
			if (adapt_ops->gpio_disable_nolock)
				adapt_ops->gpio_disable_nolock(LCD_KIT_MIPI_SWITCH);
		}
		break;
	case NONE_MODE:
		LCD_KIT_DEBUG("lcd mipi switch is none mode\n");
		break;
	default:
		LCD_KIT_ERR("lcd mipi switch is not normal\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

int lcd_kit_iovcc_power_ctrl(int enable)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (power_hdl->lcd_iovcc.buf == NULL) {
		LCD_KIT_ERR("can not get lcd iovcc!\n");
		return LCD_KIT_FAIL;
	}
	switch (power_hdl->lcd_iovcc.buf[0]) {
	case GPIO_MODE:
		lcd_kit_gpio_power_ctrl(enable, LCD_KIT_IOVCC, adapt_ops);
		break;
	case REGULATOR_MODE:
		lcd_kit_regulator_power_ctrl(enable, LCD_KIT_IOVCC, adapt_ops);
		break;
	case NONE_MODE:
		LCD_KIT_DEBUG("lcd iovcc mode is none mode\n");
		break;
	default:
		LCD_KIT_ERR("lcd iovcc mode is not normal\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

int lcd_kit_vdd_power_ctrl(int enable)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (power_hdl->lcd_vdd.buf == NULL) {
		LCD_KIT_ERR("can not get lcd vdd!\n");
		return LCD_KIT_FAIL;
	}
	switch (power_hdl->lcd_vdd.buf[0]) {
	case GPIO_MODE:
		lcd_kit_gpio_power_ctrl(enable, LCD_KIT_VDD, adapt_ops);
		break;
	case REGULATOR_MODE:
		lcd_kit_regulator_power_ctrl(enable, LCD_KIT_VDD, adapt_ops);
		break;
	case NONE_MODE:
		LCD_KIT_DEBUG("lcd vdd mode is none mode\n");
		break;
	default:
		LCD_KIT_ERR("lcd vdd mode is not normal\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

int lcd_kit_vsp_power_ctrl(int enable)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (power_hdl->lcd_vsp.buf == NULL) {
		LCD_KIT_ERR("can not get lcd vsp!\n");
		return LCD_KIT_FAIL;
	}
	switch (power_hdl->lcd_vsp.buf[0]) {
	case GPIO_MODE:
		lcd_kit_gpio_power_ctrl(enable, LCD_KIT_VSP, adapt_ops);
		break;
	case REGULATOR_MODE:
		lcd_kit_regulator_power_ctrl(enable, LCD_KIT_VSP, adapt_ops);
		break;
	case NONE_MODE:
		LCD_KIT_DEBUG("lcd vsp mode is none mode\n");
		break;
	default:
		LCD_KIT_ERR("lcd vsp mode is not normal\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

int lcd_kit_vsn_power_ctrl(int enable)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (power_hdl->lcd_vsn.buf == NULL) {
		LCD_KIT_ERR("can not get lcd vsn!\n");
		return LCD_KIT_FAIL;
	}
	switch (power_hdl->lcd_vsn.buf[0]) {
	case GPIO_MODE:
		lcd_kit_gpio_power_ctrl(enable, LCD_KIT_VSN, adapt_ops);
		break;
	case REGULATOR_MODE:
		lcd_kit_regulator_power_ctrl(enable, LCD_KIT_VSN, adapt_ops);
		break;
	case NONE_MODE:
		LCD_KIT_DEBUG("lcd vsn mode is none mode\n");
		break;
	default:
		LCD_KIT_ERR("lcd vsn mode is not normal\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

int lcd_kit_set_bias_voltage(void)
{
	struct lcd_kit_bias_ops *bias_ops = NULL;
	int ret = LCD_KIT_OK;

	/* bias recognize */
	lcd_kit_bias_recognize_call();
	bias_ops = lcd_kit_get_bias_ops();
	if (!bias_ops) {
		LCD_KIT_ERR("can not get bias_ops!\n");
		return LCD_KIT_FAIL;
	}
	/* set bias voltage buf[2] means bias value */
	if (bias_ops->set_bias_voltage)
		ret = bias_ops->set_bias_voltage(power_hdl->lcd_vsp.buf[2],
			power_hdl->lcd_vsn.buf[2]);
	return ret;
}

int lcd_kit_set_backlight(int bl_level)
{
	struct lcd_kit_bl_ops *backlight_ops = NULL;
	int ret = LCD_KIT_OK;

	/* bias recognize */
	lcd_kit_backlight_recognize_call();
	backlight_ops = lcd_kit_get_bl_ops();
	if (!backlight_ops) {
		LCD_KIT_ERR("can not get backlight_ops!\n");
		return LCD_KIT_FAIL;
	}
	/* set bl level */
	if (backlight_ops->set_backlight)
		ret = backlight_ops->set_backlight(bl_level);
	return ret;
}

int lcd_kit_reset_power_ctrl(int enable)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (power_hdl->lcd_rst.buf == NULL) {
		LCD_KIT_ERR("can not get lcd reset!\n");
		return LCD_KIT_FAIL;
	}
	switch (power_hdl->lcd_rst.buf[0]) {
	case GPIO_MODE:
		if (enable == LCD_RESET_HIGH) {
			if (adapt_ops->gpio_enable)
				adapt_ops->gpio_enable(LCD_KIT_RST);
		} else {
			if (adapt_ops->gpio_disable)
				adapt_ops->gpio_disable(LCD_KIT_RST);
		}
		break;
	default:
		LCD_KIT_ERR("not support mode\n");
		break;
	}
	return LCD_KIT_OK;
}

int lcd_kit_tp_reset_power_ctrl(int enable)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (power_hdl->tp_rst.buf == NULL) {
		LCD_KIT_ERR("can not get lcd reset!\n");
		return LCD_KIT_FAIL;
	}
	switch (power_hdl->tp_rst.buf[0]) {
	case GPIO_MODE:
		/* judge gpio is valid */
		if (power_hdl->tp_rst.buf[1] > 0) {
			if (enable) {
				if (adapt_ops->gpio_enable)
					adapt_ops->gpio_enable(LCD_KIT_TP_RST);
			} else {
				if (adapt_ops->gpio_disable)
					adapt_ops->gpio_disable(LCD_KIT_TP_RST);
			}
		}
		break;
	default:
		LCD_KIT_ERR("not support mode\n");
		break;
	}
	return LCD_KIT_OK;
}

static int lcd_kit_on_cmds(void *hld)
{
	int ret = LCD_KIT_OK;
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	/* init code */
	if (adapt_ops->mipi_tx) {
		ret = adapt_ops->mipi_tx(hld, &common_info->panel_on_cmds);
		if (ret)
			LCD_KIT_ERR("send panel on cmds error\n");
		if (common_info->effect_on.support) {
			ret = adapt_ops->mipi_tx(hld,
				&common_info->effect_on.cmds);
			if (ret)
				LCD_KIT_ERR("send effect on cmds error\n");
		}
	}
	return ret;
}

static int lcd_kit_off_cmds(void *hld)
{
	int ret = LCD_KIT_OK;
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (adapt_ops->mipi_tx)
		ret = adapt_ops->mipi_tx(hld, &common_info->panel_off_cmds);
	return ret;
}

static int lcd_kit_check_reg_report_dsm(void *hld,
	struct lcd_kit_check_reg_dsm *check_reg_dsm)
{
	int ret = LCD_KIT_OK;
	uint8_t read_value[MAX_REG_READ_COUNT] = {0};
	int i;
	char *expect_ptr = NULL;
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	if ((hld == NULL) || (check_reg_dsm == NULL)) {
		LCD_KIT_ERR("null pointer!\n");
		return LCD_KIT_FAIL;
	}

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not register adapt_ops!\n");
		return LCD_KIT_FAIL;
	}

	if (!(check_reg_dsm->support))
		return ret;
	expect_ptr = (char *)check_reg_dsm->value.buf;
	if (adapt_ops->mipi_rx)
		ret = adapt_ops->mipi_rx(hld, read_value, MAX_REG_READ_COUNT,
			&check_reg_dsm->cmds);
	if (ret != LCD_KIT_OK)
		LCD_KIT_ERR("mipi read error!\n");
	for (i = 0; i < check_reg_dsm->cmds.cmd_cnt; i++) {
		if (check_reg_dsm->support_dsm_report) {
			if ((char)read_value[i] != expect_ptr[i]) {
				ret = LCD_KIT_FAIL;
				LCD_KIT_ERR("read_value[%d] = 0x%x, but expect_ptr[%d] = 0x%x!\n",
					i, read_value[i], i, expect_ptr[i]);
				break;
			}
			LCD_KIT_INFO("read_value[%d] = 0x%x same with expect value!\n",
				i, read_value[i]);
		} else {
			LCD_KIT_INFO("read_value[%d] = 0x%x!\n",
				i, read_value[i]);
		}
	}
	return ret;
}


int lcd_kit_panel_cmd_ctrl(void *hld, int enable)
{
	int ret = LCD_KIT_OK;
	int ret_check_reg = LCD_KIT_OK;

	if (enable) {
		ret = lcd_kit_on_cmds(hld);
		ret_check_reg = lcd_kit_check_reg_report_dsm(hld,
			&common_info->check_reg_on);
		if (ret_check_reg != LCD_KIT_OK)
			LCD_KIT_ERR("power on check reg error!\n");
	} else {
		ret = lcd_kit_off_cmds(hld);
		ret_check_reg = lcd_kit_check_reg_report_dsm(hld,
			&common_info->check_reg_off);
		if (ret_check_reg != LCD_KIT_OK)
			LCD_KIT_ERR("power off check reg error!\n");
	}
	return ret;
}


int lcd_kit_event_handler(void *hld, uint32_t event, uint32_t data,
	uint32_t delay)
{
	int ret = LCD_KIT_OK;

	LCD_KIT_INFO("event = 0x%x, data = 0x%x, delay = %d\n",
		event, data, delay);
	switch (event) {
	case EVENT_VCI:
		ret = lcd_kit_vci_power_ctrl(data);
		break;
	case EVENT_IOVCC:
		ret = lcd_kit_iovcc_power_ctrl(data);
		break;
	case EVENT_VSP:
		ret = lcd_kit_vsp_power_ctrl(data);
		break;
	case EVENT_VSN:
		ret = lcd_kit_vsn_power_ctrl(data);
		/* set biase voltge */
		lcd_kit_set_bias_voltage();
		break;
	case EVENT_RESET:
		ret = lcd_kit_reset_power_ctrl(data);
		break;
	case EVENT_MIPI:
		ret = lcd_kit_panel_cmd_ctrl(hld, data);
		break;
	case EVENT_EARLY_TS:
	case EVENT_LATER_TS:
		ret = lcd_kit_tp_reset_power_ctrl(data);
		break;
	case EVENT_VDD:
		ret = lcd_kit_vdd_power_ctrl(data);
		break;
	case EVENT_AOD:
		ret = lcd_kit_aod_power_ctrl(data);
		break;
	case EVENT_MIPI_SWITCH:
		ret = lcd_kit_set_mipi_switch_ctrl(data);
		break;
	case EVENT_NONE:
		LCD_KIT_INFO("none event\n");
		break;
	default:
		LCD_KIT_INFO("event not exist\n");
		break;
	}
	if (delay > 0)
		lcd_kit_delay(delay, LCD_KIT_WAIT_MS);
	return ret;
}

int lcd_kit_panel_power_on(void *hld)
{
	int i;
	int ret = LCD_KIT_OK;
	struct lcd_kit_array_data *pevent = NULL;

	pevent = power_seq->power_on_seq.arry_data;
	for (i = 0; i < power_seq->power_on_seq.cnt; i++) {
		if ((!pevent) || (!pevent->buf)) {
			LCD_KIT_ERR("on pevent is null!\n");
			return LCD_KIT_FAIL;
		}
		/*
		 * buf[0] means on or off buf[1] means gpio number
		 * buf[2] means delay time
		 */
		ret = lcd_kit_event_handler(hld, pevent->buf[0], pevent->buf[1],
			pevent->buf[2]);
		if (ret)
			LCD_KIT_ERR("send on event 0x%x error!\n",
				pevent->buf[0]);
		pevent++;
	}
	return ret;
}

int lcd_kit_panel_power_off(void *hld)
{
	int i;
	int ret = LCD_KIT_OK;
	struct lcd_kit_array_data *pevent = NULL;

	pevent = power_seq->power_off_seq.arry_data;
	for (i = 0; i < power_seq->power_off_seq.cnt; i++) {
		if ((!pevent) || (!pevent->buf)) {
			LCD_KIT_ERR("off pevent is null!\n");
			return LCD_KIT_FAIL;
		}
		/*
		 * buf[0] means on or off buf[1] means gpio number
		 * buf[2] means delay time
		 */
		ret = lcd_kit_event_handler(hld, pevent->buf[0], pevent->buf[1],
			pevent->buf[2]);
		if (ret)
			LCD_KIT_ERR("send off event 0x%x error!\n",
				pevent->buf[0]);
		pevent++;
	}
	return ret;
}

int lcd_kit_panel_on_lp(void *hld)
{
	int i;
	int ret = LCD_KIT_OK;
	struct lcd_kit_array_data *pevent = NULL;

	pevent = power_seq->panel_on_lp_seq.arry_data;
	for (i = 0; i < power_seq->panel_on_lp_seq.cnt; i++) {
		if ((!pevent) || (!pevent->buf)) {
			LCD_KIT_ERR("on lp pevent is null!\n");
			return LCD_KIT_FAIL;
		}
		/*
		 * buf[0] means on or off buf[1] means gpio number
		 * buf[2] means delay time
		 */
		ret = lcd_kit_event_handler(hld, pevent->buf[0], pevent->buf[1],
			pevent->buf[2]);
		if (ret)
			LCD_KIT_ERR("send on lp event 0x%x error!\n",
				pevent->buf[0]);
		pevent++;
	}
	return ret;
}

int lcd_kit_panel_on_hs(void *hld)
{
	int i;
	int ret = LCD_KIT_OK;
	struct lcd_kit_array_data *pevent = NULL;

	pevent = power_seq->panel_on_hs_seq.arry_data;
	for (i = 0; i < power_seq->panel_on_hs_seq.cnt; i++) {
		if ((!pevent) || (!pevent->buf)) {
			LCD_KIT_ERR("on hs pevent is null!\n");
			return LCD_KIT_FAIL;
		}
		/*
		 * buf[0] means on or off buf[1] means gpio number
		 * buf[2] means delay time
		 */
		ret = lcd_kit_event_handler(hld, pevent->buf[0], pevent->buf[1],
			pevent->buf[2]);
		if (ret)
			LCD_KIT_ERR("send on hs event 0x%x error!\n",
				pevent->buf[0]);
		pevent++;
	}
	return ret;
}

int lcd_kit_panel_off_hs(void *hld)
{
	int i;
	int ret = LCD_KIT_OK;
	struct lcd_kit_array_data *pevent = NULL;

	pevent = power_seq->panel_off_hs_seq.arry_data;
	for (i = 0; i < power_seq->panel_off_hs_seq.cnt; i++) {
		if ((!pevent) || (!pevent->buf)) {
			LCD_KIT_ERR("off hs pevent is null!\n");
			return LCD_KIT_FAIL;
		}
		/*
		 * buf[0] means on or off buf[1] means gpio number
		 * buf[2] means delay time
		 */
		ret = lcd_kit_event_handler(hld, pevent->buf[0], pevent->buf[1],
			pevent->buf[2]);
		if (ret)
			LCD_KIT_ERR("send off hs event 0x%x error!\n",
				pevent->buf[0]);
		pevent++;
	}
	return ret;
}

int lcd_kit_panel_off_lp(void *hld)
{
	int i;
	int ret = LCD_KIT_OK;
	struct lcd_kit_array_data *pevent = NULL;

	pevent = power_seq->panel_off_lp_seq.arry_data;
	for (i = 0; i < power_seq->panel_off_lp_seq.cnt; i++) {
		if ((!pevent) || (!pevent->buf)) {
			LCD_KIT_ERR("off lp pevent is null!\n");
			return LCD_KIT_FAIL;
		}
		/*
		 * buf[0] means on or off buf[1] means gpio number
		 * buf[2] means delay time
		 */
		ret = lcd_kit_event_handler(hld, pevent->buf[0], pevent->buf[1],
			pevent->buf[2]);
		if (ret)
			LCD_KIT_ERR("send off lp event 0x%x error!\n",
				pevent->buf[0]);
		pevent++;
	}
	return ret;
}

int lcd_kit_mipi_set_backlight(void *hld, u32 bl_level)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (bl_level > common_info->backlight.bl_max)
		bl_level = common_info->backlight.bl_max;
	if ((bl_level < common_info->backlight.bl_min) && bl_level)
		bl_level = common_info->backlight.bl_min;
	switch (common_info->backlight.order) {
	case BL_BIG_ENDIAN:
		if (common_info->backlight.bl_max <= 0xFF) {
			common_info->backlight.bl_cmd.cmds[0].payload[1] =
				bl_level;
		} else {
			/* 8 means 8 bits */
			common_info->backlight.bl_cmd.cmds[0].payload[1] =
				(bl_level >> 8) & 0xFF;
			common_info->backlight.bl_cmd.cmds[0].payload[2] =
				bl_level & 0xFF;
		}
		break;
	case BL_LITTLE_ENDIAN:
		if (common_info->backlight.bl_max <= 0xFF) {
			common_info->backlight.bl_cmd.cmds[0].payload[1] =
				bl_level;
		} else {
			/* 8 means 8 bits */
			common_info->backlight.bl_cmd.cmds[0].payload[1] =
				bl_level & 0xFF;
			common_info->backlight.bl_cmd.cmds[0].payload[2] =
				(bl_level >> 8) & 0xFF;
		}
		break;
	default:
		break;
	}
	if (common_info->backlight.need_prepare)
		(void)adapt_ops->mipi_tx(hld, &common_info->backlight.prepare);
	return adapt_ops->mipi_tx(hld, &common_info->backlight.bl_cmd);
}

static int lcd_kit_parse_power(const char *np)
{
	/* vci */
	if (power_hdl->lcd_vci.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-vci",
			&power_hdl->lcd_vci);
	/* iovcc */
	if (power_hdl->lcd_iovcc.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-iovcc",
			&power_hdl->lcd_iovcc);
	/* vsp */
	if (power_hdl->lcd_vsp.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-vsp",
			&power_hdl->lcd_vsp);
	/* vsn */
	if (power_hdl->lcd_vsn.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-vsn",
			&power_hdl->lcd_vsn);
	/* lcd reset */
	if (power_hdl->lcd_rst.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-reset",
			&power_hdl->lcd_rst);
	/* backlight */
	if (power_hdl->lcd_backlight.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-backlight",
			&power_hdl->lcd_backlight);
	/* TE0 */
	if (power_hdl->lcd_te0.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-te0",
			&power_hdl->lcd_te0);
	/* tp reset */
	if (power_hdl->tp_rst.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,tp-reset",
			&power_hdl->tp_rst);
	/* lcd vdd */
	if (power_hdl->lcd_vdd.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-vdd",
			&power_hdl->lcd_vdd);
	if (power_hdl->lcd_aod.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-aod",
			&power_hdl->lcd_aod);
	if (power_hdl->lcd_poweric.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-poweric",
			&power_hdl->lcd_poweric);
	if (power_hdl->lcd_mipi_switch.buf == NULL)
		lcd_kit_parse_array(np, "lcd-kit,lcd-mipi-switch",
			&power_hdl->lcd_mipi_switch);
	return LCD_KIT_OK;
}

static int lcd_kit_parse_power_seq(const char *np)
{
	/* here 3 means 3 arrays */
	lcd_kit_parse_arrays(np, "lcd-kit,power-on-stage",
		&power_seq->power_on_seq, 3);
	lcd_kit_parse_arrays(np, "lcd-kit,lp-on-stage",
		&power_seq->panel_on_lp_seq, 3);
	lcd_kit_parse_arrays(np, "lcd-kit,hs-on-stage",
		&power_seq->panel_on_hs_seq, 3);
	lcd_kit_parse_arrays(np, "lcd-kit,power-off-stage",
		&power_seq->power_off_seq, 3);
	lcd_kit_parse_arrays(np, "lcd-kit,hs-off-stage",
		&power_seq->panel_off_hs_seq, 3);
	lcd_kit_parse_arrays(np, "lcd-kit,lp-off-stage",
		&power_seq->panel_off_lp_seq, 3);
	return LCD_KIT_OK;
}

static int lcd_kit_parse_common_info(const char *np)
{
	/* panel cmds */
	lcd_kit_parse_dcs_cmds(np, "lcd-kit,panel-on-cmds",
		"lcd-kit,panel-on-cmds-state", &common_info->panel_on_cmds);
	lcd_kit_parse_dcs_cmds(np, "lcd-kit,panel-off-cmds",
		"lcd-kit,panel-off-cmds-state", &common_info->panel_off_cmds);
	/* backlight */
	lcd_kit_parse_dcs_cmds(np, "lcd-kit,backlight-cmds",
		"lcd-kit,backlight-cmds-state", &common_info->backlight.bl_cmd);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,bl-need-prepare",
		&common_info->backlight.need_prepare, 0);
	lcd_kit_parse_dcs_cmds(np, "lcd-kit,bl-prepare-cmds",
		"lcd-kit,bl-prepare-cmds-state", &common_info->backlight.prepare);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,backlight-order",
		&common_info->backlight.order, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-bl-max",
		&common_info->backlight.bl_max, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-bl-min",
		&common_info->backlight.bl_min, 0);
	/* check backlight short/open */
	lcd_kit_parse_get_u32_default(np, "lcd-kit,check-bl-support",
		&common_info->check_bl_support, 0);
	/* check reg on */
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,check-reg-on-support",
		&common_info->check_reg_on.support, 0);
	if (common_info->check_reg_on.support) {
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,check-reg-on-cmds",
			"lcd-kit,check-reg-on-cmds-state",
			&common_info->check_reg_on.cmds);
		lcd_kit_parse_array(np, "lcd-kit,check-reg-on-value",
			&common_info->check_reg_on.value);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,check-reg-on-support-dsm-report",
			&common_info->check_reg_on.support_dsm_report, 0);
	}
	/* check reg off */
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,check-reg-off-support",
		&common_info->check_reg_off.support, 0);
	if (common_info->check_reg_off.support) {
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,check-reg-off-cmds",
			"lcd-kit,check-reg-off-cmds-state",
			&common_info->check_reg_off.cmds);
		lcd_kit_parse_array(np, "lcd-kit,check-reg-off-value",
			&common_info->check_reg_off.value);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,check-reg-off-support-dsm-report",
			&common_info->check_reg_off.support_dsm_report, 0);
	}
	return LCD_KIT_OK;
}

static int lcd_kit_parse_effect(const char *np)
{
	/* effect on support */
	lcd_kit_parse_get_u32_default(np, "lcd-kit,effect-on-support",
		&common_info->effect_on.support, 0);
	if (common_info->effect_on.support)
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,effect-on-cmds",
			"lcd-kit,effect-on-cmds-state",
			&common_info->effect_on.cmds);
	return LCD_KIT_OK;
}

void lcd_kit_delay(int wait, int waittype)
{
	if (!wait)
		return;

	if (waittype == LCD_KIT_WAIT_US)
		udelay(wait);
	else if (waittype == LCD_KIT_WAIT_MS)
		mdelay(wait);
	else
		/* wait multiply 1000 means delay second */
		mdelay(wait * 1000);
}

int lcd_kit_common_init(const char *np)
{
	if (!np) {
		LCD_KIT_ERR("compatible is null\n");
		return LCD_KIT_FAIL;
	}
	lcd_kit_parse_common_info(np);
	lcd_kit_parse_power_seq(np);
	lcd_kit_parse_power(np);
	lcd_kit_parse_effect(np);
	return LCD_KIT_OK;
}

int lcd_kit_adapt_register(struct lcd_kit_adapt_ops *ops)
{
	if (g_adapt_pops) {
		LCD_KIT_ERR("g_adapt_pops has already been registered!\n");
		return LCD_KIT_FAIL;
	}
	g_adapt_pops = ops;
	return LCD_KIT_OK;
}

int lcd_kit_adapt_unregister(struct lcd_kit_adapt_ops *ops)
{
	if (g_adapt_pops == ops) {
		g_adapt_pops = NULL;
		LCD_KIT_INFO("g_adapt_pops unregister success!\n");
		return LCD_KIT_OK;
	}
	LCD_KIT_ERR("g_adapt_pops unregister fail!\n");
	return LCD_KIT_FAIL;
}

struct lcd_kit_adapt_ops *lcd_kit_get_adapt_ops(void)
{
	return g_adapt_pops;
}

/* common ops */
struct lcd_kit_common_ops g_lcd_kit_common_ops = {
	.panel_power_on = lcd_kit_panel_power_on,
	.panel_on_lp = lcd_kit_panel_on_lp,
	.panel_on_hs = lcd_kit_panel_on_hs,
	.panel_off_hs = lcd_kit_panel_off_hs,
	.panel_off_lp = lcd_kit_panel_off_lp,
	.panel_power_off = lcd_kit_panel_power_off,
	.set_mipi_backlight = lcd_kit_mipi_set_backlight,
	.common_init = lcd_kit_common_init,
};

struct lcd_kit_common_ops *lcd_kit_get_common_ops(void)
{
	return &g_lcd_kit_common_ops;
}
