#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/soc/qcom/ddr_stats.h>

#include <chipset_common/dubai/dubai_plat.h>

#include "dubai_qcom_plat.h"

static int32_t g_nr_freq = 0;
static int32_t g_nr_ip = 1;

static void dubai_init_stats(void)
{
	g_nr_freq = ddr_stats_get_freq_count();
	dubai_info("dubai_init_stats running");
}

static void dubai_clear_stats(void)
{
	dubai_info("dubai_clear_stats end");
}

static int32_t dubai_get_ddr_freq_cnt(void)
{
	return ddr_stats_get_freq_count();
}

static int32_t dubai_get_ddr_time_in_state(struct dubai_ddr_time_in_state *time_in_state)
{
	int32_t i;
	int ret = 0;
	struct ddr_freq_residency *freq_residency = NULL;

	if (!time_in_state) {
		dubai_err("Invalid parameter");
		return -1;
	}

	freq_residency = kzalloc(g_nr_freq * sizeof(struct ddr_freq_residency), GFP_KERNEL);
	if (!freq_residency) {
		dubai_err("Failed to allocate mem for freq_residency");
		return -1;
	}

	memset(freq_residency, 0, g_nr_freq * sizeof(struct ddr_freq_residency));
	ret = ddr_stats_get_residency(g_nr_freq, freq_residency);
	if (ret <= 0) {
		dubai_err("Failed to call ddr_stats_get_residency %d", ret);
		goto EXIT;
	}

	time_in_state->sr_time = 0;
	time_in_state->pd_time = 0;
	for (i = 0; i < g_nr_freq; i++) {
		time_in_state->freq_time[i].freq = freq_residency[i].freq;
		time_in_state->freq_time[i].time = freq_residency[i].residency;
	}
EXIT:
	kfree(freq_residency);
	freq_residency = NULL;

	return ret;
}

static int32_t dubai_get_ddr_ip_cnt(void)
{
	return g_nr_ip;
}

static int32_t dubai_get_ddr_ip_stats(int32_t ip_cnt, struct dubai_ddr_ip_stats *ip_stats)
{
	int32_t i, j;

	if (!ip_stats) {
		dubai_err("Invalid parameter");
		return -1;
	}

	for (i = 0; i < g_nr_ip; i++) {
		for (j = 0; j < g_nr_freq; j++) {
			ip_stats[i].freq_data[j].freq = 0;
			ip_stats[i].freq_data[j].data = 0;
		}
	}

	return 0;
}

static struct dubai_ddr_stats_ops ddr_ops = {
	.get_freq_cnt = dubai_get_ddr_freq_cnt,
	.get_ip_cnt = dubai_get_ddr_ip_cnt,
	.get_time_in_state = dubai_get_ddr_time_in_state,
	.get_ip_stats = dubai_get_ddr_ip_stats,
};

void dubai_qcom_ddr_stats_init(void)
{
	dubai_init_stats();
	dubai_register_module_ops(DUBAI_MODULE_DDR, &ddr_ops);
}

void dubai_qcom_ddr_stats_exit(void)
{
	dubai_clear_stats();
	dubai_unregister_module_ops(DUBAI_MODULE_DDR);
}
