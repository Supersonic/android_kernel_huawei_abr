/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: sensors for ipa thermal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <linux/thermal.h>
#include <securec.h>
#include <linux/energy_model.h>

#define DEFAULT_SHELL_TEMPERATURE 25000
#define DEFAULT_SOC_TEMPERATURE 85000

const char *battery_tz_name = "battery";
const char *system_h_tz_name = "system_h";
const char *cluster0_tz_name[CAPACITY_OF_ARRAY];
const char *cluster1_tz_name[CAPACITY_OF_ARRAY];
const char *cluster2_tz_name[CAPACITY_OF_ARRAY];
const char *all_tz_name[CAPACITY_OF_ARRAY];

enum hw_peripheral_temp_chanel {
	DETECT_SYSTEM_H_CHANEL = 0,
};

char *hw_peripheral_chanel[] = {
	[DETECT_SYSTEM_H_CHANEL] = "system_h",
};

enum hw_ipa_tsens {
	IPA_CLUSTER_0 = 0,
	IPA_CLUSTER_1,
	IPA_CLUSTER_2,
	IPA_GPU,
};

int ipa_get_tsensor_id(const char *name)
{
	/*
	 * This is only used by tsens_max, and
	 * get_sensor_id_by_name will handle this error correctly.
	 */
	return -ENODEV;
}
EXPORT_SYMBOL_GPL(ipa_get_tsensor_id);

int ipa_get_sensor_value(u32 sensor, int *val)
{
	struct thermal_zone_device *tz = NULL;
	int ret = -EINVAL;
	u32 sensor_num = sizeof(all_tz_name) / sizeof(char *);

	if (val == NULL) {
		pr_err("%s param null\n", __func__);
		return ret;
	}

	if (sensor < sensor_num) {
		tz = thermal_zone_get_zone_by_name(all_tz_name[sensor]);
		if (IS_ERR(tz))
			return -ENODEV;

		ret = thermal_zone_get_temp(tz, val);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(ipa_get_sensor_value);

int ipa_get_periph_id(const char *name)
{
	int ret = -ENODEV;
	u32 id;
	u32 sensor_num = sizeof(hw_peripheral_chanel) / sizeof(char *);

	if (name == NULL)
		return ret;

	pr_debug("IPA periph sensor_num = %d\n", sensor_num);
	for (id = 0; id < sensor_num; id++) {
		pr_debug("IPA: sensor_name = %s, hw_tsensor_name %d = %s\n",
		         name, id, hw_peripheral_chanel[id]);

		if (strlen(name) == strlen(hw_peripheral_chanel[id]) &&
		    strncmp(name, hw_peripheral_chanel[id], strlen(name)) == 0) {
			ret = (int)id;
			pr_debug("sensor_id = %d\n", ret);
			return ret;
		}
	}

	return ret;
}
EXPORT_SYMBOL_GPL(ipa_get_periph_id);

int ipa_get_periph_value(u32 sensor, int *val)
{
	struct thermal_zone_device *tz = NULL;
	int ret = -EINVAL;

	if (sensor != DETECT_SYSTEM_H_CHANEL)
		return -ENODEV;

	if (val == NULL) {
		pr_err("%s param null\n", __func__);
		return ret;
	}

	tz = thermal_zone_get_zone_by_name(system_h_tz_name);
	if (IS_ERR(tz))
		return -ENODEV;

	ret = thermal_zone_get_temp(tz, val);

	return ret;
}
EXPORT_SYMBOL_GPL(ipa_get_periph_value);

int hw_battery_temperature(void)
{
	struct thermal_zone_device *tz = NULL;
	int temperature = 0;
	int ret;

	tz = thermal_zone_get_zone_by_name(battery_tz_name);
	if (IS_ERR(tz))
		return DEFAULT_SHELL_TEMPERATURE;

	ret = thermal_zone_get_temp(tz, &temperature);
	if (ret)
		return DEFAULT_SHELL_TEMPERATURE;

	return temperature;
}
EXPORT_SYMBOL_GPL(hw_battery_temperature);

static unsigned int get_cluster_temp(const char **tz_name)
{
	struct thermal_zone_device *tz = NULL;
	unsigned int max_temp, temp;
	int i;
	int ret = -EINVAL;
	u32 tz_num = sizeof(tz_name) / sizeof(char *);

	temp = 0;
	max_temp = temp;
	for (i = 0; i < tz_num; i++) {
		tz = thermal_zone_get_zone_by_name(tz_name[i]);
		if (!IS_ERR(tz)) {
			ret = thermal_zone_get_temp(tz, &temp);
			if (ret)
				continue;
		} else {
			continue;
		}

		if (temp > max_temp)
			max_temp = temp;
	}

	return max_temp;
}

unsigned int ipa_get_cluster_temp(int cid)
{
	unsigned int temp = 0;

	switch (cid) {
	case IPA_CLUSTER_0:
		temp = get_cluster_temp(cluster0_tz_name);
		break;
	case IPA_CLUSTER_1:
		temp = get_cluster_temp(cluster1_tz_name);
		break;
	case IPA_CLUSTER_2:
		temp = get_cluster_temp(cluster2_tz_name);
		break;
	default:
		pr_err("%s, invalid cluster id=%d\n", __func__, cid);
		break;
	}

	return temp;
}
EXPORT_SYMBOL_GPL(ipa_get_cluster_temp);

unsigned int ipa_get_gpu_temp(const char **tz_name)
{
	return get_cluster_temp(tz_name);
}
EXPORT_SYMBOL_GPL(ipa_get_gpu_temp);
