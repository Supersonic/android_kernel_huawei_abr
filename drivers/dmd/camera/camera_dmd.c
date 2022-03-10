

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/printk.h>
#include <linux/mutex.h>
#include "chipset_common/dmd/camera_dmd.h"

#define PFX "[camhiview]"
#define log_inf(fmt, args...) pr_info(PFX "%s line:%d " fmt, __func__, __LINE__, ##args)
#define log_err(fmt, args...) pr_err(PFX "%s line:%d " fmt, __func__, __LINE__, ##args)
#define loge_if(x) \
	do { \
		if (x) \
			log_err("'%s' failed, ret = %d", #x, x); \
	} while (0)
#define loge_if_ret(x) \
	do { \
		if (x) { \
			log_err("'%s' failed, ret = %d", #x, x); \
			return -1; \
		} \
	} while (0)
#define return_on_null(x) \
	do { \
		if (!x) { \
			log_err("invalid params, %s", #x); \
			return; \
		} \
	} while (0)

static DEFINE_MUTEX(g_cam_hiview_lock);

static const struct driver_ic_position_info g_driver_ic_position_info[CAMERA_POSITION_MAX] = {
	{ CAMERA_POSITION_REAR,        0, "back main camera " },
	{ CAMERA_POSITION_REAR_SECOND, 1, "back second camera " },
	{ CAMERA_POSITION_REAR_THIRD,  2, "back third camera " },
	{ CAMERA_POSITION_REAR_FORTH,  3, "back forth camera " },
	{ CAMERA_POSITION_REAR_FIFTH,  4, "back fifth camera " },
	{ CAMERA_POSITION_FORE,        5, "front main camera " },
	{ CAMERA_POSITION_FORE_SECOND, 6, "front second camera " },
	{ CAMERA_POSITION_FORE_THIRD,  7, "front third camera " },
};

static const struct driver_error_type_info g_driver_error_type_info[DRIVER_ERROR_TYPE_MAX] = {
	{ CSI_PHY_ERR,                    927013000, "csi phy error" },
	{ BTB_CHECK_ERR,                  927013020, "BTB check error" },
	{ SENSOR_I2C_ERR,                 927013040, "sensor i2c error" },
	{ VCM_I2C_ERR,                    927013050, "vcm i2c error" },
	{ EEPROM_I2C_ERR,                 927013060, "eeprom i2c error" },
	{ OIS_I2C_ERR,                    927013070, "ois i2c error" },
	{ LED_FLASH_I2C_ERR,              927013080, "LED flash i2c error" },
	{ FPC_OPEN_CIRCUIT_ERR,           927013310, "FPC open circuit error" },
	{ LED_FLASH_OVER_FLOW_ERR,        927013320, "LED flash over flow error" },
	{ LED_FLASH_OVER_TEMPERATURE_ERR, 927013322, "LED flash over temperature error" },
	{ LED_FLASH_SHORT_CIRCUIT_ERR,    927013324, "LED flash short circuit error" },
	{ LED_FLASH_OVER_VOLTAGE_ERR,     927013326, "LED flash over voltage error" },
	{ LED_FLASH_OPEN_CIRCUIT_ERR,     927013328, "LED flash open circuit error" },
};

int camkit_hiview_init(struct camera_dmd_info* info)
{
	loge_if_ret(memset_s(&(info->extra_info), sizeof(struct driver_dmd_extra_info),
		0, sizeof(struct driver_dmd_extra_info)));
	info->error_type = DRIVER_ERROR_TYPE_INVALID;
	info->ic_position = CAMERA_POSITION_INVALID;

	return 0;
}
EXPORT_SYMBOL(camkit_hiview_init);

void hwcam_hiview_report(struct hwcam_hievent_info* driver_info)
{
	struct hiview_hievent* hi_event = NULL;
	return_on_null(driver_info);

	mutex_lock(&g_cam_hiview_lock);
	hi_event = hiview_hievent_create(driver_info->error_no);
	if (!hi_event) {
		log_err("create hievent fail");
		mutex_unlock(&g_cam_hiview_lock);
		return;
	}

	hiview_hievent_put_string(hi_event, "IC_NAME", driver_info->ic_name);
	hiview_hievent_put_string(hi_event, "MODULE_NAME", driver_info->module_name);
	hiview_hievent_put_string(hi_event, "CONTENT", driver_info->content);

	if (hiview_hievent_report(hi_event) <= 0) {
		log_err("report failed");
		hiview_hievent_destroy(hi_event);
		mutex_unlock(&g_cam_hiview_lock);
		return;
	}
	hiview_hievent_destroy(hi_event);
	mutex_unlock(&g_cam_hiview_lock);

	log_inf("report succ");
}

void camkit_hiview_report(struct camera_dmd_info* cam_info)
{
	struct hwcam_hievent_info driver_info;
	char error_info[HIVIEW_MAX_CONTENT_LEN] = { 0 };
	int error_no = 0;
	int type, index;

	if (memset_s(&driver_info, sizeof(driver_info), 0, sizeof(driver_info))) {
		log_err("dmd memset fail");
		return;
	}

	if (cam_info->error_type >= DRIVER_ERROR_TYPE_MAX ||
		cam_info->error_type <= DRIVER_ERROR_TYPE_INVALID) {
		log_err("dmd error type is invalid");
		return;
	}

	for (type = 0; type < DRIVER_ERROR_TYPE_MAX; type++)
		if (g_driver_error_type_info[type].error_type == cam_info->error_type) {
			error_no += g_driver_error_type_info[type].error_type_base_no;
			break;
		}

	for (index = 0; index < CAMERA_POSITION_MAX; index++)
		if (g_driver_ic_position_info[index].ic_position == cam_info->ic_position) {
			error_no += g_driver_ic_position_info[index].ic_position_no;
			strcat(error_info, g_driver_ic_position_info[index].ic_position_info);
			break;
		}

	if (strlen(cam_info->extra_info.ic_name)) {
		loge_if(strncpy_s(driver_info.ic_name, sizeof(driver_info.ic_name),
			cam_info->extra_info.ic_name, sizeof(driver_info.ic_name) - 1));
		strcat(error_info, cam_info->extra_info.ic_name);
		strcat(error_info, " ");
	}

	if (strlen(cam_info->extra_info.module_name)) {
		loge_if(strncpy_s(driver_info.module_name, sizeof(driver_info.module_name),
			cam_info->extra_info.module_name, sizeof(driver_info.module_name) - 1));
		strcat(error_info, "from ");
		strcat(error_info, cam_info->extra_info.module_name);
		strcat(error_info, " ");
	}

	if (type < DRIVER_ERROR_TYPE_MAX)
		strcat(error_info, g_driver_error_type_info[type].error_type_info);

	if (strlen(cam_info->extra_info.error_scene)) {
		strcat(error_info, cam_info->extra_info.error_scene);
	}

	driver_info.error_no = error_no;
	loge_if(strncpy_s(driver_info.content, sizeof(driver_info.content),
		error_info, sizeof(driver_info.content)));
	log_inf("camera dmd error_no is %d, content is %s",
		driver_info.error_no, driver_info.content);
	hwcam_hiview_report(&driver_info);
}
EXPORT_SYMBOL(camkit_hiview_report);