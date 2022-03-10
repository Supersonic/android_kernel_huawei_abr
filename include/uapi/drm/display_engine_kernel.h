

#ifndef __DISPLAY_ENGINE_KERNEL_H__
#define __DISPLAY_ENGINE_KERNEL_H__

#ifndef BIT
#define BIT(nr) (1UL << (nr))
#endif

#define BRIGHTNESS_LUT_LENGTH 10001
#define SN_CODE_LEN_MAX 54
#define PANEL_NAME_LEN 128

#if defined(__cplusplus)
extern "C" {
#endif

/* The panel ID used to specify which panel would be configured.
 * And you can use DISPLAY_ENGINE_PANEL_INNER for single panel device.
 */
enum display_engine_panel_id {
	DISPLAY_ENGINE_PANEL_INVALID = -1,
	DISPLAY_ENGINE_PANEL_INNER,
	DISPLAY_ENGINE_PANEL_OUTER,
	DISPLAY_ENGINE_PANEL_COUNT,
};

/* The module ID used to specify the detail operation of IO.
 * Module IDs can be used as single or multiple.
 */
enum display_engine_module_id {
	DISPLAY_ENGINE_DRM_HIST_ENABLE = BIT(0),
	DISPLAY_ENGINE_BRIGHTNESS_LUT = BIT(1),
	DISPLAY_ENGINE_BRIGHTNESS_MODE = BIT(2),
	DISPLAY_ENGINE_PANEL_INFO = BIT(3),
	DISPLAY_ENGINE_FINGERPRINT_BACKLIGHT = BIT(4),
};

/* The data structure of display engine IO feature.
 * Names should be started with display_engine_drm_, display_engine_ddic_ or display_engine_panel_,
 * and followed by feature name.
 */
struct display_engine_drm_hist {
	unsigned int enable;
};

enum display_engine_brightness_mode_id {
	DISPLAY_ENGINE_BRIGHTNESS_MODE_DEFAULT = 0,
	DISPLAY_ENGINE_BRIGHTNESS_MODE_DC,
	DISPLAY_ENGINE_BRIGHTNESS_MODE_HIGH_PRECISION,
	DISPLAY_ENGINE_MODE_MAX
};

/* The data structure of dc brightness */
struct display_engine_brightness_lut {
	unsigned int mode_id;

	/* length of both lut: BRIGHTNESS_LUT_LENGTH */
	unsigned short *to_dbv_lut;
	unsigned short *to_alpha_lut;
};

/* The data structure of panel information IO feature. */
struct display_engine_panel_info {
	unsigned int width;
	unsigned int height;
	unsigned int max_luminance;
	unsigned int min_luminance;
	unsigned int max_backlight;
	unsigned int min_backlight;
	unsigned int sn_code_length;
	unsigned char sn_code[SN_CODE_LEN_MAX];
	char panel_name[PANEL_NAME_LEN];
	bool is_factory;
};

/* The data structure of fingerprint backlight, includes fingerprint up and down */
struct display_engine_fingerprint_backlight {
	unsigned int scene_info;
	unsigned int hbm_level;
	unsigned int current_level;
};

/* The data structure of display engine IO interface. */
struct display_engine_param {
	unsigned int panel_id; /* It used for multi-panels */
	unsigned int modules;
	struct display_engine_drm_hist drm_hist;
	struct display_engine_brightness_lut brightness_lut;
	struct display_engine_panel_info panel_info;
	unsigned int brightness_mode;
	struct display_engine_fingerprint_backlight fingerprint_backlight;
};

#if defined(__cplusplus)
}
#endif

#endif /* __DISPLAY_ENGINE_KERNEL_H__ */
