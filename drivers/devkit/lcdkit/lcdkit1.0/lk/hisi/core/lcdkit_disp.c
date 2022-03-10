/*
 * lcdkit_disp.c
 *
 * lcdkit disp function for lcd driver
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

#include <oeminfo_ops.h>
#include <boardid.h>
#include "hisi_fb.h"
#include "lcd_bl.h"
#include "lcdkit_disp.h"
#include "lcdkit_display_effect.h"
#include "lcdkit_panels.h"
#include "tps65132.h"

#define DTS_COMP_LCDKIT_PANEL_TYPE	 "huawei,lcd_panel_type"
#define DTS_LCD_PANEL_TYPE 	"/huawei,lcd_panel"
#define REG61H_VALUE_FOR_RGBW 3800 // get the same brightness as in fastboot when enter kernel at the first time
#define ENABLE_COMMON_IC 1

static uint32_t gpio_lcdkit_reset;
static uint32_t gpio_lcdkit_te0;
static uint32_t gpio_lcdkit_iovcc;
static uint32_t gpio_lcdkit_vci;
static uint32_t gpio_lcdkit_vsp;
static uint32_t gpio_lcdkit_vsn;
static uint32_t gpio_lcdkit_bl;
static uint32_t gpio_lcdkit_bl_power;
static uint32_t gpio_lcdkit_tp_reset;
static uint32_t gpio_lcdkit_vbat;

/* panel info */
static struct hisi_panel_info lcd_info = {0};
/*
 * tp reset gpio
 */
static struct gpio_desc lcdkit_gpio_tp_reset_normal_cmds[] = {
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1,
		GPIO_LCDKIT_TP_RESET_NAME, &gpio_lcdkit_tp_reset, 1
	},
};
static struct gpio_desc lcdkit_pinctrl_tp_normal_cmds[] = {
	{
		DTYPE_GPIO_PMX, WAIT_TYPE_MS, 1,
		GPIO_LCDKIT_TP_RESET_NAME, &gpio_lcdkit_tp_reset, FUNCTION0
	},
};
/*
 * reset gpio
 */
static struct gpio_desc lcdkit_gpio_reset_request_cmds[] = {
	/* reset */
	{
		DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_RESET_NAME, &gpio_lcdkit_reset, 0
	},
};
static struct gpio_desc lcdkit_gpio_reset_free_cmds[] = {
	/* reset */
	{
		DTYPE_GPIO_FREE, WAIT_TYPE_US, 100,
		GPIO_LCDKIT_RESET_NAME, &gpio_lcdkit_reset, 0
	},
};
static struct gpio_desc lcdkit_gpio_reset_normal_cmds[] = {
	/* reset */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCDKIT_RESET_NAME, &gpio_lcdkit_reset, 1
	},
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_RESET_NAME, &gpio_lcdkit_reset, 0
	},
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCDKIT_RESET_NAME, &gpio_lcdkit_reset, 1
	},
};
static struct gpio_desc lcdkit_gpio_reset_high_cmds[] = {
	/* reset */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCDKIT_RESET_NAME, &gpio_lcdkit_reset, 1
	},
};
static struct gpio_desc lcdkit_gpio_reset_low_cmds[] = {
	/* reset */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_RESET_NAME, &gpio_lcdkit_reset, 0
	},
};
/*
 * vcc and bl gpio
 */
static struct gpio_desc lcdkit_bias_request_cmds[] = {
	/* AVDD +5.5V */
	{
		DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_P5V5_ENABLE_NAME, &gpio_lcdkit_vsp, 0
	},
	/* AVEE_-5.5V */
	{
		DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_N5V5_ENABLE_NAME, &gpio_lcdkit_vsn, 0
	},
};
static struct gpio_desc lcdkit_bias_enable_cmds[] = {
	/* AVDD_5.5V */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_P5V5_ENABLE_NAME, &gpio_lcdkit_vsp, 1
	},
	/* AVEE_-5.5V */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 20,
		GPIO_LCDKIT_N5V5_ENABLE_NAME, &gpio_lcdkit_vsn, 1
	},
};
static struct gpio_desc lcdkit_bl_enable_cmds[] = {
	/* backlight enable */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_BL_ENABLE_NAME, &gpio_lcdkit_bl, 1
	},
};
static struct gpio_desc lcdkit_bl_power_enable_cmds[] = {
	/* backlight power enable */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_BL_POWER_ENABLE_NAME, &gpio_lcdkit_bl_power, 1
	}
};
static struct gpio_desc lcdkit_bias_free_cmds[] = {
	/* AVEE_-5.5V */
	{
		DTYPE_GPIO_FREE, WAIT_TYPE_US, 50,
		GPIO_LCDKIT_N5V5_ENABLE_NAME, &gpio_lcdkit_vsn, 0
	},
	/* AVDD_5.5V */
	{
		DTYPE_GPIO_FREE, WAIT_TYPE_US, 50,
		GPIO_LCDKIT_P5V5_ENABLE_NAME, &gpio_lcdkit_vsp, 0
	},
};
static struct gpio_desc lcdkit_bias_disable_cmds[] = {
	/* AVEE_-5.5V */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 20,
		GPIO_LCDKIT_N5V5_ENABLE_NAME, &gpio_lcdkit_vsn, 0
	},
	/* AVDD_5.5V */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_P5V5_ENABLE_NAME, &gpio_lcdkit_vsp, 0
	},
	/* AVEE_-5.5V input */
	{
		DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_N5V5_ENABLE_NAME, &gpio_lcdkit_vsn, 0
	},
	/* AVDD_5.5V input */
	{
		DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_P5V5_ENABLE_NAME, &gpio_lcdkit_vsp, 0
	},
};
static struct gpio_desc lcdkit_bl_repuest_cmds[] = {
	/* BL request */
	{
		DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_BL_ENABLE_NAME, &gpio_lcdkit_bl, 0
	},
};
static struct gpio_desc lcdkit_bl_power_request_cmds[] = {
	/* BL power request */
	{
		DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_BL_POWER_ENABLE_NAME, &gpio_lcdkit_bl_power, 0
	}
};
static struct gpio_desc lcdkit_bl_disable_cmds[] = {
	/* backlight disable */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_BL_ENABLE_NAME, &gpio_lcdkit_bl, 0
	},
	{
		DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
		GPIO_LCDKIT_BL_ENABLE_NAME, &gpio_lcdkit_bl, 0
	},
};
static struct gpio_desc lcdkit_bl_power_disable_cmds[] = {
	/* backlight power disable */
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_BL_POWER_ENABLE_NAME, &gpio_lcdkit_bl_power, 0
	},
	{
		DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
		GPIO_LCDKIT_BL_POWER_ENABLE_NAME, &gpio_lcdkit_bl_power, 0
	}
};
static struct gpio_desc lcdkit_bl_free_cmds[] = {
	/* BL free */
	{
		DTYPE_GPIO_FREE, WAIT_TYPE_US, 50,
		GPIO_LCDKIT_BL_ENABLE_NAME, &gpio_lcdkit_bl, 0
	},
};
static struct gpio_desc lcdkit_bl_power_free_cmds[] = {
	/* BL free */
	{
		DTYPE_GPIO_FREE, WAIT_TYPE_US, 50,
		GPIO_LCDKIT_BL_POWER_ENABLE_NAME, &gpio_lcdkit_bl_power, 0
	}
};
/*
 * Iovcc and vci gpio
 */
static struct gpio_desc lcdkit_iovcc_request_cmds[] = {
	{
		DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_IOVCC_NAME, &gpio_lcdkit_iovcc, 0
	},
};
static struct gpio_desc lcdkit_vbat_request_cmds[] = {
	{
		DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_VBAT_NAME, &gpio_lcdkit_vbat, 0
	},
};
static struct gpio_desc lcdkit_vci_request_cmds[] = {
	{
		DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_VCI_NAME, &gpio_lcdkit_vci, 0
	},
};
static struct gpio_desc lcdkit_iovcc_enable_cmds[] = {
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_IOVCC_NAME, &gpio_lcdkit_iovcc, 1
	},
};
static struct gpio_desc lcdkit_vbat_enable_cmds[] = {
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_VBAT_NAME, &gpio_lcdkit_vbat, 1
	},
};
static struct gpio_desc lcdkit_vci_enable_cmds[] = {
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_VCI_NAME, &gpio_lcdkit_vci, 1
	},
};
static struct gpio_desc lcdkit_iovcc_free_cmds[] = {
	{
		DTYPE_GPIO_FREE, WAIT_TYPE_US, 50,
		GPIO_LCDKIT_IOVCC_NAME, &gpio_lcdkit_iovcc, 0
	},
};
static struct gpio_desc lcdkit_vbat_free_cmds[] = {
	{
		DTYPE_GPIO_FREE, WAIT_TYPE_US, 50,
		GPIO_LCDKIT_VBAT_NAME, &gpio_lcdkit_vbat, 0
	},
};
static struct gpio_desc lcdkit_iovcc_disable_cmds[] = {
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 20,
		GPIO_LCDKIT_IOVCC_NAME, &gpio_lcdkit_iovcc, 0
	},

	{
		DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_IOVCC_NAME, &gpio_lcdkit_iovcc, 0
	},
};
static struct gpio_desc lcdkit_vbat_disable_cmds[] = {
	{
		DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 20,
		GPIO_LCDKIT_VBAT_NAME, &gpio_lcdkit_vbat, 0
	},

	{
		DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 5,
		GPIO_LCDKIT_VBAT_NAME, &gpio_lcdkit_vbat, 0
	},
};
/*
 * LCD IOMUX
 */
static struct gpio_desc lcdkit_pinctrl_normal_cmds[] = {
	/* reset */
	{
		DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_RESET_NAME, &gpio_lcdkit_reset, FUNCTION0
	},
	/* te0 */
	{
		DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_TE0_NAME, &gpio_lcdkit_te0, FUNCTION2
	},
};
static struct gpio_desc lcdkit_bias_pinctrl_normal_cmds[] = {
	/* vsp */
	{
		DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_P5V5_ENABLE_NAME, &gpio_lcdkit_vsp, FUNCTION0
	},
	/* vsn */
	{
		DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_N5V5_ENABLE_NAME, &gpio_lcdkit_vsn, FUNCTION0
	},
	/* bl */
	{
		DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_BL_ENABLE_NAME, &gpio_lcdkit_bl, FUNCTION0
	},
};
static struct gpio_desc lcdkit_pinctrl_low_cmds[] = {
	/* reset */
	{
		DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_RESET_NAME, &gpio_lcdkit_reset, FUNCTION0
	},
	/* te0 */
	{
		DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_TE0_NAME, &gpio_lcdkit_te0, FUNCTION0
	},
	/* te0 input */
	{
		DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_TE0_NAME, &gpio_lcdkit_te0, 0
	},

};
static struct gpio_desc lcdkit_bias_pinctrl_low_cmds[] = {
	/* vsp input */
	{
		DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_P5V5_ENABLE_NAME, &gpio_lcdkit_vsp, 0
	},
	/* vsn input */
	{
		DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 0,
		GPIO_LCDKIT_N5V5_ENABLE_NAME, &gpio_lcdkit_vsn, 0
	},
};
static void update_lcdkit_data(uint32_t lcdkit_id);
extern uint32_t get_ldo_chanel(void);
extern uint32_t get_vci_ldo_chanel(void);
static char* tp_reset_pull_h[] = {
"jdi_ili7807e_5p2_1080p_video",
"ebbg_nt35596s_5p2_1080p_video",
"jdi_nt35696h_5p1_1080P_cmd",
"sharp_td4336_5p82_1080plus_cmd",
"jdi_td4336_5p82_1080plus_cmd",
"sharp_td4336_hma_6p53_1080plus_cmd",
"jdi_td4336_hma_6p53_1080plus_cmd",
"sharp_td4336_hma_v2_6p53_1080plus_cmd",
"jdi_td4336_hma_v2_6p53_1080plus_cmd",
"boe_td4320_6p3_1080p_video",
"boe_ft8201_10p1_1200p_video",
"inx_ft8201_10p1_1200p_video",
"tm_td4320_6p3_1080p_video",
"boe_td4320_6p5_1080p_video",
"auo_td4320_6p4_1080p_video",
"boe_td4320_6p4_1080p_video",
"lg_td4320_6p4_1080p_video",
"ofilm_td4320_6p5_1080p_video",
"boe_td4320_6p21_1080p_video",
"tm_td4320_6p21_1080p_video",
"mutto_td4320_6p21_1080p_video",
"tm_td4320_6p5_1080p_video",
"tm_td4330_6p3_1080p_video",
"tm_td4320_6p15_1080p_video",
"boe_td4320_6p15_1080p_video",
"boe_td4330_6p21_1080p_cmd",
"tm_td4330_6p21_1080p_cmd",
"jdi_td4320_6p15_1080p_video",
"tm_td4330_6p15_1080p_video",
"boe_td4330_6p5_1080p_video",
"ofilm_td4330_6p5_1080p_cmd",
"boe_td4320_6p59_1080p_video",
"tm_td4320_6p59_1080p_video",
"tm_td4320_6p4_1080p_video",
"boe_ft8615_6p39_720p_video",
"boe_ft8009_6p39_720p_video",
"tm_ft8615_6p39_720p_video",
"tm_td4321_6p59_1080p_video",
"inx_hx83102h_10p1",
"boe_hx83102h_10p1",
"inx_hx83102e_9p7",
"110_209_6p67",
"110_603_6p67",
"220_a01_6p67",
"250_603_6p67",
"190_503_10p1"
};
uint32_t g_panel_vender_id = 0xFFFF;

static uint32_t g_iovcc_on_time = 0;

bool lcdkit_is_tp_reset_pull_h(const char *name)
{
	int i;

	for (i = 0; i < (int)(sizeof(tp_reset_pull_h)/sizeof(tp_reset_pull_h[0])); i++) {
		if (!strncmp(name, tp_reset_pull_h[i],strlen(name))) {
			return true;
		}
	}
	return false;
}

struct lcdkit_panel_data lcdkit_data;
/* panel info */
struct lcdkit_disp_info lcdkit_infos;

void lcdkit_power_on_bias_enable(void)
{
	struct lcd_bias_voltage_info *pbias = NULL;
	lcdkit_dts_set_ic_name("lcd-bias-ic-name","default");
	lcdkit_bias_ic_init(lcdkit_infos.lcd_bias_ic_info.lcd_bias_ic_list, lcdkit_infos.lcd_bias_ic_info.num_of_lcd_bias_ic_list);
	pbias = get_lcd_bias_ic_info();
	if(pbias != NULL)
		lcdkit_dts_set_ic_name("lcd-bias-ic-name",pbias->name);
}
void lcdkit_dts_set_ic_name(const char *prop_name, const char *name)
{
	struct fdt_operators *fdt_ops = NULL;
	struct dtb_operators *dtimage_ops = NULL;
	void *fdt = NULL;
	int ret;
	int offset;
	if((NULL == prop_name)||(NULL == name))
	{
		return;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if(!fdt_ops)
	{
		HISI_FB_ERR("can not get fdt_ops!\n");
		return;
	}
	dtimage_ops = get_operators(DTIMAGE_MODULE_NAME_STR);
	if (NULL == dtimage_ops)
	{
		HISI_FB_ERR("failed to get dtimage operators!\n");
		return;
	}
	fdt_ops->fdt_operate_lock();
	fdt = dtimage_ops->get_dtb_addr();
	if (NULL == fdt)
	{
		HISI_FB_ERR("failed to get fdt addr!\n");
		fdt_ops->fdt_operate_unlock();
		return;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0)
	{
		HISI_FB_ERR("fdt_open_into failed!\n");
		fdt_ops->fdt_operate_unlock();
		return;
	}
	ret = fdt_ops->fdt_path_offset(fdt, DTS_LCD_PANEL_TYPE);
	if (ret < 0)
	{
		HISI_FB_ERR("Could not find panel node, change fb dts failed\n");
		fdt_ops->fdt_operate_unlock();
		return;
	}

	offset = ret;

	ret = fdt_ops->fdt_setprop_string(fdt, offset, (const char*)prop_name, (const void*)name);
	if (ret)
		HISI_FB_ERR("Cannot update lcd panel type(errno=%d)!\n", ret);
	fdt_ops->fdt_operate_unlock();
}
/*
 * name:hw_lcd_info_init
 * function:lcd info init
 * @hisifd:hisi fb data
 * @pinfo:panel info
 */
static void lcdkit_info_init(struct hisi_fb_data_type* hisifd, struct hisi_panel_info* pinfo)
{
	int ret;
	int bl_common_ic = 0;
	char lcd_bl_ic_name_buf[LCD_BL_IC_NAME_MAX]= "LM36923YFFR";

	HISI_FB_INFO("  enter ++ ");
	/* panel info */
	pinfo->xres = lcdkit_data.panel->xres;
	pinfo->yres = lcdkit_data.panel->yres;
	pinfo->width = lcdkit_data.panel->width;
	pinfo->height = lcdkit_data.panel->height;
	pinfo->orientation = lcdkit_data.panel->orientation;
	pinfo->bpp = lcdkit_data.panel->bpp;
	pinfo->bgr_fmt = lcdkit_data.panel->bgr_fmt;
	pinfo->bl_set_type = lcdkit_data.panel->bl_set_type;
	pinfo->bl_min = lcdkit_data.panel->bl_min;
	pinfo->bl_max = lcdkit_data.panel->bl_max;
	pinfo->bl_def = lcdkit_data.panel->bl_def;
	pinfo->bl_min = lcdkit_data.panel->blpwm_intr_value;
	pinfo->bl_max = lcdkit_data.panel->blpwm_max_value;
	pinfo->bl_v200 = lcdkit_data.panel->bl_v200;
	pinfo->bl_otm = lcdkit_data.panel->bl_otm;
	pinfo->type = lcdkit_data.panel->type;
	pinfo->pxl_clk_to_pll0 = lcdkit_data.panel->pxl_clk_to_pll0;
	pinfo->ifbc_type = lcdkit_data.panel->ifbc_type;
	pinfo->backlight_byte_order = lcdkit_data.panel->bl_byte_order;
	pinfo->lcd_type = LCDKIT;
	pinfo->lcd_name = lcdkit_data.panel->lcd_name;
	pinfo->frc_enable = lcdkit_data.panel->frc_enable;
	pinfo->esd_enable = lcdkit_data.panel->esd_enable;
	pinfo->prefix_ce_support = lcdkit_data.panel->prefix_ce_support;
	pinfo->prefix_sharpness_support = lcdkit_data.panel->prefix_sharpness_support;
	pinfo->lcd_uninit_step_support = lcdkit_data.panel->lcd_uninit_step_support;
	pinfo->dpi01_exchange_flag = lcdkit_data.panel->dpi01_set_change;
	pinfo->blpwm_precision = lcdkit_data.panel->blpwm_precision;
	pinfo->blpwm_div = lcdkit_data.panel->blpwm_div;

	/* aod */
	pinfo->aod_support = lcdkit_data.panel->sh_aod_support;
	/* 16 means high 16bits of 32bit */
	pinfo->aod_disp_area_y = ((lcdkit_data.panel->sh_aod_start_y & 0xffff) << 16) |
		(lcdkit_data.panel->sh_aod_end_y & 0xffff);
	pinfo->aod_disp_area_y_one_bit = ((lcdkit_data.panel->sh_aod_start_y & 0xffff) << 16) |
		(lcdkit_data.panel->sh_aod_end_y_one_bit & 0xffff);
	pinfo->aod_need_reset = lcdkit_data.panel->aod_need_reset;
	pinfo->aod_no_need_init = lcdkit_data.panel->aod_no_need_init;
	pinfo->txoff_rxulps_en = lcdkit_data.panel->txoff_rxulps_en;
	pinfo->gpio_request_cmds = lcdkit_gpio_reset_request_cmds;
	pinfo->gpio_request_cmds_len = ARRAY_SIZE(lcdkit_gpio_reset_request_cmds);

	pinfo->pinctrl_normal_cmds_len = ARRAY_SIZE(lcdkit_pinctrl_normal_cmds);
	pinfo->pinctrl_normal_cmds = lcdkit_pinctrl_normal_cmds;
	pinfo->pinctrl_lowpower_cmds_len = ARRAY_SIZE(lcdkit_pinctrl_low_cmds);
	pinfo->pinctrl_lowpower_cmds = lcdkit_pinctrl_low_cmds;

	pinfo->gpio_normal_cmds1_len = ARRAY_SIZE(lcdkit_gpio_reset_normal_cmds);
	pinfo->gpio_normal_cmds1 = lcdkit_gpio_reset_normal_cmds;
	pinfo->gpio_lowpower_cmds1_len = ARRAY_SIZE(lcdkit_gpio_reset_low_cmds);
	pinfo->gpio_lowpower_cmds1 = lcdkit_gpio_reset_low_cmds;
	pinfo->gpio_lowpower_cmds2_len = ARRAY_SIZE(lcdkit_gpio_reset_free_cmds);
	pinfo->gpio_lowpower_cmds2 = lcdkit_gpio_reset_free_cmds;

	pinfo->disp_on_cmds_len = lcdkit_infos.display_on_cmds.cmd_cnt;
	pinfo->disp_on_cmds = lcdkit_infos.display_on_cmds.cmds_set;
	pinfo->disp_off_cmds_len = lcdkit_infos.display_off_cmds.cmd_cnt;
	pinfo->disp_off_cmds = lcdkit_infos.display_off_cmds.cmds_set;

	pinfo->aod_enter_cmds_len = lcdkit_infos.aod_enter_cmds.cmd_cnt;
	pinfo->aod_enter_cmds = lcdkit_infos.aod_enter_cmds.cmds_set;
	pinfo->aod_exit_cmds_len = lcdkit_infos.aod_exit_cmds.cmd_cnt;
	pinfo->aod_exit_cmds = lcdkit_infos.aod_exit_cmds.cmds_set;
	
	pinfo->aod_high_brightness_cmds_len = lcdkit_infos.aod_high_brightness_cmds.cmd_cnt;
	pinfo->aod_high_brightness_cmds = lcdkit_infos.aod_high_brightness_cmds.cmds_set;
	pinfo->aod_low_brightness_cmds_len = lcdkit_infos.aod_low_brightness_cmds.cmd_cnt;
	pinfo->aod_low_brightness_cmds = lcdkit_infos.aod_low_brightness_cmds.cmds_set;

	pinfo->hbm_fp_prepare_support = lcdkit_data.panel->hbm_support;
	pinfo->hbm_fp_prepare_cmds_len = lcdkit_infos.aod_hbm_enter_cmds.cmd_cnt;
	pinfo->hbm_fp_prepare_cmds = lcdkit_infos.aod_hbm_enter_cmds.cmds_set;
	pinfo->hbm_fp_post_cmds_len = lcdkit_infos.aod_hbm_exit_cmds.cmd_cnt;
	pinfo->hbm_fp_post_cmds = lcdkit_infos.aod_hbm_exit_cmds.cmds_set;

	/* acm */
	pinfo->acm_support = lcdkit_data.panel->acm_support;
	pinfo->acm_lut_hue_table = 0;
	pinfo->acm_lut_hue_table_len = 0;
	pinfo->acm_lut_sata_table = 0;
	pinfo->acm_lut_sata_table_len = 0;
	pinfo->acm_lut_satr_table = 0;
	pinfo->acm_lut_satr_table_len = 0;
	/* acm ce */
	pinfo->acm_ce_support = lcdkit_data.panel->acm_ce_support;

	/* gamma lcp */
	pinfo->gamma_support = lcdkit_data.panel->gamma_support;
	pinfo->xcc_support = 0;
	pinfo->gmp_support = 0;

	pinfo->gamma_table = 0;
	pinfo->gamma_table_len = 0;
	pinfo->igm_table = 0;
	pinfo->igm_table_len = 0;
	pinfo->xcc_table = 0;
	pinfo->xcc_table_len = 0;

	/* for dynamic gamma calibration */
	pinfo ->dynamic_gamma_support = lcdkit_data.panel->dynamic_gamma_support;

	/* ldi info */
	pinfo->ldi.h_back_porch = lcdkit_data.ldi->h_back_porch;
	pinfo->ldi.h_front_porch = lcdkit_data.ldi->h_front_porch;
	pinfo->ldi.h_pulse_width = lcdkit_data.ldi->h_pulse_width;
	pinfo->ldi.v_back_porch = lcdkit_data.ldi->v_back_porch;
	pinfo->ldi.v_front_porch = lcdkit_data.ldi->v_front_porch;
	pinfo->ldi.v_pulse_width = lcdkit_data.ldi->v_pulse_width;
	pinfo->ldi.hsync_plr = lcdkit_data.ldi->hsync_plr;
	pinfo->ldi.vsync_plr = lcdkit_data.ldi->vsync_plr;
	pinfo->ldi.pixelclk_plr = lcdkit_data.ldi->pixelclk_plr;
	pinfo->ldi.data_en_plr = lcdkit_data.ldi->data_en_plr;

	/* Disable mipi eotp flag.  0: eotp enable, 1:eotp disable */
	pinfo->mipi.eotp_disable_flag = lcdkit_data.mipi->eotp_disable_flag;
	/* mipi info */
	pinfo->mipi.lane_nums = lcdkit_data.mipi->lane_nums;
	pinfo->mipi.color_mode = lcdkit_data.mipi->color_mode;
	pinfo->mipi.vc = lcdkit_data.mipi->vc;
	pinfo->mipi.dsi_bit_clk = lcdkit_data.mipi->dsi_bit_clk;
	pinfo->mipi.max_tx_esc_clk = lcdkit_data.mipi->max_tx_esc_clk * 1000000;
	pinfo->mipi.burst_mode = lcdkit_data.mipi->burst_mode;
	pinfo->mipi.dsi_bit_clk_val1 = lcdkit_data.mipi->dsi_bit_clk_val1;
	pinfo->mipi.dsi_bit_clk_val2 = lcdkit_data.mipi->dsi_bit_clk_val2;
	pinfo->mipi.dsi_bit_clk_val3 = lcdkit_data.mipi->dsi_bit_clk_val3;
	pinfo->mipi.dsi_bit_clk_val4 = lcdkit_data.mipi->dsi_bit_clk_val4;
	pinfo->mipi.dsi_bit_clk_val5 = lcdkit_data.mipi->dsi_bit_clk_val5;
	pinfo->mipi.dsi_bit_clk_upt = pinfo->mipi.dsi_bit_clk;
	pinfo->mipi.non_continue_en = lcdkit_data.mipi->non_continue_en;
	pinfo->mipi.clk_post_adjust = lcdkit_data.mipi->clk_post_adjust;
	pinfo->mipi.data_t_hs_trial_adjust = lcdkit_data.mipi->data_t_hs_trial_adjust;
	pinfo->mipi.data_t_hs_prepare_adjust = lcdkit_data.mipi->data_t_hs_prepare_adjust;
	pinfo->mipi.data_t_hs_zero_adjust = lcdkit_data.mipi->data_t_hs_zero_adjust;
	pinfo->mipi.clk_t_hs_prepare_adjust = lcdkit_data.mipi->clk_ths_prepare_adjust;
	pinfo->mipi_switch_support = lcdkit_data.panel->mipi_switch_support;
	if((lcdkit_data.mipi->data_t_lpx_adjust & 0xff00) == 0xff00)
	{
		pinfo->mipi.data_t_lpx_adjust = 0 - (lcdkit_data.mipi->data_t_lpx_adjust & 0xff);
	}
	else
	{
		pinfo->mipi.data_t_lpx_adjust = lcdkit_data.mipi->data_t_lpx_adjust;
	}
	pinfo->mipi.rg_vrefsel_vcm_adjust = lcdkit_data.mipi->rg_vrefsel_vcm_adjust;
	pinfo->mipi.rg_vrefsel_lptx_adjust= lcdkit_data.mipi->rg_vrefsel_lptx_adjust;
	pinfo->mipi.rg_lptx_sri_adjust = lcdkit_data.mipi->rg_lptx_sri_adjust;
	pinfo->mipi.rg_vrefsel_vcm_clk_adjust = lcdkit_data.mipi->rg_vrefsel_vcm_clk_adjust;
	pinfo->mipi.rg_vrefsel_vcm_data_adjust = lcdkit_data.mipi->rg_vrefsel_vcm_data_adjust;
	pinfo->mipi.phy_mode = lcdkit_data.mipi->phy_mode;
	pinfo->mipi.lp11_flag = lcdkit_data.mipi->lp11_flag;
	pinfo->mipi.hs_wr_to_time = lcdkit_data.mipi->hs_wr_to_time;
	pinfo->mipi.phy_m_n_count_update = lcdkit_data.mipi->phy_m_n_count_update;

	pinfo->dsi_bit_clk_upt_support = lcdkit_data.panel->dsi_bit_clk_upt_support;
	pinfo->pxl_clk_rate = lcdkit_data.panel->pxl_clk_rate * 1000000UL;
	pinfo->pxl_clk_rate_div = lcdkit_data.panel->pxl_clk_rate_div;

	ret = get_dts_string_index(DTS_COMP_LCDKIT_PANEL_TYPE, "lcd-bl-ic-name", 0, lcd_bl_ic_name_buf);
	if (ret < 0) {
		ret = memcpy_s(lcd_bl_ic_name_buf,
			sizeof(lcd_bl_ic_name_buf), "INVALID", strlen("INVALID"));
		if (ret != 0)
			HISI_FB_INFO("memcpy_s error\n");
	}

	ret = get_dts_value(DTS_COMP_LCDKIT_PANEL_TYPE,"lcd-bl-common-ic-support",&bl_common_ic);
	if (ret < 0)
		HISI_FB_INFO("lcd-bl-common-ic-support don't support!\n");

	if (bl_common_ic == ENABLE_COMMON_IC)
	{
		HISI_FB_ERR("bl_common_ic is true\n");
		memcpy_s(lcd_bl_ic_name_buf, sizeof(lcd_bl_ic_name_buf), "default", strlen("default"));
	}
	HISI_FB_ERR("lcd_bl_ic_name=%s!\n", lcd_bl_ic_name_buf);

	if (!strncmp(lcd_bl_ic_name_buf, "LM36923YFFR", strlen("LM36923YFFR"))) {
		pinfo->bl_min = 4;
		pinfo->bl_max = 1636;
		pinfo->bl_ic_ctrl_mode = RAMP_THEN_MUTI_MODE;
	} else if (!strncmp(lcd_bl_ic_name_buf, "LM36274", strlen("LM36274"))) {
		pinfo->bl_min = 4;
		pinfo->bl_max = 1636;
		pinfo->bl_ic_ctrl_mode = I2C_ONLY_MODE;
	} else if (!strncmp(lcd_bl_ic_name_buf, "LP8556", strlen("LP8556"))) {
		pinfo->bl_min = 3;
		pinfo->bl_max = 255;
		pinfo->bl_ic_ctrl_mode = BLPWM_AND_CABC_MODE;
	} else if(!strncmp(lcd_bl_ic_name_buf, "amoled_no_backlight_ic", strlen("amoled_no_backlight_ic"))){
		pinfo->bl_ic_ctrl_mode = AMOLED_NO_BL_IC_MODE;
		pinfo->bl_min = lcdkit_data.panel->bl_min;
		pinfo->bl_max = lcdkit_data.panel->bl_max;
	} else if(!strncmp(lcd_bl_ic_name_buf, "default", strlen("default")))
	{
		pinfo->bl_ic_ctrl_mode = COMMON_IC_MODE;
		pinfo->bl_min = lcdkit_data.panel->bl_min;
		pinfo->bl_max = lcdkit_data.panel->bl_max;
	} else if(!strncmp(lcd_bl_ic_name_buf, "blpwm_blic", strlen("blpwm_blic")))
	{
		pinfo->bl_ic_ctrl_mode = BLPWM_MODE;
		pinfo->bl_min = 3;
		pinfo->bl_max = 255;
	} else {
		pinfo->bl_min = 3;
		pinfo->bl_max = 255;
	}

	if (is_mipi_cmd_panel(hisifd))
	{
		pinfo->vsync_ctrl_type = lcdkit_data.panel->vsync_ctrl_type; // VSYNC_CTRL_ISR_OFF | VSYNC_CTRL_MIPI_ULPS | VSYNC_CTRL_CLK_OFF;
		pinfo->dirty_region_updt_support = lcdkit_data.panel->dirty_region_updt_support;
	}
	else
	{
		pinfo->mipi.burst_mode = lcdkit_data.mipi->burst_mode;
		pinfo->vsync_ctrl_type = lcdkit_data.panel->vsync_ctrl_type;
		pinfo->dirty_region_updt_support = lcdkit_data.panel->dirty_region_updt_support;
	}

	if (pinfo->ifbc_type == IFBC_TYPE_ORISE2X)
	{
		pinfo->ifbc_cmp_dat_rev0 = 1;
		pinfo->ifbc_cmp_dat_rev1 = 0;
		pinfo->ifbc_auto_sel = 0;
		pinfo->pxl_clk_rate_div = 2;
	}

	if (pinfo->ifbc_type == IFBC_TYPE_VESA3X_SINGLE) {
		pinfo->pxl_clk_rate_div = 3;

		/* dsc parameter info */
		pinfo->vesa_dsc.bits_per_component = 8;
		pinfo->vesa_dsc.bits_per_pixel = 8;
		pinfo->vesa_dsc.slice_width = 719;
		pinfo->vesa_dsc.slice_height = 31;

		pinfo->vesa_dsc.initial_xmit_delay = 512;
		pinfo->vesa_dsc.first_line_bpg_offset = 12;
		pinfo->vesa_dsc.mux_word_size = 48;

		/*	DSC_CTRL */
		pinfo->vesa_dsc.block_pred_enable = 1;
		pinfo->vesa_dsc.linebuf_depth = 9;

		/* RC_PARAM3 */
		pinfo->vesa_dsc.initial_offset = 6144;

		/* FLATNESS_QP_TH */
		pinfo->vesa_dsc.flatness_min_qp = 3;
		pinfo->vesa_dsc.flatness_max_qp = 12;

		/* DSC_PARAM4 */
		pinfo->vesa_dsc.rc_edge_factor = 0x6;
		pinfo->vesa_dsc.rc_model_size = 8192;

		/* DSC_RC_PARAM5: 0x330b0b */
		pinfo->vesa_dsc.rc_tgt_offset_lo = (0x330b0b >> 20) & 0xF;
		pinfo->vesa_dsc.rc_tgt_offset_hi = (0x330b0b >> 16) & 0xF;
		pinfo->vesa_dsc.rc_quant_incr_limit1 = (0x330b0b >> 8) & 0x1F;
		pinfo->vesa_dsc.rc_quant_incr_limit0 = (0x330b0b >> 0) & 0x1F;

		/* DSC_RC_BUF_THRESH0: 0xe1c2a38 */
		pinfo->vesa_dsc.rc_buf_thresh0 = (0xe1c2a38 >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh1 = (0xe1c2a38 >> 16) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh2 = (0xe1c2a38 >> 8) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh3 = (0xe1c2a38 >> 0) & 0xFF;

		/* DSC_RC_BUF_THRESH1: 0x46546269 */
		pinfo->vesa_dsc.rc_buf_thresh4 = (0x46546269 >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh5 = (0x46546269 >> 16) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh6 = (0x46546269 >> 8) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh7 = (0x46546269 >> 0) & 0xFF;

		/* DSC_RC_BUF_THRESH2: 0x7077797b */
		pinfo->vesa_dsc.rc_buf_thresh8 = (0x7077797b >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh9 = (0x7077797b >> 16) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh10 = (0x7077797b >> 8) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh11 = (0x7077797b >> 0) & 0xFF;

		/* DSC_RC_BUF_THRESH3: 0x7d7e0000 */
		pinfo->vesa_dsc.rc_buf_thresh12 = (0x7d7e0000 >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh13 = (0x7d7e0000 >> 16) & 0xFF;

		/* DSC_RC_RANGE_PARAM0: 0x1020100 */
		pinfo->vesa_dsc.range_min_qp0 = (0x1020100 >> 27) & 0x1F; //lint !e572
		pinfo->vesa_dsc.range_max_qp0 = (0x1020100 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset0 = (0x1020100 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp1 = (0x1020100 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp1 = (0x1020100 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset1 = (0x1020100 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM1: 0x94009be */
		pinfo->vesa_dsc.range_min_qp2 = (0x94009be >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp2 = (0x94009be >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset2 = (0x94009be >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp3 = (0x94009be >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp3 = (0x94009be >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset3 = (0x94009be >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM2, 0x19fc19fa */
		pinfo->vesa_dsc.range_min_qp4 = (0x19fc19fa >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp4 = (0x19fc19fa >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset4 = (0x19fc19fa >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp5 = (0x19fc19fa >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp5 = (0x19fc19fa >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset5 = (0x19fc19fa >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM3, 0x19f81a38 */
		pinfo->vesa_dsc.range_min_qp6 = (0x19f81a38 >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp6 = (0x19f81a38 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset6 = (0x19f81a38 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp7 = (0x19f81a38 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp7 = (0x19f81a38 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset7 = (0x19f81a38 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM4, 0x1a781ab6 */
		pinfo->vesa_dsc.range_min_qp8 = (0x1a781ab6 >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp8 = (0x1a781ab6 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset8 = (0x1a781ab6 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp9 = (0x1a781ab6 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp9 = (0x1a781ab6 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset9 = (0x1a781ab6 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM5, 0x2af62b34 */
		pinfo->vesa_dsc.range_min_qp10 = (0x2af62b34 >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp10 = (0x2af62b34 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset10 = (0x2af62b34 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp11 = (0x2af62b34 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp11 = (0x2af62b34 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset11 = (0x2af62b34 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM6, 0x2b743b74 */
		pinfo->vesa_dsc.range_min_qp12 = (0x2b743b74 >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp12 = (0x2b743b74 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset12 = (0x2b743b74 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp13 = (0x2b743b74 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp13 = (0x2b743b74 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset13 = (0x2b743b74 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM7, 0x6bf40000 */
		pinfo->vesa_dsc.range_min_qp14 = (0x6bf40000 >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp14 = (0x6bf40000 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset14 = (0x6bf40000 >> 16) & 0x3F;

		if (pinfo->pxl_clk_rate_div > 1) {
			pinfo->ldi.h_back_porch /= pinfo->pxl_clk_rate_div;
			pinfo->ldi.h_front_porch /= pinfo->pxl_clk_rate_div;
			pinfo->ldi.h_pulse_width /= pinfo->pxl_clk_rate_div;
		}
		/* IFBC Setting end */
	}

	if (pinfo->ifbc_type == IFBC_TYPE_VESA3_75X_DUAL) {
		pinfo->vesa_dsc.bits_per_component = 10;
		pinfo->vesa_dsc.linebuf_depth = 11;
		pinfo->vesa_dsc.bits_per_pixel = 8;
		pinfo->vesa_dsc.initial_xmit_delay = 512;

		pinfo->vesa_dsc.slice_width = 719; // 1439
		pinfo->vesa_dsc.slice_height = 7; // 31

		pinfo->vesa_dsc.first_line_bpg_offset = 12;
		pinfo->vesa_dsc.mux_word_size = 48;

		/* DSC_CTRL */
		pinfo->vesa_dsc.block_pred_enable = 1; // 0

		/* RC_PARAM3 */
		pinfo->vesa_dsc.initial_offset = 6144;

		/* FLATNESS_QP_TH */
		pinfo->vesa_dsc.flatness_min_qp = 7;
		pinfo->vesa_dsc.flatness_max_qp = 16;

		/* DSC_PARAM4 */
		pinfo->vesa_dsc.rc_edge_factor= 0x6;
		pinfo->vesa_dsc.rc_model_size = 8192;

		/* DSC_RC_PARAM5: 0x330f0f */
		pinfo->vesa_dsc.rc_tgt_offset_lo = (0x330f0f >> 20) & 0xF;
		pinfo->vesa_dsc.rc_tgt_offset_hi = (0x330f0f >> 16) & 0xF;
		pinfo->vesa_dsc.rc_quant_incr_limit1 = (0x330f0f >> 8) & 0x1F;
		pinfo->vesa_dsc.rc_quant_incr_limit0 = (0x330f0f >> 0) & 0x1F;

		/* DSC_RC_BUF_THRESH0: 0xe1c2a38 */
		pinfo->vesa_dsc.rc_buf_thresh0 = (0xe1c2a38 >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh1 = (0xe1c2a38 >> 16) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh2 = (0xe1c2a38 >> 8) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh3 = (0xe1c2a38 >> 0) & 0xFF;

		/* DSC_RC_BUF_THRESH1: 0x46546269 */
		pinfo->vesa_dsc.rc_buf_thresh4 = (0x46546269 >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh5 = (0x46546269 >> 16) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh6 = (0x46546269 >> 8) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh7 = (0x46546269 >> 0) & 0xFF;

		/* DSC_RC_BUF_THRESH2: 0x7077797b */
		pinfo->vesa_dsc.rc_buf_thresh8 = (0x7077797b >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh9 = (0x7077797b >> 16) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh10 = (0x7077797b >> 8) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh11 = (0x7077797b >> 0) & 0xFF;

		/* DSC_RC_BUF_THRESH3: 0x7d7e0000 */
		pinfo->vesa_dsc.rc_buf_thresh12 = (0x7d7e0000 >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh13 = (0x7d7e0000 >> 16) & 0xFF;

		/* DSC_RC_RANGE_PARAM0: 0x2022200 */
		pinfo->vesa_dsc.range_min_qp0 = (0x2022200 >> 27) & 0x1F; //lint !e572
		pinfo->vesa_dsc.range_max_qp0 = (0x2022200 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset0 = (0x2022200 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp1 = (0x2022200 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp1 = (0x2022200 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset1 = (0x2022200 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM1: 0x94009be */
		pinfo->vesa_dsc.range_min_qp2 = 5; // (0x94009be >> 27) & 0x1F
		pinfo->vesa_dsc.range_max_qp2 = 9; // (0x94009be >> 22) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset2 = (0x94009be >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp3 = 5; // (0x94009be >> 11) & 0x1F
		pinfo->vesa_dsc.range_max_qp3 = 10; // (0x94009be >> 6) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset3 = (0x94009be >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM2, 0x19fc19fa */
		pinfo->vesa_dsc.range_min_qp4 = 7; // (0x19fc19fa >> 27) & 0x1F
		pinfo->vesa_dsc.range_max_qp4 = 11; // (0x19fc19fa >> 22) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset4 = (0x19fc19fa >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp5 = 7; // (0x19fc19fa >> 11) & 0x1F
		pinfo->vesa_dsc.range_max_qp5 = 11; // (0x19fc19fa >> 6) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset5 = (0x19fc19fa >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM3, 0x19f81a38 */
		pinfo->vesa_dsc.range_min_qp6 = 7; // (0x19f81a38 >> 27) & 0x1F
		pinfo->vesa_dsc.range_max_qp6 = 11; // (0x19f81a38 >> 22) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset6 = (0x19f81a38 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp7 = 7; // (0x19f81a38 >> 11) & 0x1F
		pinfo->vesa_dsc.range_max_qp7 = 12; // (0x19f81a38 >> 6) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset7 = (0x19f81a38 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM4, 0x1a781ab6 */
		pinfo->vesa_dsc.range_min_qp8 = 7; // (0x1a781ab6 >> 27) & 0x1F
		pinfo->vesa_dsc.range_max_qp8 = 13; // (0x1a781ab6 >> 22) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset8 = (0x1a781ab6 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp9 = 7; // (0x1a781ab6 >> 11) & 0x1F
		pinfo->vesa_dsc.range_max_qp9 = 14; // (0x1a781ab6 >> 6) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset9 = (0x1a781ab6 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM5, 0x2af62b34 */
		pinfo->vesa_dsc.range_min_qp10 = 9; // (0x2af62b34 >> 27) & 0x1F
		pinfo->vesa_dsc.range_max_qp10 = 15; // (0x2af62b34 >> 22) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset10 = (0x2af62b34 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp11 = 9; // (0x2af62b34 >> 11) & 0x1F
		pinfo->vesa_dsc.range_max_qp11 = 16; // (0x2af62b34 >> 6) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset11 = (0x2af62b34 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM6, 0x2b743b74 */
		pinfo->vesa_dsc.range_min_qp12 = 9; // (0x2b743b74 >> 27) & 0x1F
		pinfo->vesa_dsc.range_max_qp12 = 17; // (0x2b743b74 >> 22) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset12 = (0x2b743b74 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp13 = 11; // (0x2b743b74 >> 11) & 0x1F
		pinfo->vesa_dsc.range_max_qp13 = 17; // (0x2b743b74 >> 6) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset13 = (0x2b743b74 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM7, 0x6bf40000 */
		pinfo->vesa_dsc.range_min_qp14 = 17; // (0x6bf40000 >> 27) & 0x1F
		pinfo->vesa_dsc.range_max_qp14 = 19; // (0x6bf40000 >> 22) & 0x1F
		pinfo->vesa_dsc.range_bpg_offset14 = (0x6bf40000 >> 16) & 0x3F;
	}
	if (pinfo->ifbc_type == IFBC_TYPE_VESA3X_DUAL) {
		pinfo->vesa_dsc.bits_per_component = 8;
		pinfo->vesa_dsc.linebuf_depth = 9;
		pinfo->vesa_dsc.bits_per_pixel = 8;
		pinfo->vesa_dsc.initial_xmit_delay = 512;

		if(lcdkit_data.panel->ifbc_vesa3x_set == 1){
			pinfo->vesa_dsc.slice_width = 719;
			pinfo->vesa_dsc.slice_height = 719;
		}else if(lcdkit_data.panel->ifbc_vesa3x_set == 2){
			pinfo->vesa_dsc.slice_width = 719;
			pinfo->vesa_dsc.slice_height = 29;
		}else {
			pinfo->vesa_dsc.slice_width = 719; // 1439
			pinfo->vesa_dsc.slice_height = 7; // 31
		}

		pinfo->vesa_dsc.first_line_bpg_offset = 12;
		pinfo->vesa_dsc.mux_word_size = 48;

		/* DSC_CTRL */
		pinfo->vesa_dsc.block_pred_enable = 1; // 0

		/* RC_PARAM3 */
		pinfo->vesa_dsc.initial_offset = 6144;

		/* FLATNESS_QP_TH */
		pinfo->vesa_dsc.flatness_min_qp = 3;
		pinfo->vesa_dsc.flatness_max_qp = 12;

		/* DSC_PARAM4 */
		pinfo->vesa_dsc.rc_edge_factor= 0x6;
		pinfo->vesa_dsc.rc_model_size = 8192;

		/* DSC_RC_PARAM5: 0x330b0b */
		pinfo->vesa_dsc.rc_tgt_offset_lo = (0x330b0b >> 20) & 0xF;
		pinfo->vesa_dsc.rc_tgt_offset_hi = (0x330b0b >> 16) & 0xF;
		pinfo->vesa_dsc.rc_quant_incr_limit1 = (0x330b0b >> 8) & 0x1F;
		pinfo->vesa_dsc.rc_quant_incr_limit0 = (0x330b0b >> 0) & 0x1F;

		/* DSC_RC_BUF_THRESH0: 0xe1c2a38 */
		pinfo->vesa_dsc.rc_buf_thresh0 = (0xe1c2a38 >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh1 = (0xe1c2a38 >> 16) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh2 = (0xe1c2a38 >> 8) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh3 = (0xe1c2a38 >> 0) & 0xFF;

		/* DSC_RC_BUF_THRESH1: 0x46546269 */
		pinfo->vesa_dsc.rc_buf_thresh4 = (0x46546269 >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh5 = (0x46546269 >> 16) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh6 = (0x46546269 >> 8) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh7 = (0x46546269 >> 0) & 0xFF;

		/* DSC_RC_BUF_THRESH2: 0x7077797b */
		pinfo->vesa_dsc.rc_buf_thresh8 = (0x7077797b >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh9 = (0x7077797b >> 16) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh10 = (0x7077797b >> 8) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh11 = (0x7077797b >> 0) & 0xFF;

		/* DSC_RC_BUF_THRESH3: 0x7d7e0000 */
		pinfo->vesa_dsc.rc_buf_thresh12 = (0x7d7e0000 >> 24) & 0xFF;
		pinfo->vesa_dsc.rc_buf_thresh13 = (0x7d7e0000 >> 16) & 0xFF;

		/* DSC_RC_RANGE_PARAM0: 0x1020100 */
		pinfo->vesa_dsc.range_min_qp0 = (0x1020100 >> 27) & 0x1F; //lint !e572
		pinfo->vesa_dsc.range_max_qp0 = (0x1020100 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset0 = (0x1020100 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp1 = (0x1020100 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp1 = (0x1020100 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset1 = (0x1020100 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM1: 0x94009be */
		pinfo->vesa_dsc.range_min_qp2 = (0x94009be >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp2 = (0x94009be >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset2 = (0x94009be >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp3 = (0x94009be >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp3 = (0x94009be >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset3 = (0x94009be >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM2, 0x19fc19fa */
		pinfo->vesa_dsc.range_min_qp4 = (0x19fc19fa >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp4 = (0x19fc19fa >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset4 = (0x19fc19fa >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp5 = (0x19fc19fa >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp5 = (0x19fc19fa >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset5 = (0x19fc19fa >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM3, 0x19f81a38 */
		pinfo->vesa_dsc.range_min_qp6 = (0x19f81a38 >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp6 = (0x19f81a38 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset6 = (0x19f81a38 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp7 = (0x19f81a38 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp7 = (0x19f81a38 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset7 = (0x19f81a38 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM4, 0x1a781ab6 */
		pinfo->vesa_dsc.range_min_qp8 = (0x1a781ab6 >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp8 = (0x1a781ab6 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset8 = (0x1a781ab6 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp9 = (0x1a781ab6 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp9 = (0x1a781ab6 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset9 = (0x1a781ab6 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM5, 0x2af62b34 */
		pinfo->vesa_dsc.range_min_qp10 = (0x2af62b34 >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp10 = (0x2af62b34 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset10 = (0x2af62b34 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp11 = (0x2af62b34 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp11 = (0x2af62b34 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset11 = (0x2af62b34 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM6, 0x2b743b74 */
		pinfo->vesa_dsc.range_min_qp12 = (0x2b743b74 >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp12 = (0x2b743b74 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset12 = (0x2b743b74 >> 16) & 0x3F;
		pinfo->vesa_dsc.range_min_qp13 = (0x2b743b74 >> 11) & 0x1F;
		pinfo->vesa_dsc.range_max_qp13 = (0x2b743b74 >> 6) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset13 = (0x2b743b74 >> 0) & 0x3F;

		/* DSC_RC_RANGE_PARAM7, 0x6bf40000 */
		pinfo->vesa_dsc.range_min_qp14 = (0x6bf40000 >> 27) & 0x1F;
		pinfo->vesa_dsc.range_max_qp14 = (0x6bf40000 >> 22) & 0x1F;
		pinfo->vesa_dsc.range_bpg_offset14 = (0x6bf40000 >> 16) & 0x3F;
	}

	if (lcdkit_data.panel->blpwm_precision_type == 3) {
		 pinfo->bl_ic_ctrl_mode = BLPWM_ONLY_MODE;
		 pinfo->bl_max = 255;
	}
}

/*
 * name:hw_lcd_reset_init
 * function:set reset timing
 * @cmds:gpio cmds
 */
static void lcdkit_reset_init(struct gpio_desc* cmds)
{
	struct gpio_desc* cm = NULL;

	cm = cmds;
	cm->wait = lcdkit_infos.lcd_delay->reset_step1_H;
	cm++;
	cm->wait = lcdkit_infos.lcd_delay->reset_L;
	cm++;
	cm->wait = lcdkit_infos.lcd_delay->reset_step2_H;
}
static void lcdkit_reset_high_init(struct gpio_desc* cmds)
{
	struct gpio_desc* cm = NULL;

	cm = cmds;
	cm->wait = lcdkit_infos.lcd_delay->reset_step2_H;
}
static void lcdkit_bias_on_dealy_init(struct gpio_desc* cmds)
{
	struct gpio_desc* cm = NULL;

	cm = cmds;
	cm->wait = lcdkit_infos.lcd_delay->delay_af_vsp_on;
	cm++;
	cm->wait = lcdkit_infos.lcd_delay->delay_af_vsn_on;
}

static void lcdkit_bias_off_dealy_init(struct gpio_desc* cmds)
{
	struct gpio_desc* cm = NULL;

	cm = cmds;
	cm->wait = lcdkit_infos.lcd_delay->delay_af_vsn_off;
	cm++;
	cm->wait = lcdkit_infos.lcd_delay->delay_af_vsp_off;
}

void hostprocessing_read_oem_info(struct hisi_fb_data_type* hisifd)
{
	int i;
	uint32_t read_ret;
	uint32_t read_value_oem_1[LCD_OEM_LEN] = {0};
	uint32_t read_value_oem_2[LCD_OEM_LEN] = {0};
	struct hisi_panel_info* pinfo = NULL;
	uint32_t mipi_dsi0_base;

	if (!hisifd)
	{
		HISI_FB_ERR("hisifd is NULL!\n");
		return;
	}

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	pinfo = hisifd->panel_info;

	if (!pinfo)
	{
		HISI_FB_ERR("panel_info is NULL!\n");
		return;
	}

	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;

	/* Read Hostprocessing LCD and TP OEM information */
	if (lcdkit_infos.lcd_misc->host_tp_oem_support)
	{
		if (lcdkit_infos.lcd_misc->host_panel_oem_pagea_support)
			mipi_dsi_cmds_tx(lcdkit_infos.lcd_oemprotectoffpagea.cmds_set, \
							 lcdkit_infos.lcd_oemprotectoffpagea.cmd_cnt, mipi_dsi0_base);

		if (lcdkit_infos.lcd_misc->host_panel_oem_readpart1_support)
		{
			read_ret = mipi_dsi_cmds_rx(read_value_oem_1, lcdkit_infos.lcd_oemreadfirstpart.cmds_set, \
				lcdkit_infos.lcd_oemreadfirstpart.cmd_cnt,
				(char *)(uintptr_t)mipi_dsi0_base);
			if (read_ret)
			{
				memset_s(read_value_oem_1, sizeof(read_value_oem_1), 0, sizeof(read_value_oem_1));
				HISI_FB_INFO("panel_info read lcd ddic oem1 failed!\n");
			}
		}

		if (lcdkit_infos.lcd_misc->host_panel_oem_pageb_support)
			mipi_dsi_cmds_tx(lcdkit_infos.lcd_oemprotectoffpageb.cmds_set, \
							 lcdkit_infos.lcd_oemprotectoffpageb.cmd_cnt, mipi_dsi0_base);

		if (lcdkit_infos.lcd_misc->host_panel_oem_readpart2_support)
		{
			read_ret = mipi_dsi_cmds_rx(read_value_oem_2, lcdkit_infos.lcd_oemreadsecondpart.cmds_set, \
				lcdkit_infos.lcd_oemreadsecondpart.cmd_cnt,
				(char *)(uintptr_t)mipi_dsi0_base);
			if (read_ret)
			{
				memset_s(read_value_oem_2, sizeof(read_value_oem_2), 0, sizeof(read_value_oem_2));
				HISI_FB_INFO("panel_info read lcd ddic oem1 failed!\n");
			}
		}

		if (lcdkit_infos.lcd_misc->host_panel_oem_backtouser_support)
			mipi_dsi_cmds_tx(lcdkit_infos.lcd_oembacktouser.cmds_set, \
							 lcdkit_infos.lcd_oembacktouser.cmd_cnt, mipi_dsi0_base);

		for (i = 0; i < lcdkit_infos.lcd_misc->host_panel_oem_readpart1_len; i++)
		{
			pinfo->lcd_oem_info[i] = (uint8_t)read_value_oem_1[i];
			HISI_FB_INFO("pinfo->lcd_oem_info[%d] = %c!\n", i, pinfo->lcd_oem_info[i]);
		}

		for (i = 0; i < lcdkit_infos.lcd_misc->host_panel_oem_readpart2_len; i++)
		{
			pinfo->lcd_oem_info[i + lcdkit_infos.lcd_misc->host_panel_oem_readpart1_len] = (uint8_t)read_value_oem_2[i];
			HISI_FB_INFO("pinfo->lcd_oem_info[%d] = %c!\n",
						 i + lcdkit_infos.lcd_misc->host_panel_oem_readpart1_len, pinfo->lcd_oem_info[i+lcdkit_infos.lcd_misc->host_panel_oem_readpart1_len]);
		}
	}
}

static void lcdkit_backlight_bias_ic_power_on(void)
{
	struct backlight_ic_info *tmp = NULL;
	tmp =  get_lcd_backlight_ic_info();
	if (tmp != NULL)
	{
		switch (tmp->ic_type) {
		case BACKLIGHT_BIAS_IC:
			HISI_FB_INFO("backlight ic is backlight and bias ic \n");
			if (lcdkit_infos.lcd_misc->use_gpio_lcd_bl_power)
			{
				gpio_cmds_tx(lcdkit_bl_power_request_cmds,
					ARRAY_SIZE(lcdkit_bl_power_request_cmds));
				gpio_cmds_tx(lcdkit_bl_power_enable_cmds,
					ARRAY_SIZE(lcdkit_bl_power_enable_cmds));
			}

			if (lcdkit_infos.lcd_misc->use_gpio_lcd_bl)
			{
				gpio_cmds_tx(lcdkit_bl_repuest_cmds,
					ARRAY_SIZE(lcdkit_bl_repuest_cmds));
				gpio_cmds_tx(lcdkit_bl_enable_cmds,
					ARRAY_SIZE(lcdkit_bl_enable_cmds));
			}
			lcdkit_backlight_ic_inital(tmp);
			break;
		default:
			break;
		}
	}
}

static void lcdkit_backlight_ic_power_on(void)
{
	struct backlight_ic_info *tmp = NULL;
	tmp =  get_lcd_backlight_ic_info();
	if (tmp != NULL)
	{
		switch (tmp->ic_type) {
		case BACKLIGHT_IC:
			HISI_FB_INFO("backlight ic is backlight ic \n");
			if (lcdkit_infos.lcd_misc->use_gpio_lcd_bl_power)
			{
				gpio_cmds_tx(lcdkit_bl_power_request_cmds,
					ARRAY_SIZE(lcdkit_bl_power_request_cmds));
				gpio_cmds_tx(lcdkit_bl_power_enable_cmds,
					ARRAY_SIZE(lcdkit_bl_power_enable_cmds));
			}

			if (lcdkit_infos.lcd_misc->use_gpio_lcd_bl)
			{
				gpio_cmds_tx(lcdkit_bl_repuest_cmds,
					ARRAY_SIZE(lcdkit_bl_repuest_cmds));
				gpio_cmds_tx(lcdkit_bl_enable_cmds,
					ARRAY_SIZE(lcdkit_bl_enable_cmds));
			}
			lcdkit_backlight_ic_inital(tmp);
			break;
		default:
			break;
		}
	}
}

static void lcdkit_backlight_ic_power_off(void)
{
	struct backlight_ic_info *tmp = NULL;
	tmp =  get_lcd_backlight_ic_info();
	if(tmp != NULL)
	{
		HISI_FB_INFO("backlight ic power off \n");
		lcdkit_backlight_ic_disable_device(tmp);
		if(lcdkit_infos.lcd_misc->use_gpio_lcd_bl)
		{
			gpio_cmds_tx(lcdkit_bl_disable_cmds, ARRAY_SIZE(lcdkit_bl_disable_cmds));
			gpio_cmds_tx(lcdkit_bl_free_cmds, ARRAY_SIZE(lcdkit_bl_free_cmds));
		}

		if(lcdkit_infos.lcd_misc->use_gpio_lcd_bl_power)
		{
			gpio_cmds_tx(lcdkit_bl_power_disable_cmds, ARRAY_SIZE(lcdkit_bl_power_disable_cmds));
			gpio_cmds_tx(lcdkit_bl_power_free_cmds, ARRAY_SIZE(lcdkit_bl_power_free_cmds));
		}
	}
}

static int jdi_panel_otp_reload(uint32_t mipi_dsi0_base)
{
	int ret;
	uint32_t read_back[2] = {0};
	uint8_t jdi_otp_id0;
	char lcd_reg[] = {0xa1};
	char reg_8f[] = {0x8f};
	char reg_90[] = {0x90};
	char reg_91[] = {0x91};
	char reg_32[2] = {0x32, 0x00};
	char reg_33[2] = {0x33, 0x00};
	char reg_34[2] = {0x34, 0x00};
	char page20[2] = {0xff, 0x20};
	char page2a[2] = {0xff, 0x2a};
	char reg_fb[2] = {0xfb, 0x01};
	char cmd1[2] = {0xff, 0x10};

	struct dsi_cmd_desc lcd_reg_cmd[] = {
		{DTYPE_GEN_READ1, 0, 1, WAIT_TYPE_MS,
			sizeof(lcd_reg), lcd_reg},
	};
	struct dsi_cmd_desc lcd_reg_8f_cmd[] = {
		{DTYPE_GEN_READ1, 0, 1, WAIT_TYPE_MS,
			sizeof(reg_8f), reg_8f},
	};
	struct dsi_cmd_desc lcd_reg_90_cmd[] = {
		{DTYPE_GEN_READ1, 0, 1, WAIT_TYPE_MS,
			sizeof(reg_90), reg_90},
	};
	struct dsi_cmd_desc lcd_reg_91_cmd[] = {
		{DTYPE_GEN_READ1, 0, 1, WAIT_TYPE_MS,
			sizeof(reg_91), reg_91},
	};
	struct dsi_cmd_desc lcd_reg_32_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(reg_32), reg_32},
	};
	struct dsi_cmd_desc lcd_reg_33_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(reg_33), reg_33},
	};
	struct dsi_cmd_desc lcd_reg_34_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(reg_34), reg_34},
	};
	struct dsi_cmd_desc lcd_page20_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(page20), page20},
	};
	struct dsi_cmd_desc lcd_page2a_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(page2a), page2a},
	};
	struct dsi_cmd_desc lcd_reg_fb_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(reg_fb), reg_fb},
	};

	struct dsi_cmd_desc cmd1_code_cmds[] = {
		{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
			sizeof(cmd1), cmd1},
	};

	/* read 0xa1 reg */
	ret = mipi_dsi_lread(read_back, lcd_reg_cmd, 5,
		(char *)(uintptr_t)mipi_dsi0_base);
	if (ret) {
		HISI_FB_INFO("read error, ret=%d\n", ret);
		return ret;
	}
	HISI_FB_INFO("jdi  read_back[0] = 0x%x,\n",read_back[0]);
	HISI_FB_INFO("jdi  read_back[1] = 0x%x,\n",read_back[1]);

	jdi_otp_id0 = read_back[1] & 0xFF;
	if (jdi_otp_id0 == 0x0A) {  /* switch page20 */
		mipi_dsi_cmds_tx(lcd_page20_cmds, \
			ARRAY_SIZE(lcd_page20_cmds), mipi_dsi0_base);
		mipi_dsi_cmds_tx(lcd_reg_fb_cmds, \
			ARRAY_SIZE(lcd_reg_fb_cmds), mipi_dsi0_base);

	/* read reg 8f,90,91 */
	ret = mipi_dsi_lread(read_back, lcd_reg_8f_cmd, 1,
		(char *)(uintptr_t)mipi_dsi0_base);
	if (ret)
		HISI_FB_ERR("read error, ret=%d\n", ret);

	reg_32[1] =  read_back[0] & 0xFF;
	HISI_FB_INFO("jdi  lcd  8f  first=0x%x\n", read_back[0]);
	ret = mipi_dsi_lread(read_back, lcd_reg_90_cmd, 1,
		(char *)(uintptr_t)mipi_dsi0_base);
	if (ret)
		HISI_FB_ERR("read error, ret=%d\n", ret);

	reg_33[1] =  read_back[0] & 0xFF;
	HISI_FB_INFO("jdi  lcd  90  first=0x%x\n", read_back[0]);
	ret = mipi_dsi_lread(read_back, lcd_reg_91_cmd, 1,
		(char *)(uintptr_t)mipi_dsi0_base);
	if (ret)
		HISI_FB_ERR("read error, ret=%d\n", ret);

	reg_34[1] =  read_back[0] & 0xFF;
	HISI_FB_INFO("jdi lcd  91  first=0x%x\n", read_back[0]);

	/* switch page2a */
	mipi_dsi_cmds_tx(lcd_page2a_cmds, \
		ARRAY_SIZE(lcd_page2a_cmds), mipi_dsi0_base);
	mipi_dsi_cmds_tx(lcd_reg_fb_cmds, \
		ARRAY_SIZE(lcd_reg_fb_cmds), mipi_dsi0_base);

	/* write reg 32,33,34 */
	mipi_dsi_cmds_tx(lcd_reg_32_cmds, \
		ARRAY_SIZE(lcd_reg_32_cmds), mipi_dsi0_base);
	mipi_dsi_cmds_tx(lcd_reg_33_cmds, \
		ARRAY_SIZE(lcd_reg_33_cmds), mipi_dsi0_base);
	mipi_dsi_cmds_tx(lcd_reg_34_cmds, \
		ARRAY_SIZE(lcd_reg_34_cmds), mipi_dsi0_base);

	/* switch page10 */
	mipi_dsi_cmds_tx(cmd1_code_cmds, \
		ARRAY_SIZE(cmd1_code_cmds), mipi_dsi0_base);
	}

	return 0;
}

/*
 * always disable pwm of backlight before light on lcd
*/
static void lcdkit_check_pwm_status(struct hisi_panel_info* pinfo)
{
	if (pinfo->bl_set_type & BL_SET_BY_BLPWM && lcdkit_infos.lcd_misc->bl_power_ctrl_mode & POWER_CTRL_BY_NONE)
	   hisi_blpwm_disable_pwmout();
}

static void pullup_request()
{
	int ret;
	int req_dis_gpio = 0;

	HISI_FB_INFO("+!\n");
	ret = get_dts_value(DTS_COMP_LCDKIT_PANEL_TYPE, "req_gpio_id", &req_dis_gpio);
	if (ret < 0 || req_dis_gpio <= 0) {
		HISI_FB_ERR("get req_gpio_id failed!\n");
		return;
	}
	struct gpio_operators *gpio_ops = NULL;
	gpio_ops = get_operators(GPIO_MODULE_NAME_STR);
	if (gpio_ops == NULL) {
		HISI_FB_ERR("can not get gpio_ops!\n");
		return;
	}
	ret = gpio_ops->gpio_request(req_dis_gpio);
	if (ret < 0) {
		HISI_FB_ERR("request req_gpio_id failed!\n");
		return;
	}
	gpio_ops->set_direction_output(req_dis_gpio);
	gpio_ops->set_value(req_dis_gpio, 1);
}

static void wait_switch_gpio()
{
	int ret;
	int dis_switch_gpio = 0;
	int timeout = 1000; // 5s timeout

	HISI_FB_INFO("+!\n");
	ret = get_dts_value(DTS_COMP_LCDKIT_PANEL_TYPE, "switch_gpio_id", &dis_switch_gpio);
	if (ret < 0 || dis_switch_gpio <= 0) {
		HISI_FB_ERR("get switch_gpio_id failed!\n");
		return;
	}
	struct gpio_operators *gpio_ops = NULL;
	gpio_ops = get_operators(GPIO_MODULE_NAME_STR);
	if (gpio_ops == NULL) {
		HISI_FB_ERR("can not get gpio_ops!\n");
		return;
	}
	ret = gpio_ops->gpio_request(dis_switch_gpio);
	if (ret < 0) {
		HISI_FB_ERR("request dis_switch_gpio failed!\n");
		return;
	}
	gpio_ops->set_direction_input(dis_switch_gpio);
	while ((gpio_ops->get_value(dis_switch_gpio) != 1) && (timeout >= 0)) {
		mdelay(5);
		timeout--;
	}
	if (timeout < 0)
		HISI_FB_INFO("gpio not switch to ap!\n");
}

/*
 * name:hw_lcd_on
 * function:set panel on
 * @pinfo:panel info
 */
static int lcdkit_on(struct hisi_fb_panel_data* pdata, struct hisi_fb_data_type* hisifd)
{
	struct charger_power_operators* charger_power_ops = NULL;
	static struct pmu_operators* pmu_ops = NULL;
	struct hisi_panel_info* pinfo = NULL;
	uint32_t mipi_dsi0_base;
	uint32_t mipi_dsi1_base;
	uint32_t read_ret;
	uint32_t read_value[2] = {0};
	uint32_t lcd_id1;
	uint32_t iovcc_time_interval;

	if (!hisifd || !pdata)
	{
		HISI_FB_ERR("hisifd or pdata is NULL!\n");
		return -1;
	}

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	pinfo = hisifd->panel_info;

	if (!pinfo)
	{
		HISI_FB_ERR("panel_info is NULL!\n");
		return -1;
	}

	if ( (lcdkit_infos.lcd_misc->vci_power_ctrl_mode & POWER_CTRL_BY_REGULATOR) || (lcdkit_infos.lcd_misc->iovcc_power_ctrl_mode & POWER_CTRL_BY_REGULATOR) || (lcdkit_infos.lcd_misc->vdd_power_ctrl_mode & POWER_CTRL_BY_REGULATOR) )
	{
		pmu_ops = get_operators(PMU_MODULE_NAME_STR);
		if (!pmu_ops)
		{
			HISI_FB_ERR("[fastboot]:can not get pmu_ops!\n");
			return -1;
		}
	}

	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	mipi_dsi1_base = hisifd->dss_base + DSS_MIPI_DSI1_OFFSET;

	lcdkit_check_pwm_status(pinfo);

	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON)
	{
		if (pinfo->mipi_switch_support) {
			pullup_request();
			wait_switch_gpio();
		}

		if (lcdkit_infos.lcd_delay->delay_af_first_iovcc_off > 0){
			iovcc_time_interval = get_boot_time() - g_iovcc_on_time;
			HISI_FB_INFO("iovcc_time_interval = %d\n", iovcc_time_interval);
			if (iovcc_time_interval < lcdkit_infos.lcd_delay->delay_af_first_iovcc_off)
				mdelay((lcdkit_infos.lcd_delay->delay_af_first_iovcc_off - iovcc_time_interval));
		}

		if(pinfo->bl_ic_ctrl_mode == COMMON_IC_MODE)
			lcdkit_backlight_bias_ic_power_on();
		if (lcdkit_data.panel->iovcc_before_vci == 1)
		{
			if (lcdkit_infos.lcd_misc->iovcc_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
			{
				pmu_ops->pmu_set_voltage(get_ldo_chanel(), lcdkit_infos.lcd_platform->lcdio_vcc);
				pmu_ops->pmu_enable(get_ldo_chanel());
			}
			else if (lcdkit_infos.lcd_misc->iovcc_power_ctrl_mode & POWER_CTRL_BY_GPIO)
			{
				gpio_cmds_tx(lcdkit_iovcc_request_cmds, ARRAY_SIZE(lcdkit_iovcc_request_cmds));
				gpio_cmds_tx(lcdkit_iovcc_enable_cmds, ARRAY_SIZE(lcdkit_iovcc_enable_cmds));
			}
			else
			{
				HISI_FB_ERR("the panel type or the power mode is not right for iovcc ctrl!\n ");
			}

			mdelay(lcdkit_infos.lcd_delay->delay_af_iovcc_on);

			if (HYBRID == lcdkit_infos.lcd_misc->lcd_type || AMOLED == lcdkit_infos.lcd_misc->lcd_type)
			{
				if (lcdkit_infos.lcd_misc->vci_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
				{
					pmu_ops->pmu_set_voltage(get_vci_ldo_chanel(),
						lcdkit_infos.lcd_platform->lcdanalog_vcc);
					pmu_ops->pmu_enable(get_vci_ldo_chanel());
				}
				else if ( lcdkit_infos.lcd_misc->vci_power_ctrl_mode & POWER_CTRL_BY_GPIO)
				{
					gpio_cmds_tx(lcdkit_vci_request_cmds, ARRAY_SIZE(lcdkit_vci_request_cmds));
					gpio_cmds_tx(lcdkit_vci_enable_cmds, ARRAY_SIZE(lcdkit_vci_enable_cmds));
				}
				else
				{
					 HISI_FB_ERR("the panel type or the power mode is not right for vci ctrl!\n ");
				}
			}

			mdelay(lcdkit_infos.lcd_delay->delay_af_vci_on);
		}
		else
		{
			if (HYBRID == lcdkit_infos.lcd_misc->lcd_type || AMOLED == lcdkit_infos.lcd_misc->lcd_type)
			{
				if (lcdkit_infos.lcd_misc->vci_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
				{
					pmu_ops->pmu_set_voltage(get_vci_ldo_chanel(),
						lcdkit_infos.lcd_platform->lcdanalog_vcc);
					pmu_ops->pmu_enable(get_vci_ldo_chanel());
				}
				else if ( lcdkit_infos.lcd_misc->vci_power_ctrl_mode & POWER_CTRL_BY_GPIO)
				{
					gpio_cmds_tx(lcdkit_vci_request_cmds, ARRAY_SIZE(lcdkit_vci_request_cmds));
					gpio_cmds_tx(lcdkit_vci_enable_cmds, ARRAY_SIZE(lcdkit_vci_enable_cmds));
				}
				else
				{
					HISI_FB_ERR("the panel type or the power mode is not right for vci ctrl!\n ");
				}
			}

			mdelay(lcdkit_infos.lcd_delay->delay_af_vci_on);

			if (lcdkit_infos.lcd_misc->iovcc_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
			{
				pmu_ops->pmu_set_voltage(get_ldo_chanel(), lcdkit_infos.lcd_platform->lcdio_vcc);
				pmu_ops->pmu_enable(get_ldo_chanel());
			}
			else if (lcdkit_infos.lcd_misc->iovcc_power_ctrl_mode & POWER_CTRL_BY_GPIO)
			{
				gpio_cmds_tx(lcdkit_iovcc_request_cmds, ARRAY_SIZE(lcdkit_iovcc_request_cmds));
				gpio_cmds_tx(lcdkit_iovcc_enable_cmds, ARRAY_SIZE(lcdkit_iovcc_enable_cmds));
			}
			else
			{
				HISI_FB_ERR("the panel type or the power mode is not right for iovcc ctrl!\n ");
			}
			mdelay(lcdkit_infos.lcd_delay->delay_af_iovcc_on);
		}

		if (lcdkit_infos.lcd_misc->vbat_power_ctrl_mode & POWER_CTRL_BY_GPIO)
		{
			gpio_cmds_tx(lcdkit_vbat_request_cmds, ARRAY_SIZE(lcdkit_vbat_request_cmds));
			gpio_cmds_tx(lcdkit_vbat_enable_cmds, ARRAY_SIZE(lcdkit_vbat_enable_cmds));
		}
		mdelay(lcdkit_infos.lcd_delay->delay_af_vbat_on);

		if (lcdkit_is_tp_reset_pull_h(lcdkit_data.panel->compatible))
		{
			pinctrl_cmds_tx(lcdkit_pinctrl_tp_normal_cmds, \
						ARRAY_SIZE(lcdkit_pinctrl_tp_normal_cmds));
			gpio_cmds_tx(lcdkit_gpio_tp_reset_normal_cmds, \
						ARRAY_SIZE(lcdkit_gpio_tp_reset_normal_cmds));
		}
		// lcd pinctrl normal
		pinctrl_cmds_tx(lcdkit_pinctrl_normal_cmds, \
						ARRAY_SIZE(lcdkit_pinctrl_normal_cmds));

		if (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_GPIO)
			pinctrl_cmds_tx(lcdkit_bias_pinctrl_normal_cmds, \
							ARRAY_SIZE(lcdkit_bias_pinctrl_normal_cmds));

		if (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_IC)
			pinctrl_cmds_tx(lcdkit_bias_pinctrl_normal_cmds, \
							ARRAY_SIZE(lcdkit_bias_pinctrl_normal_cmds));
		// lcd gpio request
		gpio_cmds_tx(lcdkit_gpio_reset_request_cmds, \
					 ARRAY_SIZE(lcdkit_gpio_reset_request_cmds));

		if (lcdkit_infos.lcd_misc->first_reset)
			// lcd gpio normal
			gpio_cmds_tx(lcdkit_gpio_reset_normal_cmds, \
						 ARRAY_SIZE(lcdkit_gpio_reset_normal_cmds));

		if ((lcdkit_infos.lcd_misc->lcd_panel_type & PANEL_TYPE_LCD) && (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_REGULATOR))
		{
			charger_power_ops = get_operators(CHARGER_POWER_MODULE_NAME_STR);
			if (!charger_power_ops)
			{
				HISI_FB_ERR("[display]: can not get charger_power_ops!\n");
				return -1;
			}

			/* set vsp/vsn voltage */
			charger_power_ops->charger_power_set_voltage(DEVICE_LCD_BIAS, lcdkit_infos.lcd_platform->lcd_bias);
			charger_power_ops->charger_power_set_voltage(DEVICE_LCD_VSP, lcdkit_infos.lcd_platform->lcd_vsp);
			charger_power_ops->charger_power_set_voltage(DEVICE_LCD_VSN, lcdkit_infos.lcd_platform->lcd_vsn);

			// Power enable
			charger_power_ops->charger_power_enable(DEVICE_LCD_BIAS);
			mdelay(lcdkit_infos.lcd_delay->delay_af_bias_on);
			charger_power_ops->charger_power_enable(DEVICE_LCD_VSP);
			mdelay(lcdkit_infos.lcd_delay->delay_af_vsp_on);
			charger_power_ops->charger_power_enable(DEVICE_LCD_VSN);
			mdelay(lcdkit_infos.lcd_delay->delay_af_vsn_on);
		}
		else if ((lcdkit_infos.lcd_misc->lcd_panel_type & PANEL_TYPE_LCD) && (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_GPIO))
		{
			gpio_cmds_tx(lcdkit_bias_request_cmds, ARRAY_SIZE(lcdkit_bias_request_cmds));
			gpio_cmds_tx(lcdkit_bias_enable_cmds, ARRAY_SIZE(lcdkit_bias_enable_cmds));
		}
		else if ((lcdkit_infos.lcd_misc->lcd_panel_type & PANEL_TYPE_LCD) && (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_IC))
		{
			gpio_cmds_tx(lcdkit_bias_request_cmds, ARRAY_SIZE(lcdkit_bias_request_cmds));
			gpio_cmds_tx(lcdkit_bias_enable_cmds, ARRAY_SIZE(lcdkit_bias_enable_cmds));
			lcdkit_power_on_bias_enable();
		}
		else
		{
			HISI_FB_ERR("the panel type or the power mode is not right for bias ctrl!\n ");
		}
		if(pinfo->bl_ic_ctrl_mode == COMMON_IC_MODE)
			lcdkit_backlight_ic_power_on();
		if(lcdkit_infos.lcd_misc->tprst_before_lcdrst)
		{
			pinctrl_cmds_tx(lcdkit_pinctrl_tp_normal_cmds, \
						ARRAY_SIZE(lcdkit_pinctrl_tp_normal_cmds));
			gpio_cmds_tx(lcdkit_gpio_tp_reset_normal_cmds, \
						ARRAY_SIZE(lcdkit_gpio_tp_reset_normal_cmds));
		}
		pinfo->lcd_init_step = LCD_INIT_MIPI_LP_SEND_SEQUENCE;
	}
	else if (pinfo->lcd_init_step == LCD_INIT_MIPI_LP_SEND_SEQUENCE)
	{
		mdelay(lcdkit_infos.lcd_delay->delay_af_LP11);

		// lcd gpio normal
		if (lcdkit_infos.lcd_misc->second_reset)
		{
			if(lcdkit_infos.lcd_misc->reset_pull_high_flag == 1)
			{
				gpio_cmds_tx(lcdkit_gpio_reset_high_cmds, \
					ARRAY_SIZE(lcdkit_gpio_reset_high_cmds));
			}
			else
			{
				gpio_cmds_tx(lcdkit_gpio_reset_normal_cmds, \
					ARRAY_SIZE(lcdkit_gpio_reset_normal_cmds));
			}
		}
#if defined (FASTBOOT_DISPLAY_LOGO_KIRIN980)
		if (lcdkit_infos.lcd_misc->vdd_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
		{
			pmu_ops->pmu_set_voltage(DEVICE_LDO25, lcdkit_infos.lcd_platform->lcdvdd_vcc);
			pmu_ops->pmu_enable(DEVICE_LDO25);
		}
		mdelay(lcdkit_infos.lcd_delay->delay_af_vdd_on);
#endif
		// Read LCD ID
		if (lcdkit_infos.lcd_misc->id_pin_read_support) {
			read_ret = mipi_dsi_sread(read_value,
				lcdkit_infos.id_pin_read_cmds.cmds_set,
				(char *)(uintptr_t)mipi_dsi0_base);
			if (read_ret) {
				lcd_id1 = 0;
			} else {
				lcd_id1 = read_value[0];
			}

			pinfo->panel_lcd_id= lcd_id1;
			update_lcdkit_data(lcd_id1);
			HISI_FB_INFO("panel lcd id=0x%x\n", lcd_id1);
		}

		// lcd display on sequence
		mipi_dsi_cmds_tx(lcdkit_infos.display_on_cmds.cmds_set, \
						 lcdkit_infos.display_on_cmds.cmd_cnt, mipi_dsi0_base);
		if (lcdkit_data.panel->dsi1_snd_cmd_panel_support) {
			mipi_dsi_cmds_tx(lcdkit_infos.display_on_cmds.cmds_set, \
							 lcdkit_infos.display_on_cmds.cmd_cnt, mipi_dsi1_base);
			mdelay(lcdkit_infos.lcd_delay->delay_af_display_on);

			mipi_dsi_cmds_tx(lcdkit_infos.display_on_second_cmds.cmds_set, \
							 lcdkit_infos.display_on_second_cmds.cmd_cnt, mipi_dsi0_base);
			mipi_dsi_cmds_tx(lcdkit_infos.display_on_second_cmds.cmds_set, \
							 lcdkit_infos.display_on_second_cmds.cmd_cnt, mipi_dsi1_base);
		}

#ifndef CONFIG_FACTORY_MODE
		if(lcdkit_infos.lcd_misc->display_effect_on_support)
		{
			HISI_FB_INFO("display on effect is support!\n");
			mipi_dsi_cmds_tx(lcdkit_infos.display_on_effect_cmds.cmds_set,\
			lcdkit_infos.display_on_effect_cmds.cmd_cnt,mipi_dsi0_base);
			if (lcdkit_data.panel->dsi1_snd_cmd_panel_support)
				mipi_dsi_cmds_tx(lcdkit_infos.display_on_effect_cmds.cmds_set,\
								 lcdkit_infos.display_on_effect_cmds.cmd_cnt,mipi_dsi1_base);
		}
#endif
		/* Read lcd color */
		if (lcdkit_infos.lcd_misc->tp_color_support)
		{
			mipi_dsi_cmds_tx(lcdkit_infos.command2_enter_cmds.cmds_set, \
				 lcdkit_infos.command2_enter_cmds.cmd_cnt, mipi_dsi0_base);

			read_ret = mipi_dsi_sread(read_value,
				lcdkit_infos.tp_color_cmds.cmds_set,
				(char *)(uintptr_t)mipi_dsi0_base);
			if (read_ret)
			{
				pinfo->tp_color = 0;
			}
			else
			{
				pinfo->tp_color = read_value[0];
			}

			mipi_dsi_cmds_tx(lcdkit_infos.command2_exit_cmds.cmds_set, \
				 lcdkit_infos.command2_exit_cmds.cmd_cnt, mipi_dsi0_base);

			HISI_FB_INFO("tp color = %d\n", pinfo->tp_color);
		}

		if (lcdkit_infos.lcd_misc->lcd_otp_support)
			jdi_panel_otp_reload(mipi_dsi0_base);

		hostprocessing_read_oem_info(hisifd);
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN970)
		if(lcdkit_infos.lcd_misc->lcd_brightness_color_uniform_support)
			lcd_bright_rgbw_id_from_oeminfo_process(hisifd);
#endif

		pinfo->lcd_init_step = LCD_INIT_MIPI_HS_SEND_SEQUENCE;
	}
	else if (pinfo->lcd_init_step == LCD_INIT_MIPI_HS_SEND_SEQUENCE)
	{
		;
	}
	else
	{
		HISI_FB_ERR("Invalid step to init lcd: %d!\n", pinfo->lcd_init_step);
	}

	// backlight on

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int lcdkit_off(struct hisi_fb_panel_data* pdata, struct hisi_fb_data_type* hisifd)
{
	uint32_t mipi_dsi0_base;
	uint32_t mipi_dsi1_base;
	struct hisi_panel_info* pinfo = NULL;
	static struct pmu_operators* pmu_ops = NULL;
	struct charger_power_operators* charger_power_ops = NULL;

	if (!hisifd || !pdata)
	{
		HISI_FB_ERR("hisifd or pdata is NULL!\n");
		return -1;
	}

	pinfo = hisifd->panel_info;

	if (!pinfo)
	{
		HISI_FB_ERR("panel_info is NULL!\n");
		return -1;
	}

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	mipi_dsi1_base = hisifd->dss_base + DSS_MIPI_DSI1_OFFSET;

	if (pinfo->lcd_uninit_step == LCD_UNINIT_MIPI_HS_SEND_SEQUENCE)
	{
		// lcd display off sequence
		mipi_dsi_cmds_tx(lcdkit_infos.display_off_cmds.cmds_set, \
						 lcdkit_infos.display_off_cmds.cmd_cnt, mipi_dsi0_base);

	if (lcdkit_data.panel->dsi1_snd_cmd_panel_support) {
		mipi_dsi_cmds_tx(lcdkit_infos.display_off_cmds.cmds_set, \
						 lcdkit_infos.display_off_cmds.cmd_cnt, mipi_dsi1_base);
		mdelay(lcdkit_infos.lcd_delay->delay_af_display_off);

		mipi_dsi_cmds_tx(lcdkit_infos.display_off_second_cmds.cmds_set, \
						lcdkit_infos.display_off_second_cmds.cmd_cnt, mipi_dsi0_base);
		mipi_dsi_cmds_tx(lcdkit_infos.display_off_second_cmds.cmds_set, \
						lcdkit_infos.display_off_second_cmds.cmd_cnt, mipi_dsi1_base);
		mdelay(lcdkit_infos.lcd_delay->delay_af_display_off_second);
	}

		pinfo->lcd_uninit_step = LCD_UNINIT_MIPI_LP_SEND_SEQUENCE;
	}
	else if (pinfo->lcd_uninit_step == LCD_UNINIT_MIPI_LP_SEND_SEQUENCE)
	{
		pinfo->lcd_uninit_step = LCD_UNINIT_POWER_OFF;
	}
	else if (pinfo->lcd_uninit_step == LCD_UNINIT_POWER_OFF)
	{
		if ((lcdkit_infos.lcd_misc->lcd_panel_type & PANEL_TYPE_LCD) && (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_REGULATOR))
		{
			charger_power_ops = get_operators(CHARGER_POWER_MODULE_NAME_STR);
			if (!charger_power_ops)
			{
				HISI_FB_ERR("[display]: can not get gpio_ops!\n");
				return -1;
			}

			charger_power_ops->charger_power_disable(DEVICE_LCD_VSN);
			mdelay(lcdkit_infos.lcd_delay->delay_af_vsn_off);
			charger_power_ops->charger_power_disable(DEVICE_LCD_VSP);
			mdelay(lcdkit_infos.lcd_delay->delay_af_vsp_off);
			charger_power_ops->charger_power_disable(DEVICE_LCD_BIAS);
			mdelay(lcdkit_infos.lcd_delay->delay_af_bias_off);
		}
		else if ((lcdkit_infos.lcd_misc->lcd_panel_type & PANEL_TYPE_LCD) && (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_GPIO))
		{
			gpio_cmds_tx(lcdkit_bias_disable_cmds, ARRAY_SIZE(lcdkit_bias_disable_cmds));
			gpio_cmds_tx(lcdkit_bias_free_cmds, ARRAY_SIZE(lcdkit_bias_free_cmds));
		}
		else if((lcdkit_infos.lcd_misc->lcd_panel_type & PANEL_TYPE_LCD) && (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_IC))
		{
			gpio_cmds_tx(lcdkit_bias_disable_cmds, ARRAY_SIZE(lcdkit_bias_disable_cmds));
			gpio_cmds_tx(lcdkit_bias_free_cmds, ARRAY_SIZE(lcdkit_bias_free_cmds));
		}
		else
		{
			 HISI_FB_ERR("the panel type or the power mode is not right for bias ctrl!\n ");
		}

		/* reset shutdown before vsn disable */
		if (!lcdkit_infos.lcd_misc->reset_shutdown_later)
		{
			/* lcd gpio lowpower */
			gpio_cmds_tx(lcdkit_gpio_reset_low_cmds, \
						 ARRAY_SIZE(lcdkit_gpio_reset_low_cmds));
			// lcd gpio free
			gpio_cmds_tx(lcdkit_gpio_reset_free_cmds, \
						 ARRAY_SIZE(lcdkit_gpio_reset_free_cmds));
			// lcd pinctrl lowpower
			pinctrl_cmds_tx(lcdkit_pinctrl_low_cmds,
							ARRAY_SIZE(lcdkit_pinctrl_low_cmds));
		}

		if (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & (POWER_CTRL_BY_GPIO | POWER_CTRL_BY_IC))
			pinctrl_cmds_tx(lcdkit_bias_pinctrl_low_cmds,
							ARRAY_SIZE(lcdkit_bias_pinctrl_low_cmds));

		if ( (lcdkit_infos.lcd_misc->vci_power_ctrl_mode & POWER_CTRL_BY_REGULATOR) || (lcdkit_infos.lcd_misc->iovcc_power_ctrl_mode & POWER_CTRL_BY_REGULATOR) || (lcdkit_infos.lcd_misc->vdd_power_ctrl_mode & POWER_CTRL_BY_REGULATOR) )
		{
			// lcd vcc disable
			pmu_ops = get_operators(PMU_MODULE_NAME_STR);
			if (!pmu_ops)
			{
				HISI_FB_ERR("[fastboot]:can not get pmu_ops!\n");
				return -1;
			}
		}

		if (lcdkit_infos.lcd_misc->vbat_power_ctrl_mode & POWER_CTRL_BY_GPIO)
		{
			gpio_cmds_tx(lcdkit_vbat_disable_cmds, ARRAY_SIZE(lcdkit_vbat_disable_cmds));
			gpio_cmds_tx(lcdkit_vbat_free_cmds, ARRAY_SIZE(lcdkit_vbat_free_cmds));
		}
		mdelay(lcdkit_infos.lcd_delay->delay_af_vbat_off);

#if defined (FASTBOOT_DISPLAY_LOGO_KIRIN980)
		if (lcdkit_infos.lcd_misc->vdd_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
			pmu_ops->pmu_disable(DEVICE_LDO25);
#endif

		if (lcdkit_infos.lcd_misc->iovcc_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
		{
			pmu_ops->pmu_disable(get_ldo_chanel());
		}
		else if (lcdkit_infos.lcd_misc->iovcc_power_ctrl_mode & POWER_CTRL_BY_GPIO)
		{
			gpio_cmds_tx(lcdkit_iovcc_disable_cmds, ARRAY_SIZE(lcdkit_iovcc_disable_cmds));
			gpio_cmds_tx(lcdkit_iovcc_free_cmds, ARRAY_SIZE(lcdkit_iovcc_free_cmds));
		}

		mdelay(lcdkit_infos.lcd_delay->delay_af_iovcc_off);

		/* reset shutdown after vsp disable */
		if (lcdkit_infos.lcd_misc->reset_shutdown_later)
		{
			/* lcd gpio lowpower */
			gpio_cmds_tx(lcdkit_gpio_reset_low_cmds, \
					ARRAY_SIZE(lcdkit_gpio_reset_low_cmds));
			// lcd gpio free
			gpio_cmds_tx(lcdkit_gpio_reset_free_cmds, \
					ARRAY_SIZE(lcdkit_gpio_reset_free_cmds));
			// lcd pinctrl lowpower
			pinctrl_cmds_tx(lcdkit_pinctrl_low_cmds,
					ARRAY_SIZE(lcdkit_pinctrl_low_cmds));
		}

	   if ( HYBRID == lcdkit_infos.lcd_misc->lcd_type || AMOLED == lcdkit_infos.lcd_misc->lcd_type)
		{
			if (lcdkit_infos.lcd_misc->vci_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
			{
				pmu_ops->pmu_disable(get_vci_ldo_chanel());
			}
			else if (lcdkit_infos.lcd_misc->vci_power_ctrl_mode & POWER_CTRL_BY_GPIO)
			{
				gpio_cmds_tx(lcdkit_iovcc_disable_cmds, ARRAY_SIZE(lcdkit_iovcc_disable_cmds));
				gpio_cmds_tx(lcdkit_iovcc_free_cmds, ARRAY_SIZE(lcdkit_iovcc_free_cmds));
			}

			mdelay(lcdkit_infos.lcd_delay->delay_af_vci_off);
		}
		if(pinfo->bl_ic_ctrl_mode == COMMON_IC_MODE)
			lcdkit_backlight_ic_power_off();
	}

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

static int  lcdkit_set_backlight(struct hisi_fb_panel_data* pdata,
	struct hisi_fb_data_type* hisifd, uint32_t bl_level)
{
	struct hisi_panel_info* pinfo = NULL;
	uint32_t mipi_dsi0_base;
	uint32_t ret = 0;
	static bool already_enable = FALSE;
	struct charger_power_operators* charger_power_ops = NULL;
	char bl_level_adjust[3] = {0x61,0x00,0x00};

	struct dsi_cmd_desc lcd_bl_level_adjust[] = {
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
	   sizeof(bl_level_adjust), bl_level_adjust},
	};

	if (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
	{
		charger_power_ops = get_operators(CHARGER_POWER_MODULE_NAME_STR);
		if (!charger_power_ops)
		{
			HISI_FB_ERR("[display]: can not get gpio_ops!\n");
			return -1;
		}
	}

	if (!pdata)
		HISI_FB_ERR("pdata is null!\n");

	HISI_FB_DEBUG("fb%d, +.\n", hisifd->index);

	pinfo = hisifd->panel_info;
	mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;

	HISI_FB_INFO("bl_level is %d\n", bl_level);

	if(lcdkit_infos.lcd_misc->display_on_in_backlight)
		mipi_dsi_cmds_tx(lcdkit_infos.display_on_in_backlight_cmds.cmds_set, lcdkit_infos.display_on_in_backlight_cmds.cmd_cnt, mipi_dsi0_base);

	if (lcdkit_infos.lcd_misc->dis_on_cmds_delay_margin_support)
		mdelay(lcdkit_infos.lcd_misc->dis_on_cmds_delay_margin_time);

	if (pinfo->bl_set_type & BL_SET_BY_PWM)
	{
		ret = hisi_pwm_set_backlight(hisifd, bl_level);
	}
	else if (pinfo->bl_set_type & BL_SET_BY_BLPWM)
	{
		if(lcdkit_infos.lcd_misc->rgbw_support == 1) {
			bl_level_adjust[1] = (REG61H_VALUE_FOR_RGBW>>8)&0x0f;
			bl_level_adjust[2] = REG61H_VALUE_FOR_RGBW&0xff;
			mipi_dsi_cmds_tx(lcd_bl_level_adjust, ARRAY_SIZE(lcd_bl_level_adjust), mipi_dsi0_base);
		}
		if(pinfo->bl_ic_ctrl_mode == BLPWM_MODE)
		{
			if(lcdkit_infos.lcd_misc->use_gpio_lcd_bl)
			{
				if ((bl_level == 0) && already_enable)
				{
					gpio_cmds_tx(lcdkit_bl_disable_cmds, ARRAY_SIZE(lcdkit_bl_disable_cmds));
					gpio_cmds_tx(lcdkit_bl_free_cmds, ARRAY_SIZE(lcdkit_bl_free_cmds));
					already_enable = FALSE;
				}
				else if (!already_enable)
				{
					gpio_cmds_tx(lcdkit_bl_repuest_cmds,ARRAY_SIZE(lcdkit_bl_repuest_cmds));
					gpio_cmds_tx(lcdkit_bl_enable_cmds,ARRAY_SIZE(lcdkit_bl_enable_cmds));
					already_enable = TRUE;
				}
				else
				{
					HISI_FB_INFO("[display]: level = %d\n", bl_level);
				}
			}
		}
		ret = hisi_blpwm_set_backlight(hisifd, bl_level);

		if(pinfo->bl_v200)
		{
			HISI_FB_INFO("The backlight is controled by soc and v200\n");
			/* enable/disable backlight */
			if (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
			{
				 charger_power_ops = get_operators(CHARGER_POWER_MODULE_NAME_STR);
				 if (!charger_power_ops)
				 {
					HISI_FB_ERR("[display]: can not get gpio_ops!\n");
					return -1;
				 }

				 if ((bl_level == 0) && already_enable)
				 {
					charger_power_ops->charger_power_disable(DEVICE_WLED);
					already_enable = FALSE;
				 }
				 else if (!already_enable)
				 {
					charger_power_ops->charger_power_enable(DEVICE_WLED);
					already_enable = TRUE;
				 }
				 else
				 {
					HISI_FB_INFO("[display]: level = %d\n", bl_level);
				 }
			}
		}
	}
	else if (pinfo->bl_set_type & BL_SET_BY_SH_BLPWM)
	{
		ret = hisi_sh_blpwm_set_backlight(hisifd, bl_level);
	}
	else if (pinfo->bl_set_type & BL_SET_BY_MIPI)
	{
		bl_level = (bl_level < lcdkit_data.panel->bl_max) ? bl_level : lcdkit_data.panel->bl_max;
		if(lcdkit_infos.backlight_cmds.cmds_set[0].dlen == 2)
		{
			lcdkit_infos.backlight_cmds.cmds_set[0].payload[1] = bl_level * 255 / lcdkit_data.panel->bl_max; // one byte
		}
		else
		{
			if(pinfo->bl_otm)
			{
				lcdkit_infos.backlight_cmds.cmds_set[0].payload[1] =
					(bl_level >> 2) & 0xff;  // high 8bit
				lcdkit_infos.backlight_cmds.cmds_set[0].payload[2] =
					(bl_level << 2) & 0x00ff;  // low bit2-3
			}
			else if(lcdkit_data.panel->bl_byte_order == 1) {
				lcdkit_infos.backlight_cmds.cmds_set[0].payload[1] =
					bl_level & 0x00ff;  // low 8bit
				lcdkit_infos.backlight_cmds.cmds_set[0].payload[2] =
					(bl_level >> 8) & 0xff; // high 8bit
			} else {
				lcdkit_infos.backlight_cmds.cmds_set[0].payload[1] =
					(bl_level >> 8) & 0xff; // high 8bit
				lcdkit_infos.backlight_cmds.cmds_set[0].payload[2] =
					bl_level & 0x00ff;  // low 8bit
			}
		}
		/* display set backlight by mipi */
		mipi_dsi_cmds_tx(lcdkit_infos.backlight_cmds.cmds_set,lcdkit_infos.backlight_cmds.cmd_cnt, mipi_dsi0_base);

		if (lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_REGULATOR)
		{
			charger_power_ops = get_operators(CHARGER_POWER_MODULE_NAME_STR);
			if (!charger_power_ops)
			{
				HISI_FB_ERR("[display]: can not get gpio_ops!\n");
				return -1;
			}

			if ((bl_level == 0) && already_enable)
			{
				charger_power_ops->charger_power_disable(DEVICE_WLED);
				already_enable = FALSE;
			}
			else if (!already_enable)
			{
				charger_power_ops->charger_power_enable(DEVICE_WLED);
				already_enable = TRUE;
			}
			else
			{
				HISI_FB_INFO("[display]: level = %d\n", bl_level);
			}
		}
	}
	else
	{
		HISI_FB_ERR("fb%d, not support this bl_set_type %d!\n",
			hisifd->index, pinfo->bl_set_type);
	}

	HISI_FB_DEBUG("fb%d, -.\n", hisifd->index);

	return ret;
}

/* panel data */
static struct hisi_fb_panel_data lcdkit_panel_data =
{
	.on			 = lcdkit_on,
	.off			= lcdkit_off,
	.set_backlight  = lcdkit_set_backlight,
	.next		   = NULL,
};

static int  lcdkit_probe(struct hisi_fb_data_type* hisifd)
{
	struct hisi_panel_info* pinfo = NULL;
#if defined (FASTBOOT_DISPLAY_LOGO_HI3660) || defined (FASTBOOT_DISPLAY_LOGO_KIRIN970)
	int ret;
#endif
	if (!hisifd)
	{
		HISI_FB_ERR("hisifd is NULL!\n");
		return -1;
	}

	HISI_FB_INFO("+.\n");

	/* lcd reset gpio */
	gpio_lcdkit_reset = lcdkit_infos.lcd_platform->gpio_lcd_reset;
	/* lcd te gpio */
	gpio_lcdkit_te0 = lcdkit_infos.lcd_platform->gpio_lcd_te;
	/* lcd iovcc gpio */
	gpio_lcdkit_iovcc = lcdkit_infos.lcd_platform->gpio_lcd_iovcc;
	/* lcd vbat gpio */
	gpio_lcdkit_vbat = lcdkit_infos.lcd_platform->gpio_lcd_vbat;
	/* lcd vsn gpio */
	gpio_lcdkit_vci =   lcdkit_infos.lcd_platform->gpio_lcd_vci;
	/* lcd vsp gpio */
	gpio_lcdkit_vsp =   lcdkit_infos.lcd_platform->gpio_lcd_vsp;
	/* lcd vsn gpio */
	gpio_lcdkit_vsn =   lcdkit_infos.lcd_platform->gpio_lcd_vsn;
	/* lcd bl gpio */
	gpio_lcdkit_bl =   lcdkit_infos.lcd_platform->gpio_lcd_bl;
	gpio_lcdkit_bl_power = lcdkit_infos.lcd_platform->gpio_lcd_bl_power;

	if (lcdkit_is_tp_reset_pull_h(lcdkit_data.panel->compatible))
		gpio_lcdkit_tp_reset = lcdkit_infos.lcd_platform->gpio_tp_rst;
	if (lcdkit_infos.lcd_misc->tprst_before_lcdrst)
		gpio_lcdkit_tp_reset = lcdkit_infos.lcd_platform->gpio_tp_rst;
	// init lcd panel info
	pinfo = &lcd_info;
	memset_s(pinfo, sizeof(struct hisi_panel_info), 0, sizeof(struct hisi_panel_info));
	lcdkit_info_init(hisifd, pinfo);

#if defined (FASTBOOT_DISPLAY_LOGO_HI3660) || defined (FASTBOOT_DISPLAY_LOGO_KIRIN970)
	// Here to judge if the dynamic gamma calibration be supported.
	if(lcdkit_data.panel->dynamic_gamma_support == 1)
	{
		ret = lcdkit_write_gm_to_reserved_mem();
		if(ret < 0)
			HISI_FB_ERR("Writing the gamma data to shared memory is failed!");
	}
	else
	{
		HISI_FB_INFO("The panel does not support dynamic gamma!\n");
	}
#endif

	lcdkit_reset_init(lcdkit_gpio_reset_normal_cmds);
	lcdkit_bias_on_dealy_init(lcdkit_bias_enable_cmds);
	lcdkit_bias_off_dealy_init(lcdkit_bias_disable_cmds);

	if(lcdkit_infos.lcd_misc->lcdph_delay_set_flag == 1)
		lcdkit_reset_high_init(lcdkit_gpio_reset_high_cmds);
	// panel chain
	hisifd->panel_info = pinfo;
	lcdkit_panel_data.next = hisifd->panel_data;

	hisifd->panel_data = &lcdkit_panel_data;

	// add device
	hisi_fb_add_device(hisifd);

	if(pinfo->bl_ic_ctrl_mode == COMMON_IC_MODE)
	{
		struct backlight_ic_info *tmp = NULL;
		HISI_FB_INFO("detect backlight ic bl_power is %d  lcd_bl is %d\n",lcdkit_infos.lcd_misc->use_gpio_lcd_bl_power,lcdkit_infos.lcd_misc->use_gpio_lcd_bl);

		// register function for hisi
		hisi_blpwm_bl_register(lcdkit_backlight_common_set);
		// detect lcd backlight ic
		lcdkit_dts_set_ic_name("lcd-bl-ic-name","default");
		if(lcdkit_infos.lcd_misc->use_gpio_lcd_bl_power)
		{
			gpio_cmds_tx(lcdkit_bl_power_request_cmds,ARRAY_SIZE(lcdkit_bl_power_request_cmds));
			gpio_cmds_tx(lcdkit_bl_power_enable_cmds,ARRAY_SIZE(lcdkit_bl_power_enable_cmds));
		}
		if(lcdkit_infos.lcd_misc->use_gpio_lcd_bl)
		{
			gpio_cmds_tx(lcdkit_bl_repuest_cmds,ARRAY_SIZE(lcdkit_bl_repuest_cmds));
			gpio_cmds_tx(lcdkit_bl_enable_cmds,ARRAY_SIZE(lcdkit_bl_enable_cmds));
		}
		lcdkit_backlight_ic_select(lcdkit_infos.lcd_backlight_ic_info.lcd_backlight_ic_list,lcdkit_infos.lcd_backlight_ic_info.num_of_lcd_backlight_ic_list);
		tmp =  get_lcd_backlight_ic_info();
		if(tmp != NULL)
			lcdkit_dts_set_ic_name("lcd-bl-ic-name",tmp->name);
		if(lcdkit_infos.lcd_misc->use_gpio_lcd_bl)
		{
			gpio_cmds_tx(lcdkit_bl_disable_cmds, ARRAY_SIZE(lcdkit_bl_disable_cmds));
			gpio_cmds_tx(lcdkit_bl_free_cmds, ARRAY_SIZE(lcdkit_bl_free_cmds));
		}
		if(lcdkit_infos.lcd_misc->use_gpio_lcd_bl_power)
		{
			gpio_cmds_tx(lcdkit_bl_power_disable_cmds, ARRAY_SIZE(lcdkit_bl_power_disable_cmds));
			gpio_cmds_tx(lcdkit_bl_power_free_cmds, ARRAY_SIZE(lcdkit_bl_power_free_cmds));
		}
	}
	HISI_FB_INFO("-.\n");

	return 0;
}

struct hisi_fb_data_type lcdkit_hisifd =
{
	.panel_probe = lcdkit_probe,
};

static void update_lcdkit_data(uint32_t lcdkit_id)
{
	unsigned int productid;
	struct lcd_type_operators* lcd_type_ops = NULL;
	u8 panel_id;
	int select_panel_by_ic = 0;
	int ret;

	ret = get_dts_value(DTS_COMP_LCDKIT_PANEL_TYPE, "set_panel_by_ic", &select_panel_by_ic);
	if (ret < 0 || select_panel_by_ic <= 0) {
		HISI_FB_INFO("not support set_panel_by_ic!\n");
		return;
	}

	lcd_type_ops = get_operators(LCD_TYPE_MODULE_NAME_STR);
	if (!lcd_type_ops) {
		HISI_FB_ERR("failed to get lcd type operator!\n");
		return;
	}
	productid = lcd_type_ops->get_product_id();
	panel_id = lcdkit_panel_update(productid, lcdkit_id);
	if (!hw_init_panel_data(&lcdkit_data, &lcdkit_infos, panel_id))
		HISI_FB_ERR("Init panel data error.\n");
	lcd_type_ops->set_lcd_panel_type(lcdkit_data.panel->compatible);
	lcd_type_ops->set_hisifd(&lcdkit_hisifd);
	lcdkit_probe(&lcdkit_hisifd);
	HISI_FB_INFO("update_lcdkit_data compatible=%s.\n", lcdkit_data.panel->compatible);
}

static int lcdkit_init(struct system_table* systable)
{
	u8 panel_id;
	unsigned int productid;
	struct lcd_type_operators* lcd_type_ops = NULL;
	int lcd_type;
	char* panel_name = NULL;

	if (!systable)
		HISI_FB_ERR("systable is null!\n");

	lcd_type_ops = get_operators(LCD_TYPE_MODULE_NAME_STR);
	if (!lcd_type_ops)
	{
		HISI_FB_ERR("failed to get lcd type operator!\n");
	}
	else
	{
		lcd_type = lcd_type_ops->get_lcd_type();
		if  (lcd_type == LCDKIT)
		{
			HISI_FB_INFO("lcd type is LCDKIT.\n");
			memset_s(&lcdkit_infos, sizeof(struct lcdkit_disp_info), 0, sizeof(struct lcdkit_disp_info));
			memset_s(&lcdkit_data, sizeof(struct lcdkit_panel_data), 0, sizeof(struct lcdkit_panel_data));
			productid = lcd_type_ops->get_product_id();
			panel_id = lcdkit_panel_init(productid);
			g_iovcc_on_time = get_boot_time();
			if (!hw_init_panel_data(&lcdkit_data, &lcdkit_infos, panel_id))
				HISI_FB_ERR("Init panel data error.\n");

			if(!strncmp(lcdkit_data.panel->compatible, "auo_otm1901a_5p2_1080p_video_default",strlen("auo_otm1901a_5p2_1080p_video_default")))
			{
				HISI_FB_ERR("the panel is not buckled! \n");
				return 0;
			}

			panel_name = lcdkit_data.panel->panel_model ? lcdkit_data.panel->panel_model : lcdkit_data.panel->compatible;
			HISI_FB_INFO("productid = %d compatible = %s.\n", productid, panel_name);
			lcd_type_ops->set_lcd_panel_type(lcdkit_data.panel->compatible);
			lcd_type_ops->set_hisifd(&lcdkit_hisifd);
		}
		else
		{
			HISI_FB_INFO("lcd type is not LCDKIT.\n");
		}
	}

	return 0;
}

static struct module_data lcdkit_module_data =
{
	.name = LCDKIT_MODULE_NAME_STR,
	.level   = LCDKIT_MODULE_LEVEL,
	.init = lcdkit_init,
};

MODULE_INIT(LCDKIT_MODULE_NAME, lcdkit_module_data);
