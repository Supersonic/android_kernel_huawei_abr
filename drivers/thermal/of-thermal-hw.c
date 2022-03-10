/*
 * of-thermal-hw.c
 *
 * hw thermal enhance
 *
 * Copyright (C) 2020-2021 Huawei Technologies Co., Ltd.
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

#ifdef CONFIG_HW_THERMAL_SPM
int of_thermal_get_num_tbps(struct thermal_zone_device *tz)
{
	struct __thermal_zone *data = tz->devdata;

	return data->num_tbps;
}
EXPORT_SYMBOL(of_thermal_get_num_tbps);

int of_thermal_get_actor_weight(struct thermal_zone_device *tz, int i,
				int *actor, unsigned int *weight)
{
	struct __thermal_zone *data = tz->devdata;

	if (i >= data->num_tbps || i < 0)
		return -EDOM;

	*actor = ipa_get_actor_id(data->tbps[i].tcbp->cooling_device->name);
	if (*actor < 0)
		*actor = ipa_get_actor_id("gpu");

	*weight = data->tbps[i].usage;
	pr_err("IPA: matched actor: %d, weight: %d\n", *actor, *weight);

	return 0;
}
EXPORT_SYMBOL(of_thermal_get_actor_weight);
#endif

