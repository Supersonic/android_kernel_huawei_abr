

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include "securec.h"

#define WLAN_DEVICE_VERSION "1.0"
#define PROC_DIR "wlan_feature"
#define WIFI_CODE_UNKNOWN "Unknown"
#define WIFI_PROPERTY_CHIP_TYPE "wifi,chiptype"
#define WIFI_PROPERTY_BOARD_NAME "wifi,pubfile_id"

static struct proc_dir_entry *wlan_dir = NULL;

static int wlan_featuretrans_remove(struct platform_device *pdev)
{
	pr_debug("wlan devicefeature removed.");
	return 0;
}
static struct platform_driver wlan_featuretrans_driver = {
	.remove = wlan_featuretrans_remove,
	.driver = {
		.name = "wlanfeaturetrans",
		.owner = THIS_MODULE,
	},
};

struct wifi_chip_cfg {
	const char *chip_type;
	const char *chip_code;
	const char *driver_module_name;
};

/* wifi chip name defination. */
static const struct wifi_chip_cfg wifi_chip_cfgs[] = {
	{ "WIFI_BROADCOM_4330", "1.2", NULL },
	{ "WIFI_BROADCOM_4330X", "1.3", NULL },
	{ "WIFI_QUALCOMM_WCN3660", "2.2", NULL },
	{ "WIFI_QUALCOMM_WCN3620", "2.3", NULL },
	{ "WIFI_QUALCOMM_WCN3660B", "2.4", NULL },
	{ "WIFI_QUALCOMM_WCN3680", "2.5", NULL },
	{ "WIFI_QUALCOMM_WCN3610", "2.6", NULL },
	{ "WIFI_QUALCOMM_WCN3615", "2.8", NULL },
	{ "WIFI_QUALCOMM_QCA6391", "3.2", NULL },
	{ "WIFI_QUALCOMM_WCN3950", "4.1", NULL },
	{ "WIFI_QUALCOMM_WCN3980", "4.2", NULL },
	{ "WIFI_QUALCOMM_WCN6851", "4.3", "wlan" },
	{ "WIFI_QUALCOMM_WCN6750", "4.4", "qca6750" },
	{ "WIFI_TYPE_UNKNOWN", WIFI_CODE_UNKNOWN, NULL }
};

static const void *hw_wifi_get_property(const char *name, int *length)
{
	struct device_node *dp = NULL;
	if (name != NULL) {
		dp = of_find_node_by_path("/huawei_wifi_info");
		if(dp != NULL) {
			return of_get_property(dp, name, length);
		}
	}

	pr_err("hw_wifi_get_property fail");
	return NULL;
}

/* Get wifi device type.*/
static const char *hw_wifi_get_chip_code(void)
{
	int i = 0;
	int length = 0;
	int array_size = ARRAY_SIZE(wifi_chip_cfgs);
	const char *wifi_chip_type = NULL;

	/* get wifi chip type from the device feature configuration (.dtsi file) */
	wifi_chip_type = hw_wifi_get_property(WIFI_PROPERTY_CHIP_TYPE, &length);
	if(wifi_chip_type == NULL) {
		pr_err("hw_wifi_get_chip_code fail, not dtsi node");
		return wifi_chip_cfgs[array_size - 1].chip_code;
	}

	/* lookup wifi_device_model in wifi_chip_cfgs[] */
	for(i = 0; i < array_size; i++) {
		if(0 == strncmp(wifi_chip_type, wifi_chip_cfgs[i].chip_type, length)) {
			break;
		}
	}
	/* If we get invalid type name, return "Unknown".*/
	if(i == array_size) {
		 pr_err("hw_wifi_get_chip_code fail, dismatch");
		 return WIFI_CODE_UNKNOWN;
	}

	return wifi_chip_cfgs[i].chip_code;
}

static int devtype_proc_show(struct seq_file *m, void *v)
{
	const char *wifi_device_type = NULL;
	wifi_device_type = hw_wifi_get_chip_code();
	pr_debug("wifi_device_type:%s", wifi_device_type);
	seq_printf(m,"%s",wifi_device_type);
	return 0;
}

static int devtype_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, devtype_proc_show, NULL);
}

 static const struct file_operations devtype_proc_fops = {
	.owner		= THIS_MODULE,
	.open		= devtype_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

const void *hw_wlan_get_board_name(void)
{
	int length = 0;
	return hw_wifi_get_property(WIFI_PROPERTY_BOARD_NAME, &length);
}

EXPORT_SYMBOL(hw_wlan_get_board_name);

static int pubfd_proc_show(struct seq_file *m, void *v)
{
	const char *wifi_pubfile_id = NULL;
	wifi_pubfile_id = hw_wlan_get_board_name();

	if (NULL != wifi_pubfile_id) {
		pr_debug("%s enter wifi_device_type:%s;\n", __func__,wifi_pubfile_id);
	} else {
		pr_err("%s get pubfd failed;\n", __func__);
		return 0;
	}

	seq_printf(m,"%s",wifi_pubfile_id);
	return 0;
}

static int pubfd_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, pubfd_proc_show, NULL);
}

 static const struct file_operations pubfd_proc_fops = {
	.owner		= THIS_MODULE,
	.open		= pubfd_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

const void *get_wlan_fw_ver(void)
{
	int wlan_fw_ver_len;
	struct device_node *dp = NULL;
	dp = of_find_node_by_path("/huawei_wifi_info");
	if(dp == NULL) {
		 pr_err("device is not available!\n");
		 return NULL;
	} else {
		 pr_debug("%s:dp->name:%s,dp->full_name:%s;\n",__func__,dp->name,dp->full_name);
	}

	return of_get_property(dp,"wifi,fw_ver", &wlan_fw_ver_len);
}

static int wlan_fw_ver_proc_show(struct seq_file *m, void *v)
{
	const char *wlan_fw_ver = NULL;
	wlan_fw_ver = get_wlan_fw_ver();

	if (wlan_fw_ver != NULL) {
		 pr_debug("%s enter wifi_device_type:%s;\n", __func__,wlan_fw_ver);
	} else {
		pr_err("%s get wlan_fw_ver failed;\n", __func__);
		return 0;
	}

	seq_printf(m,"%s",wlan_fw_ver);
	return 0;
}

static int wlan_fw_ver_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, wlan_fw_ver_proc_show, NULL);
}

 static const struct file_operations wlan_fw_ver_proc_fops = {
	.owner		= THIS_MODULE,
	.open		= wlan_fw_ver_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

const int get_hw_wifi_no_autodetect_xo(void)
{
	struct device_node *dp = NULL;
	int ret = -1;

	dp = of_find_node_by_path("/huawei_wifi_info");
	if (dp == NULL) {
		 pr_err("device is not available!\n");
		 return ret;
	} else {
		 pr_debug("%s:dp->name:%s,dp->full_name:%s;\n",__func__,dp->name,dp->full_name);
	}

	return of_property_read_bool(dp,"wifi,no-autodetect-xo");
}
EXPORT_SYMBOL(get_hw_wifi_no_autodetect_xo);

const void *get_fem_check_flag(void)
{
	int get_fem_check_len;
	struct device_node *dp = NULL;

	dp = of_find_node_by_path("/huawei_wifi_info");
	if (dp == NULL) {
		pr_err("%s:device is not available!\n",__func__);
		return NULL;
	}

	return of_get_property(dp,"wifi,fem_check_flag", &get_fem_check_len);
}
EXPORT_SYMBOL(get_fem_check_flag);

static int fen_check_flag_show(struct seq_file *m, void *v)
{
	const char *fem_check_flag = NULL;
	fem_check_flag = get_fem_check_flag();
	if (fem_check_flag != NULL) {
		pr_err("%s enter fem_check_flag:%s;\n", __func__,fem_check_flag);
	} else {
		pr_err("%s get fem_check_flag failed;\n", __func__);
		return 0;
	}

	seq_printf(m,"%s",fem_check_flag);
	return 0;
}

static int fem_check_flag_open(struct inode *inode, struct file *file)
{
	return single_open(file, fen_check_flag_show, NULL);
}

 static const struct file_operations fem_check_flag_proc_fops = {
	.owner		= THIS_MODULE,
	.open		= fem_check_flag_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
const void *get_fem_check_5g_flag(void)
{
	int get_fem_check_5g_len;
	struct device_node *dp = NULL;

	dp = of_find_node_by_path("/huawei_wifi_info");
	if (dp == NULL) {
		pr_err("get_fem_check_5g_flag:device is not available!\n");
		return NULL;
	}

	return of_get_property(dp,"wifi,fem_check_5g_flag", &get_fem_check_5g_len);
}
EXPORT_SYMBOL(get_fem_check_5g_flag);

static int fem_check_5g_flag_show(struct seq_file *m, void *v)
{
	const char *fem_check_5g_flag = NULL;

	fem_check_5g_flag = get_fem_check_5g_flag();
	if (fem_check_5g_flag != NULL) {
		pr_err("get_fem_check_5g_flag: enter fem_check_5g_flag:%s;\n", fem_check_5g_flag);
	} else {
		pr_err("get_fem_check_5g_flag get fem_check_5g_flag failed;\n");
		return 0;
	}

	seq_printf(m,"%s",fem_check_5g_flag);
	return 0;
}

static int fem_check_5g_flag_open(struct inode *inode, struct file *file)
{
	return single_open(file, fem_check_5g_flag_show, NULL);
}

static const struct file_operations fem_check_5g_flag_proc_fops = {
	.owner		= THIS_MODULE,
	.open		= fem_check_5g_flag_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static const void *get_hw_wifi_ini_type(void)
{
	int wifi_ini_type_len;
	struct device_node *dp = NULL;

	dp = of_find_node_by_path("/huawei_wifi_info");
	if (dp == NULL) {
		 pr_err("device is not available!\n");
		 return NULL;
	} else {
		 pr_debug("%s:dp->name:%s,dp->full_name:%s;\n",__func__,dp->name,dp->full_name);
	}

	return of_get_property(dp,"wifi,ini_type", &wifi_ini_type_len);
}

static int ini_type_proc_show(struct seq_file *m, void *v)
{
	const char *wifi_ini_type = NULL;
	wifi_ini_type = get_hw_wifi_ini_type();

	if (NULL != wifi_ini_type) {
		pr_debug("%s enter wifi_device_type:%s;\n", __func__,wifi_ini_type);
	} else {
		pr_err("%s get pubfd failed;\n", __func__);
		return 0;
	}

	seq_printf(m,"%s",wifi_ini_type);
	return 0;
}

static int ini_type_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ini_type_proc_show, NULL);
}

static const struct file_operations ini_type_proc_fops = {
	.owner		= THIS_MODULE,
	.open		= ini_type_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init wlanfeaturetrans_init(void)
{
	int retval = 0;
	struct proc_dir_entry *entry = NULL;

	pr_debug("WIFI DEVICE FEATURE VERSION: %s", WLAN_DEVICE_VERSION);

	/* Driver Register */
	retval = platform_driver_register(&wlan_featuretrans_driver);
	if (0 != retval) {
		pr_err("[%s],featurntransfer driver register fail.", __func__);
		return retval;
	}

	/* create device_feature directory for wifi chip info */
	wlan_dir = proc_mkdir("wlan_feature", NULL);
	if (wlan_dir == NULL) {
		pr_err("Unable to create /proc/wlan_feature directory");
		retval = -ENOMEM;
		goto error_dir;
	}

	/* Creating read/write "devtype" entry */
	entry = proc_create("devtype", 0, wlan_dir, &devtype_proc_fops);
	if (entry == NULL) {
		pr_err("Unable to create /proc/%s/devtype entry", PROC_DIR);
		retval = -ENOMEM;
		goto error_devtype;
	}

	/* Creating read/write "pubfd" entry */
	entry = proc_create("pubfd", 0, wlan_dir, &pubfd_proc_fops);
	if (entry == NULL) {
		pr_err("Unable to create /proc/%s/pubfd entry", PROC_DIR);
		retval = -ENOMEM;
		goto error_pubfd;
	}

	/* Creating read/write "wlan_fw_ver" entry for wifi chip type */
	entry = proc_create("wlan_fw_ver", 0, wlan_dir, &wlan_fw_ver_proc_fops);
	if (entry == NULL) {
		pr_err("Unable to create /proc/%s/wlan_fw_ver entry", PROC_DIR);
		retval = -ENOMEM;
		goto error_wlan_fw_ver;
	}

	/* Creating read/write "fem_check_flag" entry for wifi chip type */
	entry = proc_create("fem_check_flag", 0, wlan_dir, &fem_check_flag_proc_fops);
	if (entry == NULL) {
		pr_err("%s:Unable to create /proc/%s/fem_check_flag entry",__func__,PROC_DIR);
		retval = -ENOMEM;
		goto error_fem_check_flag;
	}
	/* Creating read/write "fem_check_5g_flag" entry for wifi chip type */
	entry = proc_create("fem_check_5g_flag", 0, wlan_dir, &fem_check_5g_flag_proc_fops);
	if (entry == NULL) {
		pr_err("%s:Unable to create /proc/%s/fem_check_5g_flag entry",__func__,PROC_DIR);
		retval = -ENOMEM;
		goto error_fem_check_5g_flag;
	}
	/* Creating read/write "ini_type" entry*/
	entry = proc_create("ini_type", 0, wlan_dir, &ini_type_proc_fops);
	if (entry == NULL) {
		pr_err("Unable to create /proc/%s/ini_type entry", PROC_DIR);
		retval = -ENOMEM;
		goto error_ini_type;
	}
	return 0;

error_ini_type:
	remove_proc_entry("ini_type", wlan_dir);
error_fem_check_5g_flag:
	remove_proc_entry("fem_check_5g_flag", wlan_dir);
error_fem_check_flag:
	remove_proc_entry("fem_check_flag", wlan_dir);
error_wlan_fw_ver:
	remove_proc_entry("wlan_fw_ver", wlan_dir);
error_pubfd:
	remove_proc_entry("pubfd", wlan_dir);
error_devtype:
	remove_proc_entry("devtype", wlan_dir);
error_dir:
	remove_proc_entry("wlan_feature", NULL);
	platform_driver_unregister(&wlan_featuretrans_driver);
	return retval;
}

/**
 * Cleans up the module.
 */
static void __exit wlanfeaturetrans_exit(void)
{
	platform_driver_unregister(&wlan_featuretrans_driver);
	remove_proc_entry("devtype", wlan_dir);
	remove_proc_entry("pubfd", wlan_dir);
	remove_proc_entry("wlan_fw_ver", wlan_dir);
	remove_proc_entry("fem_check_flag", wlan_dir);
	remove_proc_entry("fem_check_5g_flag", wlan_dir);
	remove_proc_entry("ini_type", wlan_dir);
	remove_proc_entry("wlan_feature", NULL);
}

int hw_wlan_get_cust_ini_path(char *ini_path, size_t len)
{
	const char *ini_type = NULL;
	int ret;

	ini_type = get_hw_wifi_ini_type();
	if (ini_type == NULL) {
		pr_err("hw_wlan: get_hw_wifi_ini_type fail");
		return -1;
	}

	ret = snprintf_s(ini_path, len, len, "WCNSS_qcom_cfg_%s.ini", ini_type);
	if (ret < 0) {
		pr_err("hw_wlan: hw_wlan_get_cust_ini_path fail");
		return -1;
	}
	return 0;
}

EXPORT_SYMBOL(hw_wlan_get_cust_ini_path);

bool hw_wlan_is_driver_match(const char *driver_module_name)
{
	int i;
	int length = 0;
	const char *wifi_chip_type = NULL;
	int array_size = ARRAY_SIZE(wifi_chip_cfgs);

	if (driver_module_name == NULL) {
		pr_err("hw_wlan: driver_match fail, module name invalid");
		return false;
	}

	wifi_chip_type = hw_wifi_get_property(WIFI_PROPERTY_CHIP_TYPE, &length);
	if (wifi_chip_type == NULL) {
		pr_err("driver_match fail, not dtsi node");
		return false;
	}

	for(i = 0; i < array_size; i++) {
		if(0 == strncmp(wifi_chip_type, wifi_chip_cfgs[i].chip_type, length)) {
			break;
		}
	}

	if(i == array_size) {
		 pr_err("driver_match fail, dismatch chip type");
		 return false;
	}

	if(wifi_chip_cfgs[i].driver_module_name == NULL) {
		pr_err("driver_match fail, driver_module_name not provided");
		return false;
	}

	return !strncmp(wifi_chip_cfgs[i].driver_module_name, driver_module_name, length);
}

EXPORT_SYMBOL(hw_wlan_is_driver_match);

module_init(wlanfeaturetrans_init);
module_exit(wlanfeaturetrans_exit);

MODULE_DESCRIPTION("WIFI DEVICE FEATURE VERSION: %s " WLAN_DEVICE_VERSION);
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif
