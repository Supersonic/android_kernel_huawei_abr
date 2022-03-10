

#include <linux/delay.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/host.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>
#include <linux/mmc/slot-gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/spmi.h>
#include <linux/mmc/sdhci_mux_sdsim.h>
#include "hwnet/hwril_sim_sd/hwril_sim_sd.h"
#include "../core/core.h"
#include "../core/pwrseq.h"
#include "../core/mmc_ops.h"
#include <sdhci-pltfm.h>


#define OCR_REGISTER_BIT_0_TO_27_MASK 0xFFFFFFF
#define OCR_REGISTER_SD 0xFF8080

#define REGULATOR_PM8350C 0x2
#define REGULATOR_VREG_ADDR_L5C 0xC5A0
#define REGULATOR_VREG_ADDR_L9C 0xC9A0

#define SLEEP_MS_TIME_FOR_DETECT_UNSTABLE   20
#define VDD_PMU_POWER_ON 0x1
#define VDD_PMU_POWER_OFF 0x0

static unsigned g_freqs[] = { 400000, 300000, 200000, 100000 };
u32 g_sd_sim_detect_status_current = SD_SIM_UNDETECTED;
u32 g_sd_sim_detect_status_old = SD_SIM_UNDETECTED;


enum {
	REGULATOR_VREG_TYPE_READ = 0,
	REGULATOR_VREG_TYPE_WRITE,
};

enum {
	VDD_SD_PMU_INDEX = 0,
	VDD_SD_IO_PMU_INDEX,
	VDD_SIM_PMU_INDEX,
};
#define SD_SIM_SWITCH_SD 0x0
#define SD_SIM_SWITCH_SIM 0x1

enum vdd_io_level {
	VOL_LOW_LEVEL = 0,
	VOL_HIGH_LEVEL,
	VOL_SET_LEVEL,
};

/* This structure keeps information per regulator */
struct sdhci_msm_reg_data {
	/* voltage regulator handle */
	struct regulator *reg;
	/* regulator name */
	const char *name;
	/* voltage level to be set */
	u32 low_vol_level;
	u32 high_vol_level;
	/* Load values for low power and high power mode */
	u32 lpm_uA;
	u32 hpm_uA;

	/* is this regulator enabled? */
	bool is_enabled;
	/* is this regulator needs to be always on? */
	bool is_always_on;
	/* is low power mode setting required for this regulator? */
	bool lpm_sup;
	bool set_voltage_sup;
};

/*
 * This structure keeps information for all the
 * regulators required for a SDCC slot.
 */
struct sdhci_msm_vreg_data {
	/* keeps VDD/VCC regulator info */
	struct sdhci_msm_reg_data *vdd_data;
	 /* keeps VDD IO regulator info */
	struct sdhci_msm_reg_data *vdd_io_data;
#ifdef CONFIG_SDSIM_MUX
	 /* keeps VDD SIM regulator info */
	struct sdhci_msm_reg_data *vdd_sim_data;
#endif
};

struct sdhci_msm_host {
	struct platform_device *pdev;
	void __iomem *core_mem; /* MSM SDCC mapped address */
	int pwr_irq; /* power irq */
	struct clk *bus_clk; /* SDHC bus voter clock */
	struct clk *xo_clk; /* TCXO clk needed for FLL feature of cm_dll */
	/* core, iface, ice, cal, sleep clocks */
	struct clk_bulk_data bulk_clks[5];
	unsigned long clk_rate;
	struct sdhci_msm_vreg_data *vreg_data;
};

extern struct mmc_host *mmc_host_det;
static DEFINE_MUTEX(card_insert_muxlock);

int sd_sim_set_pin(struct mmc_host *host, char *pinctrl_name)
{
	int ret = 0;
	struct pinctrl *pctrl = NULL;
	struct pinctrl_state *pinctrl_state_sd_sim = NULL;

	pctrl = devm_pinctrl_get(host->parent);
	if (pctrl == NULL) {
		ret = -EINVAL;
		pr_err("sdsim pinctrl get fail\n");
		return ret;
	}
	/* "sim_off" or "sd_off" */
	pinctrl_state_sd_sim = pinctrl_lookup_state(pctrl, pinctrl_name);
	if (pinctrl_state_sd_sim == NULL) {
		ret = -EINVAL;
		goto err_pinctrl_put;
	}

	ret = pinctrl_select_state(pctrl, pinctrl_state_sd_sim);
	if (ret) {
		ret = -EINVAL;
		goto err_pinctrl_put;
	}
	return 0;
err_pinctrl_put:
	pr_err("sdsim pin set fail\n");
	devm_pinctrl_put(pctrl);
	return ret;
}

int gpio_pin_input_set(int gpio_num)
{
	int ret;

	ret = gpio_request(gpio_num, "gpio_num");
	if (ret) {
		pr_err("sdsim can't request gpio number %d\n", gpio_num);
		return -ENOSYS;
	}
	pr_err("sdsim %s sim gpio num: %d\n", __func__, gpio_num);

	gpio_direction_input(gpio_num);
	gpio_free(gpio_num);
	return 0;
}

int sim_gpio_input_setup(struct mmc_host *host)
{
	int ret;
	ret = gpio_pin_input_set(host->sim_dat_gpio);
	msleep(5); /* delay 5ms */
	ret |= gpio_pin_input_set(host->sim_clk_gpio);
	msleep(5); /* delay 5ms */
	ret |= gpio_pin_input_set(host->sim_rst_gpio);
	msleep(5); /* delay 5ms */
	return ret;
}

int sim_gpio_setup(struct mmc_host *host)
{
	int ret;
	/* sim gpio config hi-z */
	ret = sd_sim_set_pin(host, "sim_off");
	if (ret) {
		pr_err("sdsim set sim-off pin fail,ret:%d\n", ret);
		return ret;
	}
	msleep(10); /* delay 10ms */
	ret = sim_gpio_input_setup(host);
	if (ret) {
		pr_err("sdsim set sim-input direction set fail,ret:%d\n", ret);
		return ret;
	}
	msleep(10); /* delay 10ms */
	return ret;
}

int regulator_fast_mos_vreg(u8 type_vreg, u16 addr, u8 value)
{
	u8 buf_vreg[2] = {0};

	if (type_vreg == REGULATOR_VREG_TYPE_WRITE) {
		buf_vreg[0] = value;
		huawei_pmic_reg_write(REGULATOR_PM8350C, addr, buf_vreg, 1);
		return 0;
	} else if (type_vreg == REGULATOR_VREG_TYPE_READ) {
		huawei_pmic_reg_read(REGULATOR_PM8350C, addr, buf_vreg, 1);
		return buf_vreg[0];
	}
	return 0;
}

int sdhci_sd_sim_singal_switch(struct mmc_host *mmc, int switch_value)
{
	int ret;
	int gpio_num = mmc->sd_sim_gpio_switch;

	ret = gpio_request(gpio_num, "gpio_num");
	if (ret) {
		pr_err("sdsim can't request gpio number %d\n", gpio_num);
		return -ENOSYS;
	}
	pr_err("sdsim %s:%s mmc gpio num: %d, switch_value: %d\n",
		mmc_hostname(mmc), __func__, gpio_num, switch_value);

	gpio_direction_output(gpio_num, switch_value);
	gpio_set_value(gpio_num, switch_value);
	gpio_free(gpio_num);
	return 0;
}

int sdhci_msm_vreg_disable_sdsim(struct sdhci_msm_reg_data *vreg, int is_power_off)
{
	int ret = 0;

	/* Never disable regulator marked as always_on */
	if (is_power_off) {
		ret = regulator_disable(vreg->reg);
		if (ret) {
			pr_err("sdsim %s: regulator_disable(%s) failed. ret=%d\n",
				__func__, vreg->name, ret);
			goto out;
		}
		vreg->is_enabled = false;

		ret = sdhci_msm_vreg_set_optimum_mode(vreg, 0);
		if (ret < 0)
			goto out;

		/* Set min. voltage level to 0 */
		ret = sdhci_msm_vreg_set_voltage(vreg, 0, vreg->high_vol_level);
		if (ret)
			goto out;
	} else {
		if (vreg->lpm_sup) {
			/* Put always_on regulator in LPM (low power mode) */
			ret = sdhci_msm_vreg_set_optimum_mode(vreg, vreg->lpm_uA);
			if (ret < 0)
				goto out;
		}
	}
out:
	return ret;
}

int sdhci_msm_set_vol_sdsim(struct sdhci_msm_reg_data *vreg,
	enum vdd_io_level level, unsigned int voltage_level)
{
	int ret = 0;
	int set_level;

	if (vreg && vreg->is_enabled) {
		switch (level) {
		case VOL_LOW_LEVEL:
			set_level = vreg->low_vol_level;
			break;
		case VOL_HIGH_LEVEL:
			set_level = vreg->high_vol_level;
			break;
		case VOL_SET_LEVEL:
			set_level = voltage_level;
			break;
		default:
			pr_err("sdsim %s: invalid argument level = %d\n", __func__, level);
			ret = -EINVAL;
			return ret;
		}
		pr_err("sdsim set level %d\n", set_level);
		ret = sdhci_msm_vreg_set_voltage(vreg, set_level, set_level);
	}
	return ret;
}

int sdhci_msm_setup_vreg_sdsim(struct mmc_host *host, int pmc_type, int vol_level, bool enable)
{
	int ret = 0;
	int i;
	struct sdhci_host *sdhci_host = mmc_priv(host);
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(sdhci_host);
	struct sdhci_msm_host *msm_host = sdhci_pltfm_priv(pltfm_host);

	struct sdhci_msm_vreg_data *curr_slot;
	struct sdhci_msm_reg_data *vreg_table[3];

	curr_slot = msm_host->vreg_data;
	if (!curr_slot) {
		pr_debug("%s: vreg info unavailable,assuming the slot is powered by always on domain\n",
			 __func__);
		goto out;
	}

	vreg_table[0] = curr_slot->vdd_data; /* L9c */
	vreg_table[1] = curr_slot->vdd_io_data; /* L6c */
	vreg_table[2] = curr_slot->vdd_sim_data; /* L5c */

	for (i = 0; i < ARRAY_SIZE(vreg_table); i++) {
		if (vreg_table[i] && (i == pmc_type)) {
			pr_err("sdsim enable pmc_type %d,vol level:%d\n",pmc_type, vol_level);
			if (enable) {
				ret = sdhci_msm_vreg_enable(vreg_table[i]);
				if (!ret)
					ret = sdhci_msm_set_vol_sdsim(vreg_table[i], vol_level, 0);
			} else {
				if ((vol_level == VOL_SET_LEVEL) && vreg_table[i]->is_enabled) {
					pr_err("sdsim vreg ori enable %d,is_always_on %d\n",
						vreg_table[i]->is_enabled, vreg_table[i]->is_always_on);
					ret = sdhci_msm_vreg_disable_sdsim(vreg_table[i], 0x1);
				} else {
 					ret = sdhci_msm_vreg_disable(vreg_table[i]);
				}
			}
			if (ret)
				goto out;
		}
	}

out:
	return ret;
}

void sdhci_msm_adjust_vddio_level(struct mmc_host *host, int vol_level)
{
	struct sdhci_host *sdhci_host = mmc_priv(host);
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(sdhci_host);
	struct sdhci_msm_host *msm_host = sdhci_pltfm_priv(pltfm_host);
	struct sdhci_msm_vreg_data *curr_slot;

	curr_slot = msm_host->vreg_data;
	if (!curr_slot) {
		pr_err("sdsim %s: vreg_data info unavailable\n", __func__);
		return;
	}
	if (vol_level == VOL_HIGH_LEVEL)
		curr_slot->vdd_io_data->high_vol_level = 2960000; /* L6c high level 2960000uv */
	else
		curr_slot->vdd_io_data->high_vol_level = 1808000; /* L6c high level 1808000uv */
	msleep(5); /* delay 5ms */
	pr_err("sdsim vddio vol %d:%duv\n", vol_level, curr_slot->vdd_io_data->high_vol_level);
}

bool sdhci_msm_vdd_is_enabled(struct mmc_host *mmc_host)
{
	int ret = 0;
	struct sdhci_host *sdhci_host = mmc_priv(mmc_host);
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(sdhci_host);
	struct sdhci_msm_host *msm_host = sdhci_pltfm_priv(pltfm_host);
	struct sdhci_msm_vreg_data *curr_slot;

	curr_slot = msm_host->vreg_data;
	if (!curr_slot) {
		pr_err("sdsim %s: vreg_data info unavailable\n", __func__);
		return true;
	}
	ret = regulator_is_enabled(curr_slot->vdd_data->reg);
	if (ret)
		return true;
	else
		return false;
}

int sim_nmc_cmd_handler(int cmd, int param)
{
	int ret;

	pr_err("sdsim sim_nmc_cmd_handler cmd:%d, param:%d\n", cmd, param);
	switch (cmd) {
	case 0:
		pr_err("sdsim sim get result %d\n", g_sd_sim_detect_status_current);
		hwril_sim_sd_nmc_detect_result_notify(g_sd_sim_detect_status_current);
		break;
	case 1:
		if (param == 0) {
			/* sim detect fail */
			g_sd_sim_detect_status_current = SD_SIM_SIM_FAIL;
			pr_err("sdsim sim detect fail,cmd handle status %d\n", g_sd_sim_detect_status_current);
			/* L6c power off */
			ret = sdhci_msm_setup_vreg_sdsim(mmc_host_det, VDD_SD_IO_PMU_INDEX, VOL_SET_LEVEL, VDD_PMU_POWER_OFF);
			if (ret) {
				pr_err("vdd-io(L6c) sim power off fail,ret:%d\n", ret);
				break;
			}
		} else if (param == 1) {
			/* sim detect ok */
			g_sd_sim_detect_status_current = SD_SIM_SIM_PASS;
			pr_err("sdsim sim detect ok,cmd handle status %d\n", g_sd_sim_detect_status_current);
		} else {
			pr_err("sdsim detect exception\n");
		}
		break;
	default:
		break;
	}
	return 0;
}

int mmc_detect_mmc(struct mmc_host *host)
{
	int err;
	u32 ocr;
	int ret;

	BUG_ON(!host);
	WARN_ON(!host->claimed);

	/* Set correct bus mode for MMC before attempting attach */
	if (!mmc_host_is_spi(host))
		mmc_set_bus_mode(host, MMC_BUSMODE_OPENDRAIN);

	/* if we just want to probe card type for shared tray,
	 * just send to a cmd1 once.
	 * 0x5A(magic num) indicate that mmc detect probe base vdd 1.8v,
	 * because MMC_CARD_BUSY bit is 0 with vdd 1.8v,will consume 1s to
	 * detect card type,it is so long for shared tray.
	 */
	err = mmc_send_op_cond(host, 0x5A, &ocr);

	pr_err("sdsim %s:%d err=%d ocr=0x%08X bit[0:27]:0x%08X\n",
		__func__, __LINE__, err, ocr,
		ocr & OCR_REGISTER_BIT_0_TO_27_MASK);

	if ((ocr & OCR_REGISTER_BIT_0_TO_27_MASK) != OCR_REGISTER_SD) {
		pr_err("sdsim %s:%d OCR register bit[0:27] is not 0x0FF8080!!\n",
			 __func__, __LINE__);
		ret = -1;
	} else {
		/* ocr = 0xFF8080 return sd detected */
		ret = 0;
	}

	return ret;
}

static int mmc_rescan_detect_sd_mmc(struct mmc_host *host, unsigned int freq)
{
	int sd_mmc_detected = -1;
	int ret;

	host->f_init = freq;

	mmc_power_up(host, host->ocr_avail);

	msleep(5); /* delay 5ms */
	/* sd io config to active */
	ret = sd_sim_set_pin(host, "default");
	if (ret)
		pr_err("set sd pin active failed,ret:%d\n", ret);
	msleep(5); /* delay 5ms */

	/*
	 * Some eMMCs (with VCCQ always on) may not be reset after power up, so
	 * do a hardware reset if possible.
	 */
	mmc_hw_reset_for_init(host);

	mmc_go_idle(host);
	pr_err("sdsim mmc_detect_mmc start...\n");
	if (!(host->caps2 & MMC_CAP2_NO_MMC)) {
		if (!mmc_detect_mmc(host))
			sd_mmc_detected = 0;
	}

	mmc_power_off(host);
	return sd_mmc_detected;
}

static int mmc_detect_sd_mmc(struct mmc_host *host)
{
	int i = 0;
	int sd_mmc_detected = -1;

	if (!mmc_rescan_detect_sd_mmc(host, max(g_freqs[i], host->f_min)))
		sd_mmc_detected = 0;

	return sd_mmc_detected;
}

int sdsim_send_cmd_detect(struct mmc_host *host, int cd_state)
{
	int ret;
	int detect_result;
	struct mmc_host *mmc_detect = host;

	/* if cd-gpios level is low, return cmd fail */
	if (cd_state) {
		pr_err("sdsim detect SD_SIM_NO_CARD\n");
		return SD_SIM_NO_CARD;
	}
	/* L9c fast mos off */
	regulator_fast_mos_vreg(REGULATOR_VREG_TYPE_WRITE, REGULATOR_VREG_ADDR_L9C, 0x0);
	/* L5c fast mos off */
	regulator_fast_mos_vreg(REGULATOR_VREG_TYPE_WRITE, REGULATOR_VREG_ADDR_L5C, 0x0);
	msleep(5); /* delay 5ms */
	/* sd singal io switch on,0x0:sd,0x1:sim */
	ret = sdhci_sd_sim_singal_switch(mmc_detect, SD_SIM_SWITCH_SD);
	if (ret) {
		pr_err("sdsim switch to sd singal pin fail,ret:%d\n", ret);
		return ret;
	}
	msleep(10); /* delay 10ms */
	/* L5c 1.8V power on */
	ret = sdhci_msm_setup_vreg_sdsim(mmc_detect, VDD_SIM_PMU_INDEX, VOL_LOW_LEVEL, VDD_PMU_POWER_ON);
	if (ret) {
		pr_err("sdsim vdd(L5c) sim power on fail,ret:%d\n", ret);
		return ret;
	}
	msleep(10); /* delay 10ms */
	/* sim gpio setup for sd */
	sim_gpio_setup(mmc_detect);

	mmc_claim_host(mmc_detect);
	msleep(SLEEP_MS_TIME_FOR_DETECT_UNSTABLE);
	pr_err("sdsim %s enter CMD1-RESPONSE STATUS detect stage after sleep 20 ms\n", __func__);
	detect_result = mmc_detect_sd_mmc(mmc_detect);
	msleep(SLEEP_MS_TIME_FOR_DETECT_UNSTABLE);
	pr_err("sdsim %s enter OVER-ALL STATUS detect stage after sleep 20 ms\n", __func__);
	mmc_release_host(mmc_detect);
	pr_err("sdsim sdsim_send_cmd_detect detect_result %d\n", detect_result);
	return detect_result;
}

int sdsim_detect_result_handle(struct mmc_host *host)
{
	int ret;

	if (host->sd_cmd_det_result == SD_SIM_NO_CARD) {
		if (g_sd_sim_detect_status_old == SD_SIM_NO_CARD) {
			g_sd_sim_detect_status_current = g_sd_sim_detect_status_old;
		} else {
			g_sd_sim_detect_status_current = SD_SIM_NO_CARD;
			pr_err("sdsim notify no card type %d\n", g_sd_sim_detect_status_current);
			hwril_sim_sd_nmc_detect_result_notify(g_sd_sim_detect_status_current);
		}
		return g_sd_sim_detect_status_current;
	}
	pr_err("sdsim detect handle sd_card_inserted %d\n", host->sd_card_inserted);
	if (host->sd_card_inserted) {
		msleep(20); /* delay 20ms */
		/* L5c fast mos on */
		regulator_fast_mos_vreg(REGULATOR_VREG_TYPE_WRITE, REGULATOR_VREG_ADDR_L5C, 0x80);
		msleep(10); /* delay 10ms */
		/* L5c power off */
		ret = sdhci_msm_setup_vreg_sdsim(host, VDD_SIM_PMU_INDEX, VOL_LOW_LEVEL, VDD_PMU_POWER_OFF);
		if (ret) {
			pr_err("sdsim vdd(L5c) sim power off fail,ret:%d\n", ret);
			return ret;
		}
		/* delay 200ms */
		msleep(200);
		/* L5c fast mos off */
		regulator_fast_mos_vreg(REGULATOR_VREG_TYPE_WRITE, REGULATOR_VREG_ADDR_L5C, 0x0);
		msleep(5); /* delay 5ms */
		/* L9c fast mos on */
		regulator_fast_mos_vreg(REGULATOR_VREG_TYPE_WRITE, REGULATOR_VREG_ADDR_L9C, 0x80);
		msleep(5); /* delay 5ms */
		if (g_sd_sim_detect_status_old == SD_SIM_SD_NORMAL) {
			g_sd_sim_detect_status_current = g_sd_sim_detect_status_old;
		} else {
			g_sd_sim_detect_status_current = SD_SIM_SD_NORMAL;
			pr_err("sdsim notify sd card type %d\n", g_sd_sim_detect_status_current);
			hwril_sim_sd_nmc_detect_result_notify(g_sd_sim_detect_status_current);
		}
		return g_sd_sim_detect_status_current;
	} else {
		msleep(20); /* delay 20ms */
		/* L5c fast mos on */
		regulator_fast_mos_vreg(REGULATOR_VREG_TYPE_WRITE, REGULATOR_VREG_ADDR_L5C, 0x80);
		msleep(5); /* delay 5ms */
		/* L5c power off */
		ret = sdhci_msm_setup_vreg_sdsim(host, VDD_SIM_PMU_INDEX, VOL_LOW_LEVEL, VDD_PMU_POWER_OFF);
		if (ret) {
			pr_err("sdsim vdd(L5c) sim power off fail,ret:%d\n", ret);
			return ret;
		}
		/* delay 200ms */
		msleep(200);
		/* L9c fast mos off */
		regulator_fast_mos_vreg(REGULATOR_VREG_TYPE_WRITE, REGULATOR_VREG_ADDR_L9C, 0x0);
		/* L5c fast mos on */
		regulator_fast_mos_vreg(REGULATOR_VREG_TYPE_WRITE, REGULATOR_VREG_ADDR_L5C, 0x80);
		msleep(5); /* delay 5ms */
		/* sim singal io switch on,0x0:sd,0x1:sim */
		ret = sdhci_sd_sim_singal_switch(host, SD_SIM_SWITCH_SIM);
		if (ret) {
			pr_err("sdsim switch to sim singal pin fail,ret:%d\n", ret);
			return ret;
		}
		/* adjust vddio high level to 2.96v */
		sdhci_msm_adjust_vddio_level(host, VOL_HIGH_LEVEL);
		/* L6c 2.96V power on */
		ret = sdhci_msm_setup_vreg_sdsim(host, VDD_SD_IO_PMU_INDEX, VOL_HIGH_LEVEL, VDD_PMU_POWER_ON);
		if (ret) {
			pr_err("sdsim vdd-io(L6c) sd power on fail,ret:%d\n", ret);
			return ret;
		}
		msleep(5); /* delay 5ms */
		/* sd io config to input & no pull */
		ret = sd_sim_set_pin(host, "sd_off");
		if (ret) {
			pr_err("sdsim set sd_off pin fail,ret:%d\n", ret);
			return ret;
		}
		msleep(5); /* delay 5ms */
		if (g_sd_sim_detect_status_old == SD_SIM_SIM_NORMAL ||
			g_sd_sim_detect_status_old == SD_SIM_SIM_FAIL) {
			g_sd_sim_detect_status_current = g_sd_sim_detect_status_old;
		} else {
			g_sd_sim_detect_status_current = SD_SIM_SIM_NORMAL;
			pr_err("sdsim notify sim card type %d\n", g_sd_sim_detect_status_current);
			hwril_sim_sd_nmc_detect_result_notify(g_sd_sim_detect_status_current);
		}
		return g_sd_sim_detect_status_current;
	}

}

void sdsim_detect_run(struct mmc_host *mmc)
{
	struct mmc_host *detect_host = mmc;
	int result;
	int gpio_detect = mmc_gpio_get_cd(detect_host);

	mutex_lock(&card_insert_muxlock);
	detect_host->sd_card_inserted = (gpio_detect == 1) ? 1 : 0;
	detect_host->sd_cmd_det_result = 1; /* init detect result */
	result = sdsim_send_cmd_detect(detect_host, !(gpio_detect));
	if (result == 0)
		detect_host->sd_card_inserted = 1;
	else
		detect_host->sd_card_inserted = 0;
	detect_host->sd_cmd_det_result = result;
	mutex_unlock(&card_insert_muxlock);
}

void sdsim_vddio_power_off(struct mmc_host *host, int cd_detect)
{
	int ret;
	/* vddio power off when card tray plug out */
	if (cd_detect == 0) {
		if (g_sd_sim_detect_status_current == SD_SIM_SIM_NORMAL ||
			g_sd_sim_detect_status_current == SD_SIM_SIM_FAIL ||
			g_sd_sim_detect_status_current == SD_SIM_SIM_PASS) {
			/* L6c power off:sim -> sd,sim -> sim */
			ret = sdhci_msm_setup_vreg_sdsim(host, VDD_SD_IO_PMU_INDEX,
				VOL_SET_LEVEL, VDD_PMU_POWER_OFF);
			if (ret)
				pr_err("sdsim vdd-io(L6c) sd power on fail,ret:%d\n", ret);
		}
	}
}

bool sdsim_sim_detect_manage(int cd_detect)
{
	if (cd_detect == 1) {
		if (g_sd_sim_detect_status_current == SD_SIM_SIM_FAIL ||
			g_sd_sim_detect_status_current == SD_SIM_SIM_PASS ||
			g_sd_sim_detect_status_current == SD_SIM_SIM_NORMAL ||
			g_sd_sim_detect_status_current == SD_SIM_SD_NORMAL) {
			pr_err("sdsim card has been detected,no need to detect\n");
			return true;
		}
	} else if (cd_detect < 0) {
		pr_err("sdsim cd_detect exception,no need to detect\n");
		return true;
	}
	return false;
}

void sdsim_detect(struct mmc_host *host)
{
	int ret;
	int cd_detect = mmc_gpio_get_cd(host);

	pr_err("sdsim mmc_rescan shared tray start,g_sd_sim_detect_status_current %d\n",
		g_sd_sim_detect_status_current);

	sdsim_vddio_power_off(host, cd_detect);
	if (sdsim_sim_detect_manage(cd_detect)) {
		pr_err("sdsim card detect return\n");
		return ;
	}
	g_sd_sim_detect_status_old = g_sd_sim_detect_status_current;
	g_sd_sim_detect_status_current = SD_SIM_UNDETECTED;
	pr_err("sdsim rescan init g_sd_sim_detect_status_current %d",
		g_sd_sim_detect_status_current);
	/* adjust vddio high level to 1.8v */
	sdhci_msm_adjust_vddio_level(host, VOL_LOW_LEVEL);
	if (sdhci_msm_vdd_is_enabled(host) && cd_detect) {
		pr_err("sdsim vdd is enabled,can not detect\n");
		return ;
	}
	sdsim_detect_run(host);
	ret = sdsim_detect_result_handle(host);
	if (ret)
		pr_err("sdsim sdsim_detect ret %d status %d\n",
			ret, g_sd_sim_detect_status_current);
	pr_err("sdsim mmc_rescan shared tray end\n");
}


