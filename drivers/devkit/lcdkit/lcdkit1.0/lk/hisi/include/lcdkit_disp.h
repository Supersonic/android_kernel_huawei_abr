/*
 * lcdkit_disp.h
 *
 * lcdkit disp head file for lcd driver
 *
 * Copyright (c) 2018-2020 Huawei Technologies Co., Ltd.
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

#ifndef _LCDKIT_DISP_H_
#define _LCDKIT_DISP_H_

#include <stdint.h>
#include "hisi_mipi_dsi.h"
#include "lcdkit_display_effect.h"
#include "lcdkit_bias_ic_common.h"
#include "lcd_bl.h"
/*
 * macro define
 */
#define LCDKIT_MODULE_NAME		  lcdkit_panel
#define LCDKIT_MODULE_NAME_STR	 "lcdkit_panel"

#define LCDKIT_REG_TOTAL_BYTES	4
#define LCDKIT_REG_TOTAL_WORDS	((LCDKIT_REG_TOTAL_BYTES + 3)/4)

#define LCDKIT_POWER_STATUS_CHECK (1)
#define LCDKIT_TP_COLOR   (1)
#define LCD_OEM_LEN 64
#define HYBRID   (1)
#define AMOLED   (3)


#define BITS(x)	 (1<<x)
/* panel type
 * 0x01:LCD
 * 0x10:OLED
 */
#define  PANEL_TYPE_LCD  BITS(0)
#define  PANEL_TYPE_OLED BITS(1)

#define  POWER_CTRL_BY_NONE BITS(4)
#define  POWER_CTRL_BY_REGULATOR  BITS(0)
#define  POWER_CTRL_BY_GPIO BITS(1)
#define  POWER_CTRL_BY_IC BITS(2)

/*
 * LCD GPIO
 */
#define GPIO_LCDKIT_RESET_NAME	"gpio_lcdkit_reset"
#define GPIO_LCDKIT_TE0_NAME	"gpio_lcdkit_te0"
#define GPIO_LCDKIT_BL_ENABLE_NAME		  "gpio_lcdkit_bl_enable"
#define GPIO_LCDKIT_BL_POWER_ENABLE_NAME   "gpio_lcdkit_bl_power_enable"
#define GPIO_LCDKIT_P5V5_ENABLE_NAME	   "gpio_lcdkit_p5v5_enable"
#define GPIO_LCDKIT_N5V5_ENABLE_NAME	   "gpio_lcdkit_n5v5_enable"
#define GPIO_LCDKIT_IOVCC_NAME	   "gpio_lcdkit_iovcc"
#define GPIO_LCDKIT_VCI_NAME	   "gpio_lcdkit_vci"
#define GPIO_LCDKIT_TP_RESET_NAME	"gpio_lcdkit_tp_reset"
#define GPIO_LCDKIT_VBAT_NAME	   "gpio_lcdkit_vbat"

extern char lcd_type_buf[LCD_TYPE_NAME_MAX];
void lcdkit_dts_set_ic_name(const char *prop_name, const char *name);
void lcdkit_power_on_bias_enable(void);

/*
 * variable define
 */
/* panel data */
extern struct lcdkit_panel_data lcdkit_data;
/* panel info */
extern struct lcdkit_disp_info lcdkit_infos;

/*
 * extern
 */
extern int lcdkit_get_lcd_id(void);

/*
 * struct
 */
/* panel structure */
struct lcdkit_panel_info
{
	uint32_t xres;
	uint32_t yres;
	uint32_t width;
	uint32_t height;
	uint32_t orientation;
	uint32_t bpp;
	uint32_t bgr_fmt;
	uint32_t bl_set_type;
	uint32_t bl_min;
	uint32_t bl_max;
	uint32_t bl_def;
	uint32_t bl_v200;
	uint32_t bl_otm;
	uint32_t blpwm_intr_value;
	uint32_t blpwm_max_value;
	uint32_t type;
	uint8_t pxl_clk_to_pll0;
	char* compatible;
	uint32_t ifbc_type;
	uint8_t ifbc_vesa3x_set;
	char* lcd_name;
	uint8_t frc_enable;
	uint8_t esd_enable;
	uint8_t prefix_ce_support;
	uint8_t prefix_sharpness_support;
	uint8_t sbl_support;
	uint8_t acm_support;
	uint8_t acm_ce_support;
	uint8_t gamma_support;
	/* for dynamic gamma */
	uint8_t dynamic_gamma_support;
	uint8_t sh_aod_support;
	uint32_t sh_aod_start_y;
	uint32_t sh_aod_end_y;
	uint32_t sh_aod_end_y_one_bit;
	uint8_t aod_need_reset;
	uint8_t aod_no_need_init;
	uint8_t txoff_rxulps_en;
	uint8_t hbm_support;
	uint8_t eml_read_reg_flag;
	uint64_t pxl_clk_rate;
	uint32_t pxl_clk_rate_div;
	uint8_t dirty_region_updt_support;
	uint8_t dsi_bit_clk_upt_support;
	uint32_t vsync_ctrl_type;
	uint32_t blpwm_precision_type;
	uint8_t lcd_uninit_step_support;
	uint8_t dsi1_snd_cmd_panel_support;
	uint8_t dpi01_set_change;
	uint32_t blpwm_precision;
	uint32_t blpwm_div;
	uint32_t bl_byte_order;
	uint32_t iovcc_before_vci;
	char* panel_model;
	uint8_t mipi_switch_support;
};

struct lcdkit_panel_mipi
{
	uint8_t vc;
	uint8_t lane_nums;
	uint8_t color_mode;
	uint32_t dsi_bit_clk;
	uint32_t burst_mode;
	uint32_t max_tx_esc_clk;
	uint32_t dsi_bit_clk_val1;
	uint32_t dsi_bit_clk_val2;
	uint32_t dsi_bit_clk_val3;
	uint32_t dsi_bit_clk_val4;
	uint32_t dsi_bit_clk_val5;
	uint32_t dsi_bit_clk_upt;
	uint8_t non_continue_en;
	uint32_t clk_post_adjust;
	uint32_t clk_pre_adjust;
	uint32_t clk_ths_prepare_adjust;
	uint32_t clk_tlpx_adjust;
	uint32_t clk_ths_trail_adjust;
	uint32_t clk_ths_exit_adjust;
	uint32_t clk_ths_zero_adjust;
	uint32_t data_t_hs_trial_adjust;
	uint32_t data_t_hs_prepare_adjust;
	uint32_t data_t_hs_zero_adjust;
	int data_t_lpx_adjust;
	uint32_t rg_vrefsel_vcm_adjust;
	uint32_t rg_vrefsel_lptx_adjust;
	uint32_t rg_lptx_sri_adjust;
	uint32_t rg_vrefsel_vcm_clk_adjust;
	uint32_t rg_vrefsel_vcm_data_adjust;
	uint32_t phy_mode;  // 0: DPHY, 1:CPHY
	uint32_t lp11_flag; /* 0: nomal_lp11_last_time, 1:short_lp11_last_time, 2:disable_lp11 */
	uint32_t hs_wr_to_time;
	uint32_t phy_m_n_count_update;  // 0:old ,1:new can get 988.8M
	uint32_t eotp_disable_flag;  /* 0: eotp enable, 1:eotp disable */
};

struct lcdkit_panel_ldi
{
	uint32_t h_back_porch;
	uint32_t h_front_porch;
	uint32_t h_pulse_width;
	uint32_t v_back_porch;
	uint32_t v_front_porch;
	uint32_t v_pulse_width;
	uint8_t hsync_plr;
	uint8_t vsync_plr;
	uint8_t pixelclk_plr;
	uint8_t data_en_plr;
	uint8_t dpi0_overlap_size;
	uint8_t dpi1_overlap_size;
};

struct lcdkit_platform_config
{
	/* reset gpio */
	uint32_t gpio_lcd_reset;
	/* te gpio */
	uint32_t gpio_lcd_te;
	/* iovcc gpio */
	uint32_t gpio_lcd_iovcc;
	/* vci gpio */
	uint32_t gpio_lcd_vci;
	/* vci gpio */
	uint32_t gpio_tp_rst;
	/* vsp gpio */
	uint32_t gpio_lcd_vsp;
	/* vsn gpio */
	uint32_t gpio_lcd_vsn;
	/* bl gpio */
	uint32_t gpio_lcd_bl;
	uint32_t gpio_lcd_bl_power;
	/* vbat gpio */
	uint32_t gpio_lcd_vbat;
	/* LcdanalogVcc */
	uint32_t lcdanalog_vcc;
	/* LcdVdd */
	uint32_t lcdvdd_vcc;
	/* LcdioVcc */
	uint32_t lcdio_vcc;
	/* LcdBias */
	uint32_t lcd_bias;
	/* LcdVsp */
	uint32_t lcd_vsp;
	/* LcdVsn */
	uint32_t lcd_vsn;
};

struct lcdkit_misc_info
{
	/* orise ic is need reset after iovcc power on */
	uint8_t first_reset;
	uint8_t second_reset;
	uint8_t reset_pull_high_flag;
	uint8_t tprst_before_lcdrst;
	/* incell panel, lcd control tp */
	uint8_t lcd_type;
	/* is default lcd */
	uint8_t tp_color_support;
	uint8_t id_pin_read_support;
	uint8_t lcd_panel_type;
	uint8_t bias_power_ctrl_mode;
	uint8_t iovcc_power_ctrl_mode;
	uint8_t vci_power_ctrl_mode;
	uint8_t vdd_power_ctrl_mode;
	uint8_t vbat_power_ctrl_mode;
	uint8_t bl_power_ctrl_mode;
	uint8_t display_on_in_backlight;
	/* display on new seq and reset time for synaptics only */
	uint8_t panel_display_on_new_seq;
	uint8_t display_effect_on_support;
	uint8_t lcd_brightness_color_uniform_support;
	/* for Hostprocessing TP/LCD OEM infor only */
	uint8_t host_tp_oem_support;
	uint8_t host_panel_oem_pagea_support;
	uint8_t host_panel_oem_pageb_support;
	uint8_t host_panel_oem_backtouser_support;
	uint8_t host_panel_oem_readpart1_support;
	uint8_t host_panel_oem_readpart2_support;
	uint8_t host_panel_oem_readpart1_len;
	uint8_t host_panel_oem_readpart2_len;
	uint8_t reset_shutdown_later;
	uint8_t dis_on_cmds_delay_margin_support;
	uint8_t dis_on_cmds_delay_margin_time;
	uint8_t use_gpio_lcd_bl;
	uint8_t use_gpio_lcd_bl_power;
	uint8_t rgbw_support;
	uint8_t bias_change_lm36274_from_panel_support;
	uint8_t init_lm36923_after_panel_power_on_support;
	uint8_t lcd_otp_support;
	uint8_t lcdph_delay_set_flag;
};

struct lcdkit_delay_ctrl
{
	/* power on delay ctrl */
	uint32_t delay_af_vci_on;
	uint32_t delay_af_iovcc_on;
	uint32_t delay_af_vbat_on;
	uint32_t delay_af_bias_on;
	uint32_t delay_af_vsp_on;
	uint32_t delay_af_vsn_on;
	uint32_t delay_af_vdd_on;
	uint32_t delay_af_LP11;
	uint8_t reset_step1_H;
	uint8_t reset_L;
	uint8_t reset_step2_H;
	/* power off delay ctrl */
	uint32_t delay_af_vsn_off;
	uint32_t delay_af_vsp_off;
	uint32_t delay_af_bias_off;
	uint32_t delay_af_vbat_off;
	uint32_t delay_af_iovcc_off;
	uint32_t delay_af_vci_off;
	uint32_t delay_af_first_iovcc_off;
	uint32_t delay_af_display_on;
	uint32_t delay_af_display_off;
	uint32_t delay_af_display_off_second;
};

struct lcdkit_panel_data
{
	struct lcdkit_panel_info* panel;
	struct lcdkit_panel_ldi* ldi;
	struct lcdkit_panel_mipi* mipi;
};

/* dsi command structure */
struct lcdkit_dsi_panel_cmd
{
	struct dsi_cmd_desc* cmds_set;
	uint32_t cmd_cnt;
};

struct lcdkit_disp_info
{
	/* panel on dsi command */
	struct lcdkit_dsi_panel_cmd display_on_cmds;
	struct lcdkit_dsi_panel_cmd display_on_second_cmds;
	struct lcdkit_dsi_panel_cmd display_on_in_backlight_cmds;
	struct lcdkit_dsi_panel_cmd display_on_effect_cmds;
	/* panel off dsi command */
	struct lcdkit_dsi_panel_cmd display_off_cmds;
	struct lcdkit_dsi_panel_cmd display_off_second_cmds;
	struct lcdkit_platform_config* lcd_platform;
	struct lcdkit_misc_info* lcd_misc;
	struct lcd_bias_ic  lcd_bias_ic_info;
	struct lcd_backlight_ic lcd_backlight_ic_info;
	struct lcdkit_delay_ctrl* lcd_delay;
	struct lcdkit_dsi_panel_cmd tp_color_cmds;
	struct lcdkit_dsi_panel_cmd lcd_oemprotectoffpagea;
	struct lcdkit_dsi_panel_cmd lcd_oemreadfirstpart;
	struct lcdkit_dsi_panel_cmd lcd_oemprotectoffpageb;
	struct lcdkit_dsi_panel_cmd lcd_oemreadsecondpart;
	struct lcdkit_dsi_panel_cmd lcd_oembacktouser;
	struct lcdkit_dsi_panel_cmd id_pin_read_cmds;
	/* color consistency support */
	struct lcdkit_dsi_panel_cmd color_coordinate_enter_cmds;
	struct lcdkit_dsi_panel_cmd color_coordinate_cmds;
	struct lcdkit_dsi_panel_cmd color_coordinate_exit_cmds;
	/* panel info consistency support */
	struct lcdkit_dsi_panel_cmd panel_info_consistency_enter_cmds;
	struct lcdkit_dsi_panel_cmd panel_info_consistency_cmds;
	struct lcdkit_dsi_panel_cmd panel_info_consistency_exit_cmds;
	/* set backlight cmd reg 51h or 61h */
	struct lcdkit_dsi_panel_cmd backlight_cmds;
	/* for sensorhub AOD */
	struct lcdkit_dsi_panel_cmd aod_enter_cmds;
	struct lcdkit_dsi_panel_cmd aod_exit_cmds;
	struct lcdkit_dsi_panel_cmd aod_high_brightness_cmds;
	struct lcdkit_dsi_panel_cmd aod_low_brightness_cmds;
	struct lcdkit_dsi_panel_cmd command2_enter_cmds;
	struct lcdkit_dsi_panel_cmd command2_exit_cmds;
	struct lcdkit_dsi_panel_cmd hbm_enter_cmds;
	struct lcdkit_dsi_panel_cmd hbm_exit_cmds;
	struct lcdkit_dsi_panel_cmd aod_hbm_enter_cmds;
	struct lcdkit_dsi_panel_cmd aod_hbm_exit_cmds;
};
#endif
