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
#include "lcd_kit_adapt.h"
#include "lcd_kit_panel.h"

#define DTS_LCD_PANEL_TYPE  "/huawei,lcd_panel"
#define LCD_KIT_DEFAULT_PANEL	"/huawei,lcd_config/lcd_kit_default_auo_otm1901a_5p2_1080p_video_default"
#define LCD_KIT_DEFAULT_COMPATIBLE	"auo_otm1901a_5p2_1080p_video_default"
#define LCD_DDIC_INFO_LEN	64

#define REG61H_VALUE_FOR_RGBW	3800

/* dcs read/write */
#define DTYPE_DCS_WRITE		0x05 /* short write, 0 parameter */
#define DTYPE_DCS_WRITE1	0x15 /* short write, 1 parameter */
#define DTYPE_DCS_READ		0x06 /* read */
#define DTYPE_DCS_LWRITE	0x39 /* long write */
#define DTYPE_DSC_LWRITE	0x0A /* dsc dsi1.2 vesa3x long write */

/* generic read/write */
#define DTYPE_GEN_WRITE		0x03 /* short write, 0 parameter */
#define DTYPE_GEN_WRITE1	0x13 /* short write, 1 parameter */
#define DTYPE_GEN_WRITE2	0x23 /* short write, 2 parameter */
#define DTYPE_GEN_LWRITE	0x29 /* long write */
#define DTYPE_GEN_READ		0x04 /* long read, 0 parameter */
#define DTYPE_GEN_READ1		0x14 /* long read, 1 parameter */
#define DTYPE_GEN_READ2		0x24 /* long read, 2 parameter */

#define BL_MIN	0
#define BL_MAX	256
#define BL_NIT	400
#define BL_REG_NOUSE_VALUE	128
#define BL_BOOT_DEF 255

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array)	((sizeof(array)) / (sizeof(array[0])))
#endif

#ifndef errno_t
typedef int errno_t;
#endif

/* get blmaxnit */
enum {
	GET_BLMAXNIT_FROM_DDIC = 1,
};

enum {
	WAIT_TYPE_US = 0,
	WAIT_TYPE_MS,
};

/* pinctrl data */
struct pinctrl_data {
	struct pinctrl *p;
	struct pinctrl_state *pinctrl_def;
	struct pinctrl_state *pinctrl_idle;
};
struct pinctrl_cmd_desc {
	int dtype;
	struct pinctrl_data *pctrl_data;
	int mode;
};

/* dtype for gpio */
enum {
	DTYPE_GPIO_REQUEST,
	DTYPE_GPIO_FREE,
	DTYPE_GPIO_INPUT,
	DTYPE_GPIO_OUTPUT,
};
/* gpio desc */
struct gpio_desc {
	int dtype;
	int waittype;
	int wait;
	char *label;
	uint32_t *gpio;
	int value;
};

/* dsc parmas */
struct dsc_params {
	unsigned int ver;
	unsigned int slice_mode;
	unsigned int rgb_swap;
	unsigned int dsc_cfg;
	unsigned int rct_on;
	unsigned int bit_per_channel;
	unsigned int dsc_line_buf_depth;
	unsigned int bp_enable;
	unsigned int bit_per_pixel;
	unsigned int pic_height;
	unsigned int pic_width;
	unsigned int slice_height;
	unsigned int slice_width;
	unsigned int chunk_size;
	unsigned int xmit_delay;
	unsigned int dec_delay;
	unsigned int scale_value;
	unsigned int increment_interval;
	unsigned int decrement_interval;
	unsigned int line_bpg_offset;
	unsigned int nfl_bpg_offset;
	unsigned int slice_bpg_offset;
	unsigned int initial_offset;
	unsigned int final_offset;
	unsigned int flatness_minqp;
	unsigned int flatness_maxqp;
	unsigned int rc_model_size;
	unsigned int rc_edge_factor;
	unsigned int rc_quant_incr_limit0;
	unsigned int rc_quant_incr_limit1;
	unsigned int rc_tgt_offset_hi;
	unsigned int rc_tgt_offset_lo;
};

struct ldi_panel_info {
	u32 h_back_porch;
	u32 h_front_porch;
	u32 h_pulse_width;

	/*
	 * note: vbp > 8 if used overlay compose,
	 * also lcd vbp > 8 in lcd power on sequence
	 */
	u32 v_back_porch;
	u32 v_front_porch;
	u32 v_pulse_width;


	u8 hsync_plr;
	u8 vsync_plr;
	u8 pixelclk_plr;
	u8 data_en_plr;

	/* for cabc */
	u8 dpi0_overlap_size;
	u8 dpi1_overlap_size;
	u32 v_front_porch_forlp;
};

#ifdef MTK_ROUND_CORNER_SUPPORT
struct panel_round_corner {
	unsigned int round_corner_en;
	unsigned int full_content;
	unsigned int w;
	unsigned int h;
	unsigned int w_bot;
	unsigned int h_bot;
	unsigned int tp_size;
	unsigned int bt_size;
	void *lt_addr;
	void *rt_addr;
	void *lb_addr;
	void *rb_addr;
};
#endif

/* get blmaxnit */
struct lcd_kit_blmaxnit {
	u32 get_blmaxnit_type;
	u32 lcd_kit_brightness_ddic_info;
	struct lcd_kit_dsi_panel_cmds bl_maxnit_cmds;
};

struct mipi_panel_info {
	u32 dsi_version;
	u32 vc;
	u32 lane_nums;
	u32 lane_nums_select_support;
	u32 color_mode;
	u32 dsi_bit_clk; /* clock lane(p/n) */
	u32 burst_mode;
	u32 max_tx_esc_clk;
	u32 non_continue_en;

	u32 dsi_bit_clk_val1;
	u32 dsi_bit_clk_val2;
	u32 dsi_bit_clk_val3;
	u32 dsi_bit_clk_val4;
	u32 dsi_bit_clk_val5;
	u32 dsi_bit_clk_upt;

	u32 hs_wr_to_time;

	/* dphy config parameter adjust */
	u32 clk_post_adjust;
	u32 clk_pre_adjust;
	u32 clk_pre_delay_adjust;
	u32 clk_t_hs_exit_adjust;
	u32 clk_t_hs_trial_adjust;
	u32 clk_t_hs_prepare_adjust;
	int clk_t_lpx_adjust;
	u32 clk_t_hs_zero_adjust;
	u32 data_post_delay_adjust;
	int data_t_lpx_adjust;
	u32 data_t_hs_prepare_adjust;
	u32 data_t_hs_zero_adjust;
	u32 data_t_hs_trial_adjust;
	u32 rg_vrefsel_vcm_adjust;

	/* only for 3660 use */
	u32 rg_vrefsel_vcm_clk_adjust;
	u32 rg_vrefsel_vcm_data_adjust;

	u32 phy_mode;  /* 0: DPHY, 1:CPHY */
	u32 lp11_flag; /* 0: nomal_lp11, 1:short_lp11, 2:disable_lp11 */
	u32 phy_m_n_count_update;  /* 0:old ,1:new can get 988.8M */
	u32 eotp_disable_flag; /* 0: eotp enable, 1:eotp disable */
};

struct mtk_panel_info {
	u32 panel_state;
	u32 panel_lcm_type;
	u32 panel_dsi_mode;
	u32 panel_dsi_switch_mode;
	u32 panel_trans_seq;
	u32 panel_data_padding;
	u32 panel_packtet_size;
	u32 panel_ps;

	u32 output_mode; /* MTK_PANEL_OUTPUT_MODE */
	u32 type;
	u32 xres;
	u32 yres;
	u32 width; /* mm */
	u32 height;
	u32 bpp;
	u32 fps;
	u32 fps_updt;
	u32 orientation;
	u32 bgr_fmt;
	u32 bl_set_type;
	u32 bl_min;
	u32 bl_max;
	/* default max nit */
	u32 bl_max_nit;
	/* actual max nit */
	u32 actual_bl_max_nit;
	u32 bl_boot;
	u32 bl_v200;
	u32 bl_otm;
	u32 bl_default;
	u32 blpwm_precision_type;
	u32 blpwm_preci_no_convert;
	u32 blpwm_out_div_value;
	u32 blpwm_input_ena;
	u32 blpwm_input_disable;
	u32 blpwm_in_num;
	u32 blpwm_input_precision;
	u32 bl_ic_ctrl_mode;
	u32 bias_ic_ctrl_mode;
	u64 pxl_clk_rate;
	u64 pxl_clk_rate_adjust;
	u32 pxl_clk_rate_div;
	u32 data_rate;
	u32 pxl_fbk_div;
	u32 vsync_ctrl_type;
	u32 fake_external;
	u32 mipi_read_gcs_support;
	u32 tp_color;
	u8  reserved[3];

	u32 ifbc_type;
	u32 ifbc_cmp_dat_rev0;
	u32 ifbc_cmp_dat_rev1;
	u32 ifbc_auto_sel;
	u32 ifbc_orise_ctl;
	u32 ifbc_orise_ctr;
	u32 ssc_disable;
	u32 ssc_range;
	u8 sbl_support;
	u8 sre_support;
	u8 color_temperature_support;
	u8 color_temp_rectify_support;
	u32 color_temp_rectify_R;
	u32 color_temp_rectify_G;
	u32 color_temp_rectify_B;
	u8 comform_mode_support;
	u8 cinema_mode_support;
	u8 frc_enable;
	u8 esd_enable;
	u8 esd_skip_mipi_check;
	u8 esd_recover_step;
	u8 esd_expect_value_type;
	u8 dirty_region_updt_support;
	u8 snd_cmd_before_frame_support;
	u8 dsi_bit_clk_upt_support;
	u8 mipiclk_updt_support_new;
	u8 fps_updt_support;
	u8 fps_updt_panel_only;
	u8 fps_updt_force_update;
	u8 fps_scence;

	u8 panel_effect_support;

	u8 gmp_support;
	u8 colormode_support;
	u8 gamma_support;
	u8 gamma_type; /* normal, cinema */
	u8 xcc_support;
	u8 acm_support;
	u8 acm_ce_support;
	u8 rgbw_support;
	u8 hbm_support;

	u8 hiace_support;
	u8 dither_support;
	u8 arsr1p_sharpness_support;
	struct ldi_panel_info ldi;
	struct mipi_panel_info mipi;
	/* get_blmaxnit */
	struct lcd_kit_blmaxnit blmaxnit;

	unsigned int dsc_enable;
	struct dsc_params dsc_params;
	u8 bias_bl_ic_checked;
	u32 bl_max_nit_min_value;
#ifdef MTK_ROUND_CORNER_SUPPORT
	struct panel_round_corner round_corner;
#endif
};
struct lcd_kit_quickly_sleep_out_desc {
	uint32_t support;
	uint32_t interval;
	uint32_t panel_on_tag;
	unsigned long panel_on_record;
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

enum bl_control_mode {
	MTK_AP_MODE = 0,
	I2C_ONLY_MODE = 1,
	PWM_ONLY_MODE,
	MTK_PWM_HIGH_I2C_MODE,
	MUTI_THEN_RAMP_MODE,
	RAMP_THEN_MUTI_MODE,
	MTK_AAL_I2C_MODE,
	MTK_MIPI_MODE,
	MTK_MIPI_BL_IC_PWM_MODE,
	PWM_I2C_MODE,
};

enum bias_control_mode {
	MT_AP_MODE = 0,
	PMIC_ONLY_MODE = 1,
	GPIO_ONLY_MODE,
	GPIO_THEN_I2C_MODE,
	LCD_BIAS_COMMON_MODE,
};
int lcd_kit_adapt_init(void);
int lcd_kit_dsi_fifo_is_full(uint32_t dsi_base);
char *lcd_kit_get_compatible(uint32_t product_id, uint32_t lcd_id);
char *lcd_kit_get_lcd_name(uint32_t product_id, uint32_t lcd_id);
int lcd_kit_dsi_cmds_tx(void *hld, struct lcd_kit_dsi_panel_cmds *cmds);
int lcd_kit_dsi_cmds_rx(void *hld, uint8_t *out, int out_len,
	struct lcd_kit_dsi_panel_cmds *cmds);
u32 lcd_kit_get_blmaxnit(struct mtk_panel_info *pinfo);
struct mtk_panel_info *lcm_get_panel_info(void);
int lcd_kit_utils_init(struct mtk_panel_info *pinfo, const void *fdt, int nodeoffset);
void lcd_kit_disp_on_check_delay(void);
void lcd_kit_disp_on_record_time(void);
int lcd_kit_get_tp_color(void);
int lcd_kit_pmu_ctrl(uint32_t type, uint32_t enable);
int lcd_kit_charger_ctrl(uint32_t type, uint32_t enable);
extern errno_t memcpy_s(void *dest, size_t destMax, const void *src, size_t count);
extern int mt6360_pmic_config_interface(u8 addr, u8 data, u8 mask, u8 shift);
extern int mt6360_ldo_config_interface(u8 addr, u8 data, u8 mask, u8 shift);
extern void *malloc(size_t size);
extern void free(void *ptr);
extern void mdelay (unsigned long msec);
extern void udelay (unsigned long usec);
extern void *get_lk_overlayed_dtb(void);
#endif
