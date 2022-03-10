#include <linux/slab.h>

#include <chipset_common/dubai/dubai_plat.h>
#include <huawei_platform/gpu_stats/gpu_stats.h>

static atomic_t stats_enable = ATOMIC_INIT(0);
static int freq_num = -1;
static struct hw_gpu_opp_stats *stat_data = NULL;

static int dubai_set_gpu_enable(bool enable)
{
	atomic_set(&stats_enable, enable ? 1 : 0);
	dubai_info("Gpu stats enable: %d", enable ? 1 : 0);

	return 0;
}

static int dubai_get_gpu_info(struct dubai_gpu_freq_info *data, int num)
{
	int ret, i;

	if (!atomic_read(&stats_enable))
		return -EPERM;

	if (!data || (num != freq_num)) {
		dubai_err("Invalid param: %d, %d", num, freq_num);
		return -EINVAL;
	}
	if (!stat_data) {
		stat_data = kzalloc(freq_num * sizeof(struct hw_gpu_opp_stats), GFP_KERNEL);
		if (!stat_data) {
			dubai_err("Failed to alloc memory");
			return -ENOMEM;
		}
	}
	ret = hw_gpu_query_opp_cost(stat_data, freq_num);
	if (!ret) {
		for (i = 0; i < freq_num; i++) {
			data[i].freq = stat_data[i].freq;
			data[i].run_time = stat_data[i].active;
			data[i].idle_time = stat_data[i].idle;
		}
	} else {
		dubai_err("Failed to get gpu stats");
	}

	return ret;
}

static int dubai_get_gpu_freq_num(void)
{
	int num = (int)hw_gpu_get_opp_cnt();
	if (freq_num < 0)
		freq_num = num;

	return num;
}

static struct dubai_gpu_stats_ops gpu_ops = {
	.enable = dubai_set_gpu_enable,
	.get_stats = dubai_get_gpu_info,
	.get_num = dubai_get_gpu_freq_num,
};

void dubai_gpu_freq_stats_init(void)
{
	dubai_register_module_ops(DUBAI_MODULE_GPU, &gpu_ops);
}

void dubai_gpu_freq_stats_exit(void)
{
	dubai_unregister_module_ops(DUBAI_MODULE_GPU);
	atomic_set(&stats_enable, 0);
	freq_num = -1;
	if (stat_data) {
		kfree(stat_data);
		stat_data = NULL;
	}
}
