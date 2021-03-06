/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    Copyright (C) 2011-2012  Huawei Corporation
*/

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/notifier.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/param.h>
#include <linux/termios.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/stat.h>
#include <net/bluetooth/bluetooth.h>
#include <asm/uaccess.h>
#include <linux/of.h>
#include <asm/uaccess.h>

#ifndef KERNEL_HWFLOW
#define KERNEL_HWFLOW true
#endif

#undef FMDBG
#define FMDBG(fmt, args...) \
	do \
	{ \
		  if (KERNEL_HWFLOW) \
			pr_info("bt_fm_feature: " fmt, ##args); \
	} while (0)

#undef FMDERR
#define FMDERR(fmt, args...) pr_err("bt_fm_feature: " fmt, ##args)

#define BT_DEVICE_DBG
#ifndef BT_DEVICE_DBG
#define BT_DBG(fmt, arg...)
#endif

#define LOG_TAG "Connectivity"

/*
 * Defines
 */
#define VERSION		"1.0"
#define PROC_DIR	"device_feature"
#define BT_FIRMWARE_UNKOWN	"Unkown"
#define BT_POWER_DEFAULT	"default"
#define GPIO_PULL_UP		1
#define GPIO_PULL_DOWN		0
#define FM_STATUS_CMD_LEN	2
#define DEFAULT_FM_LNA_NONEED	(-1)

struct proc_dir_entry *device_dir, *bt_dir, *fm_dir;

typedef enum {
        FM_SINR_5 = 5,
        FM_SINR_6 = 6,
        FM_SINR_7 = 7,
        FM_SINR_8 = 8,
        FM_SINR_MAX = 0xff
}fm_sinr_value;

typedef enum {
        FM_SINR_SAMPLES_9 = 9,
        FM_SINR_SAMPLES_10 = 10,
        FM_SINR_SAMPLES_MAX = 0xff
}fm_sinr_samples;

typedef enum {
        FM_OFF = 0,
        FM_ON = 1,
        FM_DEFAULT = 0xff
}fm_status_value;

static int g_sinr_threshold = FM_SINR_MAX;
static int g_sinr_samples = FM_SINR_SAMPLES_MAX;
static int g_lna_en_gpio = -1;
static int g_fm_status = -1;
static bool g_is_headset_on = false;

struct bt_device {
        char *chip_type;
        char *dev_name;
};
/* TODO: this definition should be removed and
 * the chipset info should be written in dts directly */
const struct bt_device bt_device_array[] = {
        { "BT_FM_QUALCOMM_WCN3660", "qcom_wcn3660" },
        { "BT_FM_QUALCOMM_WCN3620", "qcom_wcn3620" },
        { "BT_FM_QUALCOMM_WCN3680", "qcom_wcn3680" },
        { "BT_FM_QUALCOMM_WCN3660B", "qcom_wcn3660b" },
        { "BT_FM_QUALCOMM_WCN3610", "qcom_wcn3610" },
        { "BT_FM_QUALCOMM_WCN3615", "qcom_wcn3615" },
        { "BT_FM_QUALCOMM_WCN3950", "qcom_wcn3950" },
        { "BT_FM_QUALCOMM_QCA6147", "qcom_qca6174" },
        { "BT_FM_UNKNOWN_DEVICE", "Unknown" }
};
const void *get_bt_fm_device_type(void)
{
        int chip_type_len;
	struct device_node *dp = NULL;

	dp = of_find_node_by_path("/huawei_bt_info");
	if (!of_device_is_available(dp))
	{
		FMDERR("device is not available!\n");
		return NULL;
	}

	return  of_get_property(dp,"bt,chiptype", &chip_type_len);
}

static char *get_bt_fm_device_name(void)
{
	int i = 0;
	int arry_size = sizeof(bt_device_array)/sizeof(bt_device_array[0]);
	const char *bt_fm_chip_type = "BT_FM_UNKNOWN_DEVICE";

	/* get bt/fm chip type from the device feature configuration (.xml file) */
	bt_fm_chip_type = get_bt_fm_device_type();
	FMDBG("get_bt_fm_device_name bt_fm_chip_type:%s", bt_fm_chip_type);

	if (NULL == bt_fm_chip_type)
	{
		FMDERR("BT-FM, Get chip type fail.\n");
		return bt_device_array[arry_size - 1].dev_name;
	}

	/* lookup bt_device_model in bt_device_array[] */
	for (i = 0; i < arry_size; i++)
	{
		if (0 == strncmp(bt_fm_chip_type,bt_device_array[i].chip_type,strlen(bt_device_array[i].chip_type)))
		{
			break;
		}
	}
	if ( i == arry_size)
	{
		/* If we get invalid type name, return "Unknown".*/
		FMDERR("BT-FM, Get chip type fail.\n");
		return bt_device_array[arry_size - 1].dev_name;
	}

	return bt_device_array[i].dev_name;
}

static int featuretransfer_remove(struct platform_device *pdev)
{
	FMDBG("devicefeature removed.");
	return 0;
}

static struct platform_driver featuretransfer_driver = {
	.remove = featuretransfer_remove,
	.driver = {
		.name = "featuretransfer",
		.owner = THIS_MODULE,
	},
};

EXPORT_SYMBOL(get_bt_fm_device_name);

static int subchiptype_proc_show(struct seq_file *m, void *v)
{
	const char *bt_chiptype_name = NULL;

	bt_chiptype_name = get_bt_fm_device_name();
	FMDBG("%s enter chiptype:%s;\n", __func__, bt_chiptype_name);
	seq_printf(m,"%s",bt_chiptype_name);

	return 0;
}

static int subchiptype_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, subchiptype_proc_show, NULL);
}

static const struct file_operations subchiptype_proc_fops = {
	.owner		= THIS_MODULE,
	.open		= subchiptype_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int chiptype_proc_show(struct seq_file *m, void *v)
{
	const char bt_chiptype_name[] = "Qualcomm";

	seq_printf(m,"%s",bt_chiptype_name);

	return 0;
}

static int chiptype_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, chiptype_proc_show, NULL);
}

static const struct file_operations chiptype_proc_fops = {
	.owner		= THIS_MODULE,
	.open		= chiptype_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

const void *get_bt_fm_fw_ver(void)
{
	int bt_fm_fw_ver_len;
	struct device_node *dp = NULL;

	dp = of_find_node_by_path("/huawei_bt_info");
	if (!dp)
	{
		 FMDERR("device is not available!\n");
		 return NULL;
	}
	else
	{
		 FMDBG("%s:dp->name:%s,dp->full_name:%s;\n",__func__,dp->name,dp->full_name);
	}

	return  of_get_property(dp,"bt,fw_ver", &bt_fm_fw_ver_len);
}

EXPORT_SYMBOL(get_bt_fm_fw_ver);

static int bt_fm_ver_proc_show(struct seq_file *m, void *v)
{
	const char *bt_fm_fw_ver = NULL;

	bt_fm_fw_ver = get_bt_fm_fw_ver();
	if ( NULL != bt_fm_fw_ver )
	{
		FMDBG("%s enter wifi_device_type:%s;\n", __func__,bt_fm_fw_ver);
		seq_printf(m,"%s",bt_fm_fw_ver);
	}
	else
	{
		FMDERR("%s get bt_fm_fw_ver failed;\n", __func__);
	}

	return 0;
}

static int bt_fm_ver_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, bt_fm_ver_proc_show, NULL);
}

static const struct file_operations bt_fm_ver_proc_fops = {
	.owner	= THIS_MODULE,
	.open	= bt_fm_ver_proc_open,
	.read	= seq_read,
	.llseek	= seq_lseek,
	.release	= single_release,
};

const void *get_bt_power_level(void)
{
	int bt_power_level_len;
	struct device_node *dp = NULL;

	dp = of_find_node_by_path("/huawei_bt_info");
	if (!dp)
	{
		 FMDERR("device is not available!\n");
		 return NULL;
	}
	else
	{
		 FMDBG("%s:dp->name:%s,dp->full_name:%s;\n", __func__, dp->name, dp->full_name);
	}

	return  of_get_property(dp,"bt,power_level", &bt_power_level_len);
}

EXPORT_SYMBOL(get_bt_power_level);

static int bt_power_level_proc_show(struct seq_file *m, void *v)
{
	const char *bt_power_level = NULL;

	bt_power_level = get_bt_power_level();
	if (NULL != bt_power_level)
	{
		FMDBG("%s enter bt_power_level:%s;\n", __func__, bt_power_level);
		seq_printf(m, "%s", bt_power_level);
	}
	else
	{
		FMDERR("%s get bt_power_level failed;\n", __func__);
	}

	return 0;
}

static int bt_power_level_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, bt_power_level_proc_show, NULL);
}

static const struct file_operations bt_power_level_proc_fops = {
	.owner	= THIS_MODULE,
	.open	= bt_power_level_proc_open,
	.read	= seq_read,
	.llseek	= seq_lseek,
	.release	= single_release,
};

const void *get_bt_nv_path(void)
{
	int bt_nv_path_len;
	struct device_node *dp = NULL;

	dp = of_find_node_by_path("/huawei_bt_info");
	if (dp == NULL)
	{
		FMDERR("device is not available!\n");
		return NULL;
	}
	else
	{
		FMDBG("%s:dp->name:%s,dp->full_name:%s;\n", __func__, dp->name, dp->full_name);
	}

	return of_get_property(dp,"bt,nv_path", &bt_nv_path_len);
}

static int bt_nv_path_proc_show(struct seq_file *m, void *v)
{
	const char *bt_nv_path = NULL;

	bt_nv_path = get_bt_nv_path();
	if (bt_nv_path != NULL)
	{
		FMDBG("%s enter bt_nv_path:%s;\n", __func__, bt_nv_path);
		seq_printf(m, "%s", bt_nv_path);
	}
	else
	{
		FMDERR("%s get bt_nv_path failed;\n", __func__);
	}

	return 0;
}


static int bt_nv_path_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, bt_nv_path_proc_show, NULL);
}

static const struct file_operations bt_nv_path_proc_fops = {
	.owner	= THIS_MODULE,
	.open	= bt_nv_path_proc_open,
	.read	= seq_read,
	.llseek	= seq_lseek,
	.release	= single_release,
};

int get_fm_sinr_threshold_string(int *sinr)
{
	int sinr_threshold_len;
	struct device_node *dp = NULL;
	const char *psinr_threshold = NULL;
	int sinr_threshold;

	dp = of_find_node_by_path("/huawei_fm_info");
	if (!dp)
	{
		FMDERR("device is not available!\n");
		return -1;
	}

	psinr_threshold = of_get_property(dp,"fm,sinr_threshold", &sinr_threshold_len);
	if (NULL == psinr_threshold)
	{
		FMDERR("get sinr threshold value failed .\n");
		return -1;
	}
	sinr_threshold = simple_strtol(psinr_threshold,NULL,10);
	*sinr = sinr_threshold;

	return 0;
}

EXPORT_SYMBOL(get_fm_sinr_threshold_string);

static int fm_sinr_threshold_proc_show(struct seq_file *m, void *v)
{
	int sinr_threshold = FM_SINR_MAX;
	int ret ;

	FMDBG("fm_sinr_threshold_proc_show  enter\n");
	ret = get_fm_sinr_threshold_string(&sinr_threshold);
	if (-1 == ret)
	{
		// 7 is the default value
		FMDERR("Get FM Sinr Threshold failed and will use default value 7.\n");
		sinr_threshold = FM_SINR_7;
	}

	FMDBG("fm_sinr_threshold_proc_show g_sinr_threshold = %d .\n",g_sinr_threshold);
	/* if we changed the sinr value, get the new value. used for debug */
	if (FM_SINR_MAX != g_sinr_threshold)
	{
		sinr_threshold = g_sinr_threshold;
	}

	FMDBG("fm_sinr_threshold_proc_show ,g_sinr_threshold = %d, sinr_threshold:%d\n", g_sinr_threshold, sinr_threshold);

	seq_printf(m, "%d", sinr_threshold);

	return 0;
}

static int fm_sinr_threshold_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fm_sinr_threshold_proc_show, NULL);
}

static ssize_t fm_sinr_threshold_write(struct file *filp, const char __user *userbuf,
						size_t count, loff_t *ppos)
{
	char sinr_threshold[10] = { 0 };

	FMDBG("fm_sinr_threshold_write.  count:%d\n", (int)count);

	if (NULL == userbuf )
	{
		FMDERR("[%s]: Invlid value.line:%d\n",__FUNCTION__,__LINE__);
		return -EFAULT;
	}

	if (10 < count)
	{
		FMDERR("fm_sinr_threshold_write count value too much!\n");
		return -EFAULT;
	}

	if (copy_from_user(sinr_threshold, userbuf, count))
	{
		return -EFAULT;
	}

	g_sinr_threshold = simple_strtol(sinr_threshold, NULL, 10);
	FMDBG("fm_sinr_threshold_write exit  g_sinr_threshold:%d\n", g_sinr_threshold);
	return count;
}

static const struct file_operations fm_sinr_threshold_proc_fops = {
	.owner	= THIS_MODULE,
	.open	= fm_sinr_threshold_proc_open,
	.read	= seq_read,
	.write	= fm_sinr_threshold_write,
	.llseek	= seq_lseek,
	.release	= single_release,
};

int get_fm_sinr_samples(int *sinr)
{
	int sinr_samples_len;
	struct device_node *dp = NULL;
	const char *psinr_samples = NULL;
	int sinr_samples;

	dp = of_find_node_by_path("/huawei_fm_info");
	if (!dp)
	{
		FMDERR("device is not available!\n");
		return -1;
	}

	psinr_samples = of_get_property(dp,"fm,sinr_samples", &sinr_samples_len);
	if (NULL == psinr_samples)
	{
		FMDERR("get sinr threshold value failed .\n");
		return -1;
	}
	sinr_samples = simple_strtol(psinr_samples,NULL,10);
	*sinr = sinr_samples;
	return 0;
}

EXPORT_SYMBOL(get_fm_sinr_samples);

static int fm_sinr_samples_proc_show(struct seq_file *m, void *v)
{
	unsigned int sinr_samples = FM_SINR_SAMPLES_MAX;
	int ret;

	FMDBG("fm_sinr_samples_proc_show enter.\n");
	ret = get_fm_sinr_samples(&sinr_samples);
	if (-1 == ret)
	{
		FMDERR("Get FM Sinr Samples failed and will use default value 10.\n");
		sinr_samples = FM_SINR_SAMPLES_10;
	}

	if (FM_SINR_SAMPLES_MAX != g_sinr_samples)
	{
		sinr_samples = g_sinr_samples;
	}

	FMDBG("fm_sinr_samples_proc_show ,g_sinr_samples = %d, sinr_samples:%d\n", g_sinr_samples, sinr_samples);

	seq_printf(m, "%d", sinr_samples);
	return 0;
}

static int fm_sinr_samples_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fm_sinr_samples_proc_show, NULL);
}

static ssize_t fm_sinr_samples_write(struct file *filp, const char __user *userbuf,
						size_t count, loff_t *ppos)
{
	char sinr_samples[10] = { 0 };

	FMDBG("fm_sinr_samples_write  enter.  count:%d\n", (int)count);

	if (NULL == userbuf)
	{
		FMDERR("[%s]: Invlid value.line:%d\n",__FUNCTION__,__LINE__);
		return -EFAULT;
	}

	if ( 10 < count)
	{
		FMDERR("count value too much.");
		return -EFAULT;
	}

	if (copy_from_user(sinr_samples, userbuf, count))
		return -EFAULT;

	g_sinr_samples = simple_strtol(sinr_samples, NULL, 10);
	FMDBG("fm_sinr_samples_write exit  g_sinr_samples:%d\n", g_sinr_samples);
	return count;
}

static const struct file_operations fm_sinr_samples_proc_fops = {
	.owner	= THIS_MODULE,
	.open	= fm_sinr_samples_proc_open,
	.read	= seq_read,
	.write	= fm_sinr_samples_write,
	.llseek	= seq_lseek,
	.release	= single_release,
};

static bool hw_fm_buildin_gpio_set_value(int value)
{
	int ret;
	if (gpio_is_valid(g_lna_en_gpio)) {
		ret = gpio_direction_output(g_lna_en_gpio, value);
		FMDBG("fm gpio %d (0:down 1:up), ret = %d", value, ret);
		if (ret) {
			FMDERR("Unable to set direction");
			g_lna_en_gpio = DEFAULT_FM_LNA_NONEED;
			return false;
		}
	} else {
		FMDERR("fm_buildin_gpio is invalid");
		return false;
	}
	return true;
}

void set_headset_status(bool status) {
	g_is_headset_on = status;
	FMDBG("headset status is %s", g_is_headset_on == 1 ? "inserted":"not inserted");
	if (g_is_headset_on)
		hw_fm_buildin_gpio_set_value(GPIO_PULL_DOWN);
	else
		hw_fm_buildin_gpio_set_value(GPIO_PULL_UP);
	return;
}
EXPORT_SYMBOL(set_headset_status);

static bool hw_fm_buildin_gpio_init(void)
{
	int ret;
	struct device_node *fm_node = of_find_compatible_node(NULL, NULL,
							      "huawei,hw_fm_info");
	if (fm_node == NULL) {
		FMDERR("failed to find device node");
		return false;
	}
	g_lna_en_gpio = of_get_named_gpio_flags(fm_node, "fm,lna_gpio", 0, NULL);
	if (!gpio_is_valid(g_lna_en_gpio)) {
		FMDERR("fm_lna_gpio invalid, gpio= %d", g_lna_en_gpio);
		return false;
	}
	ret = gpio_request_one(g_lna_en_gpio, GPIOF_OUT_INIT_LOW, "g_lna_en_gpio");
	FMDBG("FM buildin gpio request ret = %d gpio = %d", ret, g_lna_en_gpio);
	if (ret) {
		FMDERR("failed to request gpio");
		g_lna_en_gpio = DEFAULT_FM_LNA_NONEED;
		return false;
	}
	return true;
}

static int fm_buildin_proc_show(struct seq_file *m, void *v)
{
	FMDBG("fm_buildin_proc_show g_fm_status = %d\n", g_fm_status);
	seq_printf(m, "%d", g_fm_status);
	return 0;
}

static int fm_buildin_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fm_buildin_proc_show, NULL);
}

static ssize_t fm_buildin_write(struct file *filp, const char __user *userbuf,
				size_t count, loff_t *ppos)
{
	int ret;
	unsigned int base = 10; /* set the number base to decimal */
	char fm_status[FM_STATUS_CMD_LEN] = {0};
	FMDBG("fm_buildin_write enter count:%d", (int)count);
	if (FM_STATUS_CMD_LEN < count) {
		FMDERR("count value too long");
		return -EFAULT;
	}
	ret = copy_from_user(fm_status, userbuf, count);
	if (ret) {
		FMDERR("copy_from_user error! ret = %d", ret);
		return -EFAULT;
	}
	g_fm_status = simple_strtol(fm_status, NULL, base);
	FMDBG("fm_buildin_write enter fm_status:%d (0:off 1:on)", g_fm_status);

	switch (g_fm_status) {
	case FM_ON:
		if (g_is_headset_on == false)
			hw_fm_buildin_gpio_set_value(GPIO_PULL_UP);
		else
			hw_fm_buildin_gpio_set_value(GPIO_PULL_DOWN);
		break;
	case FM_OFF:
		hw_fm_buildin_gpio_set_value(GPIO_PULL_DOWN);
		break;
	default:
		break;
	}
	return count;
}

static const struct file_operations fm_buildin_proc_fops = {
	.owner	 = THIS_MODULE,
	.open	 = fm_buildin_proc_open,
	.read	 = seq_read,
	.write	 = fm_buildin_write,
	.llseek	 = seq_lseek,
	.release = single_release,
};

/**
 * Initializes the module.
 * @return On success, 0. On error, -1, and <code>errno</code> is set
 * appropriately.
 */
static int __init featuretransfer_init(void)
{
	int retval = 0;
	struct proc_dir_entry *ent = NULL;

	FMDBG("BT DEVICE FEATURE VERSION: %s", VERSION);

	/* Driver Register */
	retval = platform_driver_register(&featuretransfer_driver);
	if (0 != retval)
	{
		FMDERR("[%s],featurntransfer driver register fail.", LOG_TAG);
		return retval;
	}

	/* create device_feature directory for bt chip info */
	device_dir = proc_mkdir("connectivity", NULL);
	if (NULL == device_dir)
	{
		FMDERR("Unable to create /proc/device_feature directory");
		return -ENOMEM;
	}

	/* Creating read/write "chiptype" entry for bluetooth chip type*/
	ent = proc_create("chiptype", 0, device_dir, &chiptype_proc_fops);
	if (NULL == ent)
	{
		FMDERR("Unable to create /proc/%s/chiptype entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}

	/* Creating read/write "chiptype" entry for bluetooth chip type*/
	ent = proc_create("subchiptype", 0, device_dir, &subchiptype_proc_fops);
	if (NULL == ent)
	{
		FMDERR("Unable to create /proc/%s/subchiptype entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}


	/* Creating read/write "bt_fm_fw_ver" entry */
	ent = proc_create("bt_fm_fw_ver", 0, device_dir, &bt_fm_ver_proc_fops);
	if (NULL == ent)
	{
		FMDERR("Unable to create /proc/%s/bt_fm_fw_ver entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}

	/* Creating read/write "bt_power_level" entry */
	ent = proc_create("power_level", 0, device_dir, &bt_power_level_proc_fops);
	if (NULL == ent)
	{
		FMDERR("Unable to create /proc/%s/power_level entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}

	/* Creating read/write "nv_path" entry */
	ent = proc_create("nv_path", 0, device_dir, &bt_nv_path_proc_fops);
	if (NULL == ent)
	{
		FMDERR("Unable to create /proc/%s/nv_path entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}

	/* Creating read/write "sinr" entry for bluetooth chip type*/
	ent = proc_create("sinr_threshold", 0, device_dir, &fm_sinr_threshold_proc_fops);
	if (NULL == ent)
	{
		FMDERR("Unable to create /proc/%s/sinr_threshold entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}

	/* Creating read/write "rssi" entry for bcm4330 fm*/
	ent = proc_create("sinr_samples", 0, device_dir, &fm_sinr_samples_proc_fops);
	if (NULL == ent)
	{
		FMDERR("Unable to create /proc/%s/sinr_samples entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}

	/* Creating read/write "fm_status" entry for fm */
	ent = proc_create("fm_status", S_IRWXUGO, device_dir, &fm_buildin_proc_fops);
	if (ent == NULL) {
		FMDERR("Unable to create /proc/%s/fm_status entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}
	if (hw_fm_buildin_gpio_init())
		hw_fm_buildin_gpio_set_value(GPIO_PULL_DOWN);

	return 0;

fail:
	remove_proc_entry("chiptype", device_dir);
	remove_proc_entry("subchiptype", bt_dir);
	remove_proc_entry("bt_fm_fw_ver", device_dir);
	remove_proc_entry("power_level", device_dir);
	remove_proc_entry("sinr_threshold", device_dir);
	remove_proc_entry("sinr_samples", device_dir);
	remove_proc_entry("fm_status", device_dir);
	remove_proc_entry("connectivity", 0);
	return retval;
}

/**
 * Cleans up the module.
 */
static void __exit featuretransfer_exit(void)
{
	platform_driver_unregister(&featuretransfer_driver);
	remove_proc_entry("chiptype", device_dir);
	remove_proc_entry("subchiptype", bt_dir);
	remove_proc_entry("bt_fm_fw_ver", device_dir);
	remove_proc_entry("power_level", device_dir);
	remove_proc_entry("sinr_threshold", device_dir);
	remove_proc_entry("sinr_samples", device_dir);
	remove_proc_entry("fm_status", device_dir);
	remove_proc_entry("connectivity", 0);
	if (gpio_is_valid(g_lna_en_gpio))
		gpio_free(g_lna_en_gpio);
}

module_init(featuretransfer_init);
module_exit(featuretransfer_exit);

MODULE_DESCRIPTION("BT DEVICE FEATURE VERSION: %s " VERSION);
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif
