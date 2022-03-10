
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>

#include "hw_device.h"

#define HW_DEVICES_CLASS_NAME "hw_devices"
static struct class *hw_device_class = NULL;
static struct device *hw_device_dev = NULL;
static struct hw_device_info *g_chip = NULL;
#define PON_REASON_REG      0x08
#define SMPL_MASK         BIT(1)
#define SMPL_SHIFT         1
#define SIZE               1

int check_power_on_reason(struct hw_device_info *chip)
{
    int rc = 0;
    u8 reg = 0;

    if((NULL == chip)||(NULL == chip->spmi->ctrl))
    {

	pr_err( "chip or ctrl is NULL\n");
        return -ENODEV;
    }

	rc = spmi_ext_register_readl(chip->spmi->ctrl, chip->spmi->sid,
	chip->base_addr + PON_REASON_REG, &reg, SIZE);

 	if (rc) {
        pr_err( "Unable to read PON_RESASON reg\n");
        return -ENODEV;
    }
	chip->pmic_status = (SMPL_MASK&reg) >> SMPL_SHIFT;
	return 0;

}

/*get pmic status*/

static ssize_t get_pmic_status(struct device *dev,
                       struct device_attribute *attr, char *buf)
{
    int ret =0;
    if((NULL == g_chip)||(NULL == dev) ||(NULL == attr))
    {

         pr_err( "g_chip is NULL\n");
         return -ENODEV;
    }

    ret = check_power_on_reason(g_chip);

    if(ret == -ENODEV)
    {
        return snprintf(buf, PAGE_SIZE,"%u\n", SIZE);
    }
    pr_info("chip->pmic_status :%d\n",g_chip->pmic_status);

    return snprintf(buf, PAGE_SIZE,"%u\n", g_chip->pmic_status);

}

static DEVICE_ATTR(check_pmic_status, S_IRUGO,
                        get_pmic_status,
                        NULL);
/*create the device attribute table,if want to add the test item,pls fill the table */
static struct device_attribute *hw_dev_attr_tbl[] =
{
    &dev_attr_check_pmic_status,
};

static int hw_device_probe(struct spmi_device *spmi)
{
    int ret = 0;
    struct hw_device_info *chip = NULL;
    struct resource *res = NULL;
    int len =0;
    int i = 0;
    int j = 0;

    if (NULL == spmi)
    {
		pr_err("%s: spmi err\n", __func__);
		return -ENODEV;
    }

    struct device_node *node = spmi->dev.of_node;
    if (NULL == node)
    {
		dev_err(&spmi->dev, "%s: device node missing\n", __func__);
		return -ENODEV;
    }

    chip = devm_kzalloc(&spmi->dev, sizeof(*chip), GFP_KERNEL);
    if (!chip)
    {
        pr_err("alloc chip failed\n");
        return -ENOMEM;
    }

    chip->dev = &spmi->dev;
    chip->spmi = spmi;
    res = spmi_get_resource(spmi, NULL, IORESOURCE_MEM, 0);
    if (!res)
    {
		dev_err(&spmi->dev, "%s: node is missing base address\n",
				__func__);
		return -EINVAL;
    }
    chip->base_addr = res->start;
    dev_err(&spmi->dev, "%s base_addr:%lu\n",
				__func__,chip->base_addr);
    chip->pmic_status = NORMAL_STATE;
    len = ARRAY_SIZE(hw_dev_attr_tbl);
    dev_set_drvdata(&spmi->dev, chip);
    g_chip = chip;
  /*create the hw device class*/
    if (NULL == hw_device_class)
    {
        hw_device_class = class_create(THIS_MODULE, HW_DEVICES_CLASS_NAME);
        if (NULL == hw_device_class)
        {
            pr_err("hw_device_class create fail");
            goto hw_device_probe_failed0;
        }
    }

    /*create the hw device dev*/
    if(hw_device_class)
    {
        if(hw_device_dev == NULL)
        {
            hw_device_dev = device_create(hw_device_class, NULL, 0, NULL,"hw_detect_device");
            if(IS_ERR(hw_device_dev))
            {
                pr_err("create hw_dev failed!\n");
                goto hw_device_probe_failed1;
            }
        }

        /*create the hw dev attr*/
        for(i=0;i<len;i++)
        {
           ret = sysfs_create_file(&hw_device_dev->kobj, &(hw_dev_attr_tbl[i]->attr));
           if(ret<0)
           {
               pr_err("fail to create pmic attr for %s\n", hw_device_dev->kobj.name);
               ++j;
               continue;
           }

        }
        if(len == j)
        {
           pr_err("fail to create all attr  %d\n",j);
           goto hw_device_probe_failed2;
        }
    }
    pr_info("huawei device  probe ok!\n");
    return 0;

hw_device_probe_failed2:
    device_destroy(hw_device_class,0);
    hw_device_dev = NULL;
hw_device_probe_failed1:
    class_destroy(hw_device_class);
    hw_device_class = NULL;
hw_device_probe_failed0:
    kfree(chip);
    chip = NULL;
    g_chip = NULL;
    return -1;
}

/*remove the hw device*/
static int hw_device_remove(struct spmi_device *spmi)
{
    struct hw_device_info *chip = NULL;
    if (!spmi) {
        pr_err("%s: invalid param, fatal error\n", __func__);
        return -EINVAL;
    }

    chip = dev_get_drvdata(&spmi->dev);
    if (NULL == chip) {
        pr_err("[%s]di is NULL!\n",__func__);
        return -ENODEV;
    }

    kfree(chip);
    chip = NULL;
    g_chip = NULL;
    return 0;
}

/*
 *probe match table
*/
static struct of_device_id hw_device_table[] = {
    {
        .compatible = "huawei,hw_device",
        .data = NULL,
    },
    {},
};

/*
 *hw device detect driver
 */
static struct spmi_driver hw_device_driver = {
    .probe = hw_device_probe,
    .remove = hw_device_remove,
    .driver = {
        .name = "huawei,hw_device",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(hw_device_table),
    },
};
/***************************************************************
 * Function: hw_device_init
 * Description: hw device detect module initialization
 * Parameters:  Null
 * return value: 0-sucess or others-fail
 * **************************************************************/
static int __init hw_device_init(void)
{
    return spmi_driver_register(&hw_device_driver);
}
/*******************************************************************
 * Function:       hw_device_exit
 * Description:    hw device module exit
 * Parameters:   NULL
 * return value:  NULL
 * *********************************************************/
static void __exit hw_device_exit(void)
{
    spmi_driver_unregister(&hw_device_driver);
}
module_init(hw_device_init);
module_exit(hw_device_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("huawei hw device driver");
MODULE_AUTHOR("HUAWEI Inc");
