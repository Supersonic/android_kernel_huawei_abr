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
#include <linux/version.h>
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
#include <linux/wakelock.h>
#include <linux/qpnp/qpnp-adc.h>
#else
#include <linux/mutex.h>
#include <linux/iio/consumer.h>
#endif
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/power/huawei_adc.h>


struct adc_chip
{
    struct device           *dev;
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
    struct qpnp_vadc_chip   *vadc_dev;
#else
	struct iio_channel *vadc_chan;
#endif
    struct list_head        list;
    int                     channel;
    int                     switch_gpio;
    bool                    switch_enable;
    struct mutex            switch_lock;
    struct regulator        *gpio_pu_regulator;
    struct regulator        *gpio_pd_regulator;
    struct regulator        *regulator;
};
LIST_HEAD(vadc_device_list);

static struct adc_chip* find_adc(struct device_node *np)
{
    struct adc_chip *vadc = NULL;

    if (!np) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return NULL;
    }

    list_for_each_entry(vadc, &vadc_device_list, list)
    if (vadc->dev->of_node == np)
        return vadc;
    return NULL;
}

bool huawei_adc_init_done(struct device_node *np)
{
    if (!np) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return false;
    }

    if(find_adc(np)) {
        return true;
    } else {
        return false;
    }
}

static int get_adc_value_raw(struct device_node *np,
                            int gpio_up, int64_t *result)
{
    int rc = 0;
    int gpio_status = 0;
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
    struct qpnp_vadc_result results;
#else
	int res = 0;
#endif
    struct adc_chip *vadc = NULL;
    struct regulator *ldo = NULL;

    if (!np || !result) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return -EINVAL;
    }

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
    memset(&results, 0, sizeof(struct qpnp_vadc_result));
#endif

    vadc = find_adc(np);
    if(NULL == vadc)
    {
        pr_err("Error %s not find vadc\n", __func__);
        return -ENODEV;
    }

    mutex_lock(&vadc->switch_lock);
    if(vadc->switch_enable){
        gpio_status = gpio_get_value(vadc->switch_gpio);
        if(gpio_up) {
            if(gpio_status != GPIO_UP){
                gpio_set_value(vadc->switch_gpio, GPIO_UP);
                pr_err("%s switch mode %d\n", __func__,
                        gpio_get_value(vadc->switch_gpio));
            }
            ldo = vadc->gpio_pu_regulator;
        } else {
            if(gpio_status != GPIO_DOWN){
                gpio_set_value(vadc->switch_gpio, GPIO_DOWN);
                pr_err("%s switch mode %d\n", __func__,
                        gpio_get_value(vadc->switch_gpio));
            }
            ldo = vadc->gpio_pd_regulator;
        }
    } else {
        ldo = vadc->regulator;
    }

    if (NULL != ldo) {
        rc = regulator_enable(ldo);
        if (rc) {
            pr_err("%s: enable ldo fail, rc = %d\n", __func__, rc);
        }
    }
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
    rc = qpnp_vadc_read(vadc->vadc_dev ,vadc->channel, &results);
#else
	rc = iio_read_channel_processed(vadc->vadc_chan, &res);
	*result = (int64_t)res;
#endif
	if (rc < 0)
		pr_err("Unable to read sub vadc temp rc=%d\n", rc);

    if (NULL != ldo) {
        rc = regulator_disable(ldo);
        if (rc) {
            pr_err("%s: disable ldo fail, rc = %d\n", __func__, rc);
        }
    }

    mutex_unlock(&vadc->switch_lock);
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
    *result = results.physical;
#endif
    return rc;
}

int get_adc_converse_voltage(struct device_node *np, int gpio_up)
{
    int rc = 0, result = 0;
    int64_t value_raw = 0;

    if(NULL == np)
    {
        pr_err("Error %s not find np\n", __func__);
        return -EINVAL;
    }

    rc = get_adc_value_raw(np, gpio_up, &value_raw);
    if (rc) {
        pr_err("Unable to read adc voltage rc = %d\n", rc);
        return rc;
    }

    result = div_s64(value_raw ,GET_PMI_SUB_VOLTAGE_RETURN_RATIO);
    return result;
}

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
static int32_t huawei_vadc_map_temp_voltage(const struct qpnp_vadc_map_pt *pts,
                    uint32_t tablesize, int32_t input, int32_t *output)
{
    bool descending = 1;
    uint32_t i = 0;

    if (!pts || !output) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return -EINVAL;
    }

    /* Check if table is descending or ascending */
    if (tablesize > 1) {
        if (pts[0].y < pts[1].y)
            descending = 0;
    }

    while (i < tablesize) {
        if ((descending == 1) && (pts[i].y < input)) {
            /* table entry is less than measured
                value and table is descending, stop */
            break;
        } else if ((descending == 0) && (pts[i].y > input)) {
            /* table entry is greater than measured
                value and table is ascending, stop */
            break;
        } else {
            i++;
        }
    }

    if (i == 0) {
        *output = pts[0].x;
    } else if (i == tablesize) {
        *output = pts[tablesize-1].x;
    } else {
        /* result is between search_index and search_index-1 */
        /* interpolate linearly */
        *output = (((int32_t) ((pts[i].x - pts[i-1].x)*
            (input - pts[i-1].y))/
            (pts[i].y - pts[i-1].y))+
            pts[i-1].x);
    }

    return 0;
}

int get_adc_converse_temp(struct device_node *np,
                int gpio_up, struct qpnp_vadc_map_pt *pts,
                uint32_t tablesize, int32_t *output)
{
    int rc = 0;
    int32_t volt = 0, result = 0;
    int64_t value_raw = 0;

    if (!np || !pts || !output) {
        pr_err("Error %s not found np\n", __func__);
        return -EINVAL;
    }

    rc = get_adc_value_raw(np, gpio_up, &value_raw);
    if (rc) {
        pr_err("Unable to read adc voltage rc = %d\n", rc);
        return rc;
    }

    volt = div_s64(value_raw ,GET_PMI_SUB_VOLTAGE_RETURN_RATIO);
    rc = huawei_vadc_map_temp_voltage(pts, tablesize, volt, &result);
    if (rc < 0) {
        pr_err("%s: read temp for error, rc = %d\n", __func__, rc);
    }
    *output = result;
    return rc;
}
#endif

static const struct of_device_id huawei_vadc_match_table[] = {
    {   .compatible = "huawei-vadc",
    },
    {}
};

static int huawei_vadc_probe(struct platform_device *pdev)
{
    int ret = 0;
    const char *name = NULL;
    struct adc_chip *vadc = NULL;
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	struct qpnp_vadc_chip *vadc_dev = NULL;
#else
	struct iio_channel *vadc_chan = NULL;
#endif
    struct device_node* np = NULL;

    if (!pdev) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return -EINVAL;
    }

    np = pdev->dev.of_node;
    if(NULL == np)
    {
        pr_err("np is NULL\n");
        return -EINVAL;
    }

    vadc = devm_kzalloc(&pdev->dev, sizeof(struct adc_chip), GFP_KERNEL);
    if (!vadc) {

        pr_err("Unable to allocate memory\n");
        return -ENOMEM;
    }
    vadc->dev = &(pdev->dev);

    ret = of_property_read_string(np, "vadc-name", &name);
    pr_info("huawei read %s\n", name);
    if(ret)
    {
        pr_err("of_property_read_string read err\n");
        goto READ_PROP_FAIL;
    }

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
    vadc_dev = qpnp_get_vadc(vadc->dev, name);
    if (IS_ERR(vadc_dev)){
        pr_err("huawei adc probe get vadc_dev err %ld\n", PTR_ERR(vadc_dev));
        ret = -EPROBE_DEFER;
        goto READ_PROP_FAIL;
    }
    vadc->vadc_dev = vadc_dev;
    dev_set_drvdata(&(pdev->dev), vadc);

    ret = of_property_read_u32(np, "channel", &(vadc->channel));
    if (ret) {
        pr_err("Read adc channel fail, err %d\n", ret);
        ret = -EFAULT;
        goto READ_PROP_FAIL;
    }
#else
	vadc_chan = iio_channel_get(vadc->dev, name);
	if (IS_ERR(vadc_chan)) {
		pr_err("huawei adc probe get vadc_dev err %ld\n",
			 PTR_ERR(vadc_chan));
		ret = -EPROBE_DEFER;
		goto READ_PROP_FAIL;
	}
	vadc->vadc_chan = vadc_chan;
	dev_set_drvdata(&(pdev->dev), vadc);
#endif

    vadc->switch_enable = of_property_read_bool(np, "huawei,need_switch_enable");
    if(vadc->switch_enable) {
        vadc->switch_gpio = of_get_named_gpio(np,"switch_gpio" ,0);
        if(!gpio_is_valid(vadc->switch_gpio))
        {
            pr_err("gpio_int is not valid\n");
            vadc->switch_enable = false;
        } else {
            ret = gpio_request(vadc->switch_gpio, "switch_gpio");
            if(ret)
            {
                pr_err("could not request gpio_int\n");
                vadc->switch_enable = false;
            } else {
                gpio_direction_output(vadc->switch_gpio, 0);
            }
        }
    }

    /* parse regulator */
    vadc->gpio_pu_regulator = regulator_get(vadc->dev, "gpio-pu-regulator");
    if (IS_ERR(vadc->gpio_pu_regulator)) {
        vadc->gpio_pu_regulator = NULL;
    }
    vadc->gpio_pd_regulator = regulator_get(vadc->dev, "gpio-pd-regulator");
    if (IS_ERR(vadc->gpio_pd_regulator)) {
        vadc->gpio_pd_regulator = NULL;
    }
    vadc->regulator = regulator_get(vadc->dev, "regulator");
    if (IS_ERR(vadc->regulator)) {
        vadc->regulator = NULL;
    }

    mutex_init(&vadc->switch_lock);
    list_add(&vadc->list, &vadc_device_list);

    pr_info("%s is ok\n", __func__);
    return 0;

READ_PROP_FAIL:
    devm_kfree(&pdev->dev, vadc);
    return ret;
}

static int huawei_vadc_remove(struct platform_device *pdev)
{
    struct adc_chip *vadc = NULL;

    if (!pdev) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return -EINVAL;
    }

    vadc = dev_get_drvdata(&pdev->dev);
    if (!vadc) {
        pr_err("%s: Cannot get adc, fatal error\n", __func__);
        return -EINVAL;
    }

    mutex_destroy(&vadc->switch_lock);
    list_del(&vadc->list);
    if(vadc->switch_enable) {
        gpio_free(vadc->switch_gpio);
    }
    devm_kfree(&(pdev->dev), vadc);
    return 0;
}

static struct platform_driver huawei_vadc_driver = {
    .driver     = {
        .name   = "huawei-vadc",
        .of_match_table = huawei_vadc_match_table,
    },
    .probe      = huawei_vadc_probe,
    .remove     = huawei_vadc_remove,
};

static int __init huawei_vadc_init(void)
{
    return platform_driver_register(&huawei_vadc_driver);
}
module_init(huawei_vadc_init);

static void __exit huawei_vadc_exit(void)
{
    platform_driver_unregister(&huawei_vadc_driver);
}
module_exit(huawei_vadc_exit);

MODULE_DESCRIPTION("QPNP PMIC Voltage ADC driver");
MODULE_LICENSE("GPL v2");

