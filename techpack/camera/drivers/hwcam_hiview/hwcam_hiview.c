

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/printk.h>
#include <linux/mutex.h>
#include "hwcam_hiview.h"
#include "debug_adapter.h"

#define PFX "[camhiview]"
#define log_inf(fmt, args...) pr_info(PFX "%s %d " fmt, __func__, __LINE__, ##args)
#define log_err(fmt, args...) pr_err(PFX "%s %d " fmt, __func__, __LINE__, ##args)

void cam_hiview_handle(int error_type,
	struct cam_sensor_ctrl_t* s_ctrl, const char* error_scene)
{
	struct camera_dmd_info cam_info;

	if (camkit_hiview_init(&cam_info)) {
		log_err("camkit hiview init failed");
		return;
	}

	cam_info.error_type = error_type;
	if (s_ctrl) {
		cam_info.ic_position = s_ctrl->sensordata->ic_position;
		if (sprintf_s(cam_info.extra_info.ic_name, sizeof(cam_info.extra_info.ic_name),
			"0x%x", s_ctrl->sensordata->slave_info.sensor_id) < 0) {
			log_err("sprintf_s fail");
			return;
		}
	}

	if (error_scene) {
		if (strncpy_s(cam_info.extra_info.error_scene, sizeof(cam_info.extra_info.error_scene),
			error_scene, sizeof(cam_info.extra_info.error_scene) - 1)) {
			log_err("strncpy_s fail");
			return;
		}
	}

	camkit_hiview_report(&cam_info);
}

void cam_i2c_error_hiview_handle(struct camera_io_master *io_master_info,
	const char* error_scene)
{
	int error_type;
	struct cam_actuator_ctrl_t* a_ctrl = NULL;
	struct cam_eeprom_ctrl_t *e_ctrl = NULL;
	struct cam_ois_ctrl_t *o_ctrl = NULL;
	struct cam_sensor_ctrl_t *s_ctrl = NULL;

	if (!io_master_info) {
		log_err("io_master_info is InValid");
		return;
	}

	switch (io_master_info->device_type) {
	case CAM_SENSOR:
		error_type = SENSOR_I2C_ERR;
		s_ctrl = container_of(io_master_info, struct cam_sensor_ctrl_t, io_master_info);
		break;
	case CAM_ACTUATOR:
		error_type = VCM_I2C_ERR;
		a_ctrl = container_of(io_master_info, struct cam_actuator_ctrl_t, io_master_info);
		s_ctrl = get_sensor_ctrl(a_ctrl->soc_info.index, CAM_ACTUATOR);
		break;
	case CAM_EEPROM:
		error_type = EEPROM_I2C_ERR;
		e_ctrl = container_of(io_master_info, struct cam_eeprom_ctrl_t, io_master_info);
		s_ctrl = get_sensor_ctrl(e_ctrl->soc_info.index, CAM_EEPROM);
		break;
	case CAM_OIS:
		error_type = OIS_I2C_ERR;
		o_ctrl = container_of(io_master_info, struct cam_ois_ctrl_t, io_master_info);
		s_ctrl = get_sensor_ctrl(o_ctrl->soc_info.index, CAM_OIS);
		break;
	default:
		log_err("Not support DMD error device Type: %d", io_master_info->device_type);
		return;
	}

	if (!s_ctrl) {
		log_err("sensor ctrl is InValid");
		return;
	}
	return cam_hiview_handle(error_type, s_ctrl, error_scene);
}
