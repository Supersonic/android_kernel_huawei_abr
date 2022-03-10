/*
 * lcd_kit_displayengine.c
 *
 * diplay engine common function in lcd
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
 *
 *
 */

#include <linux/backlight.h>
#include <linux/device.h>
#include <linux/workqueue.h>
#include <sde_drm.h>
#include <huawei_platform/fingerprint_interface/fingerprint_interface.h>
#include "lcd_kit_displayengine.h"
#include "lcd_kit_adapt.h"
#include "lcd_kit_common.h"
#include "lcd_kit_utils.h"
#include "display_engine_kernel.h"
#include "lcd_kit_drm_panel.h"
#include "sde_trace.h"

#if defined(CONFIG_DPU_FB_V501) || defined(CONFIG_DPU_FB_V330) || \
	defined(CONFIG_DPU_FB_V350) || defined(CONFIG_DPU_FB_V510) || \
	defined(CONFIG_DPU_FB_V600)
#include "lcd_kit_disp.h"
#define CONFIG_DE_XCC 1
#else
/* The same define with lcd_kit_disp.h */
#define LCD_KIT_ENABLE_ELVSSDIM  0
#define LCD_KIT_DISABLE_ELVSSDIM 1
#define LCD_KIT_ELVSSDIM_NO_WAIT 0
#endif

#define BACKLIGHT_HIGH_LEVEL 1
#define BACKLIGHT_LOW_LEVEL  2
#define MAX_DBV 4095
#define FINGERPRINT_HBM_NOTIFY_WAIT_FRAME_COUNT 2
#define SN_TEXT_SIZE 128

enum te_interval_type {
	TE_INTERVAL_60_FPS_US = 16667,
	TE_INTERVAL_90_FPS_US = 11111,
	TE_INTERVAL_120_FPS_US = 8334
};

/* frame rate type */
enum fps_type {
	FPS_LEVEL1 = 60,
	FPS_LEVEL2 = 90,
	FPS_LEVEL3 = 120
};

/* Structure types for different features, you can add your type after de_context_drm_hist. */
/* DRM modules: */
struct de_context_drm_hist {
	bool is_enable;
	bool is_enable_change;
};

/* Displayengine brightness related parameters */
struct de_context_brightness {
	unsigned int mode_set_by_ioctl;
	unsigned int mode_in_use;
	bool is_alpha_updated_at_mode_change;
	struct display_engine_brightness_lut lut[DISPLAY_ENGINE_MODE_MAX];
	unsigned int last_level_in;
	unsigned int last_level_out;
	unsigned int current_alpha;
	unsigned int last_alpha;
	bool is_alpha_bypass;
	ktime_t te_timestamp;
	ktime_t te_timestamp_last;
	/* UD fingerprint backlight */
	struct display_engine_fingerprint_backlight fingerprint_backlight;
};

/* Context for all display engine features, you can add your data after drm_hist. */
struct display_engine_context {
	bool is_inited;
	/* DRM modules */
	struct de_context_drm_hist drm_hist;
	/* Brightness */
	struct de_context_brightness brightness;
};

static struct display_engine_context g_de_context[DISPLAY_ENGINE_PANEL_COUNT] = {
	{ /* Inner panel or primay panel */
		.is_inited = false,
		.drm_hist.is_enable = false,
		.drm_hist.is_enable_change = false,
		.brightness.mode_in_use = 0,
		.brightness.mode_set_by_ioctl = 0,
		.brightness.is_alpha_updated_at_mode_change = false,
		.brightness.last_level_in = MAX_DBV,
		.brightness.last_level_out = MAX_DBV,
		.brightness.current_alpha = ALPHA_DEFAULT,
		.brightness.last_alpha = ALPHA_DEFAULT,
		.brightness.is_alpha_bypass = false,
		.brightness.te_timestamp = 0,
		.brightness.te_timestamp_last = 0,
		.brightness.fingerprint_backlight.scene_info = 0,
		.brightness.fingerprint_backlight.hbm_level = MAX_DBV,
		.brightness.fingerprint_backlight.current_level = MAX_DBV,
	},
	{ /* Outer panel */
		.is_inited = false,
		.drm_hist.is_enable = false,
		.drm_hist.is_enable_change = false,
		.brightness.mode_in_use = 0,
		.brightness.mode_set_by_ioctl = 0,
		.brightness.is_alpha_updated_at_mode_change = false,
		.brightness.last_level_in = MAX_DBV,
		.brightness.last_level_out = MAX_DBV,
		.brightness.current_alpha = ALPHA_DEFAULT,
		.brightness.last_alpha = ALPHA_DEFAULT,
		.brightness.is_alpha_bypass = false,
		.brightness.te_timestamp = 0,
		.brightness.te_timestamp_last = 0,
		.brightness.fingerprint_backlight.scene_info = 0,
		.brightness.fingerprint_backlight.hbm_level = MAX_DBV,
		.brightness.fingerprint_backlight.current_level = MAX_DBV,
	},
};

static int g_fingerprint_hbm_level = MAX_DBV;
static int g_fingerprint_backlight_type = BACKLIGHT_LOW_LEVEL;
static int g_fingerprint_backlight_type_real = BACKLIGHT_LOW_LEVEL;

static int display_engine_panel_id_to_de(enum lcd_active_panel_id lcd_id)
{
	switch (lcd_id) {
	case PRIMARY_PANEL:
		return DISPLAY_ENGINE_PANEL_INNER;
	case SECONDARY_PANEL:
		return DISPLAY_ENGINE_PANEL_OUTER;
	default:
		LCD_KIT_WARNING("unknown lcd kit panel id [%d]!\n", lcd_id);
	}
	return DISPLAY_ENGINE_PANEL_INNER;
}

static uint32_t display_engine_panel_id_to_lcdkit(uint32_t de_id)
{
	switch (de_id) {
	case DISPLAY_ENGINE_PANEL_INNER:
		return PRIMARY_PANEL;
	case DISPLAY_ENGINE_PANEL_OUTER:
		return SECONDARY_PANEL;
	default:
		LCD_KIT_WARNING("unknown display engine panel id [%d]!\n", de_id);
		break;
	}
	return PRIMARY_PANEL;
}

static struct display_engine_context *get_de_context(uint32_t panel_id)
{
	if (panel_id >= DISPLAY_ENGINE_PANEL_COUNT) {
		LCD_KIT_ERR("Invalid input: panel_id=%u\n", panel_id);
		return NULL;
	}
	return &g_de_context[panel_id];
}

uint32_t get_hbm_level(uint32_t panel_id)
{
	struct display_engine_context *de_context = get_de_context(panel_id);

	if (!de_context) {
		LCD_KIT_ERR("de_context is null\n");
		return MAX_DBV;
	}
	return de_context->brightness.fingerprint_backlight.hbm_level;
}

static void display_engine_brightness_update_backlight(void)
{
	struct backlight_device *bd;

	/* 0 refers to the first display */
	bd = backlight_device_get_by_type_and_display_count(BACKLIGHT_RAW, 0);
	if (!bd) {
		LCD_KIT_ERR("bd is null\n");
		return;
	}
	SDE_ATRACE_BEGIN("backlight_update_status");
	backlight_update_status(bd);
	SDE_ATRACE_END("backlight_update_status");
}

static int display_engine_brightness_get_backlight(void)
{
	struct backlight_device *bd;

	/* 0 refers to the first display */
	bd = backlight_device_get_by_type_and_display_count(BACKLIGHT_RAW, 0);
	if (!bd) {
		LCD_KIT_ERR("bd is null\n");
		return MAX_DBV;
	}
	return bd->props.brightness;
}

static void display_engine_brightness_handle_fingerprint_hbm_work(struct work_struct *work)
{
	struct qcom_panel_info *panel_info = container_of(work,
		struct qcom_panel_info, backlight_work);
	struct dsi_panel *panel = NULL;
	uint32_t panel_id;

	if (!panel_info || !panel_info->display || !panel_info->display->panel) {
		LCD_KIT_ERR("panel_info is NULL!\n");
		return;
	}

	panel = panel_info->display->panel;
	panel_id = lcd_kit_get_current_panel_id(panel);

	LCD_KIT_INFO("work start, panel_id %u, hbm_level %d, type %d\n",
		panel_id, g_fingerprint_hbm_level, g_fingerprint_backlight_type);
	display_engine_brightness_update_backlight();
	LCD_KIT_INFO("work done, hbm_level %d, type %d\n",
		g_fingerprint_hbm_level, g_fingerprint_backlight_type);
	g_fingerprint_backlight_type_real = g_fingerprint_backlight_type;
}

static void display_engine_brightness_print_power_mode(int mode)
{
	switch (mode) {
	case SDE_MODE_DPMS_ON:
		LCD_KIT_DEBUG("sde power mode [SDE_MODE_DPMS_ON]");
		break;
	case SDE_MODE_DPMS_LP1:
		LCD_KIT_DEBUG("bd->props.power [SDE_MODE_DPMS_LP1]");
		break;
	case SDE_MODE_DPMS_LP2:
		LCD_KIT_DEBUG("sde power mode [SDE_MODE_DPMS_LP2]");
		break;
	case SDE_MODE_DPMS_STANDBY:
		LCD_KIT_DEBUG("sde power mode [SDE_MODE_DPMS_STANDBY]");
		break;
	case SDE_MODE_DPMS_SUSPEND:
		LCD_KIT_DEBUG("sde power mode [SDE_MODE_DPMS_SUSPEND]");
		break;
	case SDE_MODE_DPMS_OFF:
		LCD_KIT_DEBUG("sde power mode [SDE_MODE_DPMS_OFF]");
		break;
	default:
		LCD_KIT_WARNING("sde power mode unknown value [%d]", mode);
	}
}

static int display_engine_brightness_get_power_mode(void)
{
	struct qcom_panel_info *panel_info = lcm_get_panel_info(PRIMARY_PANEL);

	if (!panel_info || !panel_info->display || !panel_info->display->panel) {
		LCD_KIT_ERR("panel_info is NULL!\n");
		return SDE_MODE_DPMS_ON;
	}
	display_engine_brightness_print_power_mode(panel_info->display->panel->power_mode);
	return panel_info->display->panel->power_mode;
}

static void display_engine_brightness_handle_vblank_work(struct work_struct *work)
{
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);
	ktime_t te_diff_ms = 0;
	static u32 vblank_count;

	(void)work;
	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return;
	}
	/* ns to ms */
	te_diff_ms = (de_context->brightness.te_timestamp -
		de_context->brightness.te_timestamp_last) / 1000000;

	LCD_KIT_DEBUG("te: diff [%lld] ms, timestamp [%lld]\n",
		te_diff_ms, de_context->brightness.te_timestamp);

	/* Handle fingerprint hbm notification */
	if (g_fingerprint_backlight_type_real == BACKLIGHT_HIGH_LEVEL &&
		display_engine_brightness_should_do_fingerprint()) {
		vblank_count++;
		if (vblank_count == FINGERPRINT_HBM_NOTIFY_WAIT_FRAME_COUNT) {
			LCD_KIT_INFO("Fingerprint HBM notify...\n");
			ud_fp_on_hbm_completed();
		}
	} else {
		vblank_count = 0;
	}
}

static void display_engine_brightness_handle_backlight_update(struct work_struct *work)
{
	(void)work;
	display_engine_brightness_update_backlight();
}

static void displayengine_set_max_mipi_backlight(uint32_t panel_id, void *hld)
{
#if defined(CONFIG_DE_XCC)
	struct hisi_fb_data_type *hisifd = hld;
	lcd_kit_get_common_ops()->set_mipi_backlight(panel_id, hld,
		hisifd->panel_info.bl_max);
#endif
	return;
}

static void displayengine_clear_xcc(void *hld)
{
#if defined(CONFIG_DE_XCC)
	struct hisi_fb_data_type *hisifd = hld;
		/*
		 * clear XCC config(include XCC disable state)
		 * while fp is using
		 */
		hisifd->mask_layer_xcc_flag = 1;
		clear_xcc_table(hld);
#endif
	return;
}

static void displayengine_restore_xcc(void *hld)
{
#if defined(CONFIG_DE_XCC)
	struct hisi_fb_data_type *hisifd = hld;
		/*
		 * restore XCC config(include XCC enable state)
		 * while fp is end
		 */
		restore_xcc_table(hisifd);
		hisifd->mask_layer_xcc_flag = 0;
#endif
	return;
}

static int lcd_kit_set_elvss_dim_fp(uint32_t panel_id, void *hld,
	struct lcd_kit_adapt_ops *adapt_ops, int disable_elvss_dim)
{
	int ret = LCD_KIT_OK;

	if (!hld || !adapt_ops) {
		LCD_KIT_ERR("param is NULL!\n");
		return LCD_KIT_FAIL;
	}
	if (!common_info->hbm.hbm_fp_elvss_support) {
		LCD_KIT_INFO("Do not support ELVSS Dim, just return\n");
		return ret;
	}
	if (common_info->hbm.elvss_prepare_cmds.cmds) {
		if (adapt_ops->mipi_tx)
			ret = adapt_ops->mipi_tx(hld, &common_info->hbm.elvss_prepare_cmds);
	}
	if (common_info->hbm.elvss_write_cmds.cmds) {
		if (!adapt_ops->mipi_tx)
			return LCD_KIT_FAIL;
#if defined(CONFIG_DE_XCC)
		if (disable_elvss_dim != 0)
			disable_elvss_dim_write(panel_id);
		else
			enable_elvss_dim_write(panel_id);
#endif
		ret = adapt_ops->mipi_tx(hld, &common_info->hbm.elvss_write_cmds);
		LCD_KIT_INFO("[fp set elvss dim] send:0x%x\n",
			common_info->hbm.elvss_write_cmds.cmds[0].payload[1]);
	}
	if (common_info->hbm.elvss_post_cmds.cmds) {
		if (adapt_ops->mipi_tx)
			ret = adapt_ops->mipi_tx(hld, &common_info->hbm.elvss_post_cmds);
	}
	return ret;
}

static bool lcd_kit_fps_is_high(uint32_t panel_id)
{
#if defined(CONFIG_DE_XCC)
	return disp_info->fps.last_update_fps == LCD_KIT_FPS_HIGH;
#endif
	return false;
}

static int lcd_kit_enter_hbm_fb(uint32_t panel_id, void *hld, struct lcd_kit_adapt_ops *adapt_ops)
{
	int ret = LCD_KIT_OK;

	if (!hld || !adapt_ops) {
		LCD_KIT_ERR("param is NULL!\n");
		return LCD_KIT_FAIL;
	}

	if (lcd_kit_fps_is_high(panel_id) && common_info->dfr_info.fps_lock_command_support) {
		ret = adapt_ops->mipi_tx(hld, &common_info->dfr_info.cmds[FPS_90_HBM_NO_DIM]);
		LCD_KIT_INFO("fp enter hbm when 90hz\n");
	} else if (common_info->hbm.fp_enter_cmds.cmds) {
		if (adapt_ops->mipi_tx)
			ret = adapt_ops->mipi_tx(hld, &common_info->hbm.fp_enter_cmds);
		LCD_KIT_INFO("fp enter hbm\n");
	}
	return ret;
}

static int lcd_kit_enter_fp_hbm(uint32_t panel_id, void *hld, uint32_t level)
{
	int ret = LCD_KIT_OK;
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	if (!hld) {
		LCD_KIT_ERR("hld is NULL!\n");
		return LCD_KIT_FAIL;
	}
	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not register adapt_ops!\n");
		return LCD_KIT_FAIL;
	}
	if (lcd_kit_set_elvss_dim_fp(panel_id, hld, adapt_ops,
		LCD_KIT_DISABLE_ELVSSDIM) < 0)
		LCD_KIT_ERR("set_elvss_dim_fp: disable failed!\n");
	if (lcd_kit_enter_hbm_fb(panel_id, hld, adapt_ops) < 0)
		LCD_KIT_ERR("enter_hbm_fb: enable hbm failed!\n");
	if (lcd_kit_get_common_ops()->hbm_set_level(panel_id, hld, level) < 0)
		LCD_KIT_ERR("hbm_set_level: set level failed!\n");
	return ret;
}

static int lcd_kit_disable_hbm_fp(uint32_t panel_id, void *hld, struct lcd_kit_adapt_ops *adapt_ops)
{
	int ret = LCD_KIT_OK;

	if (!hld || !adapt_ops) {
		LCD_KIT_ERR("param is NULL!\n");
		return LCD_KIT_FAIL;
	}

	if (lcd_kit_fps_is_high(panel_id) &&
		common_info->dfr_info.fps_lock_command_support) {
		ret = adapt_ops->mipi_tx(hld, &common_info->dfr_info.cmds[FPS_90_NORMAL_NO_DIM]);
		LCD_KIT_INFO("fp exit hbm no dim with 90hz\n");
	} else if (common_info->hbm.exit_dim_cmds.cmds) {
		if (adapt_ops->mipi_tx)
			ret = adapt_ops->mipi_tx((void *)hld, &common_info->hbm.exit_dim_cmds);
		LCD_KIT_INFO("fp exit dim\n");
	}
	if (common_info->hbm.exit_cmds.cmds) {
		if (adapt_ops->mipi_tx)
			ret = adapt_ops->mipi_tx((void *)hld, &common_info->hbm.exit_cmds);
		LCD_KIT_INFO("fp exit hbm\n");
	}

	return ret;
}

static int lcd_kit_exit_fp_hbm(uint32_t panel_id, void *hld, uint32_t level)
{
	int ret = LCD_KIT_OK;
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	if (!hld) {
		LCD_KIT_ERR("hld is NULL!\n");
		return LCD_KIT_FAIL;
	}
	adapt_ops = lcd_kit_get_adapt_ops();
	if (level > 0) {
		if (lcd_kit_get_common_ops()->hbm_set_level(panel_id, hld, level) < 0)
			LCD_KIT_ERR("hbm_set_level: set level failed!\n");
	} else {
		if (lcd_kit_disable_hbm_fp(panel_id, hld, adapt_ops) < 0)
			LCD_KIT_ERR("disable hbm failed!\n");
	}
	if (lcd_kit_set_elvss_dim_fp(panel_id, hld, adapt_ops,
		LCD_KIT_ENABLE_ELVSSDIM) < 0)
		LCD_KIT_ERR("set_elvss_dim_fp: enable failed!\n");
	return ret;
}

int lcd_kit_set_fp_backlight(uint32_t panel_id, void *hld, u32 level, u32 backlight_type) {
	int ret = LCD_KIT_OK;

	if (!hld) {
		LCD_KIT_ERR("hld is NULL!\n");
		return LCD_KIT_FAIL;
	}
	switch (backlight_type) {
	case BACKLIGHT_HIGH_LEVEL:
		common_info->hbm.hbm_if_fp_is_using = 1;
		LCD_KIT_DEBUG("bl_type=%d, level=%d\n", backlight_type, level);
		/*
		 * Below codes will write level to set fp backlight.
		 * We suggest set hbm_fp_support TRUE to set backlight.
		 * If hbm_fp_support(<PanelHbmFpCaptureSupport>) is 1,
		 * it will write <PanelHbmFpPrepareCommand>,<PanelHbmCommand>,
		 * PanelHbmFpPostCommand>.
		 * Those cmds are also be used by hbm backlight.
		 * Or not, set_mipi_backlight will write <PanelBacklightCommand>
		 */
		if (common_info->hbm.hbm_fp_support) {
			displayengine_set_max_mipi_backlight(panel_id, hld);
			mutex_lock(&COMMON_LOCK->hbm_lock);
			lcd_kit_enter_fp_hbm(panel_id, hld, level);
			mutex_unlock(&COMMON_LOCK->hbm_lock);
		} else {
			LCD_KIT_DEBUG("set_mipi_backlight +\n");
			ret = lcd_kit_get_common_ops()->set_mipi_backlight(panel_id, hld,
				level);
			LCD_KIT_DEBUG("set_mipi_backlight -\n");
		}
		/*
		 * The function is only used for UD PrintFinger HBM
		 * It will write <PanelFpEnterExCommand> after write fp level.
		 */
		lcd_kit_fp_hbm_extern(panel_id, hld, BACKLIGHT_HIGH_LEVEL);
		displayengine_clear_xcc(hld);
		break;
	case BACKLIGHT_LOW_LEVEL:
		LCD_KIT_DEBUG("bl_type=%d  level=%d\n", backlight_type, level);
		/*
		 * The function is will restore hbm level.
		 * if <PanelHbmCommand> is same as <PanelBacklightCommand>
		 * it will be cover by set_mipi_backlight.
		 * If they are different,
		 * <PanelHbmCommand> write common_info->hbm.hbm_level_current,
		 * and <PanelBacklightCommand> write level by param,
		 * <PanelBacklightCommand> usually write 51h.
		 */
		if (common_info->hbm.hbm_fp_support)
			lcd_kit_restore_hbm_level(panel_id, hld);
		common_info->hbm.hbm_if_fp_is_using = 0;
		LCD_KIT_DEBUG("set_mipi_backlight +\n");
		ret = lcd_kit_get_common_ops()->set_mipi_backlight(panel_id, hld, level);
		LCD_KIT_DEBUG("set_mipi_backlight -\n");
		/*
		 * The function is only used for UD PrintFinger HBM
		 * It will write <PanelFpExitExCommand> after restore normal bl.
		 */
		lcd_kit_fp_hbm_extern(panel_id, hld, BACKLIGHT_LOW_LEVEL);
		displayengine_restore_xcc(hld);
		break;
	default:
		LCD_KIT_ERR("bl_type is not define(%d)\n", backlight_type);
		break;
	}
	LCD_KIT_DEBUG("bl_type=%d  level=%d\n", backlight_type, level);
	return ret;
}

#if defined(CONFIG_DE_XCC)
void disable_elvss_dim_write(uint32_t panel_id)
{
	common_info->hbm.elvss_write_cmds.cmds[0].wait =
		common_info->hbm.hbm_fp_elvss_cmd_delay;
	common_info->hbm.elvss_write_cmds.cmds[0].payload[1] =
		common_info->hbm.ori_elvss_val & LCD_KIT_DISABLE_ELVSSDIM_MASK;
}

void enable_elvss_dim_write(uint32_t panel_id)
{
	common_info->hbm.elvss_write_cmds.cmds[0].wait =
		LCD_KIT_ELVSSDIM_NO_WAIT;
	common_info->hbm.elvss_write_cmds.cmds[0].payload[1] =
		common_info->hbm.ori_elvss_val | LCD_KIT_ENABLE_ELVSSDIM_MASK;
}
#endif

int lcd_kit_fp_hbm_extern(uint32_t panel_id, void *hld, int type)
{
	int ret = LCD_KIT_OK;
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	if (!hld) {
		LCD_KIT_ERR("hld is NULL!\n");
		return LCD_KIT_FAIL;
	}
	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not register adapt_ops!\n");
		return LCD_KIT_FAIL;
	}

	switch (type) {
	case BACKLIGHT_HIGH_LEVEL:
		if (common_ops->fp_hbm_enter_extern)
			ret = common_ops->fp_hbm_enter_extern(panel_id, hld);
		break;
	case BACKLIGHT_LOW_LEVEL:
		if (common_ops->fp_hbm_exit_extern)
			ret = common_ops->fp_hbm_exit_extern(panel_id, hld);
		break;
	default:
		LCD_KIT_ERR("unknown case!\n");
		break;
	}
	if (ret != LCD_KIT_OK)
		LCD_KIT_ERR("lcd_kit_fp_hbm_extern fail!\n");
	return ret;
}

int lcd_kit_restore_hbm_level(uint32_t panel_id, void *hld)
{
	int ret = LCD_KIT_OK;

	if (!hld) {
		LCD_KIT_ERR("hld is NULL!\n");
		return ret;
	}
	if (common_info->hbm.hbm_fp_support)
		LCD_KIT_INFO("hbm_fp_support is not support!\n");
	mutex_lock(&COMMON_LOCK->hbm_lock);
	ret = lcd_kit_exit_fp_hbm(panel_id, hld, common_info->hbm.hbm_level_current);
	mutex_unlock(&COMMON_LOCK->hbm_lock);
	return ret;
}

bool display_engine_hist_is_enable(void)
{
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);

	if (!de_context) {
		LCD_KIT_ERR("de_context is null\n");
		return false;
	}
	LCD_KIT_DEBUG("is_enable:%d\n", de_context->drm_hist.is_enable);
	return de_context->drm_hist.is_enable;
}

bool display_engine_hist_is_enable_change(void)
{
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);
	bool is_change = false;

	if (!de_context) {
		LCD_KIT_ERR("de_context is null\n");
		return false;
	}
	if (is_change != de_context->drm_hist.is_enable_change) {
		is_change = de_context->drm_hist.is_enable_change;
		de_context->drm_hist.is_enable_change = false;
		LCD_KIT_INFO("is_enable=%d is_enable_change: %d -> %d\n",
			de_context->drm_hist.is_enable, is_change,
			de_context->drm_hist.is_enable_change);
	}
	LCD_KIT_DEBUG("is_enable=%d is_enable_change=%d\n",
		de_context->drm_hist.is_enable, de_context->drm_hist.is_enable_change);
	return is_change;
}

static uint64_t correct_time_based_on_fps(uint32_t real_te_interval, uint64_t time_60_fps)
{
	return time_60_fps * real_te_interval / TE_INTERVAL_60_FPS_US;
}

static uint64_t get_real_te_interval(uint32_t panel_id)
{
	if (disp_info->fps.current_fps == FPS_LEVEL3)
		return TE_INTERVAL_120_FPS_US;
	else if (disp_info->fps.current_fps == FPS_LEVEL2)
		return TE_INTERVAL_90_FPS_US;
	else
		return TE_INTERVAL_60_FPS_US;
}

static uint64_t get_backlight_sync_delay_time_us(struct qcom_panel_info *panel_info,
	struct display_engine_context *de_context, uint32_t panel_id)
{
	uint64_t left_thres_us;
	uint64_t right_thres_us;
	uint64_t te_interval;
	uint64_t diff_from_te_time;
	uint64_t current_frame_time;
	uint64_t delay_us;
	uint32_t real_te_interval;

	if (panel_info->finger_unlock_state_support && !panel_info->finger_unlock_state) {
		LCD_KIT_INFO("in screen off fingerprint unlock\n");
		delay_us = 0;
		return delay_us;
	}
	LCD_KIT_INFO("panel_info->mask_delay.left_time_to_te_us = %lu us,"
		"panel_info->mask_delay.right_time_to_te_us = %lu us,"
		"panel_info->mask_delay.te_interval_us = %lu us\n",
		panel_info->mask_delay.left_time_to_te_us,
		panel_info->mask_delay.right_time_to_te_us, panel_info->mask_delay.te_interval_us);
	real_te_interval = get_real_te_interval(panel_id);
	left_thres_us = correct_time_based_on_fps(real_te_interval,
		panel_info->mask_delay.left_time_to_te_us);
	right_thres_us = correct_time_based_on_fps(real_te_interval,
		panel_info->mask_delay.right_time_to_te_us);
	te_interval = correct_time_based_on_fps(real_te_interval,
		panel_info->mask_delay.te_interval_us);
	if (te_interval == 0) {
		LCD_KIT_ERR("te_interval is 0, used default time\n");
		te_interval = TE_INTERVAL_60_FPS_US;
	}
	diff_from_te_time = ktime_to_us(ktime_get()) -
		ktime_to_us(de_context->brightness.te_timestamp);

	/* when diff time is more than 10 times TE, FP maybe blink. */
	if (diff_from_te_time > real_te_interval) {
		usleep_range(real_te_interval, real_te_interval);
		diff_from_te_time = ktime_to_us(ktime_get()) - ktime_to_us(
			de_context->brightness.te_timestamp);
		LCD_KIT_INFO("delay 1 frame to wait te, te = %d, diff_from_te_time =%ld\n",
			real_te_interval, diff_from_te_time);
	}
	current_frame_time = diff_from_te_time - ((diff_from_te_time / te_interval) * te_interval);
	if (current_frame_time < left_thres_us)
		delay_us = left_thres_us - current_frame_time;
	else if (current_frame_time > right_thres_us)
		delay_us = te_interval - current_frame_time + left_thres_us;
	else
		delay_us = 0;
	LCD_KIT_INFO("backlight_sync left_thres_us = %lu us, right_thres_us = %lu us,"
		"current_frame_time = %lu us, diff_from_te_time = %lu us,"
		"te_interval = %lu us, delay = %lu us\n", left_thres_us, right_thres_us,
		current_frame_time, diff_from_te_time, te_interval, delay_us);

	return delay_us;
}

static void handle_fingerprint_panel_hbm_delay(struct qcom_panel_info *panel_info,
	uint32_t panel_id, int fingerprint_backlight_type)
{
	uint32_t mask_delay_time;
	uint32_t real_te_interval = get_real_te_interval(panel_id);

	if (fingerprint_backlight_type == BACKLIGHT_HIGH_LEVEL) {
		mask_delay_time = correct_time_based_on_fps(real_te_interval,
			panel_info->mask_delay.delay_time_before_fp);
	} else if (fingerprint_backlight_type == BACKLIGHT_LOW_LEVEL) {
		mask_delay_time = correct_time_based_on_fps(real_te_interval,
			panel_info->mask_delay.delay_time_after_fp);
	} else {
		LCD_KIT_ERR("invalid fingerprint_backlight_type, fingerprint_backlight_type = %d\n",
			fingerprint_backlight_type);
		mask_delay_time = 0;
	}
	LCD_KIT_INFO("real_te_interval:%d, delay_time_before_fp:%d,"
		"delay_time_after_fp:%d, mask_delay_time:%d\n",
		real_te_interval, panel_info->mask_delay.delay_time_before_fp,
		panel_info->mask_delay.delay_time_after_fp, mask_delay_time);
	if (mask_delay_time != 0)
		usleep_range(mask_delay_time, mask_delay_time);
}

int display_engine_fp_backlight_set_sync(uint32_t panel_id, u32 level, u32 backlight_type)
{
	struct qcom_panel_info *panel_info = lcm_get_panel_info(panel_id);
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);
	uint64_t fp_delay_time_us = 0;

	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return LCD_KIT_FAIL;
	}

	if (!panel_info) {
		LCD_KIT_ERR("panel_info is NULL!\n");
		return LCD_KIT_FAIL;
	}
	if (!de_context->is_inited) {
		LCD_KIT_ERR("de_context is not inited yet!\n");
		return LCD_KIT_FAIL;
	}

	/* Calculate the delay */
	fp_delay_time_us = get_backlight_sync_delay_time_us(panel_info, de_context, panel_id);
	if (fp_delay_time_us > 0)
		usleep_range(fp_delay_time_us, fp_delay_time_us);

	g_fingerprint_backlight_type = backlight_type;
	g_fingerprint_hbm_level = level;
	display_engine_brightness_update_backlight();
	handle_fingerprint_panel_hbm_delay(panel_info, panel_id, g_fingerprint_backlight_type);
	g_fingerprint_backlight_type_real = g_fingerprint_backlight_type;
	return LCD_KIT_OK;
}

void display_engine_brightness_set_alpha_bypass(bool is_bypass)
{
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);

	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return;
	}
	LCD_KIT_INFO("brightness.is_alpha_bypass [%u] -> [%u]\n",
		de_context->brightness.is_alpha_bypass, is_bypass);
	de_context->brightness.is_alpha_bypass = is_bypass;
}

bool display_engine_brightness_should_do_fingerprint(void)
{
	int power_mode = display_engine_brightness_get_power_mode();

	display_engine_brightness_print_power_mode(power_mode);
	return (power_mode != SDE_MODE_DPMS_LP1) &&
		(power_mode != SDE_MODE_DPMS_LP2) &&
		(power_mode != SDE_MODE_DPMS_STANDBY) &&
		(power_mode != SDE_MODE_DPMS_SUSPEND);
}

uint32_t display_engine_brightness_get_mipi_level(void)
{
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);

	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return 0;
	}
	return de_context->brightness.last_level_out;
}

uint32_t display_engine_brightness_get_mode_in_use(void)
{
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);

	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return 0;
	}
	return de_context->brightness.mode_in_use;
}

static bool is_brightness_need_sync(
	int mode_in_use, int mode_set_by_ioctl, int last_alpha, int alpha)
{
	if (mode_in_use == mode_set_by_ioctl || alpha == last_alpha)
		return false;
	LCD_KIT_DEBUG("last_alpha [%d], alpa [%d]\n", last_alpha, alpha);
	return true;
}

static int get_mapped_alpha_inner_handle_mode(int bl_level,
	struct display_engine_context *de_context)
{
	int out_alpha = ALPHA_DEFAULT;
	unsigned int mode_set_by_ioctl;
	__u16 *to_alpha_lut = NULL;

	mode_set_by_ioctl = de_context->brightness.mode_set_by_ioctl;
	if (mode_set_by_ioctl >= DISPLAY_ENGINE_MODE_MAX) {
		LCD_KIT_ERR("mode_set_by_ioctl out of range, mode_set_by_ioctl = %d\n",
			mode_set_by_ioctl);
		return ALPHA_DEFAULT;
	}
	if (de_context->brightness.lut[mode_set_by_ioctl].mode_id ==
		DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT) {
		return ALPHA_DEFAULT;
	}
	to_alpha_lut = de_context->brightness.lut[mode_set_by_ioctl].to_alpha_lut;
	if (!to_alpha_lut) {
		LCD_KIT_ERR("to_alpha_lut is NULL, mode_set_by_ioctl:%d\n", mode_set_by_ioctl);
		return ALPHA_DEFAULT;
	}
	switch (mode_set_by_ioctl) {
	case DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT:
		out_alpha = ALPHA_DEFAULT;
		break;
	case DISPLAY_ENGINE_BRIGHTNESS_MODE_DC:
	case DISPLAY_ENGINE_BRIGHTNESS_MODE_HIGH_PRECISION:
		out_alpha = to_alpha_lut[bl_level];
		LCD_KIT_DEBUG("brightness.mode_set_by_ioctl[%u], in_level[%u] -> out_alpha[%u]\n",
			mode_set_by_ioctl, bl_level, out_alpha);
		break;
	default:
		LCD_KIT_WARNING("invalid brightness.mode_set_by_ioctl %d\n", mode_set_by_ioctl);
		out_alpha = ALPHA_DEFAULT;
	}
	return out_alpha;
}

static int get_mapped_alpha_inner(int bl_level)
{
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);

	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return ALPHA_DEFAULT;
	}

	/* bypass alpha at request, e.g. fingerprint */
	if (de_context->brightness.is_alpha_bypass) {
		LCD_KIT_INFO("alpha bypassed...\n");
		return ALPHA_DEFAULT;
	}
	if (!de_context->is_inited) {
		LCD_KIT_WARNING("display engine is not inited yet.\n");
		return ALPHA_DEFAULT;
	}
	if (bl_level < 0 || bl_level >= BRIGHTNESS_LUT_LENGTH) {
		LCD_KIT_ERR("invalid bl_level %d\n", bl_level);
		return ALPHA_DEFAULT;
	}

	return get_mapped_alpha_inner_handle_mode(bl_level, de_context);
}

static bool handle_brightness_lut(
	int in_level, struct display_engine_context *de_context, __u16 *out_level, int mode_in_use)
{
	__u16 *to_dbv_lut = NULL;

	if (de_context->brightness.lut[mode_in_use].mode_id ==
		DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT) {
		LCD_KIT_WARNING("invalid brightness lut, mode_in_use = %d\n", mode_in_use);
		return false;
	}
	to_dbv_lut = de_context->brightness.lut[mode_in_use].to_dbv_lut;
	if (!to_dbv_lut) {
		LCD_KIT_ERR("to_dbv_lut is NULL, mode_in_use = %d\n", mode_in_use);
		return false;
	}
	*out_level = to_dbv_lut[in_level];
	LCD_KIT_DEBUG("brightness.mode_in_use [%u], in_level [%u] -> out_level [%u]\n",
		mode_in_use, in_level, *out_level);
	return true;
}

static int get_mapped_level_handle_mode(int in_level, struct display_engine_context *de_context)
{
	__u16 out_level = in_level;
	unsigned int mode_in_use;

	mode_in_use = de_context->brightness.mode_in_use;
	if (mode_in_use >= DISPLAY_ENGINE_MODE_MAX) {
		LCD_KIT_ERR("mode_in_use out of range, mode_in_use = %d\n", mode_in_use);
		goto error_out;
	}
	if (in_level == 0) {
		de_context->brightness.mode_in_use = DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT;
		LCD_KIT_DEBUG("in_level is 0, need sync later, mdoe = %d\n", mode_in_use);
		goto error_out;
	}

	switch (mode_in_use) {
	case DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT:
		/* mode_in_use is set to DEFAULT when level is 0, to trigger a sync at screen on */
		if (is_brightness_need_sync(
			de_context->brightness.mode_in_use,
			de_context->brightness.mode_set_by_ioctl,
			ALPHA_DEFAULT, get_mapped_alpha_inner(in_level))) {
			/*
			 * Need to sync: since mode_in_use is still DEFAULT, it means
			 * brightness_handle_sync is not processed yet. To avoid flickering, we have
			 * to use the last level out, which is 0 in the screen on case.
			 */
			LCD_KIT_INFO("in default mode need sync, use last level.\n");
			out_level = de_context->brightness.last_level_out;
		} else {
			/*
			 * No need to sync: use mode_set_by_ioctl directly.
			 * brightness_handle_sync will update mode_in_use.
			 */
			if (!handle_brightness_lut(in_level, de_context, &out_level,
				de_context->brightness.mode_set_by_ioctl))
				goto error_out;
		}
		break;
	case DISPLAY_ENGINE_BRIGHTNESS_MODE_DC:
	case DISPLAY_ENGINE_BRIGHTNESS_MODE_HIGH_PRECISION:
		/* Normal brightness update goes here */
		if (!handle_brightness_lut(in_level, de_context, &out_level, mode_in_use))
			goto error_out;
		break;
	default:
		LCD_KIT_ERR("invalid brightness.mode_in_use %d\n", mode_in_use);
		goto error_out;
	}
	de_context->brightness.last_level_out = out_level;
	return out_level;

error_out:
	de_context->brightness.last_level_out = in_level;
	return in_level;
}

int display_engine_brightness_get_mapped_level(int in_level, uint32_t panel_id)
{
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);

	/* Keep fingerprint on top, unless the priority of new code is higher than fingerprint */
	if (g_fingerprint_backlight_type == BACKLIGHT_HIGH_LEVEL) {
		return g_fingerprint_hbm_level;
	}

	if (panel_id != PRIMARY_PANEL) {
		LCD_KIT_DEBUG("panel_id is not PRIMARY_PANEL\n");
		return in_level;
	}

	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return in_level;
	}

	de_context->brightness.last_level_in = in_level;
	if (in_level < 0 || in_level >= BRIGHTNESS_LUT_LENGTH) {
		LCD_KIT_ERR("invalid in_level %d\n", in_level);
		goto error_out;
	}
	if (!de_context->is_inited) {
		LCD_KIT_WARNING("display engine is not inited yet.\n");
		goto error_out;
	}
	if (de_context->brightness.mode_set_by_ioctl == DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT &&
		de_context->brightness.mode_in_use == DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT) {
		LCD_KIT_DEBUG("default brightness mode.\n");
		goto error_out;
	}

	return get_mapped_level_handle_mode(in_level, de_context);

error_out:
	de_context->brightness.last_level_out = in_level;
	return in_level;
}

int display_engine_brightness_get_mapped_alpha(void)
{
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);
	int alpha;
	int power_mode = display_engine_brightness_get_power_mode();

	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return ALPHA_DEFAULT;
	}
	de_context->brightness.is_alpha_updated_at_mode_change =
		de_context->brightness.mode_in_use != de_context->brightness.mode_set_by_ioctl;
	if (power_mode != SDE_MODE_DPMS_ON && power_mode != SDE_MODE_DPMS_OFF) {
		LCD_KIT_DEBUG("not in power on/off state, return default alpha\n");
		de_context->brightness.last_alpha = de_context->brightness.current_alpha;
		de_context->brightness.current_alpha = ALPHA_DEFAULT;
		return ALPHA_DEFAULT;
	}
	alpha = get_mapped_alpha_inner(
		de_context->brightness.last_level_in);
	de_context->brightness.last_alpha = de_context->brightness.current_alpha;
	de_context->brightness.current_alpha = alpha;
	return alpha;
}

/* IO implementation functions: */
/* DRM features: */
static int display_engine_drm_hist_get_enable(uint32_t panel_id,
	struct display_engine_drm_hist *hist)
{
	struct display_engine_context *de_context = get_de_context(panel_id);

	if (!de_context) {
		LCD_KIT_ERR("de_context is null, panel_id=%u\n", panel_id);
		return LCD_KIT_FAIL;
	}

	hist->enable = de_context->drm_hist.is_enable ? 1 : 0;
	LCD_KIT_INFO("panel_id=%u is_enable=%u hist->enable=%u\n",
		panel_id, de_context->drm_hist.is_enable, hist->enable);
	return LCD_KIT_OK;
}

static int display_engine_drm_hist_set_enable(uint32_t panel_id,
	const struct display_engine_drm_hist *hist)
{
	struct display_engine_context *de_context = get_de_context(panel_id);
	bool is_enable = (hist->enable == 1);

	if (!de_context) {
		LCD_KIT_ERR("de_context is null, panel_id=%u\n", panel_id);
		return LCD_KIT_FAIL;
	}

	LCD_KIT_INFO("panel_id=%u is_enable=%u hist->enable=%u\n", is_enable, hist->enable);
	if (de_context->drm_hist.is_enable != is_enable) {
		de_context->drm_hist.is_enable = is_enable;
		de_context->drm_hist.is_enable_change = true;
		LCD_KIT_INFO("panel_id=%u is_enable=%d is_enable_change=%d\n",
			panel_id,
			de_context->drm_hist.is_enable,
			de_context->drm_hist.is_enable_change);
	}
	return LCD_KIT_OK;
}

/* Brightness features */
static int display_engine_brightness_set_mode(uint32_t panel_id, uint32_t mode)
{
	struct display_engine_context *de_context = get_de_context(panel_id);

	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return ALPHA_DEFAULT;
	}

	LCD_KIT_INFO("brightness.mode_set_by_ioctl [%d] -> [%d]\n",
		de_context->brightness.mode_set_by_ioctl, mode);

	/*
	 * Sync between mode_set_by_ioctl and mode_in_use will be handle by
	 * display_engine_brightness_handle_mode_change(void) in sde_crtc_atomic_flush
	 */
	de_context->brightness.mode_set_by_ioctl = mode;
	return LCD_KIT_OK;
}

static int display_engine_brightness_set_fingerprint_backlight(uint32_t panel_id,
	struct display_engine_fingerprint_backlight fingerprint_backlight)
{
	struct display_engine_context *de_context = get_de_context(panel_id);

	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return ALPHA_DEFAULT;
	}
	de_context->brightness.fingerprint_backlight = fingerprint_backlight;
	LCD_KIT_INFO("[effect] scene:%d hbm_level:%d current_level:%d\n",
		de_context->brightness.fingerprint_backlight.scene_info,
		de_context->brightness.fingerprint_backlight.hbm_level,
		de_context->brightness.fingerprint_backlight.current_level);
	return LCD_KIT_OK;
}

static void display_engine_brightness_print_lut(struct display_engine_brightness_lut *lut)
{
	LCD_KIT_INFO("mode_id [%u]", lut->mode_id);
	LCD_KIT_INFO("to_dbv_lut[0 - 4]: [%u, %u, %u, %u, %u], [last]: [%u]",
		lut->to_dbv_lut[0], lut->to_dbv_lut[1],
		lut->to_dbv_lut[2], lut->to_dbv_lut[3],
		lut->to_dbv_lut[4], lut->to_dbv_lut[BRIGHTNESS_LUT_LENGTH - 1]);
	LCD_KIT_INFO("to_alpha_lut[0 - 4]: [%u, %u, %u, %u, %u], [last]: [%u]",
		lut->to_alpha_lut[0], lut->to_alpha_lut[1],
		lut->to_alpha_lut[2], lut->to_alpha_lut[3],
		lut->to_alpha_lut[4], lut->to_alpha_lut[BRIGHTNESS_LUT_LENGTH - 1]);
}

static int display_engine_brightness_check_lut(struct display_engine_brightness_lut *lut)
{
	int to_dbv_lut_index = 0;
	int to_alpha_lut_index = 0;

	/* Check if either lut contains nothing but zeros */
	for (; to_dbv_lut_index < BRIGHTNESS_LUT_LENGTH; to_dbv_lut_index++) {
		if (lut->to_dbv_lut[to_dbv_lut_index] > 0)
			break;
	}
	for (; to_alpha_lut_index < BRIGHTNESS_LUT_LENGTH; to_alpha_lut_index++) {
		if (lut->to_alpha_lut[to_alpha_lut_index] > 0)
			break;
	}
	if (to_dbv_lut_index == BRIGHTNESS_LUT_LENGTH
		|| to_alpha_lut_index == BRIGHTNESS_LUT_LENGTH)
		return LCD_KIT_FAIL;
	else
		return LCD_KIT_OK;
}

/* brightness init */
static int display_engine_brightness_init_lut(uint32_t panel_id, __u32 mode_id)
{
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);

	if (panel_id != PRIMARY_PANEL) {
		LCD_KIT_INFO("not primary panel, skip lut init\n");
		return LCD_KIT_OK;
	}
	if (!de_context) {
		LCD_KIT_ERR("de_context is null\n");
		return LCD_KIT_FAIL;
	}
	if (de_context->brightness.lut[mode_id].to_dbv_lut &&
		de_context->brightness.lut[mode_id].to_alpha_lut) {
		LCD_KIT_WARNING("Repeated initialization, mode_id = %d\n", mode_id);
		return LCD_KIT_OK;
	}
	de_context->brightness.lut[mode_id].to_dbv_lut =
		kcalloc(BRIGHTNESS_LUT_LENGTH, sizeof(__u16), GFP_KERNEL);
	if (!de_context->brightness.lut[mode_id].to_dbv_lut) {
		LCD_KIT_ERR("to_dbv_lut malloc fail\n");
		goto error_out_after_alloc;
	}
	de_context->brightness.lut[mode_id].to_alpha_lut =
		kcalloc(BRIGHTNESS_LUT_LENGTH, sizeof(__u16), GFP_KERNEL);
	if (!de_context->brightness.lut[mode_id].to_alpha_lut) {
		LCD_KIT_ERR("to_alpha_lut malloc fail\n");
		goto error_out_after_alloc;
	}
	/*
	 * in DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT, brightness will not be mapped.
	 * Both luts should NOT be used in this situation, since they are likely not set yet.
	 */
	de_context->brightness.lut[mode_id].mode_id = DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT;
	return LCD_KIT_OK;

error_out_after_alloc:
	kfree(de_context->brightness.lut[mode_id].to_dbv_lut);
	kfree(de_context->brightness.lut[mode_id].to_alpha_lut);
	de_context->brightness.lut[mode_id].to_dbv_lut = NULL;
	de_context->brightness.lut[mode_id].to_alpha_lut = NULL;
	return LCD_KIT_FAIL;
}

static int display_engine_brightness_set_lut(uint32_t panel_id,
	struct display_engine_brightness_lut *input_lut)
{
	int ret = LCD_KIT_OK;
	struct display_engine_brightness_lut *target_lut = NULL;
	struct display_engine_context *de_context = get_de_context(panel_id);

	if (!de_context) {
		LCD_KIT_ERR("de_context is null\n");
		return LCD_KIT_FAIL;
	}
	if (!input_lut) {
		LCD_KIT_ERR("input_lut is NULL\n");
		return LCD_KIT_FAIL;
	}
	if (input_lut->mode_id >= DISPLAY_ENGINE_MODE_MAX) {
		LCD_KIT_ERR("mode_id out of range, mode_id_max = %d\n, mode_id = %d\n",
			DISPLAY_ENGINE_MODE_MAX, input_lut->mode_id);
		return LCD_KIT_FAIL;
	}

	/* init brightness */
	if (display_engine_brightness_init_lut(panel_id, input_lut->mode_id) != LCD_KIT_OK) {
		LCD_KIT_ERR("brightness_lut_init failed, exit.\n");
		return LCD_KIT_FAIL;
	}

	target_lut = &de_context->brightness.lut[input_lut->mode_id];
	if (!target_lut->to_dbv_lut || !target_lut->to_alpha_lut) {
		LCD_KIT_ERR("to_dbv_lut or to_alpha_lut is NULL\n");
		return LCD_KIT_FAIL;
	}
	ret = copy_from_user(target_lut->to_dbv_lut, input_lut->to_dbv_lut,
		BRIGHTNESS_LUT_LENGTH * sizeof(__u16));
	if (ret != 0) {
		LCD_KIT_ERR("[effect] copy dbv lut failed, ret = %d\n", ret);
		return LCD_KIT_FAIL;
	}
	ret = copy_from_user(target_lut->to_alpha_lut, input_lut->to_alpha_lut,
		BRIGHTNESS_LUT_LENGTH * sizeof(__u16));
	if (ret != 0) {
		LCD_KIT_ERR("[effect] copy alpha lut failed, ret = %d\n", ret);
		return LCD_KIT_FAIL;
	}
	target_lut->mode_id = input_lut->mode_id;
	display_engine_brightness_print_lut(target_lut);

	ret = display_engine_brightness_check_lut(target_lut);
	if (ret != 0) {
		LCD_KIT_ERR("[effect] invalid lut, use mode default!\n", ret);
		target_lut->mode_id = DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT;
		return LCD_KIT_FAIL;
	}
	return ret;
}

void display_engine_brightness_handle_vblank(ktime_t te_timestamp)
{
	struct qcom_panel_info *panel_info = lcm_get_panel_info(PRIMARY_PANEL);
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);

	if (!panel_info) {
		LCD_KIT_ERR("panel_info is NULL\n");
		return;
	}
	if (!de_context) {
		LCD_KIT_ERR("de_context is NULL\n");
		return;
	}
	de_context->brightness.te_timestamp_last = de_context->brightness.te_timestamp;
	de_context->brightness.te_timestamp = te_timestamp;
	if (de_context->is_inited)
		queue_work(panel_info->vblank_workqueue, &panel_info->vblank_work);
}

static void brightness_handle_no_sync(struct qcom_panel_info *panel_info,
	struct display_engine_context *de_context)
{
	if (de_context->brightness.mode_in_use != de_context->brightness.mode_set_by_ioctl)
		LCD_KIT_INFO("no need to update, alpha [%d]->[%d].\n",
			de_context->brightness.last_alpha,
			de_context->brightness.current_alpha);

	de_context->brightness.is_alpha_updated_at_mode_change = false;
	de_context->brightness.mode_in_use = de_context->brightness.mode_set_by_ioctl;
	if (de_context->brightness.last_level_out == 0) {
		LCD_KIT_INFO("screen on update backlight\n");
		queue_work(panel_info->brightness_workqueue, &panel_info->backlight_update_work);
	}
}

static void brightness_handle_sync(struct qcom_panel_info *panel_info,
	struct display_engine_context *de_context)
{
	uint64_t sync_delay_time_us = 0;

	/* reset is_alpha_updated_at_mode_change for next sync */
	de_context->brightness.is_alpha_updated_at_mode_change = false;
	LCD_KIT_INFO("mode changed [%u] -> [%u], need sync.\n",
		de_context->brightness.mode_in_use, de_context->brightness.mode_set_by_ioctl);

	/* Delay atomic flush if needed, make sure backlight and alpha take effect in one frame */
	sync_delay_time_us = get_backlight_sync_delay_time_us(
		panel_info, de_context, PRIMARY_PANEL);
	if (sync_delay_time_us > 0)
		usleep_range(sync_delay_time_us, sync_delay_time_us);

	/* Sync both mode since alpha is ready, and backlight is about to update */
	de_context->brightness.mode_in_use = de_context->brightness.mode_set_by_ioctl;

	queue_work(panel_info->brightness_workqueue, &panel_info->backlight_update_work);
	LCD_KIT_INFO("sync complete, sync_delay_time_us [%lu]", sync_delay_time_us);
}

void display_engine_brightness_handle_mode_change(void)
{
	struct qcom_panel_info *panel_info = lcm_get_panel_info(PRIMARY_PANEL);
	struct display_engine_context *de_context = get_de_context(DISPLAY_ENGINE_PANEL_INNER);

	if (!de_context) {
		LCD_KIT_ERR("de_context is null\n");
		return;
	}
	if (!panel_info) {
		LCD_KIT_ERR("panel_info is null\n");
		return;
	}
	if (!de_context->is_inited) {
		LCD_KIT_ERR("de_context is not inited yet!\n");
		return;
	}
	if (display_engine_brightness_get_backlight() == 0) {
		LCD_KIT_DEBUG("backlight is not updated yet.\n");
		return;
	}
	if (!de_context->brightness.is_alpha_updated_at_mode_change) {
		LCD_KIT_DEBUG("alpha is not updated yet.\n");
		return;
	}
	if (!is_brightness_need_sync(
		de_context->brightness.mode_in_use, de_context->brightness.mode_set_by_ioctl,
		de_context->brightness.last_alpha, de_context->brightness.current_alpha)) {
		brightness_handle_no_sync(panel_info, de_context);
		return;
	}
	/* need sync */
	brightness_handle_sync(panel_info, de_context);
}

bool display_engine_brightness_is_fingerprint_hbm_enabled(void)
{
	return g_fingerprint_backlight_type == BACKLIGHT_HIGH_LEVEL ||
		g_fingerprint_backlight_type_real == BACKLIGHT_HIGH_LEVEL;
}

/* Panel information: */
static int display_engine_panel_info_get(uint32_t panel_id,
	struct display_engine_panel_info *info)
{
	uint32_t lcd_panel_id = display_engine_panel_id_to_lcdkit(panel_id);
	struct qcom_panel_info *panel_info = lcm_get_panel_info(lcd_panel_id);
	struct lcd_kit_common_info *lcd_common_info = lcd_kit_get_common_info(lcd_panel_id);

	if (!panel_info || !panel_info->display || !lcd_common_info) {
		LCD_KIT_ERR("NULL point: panel_info=%p common_info=%p!\n", panel_info,
			lcd_common_info);
		return LCD_KIT_FAIL;
	}

	info->width = panel_info->display->modes[0].timing.h_active;
	info->height = panel_info->display->modes[0].timing.v_active;
	info->max_luminance = panel_info->maxluminance;
	info->min_luminance = panel_info->minluminance;
	info->max_backlight = panel_info->bl_max;
	info->min_backlight = panel_info->bl_min;
	info->sn_code_length = (panel_info->sn_code_length > SN_CODE_LEN_MAX) ?
		SN_CODE_LEN_MAX : panel_info->sn_code_length;
	if (info->sn_code_length > 0) {
		char sn_text[SN_TEXT_SIZE];
		uint32_t i;
		memcpy(info->sn_code, panel_info->sn_code, info->sn_code_length);
		for (i = 0; i < info->sn_code_length; i++)
			sprintf(sn_text + (i << 1), "%02x", info->sn_code[i]);
		for (i <<= 1; i < SN_CODE_LEN_MAX; i++)
			sn_text[i] = '\0';
		LCD_KIT_INFO("sn[%u]=\'%s\'\n", info->sn_code_length, sn_text);
	} else {
		memset(info->sn_code, 0, sizeof(info->sn_code));
		LCD_KIT_INFO("sn is empty\n");
	}
	strncpy(info->panel_name, lcd_common_info->panel_name, PANEL_NAME_LEN - 1);
#ifdef LCD_FACTORY_MODE
	info->is_factory = true;
#else
	info->is_factory = false;
#endif
	LCD_KIT_INFO("panel_id=%u res(w=%u,h=%u) lum(min=%u,max=%u) bl(min=%u,max=%u) name=%s\n",
		panel_id, info->width, info->height, info->max_luminance, info->min_luminance,
		info->max_backlight, info->min_backlight, info->panel_name);
	return LCD_KIT_OK;
}

/* IO interfaces: */
int display_engine_get_param(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	struct display_engine_param *param = (struct display_engine_param *)data;
	bool is_fail = false;

	if (!param) {
		LCD_KIT_ERR("param is null\n");
		return LCD_KIT_FAIL;
	}

	if (param->modules & DISPLAY_ENGINE_DRM_HIST_ENABLE) {
		if (display_engine_drm_hist_get_enable(param->panel_id, &param->drm_hist)) {
			LCD_KIT_WARNING("display_engine_drm_hist_enable() failed!\n");
			is_fail = true;
		}
	}
	if (param->modules & DISPLAY_ENGINE_PANEL_INFO) {
		if (display_engine_panel_info_get(param->panel_id, &param->panel_info)) {
			LCD_KIT_WARNING("display_engine_panel_info_get() failed!\n");
			is_fail = true;
		}
	}

	return is_fail ? LCD_KIT_FAIL : LCD_KIT_OK;
}

int display_engine_set_param(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	struct display_engine_param *param = (struct display_engine_param *)data;
	bool is_fail = false;

	if (!param) {
		LCD_KIT_ERR("param is null\n");
		return LCD_KIT_FAIL;
	}

	if (param->modules & DISPLAY_ENGINE_DRM_HIST_ENABLE) {
		if (display_engine_drm_hist_set_enable(param->panel_id, &param->drm_hist)) {
			LCD_KIT_WARNING("display_engine_drm_hist_enable() failed!\n");
			is_fail = true;
		}
	}
	if (param->modules & DISPLAY_ENGINE_BRIGHTNESS_LUT) {
		if (display_engine_brightness_set_lut(param->panel_id, &param->brightness_lut)) {
			LCD_KIT_WARNING("display_engine_brightness_lut_set_lut() failed!\n");
			is_fail = true;
		}
	}
	if (param->modules & DISPLAY_ENGINE_BRIGHTNESS_MODE) {
		if (display_engine_brightness_set_mode(param->panel_id, param->brightness_mode)) {
			LCD_KIT_WARNING("display_engine_brightness_set_mode() failed!\n");
			is_fail = true;
		}
	}
	if (param->modules & DISPLAY_ENGINE_FINGERPRINT_BACKLIGHT) {
		if (display_engine_brightness_set_fingerprint_backlight(param->panel_id,
			param->fingerprint_backlight)) {
			LCD_KIT_WARNING(
				"display_engine_brightness_set_fingerprint_backlight() failed!\n");
			is_fail = true;
		}
	}

	return is_fail ? LCD_KIT_FAIL : LCD_KIT_OK;
}

/* init of display engine kernel */
/* workqueue init */
static int display_engine_workqueue_init(uint32_t panel_id)
{
	struct qcom_panel_info *panel_info = lcm_get_panel_info(panel_id);

	if (!panel_info) {
		LCD_KIT_ERR("panel_info is NULL!\n");
		return LCD_KIT_FAIL;
	}

	panel_info->brightness_workqueue =
		create_singlethread_workqueue("display_engine_brightness");
	panel_info->vblank_workqueue =
		create_singlethread_workqueue("display_engine_vblank");
	if (!panel_info->brightness_workqueue || !panel_info->vblank_workqueue) {
		LCD_KIT_ERR("workqueue is NULL!\n");
		return LCD_KIT_FAIL;
	}

	INIT_WORK(&panel_info->backlight_work,
		display_engine_brightness_handle_fingerprint_hbm_work);
	INIT_WORK(&panel_info->vblank_work, display_engine_brightness_handle_vblank_work);
	INIT_WORK(&panel_info->backlight_update_work,
		display_engine_brightness_handle_backlight_update);
	LCD_KIT_INFO("workqueue inited\n");
	return LCD_KIT_OK;
}

/* Entry of display engine init */
void display_engine_init(uint32_t panel_id)
{
	struct display_engine_context *de_context =
		get_de_context(display_engine_panel_id_to_de(panel_id));

	LCD_KIT_INFO("+\n");
	if (!de_context) {
		LCD_KIT_ERR("de_context is null\n");
		return;
	}
	/* init workqueue */
	if (display_engine_workqueue_init(panel_id) != LCD_KIT_OK) {
		LCD_KIT_ERR("workqueue_init failed, exit.\n");
		return;
	}
	de_context->is_inited = true;
	LCD_KIT_INFO("-\n");
}
