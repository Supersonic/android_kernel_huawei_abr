/*
 * lcd_kit_disp.c
 *
 * lcdkit display function for lcd driver
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

#include "lcd_kit_disp.h"
#include "bias_bl_utils.h"
#include "bias_ic_common.h"

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"
#include "lcd_kit_utils.h"
#include "lcd_kit_common.h"
#include "lcd_kit_power.h"
#include "lcd_kit_adapt.h"
#include "rt4801h.h"
#include "tps65132.h"
#include "lm3697.h"
#include "ktd3133.h"
#include "sgm37603a.h"
#include "nt50356.h"
#include "rt4831.h"
#include "aw99703.h"
#include "sm5109.h"
#include "sm5350.h"
#include "ktd3136.h"
#include "lp8556.h"
#include "rt8555.h"
#include "../backlight/lm36923.h"
#include "../backlight/ktz8864.h"
#include "../backlight/lm36274.h"

#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#include <string.h>
#include <debug.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#include <mach/mt_pm_ldo.h>
#include <mach/mt_gpio.h>
#endif

#define LOG_TAG		"LCM"
#define FRAME_WIDTH	720
#define FRAME_HEIGHT	1440
#define BL_LEVEL_FULL_LK 255
#define OFFSET_DEF_VAL (-1)

LCM_UTIL_FUNCS lcm_util_mtk;

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
	lcm_util_mtk.dsi_set_cmdq_V2(cmd, count, ppara, force_update)

static struct mtk_panel_info lcd_kit_pinfo = {0};

#define DETECT_ERR	(-1)
#define DETECT_GPIOID	0
#define DETECT_CHIPID	1
#define DETECT_LCD_TYPE	2
#define DEFAULT_LCD_ID	0x0A
extern char *g_lcm_compatible;
static struct lcd_kit_disp_desc g_lcd_kit_disp_info;

struct lcd_kit_disp_desc *lcd_kit_get_disp_info(void)
{
	return &g_lcd_kit_disp_info;
}

static int lcm_is_panel_find(void)
{
	/* LCD ID (0~9) */
	if (disp_info->lcd_id <= DEFAULT_LCD_ID)
		return 1;
	return 0;
}

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	if (!util) {
		LCD_KIT_ERR("util is null point!\n");
		return;
	}
	memcpy(&lcm_util_mtk, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_find_panel_params(LCM_PARAMS *params,
	struct mtk_panel_info *pinfo)
{
	params->type = pinfo->panel_lcm_type;

	params->width = pinfo->xres;
	params->height = pinfo->yres;
	params->physical_width = pinfo->width;
	params->physical_height = pinfo->height;

	params->dsi.mode = pinfo->panel_dsi_mode;
	params->dsi.switch_mode = pinfo->panel_dsi_switch_mode;
	params->dsi.switch_mode_enable = 0;
	/* MTK_PANEL_OUTPUT_MODE */
	params->dsi.output_mode  = pinfo->output_mode;

	/* DSI */
	/* Command mode setting */
	params->dsi.LANE_NUM = pinfo->mipi.lane_nums;
	/*
	 * The following defined the fomat for data
	 * coming from LCD engine
	 */
	params->dsi.data_format.color_order = pinfo->bgr_fmt;
	params->dsi.data_format.trans_seq = pinfo->panel_trans_seq;
	params->dsi.data_format.padding = pinfo->panel_data_padding;
	params->dsi.data_format.format = pinfo->bpp;

	/* Highly depends on LCD driver capability. */
	params->dsi.packet_size = pinfo->panel_packtet_size;
	/* video mode timing */
	params->dsi.PS = pinfo->panel_ps;

	params->dsi.vertical_sync_active = pinfo->ldi.v_pulse_width;
	params->dsi.vertical_backporch = pinfo->ldi.v_back_porch;
	params->dsi.vertical_frontporch = pinfo->ldi.v_front_porch;
	params->dsi.vertical_frontporch_for_low_power =
		pinfo->ldi.v_front_porch_forlp;
	params->dsi.vertical_active_line = pinfo->yres;

	params->dsi.horizontal_sync_active = pinfo->ldi.h_pulse_width;
	params->dsi.horizontal_backporch = pinfo->ldi.h_back_porch;
	params->dsi.horizontal_frontporch = pinfo->ldi.h_front_porch;
	params->dsi.horizontal_active_pixel = pinfo->xres;

	 /* this value must be in MTK suggested table */
	params->dsi.PLL_CLOCK = pinfo->pxl_clk_rate;
	params->dsi.data_rate = pinfo->data_rate;
	params->dsi.IsCphy = pinfo->mipi.phy_mode;

	params->dsi.fbk_div =  pinfo->pxl_fbk_div;
	params->dsi.CLK_HS_POST = pinfo->mipi.clk_post_adjust;
	params->dsi.ssc_disable = pinfo->ssc_disable;
	params->dsi.ssc_range = pinfo->ssc_range;
	params->dsi.clk_lp_per_line_enable = pinfo->mipi.lp11_flag;
	params->dsi.rg_vrefsel_vcm_adjust = pinfo->mipi.rg_vrefsel_vcm_adjust;
	params->dsi.esd_check_enable = pinfo->esd_enable;
	params->dsi.customization_esd_check_enable = 0;
	if (pinfo->mipi.non_continue_en == 0)
		params->dsi.cont_clock = 1;
	else
		params->dsi.cont_clock = 0;
#ifdef MTK_ROUND_CORNER_SUPPORT
	if (pinfo->round_corner.round_corner_en) {
		params->round_corner_params.round_corner_en = pinfo->round_corner.round_corner_en;
		params->round_corner_params.full_content = pinfo->round_corner.full_content;
		params->round_corner_params.h = pinfo->round_corner.h;
		params->round_corner_params.h_bot = pinfo->round_corner.h_bot;
		params->round_corner_params.tp_size = pinfo->round_corner.tp_size;
		params->round_corner_params.lt_addr = pinfo->round_corner.lt_addr;
	}
#endif
}

static void lcm_get_default_panel_params(LCM_PARAMS *params)
{
	/* this is test code,just for fastboot version start ok */
	params->type = LCM_TYPE_DSI;
	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	params->physical_width = FRAME_WIDTH;
	params->physical_height = FRAME_HEIGHT;
	params->dsi.mode = SYNC_PULSE_VDO_MODE;
	params->dsi.switch_mode_enable = 0;

	/* DSI the number is default value */
	/* Command mode setting */
	params->dsi.LANE_NUM = 4;
	/*
	 * The following defined the fomat for data
	 * coming from LCD engine
	 */
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	/* Highly depends on LCD driver capability. */
	params->dsi.packet_size = 256;
	/* video mode timing */
	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active = 2;
	params->dsi.vertical_backporch = 16;
	params->dsi.vertical_frontporch = 9;
	params->dsi.vertical_active_line = 1440;

	params->dsi.horizontal_sync_active = 20;
	params->dsi.horizontal_backporch = 40;
	params->dsi.horizontal_frontporch = 40;
	params->dsi.horizontal_active_pixel = 720;
	params->dsi.PLL_CLOCK = 217;
}

static void lcm_get_dsc_params(LCM_PARAMS *params,
	struct mtk_panel_info *pinfo)
{
	params->dsi.dsc_enable = pinfo->dsc_enable;
	params->dsi.dsc_params.ver = pinfo->dsc_params.ver;
	params->dsi.dsc_params.slice_mode = pinfo->dsc_params.slice_mode;
	params->dsi.dsc_params.rgb_swap = pinfo->dsc_params.rgb_swap;
	params->dsi.dsc_params.dsc_cfg = pinfo->dsc_params.dsc_cfg;
	params->dsi.dsc_params.rct_on = pinfo->dsc_params.rct_on;
	params->dsi.dsc_params.bit_per_channel = pinfo->dsc_params.bit_per_channel;
	params->dsi.dsc_params.dsc_line_buf_depth = pinfo->dsc_params.dsc_line_buf_depth;
	params->dsi.dsc_params.bp_enable = pinfo->dsc_params.bp_enable;
	params->dsi.dsc_params.bit_per_pixel = pinfo->dsc_params.bit_per_pixel;
	params->dsi.dsc_params.pic_height = pinfo->dsc_params.pic_height;
	params->dsi.dsc_params.pic_width = pinfo->dsc_params.pic_width;
	params->dsi.dsc_params.slice_height = pinfo->dsc_params.slice_height;
	params->dsi.dsc_params.slice_width = pinfo->dsc_params.slice_width;
	params->dsi.dsc_params.chunk_size = pinfo->dsc_params.chunk_size;
	params->dsi.dsc_params.xmit_delay = pinfo->dsc_params.xmit_delay;
	params->dsi.dsc_params.dec_delay = pinfo->dsc_params.dec_delay;
	params->dsi.dsc_params.scale_value = pinfo->dsc_params.scale_value;
	params->dsi.dsc_params.increment_interval = pinfo->dsc_params.increment_interval;
	params->dsi.dsc_params.decrement_interval = pinfo->dsc_params.decrement_interval;
	params->dsi.dsc_params.line_bpg_offset = pinfo->dsc_params.line_bpg_offset;
	params->dsi.dsc_params.nfl_bpg_offset = pinfo->dsc_params.nfl_bpg_offset;
	params->dsi.dsc_params.slice_bpg_offset = pinfo->dsc_params.slice_bpg_offset;
	params->dsi.dsc_params.initial_offset = pinfo->dsc_params.initial_offset;
	params->dsi.dsc_params.final_offset = pinfo->dsc_params.final_offset;
	params->dsi.dsc_params.flatness_minqp = pinfo->dsc_params.flatness_minqp;
	params->dsi.dsc_params.flatness_maxqp = pinfo->dsc_params.flatness_maxqp;
	params->dsi.dsc_params.rc_model_size = pinfo->dsc_params.rc_model_size;
	params->dsi.dsc_params.rc_edge_factor = pinfo->dsc_params.rc_edge_factor;
	params->dsi.dsc_params.rc_quant_incr_limit0 = pinfo->dsc_params.rc_quant_incr_limit0;
	params->dsi.dsc_params.rc_quant_incr_limit1 = pinfo->dsc_params.rc_quant_incr_limit1;
	params->dsi.dsc_params.rc_tgt_offset_hi = pinfo->dsc_params.rc_tgt_offset_hi;
	params->dsi.dsc_params.rc_tgt_offset_lo = pinfo->dsc_params.rc_tgt_offset_lo;
}

static void lcm_get_params(LCM_PARAMS *params)
{
	struct mtk_panel_info *pinfo = &lcd_kit_pinfo;

	if (!params) {
		LCD_KIT_ERR("params is null point!\n");
		return;
	}

	memset(params, 0, sizeof(LCM_PARAMS));

	if (lcm_is_panel_find())
		lcm_get_find_panel_params(params, pinfo);
	else
		lcm_get_default_panel_params(params);

	/* dsc params */
	if (pinfo->dsc_enable)
		lcm_get_dsc_params(params, pinfo);
}

void get_res_params(int *xres, int *yres)
{
	if (xres != NULL && yres != NULL) {
		*xres = lcd_kit_pinfo.xres;
		*yres = lcd_kit_pinfo.yres;
	}
}

static int32_t poweric_cmd_detect()
{
	uint8_t read_value = 0;
	int32_t ret;
	int out_len = 0;

	ret = lcd_kit_dsi_cmds_rx(NULL, &read_value, out_len,
		&disp_info->elvdd_detect.cmds);
	if (ret != 0) {
		LCD_KIT_ERR("mipi rx failed!\n");
		return LCD_KIT_OK;
	}
	if ((read_value & disp_info->elvdd_detect.exp_value_mask) !=
		disp_info->elvdd_detect.exp_value)
		ret = LCD_KIT_FAIL;

	LCD_KIT_ERR("read_value = 0x%x, exp_value = 0x%x, mask = 0x%x\n",
		read_value,
		disp_info->elvdd_detect.exp_value,
		disp_info->elvdd_detect.exp_value_mask);
	return ret;
}

static int32_t poweric_gpio_detect()
{
	unsigned int gpio_value;
	gpio_value = mt_get_gpio_in(disp_info->elvdd_detect.detect_gpio);
	if (gpio_value != disp_info->elvdd_detect.exp_value) {
		LCD_KIT_ERR("gpio = %d read_value = 0x%x, exp_value = 0x%x\n",
			disp_info->elvdd_detect.detect_gpio, gpio_value,
			disp_info->elvdd_detect.exp_value);
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

static void lcd_kit_poweric_detect()
{
	static int32_t retry_times;
	int32_t ret = 0;

	if (!disp_info->elvdd_detect.support)
		return;
	if (retry_times >= RECOVERY_TIMES) {
		LCD_KIT_WARNING("not need recovery, recovery num:%d\n",
			retry_times);
		retry_times = 0;
		return;
	}
	lcd_kit_delay(disp_info->elvdd_detect.delay, LCD_KIT_WAIT_MS);
	if (disp_info->elvdd_detect.detect_type == ELVDD_MIPI_CHECK_MODE)
		ret = poweric_cmd_detect();
	else if (disp_info->elvdd_detect.detect_type == ELVDD_GPIO_CHECK_MODE)
		ret = poweric_gpio_detect();
	if (ret) {
		LCD_KIT_ERR("detect poweric abnormal, recovery lcd\n");
		if (common_ops->panel_off_hs)
			common_ops->panel_off_hs(NULL);
		if (common_ops->panel_power_on)
			common_ops->panel_power_on(NULL);
		retry_times++;
		return;
	}
	retry_times = 0;
	LCD_KIT_INFO("detect poweric normal\n");
}

static void lcm_init(void)
{
	int ret;
	int out_len = 0;

	LCD_KIT_INFO(" +!\n");
	lcd_kit_pinfo.panel_state = 1;

	if (common_ops->panel_power_on) {
		common_ops->panel_power_on((void *)NULL);
		lcd_kit_poweric_detect();
	}
	/* record panel on time */
	if (disp_info->quickly_sleep_out.support)
		lcd_kit_disp_on_record_time();
	if (lcd_kit_pinfo.blmaxnit.get_blmaxnit_type ==
		GET_BLMAXNIT_FROM_DDIC) {
		ret = lcd_kit_dsi_cmds_rx(NULL,
			(uint8_t *)&lcd_kit_pinfo.blmaxnit.lcd_kit_brightness_ddic_info, out_len,
			&lcd_kit_pinfo.blmaxnit.bl_maxnit_cmds);
		if (ret != LCD_KIT_OK)
			LCD_KIT_ERR("read blmaxnit_reg error!\n");
		LCD_KIT_INFO("lcd_kit_brightness_ddic_info = %d\n",
			lcd_kit_pinfo.blmaxnit.lcd_kit_brightness_ddic_info);
	}
	lcd_kit_get_tp_color();
	LCD_KIT_INFO(" -!\n");
}

static void lcm_suspend(void)
{
	lcd_kit_pinfo.panel_state = 0;

	LCD_KIT_INFO(" +!\n");

	if (common_ops->panel_off_hs)
		common_ops->panel_off_hs(NULL);

	LCD_KIT_INFO(" -!\n");
}

static void lcm_resume(void)
{
	lcm_init();
}

static void lcm_setbacklight(unsigned int level)
{
	int ret;
	int bl_max_level = common_info->backlight.bl_max;
	level = level * bl_max_level / BL_LEVEL_FULL_LK;
	LCD_KIT_INFO("%s, backlight: level = %d\n", __func__, level);

	ret = common_ops->set_mipi_backlight(NULL, level);
	if (ret < 0)
		return;
}

static unsigned int lcm_compare_id(void)
{
	return lcm_is_panel_find();
}

static void lcm_update(unsigned int x, unsigned int y,
	unsigned int width, unsigned int height)
{
	return;
}

LCM_DRIVER lcdkit_mtk_common_panel = {
	.panel_info = &lcd_kit_pinfo,
	.name = "lcdkit_mtk_common_panel_drv",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	.set_backlight = lcm_setbacklight,
	.compare_id     = lcm_compare_id,
	.update = lcm_update,
};

struct mtk_panel_info *lcm_get_panel_info(void)
{
	return &lcd_kit_pinfo;
}

void lcd_kit_change_display_dts(void)
{
	int lcd_type;
	struct mtk_panel_info *panel_info = NULL;

	panel_info = (struct mtk_panel_info *)lcdkit_mtk_common_panel.panel_info;
	if (panel_info == NULL) {
		LCD_KIT_ERR("panel info is NULL\n");
		return;
	}

	/* bias ic change dts status */
	if (panel_info->bias_ic_ctrl_mode == GPIO_THEN_I2C_MODE) {
		tps65132_set_bias_status();
		rt4801h_set_bias_status();
		sm5109_set_bias_status();
	}
	/* backlight ic dts status */
	if (panel_info->bl_ic_ctrl_mode == MTK_PWM_HIGH_I2C_MODE) {
		lm3697_set_backlight_status();
		ktd3133_set_backlight_status();
#ifdef MTK_LCD_KIT_SUPPORT
		lm36923_set_backlight_status();
#endif
		ktz8864_set_backlight_status();
		rt4831_set_backlight_status();
		lm36274_set_backlight_status();
		nt50356_set_backlight_status();
		sgm37603a_set_backlight_status();
		aw99703_set_backlight_status();
		sm5350_set_backlight_status();
		ktd3133_set_backlight_status();
		lp8556_set_backlight_status();
		rt8555_set_backlight_status();
	} else if (panel_info->bl_ic_ctrl_mode == MTK_AAL_I2C_MODE) {
		sgm37603a_set_backlight_status();
#ifdef MTK_LCD_KIT_SUPPORT
		lm36923_set_backlight_status();
#endif
		ktz8864_set_backlight_status();
		rt4831_set_backlight_status();
		lm36274_set_backlight_status();
		aw99703_set_backlight_status();
		sm5350_set_backlight_status();
		ktd3133_set_backlight_status();
		nt50356_set_backlight_status();
	} else if (panel_info->bl_ic_ctrl_mode == MTK_MIPI_BL_IC_PWM_MODE) {
		lm36274_set_backlight_status();
		nt50356_set_backlight_status();
		rt4831_set_backlight_status();
		ktz8864_set_backlight_status();
	} else if (panel_info->bl_ic_ctrl_mode == PWM_I2C_MODE) {
		lp8556_set_backlight_status();
		rt8555_set_backlight_status();
	}
	/* lcd display info dts status */
	lcd_type = lcd_kit_get_lcd_type();
	if (lcd_type == LCD_KIT) {
		lcdkit_set_lcd_panel_type(disp_info->compatible);
		lcdkit_set_lcd_panel_status(disp_info->lcd_name);
	}
	if (lcd_kit_pinfo.blmaxnit.get_blmaxnit_type ==
		GET_BLMAXNIT_FROM_DDIC)
		lcdkit_set_lcd_ddic_max_brightness(
			lcd_kit_get_blmaxnit(&lcd_kit_pinfo));
}

void lcd_kit_btb_check_init()
{
	int i;
	int btb_gpio;

	if (!common_info->btb_support) {
		LCD_KIT_INFO("LCD BTB check not support\n");
		return;
	}

	for (i = 0; i < common_info->lcd_btb_gpio.cnt; i++) {
		btb_gpio = common_info->lcd_btb_gpio.buf[i];
		if (common_info->btb_check_type == LCD_KIT_BTB_CHECK_GPIO)
			mt_set_gpio_mode(btb_gpio, GPIO_MODE_00);
		else if (common_info->btb_check_type == LCD_KIT_BTB_CHECK_IRQ)
			mt_set_gpio_mode(btb_gpio, GPIO_MODE_02);

		mt_set_gpio_dir(btb_gpio, GPIO_DIR_IN);
		mt_set_gpio_pull_select(btb_gpio, GPIO_PULL_UP);
	}
	LCD_KIT_INFO("LCD BTB check init OK\n");
}

int lcd_kit_init(void)
{
	int lcd_type;
	int detect_type;
	int ret;
	struct mtk_panel_info *panel_info = NULL;
	int offset;
	void *pfdt = NULL;

	/* adapt init */
	lcd_kit_adapt_init();

	/* init lcd id invaild */
	disp_info->lcd_id = 0xff;

	detect_type = lcd_kit_get_detect_type();
	switch (detect_type) {
	case DETECT_LCD_TYPE:
		lcd_kit_get_lcdname();
		break;
	default:
		LCD_KIT_ERR("lcd: error detect_type\n");
		lcd_kit_set_lcd_name_to_no_lcd();
		return LCD_KIT_FAIL;
	}

	panel_info = (struct mtk_panel_info *)lcdkit_mtk_common_panel.panel_info;
	if (panel_info == NULL) {
		LCD_KIT_ERR("panel info is NULL\n");
		return LCD_KIT_FAIL;
	}

	lcd_type = lcd_kit_get_lcd_type();

	if  (lcd_type == LCD_KIT) {
		/* init lcd id */
		disp_info->lcd_id = lcdkit_get_lcd_id();
		disp_info->product_id = lcd_kit_get_product_id();
		disp_info->compatible = lcd_kit_get_compatible(
			disp_info->product_id, disp_info->lcd_id);
		g_lcm_compatible = disp_info->compatible;
		disp_info->lcd_name = lcd_kit_get_lcd_name(
			disp_info->product_id, disp_info->lcd_id);
		LCD_KIT_ERR(
			"lcd_id:%d, product_id:%d, compatible:%s, lcd_name:%s\n",
			disp_info->lcd_id, disp_info->product_id,
			disp_info->compatible, disp_info->lcd_name);
	} else {
		LCD_KIT_INFO("lcd type is not LCD_KIT.\n");
		return LCD_KIT_FAIL;
	}
	pfdt = get_lk_overlayed_dtb();
	if (pfdt == NULL) {
		LCD_KIT_ERR("pfdt is NULL!\n");
		return LCD_KIT_FAIL;
	}
	offset = fdt_node_offset_by_compatible(pfdt,
		OFFSET_DEF_VAL, disp_info->compatible);
	if (offset < 0) {
		LCD_KIT_INFO("can not find %s node by compatible\n",
			disp_info->compatible);
		return LCD_KIT_FAIL;
	}

	/* utils init */
	lcd_kit_utils_init(lcdkit_mtk_common_panel.panel_info, pfdt, offset);

	/* panel init */
	lcd_kit_panel_init();

	/* power init */
	lcd_kit_power_init();

	/* init btb check gpio */
	lcd_kit_btb_check_init();

	/* bias ic init */
	bias_bl_ops_init();
	if (((struct mtk_panel_info *)(lcdkit_mtk_common_panel.panel_info))->bias_ic_ctrl_mode
		== GPIO_THEN_I2C_MODE) {
		tps65132_init();
		rt4801h_init();
		sm5109_init();
	}

	if (((struct mtk_panel_info *)(lcdkit_mtk_common_panel.panel_info))->bias_ic_ctrl_mode ==
				LCD_BIAS_COMMON_MODE) {
		ret = bias_ic_init();
		if (ret < 0)
			LCD_KIT_INFO("bias ic common init fail\n");
	}

	/* backlight ic init */
	switch (panel_info->bl_ic_ctrl_mode) {
	case MTK_PWM_HIGH_I2C_MODE:
		ktd3133_init();
		lm3697_init();
		sgm37603a_init();
		lm36923_init();
		ktz8864_init(&lcd_kit_pinfo);
		lm36274_init(&lcd_kit_pinfo);
		nt50356_init(&lcd_kit_pinfo);
		rt4831_init(&lcd_kit_pinfo);
		aw99703_init(&lcd_kit_pinfo);
		sm5350_init(&lcd_kit_pinfo);
		ktd3136_init(&lcd_kit_pinfo);
		lp8556_init(&lcd_kit_pinfo);
		rt8555_init(&lcd_kit_pinfo);
		break;
	case MTK_AAL_I2C_MODE:
		lm36923_init();
		sgm37603a_init();
		ktz8864_init(&lcd_kit_pinfo);
		lm36274_init(&lcd_kit_pinfo);
		rt4831_init(&lcd_kit_pinfo);
		aw99703_init(&lcd_kit_pinfo);
		sm5350_init(&lcd_kit_pinfo);
		ktd3136_init(&lcd_kit_pinfo);
		nt50356_init(&lcd_kit_pinfo);
		break;
	case MTK_MIPI_BL_IC_PWM_MODE:
		lm36274_init(&lcd_kit_pinfo);
		nt50356_init(&lcd_kit_pinfo);
		rt4831_init(&lcd_kit_pinfo);
		ktz8864_init(&lcd_kit_pinfo);
		break;
	case PWM_I2C_MODE:
		lp8556_init(&lcd_kit_pinfo);
		rt8555_init(&lcd_kit_pinfo);
	default:
		break;
	}

	return LCD_KIT_OK;
}


