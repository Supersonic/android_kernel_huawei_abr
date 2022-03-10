/*
 * lcd_kit_utils.h
 *
 * lcdkit utils function head file for lcd driver
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

#ifndef __LCD_KIT_UTILS_H_
#define __LCD_KIT_UTILS_H_
#include "lcd_kit_common.h"
#include "hisi_mipi_dsi.h"
#include "lcd_kit_adapt.h"
#include "lcd_kit_panel.h"

#define DTS_LCD_PANEL_TYPE  "/huawei,lcd_panel"
#define LCD_KIT_DEFAULT_PANEL   "/huawei,lcd_config/lcd_kit_default_auo_otm1901a_5p2_1080p_video_default"
#define LCD_KIT_DEFAULT_COMPATIBLE   "auo_otm1901a_5p2_1080p_video_default"
#define LCD_DDIC_INFO_LEN 64
#define LCD_1_PIN 1
#define LCD_2_PIN 2
#define DEF_LCD_1_PIN 0x02
#define DEF_LCD_2_PIN 0x0a

/* get the same brightness as in fastboot when enter kernel at the first time */
#define REG61H_VALUE_FOR_RGBW 3800

#define LCD_KIT_ELVSS_DIM_DEFAULT     0x13
#define LCD_KIT_ELVSS_DIM_ENABLE_MASK 0x80
#define VERSION_VALUE_NUM_MAX 10
#define VERSION_NUM_MAX 20
#define VERSION_INFO_LEN_MAX 100

#define LCD_KIT_LOGO_CHECKSUM_SIZE   9
#define PROJECTID_LEN 10
#define FPS_CONFIG_EN 1

#define AOD_VERSION_VALUE_NUM_MAX 4

enum aod_state {
	AOD_NORMAL = 0,
	AOD_ABNORMAL,
};

struct lcd_kit_quickly_sleep_out_desc {
	uint32_t support;
	uint32_t interval;
};

struct lcd_kit_sn_code {
	uint32_t sn_support;
	uint32_t sn_check_support;
	struct lcd_kit_dsi_panel_cmds cmds;
};

struct lcd_kit_tp_color_desc {
	uint32_t support;
	struct lcd_kit_dsi_panel_cmds cmds;
};

struct lcd_kit_snd_disp {
	u32 support;
	struct lcd_kit_dsi_panel_cmds on_cmds;
	struct lcd_kit_dsi_panel_cmds off_cmds;
};

struct lcd_kit_rgbw {
	u32 support;
	struct lcd_kit_dsi_panel_cmds backlight_cmds;
};

struct lcd_kit_aod {
	u32 support;
	u32 support_ext;
	u32 ramless_aod;
	u32 update_core_clk_support;
	u8 aod_need_reset;
	u8 aod_no_need_init;
	u32 aod_y_start;
	u32 aod_y_end;
	u32 aod_y_end_one_bit;
	u32 bl_level;
	u32 aod_lcd_detect_type;
	u32 aod_pmic_ctrl_by_ap;
	u32 aod_exp_ic_reg_vaule;
	u32 aod_ic_reg_vaule_mask;
	u32 sensorhub_ulps;
	u32 max_te_len;
	/* enter aod command */
	struct lcd_kit_dsi_panel_cmds aod_on_cmds;
	struct lcd_kit_dsi_panel_cmds aod_on_cmds_ext;
	/* exit aod command */
	struct lcd_kit_dsi_panel_cmds aod_off_cmds;
	/* aod high brightness command */
	struct lcd_kit_dsi_panel_cmds aod_high_brightness_cmds;
	/* aod low brightness command */
	struct lcd_kit_dsi_panel_cmds aod_low_brightness_cmds;
	struct lcd_kit_dsi_panel_cmds aod_hbm_enter_cmds;
	struct lcd_kit_dsi_panel_cmds aod_hbm_exit_cmds;
	struct lcd_kit_dsi_panel_cmds one_bit_aod_on_cmds;
	struct lcd_kit_dsi_panel_cmds two_bit_aod_on_cmds;
	/* aod brightness command */
	struct lcd_kit_dsi_panel_cmds aod_level1_bl_cmds;
	struct lcd_kit_dsi_panel_cmds aod_level2_bl_cmds;
	struct lcd_kit_dsi_panel_cmds aod_level3_bl_cmds;
	struct lcd_kit_dsi_panel_cmds aod_level4_bl_cmds;
	/* aod pmic config */
	struct lcd_kit_arrays_data aod_pmic_cfg_tbl;
};

struct lcd_kit_hbm {
	u32 support;
	u32 hbm_special_bit_ctrl;
	u32 hbm_wait_te_times;
	struct lcd_kit_dsi_panel_cmds enter_cmds;
	struct lcd_kit_dsi_panel_cmds fp_enter_cmds;
	struct lcd_kit_dsi_panel_cmds hbm_prepare_cmds;
	struct lcd_kit_dsi_panel_cmds hbm_cmds;
	struct lcd_kit_dsi_panel_cmds hbm_post_cmds;
	struct lcd_kit_dsi_panel_cmds exit_cmds;
	struct lcd_kit_dsi_panel_cmds enter_dim_cmds;
	struct lcd_kit_dsi_panel_cmds exit_dim_cmds;
};

struct lcd_kit_hbm_elvss {
	u32 support;
	u32 default_elvss_dim_close;
	u32 set_elvss_lp_support;
	/* elvss dim read cmds */
	struct lcd_kit_dsi_panel_cmds elvss_prepare_cmds;
	struct lcd_kit_dsi_panel_cmds elvss_write_cmds;
	struct lcd_kit_dsi_panel_cmds elvss_read_cmds;
	struct lcd_kit_dsi_panel_cmds elvss_post_cmds;
};

struct lcd_kit_pwm_pulse_switch {
	u8 support;
	struct lcd_kit_dsi_panel_cmds hbm_fp_prepare_cmds;
	struct lcd_kit_dsi_panel_cmds hbm_fp_post_cmds;
	struct lcd_kit_dsi_panel_cmds hbm_fp_prepare_cmds_vn1;
	struct lcd_kit_dsi_panel_cmds hbm_fp_post_cmds_vn1;
};

struct lcd_kit_panel_version {
	u32 support;
	u32 value_number;
	u32 version_number;
	char read_value[VERSION_VALUE_NUM_MAX];
	char lcd_version_name[VERSION_NUM_MAX][LCD_PANEL_VERSION_SIZE];
	struct lcd_kit_arrays_data value;
	struct lcd_kit_dsi_panel_cmds cmds;
	struct lcd_kit_dsi_panel_cmds enter_cmds;
	struct lcd_kit_dsi_panel_cmds exit_cmds;
};

struct aod_cmds_version {
	u32 support;
	u32 value_number;
	u32 version_number;
	char read_value[AOD_VERSION_VALUE_NUM_MAX];
	struct lcd_kit_arrays_data value;
	struct lcd_kit_dsi_panel_cmds enter_cmds;
	struct lcd_kit_dsi_panel_cmds cmds;
};

struct lcd_kit_logo_checksum {
	u32 support;
	struct lcd_kit_dsi_panel_cmds checksum_cmds;
	struct lcd_kit_dsi_panel_cmds enable_cmds;
	struct lcd_kit_dsi_panel_cmds disable_cmds;
	struct lcd_kit_array_data value;
};

struct lcd_kit_project_id {
	unsigned int support;
	char default_project_id[PROJECTID_LEN + 1];
	char id[PROJECTID_LEN + 1];
	struct lcd_kit_dsi_panel_cmds cmds;
};

struct lcd_kit_fps {
	u32 support;
	u32 fps_ifbc_type;
};
struct lcd_kit_otp {
	u32 support;
	u32 otp_value;
	u32 otp_mask;
	u32 osc_value;
	u32 osc_mask;
	u32 tx_delay;
	struct lcd_kit_dsi_panel_cmds read_otp_cmds;
	struct lcd_kit_dsi_panel_cmds read_osc_cmds;
	struct lcd_kit_dsi_panel_cmds otp_flow_cmds;
	struct lcd_kit_dsi_panel_cmds otp_end_cmds;
};
/* function declare */
int lcd_kit_get_tp_color(struct hisi_fb_data_type *hisifd);
int lcd_kit_get_elvss_info(struct hisi_fb_data_type *hisifd);
int lcd_kit_panel_version_init(struct hisi_fb_data_type *hisifd);
int lcd_kit_aod_extend_cmds_init(struct hisi_fb_data_type *hisifd);
int lcd_panel_version_compare(struct hisi_fb_data_type *hisifd);
int lcd_kit_adapt_init(void);
uint32_t lcd_kit_get_backlight_type(struct hisi_panel_info *pinfo);
int lcd_kit_utils_init(struct hisi_panel_info *pinfo);
int lcd_kit_dsi_fifo_is_full(uint32_t dsi_base);
void lcd_kit_read_power_status(struct hisi_fb_data_type *hisifd);
int lcd_kit_pwm_set_backlight(struct hisi_fb_data_type *hisifd,
	uint32_t bl_level);
uint32_t lcd_kit_blpwm_set_backlight(struct hisi_fb_data_type *hisifd,
	uint32_t bl_level);
int lcd_kit_sh_blpwm_set_backlight(struct hisi_fb_data_type *hisifd,
	uint32_t bl_level);
int lcd_kit_set_mipi_backlight(struct hisi_fb_data_type *hisifd,
	uint32_t bl_level);
char *lcd_kit_get_compatible(uint32_t product_id, uint32_t lcd_id,
	int pin_num);
char *lcd_kit_get_lcd_name(uint32_t product_id, uint32_t lcd_id,
	int pin_num);
uint32_t lcd_kit_get_dsi_base_addr(struct hisi_fb_data_type *hisifd);
int lcd_kit_dsi_cmds_tx(void *hld, struct lcd_kit_dsi_panel_cmds *cmds);
int lcd_kit_dsi_cmds_rx(void *hld, uint8_t *out, int out_len,
	struct lcd_kit_dsi_panel_cmds *cmds);
void lcd_kit_set_mipi_link(struct hisi_fb_data_type *hisifd, int link_state);
void lcd_kit_set_mipi_link(struct hisi_fb_data_type *hisifd, int link_state);
int lcd_kit_change_dts_value(char *compatible, char *dts_name, u32 value);
void lcd_logo_checksum_set(struct hisi_fb_data_type *hisifd);
void lcd_logo_checksum_check(struct hisi_fb_data_type *hisifd);
int lcd_kit_read_project_id(struct hisi_fb_data_type *hisifd);
int lcd_kit_parse_sn_check(struct hisi_fb_data_type *hisifd, const char *np);
int lcd_add_status_disabled(void);
void lcd_kit_check_otp(struct hisi_fb_data_type *hisifd);
#endif
