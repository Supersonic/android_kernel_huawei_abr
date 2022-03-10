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
#include <boardid.h>
#include <oeminfo_ops.h>
#include "hisi_fb.h"
#include "lcd_kit_effect.h"
#include "lcd_kit_power.h"
#include "lcd_kit_utils.h"
#include "bias_bl_utils.h"
#include "bias_ic_common.h"
#include "lcd_kit_ext_disp.h"

static struct lcd_kit_disp_desc g_lcd_kit_disp_info[LCD_ACTIVE_PANEL_BUTT];
static struct hisi_panel_info lcd_pinfo[LCD_ACTIVE_PANEL_BUTT];
static int g_elvdd_gpio;

struct hisi_panel_info *lcd_kit_get_pinfo(void)
{
	return &lcd_pinfo[LCD_ACTIVE_PANEL];
}
struct lcd_kit_disp_desc *lcd_kit_get_disp_info(void)
{
	return &g_lcd_kit_disp_info[LCD_ACTIVE_PANEL];
}

static int32_t poweric_cmd_detect(struct hisi_fb_data_type *hisifd)
{
	uint8_t read_value = 0;
	int32_t ret;

	ret = lcd_kit_dsi_cmds_rx(hisifd, &read_value, sizeof(uint8_t),
		&disp_info->elvdd_detect.cmds);
	if (ret != 0) {
		LCD_KIT_ERR("mipi rx failed!\n");
		return LCD_KIT_OK;
	}
	if ((read_value & disp_info->elvdd_detect.exp_value_mask) !=
		disp_info->elvdd_detect.exp_value)
		ret = LCD_KIT_FAIL;

	LCD_KIT_INFO("read_value = 0x%x, exp_value = 0x%x, mask = 0x%x\n",
		read_value,
		disp_info->elvdd_detect.exp_value,
		disp_info->elvdd_detect.exp_value_mask);
	return ret;
}

static int32_t poweric_gpio_detect(void)
{
	uint32_t gpio_value;
	int32_t ret = LCD_KIT_OK;
	struct gpio_operators *gpio_ops = NULL;

	gpio_ops = get_operators(GPIO_MODULE_NAME_STR);
	if (!gpio_ops) {
		LCD_KIT_ERR("gpio_ops is null!\n");
		return LCD_KIT_FAIL;
	}
	gpio_ops->set_direction_input(disp_info->elvdd_detect.detect_gpio);
	gpio_value = gpio_ops->get_value(disp_info->elvdd_detect.detect_gpio);
	if ((disp_info->elvdd_detect.detect_gpio_type == EXTEND_GPIO_TYPE) &&
		(disp_info->elvdd_detect.exp_value > 0)) {
		disp_info->elvdd_detect.exp_value = 1; /* gpio value(High level) */
		LCD_KIT_INFO("set expect gpio_value to high level\n");
	}
	if (gpio_value != disp_info->elvdd_detect.exp_value)
		ret = LCD_KIT_FAIL;
	LCD_KIT_INFO("gpio_value = %d\n", gpio_value);
	return ret;
}

static void lcd_kit_poweric_detect(struct hisi_fb_data_type *hisifd)
{
	int32_t ret = LCD_KIT_OK;
	struct display_operators *display_type_ops = NULL;
	static uint32_t retry_times;

	if (!disp_info->elvdd_detect.support)
		return;
	if (retry_times >= RECOVERY_TIMES) {
		LCD_KIT_WARNING("not need recovery, recovery num:%d\n",
			retry_times);
		retry_times = 0;
		return;
	}
	display_type_ops = get_operators(DISPLAY_MODULE_NAME_STR);
	if (!display_type_ops) {
		LCD_KIT_ERR("failed to get lcd type operator!\n");
		return;
	}
	lcd_kit_delay(disp_info->elvdd_detect.delay, LCD_KIT_WAIT_MS);
	if (disp_info->elvdd_detect.detect_type == ELVDD_MIPI_CHECK_MODE)
		ret = poweric_cmd_detect(hisifd);
	else if ((disp_info->elvdd_detect.detect_type == ELVDD_GPIO_CHECK_MODE) ||
		(disp_info->elvdd_detect.detect_type == ELVDD_GPIO_CHECK_MODE_AP) ||
		(disp_info->elvdd_detect.detect_type == ELVDD_SH_MIPI_AP_GPIO_CHECK_MODE))
		ret = poweric_gpio_detect();
	if (ret) {
		LCD_KIT_ERR("detect poweric abnomal, recovery lcd\n");
		retry_times++;
		hisi_set_display_status(HISI_DISPLAY_ON_STATUS);
		if (display_type_ops->display_off)
			display_type_ops->display_off();
		if (display_type_ops->display_on)
			display_type_ops->display_on();
		return;
	}
	LCD_KIT_INFO("detect poweric nomal\n");
}

static int lcd_kit_panel_on(struct hisi_fb_panel_data *pdata,
	struct hisi_fb_data_type *hisifd)
{
	struct hisi_panel_info *pinfo = NULL;
	int ret = LCD_KIT_OK;

	if ((!hisifd) || (!pdata)) {
		LCD_KIT_ERR("hisifd or pdata is NULL!\n");
		return LCD_KIT_FAIL;
	}
	LCD_KIT_INFO("fb%d, +!\n", hisifd->index);
	pinfo = hisifd->panel_info;
	if (!pinfo) {
		LCD_KIT_ERR("panel_info is NULL!\n");
		return LCD_KIT_FAIL;
	}

	switch (pinfo->lcd_init_step) {
	case LCD_INIT_POWER_ON:
		ret = common_ops->panel_power_on(hisifd);
		pinfo->lcd_init_step = LCD_INIT_MIPI_LP_SEND_SEQUENCE;
		break;
	case LCD_INIT_MIPI_LP_SEND_SEQUENCE:
		ret = common_ops->panel_on_lp(hisifd);
		lcd_kit_get_tp_color(hisifd);
		lcd_kit_get_elvss_info(hisifd);
		lcd_kit_panel_version_init(hisifd);
		lcd_kit_aod_extend_cmds_init(hisifd);
		lcd_kit_parse_sn_check(hisifd, disp_info->lcd_name);
		/* read project id */
		if (lcd_kit_read_project_id(hisifd))
			LCD_KIT_ERR("read project id error\n");
		if (disp_info->brightness_color_uniform.support)
			lcd_kit_bright_rgbw_id_from_oeminfo(hisifd);
		lcd_kit_check_otp(hisifd);
		pinfo->lcd_init_step = LCD_INIT_MIPI_HS_SEND_SEQUENCE;
		break;
	case LCD_INIT_MIPI_HS_SEND_SEQUENCE:
		ret = common_ops->panel_on_hs(hisifd);
		lcd_kit_poweric_detect(hisifd);
		break;
	case LCD_INIT_NONE:
		break;
	case LCD_INIT_LDI_SEND_SEQUENCE:
		break;
	default:
		break;
	}
	LCD_KIT_INFO("fb%d, -!\n", hisifd->index);
	return ret;
}

static int lcd_kit_panel_off(struct hisi_fb_panel_data *pdata,
	struct hisi_fb_data_type *hisifd)
{
	struct hisi_panel_info *pinfo = NULL;
	int ret = LCD_KIT_OK;

	if ((!hisifd) || (!pdata)) {
		LCD_KIT_ERR("hisifd or pdata is NULL!\n");
		return LCD_KIT_FAIL;
	}
	LCD_KIT_INFO("fb%d, +!\n", hisifd->index);
	pinfo = hisifd->panel_info;
	if (!pinfo) {
		LCD_KIT_ERR("panel_info is NULL!\n");
		return LCD_KIT_FAIL;
	}
	if ((pinfo->product_type == LCD_FOLDER_TYPE) &&
		(pinfo->skip_power_on_off == true)) {
		LCD_KIT_INFO("skip panel off!\n");
		return LCD_KIT_OK;
	}
	switch (pinfo->lcd_uninit_step) {
	case LCD_UNINIT_MIPI_HS_SEND_SEQUENCE:
		ret = common_ops->panel_off_hs(hisifd);
		pinfo->lcd_uninit_step = LCD_UNINIT_MIPI_LP_SEND_SEQUENCE;
		break;
	case LCD_UNINIT_MIPI_LP_SEND_SEQUENCE:
		ret = common_ops->panel_off_lp(hisifd);
		pinfo->lcd_uninit_step = LCD_UNINIT_POWER_OFF;
		break;
	case LCD_UNINIT_POWER_OFF:
		ret = common_ops->panel_power_off(hisifd);
		break;
	default:
		break;
	}
	LCD_KIT_INFO("fb%d, -!\n", hisifd->index);
	return ret;
}

static int lcd_kit_set_backlight(struct hisi_fb_panel_data *pdata,
	struct hisi_fb_data_type *hisifd, uint32_t bl_level)
{
	uint32_t bl_type;
	int ret = LCD_KIT_OK;
	struct hisi_panel_info *pinfo = NULL;

	if ((!hisifd) || (!pdata)) {
		LCD_KIT_ERR("hisifd or pdata is NULL!\n");
		return LCD_KIT_FAIL;
	}
	/* quickly sleep */
	if (disp_info->quickly_sleep_out.support) {
		if (disp_info->quickly_sleep_out.interval > 0)
			mdelay(disp_info->quickly_sleep_out.interval);
	}
	pinfo = hisifd->panel_info;
	/* mapping bl_level from bl_max to 255 step */
	bl_level = bl_level * pinfo->bl_max / 255;
	bl_type = lcd_kit_get_backlight_type(pinfo);
	switch (bl_type) {
	case BL_SET_BY_PWM:
		ret = lcd_kit_pwm_set_backlight(hisifd, bl_level);
		break;
	case BL_SET_BY_BLPWM:
		ret = lcd_kit_blpwm_set_backlight(hisifd, bl_level);
		break;
	case BL_SET_BY_SH_BLPWM:
		ret = lcd_kit_sh_blpwm_set_backlight(hisifd, bl_level);
		break;
	case BL_SET_BY_MIPI:
		ret = lcd_kit_set_mipi_backlight(hisifd, bl_level);
		break;
	default:
		break;
	}
	LCD_KIT_INFO("bl_level = %d, bl_type = %d\n", bl_level, bl_type);
	return ret;
}

/* panel data */
static struct hisi_fb_panel_data lcd_kit_panel_data = {
	.on               = lcd_kit_panel_on,
	.off              = lcd_kit_panel_off,
	.set_backlight    = lcd_kit_set_backlight,
	.panel_switch     = lcd_kit_panel_switch,
	.get_panel_info   = lcd_kit_get_active_panel_info,
	.next             = NULL,
};

void *lcd_fdt = NULL;
struct fdt_operators *lcd_fdt_ops = NULL;

void lcd_fdt_open(void)
{
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	return;
#else
	lcd_fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (lcd_fdt_ops == NULL) {
		LCD_KIT_ERR("can't get fdt operators\n");
		return;
	}
	lcd_fdt = lcd_fdt_ops->get_dt_handle(FW_DTB_TYPE);
	if (lcd_fdt == NULL) {
		LCD_KIT_ERR("fdt is null!!!\n");
		return;
	}
#endif
}

static int lcd_kit_probe(struct hisi_fb_data_type *hisifd)
{
	struct hisi_panel_info *pinfo = NULL;
	int ret = LCD_KIT_OK;

	if (!hisifd) {
		LCD_KIT_ERR("hisifd is NULL!\n");
		return LCD_KIT_FAIL;
	}
	LCD_KIT_INFO(" enter\n");
	lcd_kit_ext_panel_probe();
	lcd_kit_panel_switch(LCD_MAIN_PANEL);
	/* init lcd panel info */
	pinfo = lcd_kit_get_pinfo();
	memset_s(pinfo, sizeof(struct hisi_panel_info), 0, sizeof(struct hisi_panel_info));
	pinfo->disp_cnnt_type = LCD_MAIN_PANEL;
#if !defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) && !defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	lcd_fdt_open();
#endif
	/* common init */
	if (common_ops->common_init)
		common_ops->common_init(disp_info->lcd_name);
	/* utils init */
	lcd_kit_utils_init(pinfo);
	/* elvdd gpio detect */
	LCD_KIT_INFO("XML_bootable elvdd detect gpio = %d\n", disp_info->elvdd_detect.detect_gpio);
	if (g_elvdd_gpio)
		disp_info->elvdd_detect.detect_gpio = g_elvdd_gpio;
	LCD_KIT_INFO("use elvdd detect gpio %d\n", disp_info->elvdd_detect.detect_gpio);
	/* panel init */
	lcd_kit_panel_init();
	/* power init */
	lcd_kit_power_init();
	/* bias ic common driver init */
	bias_bl_ops_init();
	ret = bias_ic_init();
	if (ret < 0)
		LCD_KIT_INFO("bias ic common init fail\n");
	/* panel chain */
	hisifd->panel_info = pinfo;
	lcd_kit_panel_data.next = hisifd->panel_data;
	hisifd->panel_data = &lcd_kit_panel_data;
	/* add device */
	hisi_fb_add_device(hisifd);
	LCD_KIT_INFO(" exit\n");
	return ret;
}

struct hisi_fb_data_type lcd_kit_hisifd = {
	.panel_probe = lcd_kit_probe,
};

static void transfer_power_config(u32 *in, int inlen,
	struct lcd_kit_array_data *out)
{
	u32 *buf = NULL;
	u8 i;

	if ((in == NULL) || (out == NULL)) {
		LCD_KIT_ERR("param invalid!\n");
		return;
	}
	if ((inlen <= 0) || (inlen > LCD_POWER_LEN)) {
		LCD_KIT_ERR("inlen <= 0 or > LCD_POWER_LEN!\n");
		return;
	}
	buf = (u32 *)alloc(inlen * sizeof(u32));
	if (!buf) {
		LCD_KIT_ERR("alloc buf fail\n");
		return;
	}
	for (i = 0; i < inlen; i++)
		buf[i] = in[i];
	out->buf = buf;
	out->cnt = inlen;
}

static void get_power_config_from_dts(struct lcd_type_operators *ops,
	char *str, struct lcd_kit_array_data *out)
{
	int ret;
	uint32_t power[LCD_POWER_LEN] = {0};

	if ((ops == NULL) || (str == NULL) || (out == NULL)) {
		LCD_KIT_ERR("input param is NULL\n");
		return;
	}
	ret = ops->get_power_by_str(str, power, LCD_POWER_LEN);
	if (ret == LCD_KIT_OK) {
		transfer_power_config(power, LCD_POWER_LEN, out);
		if (out->buf != NULL)
			LCD_KIT_INFO("%s 0x%x, 0x%x, 0x%x\n", str, out->buf[0],
				out->buf[1], out->buf[2]);
	}
}

static void get_power_config_by_str(struct lcd_type_operators *ops)
{
	if (ops == NULL) {
		LCD_KIT_ERR("ops is NULL\n");
		return;
	}
	get_power_config_from_dts(ops, "lcd_vci", &power_hdl->lcd_vci);
	get_power_config_from_dts(ops, "lcd_iovcc", &power_hdl->lcd_iovcc);
	get_power_config_from_dts(ops, "lcd_vdd", &power_hdl->lcd_vdd);
	get_power_config_from_dts(ops, "lcd_vsp", &power_hdl->lcd_vsp);
	get_power_config_from_dts(ops, "lcd_vsn", &power_hdl->lcd_vsn);
	get_power_config_from_dts(ops, "lcd_aod", &power_hdl->lcd_aod);
	get_power_config_from_dts(ops, "lcd_rst", &power_hdl->lcd_rst);
	get_power_config_from_dts(ops, "lcd_te0", &power_hdl->lcd_te0);
}

static void add_status_disabled()
{
#ifdef FASTBOOT_FW_DTB_ENABLE
	struct fdt_operators *fdt_ops = NULL;
#endif

	LCD_KIT_INFO("+\n");
#ifdef FASTBOOT_FW_DTB_ENABLE
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (fdt_ops == NULL) {
		LCD_KIT_ERR("fdt_ops is null\n");
		return;
	}
	fdt_ops->register_update_dts_func(lcd_add_status_disabled);
#else
	(void)lcd_add_status_disabled();
#endif
	LCD_KIT_INFO("-\n");
}

static int lcd_kit_init(struct system_table *systable)
{
	int pin_num = 0;
	struct lcd_type_operators *lcd_type_ops = NULL;

	lcd_type_ops = get_operators(LCD_TYPE_MODULE_NAME_STR);
	if (!lcd_type_ops) {
		LCD_KIT_ERR("failed to get lcd type operator!\n");
		return LCD_KIT_FAIL;
	}
	if (!systable)
		LCD_KIT_ERR("failed to get lcd type operator!\n");
	LCD_KIT_INFO("lcd_type = %d\n", lcd_type_ops->get_lcd_type());
	if (lcd_type_ops->get_lcd_type() == LCD_KIT) {
		LCD_KIT_INFO("lcd type is LCD_KIT\n");
		/* init lcd id */
		disp_info->lcd_id = lcd_type_ops->get_lcd_id(&pin_num);
		disp_info->product_id = lcd_type_ops->get_product_id();
		disp_info->compatible = lcd_kit_get_compatible(disp_info->product_id, disp_info->lcd_id, pin_num);
		disp_info->lcd_name = lcd_kit_get_lcd_name(disp_info->product_id, disp_info->lcd_id, pin_num);
		add_status_disabled();
		lcd_type_ops->set_lcd_panel_type(disp_info->compatible);
		lcd_type_ops->set_hisifd(&lcd_kit_hisifd);
		get_power_config_by_str(lcd_type_ops);
		/* adapt init */
		lcd_kit_adapt_init();
		LCD_KIT_INFO("disp_info->lcd_id = %d, disp_info->product_id = %d\n",
			disp_info->lcd_id, disp_info->product_id);
		LCD_KIT_DEBUG("disp_info->lcd_name = %s, disp_info->compatible = %s\n",
			disp_info->lcd_name, disp_info->compatible);
		/* elvdd gpio detect */
		g_elvdd_gpio = lcd_type_ops->get_elvdd_detect_gpio();
		LCD_KIT_INFO("DTS_bootable elvdd detect gpio = %d\n", g_elvdd_gpio);
		lcd_kit_ext_panel_init();
	} else {
		LCD_KIT_INFO("lcd type is not LCD_KIT\n");
	}
	return LCD_KIT_OK;
}

static struct module_data lcd_kit_module_data = {
	.name = LCD_KIT_MODULE_NAME_STR,
	.level   = LCDKIT_MODULE_LEVEL,
	.init = lcd_kit_init,
};

MODULE_INIT(LCD_KIT_MODULE_NAME, lcd_kit_module_data);
