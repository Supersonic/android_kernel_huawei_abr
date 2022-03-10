/*
 * Copyright (C) 2012-2015 HUAWEI, Inc.
 * Author: HUAWEI, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/qpnp/qpnp-adc.h>
#include <linux/platform_device.h>
#include <linux/huawei_adc.h>

#include <linux/delay.h>
#define BUF_MAX_LENGTH 8
#define Voltage_TEMP_NUM 166
#define TEMP_START -40

static struct qpnp_vadc_map_pt voltage_temp[Voltage_TEMP_NUM];
struct thermal_chip
{
    struct list_head list;
    struct device   *dev;
    struct device_node *np_node;
    struct device_attribute attr;
    int switch_mode;
    const char *file_name;
    int *voltage_list;
    int voltage_temp_num;
};

static LIST_HEAD(therm_dev_list);

#define SYSFS_NODE_MODE (S_IRUGO)
static ssize_t thermal_sysfs_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int32_t ret = 0;
	int32_t result = 0;

	struct thermal_chip *hw_chip = container_of(attr, struct thermal_chip, attr);
	if (!hw_chip) {
		pr_err("%s: Invalid param, fata error\n", __func__);
		return -EINVAL;
	}

	ret = get_adc_converse_temp(hw_chip->np_node, hw_chip->switch_mode,
					voltage_temp, ARRAY_SIZE(voltage_temp), &result);
	return snprintf(buf, BUF_MAX_LENGTH, "%d", result);
}

#define PA_THERM_NODE   "pa_thermal"
#define MSM_THERM_NODE  "msm_thermal"
#define CHG_THERM_NODE  "chg_thermal"
#define THERM_TEMP_DEFAULT     25
int dsm_get_pa_temp(void)
{
    int ret = 0;
    int32_t result = 0;

    struct thermal_chip *chip = NULL;
    list_for_each_entry(chip, &therm_dev_list, list) {
        if (!strncmp(chip->file_name,
                PA_THERM_NODE, sizeof(PA_THERM_NODE))) {
            break;
        }
    }

    if (chip == NULL) {
        pr_err("%s: Cannot find pa thermal device, use default\n", __func__);
        return THERM_TEMP_DEFAULT;
    }

    ret = get_adc_converse_temp(chip->np_node, chip->switch_mode,
                    voltage_temp, ARRAY_SIZE(voltage_temp), &result);
    if (ret) {
        pr_err("%s: Cannot read pa therm temp, use default\n", __func__);
        return THERM_TEMP_DEFAULT;
    }

    return result;
}
EXPORT_SYMBOL(dsm_get_pa_temp);

static int pa_thermal;
static int get_pa_temp(char *buf, struct kernel_param *kp)
{
	int pa_temp;
	pa_temp = dsm_get_pa_temp();
	return snprintf(buf, BUF_MAX_LENGTH, "%d", pa_temp);
}
module_param_call(pa_thermal, NULL,get_pa_temp, &pa_thermal, 0644);


int dsm_get_cpu_temp(void)
{
    int ret = 0;
    int32_t result = 0;

    struct thermal_chip *chip = NULL;
    list_for_each_entry(chip, &therm_dev_list, list) {
        if (!strncmp(chip->file_name,
                MSM_THERM_NODE, sizeof(MSM_THERM_NODE))) {
            break;
        }
    }

    if (chip == NULL) {
        pr_err("%s: Cannot find msm thermal device, use default\n", __func__);
        return THERM_TEMP_DEFAULT;
    }

    ret = get_adc_converse_temp(chip->np_node, chip->switch_mode,
                    voltage_temp, ARRAY_SIZE(voltage_temp), &result);
    if (ret) {
        pr_err("%s: Cannot read msm therm temp, use default\n", __func__);
        return THERM_TEMP_DEFAULT;
    }

    return result;
}
EXPORT_SYMBOL(dsm_get_cpu_temp);

static int msm_thermal;
static int get_msm_temp(char *buf, struct kernel_param *kp)
{
	int msm_temp;
	msm_temp = dsm_get_cpu_temp();
	return snprintf(buf, BUF_MAX_LENGTH, "%d",msm_temp);
}
module_param_call(msm_thermal, NULL,get_msm_temp, &msm_thermal, 0644);

int dsm_get_chg_temp(void)
{
    int ret = 0;
    int32_t result = 0;

    struct thermal_chip *chip = NULL;
    list_for_each_entry(chip, &therm_dev_list, list) {
        if (!strncmp(chip->file_name,
                CHG_THERM_NODE, sizeof(PA_THERM_NODE))) {
            break;
        }
    }

    if (chip == NULL) {
        pr_err("%s: Cannot find chg thermal device, use default\n", __func__);
        return THERM_TEMP_DEFAULT;
    }

    ret = get_adc_converse_temp(chip->np_node, chip->switch_mode,
                    voltage_temp, ARRAY_SIZE(voltage_temp), &result);
    if (ret) {
        pr_err("%s: Cannot read chg therm temp, use default\n", __func__);
        return THERM_TEMP_DEFAULT;
    }

    return result;
}
EXPORT_SYMBOL(dsm_get_chg_temp);

static int chg_thermal;
static int get_chg_temp(char *buf, struct kernel_param *kp)
{
	int chg_temp;
	chg_temp = dsm_get_chg_temp();
	return snprintf(buf, BUF_MAX_LENGTH, "%d", chg_temp);
}
module_param_call(chg_thermal, NULL,get_chg_temp, &chg_thermal, 0644);

static const struct of_device_id huawei_thermal_match_table[] = {
	{	.compatible = "huawei-thermal",
	},
	{}
};

static int huawei_thermal_probe(struct platform_device *pdev)
{
    struct thermal_chip *hw_thermal_chip = NULL;
    int ret = 0;
    struct device_node* np = NULL;
    bool hw_adc_init = 0;
    struct class *thermal_class = NULL;
    int i =0;

    np = pdev->dev.of_node;
    if(NULL == np)
    {
        pr_err("np is NULL\n");
        return -1;
    }
	hw_thermal_chip = devm_kzalloc(&pdev->dev, sizeof(struct thermal_chip), GFP_KERNEL);
	if (!hw_thermal_chip) {
		pr_err("Unable to allocate memory\n");
		return -ENOMEM;
	}
	hw_thermal_chip->dev = &(pdev->dev);
    hw_thermal_chip->np_node = of_parse_phandle(np, "qcom,cpu-thermal-vadc", 0);
    if (!hw_thermal_chip->np_node) {
        dev_err(hw_thermal_chip->dev, "Missing huawei_thermal_probe config\n");
        ret = -EINVAL;
		goto FAIL;
    }
	ret = of_property_read_u32(np, "voltage-temp-num", &(hw_thermal_chip->voltage_temp_num));
	if(hw_thermal_chip->voltage_temp_num > 0)
	{
		hw_thermal_chip->voltage_list = devm_kzalloc(&pdev->dev,sizeof(int) * hw_thermal_chip->voltage_temp_num, GFP_KERNEL);
		if (!hw_thermal_chip->voltage_list) {
			pr_err("Unable to allocate memory fot voltage_list\n");
			ret = -ENOMEM;
			goto FAIL;
		}
		ret = of_property_read_u32_array(np, "voltage-list",hw_thermal_chip->voltage_list, hw_thermal_chip->voltage_temp_num);
		if (ret < 0) {
		pr_err("%s: Get voltage_list fail!!! ret = %d\n", __func__, ret);
		devm_kfree(&pdev->dev, hw_thermal_chip->voltage_list);
		goto FAIL;
		}
		for(i = 0;i < hw_thermal_chip->voltage_temp_num;i++)
		{
			voltage_temp[i].x = i+TEMP_START;
			voltage_temp[i].y = hw_thermal_chip->voltage_list[i];
		}
	}

	ret = of_property_read_u32(np, "switch-mode", &(hw_thermal_chip->switch_mode));
	if (ret) {
		pr_debug("%s: Get gpio switch mode fail!!! ret = %d\n", __func__, ret);
		hw_thermal_chip->switch_mode = 0;
	}
    hw_adc_init = huawei_adc_init_done(hw_thermal_chip->np_node);
	if (!hw_adc_init) {
	 	dev_err(hw_thermal_chip->dev, "huawei_thermal_probe not found; defer probe\n");
		ret = -EPROBE_DEFER;
		goto FAIL;
	}
	ret = of_property_read_string(np, "file-node", &hw_thermal_chip->file_name);
	if (ret) {
		dev_err(hw_thermal_chip->dev, "%s: cannot get file node name, fatal error\n", __func__);
		ret = -EINVAL;
		goto FAIL;
	}
	dev_set_drvdata(&(pdev->dev), hw_thermal_chip);

	hw_thermal_chip->attr.attr.name = hw_thermal_chip->file_name;
	hw_thermal_chip->attr.attr.mode = SYSFS_NODE_MODE;
	hw_thermal_chip->attr.show = thermal_sysfs_show;
	hw_thermal_chip->attr.store = NULL;

	ret = device_create_file(hw_thermal_chip->dev, &hw_thermal_chip->attr);
	if (ret) {
		pr_err("%s: cannot create thermal file %s\n", __func__, hw_thermal_chip->file_name);
		ret = -EINVAL;
		goto FAIL;
	}

    list_add_tail(&hw_thermal_chip->list, &therm_dev_list);

    pr_info("%s is ok\n",__func__);
	return 0;

FAIL:
	devm_kfree(&pdev->dev, hw_thermal_chip);
	return ret;
}

static int huawei_thermal_remove(struct platform_device *pdev)
{
	struct thermal_chip *thermal = dev_get_drvdata(&pdev->dev);
    devm_kfree(thermal->dev, thermal);
	return 0;
}

static struct platform_driver huawei_thermal_driver =
{
	.probe = huawei_thermal_probe,
	.remove = huawei_thermal_remove,
	.driver =
	{
		.name = "huawei-thermal",
		.of_match_table = huawei_thermal_match_table,
	},
};

static int __init huawei_thermal_init(void)
{
	return platform_driver_register(&huawei_thermal_driver);
}
module_init(huawei_thermal_init);

static void __exit huawei_thermal_exit(void)
{
	platform_driver_unregister(&huawei_thermal_driver);
}
module_exit(huawei_thermal_exit);

MODULE_DESCRIPTION("QPNP PMIC Voltage ADC driver");
MODULE_LICENSE("GPL v2");
