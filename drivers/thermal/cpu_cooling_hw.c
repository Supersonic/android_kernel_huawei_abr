/*
 * cpu_cooling_hw.c
 *
 * hw cpu cooling enhance
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
#include <trace/events/thermal_power_allocator.h>
#ifdef CONFIG_HW_THERMAL_SPM
extern unsigned int get_powerhal_profile(int actor);
extern unsigned int get_minfreq_profile(int actor);
extern bool is_spm_mode_enabled(void);
#endif
extern unsigned int g_ipa_freq_limit[];
extern unsigned int g_ipa_soc_freq_limit[];
extern unsigned int g_ipa_board_freq_limit[];
extern unsigned int g_ipa_board_state[];
extern unsigned int g_ipa_soc_state[];

#ifdef CONFIG_HW_IPA_THERMAL_DEBUG
static void print_cpu_freq_table(struct cpufreq_cooling_device *cdev)
{
	u32 *f_table, *p_table;
	int i;

	f_table = kcalloc(cdev->max_level + 1, sizeof(u32), GFP_KERNEL);
	p_table = kcalloc(cdev->max_level + 1, sizeof(u32), GFP_KERNEL);

	for (i = 0; i <= cdev->max_level; i++) {
		if (f_table != NULL)
			f_table[i] = cdev->freq_table[i].frequency;
		if (p_table != NULL)
			p_table[i] = cdev->freq_table[i].power;
	}

	if (f_table != NULL && p_table != NULL)
		trace_xpu_freq_table(f_table, p_table, cdev->max_level + 1);

	if (f_table != NULL)
		kfree(f_table);
	if (p_table != NULL)
		kfree(p_table);
}
#endif

static void ipa_calc_static_power(struct cpufreq_cooling_device *cpufreq_cdev,
				  u32 voltage_mv, u32 freq_mhz, u64 freq_power)
{
	u32 static_power;
	int nr_cpus;

	nr_cpus = (int)cpumask_weight(cpufreq_cdev->policy->related_cpus);
	if (nr_cpus == 0)
		nr_cpus = 1;
	if (cpufreq_cdev->plat_get_static_power == NULL) {
		static_power = 0;
		pr_err("%s, %u MHz @ %u mV :  %u + %u = %u mW, nr_cpus = %d\n",
		   __func__, freq_mhz, voltage_mv, freq_power, static_power,
		   freq_power + static_power, nr_cpus);
		return;
	}
	cpufreq_cdev->plat_get_static_power(cpufreq_cdev->policy->related_cpus,
					    0,
					    (unsigned long)(voltage_mv * 1000),
					    &static_power);

	/* hw static_power givern in cluster */
	static_power = static_power / (u32) nr_cpus;

	pr_err("%s, %u MHz @ %u mV :  %u + %u = %u mW, nr_cpus = %d\n",
	       __func__, freq_mhz, voltage_mv, freq_power, static_power,
	       freq_power + static_power, nr_cpus);
}

static bool ipa_check_dev_offline(struct device *dev, u32 * power)
{
	if (dev == NULL) {
		*power = 0;
		return true;
	}

	device_lock(dev);
	if (dev->offline == true) {
		*power = 0;
		device_unlock(dev);
		return true;
	}
	device_unlock(dev);
	return false;
}

static unsigned long ipa_cpufreq_set_cur_state(struct cpufreq_cooling_device
					       *cpufreq_cdev,
					       unsigned long state)
{
	unsigned int cpu = cpumask_any(cpufreq_cdev->policy->related_cpus);
	unsigned int cur_cluster, tmp;
	unsigned long limit_state;
	int clustermask[MAX_THERMAL_CLUSTER_NUM] = {0};

	ipa_get_clustermask(clustermask, MAX_THERMAL_CLUSTER_NUM);
	tmp = (unsigned int)topology_physical_package_id(cpu);
	cur_cluster = clustermask[tmp];

	if (g_ipa_soc_state[cur_cluster] <= cpufreq_cdev->max_level)
		g_ipa_soc_freq_limit[cur_cluster] =
		    cpufreq_cdev->freq_table[g_ipa_soc_state[cur_cluster]].
		    frequency;

	if (g_ipa_board_state[cur_cluster] <= cpufreq_cdev->max_level)
		g_ipa_board_freq_limit[cur_cluster] =
		    cpufreq_cdev->freq_table[g_ipa_board_state[cur_cluster]].
		    frequency;

	limit_state =
	    max(g_ipa_soc_state[cur_cluster], g_ipa_board_state[cur_cluster]);

	/* only change new state when limit_state less than max_level */
	if (!WARN_ON(limit_state > cpufreq_cdev->max_level))
		state = max(state, limit_state);

	trace_set_cur_state(cur_cluster, g_ipa_soc_state[cur_cluster],
			g_ipa_board_state[cur_cluster], g_ipa_soc_freq_limit[cur_cluster],
			g_ipa_board_freq_limit[cur_cluster], limit_state, state);

	return state;
}

static void ipa_set_freq_limit(struct cpufreq_cooling_device *cpufreq_cdev,
			       unsigned int clip_freq)
{
	unsigned int cpu = cpumask_any(cpufreq_cdev->policy->related_cpus);
	unsigned int cur_cluster, tmp;
	int clustermask[MAX_THERMAL_CLUSTER_NUM] = {0};

	ipa_get_clustermask(clustermask, MAX_THERMAL_CLUSTER_NUM);
	tmp = (unsigned int)topology_physical_package_id(cpu);
	cur_cluster = clustermask[tmp];
	g_ipa_freq_limit[cur_cluster] = clip_freq;
	trace_set_freq_limit(cur_cluster, g_ipa_freq_limit[cur_cluster]);
}

/**
 * get_static_power() - calculate the static power consumed by the cpus
 * @cpufreq_cdev:	struct &cpufreq_cooling_device for this cpu cdev
 * @tz:		thermal zone device in which we're operating
 * @freq:	frequency in KHz
 * @power:	pointer in which to store the calculated static power
 *
 * Calculate the static power consumed by the cpus described by
 * @cpu_actor running at frequency @freq.  This function relies on a
 * platform specific function that should have been provided when the
 * actor was registered.  If it wasn't, the static power is assumed to
 * be negligible.  The calculated static power is stored in @power.
 *
 * Return: 0 on success, -E* on failure.
 */
static int get_static_power(struct cpufreq_cooling_device *cpufreq_cdev,
			    struct thermal_zone_device *tz, unsigned long freq,
			    u32 *power)
{
	struct dev_pm_opp *opp = NULL;
	unsigned long voltage;
	struct cpufreq_policy *policy = cpufreq_cdev->policy;
	struct cpumask *cpumask = policy->related_cpus;
	unsigned long freq_hz = freq * 1000;
	struct device *dev = NULL;

	if (!cpufreq_cdev->plat_get_static_power) {
		*power = 0;
		return 0;
	}

	dev = get_cpu_device(policy->cpu);
	WARN_ON(!dev);

	if (ipa_check_dev_offline(dev, power) == true)
		return 0;

	opp = dev_pm_opp_find_freq_exact(dev, freq_hz, true);
	if (IS_ERR(opp)) {
		dev_warn_ratelimited(dev, "Failed to find OPP for frequency %lu: %ld\n",
				     freq_hz, PTR_ERR(opp));
		return -EINVAL;
	}

	voltage = dev_pm_opp_get_voltage(opp);
	dev_pm_opp_put(opp);

	if (voltage == 0) {
		dev_err_ratelimited(dev, "Failed to get voltage for frequency %lu\n",
				    freq_hz);
		return -EINVAL;
	}
	trace_cpu_freq_voltage(policy->cpu, freq_hz, voltage);

	return cpufreq_cdev->plat_get_static_power(cpumask, tz->passive_delay,
						  voltage, power);
}
#endif

#ifdef CONFIG_HW_THERMAL_SPM
int cpufreq_update_policies(void)
{
	struct cpufreq_cooling_device *cpufreq_cdev = NULL;
	struct cpufreq_policy *policy = NULL;
	unsigned int state, freq, min_freq;
	unsigned long clipped_freq;
	int actor;
	int num = 0;
	int clustermask[MAX_THERMAL_CLUSTER_NUM] = {0};

	mutex_lock(&cooling_cpufreq_lock);
	list_for_each_entry(cpufreq_cdev, &cpufreq_cdev_list, node) {
		if (num >= (int)g_cluster_num)
			break;

		policy = cpufreq_cdev->policy;
		state = cpufreq_cdev->cpufreq_state;
		clipped_freq = cpufreq_cdev->freq_table[state].frequency;

		ipa_get_clustermask(clustermask, MAX_THERMAL_CLUSTER_NUM);
		if (is_spm_mode_enabled()) {
			actor = topology_physical_package_id(policy->cpu);
			freq = get_powerhal_profile(clustermask[actor]);
			min_freq = get_minfreq_profile(clustermask[actor]);

			freq_qos_update_request(&cpufreq_cdev->qos_req,
				freq);
			freq_qos_update_request(&cpufreq_cdev->qos_req_min,
				min_freq);
		} else {
			freq_qos_update_request(&cpufreq_cdev->qos_req,
				clipped_freq);
		}
		num++;
	}
	mutex_unlock(&cooling_cpufreq_lock);

	return 0;
}
EXPORT_SYMBOL(cpufreq_update_policies);
#endif

#ifdef CONFIG_HW_IPA_THERMAL
static struct thermal_cooling_device *
__cpufreq_cooling_register(struct device_node *np,
			struct cpufreq_policy *policy, u32 capacitance,
			get_static_t plat_static_func, bool is_ipa);

/**
 * of_cpufreq_power_cooling_register() - create cpufreq cooling device with power extensions
 * @np:	a valid struct device_node to the cooling device device tree node
 * @policy: cpufreq policy
 * @capacitance:	dynamic power coefficient for these cpus
 * @plat_static_func:	function to calculate the static power consumed by these
 *			cpus (optional)
 *
 * This interface function registers the cpufreq cooling device with
 * the name "thermal-cpufreq-%x".  This api can support multiple
 * instances of cpufreq cooling devices.  Using this API, the cpufreq
 * cooling device will be linked to the device tree node provided.
 * Using this function, the cooling device will implement the power
 * extensions by using a simple cpu power model.  The cpus must have
 * registered their OPPs using the OPP library.
 *
 * An optional @plat_static_func may be provided to calculate the
 * static power consumed by these cpus.  If the platform's static
 * power consumption is unknown or negligible, make it NULL.
 *
 * Return: a valid struct thermal_cooling_device pointer on success,
 * on failure, it returns a corresponding ERR_PTR().
 */
struct thermal_cooling_device *
of_cpufreq_power_cooling_register(struct device_node *np,
				  struct cpufreq_policy *policy,
				  u32 capacitance,
				  get_static_t plat_static_func)
{
	if (!np)
		return ERR_PTR(-EINVAL);

	return __cpufreq_cooling_register(np, policy, capacitance,
				plat_static_func, true);
}
EXPORT_SYMBOL(of_cpufreq_power_cooling_register);
#endif
