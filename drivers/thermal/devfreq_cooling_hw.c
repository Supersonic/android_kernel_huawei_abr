/*
 * devfreq_cooling_hw.c
 *
 * hw devfreq cooling enhance
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
#define PROP_NAME_LEN 20
#include <trace/events/thermal_power_allocator.h>
#include <securec.h>
extern unsigned int g_ipa_freq_limit[];
extern unsigned int g_ipa_soc_freq_limit[];
extern unsigned int g_ipa_board_freq_limit[];
extern unsigned int g_ipa_board_state[];
extern unsigned int g_ipa_soc_state[];
extern int update_devfreq(struct devfreq *devfreq);
extern int ipa_set_thermal_pwrlevel(struct kgsl_device *device, u32 new_level);

static inline unsigned long get_total_power(struct devfreq_cooling_device *dfc,
					unsigned long freq,
					unsigned long voltage);
static unsigned long get_static_power(struct devfreq_cooling_device *dfc,
					unsigned long freq);
static unsigned long get_dynamic_power(struct devfreq_cooling_device *dfc,
					unsigned long freq,
					unsigned long voltage);

static unsigned long get_voltage(struct devfreq_cooling_device *dfc, unsigned long freq)
{
	int index;
	unsigned long voltage;

	for (index = 1; index < dfc->freq_table_size; index++) {
		if (freq > dfc->freq_table[index])
			break;
	}
	voltage = dfc->voltage_table[index - 1];

	return voltage / 1000; /* mV */
}

static void ipa_power_debug_print(struct devfreq_cooling_device *dfc,
				  unsigned long freq, unsigned long voltage)
{
	unsigned long power_static;
	unsigned long power_dyn;

	power_static = get_static_power(dfc, freq);
	power_dyn = get_dynamic_power(dfc, freq, voltage / 1000);
	pr_err("%s: %lu MHz @ %lu mV: %lu + %lu = %lu mW\n",
		 __func__, freq / 1000000, voltage / 1000,
		 power_dyn, power_static, power_dyn + power_static);
}

static int of_parse_gpu_opp(int idx, struct device_node *np, unsigned long freq) {
	int ret;
	char prop_name[PROP_NAME_LEN];
	int freq_volt[2];
	unsigned long tmp = 0;

	ret = snprintf_s(prop_name, sizeof(prop_name), (sizeof(prop_name) - 1),
			"huawei,gpu_opp_%d", idx);
	if (ret < 0) {
		pr_err("%s, snprintf_s error.\n", __func__);
		return tmp;
	}

	ret = of_property_read_u32_array(np, prop_name, freq_volt, 2);
	if (!ret) {
		if (freq_volt[0] == freq)
			tmp = freq_volt[1];
		else
			pr_err("%s, freq is not equal\n", __func__);
	} else {
		pr_err("%s, get gpu_opp_%d err\n", __func__, idx);
	}

	return tmp;
}

/**
 * devfreq_cooling_gen_tables() - Generate power and freq tables.
 * @dfc: Pointer to devfreq cooling device.
 *
 * Generate power and frequency tables: the power table hold the
 * device's maximum power usage at each cooling state (OPP).  The
 * static and dynamic power using the appropriate voltage and
 * frequency for the state, is acquired from the struct
 * devfreq_cooling_power, and summed to make the maximum power draw.
 *
 * The frequency table holds the frequencies in descending order.
 * That way its indexed by cooling device state.
 *
 * The tables are malloced, and pointers put in dfc.  They must be
 * freed when unregistering the devfreq cooling device.
 *
 * Return: 0 on success, negative error code on failure.
 */
static int devfreq_cooling_gen_tables(struct devfreq_cooling_device *dfc)
{
	struct device_node *np = NULL;
	struct devfreq *df = dfc->devfreq;
	struct device *dev = df->dev.parent;
	int ret, num_opps;
	unsigned long freq;
	u32 *power_table = NULL;
	u32 *freq_table = NULL;
	u32 *voltage_table = NULL;
	int i;
	bool found = true;
	bool use_parse = false;
	int parse_num = 0;
	unsigned long power, voltage;

	np = of_find_node_by_name(NULL, "gpu_opp");
	if (np != NULL) {
		ret = of_property_read_u32(np, "huawei,gpu_opp_num", &parse_num);
		if (ret)
			pr_err("%s, parse gpu_opp_num err\n", __func__);
	} else {
		pr_err("%s, gpu_opp node not found\n", __func__);
		found = false;
	}

	num_opps = dev_pm_opp_get_opp_count(dev);
	if (found && (parse_num == num_opps))
		use_parse = true;
	if (dfc->power_ops) {
		power_table = kcalloc(num_opps, sizeof(*power_table),
				      GFP_KERNEL);
		if (!power_table) {
			if (np)
				of_node_put(np);
			return -ENOMEM;
		}
	}

	freq_table = kcalloc(num_opps, sizeof(*freq_table),
			     GFP_KERNEL);
	if (!freq_table) {
		ret = -ENOMEM;
		goto free_power_table;
	}

	voltage_table = kcalloc(num_opps, sizeof(*voltage_table),
				GFP_KERNEL);
	if (!voltage_table) {
		ret = -ENOMEM;
		goto free_freq_tables;
	}

	for (i = 0, freq = ULONG_MAX; i < num_opps; i++, freq--) {
		struct dev_pm_opp *opp;

		opp = dev_pm_opp_find_freq_floor(dev, &freq);
		if (IS_ERR(opp)) {
			ret = PTR_ERR(opp);
			goto free_tables;
		}

		if (use_parse)
			voltage = of_parse_gpu_opp(i, np, freq);
		else
			voltage = dev_pm_opp_get_voltage(opp); /* mV */
		dev_pm_opp_put(opp);

		voltage_table[i] = voltage;
		voltage /= 1000; /* mV */

		if (dfc->power_ops) {
			if (dfc->power_ops->get_real_power)
				power = get_total_power(dfc, freq, voltage);
			else
				power = get_dynamic_power(dfc, freq, voltage);

			power_table[i] = power;
		}

		freq_table[i] = freq;
	}

	if (dfc->power_ops)
		dfc->power_table = power_table;

	dfc->freq_table = freq_table;
	dfc->freq_table_size = num_opps;
	dfc->voltage_table = voltage_table;

	for (i = 0; i < dfc->freq_table_size; i++) {
		freq = dfc->freq_table[i];
		voltage = dfc->voltage_table[i];
		ipa_power_debug_print(dfc, freq, voltage);
	}

	if (np)
		of_node_put(np);

	return 0;

free_tables:
	kfree(voltage_table);
free_freq_tables:
	kfree(freq_table);
free_power_table:
	kfree(power_table);
	if (np)
		of_node_put(np);
	return ret;
}

static void ipa_set_current_load_freq(struct thermal_cooling_device *cdev,
				      struct thermal_zone_device *tz,
				      u32 dyn_power, u32 static_power,
				      u32 * power)
{
	struct devfreq_cooling_device *dfc = cdev->devdata;
	struct devfreq *df = dfc->devfreq;
	struct devfreq_dev_status *status = &df->last_status;
	unsigned long load = 0;
	unsigned long freq = status->current_frequency;

	if (status->total_time)
		load = 100 * status->busy_time / status->total_time;
	if (tz->is_soc_thermal)
		trace_IPA_actor_gpu_get_power((freq / 1000), load, dyn_power,
					      static_power, *power);

	cdev->current_load = load;
	cdev->current_freq = freq;
}

static void print_gpu_freq_table(struct devfreq_cooling_device *dfc)
{
	trace_xpu_freq_table(dfc->freq_table, dfc->power_table, dfc->freq_table_size);
}

#endif
