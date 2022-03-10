

#ifndef CAMERA_DMD_H
#define CAMERA_DMD_H

#include <log/hiview_hievent.h>
#include "securec.h"

/* dmd description info max length */
#define HIVIEW_MAX_CONTENT_LEN     150
#define HIVIEW_MAX_MODULE_NAME_LEN 32
#define HIVIEW_MAX_IC_NAME_LEN     32

/**
 * enum ic_position_t - Identifies the camera module position
 */
typedef enum {
	CAMERA_POSITION_INVALID               =    -1,
	CAMERA_POSITION_REAR                  =    0,
	CAMERA_POSITION_FORE,
	CAMERA_POSITION_REAR_SECOND,
	CAMERA_POSITION_FORE_SECOND,
	CAMERA_POSITION_REAR_THIRD,
	CAMERA_POSITION_FORE_THIRD,
	CAMERA_POSITION_REAR_FORTH,
	CAMERA_POSITION_REAR_FIFTH,
	CAMERA_POSITION_MAX,
} driver_ic_position;

struct driver_ic_position_info {
	driver_ic_position ic_position;
	int ic_position_no;
	const char* ic_position_info;
};

typedef enum {
	DRIVER_ERROR_TYPE_INVALID             =    -1,
	CSI_PHY_ERR,
	BTB_CHECK_ERR,
	SENSOR_I2C_ERR,
	VCM_I2C_ERR,
	EEPROM_I2C_ERR,
	OIS_I2C_ERR,
	LED_FLASH_I2C_ERR,
	FPC_OPEN_CIRCUIT_ERR,
	LED_FLASH_OVER_FLOW_ERR,
	LED_FLASH_OVER_TEMPERATURE_ERR,
	LED_FLASH_SHORT_CIRCUIT_ERR,
	LED_FLASH_OVER_VOLTAGE_ERR,
	LED_FLASH_OPEN_CIRCUIT_ERR,
	DRIVER_ERROR_TYPE_MAX,
} driver_error_type;

struct driver_error_type_info {
	driver_error_type error_type;
	int error_type_base_no;
	const char* error_type_info;
};

struct hwcam_hievent_info {
	int error_no;
	char ic_name[HIVIEW_MAX_IC_NAME_LEN];
	char module_name[HIVIEW_MAX_MODULE_NAME_LEN];
	char content[HIVIEW_MAX_CONTENT_LEN];
};

struct driver_dmd_extra_info {
	char ic_name[HIVIEW_MAX_IC_NAME_LEN];
	char module_name[HIVIEW_MAX_MODULE_NAME_LEN];
	char error_scene[HIVIEW_MAX_CONTENT_LEN];
};

struct camera_dmd_info {
	driver_error_type error_type;
	driver_ic_position ic_position;
	struct driver_dmd_extra_info extra_info;
};

void camkit_hiview_report(struct camera_dmd_info* cam_info);
int camkit_hiview_init(struct camera_dmd_info* info);

#endif // CAMERA_DMD_H