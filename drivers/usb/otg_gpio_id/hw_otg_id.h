#ifndef HISI_USB_OTG_ID_H
#define HISI_USB_OTG_ID_H

#include <linux/power_supply.h>

#define SAMPLE_DOING 0
#define SAMPLE_DONE 1

struct otg_gpio_id_dev {
	struct platform_device *pdev;
	struct work_struct  otg_intb_work;
	struct delayed_work init_work;
	struct power_supply otg_psy;
	struct device_node *vadc_node;
	struct power_supply *usb_psy;
	unsigned int switch_mode;
	int gpio;
	int irq;
};
extern void smb_notify_to_host(bool enable);

#define hw_usb_dbg(format, arg...) \
	do { \
		printk(KERN_INFO "[USB_DEBUG][%s]"format, __func__, ##arg); \
	} while (0)
#define hw_usb_err(format, arg...) \
	do { \
		printk(KERN_ERR "[USB_DEBUG]"format, ##arg); \
	} while (0)

#endif

