#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/delay.h>
#include <linux/of_address.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>
#include <linux/of_gpio.h>
#include "hw_otg_id.h"
#include <linux/power/huawei_adc.h>
#include <linux/version.h>
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
#include <linux/wakelock.h>
#endif


#define VBUS_IS_CONNECTED           0
#define DISABLE_USB_IRQ             0
#define FAIL                        (-1)
#define SAMPLING_TIME_OPTIMIZE_FLAG 1
#define SAMPLING_TIME_INTERVAL      10
#define SMAPLING_TIME_OPTIMIZE      5
#define VBATT_AVR_MAX_COUNT         10
#define ADC_VOLTAGE_LIMIT           150 /* mV */
#define ADC_VOLTAGE_MAX             1250 /* mV */
#define ADC_VOLTAGE_NEGATIVE        2000 /* mV */
#define USB_CHARGER_INSERTED        1
#define USB_CHARGER_REMOVE          0

static bool initialized = false;
static irqreturn_t hw_otg_id_irq_handle(int irq, void *dev_id);

/*************************************************************************************************
*  Function:       hw_is_usb_cable_connected
*  Discription:    Check whether Vbus has been exist.
*  Parameters:   void
*  return value:  zero:     Vbus isn't exist.
*                      no zero:  Vbus is exist.
**************************************************************************************************/
static int hw_is_usb_cable_connected(struct otg_gpio_id_dev *dev)
{
	int ret = 0;
	union power_supply_propval val = {0, };

	if (!dev) {
		hw_usb_err("%s: invalid param, fatal error\n", __func__);
		return 0;
	}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 4, 0))
		ret = dev->usb_psy->get_property(dev->usb_psy, POWER_SUPPLY_PROP_PRESENT, &val);
#else
		ret = dev->usb_psy->desc->get_property(dev->usb_psy, POWER_SUPPLY_PROP_PRESENT, &val);
#endif

	if (ret) {
		hw_usb_err("%s ret is %d\n", __func__, ret);
		return 0;
	}

	return val.intval;
}

/*************************************************************************************************
*  Function:       hw_otg_id_adc_sampling
*  Description:    get the adc sampling value.
*  Parameters:   otg_gpio_id_dev_p: otg device ptr
*  return value:  -1: get adc channel fail
*                      other value: the adc value.
**************************************************************************************************/
static int hw_otg_id_adc_sampling(struct otg_gpio_id_dev *otg_dev)
{
	int avgvalue;
	int vol_value;
	int sum = 0;
	int i;
	int sample_cnt = 0;

	if (!otg_dev) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	for (i = 0; i < VBATT_AVR_MAX_COUNT; i++) {
		vol_value = get_adc_converse_voltage(otg_dev->vadc_node, otg_dev->switch_mode);
		/* add some interval */
		msleep(SAMPLING_TIME_INTERVAL);
		if (vol_value < 0) {
			hw_usb_err("%s The value from ADC is error", __func__);
			continue;
		}
		sum += vol_value;
		sample_cnt++;
	}

	if (sample_cnt == 0)
		/* ESD cause the sample is always negative */
		avgvalue = ADC_VOLTAGE_NEGATIVE;
	else
		avgvalue = sum / sample_cnt;
	hw_usb_err("%s The average voltage of ADC is %d\n", __func__, avgvalue);

	return avgvalue;
}


/*************************************************************************************************
*  Function:       hw_otg_id_intb_work
*  Discription:    Handle GPIO about OTG.
*  Parameters:   work: The work struct of otg.
*  return value:  void
**************************************************************************************************/
static void hw_otg_id_intb_work(struct work_struct *work)
{
	int gpio_value;
	int avgvalue;
	static bool is_otg_has_inserted = false;
	union power_supply_propval val = {0, };
	struct otg_gpio_id_dev *dev = NULL;

	if (!work) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return;
	}

	dev = container_of(work, struct otg_gpio_id_dev, otg_intb_work);
	if (!dev) {
		pr_err("%s: Cannot get gpio id dev, fatal error\n", __func__);
		return;
	}

	/* Fix the different of schager V200 and V300 */
	if (!is_otg_has_inserted) {
		if (hw_is_usb_cable_connected(dev)) {
			hw_usb_err("%s Vbus is inerted\n", __func__);
			return;
		}
	}

	gpio_value = gpio_get_value(dev->gpio);
	if (gpio_value == 0) {
		hw_usb_err("%s Send ID_FALL_EVENT\n", __func__);
		avgvalue = hw_otg_id_adc_sampling(dev);

		if ((avgvalue >= 0) && (avgvalue <= ADC_VOLTAGE_LIMIT)) {
			is_otg_has_inserted = true;
		} else {
			hw_usb_err("%s avgvalue is %d\n", __func__, avgvalue);
			is_otg_has_inserted = false;
		}
	} else {
		hw_usb_err("%s Send ID_RISE_EVENT\n", __func__);
		is_otg_has_inserted = false;
	}

	val.intval = is_otg_has_inserted;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 4, 0))
	dev->usb_psy->set_property(dev->usb_psy, POWER_SUPPLY_PROP_USB_OTG, &val);
#else
	smb_notify_to_host(is_otg_has_inserted);
#endif

	return;
}

static void hw_otg_init_work(struct work_struct *work)
{
	struct otg_gpio_id_dev *dev = NULL;
	int ret;
	int avgvalue;

	if (!work) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return;
	}

	dev = container_of(work, struct otg_gpio_id_dev, init_work.work);
	if (!dev) {
		pr_err("%s: Cannot get gpio id dev, fatal error\n", __func__);
		return;
	}

	avgvalue = hw_otg_id_adc_sampling(dev);
	if ((avgvalue > ADC_VOLTAGE_LIMIT) && (avgvalue <= ADC_VOLTAGE_MAX)) {
		hw_usb_err("%s Set gpio_direction_output, avgvalue is %d\n",
			__func__, avgvalue);
		ret = gpio_direction_output(dev->gpio, 1);
		return;
	}

	ret = gpio_direction_input(dev->gpio);
	if (ret < 0) {
		hw_usb_err("%s gpio_direction_input error!!! ret=%d. gpio=%d\n",
			__func__, ret, dev->gpio);
		return;
	}

	initialized = true;

	ret = gpio_get_value(dev->gpio);
	if (!ret)
		schedule_work(&dev->otg_intb_work);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 9, 0))
	ret = request_irq(dev->irq, hw_otg_id_irq_handle,
		IRQF_TRIGGER_LOW |  IRQF_NO_SUSPEND | IRQF_ONESHOT,
		"otg_gpio_irq", dev);
#else
	ret = request_irq(dev->irq, hw_otg_id_irq_handle,
		IRQF_TRIGGER_LOW | IRQF_TRIGGER_HIGH | IRQF_NO_SUSPEND | IRQF_ONESHOT,
		"otg_gpio_irq", dev);
#endif
	if (ret < 0) {
		hw_usb_err("%s request otg irq handle funtion fail!! ret:%d\n", __func__, ret);
		return;
	}
}

/*************************************************************************************************
*  Function:       hw_otg_id_irq_handle
*  Discription:    Handle the GPIO200 irq.
*  Parameters:   irq: The number of irq for GPIO200
*                dev_id: The id of devices.
*  return value:  IRQ_HANDLED
**************************************************************************************************/
static irqreturn_t hw_otg_id_irq_handle(int irq, void *dev_id)
{
	struct otg_gpio_id_dev *dev = dev_id;
	int gpio_value;

	if (!dev) {
		hw_usb_err("%s: invalid param, fatal error\n", __func__);
		return IRQ_HANDLED;
	}

	if (!initialized) {
		hw_usb_err("%s: bq2415x not init finish, waiting\n", __func__);
		return IRQ_HANDLED;
	}

	disable_irq_nosync(dev->irq);
	gpio_value = gpio_get_value(dev->gpio);
	if (gpio_value == 0)
		irq_set_irq_type(dev->irq, IRQF_TRIGGER_HIGH | IRQF_NO_SUSPEND);
	else
		irq_set_irq_type(dev->irq, IRQF_TRIGGER_LOW | IRQF_NO_SUSPEND);

	enable_irq(dev->irq);

	schedule_work(&dev->otg_intb_work);
	return IRQ_HANDLED;
}

/*************************************************************************************************
*  Function:       hw_otg_parse_dt
*  Discription:    parse dts for otg_gpio_id
*  Parameters:     dev: otg_gpio_id_dev structure
*  return value:   0 -- parse success, <0 -- parse fail
**************************************************************************************************/
static int hw_otg_parse_dt(struct platform_device *pdev, struct otg_gpio_id_dev *dev)
{
	int ret;
	struct device_node *np = NULL;

	if (!pdev || !dev) {
		hw_usb_err("%s: invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	np = pdev->dev.of_node;
	if (!np) {
		hw_usb_err("%s: Cannot get of node, fatal error\n", __func__);
		return -EINVAL;
	}

	dev->vadc_node = of_parse_phandle(np, "otg-vadc", 0);
	if (!dev->vadc_node) {
		dev_err(&pdev->dev, "huawei adc not init yet, defer\n");
		ret = -EPROBE_DEFER;
		goto err_parse_dt;
	}

	ret = of_property_read_u32(np, "switch-mode", &(dev->switch_mode));
	if (ret) {
		hw_usb_err("%s Get gpio switch mode failed\n", __func__);
		goto err_parse_dt;
	}

	dev->gpio = of_get_named_gpio(np, "otg-gpio", 0);
	if (dev->gpio < 0) {
		hw_usb_err("%s of_get_named_gpio error!!! gpio=%d\n", __func__, dev->gpio);
		goto err_parse_dt;
	}

	pr_info("%s: parse dt success\n", __func__);
	return 0;

err_parse_dt:
	pr_info("%s: parse dt fail, ret = %d\n", __func__, ret);
	return ret;
}

/*************************************************************************************************
*  Function:       hw_otg_id_probe
*  Discription:    otg probe function.
*  Parameters:   pdev: The porinter of pdev
*  return value:  IRQ_HANDLED
**************************************************************************************************/
static int hw_otg_id_probe(struct platform_device *pdev)
{
	int ret;
	struct power_supply *usb_psy = NULL;
	struct otg_gpio_id_dev *otg_dev = NULL;

	hw_usb_err("Enter %s funtion\n", __func__);

	if (!pdev) {
		hw_usb_err("%s The pdev is NULL\n", __func__);
		return ret;
	}

	usb_psy = power_supply_get_by_name("usb");
	if (!usb_psy) {
		hw_usb_err("%s usb driver not present yet, defer\n", __func__);
		return -EPROBE_DEFER;
	}

	otg_dev = devm_kzalloc(&pdev->dev, sizeof(struct otg_gpio_id_dev), GFP_KERNEL);
	if (!otg_dev) {
		hw_usb_err("%s alloc otg_dev failed! not enough memory\n", __func__);
		return ret;
	}

	otg_dev->usb_psy = usb_psy;

	ret = hw_otg_parse_dt(pdev, otg_dev);
	if (ret < 0) {
		hw_usb_err("%s parse dts fail\n", __func__);
		goto ERR_PARSE_DT;
	}

	/* init otg intr handle work funtion */
	INIT_WORK(&otg_dev->otg_intb_work, hw_otg_id_intb_work);
	INIT_DELAYED_WORK(&otg_dev->init_work, hw_otg_init_work);

	/* * init otg gpio process */
	ret = gpio_request(otg_dev->gpio, "otg_gpio_irq");
	if (ret < 0) {
		hw_usb_err("%s gpio_request error!!! ret=%d. gpio=%d\n",
			__func__, ret, otg_dev->gpio);
		goto ERR_GPIO_REQUEST;
	}

	ret = gpio_direction_input(otg_dev->gpio);
	if (ret < 0) {
		hw_usb_err("%s gpio_direction_input error!!! ret=%d. gpio=%d\n",
			__func__, ret, otg_dev->gpio);
		goto ERR_GPIO_DIRECTION;
	}
	otg_dev->irq = gpio_to_irq(otg_dev->gpio);
	if (otg_dev->irq < 0) {
		hw_usb_err("%s gpio_to_irq error!!! dev_p->gpio=%d, dev_p->irq=%d\n",
			__func__, otg_dev->gpio, otg_dev->irq);
		goto ERR_GET_IRQ_NO;
	}
	hw_usb_err("%s otg irq is %d.\n", __func__, otg_dev->irq);

	platform_set_drvdata(pdev, otg_dev);

	/* schedule immediately to make sure the init status */
	schedule_delayed_work(&otg_dev->init_work, 0);
	pr_err("Exit [%s]-\n", __func__);

	return 0;

ERR_GPIO_DIRECTION:
ERR_GET_IRQ_NO:
	gpio_free(otg_dev->gpio);
ERR_GPIO_REQUEST:
ERR_PARSE_DT:
	devm_kfree(&pdev->dev, otg_dev);
	otg_dev = NULL;
	return ret;
}

/*************************************************************************************************
*  Function:       hw_otg_id_remove
*  Description:   otg remove
*  Parameters:   pdev:platform_device
*  return value:  0-sucess or others-fail
**************************************************************************************************/
static int hw_otg_id_remove(struct platform_device *pdev)
{
	struct otg_gpio_id_dev *otg_dev = NULL;

	if (!pdev) {
		hw_usb_err("%s otg_gpio_id_dev_p or pdev is NULL\n", __func__);
		return -ENODEV;
	}

	otg_dev = platform_get_drvdata(pdev);
	if (!otg_dev)
		hw_usb_err("%s: otg_gpio_id_dev is null\n", __func__);

	free_irq(otg_dev->irq, pdev);
	gpio_free(otg_dev->gpio);
	devm_kfree(&pdev->dev, otg_dev);
	return 0;
}


static struct of_device_id hw_otg_id_of_match[] = {
	{ .compatible = "huawei,usbotg-by-id", },
	{ },
};

static struct platform_driver hw_otg_id_drv = {
	.probe = hw_otg_id_probe,
	.remove = hw_otg_id_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "hw_otg_id",
		.of_match_table = hw_otg_id_of_match,
	},
};

static int __init hw_otg_id_init(void)
{
	int ret;

	ret = platform_driver_register(&hw_otg_id_drv);
	hw_usb_err("Enter [%s] function, ret is %d\n", __func__, ret);
	return ret;
}

static void __exit hw_otg_id_exit(void)
{
	platform_driver_unregister(&hw_otg_id_drv);
	return;
}

device_initcall_sync(hw_otg_id_init);
module_exit(hw_otg_id_exit);

MODULE_AUTHOR("huawei");
MODULE_DESCRIPTION("This module detect USB OTG connection/disconnection");
MODULE_LICENSE("GPL v2");
