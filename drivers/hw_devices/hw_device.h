
#ifndef _HW_DEVICE
#define _HW_DEVICE
#include <linux/device.h>
#include <linux/spmi.h>


struct hw_device_info {
	struct device		*dev;
	struct spmi_device	*spmi;
	u16 		base_addr;
	u8 pmic_status;
};
#define NORMAL_STATE 0
#define ABNORMAL_STATE 1
#endif
