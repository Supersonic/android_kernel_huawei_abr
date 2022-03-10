

#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/of_gpio.h>
#include <linux/list.h>
#include <linux/pm_wakeup.h>
#include <linux/regulator/consumer.h>
#include <asm/atomic.h>
#include <asm/io.h>

#include "hall_report.h"
#include <huawei_platform/ap_hall/ext_hall_event.h>
#include <chipset_common/hwsensor/hall/hall_interface.h>

#define AK8789_FUN_RET_FAIL  (-1)
#define AK8789_DISABLE         0
#define AK8789_ENABLE          1

#define AK8789_BUF_LEN         10
#define MAX_NAME_LEN           50

#define HALL_DEFAULT_INT_DELAY 100

/* dts name */
#define HALL_SINGLE_N_POLE "huawei,single-n-pole"
#define HALL_SINGLE_S_POLE "huawei,single-s-pole"
#define HALL_ID            "huawei,id"
#define HALL_TYPE          "huawei,type"
#define HALL_WAKEUP_FLAG   "huawei,wakeup-flag"
#define HALL_DEFAULT_STATE "huawei,default-state"
#define HALL_INT_DELAY     "hall_int_delay"
#define EXT_HALL_TYPE_T    "ext_hall_type"
#define EXT_HALL_COMP_T    "ext_hall_data_comp_type"
#define EXT_HALL_BOOT_NOTIFY_T    "ext_hall_boot_up_notify"

/* hall pole */
#define SINGLE_N_POLE 0x01
#define SINGLE_S_POLE 0x02

enum ext_data_comp_t {
	EXT_DEFAULT,      // use default
	EXT_N_HALL0_FOLD, // use hall1 S for fold
};

/* log level */
#define AK8789_LOG_FLOW 2
#define AK8789_LOG_INFO 1
#define AK8789_LOG_WARN 0
#define AK8789_LOG_ERR  0

static int ak8789_debug_mask = AK8789_LOG_FLOW;
module_param_named(ak8789_debug, ak8789_debug_mask, int, 0664);

#define AK8789_FLOWMSG(format, args...) \
do { \
	if (ak8789_debug_mask >= AK8789_LOG_FLOW) \
		printk(KERN_ERR "ap_hall %s " format, __FUNCTION__, ##args); \
} while (0)

#define AK8789_INFOMSG(format, args...) \
do { \
	if (ak8789_debug_mask >= AK8789_LOG_INFO) \
		printk(KERN_ERR "ap_hall %s " format, __FUNCTION__, ##args); \
} while (0)

#define AK8789_WARNMSG(format, args...) \
do { \
	if (ak8789_debug_mask >= AK8789_LOG_WARN) \
		printk(KERN_ERR "ap_hall %s " format, __FUNCTION__, ##args); \
} while (0)

#define AK8789_ERRMSG(format, args...) \
do { \
	if (ak8789_debug_mask >= AK8789_LOG_ERR) \
		printk(KERN_ERR "ap_hall %s " format, __FUNCTION__, ##args); \
} while (0)

typedef struct gpio_data {
	char name[MAX_NAME_LEN];
	int gpio;
	int irq;
	unsigned int hall_value;
	unsigned int hall_wakeup_flag;
	struct list_head list;
} gpio_data_t;

struct hall_dev {
	struct regulator *vdd_ldo;
	struct pinctrl *pinctrl;
	struct pinctrl_state *pin_default;
	struct list_head head;
	struct workqueue_struct *hall_wq;
	struct delayed_work hall_work;
	struct wakeup_source *wake_lock;
	unsigned int hall_int_delay;
	unsigned int ext_hall_type;
	unsigned int data_comp_type;
	unsigned int ext_hall_boot_notify;
};

static atomic_t hall_enable_status = ATOMIC_INIT(0);
static atomic_t ext_hall_enable_status = ATOMIC_INIT(0);
static atomic_t irq_no_at = ATOMIC_INIT(0);

static struct hall_dev hw_hall_dev = {
	.vdd_ldo = NULL,
	.pinctrl = NULL,
	.pin_default = NULL,
	.hall_int_delay = HALL_DEFAULT_INT_DELAY,
	.ext_hall_type = 0,
	.data_comp_type = 0,
	.ext_hall_boot_notify = 0,
};

/***************************************************************
Function: query_hall_event
Description: request the state of hall gpios,if four gpios state are
    low-low-high-high,than the value will be 1100
Parameters:void
Return:value of state of hall gpios
***************************************************************/
static unsigned int query_hall_event(void)
{
	int ret;
	unsigned int value = 0;
	gpio_data_t *gpio_ptr = NULL;

	list_for_each_entry(gpio_ptr, &hw_hall_dev.head, list) {
		ret = gpio_get_value(gpio_ptr->gpio);
		if (!ret)
			value |= gpio_ptr->hall_value;
		else
			value &= ~gpio_ptr->hall_value;

		AK8789_INFOMSG("gpio=%d, ret=%d\n", gpio_ptr->gpio, ret);
	}

	return value;
}

static int ext_hall_data_comp_report(int temp)
{
	unsigned int val = 0;

	switch (hw_hall_dev.data_comp_type) {
	case EXT_N_HALL0_FOLD:
		val = !(((unsigned int)temp >> 0) & 0x01);
		// for hall 0 N type, only use bit0
		break;
	default:
		val = temp;
		break;
	}
	return (int)val;
}

static ssize_t ak8789_store_enable_hall_sensor(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned int val = 0;
	int rc;

	if (!buf) {
		AK8789_FLOWMSG("buf is NULL\n");
		return count;
	}
	rc = kstrtouint(buf, 0, &val);
	if (rc < 0)
		return rc;

	AK8789_FLOWMSG("ap hall sensor ope = %u, some delay\n", val);
	if (val)
		atomic_set(&hall_enable_status, AK8789_ENABLE);
	else
		atomic_set(&hall_enable_status, AK8789_DISABLE);
	queue_delayed_work(system_power_efficient_wq, &hw_hall_dev.hall_work,
		msecs_to_jiffies(500));
	return count;
}

static ssize_t ak8789_show_enable_hall_sensor(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	if (buf == NULL) {
		AK8789_FLOWMSG("buf is NULL\n");
		return AK8789_FUN_RET_FAIL;
	}

	return snprintf(buf, PAGE_SIZE, "%d\n", atomic_read(&hall_enable_status));
}
static DEVICE_ATTR(enable_hall_sensor, S_IWUSR | S_IRUSR | S_IRUGO,
	ak8789_show_enable_hall_sensor, ak8789_store_enable_hall_sensor);

static ssize_t ak8789_store_enable_ext_hall_sensor(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned int val = 0;
	int rc;

	if (!buf) {
		AK8789_FLOWMSG("buf is NULL\n");
		return count;
	}
	rc = kstrtouint(buf, 0, &val);
	if (rc < 0)
		return rc;

	AK8789_FLOWMSG("ap ext hall sensor ope = %u, some delay\n", val);
	if (val)
		atomic_set(&ext_hall_enable_status, AK8789_ENABLE);
	else
		atomic_set(&ext_hall_enable_status, AK8789_DISABLE);
	queue_delayed_work(system_power_efficient_wq, &hw_hall_dev.hall_work,
		msecs_to_jiffies(500));
	return count;
}

static ssize_t ak8789_show_enable_ext_hall_sensor(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	if (buf == NULL) {
		AK8789_FLOWMSG("buf is NULL\n");
		return AK8789_FUN_RET_FAIL;
	}

	return snprintf(buf, PAGE_SIZE, "%d\n",
		atomic_read(&ext_hall_enable_status));
}
static DEVICE_ATTR(enable_ext_hall_sensor, S_IWUSR | S_IRUSR | S_IRUGO,
	ak8789_show_enable_ext_hall_sensor, ak8789_store_enable_ext_hall_sensor);

static ssize_t ak8789_show_query_ext_hall_sensor(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	if (!buf) {
		AK8789_FLOWMSG("buf is NULL\n");
		return AK8789_FUN_RET_FAIL;
	}

	return snprintf(buf, PAGE_SIZE, "%d\n", hw_hall_dev.ext_hall_type);
}
static DEVICE_ATTR(query_ext_hall, S_IWUSR | S_IRUSR | S_IRUGO, ak8789_show_query_ext_hall_sensor, NULL);

static ssize_t ak8789_show_irq_count(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	if (buf == NULL) {
		AK8789_FLOWMSG("buf is NULL\n");
		return AK8789_FUN_RET_FAIL;
	}

	return snprintf(buf, PAGE_SIZE, "%d\n", atomic_read(&irq_no_at));
}
static DEVICE_ATTR(irq_count, S_IWUSR | S_IRUSR | S_IRUGO, ak8789_show_irq_count, NULL);

static ssize_t ak8789_show_get_hall_status(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	int value;

	if (buf == NULL) {
		AK8789_FLOWMSG("buf is NULL\n");
		return AK8789_FUN_RET_FAIL;
	}

	value = query_hall_event();
	hall_report_value(value);

	return snprintf(buf, PAGE_SIZE, "%d\n", value);
}
static DEVICE_ATTR(get_hall_status, S_IWUSR | S_IRUSR | S_IRUGO,
	ak8789_show_get_hall_status, NULL);

static struct attribute *ak8789_attributes[] = {
	&dev_attr_enable_hall_sensor.attr,
	&dev_attr_get_hall_status.attr,
	&dev_attr_irq_count.attr,
	&dev_attr_query_ext_hall.attr,
	&dev_attr_enable_ext_hall_sensor.attr,
	NULL
};

static const struct attribute_group ak8789_attr_group = {
	.attrs = ak8789_attributes,
};

static void hall_work_func(struct work_struct *work)
{
	int value;

	value = query_hall_event();
	if (atomic_read(&hall_enable_status))
		hall_report_value(value);

	usleep_range(1000, 1500);
	if (atomic_read(&ext_hall_enable_status))
		ext_hall_report_value(hw_hall_dev.ext_hall_type,
			ext_hall_data_comp_report(value));
	ext_hall_notify_event(hw_hall_dev.ext_hall_type, value);
	atomic_dec(&irq_no_at);
}

#ifdef CONFIG_HUAWEI_HALL_INTERFACE
static bool ak8789_lightstrap_get_hall_status(void)
{
	int value;
	uint32_t light_status;
	uint32_t hall_lightstrap_mask = get_hall_lightstrap_value();

	value = query_hall_event();
	light_status = value & hall_lightstrap_mask;

	AK8789_INFOMSG("%s: hall value is %d, mask is %d, lightstrap_status is %d\n",
		 __func__, value, light_status, hall_lightstrap_mask);

	if (!light_status)
		return false;

	return true;
}

static struct hall_interface_ops ak8789_ops = {
	.get_hall_status = ak8789_lightstrap_get_hall_status,
};
#endif

/* interrupts handle function */
irqreturn_t hall_event_isr(int irq, void *dev)
{
	int ret;
	unsigned int trigger;
	struct hall_dev *data = NULL;
	struct irq_desc *desc = NULL;

	AK8789_FLOWMSG("irq=%d\n", irq);

	data = (struct hall_dev *)dev;
	desc = irq_to_desc(irq);
	if ((data == NULL) || (desc == NULL)) {
		AK8789_ERRMSG("dev or desc is NULL\n");
		return IRQ_NONE;
	}

	trigger = desc->irq_data.common->state_use_accessors & IRQD_TRIGGER_MASK;
	if (trigger & IRQF_TRIGGER_LOW) {
		ret = irq_set_irq_type(irq, IRQF_TRIGGER_HIGH);
	} else if (trigger & IRQF_TRIGGER_HIGH) {
		ret = irq_set_irq_type(irq, IRQF_TRIGGER_LOW);
	} else {
		AK8789_ERRMSG("hall trigger not level type, error\n");
		return IRQ_NONE;
	}

	if (ret) {
		AK8789_ERRMSG("hall irq_set_irq_type error %s\n", desc->name);
		return IRQ_NONE;
	}

	__pm_wakeup_event(data->wake_lock, jiffies_to_msecs(HZ));
	queue_delayed_work(data->hall_wq, &data->hall_work, msecs_to_jiffies(data->hall_int_delay));

	/* interrupts counter increases 1 */
	atomic_inc(&irq_no_at);

	return IRQ_HANDLED;
}

static int hall_gpio_irq_setup(struct device *dev)
{
	int ret;
	int value;
	int irq;
	unsigned long irq_flags;
	gpio_data_t *gpio_ptr = NULL;

	if (dev == NULL) {
		AK8789_ERRMSG("dev is NULL\n");
		return AK8789_FUN_RET_FAIL;
	}

	list_for_each_entry(gpio_ptr, &hw_hall_dev.head, list) {
		value = gpio_get_value(gpio_ptr->gpio);
		/* if current gpio is high, set low as irq, otherwise set high */
		irq_flags = value ? IRQF_TRIGGER_LOW : IRQF_TRIGGER_HIGH;
		irq = gpio_to_irq(gpio_ptr->gpio);
		ret = devm_request_irq(dev, irq, hall_event_isr, irq_flags, gpio_ptr->name, &hw_hall_dev);
		enable_irq_wake(irq);
		if (ret) {
			AK8789_ERRMSG("request_irq fail %s %d\n", gpio_ptr->name, ret);
		}
		gpio_ptr->irq = irq;
		AK8789_INFOMSG("irq = %d, irq_flags = 0x%x\n", irq, irq_flags);
	}

	return 0;
}

static int get_gpio_config(
	struct device_node *node,
	gpio_data_t *gpio_ptr,
	char *gpio_name)
{
	int ret;
	int gpio;

	if ((node == NULL) || (gpio_ptr == NULL)) {
		AK8789_ERRMSG("node or gpio_ptr is NULL\n");
		return AK8789_FUN_RET_FAIL;
	}

	gpio = of_get_named_gpio(node, gpio_name, 0);
	if (!gpio_is_valid(gpio)) {
		AK8789_ERRMSG("get gpio error %s\n", gpio_name);
		return AK8789_FUN_RET_FAIL;
	}
	ret = gpio_request(gpio, gpio_name);
	if (ret) {
		AK8789_ERRMSG("requset gpio %d err %d\n", gpio, ret);
		return ret;
	}
	ret = gpio_direction_input(gpio);
	if (ret) {
		AK8789_ERRMSG("gpio %d direction err %d\n", gpio, ret);
		return ret;
	}

	gpio_ptr->gpio = gpio;
	AK8789_FLOWMSG("get gpio = %d\n", gpio_ptr->gpio);

	return ret;
}

static void hall_sensor_parse_dts(struct device_node *node)
{
	if (of_property_read_u32(node, HALL_INT_DELAY,
		&hw_hall_dev.hall_int_delay)) {
		AK8789_ERRMSG("read hall int delay fail, use default\n", hw_hall_dev.hall_int_delay);
		hw_hall_dev.hall_int_delay = HALL_DEFAULT_INT_DELAY;
	}
}

static void ext_hall_sensor_parse_dts(struct device_node *node)
{
	if (of_property_read_u32(node, EXT_HALL_TYPE_T,
		&hw_hall_dev.ext_hall_type)) {
		AK8789_ERRMSG("read ext hall cfg fail, do not support\n");
		hw_hall_dev.ext_hall_type = 0;
		return;
	}
	if (of_property_read_u32(node, EXT_HALL_BOOT_NOTIFY_T,
		&hw_hall_dev.ext_hall_boot_notify)) {
		AK8789_ERRMSG("read ext ext_hall_boot_notify fail, use default\n");
		hw_hall_dev.ext_hall_boot_notify = 0;
	}
	if (of_property_read_u32(node, EXT_HALL_COMP_T,
		&hw_hall_dev.data_comp_type)) {
		AK8789_ERRMSG("read ext hall comp fail, use default\n");
		hw_hall_dev.data_comp_type = 0;
		return;
	}
}

static int hall_parse_dts(struct device *dev)
{
	int ret;
	unsigned int hall_id = 0;
	unsigned int hall_type = 0;
	unsigned int hall_wakeup_flag = 0;
	const char *state = NULL;
	struct device_node *temp = NULL;
	gpio_data_t *gpio_ptr = NULL;

	if ((dev == NULL) || (dev->of_node == NULL)) {
		AK8789_ERRMSG("dev or of_node is NULL\n");
		return AK8789_FUN_RET_FAIL;
	}
	hall_sensor_parse_dts(dev->of_node);
	ext_hall_sensor_parse_dts(dev->of_node);
	get_hall_fature_config(dev->of_node);
	INIT_LIST_HEAD(&hw_hall_dev.head);
	for_each_child_of_node(dev->of_node, temp) {
		ret = of_property_read_string(temp, HALL_DEFAULT_STATE, &state);
		if (ret || strncmp(state, "on", sizeof("on"))) {
			AK8789_INFOMSG("node %s is disabled\n", temp->name);
			continue;
		}

		if (of_property_read_u32(temp, HALL_ID, &hall_id)) {
			AK8789_ERRMSG("read hall_id fail\n");
			return AK8789_FUN_RET_FAIL;
		}
		if (of_property_read_u32(temp, HALL_TYPE, &hall_type)) {
			AK8789_ERRMSG("read hall_type fail\n");
			return AK8789_FUN_RET_FAIL;
		}
		if (of_property_read_u32(temp, HALL_WAKEUP_FLAG, &hall_wakeup_flag)) {
			AK8789_ERRMSG("read hall_wakeup_flag fail\n");
			return AK8789_FUN_RET_FAIL;
		}

		if (hall_type & SINGLE_N_POLE) {
			gpio_ptr = devm_kzalloc(dev, sizeof(*gpio_ptr), GFP_KERNEL);
			ret = get_gpio_config(temp, gpio_ptr, HALL_SINGLE_N_POLE);
			if (ret)
				return ret;

			gpio_ptr->hall_wakeup_flag = hall_wakeup_flag;
			gpio_ptr->hall_value = 1 << (hall_id * 2);
			snprintf(gpio_ptr->name, MAX_NAME_LEN, "hall%d_north", hall_id);
			list_add_tail(&gpio_ptr->list, &hw_hall_dev.head);
		}

		if (hall_type & SINGLE_S_POLE) {
			gpio_ptr = devm_kzalloc(dev, sizeof(*gpio_ptr), GFP_KERNEL);
			ret = get_gpio_config(temp, gpio_ptr, HALL_SINGLE_S_POLE);
			if (ret)
				return ret;

			gpio_ptr->hall_wakeup_flag = hall_wakeup_flag;
			gpio_ptr->hall_value = 1 << (hall_id * 2 + 1);
			snprintf(gpio_ptr->name, MAX_NAME_LEN, "hall%d_south", hall_id);
			list_add_tail(&gpio_ptr->list, &hw_hall_dev.head);
		}
	}

	return 0;
}

static int pinctrl_init(struct device *dev)
{
	int ret;

	if (dev == NULL) {
		AK8789_ERRMSG("dev is NULL\n");
		return AK8789_FUN_RET_FAIL;
	}

	hw_hall_dev.vdd_ldo = regulator_get(dev, "vdd");
	if (IS_ERR(hw_hall_dev.vdd_ldo)) {
		AK8789_ERRMSG("regulator get errer\n");
	} else {
		ret = regulator_enable(hw_hall_dev.vdd_ldo);
		if (ret)
			AK8789_ERRMSG("regulator enable errer, ret = %d\n", ret);
	}

	hw_hall_dev.pinctrl = devm_pinctrl_get(dev);
	if (IS_ERR_OR_NULL(hw_hall_dev.pinctrl)) {
		AK8789_ERRMSG("ak8789 error:devm_pinctrl_get wrong\n");
		return AK8789_FUN_RET_FAIL;
	}

	hw_hall_dev.pin_default = pinctrl_lookup_state(hw_hall_dev.pinctrl, "default");
	if (IS_ERR_OR_NULL(hw_hall_dev.pin_default)) {
		AK8789_ERRMSG("ak8789 error:pinctrl_lookup_state wrong\n");
		return AK8789_FUN_RET_FAIL;
	}

	ret = pinctrl_select_state(hw_hall_dev.pinctrl, hw_hall_dev.pin_default);
	if (ret)
		AK8789_ERRMSG("ak8789 error:pinctrl_select_state wrong\n");

	return ret;
}

static struct delayed_work hall_status_self_check_work;
static void ext_fold_delayed_work(struct work_struct *work)
{
	int hall_status;

	(void)work;
	if (!hw_hall_dev.ext_hall_boot_notify)
		return;
	hall_status = query_hall_event();
	ext_hall_notify_event(hw_hall_dev.ext_hall_type, hall_status);
	AK8789_INFOMSG("update ext_fold_delayed_work");
}

static int hall_probe(struct platform_device *pdev)
{
	int err;

	AK8789_INFOMSG("ak8789 probe enter\n");
	if (pdev == NULL) {
		AK8789_ERRMSG("pdev is NULL\n");
		return -ENODEV;
	}

	err = hall_parse_dts(&pdev->dev);
	if (err) {
		AK8789_ERRMSG("parse dts error %d\n", err);
		return err;
	}

	err = pinctrl_init(&pdev->dev);
	if (err) {
		AK8789_ERRMSG("pinctrl init error %d\n", err);
		return err;
	}

#ifdef INPUT_HALL_DATA_REPORT
	err = hall_report_register(&pdev->dev);
	if (err) {
		AK8789_ERRMSG("report register error %d\n", err);
		return err;
	}
#endif
	hw_hall_dev.hall_wq = create_singlethread_workqueue("hall_wq");
	if (IS_ERR_OR_NULL(hw_hall_dev.hall_wq)) {
		err = PTR_ERR(hw_hall_dev.hall_wq);
		AK8789_ERRMSG("wq create error %ld\n", err);
		goto workqueue_fail;
	}
	INIT_DELAYED_WORK(&hw_hall_dev.hall_work, hall_work_func);
	hw_hall_dev.wake_lock = wakeup_source_register(NULL, "ak8789");

	err = hall_gpio_irq_setup(&pdev->dev);
	if (err) {
		AK8789_ERRMSG("hall gpio and irq setup error %d\n", err);
		goto irq_setup_fail;
	}

	atomic_set(&hall_enable_status, AK8789_DISABLE);
	atomic_set(&ext_hall_enable_status, AK8789_DISABLE);

	err = sysfs_create_group(&pdev->dev.kobj, &ak8789_attr_group);
	if (err) {
		AK8789_ERRMSG("sysfs create error %d\n", err);
		goto sysfs_create_fail;
	}

#ifdef CONFIG_HUAWEI_HALL_INTERFACE
	hall_interface_ops_register(&ak8789_ops);
	AK8789_INFOMSG("hall_interface_ops_register\n");
#endif
	ext_fold_nb_init();
	INIT_DELAYED_WORK(&hall_status_self_check_work, ext_fold_delayed_work);
	schedule_delayed_work(&hall_status_self_check_work, msecs_to_jiffies(3000));
	AK8789_WARNMSG("ak8789 probe successfully\n");

	return err;

sysfs_create_fail:
	atomic_set(&hall_enable_status, AK8789_DISABLE);
	atomic_set(&ext_hall_enable_status, AK8789_DISABLE);

irq_setup_fail:
	wakeup_source_unregister(hw_hall_dev.wake_lock);
	destroy_workqueue(hw_hall_dev.hall_wq);

workqueue_fail:
#ifdef INPUT_HALL_DATA_REPORT
	hall_report_unregister();
#endif

	return err;
}

static int hall_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &ak8789_attr_group);
	atomic_set(&hall_enable_status, AK8789_DISABLE);
	wakeup_source_unregister(hw_hall_dev.wake_lock);
	destroy_workqueue(hw_hall_dev.hall_wq);

#ifdef INPUT_HALL_DATA_REPORT
	hall_report_unregister();
#endif

	return 0;
}

static const struct of_device_id ak8789_match_table[] = {
	{ .compatible = "huawei,hall-ak8789", },
	{ },
};
MODULE_DEVICE_TABLE(of, ak8789_match_table);

struct platform_driver hall_drv_pf = {
	.probe = hall_probe,
	.remove = hall_remove,
	.driver = {
		.name = "hall_platform",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(ak8789_match_table),
	},
};

module_platform_driver(hall_drv_pf);

MODULE_AUTHOR("huawei");
MODULE_DESCRIPTION("ak8789 hall");
MODULE_LICENSE("GPL v2");
