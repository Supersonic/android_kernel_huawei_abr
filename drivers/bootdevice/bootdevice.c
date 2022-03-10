#include <linux/bootdevice.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/setup.h>

#define ANDROID_BOOT_DEV_MAX 50
static char android_boot_dev[ANDROID_BOOT_DEV_MAX];

static int  get_android_boot_dev(void)
{
	int ret = 0;
	char *p1 = NULL;
	char *p2 = NULL;

	p1 = strnstr(saved_command_line, "androidboot.bootdevice=",
		COMMAND_LINE_SIZE);
	if (p1 == NULL) {
		ret = -1;
		return ret;
	}

	p2 = strnchr(p1, strlen(p1), ' ');
	if (p2 == NULL) {
		ret = -1;
		return ret;
	}

	strlcpy(android_boot_dev, p1,
		((p2 - p1 + 1) > ANDROID_BOOT_DEV_MAX) ?
			ANDROID_BOOT_DEV_MAX : (p2 - p1 + 1));

	return ret;
}

static int get_bootdevice_type_from_cmdline(void)
{
	int type;

	if (strnstr(android_boot_dev, "sdhci", ANDROID_BOOT_DEV_MAX))
		type = 0;
	else if (strnstr(android_boot_dev, "ufshc", ANDROID_BOOT_DEV_MAX))
		type = 1;
	else
		type = -1;

	return type;
}
static int __init bootdevice_init(void)
{
	int err;
	enum bootdevice_type type;

	if (get_android_boot_dev()) {
		type = -1;
		err = -ENODEV;
		goto out;
	}

	type = get_bootdevice_type_from_cmdline();
	if (type == -1) {
		err = -ENODEV;
		goto out;
	}
	pr_info("storage bootdevice: %s\n",
		type == BOOT_DEVICE_EMMC ? "eMMC" : "UFS");

	err = 0;

out:
	set_bootdevice_type(type);
	return err;
}
arch_initcall(bootdevice_init);
