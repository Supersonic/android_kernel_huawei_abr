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
#endif /* CONFIG_OF */

#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include "cts_config.h"
#include "cts_core.h"

extern bool cts_show_debug_log;

#ifndef LOG_TAG
#define LOG_TAG ""
#endif /* LOG_TAG */

enum cts_driver_log_level {
	CTS_DRIVER_LOG_ERROR,
	CTS_DRIVER_LOG_WARN,
	CTS_DRIVER_LOG_INFO,
	CTS_DRIVER_LOG_DEBUG,
};

extern int cts_start_driver_log_redirect(const char *filepath, bool append_to_file,
		char *log_buffer, int log_buf_size, int log_level);
extern void cts_stop_driver_log_redirect(void);
extern int cts_get_driver_log_redirect_size(void);
extern void cts_log(int level, const char *fmt, ...);

struct cts_device;
struct cts_device_touch_msg;
struct cts_device_gesture_info;

struct cts_platform_data {
	int irq;
	int int_gpio;
#ifdef CFG_CTS_HAS_RESET_PIN
	int rst_gpio;
#endif /* CFG_CTS_HAS_RESET_PIN */

};

#ifdef CONFIG_CTS_I2C_HOST
extern size_t cts_plat_get_max_i2c_xfer_size(struct cts_platform_data *pdata);
extern u8 *cts_plat_get_i2c_xfer_buf(struct cts_platform_data *pdata,
		size_t xfer_size);
extern int cts_plat_i2c_write(struct cts_platform_data *pdata, u8 i2c_addr,
		const void *src, size_t len, int retry, int delay);
extern int cts_plat_i2c_read(struct cts_platform_data *pdata, u8 i2c_addr,
		const u8 *wbuf, size_t wlen, void *rbuf, size_t rlen,
		int retry, int delay);
extern int cts_plat_is_i2c_online(struct cts_platform_data *pdata, u8 i2c_addr);
#else /* CONFIG_CTS_I2C_HOST */
extern size_t cts_plat_get_max_spi_xfer_size(struct cts_platform_data *pdata);
extern u8 *cts_plat_get_spi_xfer_buf(struct cts_platform_data *pdata, size_t xfer_size);
extern int cts_plat_spi_write(struct cts_platform_data *pdata, u8 i2c_addr, const void *src,
		size_t len, int retry, int delay);
extern int cts_plat_spi_read(struct cts_platform_data *pdata, u8 i2c_addr,
		const u8 *wbuf, size_t wlen, void *rbuf, size_t rlen,
		int retry, int delay);
extern int cts_plat_spi_read_delay_idle(struct cts_platform_data *pdata, u8 dev_addr,
		const u8 *wbuf, size_t wlen, void *rbuf, size_t rlen,
		int retry, int delay, int idle);
extern int cts_plat_is_normal_mode(struct cts_platform_data *pdata);
#endif /* CONFIG_CTS_I2C_HOST */

#ifdef CFG_CTS_HAS_RESET_PIN
extern int cts_plat_reset_device(struct cts_platform_data *pdata);
#else /* CFG_CTS_HAS_RESET_PIN */
static inline int cts_plat_reset_device(struct cts_platform_data *pdata) {return 0;}
#endif /* CFG_CTS_HAS_RESET_PIN */

extern int cts_plat_release_all_touch(struct cts_platform_data *pdata);

#ifdef CONFIG_CTS_VIRTUALKEY
extern int cts_plat_init_vkey_device(struct cts_platform_data *pdata);
extern void cts_plat_deinit_vkey_device(struct cts_platform_data *pdata);
extern int cts_plat_process_vkey(struct cts_platform_data *pdata, u8 vkey_state);
extern int cts_plat_release_all_vkey(struct cts_platform_data *pdata);
#else /* CONFIG_CTS_VIRTUALKEY */
static inline int cts_plat_init_vkey_device(struct cts_platform_data *pdata) {return 0;}
static inline void cts_plat_deinit_vkey_device(struct cts_platform_data *pdata) {}
static inline int cts_plat_process_vkey(struct cts_platform_data *pdata, u8 vkey_state) {return 0;}
static inline int cts_plat_release_all_vkey(struct cts_platform_data *pdata) {return 0;}
#endif /* CONFIG_CTS_VIRTUALKEY */

extern int cts_plat_set_reset(struct cts_platform_data *pdata, int val);
extern int cts_plat_get_int_pin(struct cts_platform_data *pdata);

#endif /* CTS_PLATFORM_H */

