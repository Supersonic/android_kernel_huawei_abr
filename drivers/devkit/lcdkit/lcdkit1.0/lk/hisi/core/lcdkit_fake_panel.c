/*
 * lcdkit_fake_panel.c
 *
 * lcdkit fake panel function for lcd driver
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

#include <boardid.h>
#include "hisi_fb.h"
#include "lcdkit_disp.h"
#include "lcdkit_panels.h"

#define DEFAULT_PRODUCT_ID 1
#define DEFAULT_PANEL_ID 1
#define DEFAULT_LCD_KIT_ID 1
/* panel info */
static struct hisi_panel_info lcd_info = {0};

static uint32_t gpio_lcdkit_vsp;
static uint32_t gpio_lcdkit_vsn;
static uint32_t gpio_lcdkit_bl;

static uint32_t gpio_lcdkit_bl_power;
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

#define LCDKIT_FAKE_MODULE_NAME_STR  "lcdkit_fake_panel"
#define LCDKIT_FAKE_MODULE_NAME  lcdkit_fake_panel

#define LCDKIT_DTS_COMP_MIPI_FAKE_PANEL "auo_otm1901a_5p2_1080p_video_default"
#define DTS_COMP_LCDKIT_PANEL_TYPE	 "huawei,lcd_panel_type"

static void lcdkit_info_init(struct hisi_fb_data_type* hisifd, struct hisi_panel_info* pinfo)
{
	int ret;
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
	pinfo->type = 8;
	pinfo->ifbc_type = lcdkit_data.panel->ifbc_type;
	pinfo->lcd_type = LCDKIT;
	pinfo->lcd_name = lcdkit_data.panel->lcd_name;
	pinfo->frc_enable = lcdkit_data.panel->frc_enable;
	pinfo->esd_enable = lcdkit_data.panel->esd_enable;
	pinfo->prefix_ce_support = lcdkit_data.panel->prefix_ce_support;
	pinfo->prefix_sharpness_support = lcdkit_data.panel->prefix_sharpness_support;
	pinfo->lcd_uninit_step_support = lcdkit_data.panel->lcd_uninit_step_support;

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
	HISI_FB_ERR("fake panel lcd_bl_ic_name=%s!\n", lcd_bl_ic_name_buf);

	if(!strncmp(lcd_bl_ic_name_buf, "default", strlen("default")))
	{
		pinfo->bl_ic_ctrl_mode = COMMON_IC_MODE;
	} else if(!strncmp(lcd_bl_ic_name_buf, "amoled_no_backlight_ic", strlen("amoled_no_backlight_ic")))
	{
		pinfo->bl_ic_ctrl_mode = AMOLED_NO_BL_IC_MODE;
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
	else
	{
		pinfo->ifbc_type = IFBC_TYPE_NONE;
		pinfo->pxl_clk_rate_div = 1;
	}
}

static int  lcdkit_fake_set_backlight(struct hisi_fb_panel_data *pdata,
	struct hisi_fb_data_type *hisifd, uint32_t bl_level)
{
	if (!pdata || !hisifd)
		HISI_FB_ERR("bl_level = %d\n", bl_level);

	return 0;
}
static int lcdkit_fake_on(struct hisi_fb_panel_data* pdata, struct hisi_fb_data_type* hisifd)
{
	struct hisi_panel_info* pinfo = NULL;

	if (!hisifd || !pdata)
	{
		HISI_FB_ERR("hisifd or pdata is NULL!\n");
		return -1;
	}

	HISI_FB_INFO("fb%d, +!\n", hisifd->index);

	pinfo = hisifd->panel_info;
	if (!pinfo)
	{
		lcdkit_panel_init(DEFAULT_PRODUCT_ID);
		hw_init_panel_data(&lcdkit_data, &lcdkit_infos, DEFAULT_PANEL_ID);
		lcdkit_panel_update(DEFAULT_PRODUCT_ID, DEFAULT_LCD_KIT_ID);
		HISI_FB_ERR("panel_info is NULL!\n");
		return -1;
	}

	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON)
	{
		pinfo->lcd_init_step = LCD_INIT_MIPI_LP_SEND_SEQUENCE;
	}
	else if (pinfo->lcd_init_step == LCD_INIT_MIPI_LP_SEND_SEQUENCE)
	{
		if(pinfo->bl_ic_ctrl_mode == COMMON_IC_MODE)
		{
			struct backlight_ic_info *tmp = NULL;

			tmp =  get_lcd_backlight_ic_info();
			if(tmp != NULL)
			{
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
				lcdkit_backlight_ic_inital(tmp);
				lcdkit_backlight_ic_disable_brightness(tmp);
				if(tmp->ic_type == BACKLIGHT_BIAS_IC)
					lcdkit_backlight_ic_bias(tmp,false);
			}
		}
		gpio_cmds_tx(lcdkit_bias_request_cmds, ARRAY_SIZE(lcdkit_bias_request_cmds));
		gpio_cmds_tx(lcdkit_bias_enable_cmds, ARRAY_SIZE(lcdkit_bias_enable_cmds));
		if(lcdkit_infos.lcd_misc->bias_power_ctrl_mode & POWER_CTRL_BY_IC)
			lcdkit_power_on_bias_enable();
		lcd_removed_proc(hisifd);
		pinfo->tp_color = 0;
		pinfo->lcd_type = UNKNOWN_LCD;
		pinfo->lcd_init_step = LCD_INIT_MIPI_HS_SEND_SEQUENCE;
	}
	else if (pinfo->lcd_init_step == LCD_INIT_MIPI_HS_SEND_SEQUENCE)
	{
		;
	}
	else
	{
		HISI_FB_ERR("Invalid step to init lcd!\n");
	}

	HISI_FB_INFO("fb%d, -!\n", hisifd->index);

	return 0;
}

/* panel data */
static struct hisi_fb_panel_data lcdkit_fake_panel_data =
{
	.on			 = lcdkit_fake_on,
	.off			= NULL,
	.set_backlight  = lcdkit_fake_set_backlight,
	.next		   = NULL,
};

static int lcdkit_fake_probe(struct hisi_fb_data_type* hisifd)
{
	struct hisi_panel_info* pinfo = NULL;

	if (!hisifd)
	{
		HISI_FB_ERR("hisifd is NULL!\n");
		return -1;
	}

	HISI_FB_INFO("+.\n");

	/* lcd vsp gpio */
	gpio_lcdkit_vsp =   lcdkit_infos.lcd_platform->gpio_lcd_vsp;
	/* lcd vsn gpio */
	gpio_lcdkit_vsn =   lcdkit_infos.lcd_platform->gpio_lcd_vsn;
	/* lcd bl gpio */
	gpio_lcdkit_bl =   lcdkit_infos.lcd_platform->gpio_lcd_bl;
	gpio_lcdkit_bl_power = lcdkit_infos.lcd_platform->gpio_lcd_bl_power;
	// init lcd panel info
	pinfo = &lcd_info;
	memset_s(pinfo, sizeof(struct hisi_panel_info), 0, sizeof(struct hisi_panel_info));
	lcdkit_info_init(hisifd, pinfo);
	hisifd->panel_info = pinfo;
	lcdkit_fake_panel_data.next = hisifd->panel_data;

	hisifd->panel_data = &lcdkit_fake_panel_data;

	// add device
	hisi_fb_add_device(hisifd);

	if(pinfo->bl_ic_ctrl_mode == COMMON_IC_MODE)
	{
		struct backlight_ic_info *tmp = NULL;
		HISI_FB_ERR("fake panel detect backlight ic \n");

		// register function for hisi
		hisi_blpwm_bl_register(lcdkit_backlight_common_set);
		// detect lcd backlight ic
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
	return 0;
}

struct hisi_fb_data_type lcdkit_fake_hisifd =
{
	.panel_probe = lcdkit_fake_probe,
};

static int lcdkit_fake_init(struct system_table* systable)
{
	int lcd_type;
	unsigned int productid = 0;
	struct lcd_type_operators* lcd_type_ops = NULL;
	char* panel_name = NULL;

	lcd_type_ops = get_operators(LCD_TYPE_MODULE_NAME_STR);
	if (!systable)
		HISI_FB_ERR("systable is null! \n");

	if (!lcd_type_ops)
	{
		HISI_FB_ERR("failed to get lcd type operator!\n");
	}
	else
	{
		lcd_type = lcd_type_ops->get_lcd_type();
		if  (lcd_type == LCDKIT)
		{
			if( strncmp(lcdkit_data.panel->compatible, LCDKIT_DTS_COMP_MIPI_FAKE_PANEL ,strlen(LCDKIT_DTS_COMP_MIPI_FAKE_PANEL)))
			{
				HISI_FB_ERR("the panel is buckled! \n");
				return 0;
			}

			panel_name = lcdkit_data.panel->panel_model ? lcdkit_data.panel->panel_model : lcdkit_data.panel->compatible;
			HISI_FB_ERR("productid = %d panel_name = %s.\n", productid, panel_name);
			lcd_type_ops->set_lcd_panel_type(lcdkit_data.panel->compatible);
			lcd_type_ops->set_hisifd(&lcdkit_fake_hisifd);
			memcpy(lcd_type_buf, "NO_LCD", 7);
		}
		else
		{
			HISI_FB_ERR("lcd type is not NORMAL_LCD.\n");
		}
	}

	return 0;
}

static struct module_data lcdkit_fake_module_data =
{
	.name = LCDKIT_FAKE_MODULE_NAME_STR,
	.level   = LCDKIT_FAKE_MODULE_LEVEL,
	.init = lcdkit_fake_init,
};

MODULE_INIT(LCDKIT_FAKE_MODULE_NAME, lcdkit_fake_module_data);
