#ifndef CTS_PLATFORM_H
#define CTS_PLATFORM_H

#include <linux/types.h>
#include <asm/byteorder.h>
#include <linux/bitops.h>
#include <linux/ctype.h>
#include <linux/unaligned/access_ok.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/spinlock.h>
#include <linux/rtmutex.h>
#include <linux/byteorder/generic.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/rtc.h>
#include <linux/ktime.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/suspend.h>
#include <linux/firmware.h>
#include <linux/syscalls.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#endif				/* CONFIG_OF */

#include "cts_config.h"
#include "cts_core.h"

#ifndef LOG_TAG
#define LOG_TAG         ""
#endif				/* LOG_TAG */

struct cts_device;
struct cts_device_touch_msg;
struct cts_device_gesture_info;

#define SWITCH_OFF                  0
#define SWITCH_ON                   1

struct cts_platform_data {
	int irq;
	int int_gpio;
	int rst_gpio;
	struct i2c_client *i2c_client;
};

size_t cts_plat_get_max_i2c_xfer_size(struct cts_platform_data *pdata);
u8 *cts_plat_get_i2c_xfer_buf(struct cts_platform_data *pdata,
			      size_t xfer_size);
int cts_plat_i2c_write(struct cts_platform_data *pdata, u8 i2c_addr,
		       const void *src, size_t len, int retry, int delay);
int cts_plat_i2c_read(struct cts_platform_data *pdata, u8 i2c_addr,
		      const u8 *wbuf, size_t wlen, void *rbuf,
		      size_t rlen, int retry, int delay);
int cts_plat_is_i2c_online(struct cts_platform_data *pdata, u8 i2c_addr);

int cts_plat_reset_device(struct cts_platform_data *pdata);

int cts_plat_release_all_touch(struct cts_platform_data *pdata);
int cts_plat_set_reset(struct cts_platform_data *pdata, int val);
int cts_plat_get_int_pin(struct cts_platform_data *pdata);

#endif				/* CTS_PLATFORM_H */
