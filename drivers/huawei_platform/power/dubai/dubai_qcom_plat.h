#ifndef DUBAI_QCOM_PLAT_H
#define DUBAI_QCOM_PLAT_H

#ifdef CONFIG_HUAWEI_DUBAI_GPU_STATS
void dubai_gpu_freq_stats_init(void);
void dubai_gpu_freq_stats_exit(void);
#else // !CONFIG_HUAWEI_DUBAI_GPU_STATS
static inline void dubai_gpu_freq_stats_init(void) {}
static inline void dubai_gpu_freq_stats_exit(void) {}
#endif // CONFIG_HUAWEI_DUBAI_GPU_STATS

#ifdef CONFIG_HUAWEI_DUBAI_DDR_STATS
void dubai_qcom_ddr_stats_init(void);
void dubai_qcom_ddr_stats_exit(void);
#else // !CONFIG_HUAWEI_DUBAI_DDR_STATS
static inline void dubai_qcom_ddr_stats_init(void) {}
static inline void dubai_qcom_ddr_stats_exit(void) {}
#endif // CONFIG_HUAWEI_DUBAI_DDR_STATS

void dubai_wakeup_stats_init(void);
void dubai_wakeup_stats_exit(void);

#endif // DUBAI_QCOM_PLAT_H
