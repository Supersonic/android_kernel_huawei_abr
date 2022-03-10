 /*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: This program detect the card tray gpio status
 * Author: xubuchao2@huawei.com
 * Create: 2019-11-27
 */
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <huawei_platform/log/hw_log.h>
#include <securec.h>
#include "hiview_detect_fpc_dmd.h"
#include "card_tray_gpio_detect.h"
#define HWLOG_TAG card_tray_detect
HWLOG_REGIST();

static int g_card_tray_gpio = -1;

static ssize_t card_tray_show(struct device *dev, struct device_attribute *attr,
                              char *buf)
{
    int gpio_value;

    if (g_card_tray_gpio < 0) {
        hwlog_err("%s : The card_tray_gpio is error!\n", __func__);
        return 0;
    }
    gpio_value = gpio_get_value(g_card_tray_gpio);
    if (gpio_value < 0) {
        hwlog_err("%s : Failed to get gpio value!\n", __func__);
        return 0;
    }
    hwlog_info("%s get gpio value is %d\n", __func__, gpio_value);
    return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%d\n", gpio_value);
}

/* files create */
static DEVICE_ATTR(card_tray_status, S_IRUGO, card_tray_show, NULL);

static struct attribute *g_card_tray_attrs[] = {
    &dev_attr_card_tray_status.attr,
    NULL
};

static const struct attribute_group g_card_tray_sysfs_attr_group = {
    .attrs = g_card_tray_attrs,
};

static int CardTrayDetectProbe(struct platform_device *pdev)
{
    int ret;
    struct card_tray_info *di = NULL;
    struct device_node *card_tray_node = pdev->dev.of_node;
    struct device *card_tray_dev = NULL;
    struct class *hw_card_tray_detect_class = NULL;

    di = kzalloc(sizeof(*di), GFP_KERNEL);
    if (!di) {
        hwlog_err("%s : alloc di failed!\n", __func__);
        return -ENOMEM;
    }
    di->dev = &pdev->dev;
    ret = BtbFpcDetect(di, card_tray_node);
    if (ret < 0) {
        hwlog_err("%s : Failed to fpc detect!\n", __func__);
    }
    ret = of_property_read_u32(card_tray_node, "card_tray_gpio", (u32 *)
        (&g_card_tray_gpio));
    if (ret) {
        goto free_di;
    }
    hw_card_tray_detect_class = class_create(THIS_MODULE, "hw_card_tray");
    if (IS_ERR(hw_card_tray_detect_class)) {
        ret = PTR_ERR(hw_card_tray_detect_class);
        goto free_di;
    }

    card_tray_dev = device_create(hw_card_tray_detect_class, NULL, 0, NULL,
        "card_tray");
    if (card_tray_dev == NULL) {
        hwlog_err("%s : Failed to create card_tray device!\n", __func__);
        goto free_class;
    }
    ret = sysfs_create_group(&di->dev->kobj, &g_card_tray_sysfs_attr_group);
    if (ret) {
        goto free_device;
    }
    ret = sysfs_create_link(&card_tray_dev->kobj, &di->dev->kobj,
                            "card_tray_data");
    if (ret) {
        goto free_group;
    }
    hwlog_info("Huawei card tray detect probe ok!\n");
    return 0;

free_group:
    sysfs_remove_group(&di->dev->kobj, &g_card_tray_sysfs_attr_group);
free_device:
    device_destroy(hw_card_tray_detect_class, 0);
    card_tray_dev = NULL;
free_class:
    class_destroy(hw_card_tray_detect_class);
    hw_card_tray_detect_class = NULL;
free_di:
    di->dev = NULL;
    kfree(di);
    di = NULL;
    ret = -1;
    return ret;
}

/*
 * probe match table
 */
static const struct of_device_id g_card_tray_detect_table[] = {
    {
        .compatible = "huawei,card_tray_detect",
        .data = NULL,
    },
    {},
};

/*
 * card tray detect driver
 */
static struct platform_driver g_card_tray_detect_driver = {
    .probe = CardTrayDetectProbe,
    .driver = {
        .name = "huawei,card_tray_detect",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(g_card_tray_detect_table),
    },
};

/*
 * Function: CardTrayDetectInit
 * Description: card tray status gpio detect module initialization
 * Parameters:	Null
 * return value: 0-sucess or others-fail
 */
static int __init CardTrayDetectInit(void)
{
    int ret = 0;

    hwlog_info("into init");
    ret = platform_driver_register(&g_card_tray_detect_driver);
    return ret;
}

/*
 * Function:	   CardTrayDetectExit
 * Description:	   card tray status gpio detect module exit
 * Parameters:	 NULL
 * return value:  NULL
 */
static void __exit CardTrayDetectExit(void)
{
    platform_driver_unregister(&g_card_tray_detect_driver);
}

module_init(CardTrayDetectInit);
module_exit(CardTrayDetectExit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("huawei card tray status detect driver");
MODULE_AUTHOR("HUAWEI Inc");
