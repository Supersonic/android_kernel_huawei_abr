 /*
 * hw_thermal.h
 *
 * head file for thermal
 *
 * Copyright (c) 2017-2021 Huawei Technologies Co., Ltd.
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
#ifndef __HW_THERMAL_H__
#define __HW_THERMAL_H__

#ifdef CONFIG_HW_PERF_CTRL
#include <linux/hw/thermal_perf_ctrl.h>
#endif

#define MAX_THERMAL_CLUSTER_NUM 3
typedef u32 (*get_power_t)(void);
u32 get_camera_power(void);
u32 get_backlight_power(void);
u32 get_charger_power(void);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvisibility"
void board_cooling_unregister(struct thermal_cooling_device *cdev);
#pragma GCC diagnostic pop
struct thermal_cooling_device *board_power_cooling_register(struct device_node *np,
							    get_power_t get_power);
int ipa_get_tsensor_id(const char *name);
int ipa_get_sensor_value(u32 sensor, int *val);
int ipa_get_periph_id(const char *name);
int ipa_get_periph_value(u32 sensor, int *val);
int hw_battery_temperature(void);
#ifdef CONFIG_HW_PERF_CTRL
int get_ipa_status(struct ipa_stat *status);
#endif
struct cpufreq_frequency_table *cpufreq_frequency_get_table(unsigned int cpu);
void ipa_get_clustermask(int *clustermask, int len);
#ifdef CONFIG_HW_THERMAL_SHELL
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvisibility"
int hw_get_shell_temp(struct thermal_zone_device *thermal, int *temp);
#pragma GCC diagnostic pop
#endif
#ifdef CONFIG_HW_THERMAL_GPU_HOTPLUG
extern void hw_gpu_thermal_cores_control(u64 cores);
#endif
extern int devdrv_npu_ctrl_core(u32 dev_id, u32 core_num);
extern int coul_get_battery_cc(void);
extern int cpufreq_update_policies(void);
#ifdef CONFIG_HW_EXTERNAL_SENSOR
unsigned int ipa_get_static_power(int cid);
#endif
#ifdef CONFIG_HW_IPA_THERMAL
int nametoactor(const char *weight_attr_name);
void update_pid_value(struct thermal_zone_device *);
static inline bool is_ipa_thermal(struct thermal_zone_device *tz)
{
	if (tz->is_board_thermal || tz->is_soc_thermal)
		return true;
	return false;
}
#endif
#endif
