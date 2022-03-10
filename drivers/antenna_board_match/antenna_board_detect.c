
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/version.h>
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
#include <linux/qpnp/qpnp-adc.h>
#endif
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include "antenna_board_detect.h"
#include <linux/power/huawei_adc.h>

#define ANTENNA_SYSFS_FIELD(_name, n, m, store)      \
{                                                    \
    .attr = __ATTR(_name, m, antenna_show, store),    \
    .name = ANTENNA_##n,                            \
}

#define ANTENNA_SYSFS_FIELD_RO(_name, n)            \
        ANTENNA_SYSFS_FIELD(_name, n, S_IRUGO, NULL)

static ssize_t antenna_show(struct device *dev,
        struct device_attribute *attr, char *buf);

struct antenna_detect_info {
    struct device_attribute attr;
    u8 name;
};
static struct antenna_device_info *g_di = NULL;

/* ADC match range, DONOT need change */
static int antenna_adc_match_range[2] = {820,975};
#define MAX_GPIO_NUM    3

static int g_gpio_count = 0;
static int g_gpio[MAX_GPIO_NUM] = {0};
static int g_board_version = -1;

static struct antenna_detect_info antenna_tb[] = {
    ANTENNA_SYSFS_FIELD_RO(antenna_board_match,    BOARD_STATUS_DETECT),
};

static struct attribute *antenna_sysfs_attrs[ARRAY_SIZE(antenna_tb) + 1];
static const struct attribute_group antenna_sysfs_attr_group = {
    .attrs = antenna_sysfs_attrs,
};
static void antenna_sysfs_init_attrs(void)
{
    int i = 0;
    int limit = ARRAY_SIZE(antenna_tb);

    for (i = 0; i < limit; i++) {
        antenna_sysfs_attrs[i] = &antenna_tb[i].attr.attr;
    }
    antenna_sysfs_attrs[limit] = NULL;
}

static struct antenna_detect_info *antenna_board_lookup(const char *name)
{
    int i = 0;
    int limit = ARRAY_SIZE(antenna_tb);

    if (!name) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return NULL;
    }

    for (i = 0; i< limit; i++) {
        if (!strncmp(name, antenna_tb[i].attr.attr.name, strlen(name)))
            break;
    }
    if (i >= limit)
        return NULL;
    return &antenna_tb[i];
}

static int antenna_detect_sysfs_create_group(struct antenna_device_info *di)
{
    if (!di) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return -EINVAL;
    }
    antenna_sysfs_init_attrs();
    return sysfs_create_group(&di->dev->kobj, &antenna_sysfs_attr_group);
}

static inline void antenna_detect_sysfs_remove_group(struct antenna_device_info *di)
{
    if (!di) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return;
    }
    sysfs_remove_group(&di->dev->kobj, &antenna_sysfs_attr_group);
}

static int get_boardid_det_status(void)
{
    int i = 0, ret = 0, err = 0;
    int temp_value = 0, gpio_value = 0;

    for (i = 0; i < g_gpio_count; i++) {
        temp_value = gpio_get_value(g_gpio[i]);
        gpio_value += (temp_value << i);
    }
    pr_info("gpio_value = %d g_gpio_count = %d\n", gpio_value, g_gpio_count);

    if (MAX_GPIO_NUM == g_gpio_count)
    {
        return gpio_value;
    }

    switch(gpio_value){
        case STATUS_UP_DOWN:
        case STATUS_DOWN_UP:
            ret = gpio_value;
            break;
        case STATUS_DOWN_DOWN:
            err = gpio_direction_output(g_gpio[0], 1);
            if (err) {
                pr_err("pio_direction_output high fail, err %d\n", err);
            }
            msleep(DELAY_MS);
            temp_value = gpio_get_value(g_gpio[1]);
            if (temp_value){
                ret = STATUS_DOWN_DOWN_OTHER;
            }
            else {
                ret =  STATUS_DOWN_DOWN;
            }
            break;
        case STATUS_UP_UP:
            gpio_direction_output(g_gpio[0], 0);
            if (err) {
                pr_err("pio_direction_output low fail, err %d\n", err);
            }
            msleep(DELAY_MS);
            temp_value = gpio_get_value(g_gpio[1]);
            if (temp_value){
                ret =  STATUS_UP_UP;
            }
            else {
                ret = STATUS_UP_UP_OTHER;
            }
            break;
        default:
            ret = STATUS_ERROR;
            pr_err("(%s)STATUS ERR!!HAVE NO STATUS:(%d)\n",__func__,gpio_value);
            break;
    }

    gpio_direction_input(g_gpio[0]);
    return ret;
}
/*********************************************************
*  Function:       check_match_by_adc
*  Discription:    check if main board is match with the rf board
*  Parameters:     none
*  Return value:   0-not match or 1-match
*********************************************************/
static int check_match_by_adc(void)
{
    int ret = 0;
    int rf_voltage = DEFAULT_VOL;
    if ((NULL == g_di)||(NULL == g_di->np_node)) {
        ret = -EINVAL;
        pr_err("%s Invalid argument: %d\n", __func__, ret);
        return ret;
    }
    rf_voltage = get_adc_converse_voltage(g_di->np_node,g_di->switch_mode);
    pr_info("Antenna board adc voltage = %d\n", rf_voltage);

    if (rf_voltage >= antenna_adc_match_range[0] && rf_voltage <= antenna_adc_match_range[1]) {
        ret = ANATENNA_DETECT_SUCCEED;
    }else {
        pr_err("adc voltage is not in range, Antenna_board_match error!\n");
        ret = ANATENNA_DETECT_FAIL;
    }

    return ret;
}
/*********************************************************
*  Function:       check_match_by_gpio
*  Discription:    check if main board is match with the rf board
*  Parameters:     none
*  Return value:   0-not match or 1-match
*********************************************************/
static int check_match_by_gpio(void)
{
    int ret = 0;
    int ret_status = get_boardid_det_status();

	if ( g_board_version == ret_status)
	{
		ret = ANATENNA_DETECT_SUCCEED;
	}
	else
	{
		ret = ANATENNA_DETECT_FAIL;
	}

	pr_info("%s get det_status = %d, board_version = %d, match = %d\n", __func__, ret_status, g_board_version, ret);

    return ret;
}

static ssize_t antenna_show(struct device *dev,
         struct device_attribute *attr, char *buf)
{
    int ret = -EINVAL;
    struct antenna_detect_info *info = NULL;

	if((NULL == dev)||(NULL == attr)||(NULL == buf))
	{
		pr_err("%s dev or attr is null\n", __func__);
		return ret;
	}
    info = antenna_board_lookup(attr->attr.name);
    if ((NULL == info)||(NULL == g_di)) {

        pr_err("%s Invalid argument: %d\n", __func__, ret);
        return ret;
    }

    switch(info->name) {
        case ANTENNA_BOARD_STATUS_DETECT:
			if(g_di->is_adc_mode){
	            ret = check_match_by_adc();
	            pr_info("%s Get adc match status is %d\n", __func__, ret);
			}else {
		        ret = check_match_by_gpio();
			}
            break;

        default:
            pr_err("(%s)NODE ERR!!HAVE NO THIS NODE:(%d)\n",__func__,info->name);
            break;
    }
    return snprintf(buf, PAGE_SIZE, "%d\n", ret);
}

static struct class *hw_antenna_class = NULL;
static struct class *antenna_board_detect_class = NULL;
struct device * antenna_dev = NULL;

/*get new class*/
struct class *hw_antenna_get_class(void)
{
    if (NULL == hw_antenna_class) {
        hw_antenna_class = class_create(THIS_MODULE, "hw_antenna");
        if (NULL == hw_antenna_class) {
            pr_err("hw_antenna_class create fail");
            return NULL;
        }
    }
    return hw_antenna_class;
}

static void ant_board_adc_dt_parse(struct antenna_device_info *di)
{
    struct device_node* np = NULL;
	int ret = 0;

    if(NULL == di){
        pr_err("%s di is null!\n",__FUNCTION__);
        return;
    }
    np = di->dev->of_node;
    if(NULL == np){
        pr_err("%s np is null!\n",__FUNCTION__);
        return;
    }
    di->np_node = of_parse_phandle(np, "qcom,antenna_adc-vadc", 0);
    if (!di->np_node) {
        dev_err(di->dev, "Missing adc-vadc config\n");
    }
	ret = of_property_read_u32(np, "switch-mode", (u32 *)&(di->switch_mode));
    if (ret) {
        pr_err("%s: read gpio switch mode fail !!! ret = %d\n", __func__, di->switch_mode);
        di->switch_mode = 0;
    }
    //match range
    if (of_property_read_u32_array(np, "antenna_board_match_range", antenna_adc_match_range, 2)) {
        pr_err("%s, antenna_board_match_range not exist, use default array!\n", __func__);
    }
    pr_info("antenna_adc_match_range: min = %d,max = %d\n",antenna_adc_match_range[0],antenna_adc_match_range[1]);
}

static void ant_board_gpio_dt_parse(struct antenna_device_info *di)
{
    struct device_node* np = NULL;
    int i = 0;
    enum of_gpio_flags flags = OF_GPIO_ACTIVE_LOW;

    if(NULL == di){
        pr_err("%s di is null!\n",__FUNCTION__);
        return;
    }

    np = di->dev->of_node;
    if(NULL == np){
        pr_err("%s np is null!\n",__FUNCTION__);
        return;
    }


    if (of_property_read_u32(np, "gpio_count", &g_gpio_count)) {
        /* default 1 antenna */
        g_gpio_count = DEFAULT_GPIO_CNT;
    }

    if (of_property_read_u32(np, "board_version", &g_board_version)) {
        /* default g_board_version is -1 */
        g_board_version = DEFAULT_GPIO_VERSION;
    }

    for (i = 0; i < g_gpio_count; i++) {
        g_gpio[i] = of_get_gpio_flags(np, i, &flags);
        pr_debug("g_gpio[%d] = %d\n", i, g_gpio[i]);

        if (!gpio_is_valid(g_gpio[i])) {
            pr_err("gpio_is_valid g_gpio[%d] = %d\n", i, g_gpio[i]);
            return;
        }
    }

}
#define DRV_NAME "antenna_board_gpio"

static int ant_boardid_det_gpio_process(int gpio)
{
    int err = 0;

    /* request a gpio and set the function direction in  */
    err = gpio_request_one(gpio, GPIOF_IN, DRV_NAME);
    if (err) {
        pr_err("Unable to request GPIO %d, err %d\n", gpio, err);
        return err;
    }

    return 0;
}

static int antenna_board_detect_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct antenna_device_info *di = NULL;
	int i = 0 ;

    if (!pdev) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return -EINVAL;
    }

    di = kzalloc(sizeof(*di), GFP_KERNEL);
    if (!di)
    {
        pr_err("alloc di failed\n");
        return -ENOMEM;
    }
    di->dev = &pdev->dev;
    dev_set_drvdata(&(pdev->dev), di);
	di->is_adc_mode = of_property_read_bool(di->dev->of_node,"is-adc-mode");
	if(di->is_adc_mode){

    /*get dts data*/
	    ant_board_adc_dt_parse(di);
	}else {
	    /* get dts data */
	    ant_board_gpio_dt_parse(di);

		    for (i = 0; i < g_gpio_count; i++) {
		        ret = ant_boardid_det_gpio_process(g_gpio[i]);
		        if (ret) {
		            pr_err("Process GPIO %d failed\n", g_gpio[i]);
		        } else {
		            pr_info("Process GPIO %d success\n", g_gpio[i]);
		        }
		    }
		}

	pr_err("antenna_detect_sysfs_create_group entry\n");
    ret = antenna_detect_sysfs_create_group(di);
    if (ret) {
        pr_err("can't create antenna_detect sysfs entries\n");
        goto Antenna_board_failed;
    }

	pr_err("antenna_detect_sysfs_create_group out\n");
    antenna_board_detect_class = hw_antenna_get_class();

	pr_err("antenna_board_detect_class out\n");
    if (antenna_board_detect_class) {
        if (NULL == antenna_dev)
        antenna_dev = device_create(antenna_board_detect_class, NULL, 0, NULL,"antenna_board");
        if(IS_ERR(antenna_dev)) {
            antenna_dev = NULL;
            pr_err("create rf_dev failed!\n");
            goto Antenna_board_failed;
        }

        ret = sysfs_create_link(&antenna_dev->kobj, &di->dev->kobj, "antenna_board_data");
        if (ret) {
            pr_err("create link to board_detect fail.\n");
            goto Antenna_board_failed;
        }
    }else {
        pr_err("get antenna_detect_class fail.\n");
        goto Antenna_board_failed;
    }
	g_di = di;
    pr_info("huawei antenna board detect probe ok!\n");
    return 0;

Antenna_board_failed:
    kfree(di);
    g_di = NULL;
    return ret;
}

static int antenna_board_detect_remove(struct platform_device *pdev)
{
    struct antenna_device_info *di = NULL;

    if (!pdev) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return -EINVAL;
    }

    di = dev_get_drvdata(&pdev->dev);
    if (NULL == di) {
        pr_err("[%s]di is NULL!\n",__func__);
        return -ENODEV;
    }

    kfree(di);
    g_di = NULL;
    return 0;
}
/*
 *probe match table
*/
static struct of_device_id antenna_board_table[] = {
    {
        .compatible = "huawei,antenna_board_detect",
        .data = NULL,
    },
    {},
};

/*
 *antenna board detect driver
 */
static struct platform_driver antenna_board_detect_driver = {
    .probe = antenna_board_detect_probe,
	.remove = antenna_board_detect_remove,
    .driver = {
        .name = "huawei,antenna_board_detect",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(antenna_board_table),
    },
};
/***************************************************************
 * Function: antenna_board_detect_init
 * Description: antenna board detect module initialization
 * Parameters:  Null
 * return value: 0-sucess or others-fail
 * **************************************************************/
static int __init antenna_board_detect_init(void)
{
    return platform_driver_register(&antenna_board_detect_driver);
}
/*******************************************************************
 * Function:       antenna_board_detect_exit
 * Description:    antenna board detect module exit
 * Parameters:   NULL
 * return value:  NULL
 * *********************************************************/
static void __exit antenna_board_detect_exit(void)
{
    platform_driver_unregister(&antenna_board_detect_driver);
}
late_initcall(antenna_board_detect_init);
module_exit(antenna_board_detect_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("huawei antenna board detect driver");
MODULE_AUTHOR("HUAWEI Inc");
