

#ifndef __SDHCI_MUX_SDSIM_H
#define __SDHCI_MUX_SDSIM_H

enum {
	SD_SIM_UNDETECTED = 0,
	SD_SIM_SD_NORMAL,
	SD_SIM_SIM_NORMAL,
	SD_SIM_NO_CARD,
	SD_SIM_SIM_FAIL,
	SD_SIM_SIM_PASS,
};

struct sdhci_msm_reg_data;

int sim_gpio_setup(struct mmc_host *host);
int sim_nmc_cmd_handler(int cmd, int param);
void sdsim_detect(struct mmc_host *host);
int sdhci_msm_vreg_set_optimum_mode(struct sdhci_msm_reg_data *vreg, int uA_load);
int sdhci_msm_vreg_set_voltage(struct sdhci_msm_reg_data *vreg, int min_uV, int max_uV);
int sdhci_msm_vreg_enable(struct sdhci_msm_reg_data *vreg);
int sdhci_msm_vreg_disable(struct sdhci_msm_reg_data *vreg);

#endif /* __SDHCI_MUX_SDSIM_H */


