/*
 * hw_ipa_gpu.c
 *
 * hw ipa for gpufreq driver
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
#ifdef CONFIG_HW_IPA_THERMAL
#include <linux/devfreq.h>
#define CAPACITANCES_NUM 5
#define GPU_TZ_NUMBER 2
#define MIN_TEMP -40000
#define MAX_TEMP 145000
#define MAX_CAP 0xFFFFF
#define DECIMAL 10

static u32 dynamic_capacitance = 1;
const char *temp_capacitances[CAPACITANCES_NUM] = {0};
const char *gpu_tz_name[GPU_TZ_NUMBER];

static void ipa_gpu_init (void)
{
	struct device_node *dev_node = NULL;
	int ret;
	int i;
	u32 gpu_tz_num;

	dev_node = of_find_node_by_name(NULL, "capacitances");
	if (dev_node == NULL) {
		pr_err("%s capacitances node not found\n", __func__);
		return;
	}

	ret = of_property_read_u32(dev_node, "huawei,gpu_dyn_capacitance",
				&dynamic_capacitance);
	if (ret != 0) {
		pr_err("%s gpu_dyn_capacitance read err\n", __func__);
		goto node_put;
	}

	for (i = 0; i < CAPACITANCES_NUM; i++) {
		ret = of_property_read_string_index(dev_node, "huawei,gpu_temp_scale_capacitance",
						i, &temp_capacitances[i]);
		if (ret != 0) {
			pr_err("%s gpu_temp_scale_capacitance %d read err\n", __func__, i);
				goto node_put;
		}
	}
	of_node_put(dev_node);

	dev_node = of_find_node_by_name(NULL, "tz_info");
	if (dev_node == NULL) {
		pr_err("%s tz_info node not found\n", __func__);
		return;
	}

	ret = of_property_read_u32(dev_node, "huawei,gpu_tz_num", &gpu_tz_num);
	if (ret != 0) {
		pr_err("%s gpu_tz_num read err\n", __func__);
		goto node_put;
	}
	for (i = 0; i < gpu_tz_num; i++) {
		ret = of_property_read_string_index(dev_node, "huawei,gpu_tz_name",
					i, &gpu_tz_name[i]);
		if (ret != 0) {
			pr_err("%s gpu_tz_name%d read err\n", __func__, i);
			goto node_put;
		}
	}

	pr_info("%s ipa_gpu_init succ\n", __func__);

node_put:
	of_node_put(dev_node);
}

static unsigned long
hw_model_static_power(struct devfreq *devfreq __maybe_unused,
	unsigned long voltage)
{
	int capacitance[CAPACITANCES_NUM] = {0};
	int temperature;
	int ret, i, cap;
	int t_exp = 1;
	long long t_scale = 0;
	unsigned long v_scale;
	unsigned long m_volt = voltage;

	v_scale = m_volt * m_volt * m_volt / 1000000;

	temperature = ipa_get_gpu_temp(gpu_tz_name);
	temperature /= 1000;

	temperature = clamp_val(temperature, MIN_TEMP, MAX_TEMP);
	for (i = 0; i < CAPACITANCES_NUM - 1; i++) {
		ret = kstrtoint(temp_capacitances[i], DECIMAL, &cap);
		if (ret != 0) {
			pr_warning("%s kstortoint is failed\n", __func__);
			return 0;
		}
		capacitance[i] = clamp_val(cap, -MAX_CAP, MAX_CAP);
		t_scale += (long long)capacitance[i] * t_exp;
		t_exp *= temperature;
	}

	t_scale = clamp_val(t_scale, 0, UINT_MAX);
	ret = kstrtoint(temp_capacitances[CAPACITANCES_NUM - 1],
			DECIMAL, &capacitance[CAPACITANCES_NUM - 1]);
	if (ret != 0) {
		pr_warning("%s kstortoint is failed\n", __func__);
		return 0;
	}

	trace_cpu_static_power(1, temperature, t_scale, v_scale, (capacitance[CAPACITANCES_NUM - 1] * v_scale * t_scale) / 1000000);

	return (unsigned long)(capacitance[CAPACITANCES_NUM - 1] * v_scale * t_scale) / 1000000;

}

static unsigned long
hw_model_dynamic_power(struct devfreq *devfreq __maybe_unused,
	unsigned long freq, unsigned long voltage)
{
	/* The inputs: freq (f) is in Hz, and voltage (v) in mV.
	 * The coefficient (c) is in mW/(MHz mV mV).
	 *
	 * This function calculates the dynamic power after this formula:
	 * Pdyn (mW) = c (mW/(MHz*mV*mV)) * v (mV) * v (mV) * f (MHz)
	 * unsigned long v2 = (voltage * voltage) / 1000;
	 */
	unsigned long v2 = voltage * voltage / 1000;
	unsigned long f_mhz = freq / 1000000;

	return (dynamic_capacitance * v2 * f_mhz) / 1000000;
}

static struct devfreq_cooling_power hw_model_ops = {
	.get_static_power = hw_model_static_power,
	.get_dynamic_power = hw_model_dynamic_power,
	.get_real_power = NULL,
};

int ipa_set_thermal_pwrlevel(struct kgsl_device *device, u32 level)
{
	int min_level, max_level;
	struct kgsl_pwrctrl *pwr = NULL;
	struct device *dev = NULL;
	struct dev_pm_opp *opp = NULL;
	unsigned long min_freq, max_freq;
	int index;

	if (!device)
		return -ENODEV;

	pwr = &device->pwrctrl;
	if (!pwr)
		return -ENODEV;

	min_freq = 0;
	max_freq = pwr->pwrlevels[0].gpu_freq;
	dev = &device->pdev->dev;

	opp = dev_pm_opp_find_freq_ceil(dev, &min_freq);
	if (IS_ERR(opp))
		min_freq = pwr->pwrlevels[pwr->min_pwrlevel].gpu_freq;
	else
		dev_pm_opp_put(opp);

	opp = dev_pm_opp_find_freq_floor(dev, &max_freq);
	if (IS_ERR(opp))
		return PTR_ERR(opp);
	dev_pm_opp_put(opp);

	mutex_lock(&device->mutex);

	if (level >= pwr->num_pwrlevels)
		level = pwr->num_pwrlevels - 1;

	min_level = pwr->thermal_pwrlevel_floor;
	max_level = pwr->thermal_pwrlevel;
	for (index = 0; index < pwr->num_pwrlevels; index++) {
		if (min_freq == pwr->pwrlevels[index].gpu_freq)
			min_level = index;
		if (max_freq == pwr->pwrlevels[index].gpu_freq)
			max_level = index;
	}

	pwr->thermal_pwrlevel = level < max_level ? max_level : level;
	pwr->thermal_pwrlevel_floor = min_level;
	kgsl_pwrctrl_pwrlevel_change(device, pwr->active_pwrlevel);

	trace_set_thermal_pwrlevel(min_freq, max_freq, min_level, max_level,
		pwr->thermal_pwrlevel, pwr->thermal_pwrlevel_floor);

	mutex_unlock(&device->mutex);

	return 0;
}
EXPORT_SYMBOL(ipa_set_thermal_pwrlevel);
#endif
