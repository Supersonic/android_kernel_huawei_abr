/*
 *  vi5300_module.c - Linux kernel modules for VI5300 FlightSense TOF
 *						 sensor
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/atomic.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/of_gpio.h>
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/time.h>
#include <securec.h>
#include <hwmanufac/dev_detect/dev_detect.h>
#include <cam_sensor_util.h>
#include <cam_laser.h>

#include "cam_laser_dev.h"
#include "vi5300.h"
#include "vi5300_platform.h"
#include "vi5300_firmware.h"
#include "vi5300_api.h"

#define VI5300_IOCTL_PERIOD _IOW('p', 0x01, uint32_t)
#define VI5300_IOCTL_XTALK_CALIB _IOR('p', 0x02, struct VI5300_XTALK_Calib_Data)
#define VI5300_IOCTL_XTALK_CONFIG _IOW('p', 0x03, int8_t)
#define VI5300_IOCTL_OFFSET_CALIB _IOR('p', 0x04, struct VI5300_OFFSET_Calib_Data)
#define VI5300_IOCTL_OFFSET_CONFIG _IOW('p', 0x05, int16_t)
#define VI5300_IOCTL_POWER_ON _IO('p', 0x06)
#define VI5300_IOCTL_CHIP_INIT _IO('p', 0x07)
#define VI5300_IOCTL_START _IO('p', 0x08)
#define VI5300_IOCTL_STOP _IO('p', 0x09)
#define VI5300_IOCTL_MZ_DATA _IOR('p', 0x0a, struct VI5300_Measurement_Data)
#define VI5300_IOCTL_POWER_OFF _IO('p', 0x0b)

#define VI5300_DRV_NAME "vi5300"
#define  HELLO_DEVICE_CLASS_NAME "input"
static int xtalk_mark;
static int offset_mark;
static int enable_sensor;

static struct sensors_classdev sensors_proxi_cdev = {
  .name = "laser",
  .vendor = "VIDAR",
  .version = 1,
  .handle = SENSORS_PROXIMITY_HANDLE,
  .type = SENSOR_TYPE_PROXIMITY,
  .max_range = "PROXIMITY_MAX_RANGE",
  .resolution = "1",
  .sensor_power = "18",
  .min_delay = PROXIMITY_MIN_DELAY,
  .fifo_reserved_event_count = PROXIMITY_FIFO_RESERVED_COUNT,
  .fifo_max_event_count = PROXIMITY_FIFO_MAX_COUNT,
  .enabled = 0,
  .delay_msec = 30,
  .sensors_enable = NULL,
  .sensors_poll_delay = NULL,
};

struct vi5300_api_fn_t {
	int32_t (*Power_ON)(VI5300_DEV dev);
	int32_t (*Power_OFF)(VI5300_DEV dev);
	void (*Chip_Register_Init)(VI5300_DEV dev);
	void (*Set_Period)(VI5300_DEV dev, uint32_t period);
	int32_t (*Single_Measure)(VI5300_DEV dev);
	int32_t (*Start_Continuous_Measure)(VI5300_DEV dev);
	int32_t (*Stop_Continuous_Measure)(VI5300_DEV dev);
	int32_t (*Get_Measure_Data)(VI5300_DEV dev);
	int32_t (*Get_Interrupt_State)(VI5300_DEV dev);
	int32_t (*Chip_Init)(VI5300_DEV dev);
	int32_t (*Start_XTalk_Calibration)(VI5300_DEV dev);
	int32_t (*Start_Offset_Calibration)(VI5300_DEV dev);
	int32_t (*Get_XTalk_Parameter)(VI5300_DEV dev);
	int32_t (*Config_XTalk_Parameter)(VI5300_DEV dev);
};
static struct vi5300_api_fn_t vi5300_api_func_tbl = {
	.Power_ON = VI5300_Chip_PowerON,
	.Power_OFF = VI5300_Chip_PowerOFF,
	.Chip_Register_Init = VI5300_Chip_Register_Init,
	.Set_Period = VI5300_Set_Period,
	.Single_Measure = VI5300_Single_Measure,
	.Start_Continuous_Measure = VI5300_Start_Continuous_Measure,
	.Stop_Continuous_Measure = VI5300_Stop_Continuous_Measure,
	.Get_Measure_Data = VI5300_Get_Measure_Data,
	.Get_Interrupt_State = VI5300_Get_Interrupt_State,
	.Chip_Init = VI5300_Chip_Init,
	.Start_XTalk_Calibration = VI5300_Start_XTalk_Calibration,
	.Start_Offset_Calibration = VI5300_Start_Offset_Calibration,
	.Get_XTalk_Parameter = VI5300_Get_XTalk_Parameter,
	.Config_XTalk_Parameter = VI5300_Config_XTalk_Parameter,
};
struct vi5300_api_fn_t *vi5300_func_tbl;

static void vi5300_setupAPIFunctions(void)
{
	vi5300_func_tbl->Power_ON = VI5300_Chip_PowerON;
	vi5300_func_tbl->Power_OFF = VI5300_Chip_PowerOFF;
	vi5300_func_tbl->Chip_Register_Init = VI5300_Chip_Register_Init;
	vi5300_func_tbl->Set_Period = VI5300_Set_Period;
	vi5300_func_tbl->Single_Measure = VI5300_Single_Measure;
	vi5300_func_tbl->Start_Continuous_Measure = VI5300_Start_Continuous_Measure;
	vi5300_func_tbl->Stop_Continuous_Measure = VI5300_Stop_Continuous_Measure;
	vi5300_func_tbl->Get_Measure_Data = VI5300_Get_Measure_Data;
	vi5300_func_tbl->Get_Interrupt_State = VI5300_Get_Interrupt_State;
	vi5300_func_tbl->Chip_Init = VI5300_Chip_Init;
	vi5300_func_tbl->Start_XTalk_Calibration = VI5300_Start_XTalk_Calibration;
	vi5300_func_tbl->Start_Offset_Calibration = VI5300_Start_Offset_Calibration;
	vi5300_func_tbl->Get_XTalk_Parameter = VI5300_Get_XTalk_Parameter;
	vi5300_func_tbl->Config_XTalk_Parameter = VI5300_Config_XTalk_Parameter;
}

static void vi5300_enable_irq(struct  vi5300_data *data)
{
	if(!data)
		return;

	if(data->intr_state == VI5300_INTR_DISABLED)
	{
		data->intr_state = VI5300_INTR_ENABLED;
		enable_irq(data->irq);
	}
}

static void vi5300_disable_irq(struct  vi5300_data *data)
{
	if(!data)
		return;

	if(data->intr_state == VI5300_INTR_ENABLED)
	{
		data->intr_state = VI5300_INTR_DISABLED;
		disable_irq(data->irq);
	}
}

static ssize_t vi5300_chip_enable_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;

	if(NULL != data)
		return scnprintf(buf, PAGE_SIZE, "%u\n", data->chip_enable);

	return -EINVAL;
}

static ssize_t vi5300_chip_enable_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;
	VI5300_Error Status = VI5300_ERROR_NONE;
	unsigned int val = 0;

	if(NULL != data)
	{
		mutex_lock(&data->work_mutex);
		if(sscanf(buf, "%u\n", &val) != 1)
		{
			mutex_unlock(&data->work_mutex);
			return -1;
		}
		if(val !=0 && val !=1)
		{
			vi5300_errmsg("enable store unvalid value=%u\n", val);
			mutex_unlock(&data->work_mutex);
			return count;
		}
		if(val == 1)
		{
			if(data->chip_enable == 0)
			{
				data->chip_enable = 1;
				vi5300_enable_irq(data);
				Status = vi5300_func_tbl->Power_ON(data);
				ktime_get_real_ts64(&data->start_ts);

				Status = vi5300_func_tbl->Chip_Init(data); // download firmware
				vi5300_errmsg("vi5300_chip_init_store Chip_Init ok");
				data->fwdl_status = 1;
			} else {
				vi5300_errmsg("already enabled!!\n");
			}
		} else {
			if(data->chip_enable == 1)
			{
				data->chip_enable = 0;
				vi5300_disable_irq(data);
				data->fwdl_status = 0;
				Status = vi5300_func_tbl->Power_OFF(data);
			}
			else {
				vi5300_errmsg("already disabled!!\n");
			}
		}
		mutex_unlock(&data->work_mutex);
		return Status ? -1 : count;
	}

	return -EPERM;
}

static DEVICE_ATTR(chip_enable, 0664, vi5300_chip_enable_show, vi5300_chip_enable_store);
/* for debug */
static ssize_t vi5300_enable_debug_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;

	if(NULL != data)
		return snprintf(buf, PAGE_SIZE, "%u\n", data->enable_debug);

	return -EINVAL;
}

static ssize_t vi5300_enable_debug_store(struct device *dev,
					struct device_attribute *attr, const
					char *buf, size_t count)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;
	unsigned int on = 0;

	if(NULL != data)
	{
		mutex_lock(&data->work_mutex);
		if(sscanf(buf, "%u\n", &on) != 1)
		{
			mutex_unlock(&data->work_mutex);
			return -1;
		}
		if ((on != 0) &&  (on != 1)) {
			vi5300_errmsg("set debug=%d\n", on);
			mutex_unlock(&data->work_mutex);
			return count;
		}
		mutex_unlock(&data->work_mutex);
		data->enable_debug = on;
		return count;
	}
	return -EINVAL;
}

static DEVICE_ATTR(enable_debug, 0664, vi5300_enable_debug_show, vi5300_enable_debug_store);
static ssize_t vi5300_chip_init_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;
	VI5300_Error Status = VI5300_ERROR_NONE;
	unsigned int val = 0;

	if(NULL != data)
	{
		mutex_lock(&data->work_mutex);
		if(sscanf(buf, "%u\n", &val) != 1)
		{
			vi5300_errmsg("vi5300_chip_init_store sscanf failed");
			mutex_unlock(&data->work_mutex);
			return -1;
		}

		if(val)
		{
			Status = vi5300_func_tbl->Chip_Init(data);
			vi5300_errmsg("vi5300_chip_init_store Chip_Init ok");
			data->fwdl_status = 1;
		} else {
			mutex_unlock(&data->work_mutex);
			return -EINVAL;
		}
		mutex_unlock(&data->work_mutex);
		return Status ? -1 : count;
	}

	return -EPERM;
}

static DEVICE_ATTR(chip_init, 0664, NULL, vi5300_chip_init_store);

static ssize_t vi5300_capture_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;
	VI5300_Error Status = VI5300_ERROR_NONE;
	unsigned int val = 0;

	if(NULL != data)
	{
		mutex_lock(&data->work_mutex);
		if(sscanf(buf, "%u\n", &val) != 1)
		{
			mutex_unlock(&data->work_mutex);
			return -1;
		}
		if(val !=0 && val !=1)
		{
			vi5300_errmsg("capture store unvalid value=%u\n", val);
			mutex_unlock(&data->work_mutex);
			return count;
		}
		if(val == 1)
		{
			if (enable_sensor == 0) {
				vi5300_infomsg("start Start_Continuous_Measure", val);
				Status = vi5300_func_tbl->Start_Continuous_Measure(data);
				enable_sensor = 1;
			}
		} else {
			if (enable_sensor == 1) {
				vi5300_infomsg("start Stop_Continuous_Measure", val);
				Status = vi5300_func_tbl->Stop_Continuous_Measure(data);
				enable_sensor = 0;
				/* 0： clear input sys data when vi5300 stop capture */
				input_report_abs(data->input_dev, ABS_HAT0Y, 0);
				input_report_abs(data->input_dev, ABS_HAT1X, 0);
				input_report_abs(data->input_dev, ABS_HAT1Y, 0);
				input_report_abs(data->input_dev, ABS_BRAKE, 0);
				input_report_abs(data->input_dev, ABS_TILT_X, 0);
				input_report_abs(data->input_dev, ABS_WHEEL, 0);
				input_sync(data->input_dev);
			}
		}
		mutex_unlock(&data->work_mutex);
		return Status ? -1 : count;
	}

	return -EPERM;
}

static DEVICE_ATTR(capture, 0664, NULL, vi5300_capture_store);

static ssize_t vi5300_xtalk_calib_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;

	if(data != NULL)
		return scnprintf(buf, PAGE_SIZE, "%d.%u\n", data->XtalkData.xtalk_cal, data->XtalkData.xtalk_peak);

	return -EPERM;
}

static ssize_t vi5300_xtalk_calib_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;
	VI5300_Error Status = VI5300_ERROR_NONE;
	unsigned int val = 0;

	if(NULL != data)
	{
		mutex_lock(&data->work_mutex);
		if(sscanf(buf, "%u\n", &val) != 1)
		{
			mutex_unlock(&data->work_mutex);
			return -1;
		}
		if(val !=1)
		{
			vi5300_errmsg("xtalk calibration store unvalid value=%u\n", val);
			mutex_unlock(&data->work_mutex);
			return count;
		}
		xtalk_mark = 1;
		Status = vi5300_func_tbl->Start_XTalk_Calibration(data);
		mdelay(600);
		xtalk_mark = 0;
		data->xtalk_config = data->XtalkData.xtalk_cal;
		Status = vi5300_func_tbl->Config_XTalk_Parameter(data);
		if(Status != VI5300_ERROR_NONE)
		{
			vi5300_errmsg("%d, AFTER CALIBRATION,CONFIG XTALK PARAMETER FAILED\n", __LINE__);
			mutex_unlock(&data->work_mutex);
			return -EINVAL;
		}
		mutex_unlock(&data->work_mutex);
		return Status ? -1 : count;
	}

	return -EPERM;
}

static DEVICE_ATTR(xtalk_calib, 0664, vi5300_xtalk_calib_show, vi5300_xtalk_calib_store);

static ssize_t vi5300_offset_calib_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;

	if(data != NULL)
		return scnprintf(buf, PAGE_SIZE, "%d\n", data->OffsetData.offset_cal);

	return -EPERM;
}

static ssize_t vi5300_offset_calib_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;
	VI5300_Error Status = VI5300_ERROR_NONE;
	unsigned int val = 0;

	if(NULL != data)
	{
		mutex_lock(&data->work_mutex);
		if(sscanf(buf, "%u\n", &val) != 1)
		{
			mutex_unlock(&data->work_mutex);
			return -1;
		}
		if(val !=1)
		{
			vi5300_errmsg("offset calibration store unvalid value=%u\n", val);
			mutex_unlock(&data->work_mutex);
			return count;
		}
		offset_mark = 1;
		Status = vi5300_func_tbl->Start_Offset_Calibration(data);
		offset_mark = 0;
		data->offset_config = data->OffsetData.offset_cal;
		mutex_unlock(&data->work_mutex);
		return Status ? -1 : count;
	}

	return -EPERM;
}

static DEVICE_ATTR(offset_calib, 0664, vi5300_offset_calib_show, vi5300_offset_calib_store);

static ssize_t vi5300_xtalk_config_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;
	VI5300_Error Status = VI5300_ERROR_NONE;
	int val = 0;

	if(NULL != data)
	{
		mutex_lock(&data->work_mutex);
		if(sscanf(buf, "%x\n", &val) != 1)
		{
			mutex_unlock(&data->work_mutex);
			return -1;
		}
		data->xtalk_config = (int8_t)val;
		Status = vi5300_func_tbl->Config_XTalk_Parameter(data);
		mutex_unlock(&data->work_mutex);
		return Status ? -1 : count;
	}

	return -EPERM;
}

static DEVICE_ATTR(xtalk_config, 0220, NULL, vi5300_xtalk_config_store);

static ssize_t vi5300_offset_config_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	struct cam_laser_ctrl_t *o_ctrl = dev_get_drvdata(dev);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;
	int val = 0;

	if(NULL != data)
	{
		mutex_lock(&data->work_mutex);
		if(sscanf(buf, "%x\n", &val) != 1)
		{
			mutex_unlock(&data->work_mutex);
			return -1;
		}
		data->xtalk_config = (int16_t)val;
		mutex_unlock(&data->work_mutex);
		return count;
	}

	return -EPERM;
}

static DEVICE_ATTR(offset_config, 0220, NULL, vi5300_offset_config_store);

static struct attribute *vi5300_attributes[] = {
	&dev_attr_chip_enable.attr,
	&dev_attr_enable_debug.attr,
	&dev_attr_chip_init.attr,
	&dev_attr_capture.attr,
	NULL,
};

static struct attribute *vi5300_calib_attrs[] = {
	&dev_attr_xtalk_calib.attr,
	&dev_attr_offset_calib.attr,
	&dev_attr_xtalk_config.attr,
	&dev_attr_offset_config.attr,
	NULL,
};

static const struct attribute_group vi5300_attr_group = {
	.name = NULL,
	.attrs = vi5300_attributes,
};

static const struct attribute_group vi5300_calib_group = {
	.name = "calib",
	.attrs = vi5300_calib_attrs,
};

static const struct attribute_group *tof_groups[] = {
	&vi5300_attr_group,
	&vi5300_calib_group,
  NULL,
};

static irqreturn_t vi5300_irq_handler(int vec, void *info)
{
	struct vi5300_data *data = (struct vi5300_data *)info;
	VI5300_Error Status = VI5300_ERROR_NONE;


	if(!data || !data->fwdl_status)
		return IRQ_HANDLED;

	if (data->irq == vec)
	{
		if(xtalk_mark)
		{
			Status = vi5300_func_tbl->Get_XTalk_Parameter(data);
			if(Status != VI5300_ERROR_NONE)
				vi5300_errmsg("%d : Status = %d\n" , __LINE__, Status);
		}

		if(!xtalk_mark && !offset_mark)
		{
			Status = vi5300_func_tbl->Get_Measure_Data(data);
			if(Status != VI5300_ERROR_NONE)
			{
				vi5300_errmsg("%d : Status = %d\n" , __LINE__, Status);
				return IRQ_HANDLED;
			}
			input_report_abs(data->input_dev, ABS_HAT0Y, data->Rangedata.timeUSec);
			input_report_abs(data->input_dev, ABS_HAT1X, data->Rangedata.Obj_Range);
			input_report_abs(data->input_dev, ABS_HAT1Y, data->Rangedata.RangeLimit);
			input_report_abs(data->input_dev, ABS_BRAKE, data->Rangedata.RangeStatus);
			input_report_abs(data->input_dev, ABS_TILT_X, data->Rangedata.NumberOfObject);
			input_report_abs(data->input_dev, ABS_WHEEL, data->Rangedata.AmbientRate);
			input_sync(data->input_dev);
		}
	}
	return IRQ_HANDLED;
}

static int vi5300_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int ctrl_perform_calibration_crosstalk_lock(struct vi5300_data *data)
{
	int rc = 0;

	mutex_lock(&data->work_mutex);
	xtalk_mark = 1;
	rc = vi5300_func_tbl->Start_XTalk_Calibration(data);
	if(rc != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("%d, PERFORM XTALK CALIB FAILED\n", __LINE__);
		mutex_unlock(&data->work_mutex);
		return -EINVAL;
	}
	msleep(600);

	xtalk_mark = 0;
	data->xtalk_config = data->XtalkData.xtalk_cal;
	rc = vi5300_func_tbl->Config_XTalk_Parameter(data);
	if(rc != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("%d, AFTER CALIBRATION,CONFIG XTALK PARAMETER FAILED\n", __LINE__);
		mutex_unlock(&data->work_mutex);
		return -EINVAL;
	}
	mutex_unlock(&data->work_mutex);

	return rc;
}

static int ctrl_perform_calibration_offset_lock(struct vi5300_data *data)
{
	int rc = 0;

	mutex_lock(&data->work_mutex);
	offset_mark = 1;
	rc = vi5300_func_tbl->Start_Offset_Calibration(data);
	if(rc != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("%d, PERFORM OFFSET CALIB FAILED\n", __LINE__);
		mutex_unlock(&data->work_mutex);
		return -EINVAL;
	}

	offset_mark = 0;
	data->offset_config = data->OffsetData.offset_cal;
	mutex_unlock(&data->work_mutex);

	return rc;
}

static int ctrl_perform_calibration(struct vi5300_data *data, void __user *p)
{
	int rc = 0;
	hwlaser_ioctl_perform_calibration_t calib;

	rc = copy_from_user(&calib, (struct hwlaser_ioctl_perform_calibration_t *)p, sizeof(calib));
	if (rc) {
		rc = -EFAULT;
		goto done;
	}

	switch (calib.calibration_type) {
		case HWLASER_CALIBRATION_CROSSTALK:
			rc = ctrl_perform_calibration_crosstalk_lock(data);
			vi5300_infomsg("HWLASER_CALIBRATION_CROSSTALK rc => %d", rc);
			break;
		case HWLASER_CALIBRATION_OFFSET_SIMPLE:
			rc = ctrl_perform_calibration_offset_lock(data);
			vi5300_infomsg("HWLASER_CALIBRATION_OFFSET_SIMPLE rc => %d", rc);
			break;
		default:
			vi5300_errmsg("unsupport calibration type");
			rc = -EINVAL;
			break;
		}

done:
	return rc;
}

static int VI5300_GetCalibrationData(struct vi5300_data *data, hwlaser_calibration_data_t *cal_data)
{
	int rc = 0;

	/* xtalk */
	rc = memcpy_s(&(cal_data->u.dataL3.VI5300_XTALK_Calib_Data), sizeof(hwlaser_calibration_xtalk), &(data->XtalkData), sizeof(struct VI5300_XTALK_Calib_Data));
	if (rc) {
		vi5300_errmsg("xtalk memcpy fail");
		goto done;
	}

	/* offset */
	rc = memcpy_s(&(cal_data->u.dataL3.VI5300_OFFSET_Calib_Data), sizeof(hwlaser_calibration_offset), &(data->OffsetData), sizeof(struct VI5300_OFFSET_Calib_Data));
	if (rc) {
		vi5300_errmsg("offset memcpy fail");
		goto done;
	}


done:
	return rc;
}

static int VI5300_SetCalibrationData(struct vi5300_data *data, hwlaser_calibration_data_t *cal_data)
{
	int rc = 0;

	/* send xtalk to driver */
	rc = memcpy_s(&(data->xtalk_config), sizeof(int8_t), &(cal_data->u.dataL3.VI5300_XTALK_Calib_Data.xtalk_cal), sizeof(int8_t));
	if (rc) {
		vi5300_errmsg("driver get xtalk memcpy fail");
		goto done;
	}

	vi5300_infomsg("driver get xtalk config: %d\n", data->xtalk_config);
	rc = vi5300_func_tbl->Config_XTalk_Parameter(data);
	if (rc)
	{
		vi5300_errmsg("%d, CONFIG XTALK PARAMETER FAILED\n", __LINE__);
		goto done;
	}

	/* send offset to driver */
	rc = memcpy_s(&(data->offset_config), sizeof(int16_t), &(cal_data->u.dataL3.VI5300_OFFSET_Calib_Data.offset_cal), sizeof(int16_t));
	if (rc) {
		vi5300_errmsg("driver get offset memcpy fail");
		goto done;
	}
	vi5300_infomsg("driver get offset config: %d\n", data->offset_config);

done:
	return rc;
}

static int vi5300_laser_get_set_cal_data(struct vi5300_data* data,
											hwlaser_calibration_data_t *p)
{
	int rc = 0;
	hwlaser_calibration_data_t cal_data;
	copy_from_user(&cal_data, (hwlaser_calibration_data_t *)p, sizeof(hwlaser_calibration_data_t));

	mutex_lock(&data->work_mutex);

	if (cal_data.is_read) {
		vi5300_infomsg("hal get calibration data.");
		memset(&(cal_data.u.dataL3), 0, sizeof(hwlaser_calibration_data_L3));
		rc = VI5300_GetCalibrationData(data, &cal_data);
		if (rc) {
			vi5300_errmsg("VI5300_GetCalibrationData fail %d", rc);
			goto done;
		}
		rc = copy_to_user(&(p->u.dataL3.VI5300_XTALK_Calib_Data), &(cal_data.u.dataL3.VI5300_XTALK_Calib_Data), sizeof(hwlaser_calibration_xtalk));
		if (rc) {
			vi5300_errmsg("xtalk copy to user fail");
			goto done;
		}
		rc = copy_to_user(&(p->u.dataL3.VI5300_OFFSET_Calib_Data), &(cal_data.u.dataL3.VI5300_OFFSET_Calib_Data), sizeof(hwlaser_calibration_offset));
		if (rc) {
			vi5300_errmsg("offset copy to user fail");
			goto done;
		}
	} else {
		vi5300_infomsg("hal set calibration data to driver.");
		rc = VI5300_SetCalibrationData(data, &cal_data);
		if (rc) {
			vi5300_errmsg("VI5300_SetCalibrationData fail %d", rc);
			goto done;
		}

	}

	vi5300_infomsg("vi5300 xtalk_peak is(%u), talk_cal is(%d), offset is(%d)", \
				cal_data.u.dataL3.VI5300_XTALK_Calib_Data.xtalk_peak, \
				cal_data.u.dataL3.VI5300_XTALK_Calib_Data.xtalk_cal, \
				cal_data.u.dataL3.VI5300_OFFSET_Calib_Data.offset_cal);

done:
	mutex_unlock(&data->work_mutex);
	return rc;
}

static int vi5300_laser_get_data(struct vi5300_data *data, void * p)
{
	int rc;
	uint32_t distance = 0;
	uint32_t status = 0;

	if(NULL == data || NULL == p)
		return -EINVAL;
	mutex_lock(&data->work_mutex);

	copy_from_user(&(data->udata), p, sizeof(hwlaser_RangingData_t));
	distance = data->Rangedata.Obj_Range >> 16;
	status = data->Rangedata.RangeStatus;

	data->udata.u.data_1.distance = distance;
	data->udata.u.data_1.status = status;

	rc = copy_to_user((hwlaser_RangingData_t *)p, &(data->udata), sizeof(hwlaser_RangingData_t));
	if (rc) {
		vi5300_errmsg("Rangingdata copy to user failed.");
		mutex_unlock(&data->work_mutex);
		return -EFAULT;
	}
	mutex_unlock(&data->work_mutex);

	vi5300_infomsg("vi5300 distance is(%u), status is(%d)", \
				data->udata.u.data_1.distance, \
				data->udata.u.data_1.status);
	return 0;
}

static long vi5300_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	long rc=0;
	void __user *p = (void __user *)arg;
	struct vi5300_data *data = container_of(file->private_data,
		struct vi5300_data, miscdev);
	hwlaser_calibration_data_t *cal_data = NULL;

	if(!data)
		return -EFAULT;

	switch (cmd) {

		case HWLASER_IOCTL_GET_INFO:
			vi5300_infomsg("HWLASER_IOCTL_GET_INFO");
			if (strlen(VI5300_NAME) < HWLASER_NAME_SIZE) {
				strncpy_s(data->pinfo.name, strlen(VI5300_NAME), VI5300_NAME, strlen(VI5300_NAME));
			}
			data->pinfo.i2c_idx = 2;
			data->pinfo.valid = 1;
			data->pinfo.version = HWLASER_VI_I0_VERSION;
			data->pinfo.ap_pos = HWLASER_POS_AP;
			if (strlen(VI5300_NAME) < HWLASER_NAME_SIZE) {
 				copy_to_user((hwlaser_info_t *)p, &(data->pinfo), sizeof(hwlaser_info_t));
			}
		break;
		case HWLASER_IOCTL_POWERON:
			vi5300_infomsg("HWLASER_IOCTL_POWERON");
			mutex_lock(&data->work_mutex);
			vi5300_enable_irq(data);
			rc = vi5300_func_tbl->Power_ON(data);
			if(rc != VI5300_ERROR_NONE)
			{
				vi5300_errmsg("%d, CHIP POWER ON FAILED\n", __LINE__);
				mutex_unlock(&data->work_mutex);
				return -EIO;
			}
			ktime_get_real_ts64(&data->start_ts);
			mutex_unlock(&data->work_mutex);
			break;
		case HWLASER_IOCTL_INIT:
			vi5300_infomsg("HWLASER_IOCTL_INIT");
			mutex_lock(&data->work_mutex);
			rc = vi5300_func_tbl->Chip_Init(data);
			if(rc != VI5300_ERROR_NONE)
			{
				vi5300_errmsg("%d, CHIP INIT FAILED\n", __LINE__);
				mutex_unlock(&data->work_mutex);
				return -EIO;
			}
			data->fwdl_status = 1;
			mutex_unlock(&data->work_mutex);
			break;
		case HWLASER_IOCTL_PERFORM_CALIBRATION:
			vi5300_infomsg("HWLASER_IOCTL_PERFORM_CALIBRATION");
			rc = ctrl_perform_calibration(data, p);
		break;
		case HWLASER_IOCTL_CALIBRATION_DATA:
			vi5300_infomsg("HWLASER_IOCTL_CALIBRATION_DATA");
			cal_data = (hwlaser_calibration_data_t *)p;
			rc = vi5300_laser_get_set_cal_data(data, cal_data);
		break;
		case HWLASER_IOCTL_START:
			vi5300_infomsg("HWLASER_IOCTL_START");
			mutex_lock(&data->work_mutex);
			if (enable_sensor == 0) {
				vi5300_infomsg("enter HWLASER_IOCTL_START");
				rc = vi5300_func_tbl->Start_Continuous_Measure(data);
				if(rc != VI5300_ERROR_NONE)
				{
					vi5300_errmsg("%d, CHIP START RANGE FAILED\n", __LINE__);
					mutex_unlock(&data->work_mutex);
					return -EIO;
				}
				enable_sensor = 1;
			}
			mutex_unlock(&data->work_mutex);
			break;
		case HWLASER_IOCTL_MZ_DATA:
			vi5300_infomsg("HWLASER_IOCTL_MZ_DATA");
			rc = vi5300_laser_get_data(data, p);
			break;
		case HWLASER_IOCTL_STOP:
			vi5300_infomsg("HWLASER_IOCTL_STOP");
			mutex_lock(&data->work_mutex);
			if (enable_sensor == 1) {
				vi5300_infomsg("enter HWLASER_IOCTL_STOP");
				rc = vi5300_func_tbl->Stop_Continuous_Measure(data);
				if(rc != VI5300_ERROR_NONE)
				{
					vi5300_errmsg("%d, CHIP STOP RANGE FAILED\n", __LINE__);
					mutex_unlock(&data->work_mutex);
					return -EIO;
				}
				enable_sensor = 0;
			}
			mutex_unlock(&data->work_mutex);
			break;
		case HWLASER_IOCTL_POWEROFF:
			vi5300_infomsg("HWLASER_IOCTL_POWEROFF");
			mutex_lock(&data->work_mutex);
			vi5300_disable_irq(data);
			data->fwdl_status = 0;
			rc = vi5300_func_tbl->Power_OFF(data);
			if(rc != VI5300_ERROR_NONE)
			{
				vi5300_errmsg("%d, CHIP POWER OFF FAILED\n", __LINE__);
				mutex_unlock(&data->work_mutex);
				return -EIO;
			}
			mutex_unlock(&data->work_mutex);
			break;
		default:
			rc = -EFAULT;
	}

	return rc;

}

static int vi5300_flush(struct file *file, fl_owner_t id)
{
	return 0;
}

static const struct file_operations vi5300_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = vi5300_ioctl,
	.open = vi5300_open,
	.flush = vi5300_flush,
};
static int get_xsdn(struct device *dev, struct vi5300_data *vi5300_data)
{
	int rc = 0;

	if (vi5300_data->xshut_gpio == -1) {
		vi5300_errmsg("reset gpio is required");
		rc = -ENODEV;
		goto no_gpio;
	}

	vi5300_infomsg("request xshut_gpio %d", vi5300_data->xshut_gpio);
	rc = gpio_request(vi5300_data->xshut_gpio, "vi5300_xshut");
	if (rc) {
		vi5300_errmsg("fail to acquire xshut %d", rc);
		goto request_failed;
	}

request_failed:
no_gpio:
	return rc;
}

static int get_intr(struct device *dev, struct vi5300_data *vi5300_data)
{
	int rc = 0;

	if (vi5300_data->irq_gpio == -1) {
		vi5300_errmsg("no interrupt gpio");
		goto no_gpio;
	}

	vi5300_infomsg("request irq_gpio %d", vi5300_data->irq_gpio);
	rc = gpio_request(vi5300_data->irq_gpio, "vi5300_irq");
	if (rc) {
		vi5300_errmsg("fail to acquire irq %d", rc);
		goto request_failed;
	}

	rc = gpio_direction_input(vi5300_data->irq_gpio);
	if (rc) {
		vi5300_errmsg("fail to configure irq as input %d", rc);
		goto direction_failed;
	}

	vi5300_data->irq = gpio_to_irq(vi5300_data->irq_gpio);
	vi5300_errmsg("vi5300_data->irq is %d", vi5300_data->irq);
	if (vi5300_data->irq < 0) {
		vi5300_errmsg("fail to map GPIO: %d to interrupt:%d",
				vi5300_data->irq_gpio, vi5300_data->irq);
		goto irq_failed;
	}

	return rc;

irq_failed:
direction_failed:
	gpio_free(vi5300_data->irq_gpio);

request_failed:
no_gpio:
	return rc;
}

int vi5300_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct vi5300_data *vi5300_data = NULL;
	struct device *dev = &client->dev;
	int ret  = 0;
	uint8_t buf;
	struct cam_laser_ctrl_t *o_ctrl = i2c_get_clientdata(client);

	CAM_ERR(CAM_LASER, "enter vi5300_probe");
	vi5300_data = kzalloc(sizeof(struct vi5300_data), GFP_KERNEL);
	if(!vi5300_data)
	{
		vi5300_errmsg("devm_kzalloc error\n");
		return -ENOMEM;
	}
	if (!dev->of_node)
	{
		kfree(vi5300_data);
		return -EINVAL;
	}
	/* setup device data */
	vi5300_data->dev_name = dev_name(&client->dev);
	vi5300_data->client = client;
	vi5300_data->dev = dev;
	o_ctrl->soc_info.soc_private = (void *)vi5300_data;
	o_ctrl->soc_info.dev_name = VI5300_DRV_NAME;
	mutex_init(&vi5300_data->work_mutex);
	vi5300_data->xshut_gpio = o_ctrl->soc_info.gpio_data->cam_gpio_common_tbl[0].gpio;
	vi5300_data->irq_gpio = o_ctrl->soc_info.gpio_data->cam_gpio_common_tbl[1].gpio;
	/* configure gpios */
	ret = get_xsdn(dev, vi5300_data);
	if (ret)
		goto no_xshut;

	ret = get_intr(dev, vi5300_data);
	if (ret)
		goto no_intr;
	if (vi5300_data->irq >= 0) {
		ret = request_threaded_irq(vi5300_data->irq, NULL, vi5300_irq_handler,
			IRQF_TRIGGER_FALLING |IRQF_ONESHOT, "vi5300_interrupt", (void *)vi5300_data);
		if (ret) {
			vi5300_errmsg("%s(%d), Could not allocate VI5300_INT ! result:%d\n",__FUNCTION__, __LINE__, ret);
			goto exit_free_irq;
		}
	}

	// vi5300_data->irq = irq;
	vi5300_data->intr_state = VI5300_INTR_DISABLED;
	disable_irq(vi5300_data->irq);
	vi5300_data->fwdl_status = 0;
	vi5300_func_tbl = &vi5300_api_func_tbl;
	vi5300_setupAPIFunctions();
	vi5300_func_tbl->Power_ON(vi5300_data);
	vi5300_func_tbl->Chip_Register_Init(vi5300_data);
	vi5300_read_byte(vi5300_data, VI5300_REG_DEV_ADDR, &buf);
	vi5300_func_tbl->Power_OFF(vi5300_data);
	if(buf != VI5300_CHIP_ADDR)
	{
		vi5300_errmsg("VI5300 I2C Transfer Failed\n");
		ret = -EFAULT;
		goto exit_free_irq;
	}
	vi5300_infomsg("VI5300 I2C Transfer Successfully\n");

	vi5300_data->miscdev.minor = MISC_DYNAMIC_MINOR;
	vi5300_data->miscdev.name = VI5300_MISC_DEV_NAME;
	vi5300_data->miscdev.fops = &vi5300_fops;
	if (misc_register(&vi5300_data->miscdev) != 0)
	{
		vi5300_errmsg("Could not register misc. dev for VI5300 Sensor\n");
		ret = -ENOMEM;
		goto exit_free_irq;
	}

	o_ctrl->laser_attr_group = tof_groups;
	o_ctrl->class_dev = &sensors_proxi_cdev;

	vi5300_data->period = 30;
	vi5300_data->xtalk_config = 0;
	vi5300_data->offset_config = 0;
	vi5300_data->enable_debug = 0;
	vi5300_data->Rangedata.RangeStatus = 6;

	/* match id */
	CAM_INFO(CAM_LASER, "vi5300 match id success.");
	set_hw_dev_flag(DEV_I2C_LASER);
	return 0;

exit_free_irq:
	free_irq(vi5300_data->irq, vi5300_data);
	mutex_destroy(&vi5300_data->work_mutex);
no_intr:
no_xshut:
	CAM_ERR(CAM_LASER, "vi5300_probe failed");
	kfree(vi5300_data);
	return ret;
}

int vi5300_remove(struct i2c_client *client)
{
	struct cam_laser_ctrl_t *o_ctrl = i2c_get_clientdata(client);
	struct vi5300_data *data = (struct vi5300_data *)o_ctrl->soc_info.soc_private;

	if (!IS_ERR(data->miscdev.this_device) &&
			data->miscdev.this_device != NULL) {
		vi5300_dbgmsg("to unregister misc dev\n");
		misc_deregister(&data->miscdev);
	}
	if(data->xshut_gpio)
	{
		gpio_direction_output(data->xshut_gpio, 0);
		gpio_free(data->xshut_gpio);
	}
	if(data->irq_gpio)
	{
		free_irq(data->irq, data);
		gpio_free(data->irq_gpio);
	}
	mutex_destroy(&data->work_mutex);
	kfree(data);
	return 0;
}
