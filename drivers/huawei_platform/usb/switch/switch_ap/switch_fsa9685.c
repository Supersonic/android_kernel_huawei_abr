/*
 * switch_fsa9685.c
 *
 * driver file for switch chip
 *
 * Copyright (c) 2013-2020 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/param.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <asm/irq.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/power_supply.h>
#include <huawei_platform/usb/switch/switch_usb.h>
#include "switch_chip.h"
#ifdef CONFIG_HDMI_K3
#include <../video/k3/hdmi/k3_hdmi.h>
#endif
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
#endif
#include <huawei_platform/usb/hw_pd_dev.h>
#ifdef CONFIG_BOOST_5V
#include <chipset_common/hwpower/hardware_ic/boost_5v.h>
#endif
#include <linux/pm_wakeup.h>
#include <huawei_platform/log/hw_log.h>
#include <chipset_common/hwusb/hw_usb_rwswitch.h>
#include <huawei_platform/usb/switch/switch_fsa9685.h>
#include <chipset_common/hwpower/adapter/adapter_test.h>
#include <chipset_common/hwpower/common_module/power_devices_info.h>
#include <huawei_platform/power/huawei_charger_adaptor.h>
#include <huawei_platform/power/huawei_charger_common.h>
#include <huawei_platform/power/direct_charger/direct_charger.h>
#include <chipset_common/hwpower/hardware_monitor/water_detect.h>
#include <hwmanufac/dev_detect/dev_detect.h>
#include <chipset_common/hwpower/protocol/adapter_protocol_fcp.h>
#include <chipset_common/hwpower/protocol/adapter_protocol_scp.h>
#include <chipset_common/hwpower/common_module/power_gpio.h>
#include "rt8979_bc12.h"

extern unsigned int get_boot_into_recovery_flag(void);
#define HWLOG_TAG switch_fsa9685
HWLOG_REGIST();

#define FSA9688_NON_STANDARD_RETRY_MAX 3

static struct fsa9685_device_info *g_fsa9685_dev;

static struct fsa9685_device_ops *g_fsa9685_dev_ops;

static int vendor_id;
static int gpio = -1;
static int g_switch_gpio = -1;

static u32 scp_error_flag; /* scp error flag */
static int rt8979_osc_lower_bound; /* lower bound for OSC setting */
static int rt8979_osc_upper_bound; /* upper bound for OSC setting */
static int rt8979_osc_trim_code; /* OSC trimming setting (read from IC) */
static int rt8979_osc_trim_adjust; /* OSC adjustment */
static int rt8979_osc_trim_default; /* While attaching reset OSC adjustment */
static bool rt8979_dcd_timeout_enabled;
static bool adp_plugout;
static int g_non_standard_retrycnt;

static inline bool is_rt8979(void)
{
	return (vendor_id == RT8979);
}
static bool rt8979_is_in_fm8(void);
static void rt8979_auto_restart_accp_det(void);
static void rt8979_force_restart_accp_det(bool open);
static int rt8979_sw_open(bool open);
static int rt8979_adjust_osc(int8_t val);

static void rt8979_regs_dump(void);
int is_support_fcp(void);

/* stub */
void charge_type_dcp_detected_notify(void)
{
}

#define ID_FALL_EVENT 2
#define ID_RISE_EVENT 3
inline int hisi_usb_id_change(int event)
{
	return 0;
}

static inline bool is_adp_plugout(void)
{
	if (!g_fsa9685_dev)
		return false;

	if (g_fsa9685_dev->power_by_5v && adp_plugout)
		return true;

	return false;
}

static inline void accp_sleep(void)
{
	/* sleep ACCP_POLL_TIME ms */
	usleep_range(ACCP_POLL_TIME * 1000, (ACCP_POLL_TIME + 1) * 1000);
}

void fsa9685_accp_detect_lock(void)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di) {
		hwlog_err("di is null\n");
		return;
	}

	mutex_lock(&di->accp_detect_lock);

	hwlog_info("accp_detect_lock lock\n");
}

void fsa9685_accp_detect_unlock(void)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di) {
		hwlog_err("di is null\n");
		return;
	}

	mutex_unlock(&di->accp_detect_lock);

	hwlog_info("accp_detect_lock unlock\n");
}

void fsa9685_accp_adaptor_reg_lock(void)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di) {
		hwlog_err("di is null\n");
		return;
	}

	mutex_lock(&di->accp_adaptor_reg_lock);

	hwlog_info("accp_adaptor_reg_lock lock\n");
}

void fsa9685_accp_adaptor_reg_unlock(void)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di) {
		hwlog_err("di is null\n");
		return;
	}

	mutex_unlock(&di->accp_adaptor_reg_lock);

	hwlog_info("accp_adaptor_reg_lock unlock\n");
}

void fsa9685_usb_switch_wake_lock(void)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di) {
		hwlog_err("di is null\n");
		return;
	}

	if (!di->usb_switch_lock.active) {
		__pm_stay_awake(&di->usb_switch_lock);
		hwlog_info("usb_switch_lock lock\n");
	}
}

void fsa9685_usb_switch_wake_unlock(void)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di) {
		hwlog_err("di is null\n");
		return;
	}

	if (di->usb_switch_lock.active) {
		__pm_relax(&di->usb_switch_lock);
		hwlog_info("usb_switch_lock unlock\n");
	}
}

static int fsa9685_write_reg(int reg, int val)
{
	int ret = -1;
	int i;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return ret;
	}

	for (i = 0; i < I2C_RETRY && ret < 0; i++) {
		ret = i2c_smbus_write_byte_data(di->client, reg, val);
		if (ret < 0) {
			hwlog_err("write_reg failed[%x]\n", reg);
			usleep_range(1000, 1100); /* sleep 1ms */
		}
	}

	return ret;
}

static int fsa9685_read_reg(int reg)
{
	int ret = -1;
	int i;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return ret;
	}

	for (i = 0; i < I2C_RETRY && ret < 0; i++) {
		ret = i2c_smbus_read_byte_data(di->client, reg);
		if (ret < 0) {
			hwlog_err("read_reg failed[%x]\n", reg);
			usleep_range(1000, 1100); /* sleep 1ms */
		}
	}

	return ret;
}

static int fsa9685_write_reg_mask(int reg, int value, int mask)
{
	int ret;
	int val;

	val = fsa9685_read_reg(reg);
	if (val < 0)
		return val;

	val &= ~mask;
	val |= value & mask;
	ret = fsa9685_write_reg(reg, val);

	return ret;
}

int fsa9685_common_write_reg(int reg, int val)
{
	return fsa9685_write_reg(reg, val);
}

int fsa9685_common_read_reg(int reg)
{
	return fsa9685_read_reg(reg);
}

int fsa9685_common_write_reg_mask(int reg, int value, int mask)
{
	return fsa9685_write_reg_mask(reg, value, mask);
}

static int fsa9685_get_device_id(void)
{
	int id;
	int vendor_id;
	int version_id;
	int device_id;

	id = fsa9685_read_reg(FSA9685_REG_DEVICE_ID);
	if (id < 0)
		return -1;

	vendor_id = (id & FSA9685_REG_DEVICE_ID_VENDOR_ID_MASK) >>
		FSA9685_REG_DEVICE_ID_VENDOR_ID_SHIFT;
	version_id = (id & FSA9685_REG_DEVICE_ID_VERSION_ID_MASK) >>
		FSA9685_REG_DEVICE_ID_VERSION_ID_SHIFT;

	hwlog_info("get_device_id [%x]=%x,%d,%d\n", FSA9685_REG_DEVICE_ID,
		id, vendor_id, version_id);

	if (vendor_id == FSA9685_VENDOR_ID) {
		if (version_id == FSA9683_VERSION_ID) {
			hwlog_info("find fsa9683\n");
			device_id = USBSWITCH_ID_FSA9683;
		} else if (version_id == FSA9688_VERSION_ID) {
			hwlog_info("find fsa9688\n");
			device_id = USBSWITCH_ID_FSA9688;
		} else if (version_id == FSA9688C_VERSION_ID) {
			hwlog_info("find fsa9688c\n");
			device_id = USBSWITCH_ID_FSA9688C;
		} else {
			hwlog_err("use default id (fsa9685)\n");
			device_id = USBSWITCH_ID_FSA9685;
		}
	} else if (vendor_id == RT8979_VENDOR_ID) {
		if (version_id == RT8979_1_VERSION_ID) {
			hwlog_info("find rt8979 (first revision)\n");
			device_id = USBSWITCH_ID_RT8979;
		} else if (version_id == RT8979_2_VERSION_ID) {
			hwlog_info("find rt8979 (second revision)\n");
			device_id = USBSWITCH_ID_RT8979;
		} else {
			hwlog_err("use default id (rt8979)\n");
			device_id = USBSWITCH_ID_RT8979;
		}
	} else {
		hwlog_err("use default id (fsa9685)\n");
		device_id = USBSWITCH_ID_FSA9685;
	}

	return device_id;
}

static int fsa9685_device_ops_register(struct fsa9685_device_ops *ops)
{
	if (ops) {
		g_fsa9685_dev_ops = ops;
		hwlog_info("fsa9685_device ops register ok\n");
	} else {
		hwlog_info("fsa9685_device ops register fail\n");
		return -1;
	}

	return 0;
}

static void fsa9685_select_device_ops(int device_id)
{
	switch (device_id) {
	case USBSWITCH_ID_FSA9683:
	case USBSWITCH_ID_FSA9685:
	case USBSWITCH_ID_FSA9688:
	case USBSWITCH_ID_FSA9688C:
		fsa9685_device_ops_register(usbswitch_fsa9685_get_device_ops());
		break;
	case USBSWITCH_ID_RT8979:
		fsa9685_device_ops_register(usbswitch_rt8979_get_device_ops());
		break;
	default:
		fsa9685_device_ops_register(usbswitch_fsa9685_get_device_ops());
		hwlog_err("use default ops (fsa9685)\n");
		break;
	}
}

static int fsa9685_manual_switch(int input_select)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;
	struct fsa9685_device_ops *ops = g_fsa9685_dev_ops;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return -1;
	}

	if (!ops || !ops->manual_switch) {
		hwlog_err("ops is null or manual_switch is null\n");
		return -1;
	}

	hwlog_info("input_select=%d\n", input_select);

	/* two switch not support USB2_ID */
	if (di->two_switch_flag &&
		(input_select == FSA9685_USB2_ID_TO_IDBYPASS))
		return 0;

	return ops->manual_switch(input_select);
}

static int fsa9685_manual_detach_work(void)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return -ERR_NO_DEV;
	}

	/* detach work delay 20ms */
	schedule_delayed_work(&di->detach_delayed_work, msecs_to_jiffies(20));

	return 0;
}
static void fsa9685_detach_work(struct work_struct *work)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;
	struct fsa9685_device_ops *ops = g_fsa9685_dev_ops;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return;
	}

	if (!ops || !ops->detach_work) {
		hwlog_err("ops is null or detach_work is null\n");
		return;
	}

	return ops->detach_work();
}

static int fsa9685_dcd_timeout(bool enable_flag)
{
	int reg_val;
	struct fsa9685_device_info *di = g_fsa9685_dev;
	int ret;

	if (!di) {
		hwlog_err("di is null\n");
		return SET_DCDTOUT_FAIL;
	}
	enable_flag |= di->dcd_timeout_force_enable;
	if (!is_rt8979()) {
		reg_val = fsa9685_read_reg(FSA9685_REG_DEVICE_ID);
		reg_val &= FAS9685_VERSION_ID_BIT_MASK;
		reg_val >>= FAS9685_VERSION_ID_BIT_SHIFT;
		/* 9688c 9683 except 9688 do not support */
		if (reg_val == FSA9688_VERSION_ID)
			return SET_DCDTOUT_FAIL;
		ret = fsa9685_write_reg_mask(FSA9685_REG_CONTROL2,
			enable_flag, FSA9685_DCD_TIME_OUT_MASK);
		if (ret < 0)
			return SET_DCDTOUT_FAIL;
		hwlog_info("%s enable_flag is %d\n", __func__, enable_flag);
		return SET_DCDTOUT_SUCC;
	}
	rt8979_dcd_timeout_enabled = enable_flag;
	if (enable_flag) {
		ret = fsa9685_write_reg_mask(RT8979_REG_TIMING_SET_2,
			0, RT8979_REG_TIMING_SET_2_DCDTIMEOUT);
		if (ret < 0) {
			hwlog_err("%s write RT8979_REG_TIMING_SET_2 error\n",
				__func__);
			return SET_DCDTOUT_FAIL;
		}
	} else {
		ret = fsa9685_write_reg_mask(RT8979_REG_TIMING_SET_2,
			RT8979_REG_TIMING_SET_2_DCDTIMEOUT,
			RT8979_REG_TIMING_SET_2_DCDTIMEOUT);
		if (ret < 0)
			return SET_DCDTOUT_FAIL;
	}
	ret = fsa9685_write_reg_mask(FSA9685_REG_CONTROL2,
		FSA9685_DCD_TIME_OUT_MASK, FSA9685_DCD_TIME_OUT_MASK);
	if (ret < 0) {
		hwlog_err("%s write FSA9685_REG_CONTROL2 error\n", __func__);
		return SET_DCDTOUT_FAIL;
	}
	hwlog_info("%s enable_flag is %d\n", __func__, enable_flag);
	return SET_DCDTOUT_SUCC;
}

int fsa9685_dcd_timeout_status(void)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di) {
		hwlog_err("error: di is null\n");
		return SET_DCDTOUT_FAIL;
	}

	return di->dcd_timeout_force_enable;
}

static void fsa9685_intb_work(struct work_struct *work);
static irqreturn_t fsa9685_irq_handler(int irq, void *dev_id)
{
	int gpio_value;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return IRQ_HANDLED;
	}

	if (!di->pd_support)
		fsa9685_usb_switch_wake_lock();

	gpio_value = gpio_get_value(gpio);
	if (gpio_value == 1) /* 1:irq gpio is high */
		hwlog_err("%s: intb high when interrupt occured\n", __func__);

	schedule_work(&di->g_intb_work);

	hwlog_info("%s:end gpio_value=%d\n", __func__, gpio_value);
	return IRQ_HANDLED;
}

int is_fcp_charger_type(void *dev_data)
{
	int reg_val;

	if (is_support_fcp())
		return 0;

	reg_val = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_4);

	if (reg_val < 0) {
		hwlog_err("%s read FSA9685_REG_DEVICE_TYPE_4 error\n", __func__);
		return 0;
	}
	if (reg_val & FSA9685_ACCP_CHARGER_DET)
		return 1;

	return 0;
}

static unsigned int fsa9685_get_charger_type(void)
{
	unsigned int chg_type = CHARGER_REMOVED;
	int val;
	int usb_status;
	int muic_status1;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return chg_type;
	}

	val = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
	if (val < 0) {
		hwlog_err("%s read FSA9685_REG_DEVICE_TYPE_1 error\n", __func__);
		return chg_type;
	}

	if (is_rt8979()) {
		muic_status1 = fsa9685_read_reg(RT8979_REG_MUIC_STATUS1);
		usb_status = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
		hwlog_info("%s muic_status1:0x%x usb_status:0x%x\n",
			__func__, muic_status1, usb_status);
		if (usb_status != val)
			chg_type = CHARGER_REMOVED;
		else if (muic_status1 & RT8979_DCDT)
			chg_type = CHARGER_REMOVED;
		else if (val & FSA9685_USB_DETECTED)
			chg_type = CHARGER_TYPE_USB;
		else if (val & FSA9685_CDP_DETECTED)
			chg_type = CHARGER_TYPE_BC_USB;
		else if (val & FSA9685_DCP_DETECTED)
			chg_type = CHARGER_TYPE_STANDARD;
		else
			chg_type = CHARGER_REMOVED;
	} else {
		if (val & FSA9685_USB_DETECTED)
			chg_type = CHARGER_TYPE_USB;
		else if (val & FSA9685_CDP_DETECTED)
			chg_type = CHARGER_TYPE_BC_USB;
		else if (val & FSA9685_DCP_DETECTED)
			chg_type = CHARGER_TYPE_STANDARD;
		else
			chg_type = CHARGER_TYPE_NON_STANDARD;
	}

	if ((chg_type == CHARGER_REMOVED) && is_fcp_charger_type(di)) {
		chg_type = CHARGER_TYPE_STANDARD;
		hwlog_info("%s:update by device type4 charger type is %u\n",
			__func__, chg_type);
	}

	hwlog_info("%s chg_type = %u\n", __func__, chg_type);
	return chg_type;
}

int rt8979_psy_online_changed(struct fsa9685_device_info *di)
{
	int ret = 0;
	union power_supply_propval propval;

	/* Get chg type det power supply */
	if (!di->ac_psy)
		di->ac_psy = power_supply_get_by_name("usb");
	if (!di->ac_psy) {
		hwlog_info("%s: get ac power supply failed\n", __func__);
		return -EINVAL;
	}

	if (!di->usb_psy)
		di->usb_psy = power_supply_get_by_name("pc_port");
	if (!di->usb_psy) {
		hwlog_info("%s: get usb power supply failed\n", __func__);
		return -EINVAL;
	}

	propval.intval = di->attach;
	if (di->attach == 0) {
		ret = power_supply_set_property(di->ac_psy, POWER_SUPPLY_PROP_ONLINE, &propval);
		if (ret < 0)
			hwlog_err("%s: ac psy online fail %d\n", __func__, ret);
		ret += power_supply_set_property(di->usb_psy, POWER_SUPPLY_PROP_ONLINE, &propval);
		if (ret < 0)
			hwlog_err("%s: usb psy online fail %d\n", __func__, ret);

		power_supply_changed(di->ac_psy);
		power_supply_changed(di->usb_psy);
		return ret;
	}

	if ((di->chg_type == CHARGER_TYPE_USB) || (di->chg_type == CHARGER_TYPE_BC_USB)) {
		ret = power_supply_set_property(di->usb_psy, POWER_SUPPLY_PROP_ONLINE, &propval);
		power_supply_changed(di->usb_psy);
	} else {
		ret = power_supply_set_property(di->ac_psy, POWER_SUPPLY_PROP_ONLINE, &propval);
		power_supply_changed(di->ac_psy);
	}
	if (ret < 0)
		hwlog_err("%s: psy online fail%d\n", __func__, ret);
	else
		hwlog_info("%s: pwr_rdy = %d\n",  __func__, di->attach);

	return ret;
}

int rt8979_psy_chg_type_changed(struct fsa9685_device_info *di)
{
	int ret = 0;
	union power_supply_propval propval;

	/* Get chg type det power supply */
	if (!di->psy)
		di->psy = power_supply_get_by_name("usb");
	if (!di->psy) {
		hwlog_info("%s: get power supply failed\n", __func__);
		return -EINVAL;
	}

	propval.intval = di->chg_type;
	ret = power_supply_set_property(di->psy, POWER_SUPPLY_PROP_REAL_TYPE, &propval);
	if (ret < 0)
		hwlog_err("%s: psy type failed, ret = %d\n", __func__, ret);
	else
		hwlog_info("%s: chg_type = %d\n", __func__, di->chg_type);

	return ret;
}

void hw_usb_psy_force_change_type_to_ac(void)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (di) {
		di->chg_type = CHARGER_TYPE_STANDARD;
		rt8979_psy_chg_type_changed(di);
	}
}

static void fsa9685_chg_det_work(struct work_struct *work)
{
	int ret = 0;
	unsigned int chg_type = CHARGER_REMOVED;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di)
		return;

	/* attach */
	di->attach = 1;
	chg_type = fsa9685_get_charger_type();
	if (chg_type == CHARGER_TYPE_USB) {
		/* work redo bc12 */
		schedule_delayed_work(&di->update_type_work, msecs_to_jiffies(DELAY_FOR_SDP_RETRY));
		/* notify online */
		ret = rt8979_psy_online_changed(di);
		if (ret < 0)
			hwlog_err("%s: sdp online report fail\n", __func__);

		hwlog_info("%s: sdp schedule work to redo bc12\n", __func__);
		return;
	} else if (chg_type == CHARGER_REMOVED) {
		goto chg_det_fail;
	} else if (chg_type == CHARGER_TYPE_NON_STANDARD) {
		hwlog_info("%s: bc12 g_non_standard_retrycnt:%d\n", __func__,
			g_non_standard_retrycnt);
		if (g_non_standard_retrycnt < FSA9688_NON_STANDARD_RETRY_MAX) {
			schedule_delayed_work(&di->chg_det_work,
				msecs_to_jiffies(DELAY_FOR_BC12_REG));
			g_non_standard_retrycnt++;
			return;
		}
	}

	g_non_standard_retrycnt = 0;
	di->chg_type = chg_type;
	/* notify online */
	ret = rt8979_psy_online_changed(di);
	if (ret < 0)
		goto chg_det_fail;

	ret = rt8979_psy_chg_type_changed(di);
	if (ret < 0)
		goto chg_det_fail;

	hwlog_info("%s: charger type:%d\n", __func__, di->chg_type);
	return;
chg_det_fail:
	hwlog_err("%s: chg type err\n", __func__);
	/* release usb_phy to soc */
	rt8979_set_usbsw_state(1);
	return;
}

static int fsa9685_chg_type_det(bool en)
{
	int ret;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di)
		return -EINVAL;

	atomic_set(&di->chg_type_det, en);
	/* attach */
	if (en) {
		rt8979_set_usbsw_state(0); /* set usb_phy chg_state */
		schedule_delayed_work(&di->chg_det_work, msecs_to_jiffies(DELAY_FOR_BC12_REG));
		hwlog_info("%s: schedule work to get bc12 result\n", __func__);
		return 0;
	}

	cancel_delayed_work_sync(&di->chg_det_work);
	cancel_delayed_work_sync(&di->update_type_work);

	/* detach */
	hwlog_info("%s: detach\n", __func__);
	rt8979_set_usbsw_state(1); /* release usb_phy to soc */
	di->attach = 0;
	di->chg_type = CHARGER_REMOVED;
	ret = rt8979_psy_online_changed(di);
	if (ret < 0)
		goto psy_notify_fail;
	ret = rt8979_psy_chg_type_changed(di);
	if (ret < 0)
		goto psy_notify_fail;

	return 0;
psy_notify_fail:
	hwlog_err("%s: chg type err\n", __func__);
	return -1;
}

static int usb_switch_chg_type_det(bool en)
{
	if (is_rt8979()) {
		return rt8979_enable_chg_type_det(g_fsa9685_dev, en);
	} else {
		return fsa9685_chg_type_det(en);
	}
}

static void fsa9685_resume_irq(void)
{
	int reg_ctl;
	int ret;

	/* clear INT MASK */
	reg_ctl = fsa9685_read_reg(FSA9685_REG_CONTROL);
	if (reg_ctl < 0) {
		hwlog_err("%s: read reg FSA9685_REG_CONTROL failed\n", __func__);
		return;
	}
	hwlog_info("%s: read FSA9685_REG_CONTROL. reg_ctl=0x%x\n", __func__, reg_ctl);

	reg_ctl &= (~FSA9685_INT_MASK);
	ret = fsa9685_write_reg(FSA9685_REG_CONTROL, reg_ctl);
	hwlog_info("%s: write FSA9685_REG_CONTROL. reg_ctl=0x%x, ret=%d\n",
		__func__, reg_ctl, ret);

	/* 0x0c: DMD status init */
	ret = fsa9685_write_reg(FSA9685_REG_DCD, 0x0c);
	hwlog_info("%s: write FSA9685_REG_DCD. reg_DCD=0x%x, ret=%d\n", __func__, 0x0c, ret);

	/* disable dcd time out */
	ret = fsa9685_write_reg_mask(FSA9685_REG_CONTROL2, 0, FSA9685_DCD_TIME_OUT_MASK);
	hwlog_info("%s: dcd time out 0x0e=0x%x\n", __func__,
		fsa9685_read_reg(FSA9685_REG_CONTROL2));
}

extern void rt8979_set_usbsw_state(int state);
static void fsa9685_update_type_work(struct work_struct *work)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;
	unsigned int chg_type = CHARGER_REMOVED;

	if (!di)
		return;

	hwlog_info("%s: +\n", __func__);
	rt8979_set_usbsw_state(0); /* set usb_phy chg_state */
	fsa9685_write_reg(FSA9685_REG_RESET, FSA9685_RESET_CHIP);
	msleep(DELAY_FOR_RESET);

	chg_type = fsa9685_get_charger_type();
	if (chg_type != CHARGER_REMOVED) {
		di->chg_type = chg_type;
		rt8979_psy_chg_type_changed(di);
		/* notify online */
		rt8979_psy_online_changed(di);
	}
	fsa9685_resume_irq();
	hwlog_info("%s: -\n", __func__);
}

/****************************************************************************
  Function:     is_usb_id_abnormal
  Description:  check whether is usb id abnormal
  Input:        void
  Output:       NA
  Return:       1 -- abnormal ; 0 -- normal
***************************************************************************/
static int fsa9685_is_water_intruded(void *dev_data)
{
	u8 usb_id;
	int i;
	unsigned int charger_type = mt_get_charger_type();

	if (charger_type != CHARGER_TYPE_NON_STANDARD) {
		hwlog_info("water detect with usbid must be non standard\n");
		return 0;
	}

	for (i = 0; i < 5; i++) {
		usb_id = fsa9685_read_reg(FSA9685_REG_ADC);
		usb_id &= FSA9685_REG_ADC_VALUE_MASK;
		hwlog_info("usb id is 0x%x\n", usb_id);
		if (!((usb_id >= WATER_CHECK_MIN_ID) &&
			(usb_id <= WATER_CHECK_MAX_ID) && (usb_id != 0x0B)))
			return 0;
		msleep(100);
	}
	return 1;
}

static int rt8979_sw_open(bool open)
{
	hwlog_info("%s : %s\n", __func__, open ? "open" : "auto");
	return fsa9685_write_reg_mask(FSA9685_REG_CONTROL,
		open ? 0 : FSA9685_SWITCH_OPEN, FSA9685_SWITCH_OPEN);
}

static int rt8979_accp_enable(bool en)
{
	return fsa9685_write_reg_mask(FSA9685_REG_CONTROL2, en ?
		FSA9685_ACCP_AUTO_ENABLE : 0, FSA9685_ACCP_AUTO_ENABLE);
}

static void rt8979_auto_restart_accp_det(void)
{
	int ret;

	hwlog_info("%s-%d +++\n", __func__, __LINE__);

	ret = fsa9685_read_reg(FSA9685_REG_CONTROL2);
	if (ret < 0) {
		hwlog_err("fsa9685_read_reg fail\n");
		return;
	}

	if ((ret & FSA9685_ACCP_AUTO_ENABLE) == 0) {
		hwlog_info("%s-%d do restart accp det\n", __func__, __LINE__);
		rt8979_sw_open(true);
		rt8979_accp_enable(true);
		msleep(30);
		fsa9685_write_reg_mask(RT8979_REG_USBCHGEN, 0, RT8979_REG_USBCHGEN_ACCPDET_STAGE1);
		fsa9685_write_reg_mask(RT8979_REG_MUIC_CTRL, 0, RT8979_REG_MUIC_CTRL_DISABLE_DCDTIMEOUT);
		fsa9685_write_reg_mask(RT8979_REG_USBCHGEN,
			RT8979_REG_USBCHGEN_ACCPDET_STAGE1, RT8979_REG_USBCHGEN_ACCPDET_STAGE1);
		msleep(300);
		rt8979_sw_open(false);
		fsa9685_write_reg_mask(RT8979_REG_MUIC_CTRL,
			RT8979_REG_MUIC_CTRL_DISABLE_DCDTIMEOUT, RT8979_REG_MUIC_CTRL_DISABLE_DCDTIMEOUT);
	}
	hwlog_info("%s-%d ---\n", __func__, __LINE__);
}

static void rt8979_force_restart_accp_det(bool open)
{
	rt8979_sw_open(true);
	rt8979_accp_enable(true);
	usleep_range(950, 1050); /* 1ms */
	fsa9685_write_reg_mask(RT8979_REG_USBCHGEN, 0,
		RT8979_REG_USBCHGEN_ACCPDET_STAGE1);
	fsa9685_write_reg_mask(RT8979_REG_MUIC_CTRL, 0,
		RT8979_REG_MUIC_CTRL_DISABLE_DCDTIMEOUT);
	fsa9685_write_reg_mask(RT8979_REG_USBCHGEN,
		RT8979_REG_USBCHGEN_ACCPDET_STAGE1,
		RT8979_REG_USBCHGEN_ACCPDET_STAGE1);
	msleep(300);  /* sleep 300 ms */
	rt8979_sw_open(open);
	fsa9685_write_reg_mask(RT8979_REG_MUIC_CTRL,
		RT8979_REG_MUIC_CTRL_DISABLE_DCDTIMEOUT,
		RT8979_REG_MUIC_CTRL_DISABLE_DCDTIMEOUT);
}

void rt8979_dcp_accp_init(void)
{
	/* should clear accp reg */
	fsa9685_write_reg(FSA9685_REG_ACCP_CMD, RT8979_REG_ACCP_CMD_STAGE1);
	fsa9685_write_reg(FSA9685_REG_ACCP_ADDR, RT8979_REG_ACCP_ADDR_VAL1);
	rt8979_osc_trim_adjust = rt8979_osc_trim_default;
	rt8979_adjust_osc(0);
}

static void rt8979_intb_work(struct work_struct *work)
{
	int reg_ctl, reg_intrpt, reg_adc, reg_dev_type1, reg_dev_type2, reg_dev_type3, vbus_status;
	int ret = -1;
	int ret2 = 0;
	int muic_status1, muic_status2;
	int id_valid_status = ID_VALID;
	int usb_switch_wakelock_flag = USB_SWITCH_NEED_WAKE_UNLOCK;
	static int invalid_times;
	static int otg_attach;
	static int pedestal_attach;
	static bool redo_bc12;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if ((!di) || (!di->client)) {
		hwlog_err("error: di or client is null\n");
		return;
	}

	reg_intrpt = fsa9685_read_reg(FSA9685_REG_INTERRUPT);
	vbus_status = fsa9685_read_reg(FSA9685_REG_VBUS_STATUS);
	hwlog_info("%s: reg_intrpt=0x%x, vbus_status=0x%x\n",
		__func__, reg_intrpt, vbus_status);
	muic_status1 = fsa9685_read_reg(RT8979_REG_MUIC_STATUS1); /* Patrick Added, 2017/4/19 */
	muic_status2 = fsa9685_read_reg(RT8979_REG_MUIC_STATUS2); /* Patrick Added, 2017/4/25 */
	hwlog_info("%s: read MUIC STATUS1. reg_status=0x%x\n", __func__, muic_status1);
	if (muic_status1 & RT8979_DCDT) {
		hwlog_info("%s: DCD timoeut (DCT enable = %s)\n",
			__func__, rt8979_dcd_timeout_enabled ? "true" : "false");
		if (!rt8979_dcd_timeout_enabled) {
			hwlog_info("%s: DCD timoeut -> redo USB detection\n", __func__);
			rt8979_accp_enable(false);
			fsa9685_write_reg(FSA9685_REG_INTERRUPT_MASK, 0); /* enable all interrupt, 2017/4/25 */
			fsa9685_write_reg_mask(RT8979_REG_USBCHGEN, 0, RT8979_REG_USBCHGEN_ACCPDET_STAGE1);
			fsa9685_write_reg_mask(RT8979_REG_TIMING_SET_2, 0x38, 0x38);
			fsa9685_write_reg_mask(RT8979_REG_USBCHGEN,
				RT8979_REG_USBCHGEN_ACCPDET_STAGE1, RT8979_REG_USBCHGEN_ACCPDET_STAGE1);
			redo_bc12 = true;
			goto OUT;
		}
	}

	muic_status2 >>= 4;
	muic_status2 &= 0x07;
	if (redo_bc12) {
		switch (muic_status2) {
		case 2: /* SDP */
			reg_intrpt |= FSA9685_ATTACH;
			break;
		case 5: /* CDP */
			reg_intrpt |= FSA9685_ATTACH;
			break;
		case 4: /* DCP */
			reg_intrpt |= FSA9685_ATTACH;
		}
		redo_bc12 = false;
	}
	if (!is_support_fcp() &&
		((RT8979_REG_ALLMASK != fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT_MASK1)) ||
		(RT8979_REG_ALLMASK != fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT_MASK2)))) {
		hwlog_info("disable fcp interrrupt again\n");
		ret2 |= fsa9685_write_reg_mask(FSA9685_REG_CONTROL2, 0, FSA9685_ACCP_OSC_ENABLE);
		if (!is_rt8979())
			ret2 |= fsa9685_write_reg_mask(FSA9685_REG_CONTROL2,
				di->dcd_timeout_force_enable, FSA9685_DCD_TIME_OUT_MASK);
		ret2 |= fsa9685_write_reg(FSA9685_REG_ACCP_INTERRUPT_MASK1, RT8979_REG_ALLMASK);
		ret2 |= fsa9685_write_reg(FSA9685_REG_ACCP_INTERRUPT_MASK2, RT8979_REG_ALLMASK);
		hwlog_info("%s : read ACCP interrupt,reg[0x59]=0x%x,reg[0x5A]=0x%x\n", __func__,
			fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1),
			fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2));
		if (ret2 < 0)
			hwlog_err("accp interrupt mask write failed\n");
	}
	if (unlikely(reg_intrpt < 0)) {
		hwlog_err("%s: read FSA9685_REG_INTERRUPT error\n", __func__);
	} else if (unlikely(reg_intrpt == 0)) {
		hwlog_err("%s: read FSA9685_REG_INTERRUPT, and no intr\n", __func__);
	} else {
		if (reg_intrpt & FSA9685_DEVICE_CHANGE)
			hwlog_info("%s: Device Change\n", __func__);

		if ((reg_intrpt & FSA9685_ATTACH) && !rt8979_is_in_fm8()) {
			hwlog_info("%s: FSA9685_ATTACH\n", __func__);
			rt8979_sw_open(false);
			fsa9685_write_reg(FSA9685_REG_ACCP_CMD, RT8979_REG_ACCP_CMD_STAGE1);
			fsa9685_write_reg(FSA9685_REG_ACCP_ADDR, RT8979_REG_ACCP_ADDR_VAL1);
			rt8979_osc_trim_adjust = rt8979_osc_trim_default;
			rt8979_adjust_osc(0);
			reg_dev_type1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
			reg_dev_type2 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_2);
			reg_dev_type3 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_3);
			hwlog_info("%s: reg_dev_type1=0x%X, reg_dev_type2=0x%X, reg_dev_type3= 0x%X\n", __func__,
				reg_dev_type1, reg_dev_type2, reg_dev_type3);
			if (reg_dev_type1 & FSA9685_FC_USB_DETECTED)
				hwlog_info("%s: FSA9685_FC_USB_DETECTED\n", __func__);

			if (reg_dev_type1 & FSA9685_USB_DETECTED) {
				hwlog_info("%s: FSA9685_USB_DETECTED\n", __func__);
				if (FSA9685_USB2_ID_TO_IDBYPASS == get_swstate_value()) {
					switch_usb2_access_through_ap();
					hwlog_info("%s: fsa9685 switch to USB2 by setvalue\n", __func__);
				}
			}

			if (reg_dev_type1 & FSA9685_UART_DETECTED)
				hwlog_info("%s: FSA9685_UART_DETECTED\n", __func__);

			if (reg_dev_type1 & FSA9685_MHL_DETECTED) {
				if (di->mhl_detect_disable == 1) {
					hwlog_info("%s: mhl detection is not enabled on this platform,regard as an invalid detection\n",
						__func__);
					id_valid_status = ID_INVALID;
				} else {
					hwlog_info("%s: FSA9685_MHL_DETECTED\n", __func__);
				}
			}

			if (reg_dev_type1 & FSA9685_CDP_DETECTED)
				hwlog_info("%s: FSA9685_CDP_DETECTED\n", __func__);

			if (reg_dev_type1 & FSA9685_DCP_DETECTED) {
				hwlog_info("%s: FSA9685_DCP_DETECTED\n", __func__);
				charge_type_dcp_detected_notify();
			}

			if ((reg_dev_type1 & FSA9685_USBOTG_DETECTED) && di->usbid_enable) {
				hwlog_info("%s: FSA9685_USBOTG_DETECTED\n", __func__);
				otg_attach = 1;
				usb_switch_wakelock_flag = USB_SWITCH_NEED_WAKE_LOCK;
				hisi_usb_id_change(ID_FALL_EVENT);
			}

			if (reg_dev_type1 & FSA9685_DEVICE_TYPE1_UNAVAILABLE) {
				id_valid_status = ID_INVALID;
				hwlog_info("%s: FSA9685_DEVICE_TYPE1_UNAVAILABLE_DETECTED\n", __func__);
			}

			if (reg_dev_type2 & FSA9685_JIG_UART)
				hwlog_info("%s: FSA9685_JIG_UART\n", __func__);

			if (reg_dev_type2 & FSA9685_DEVICE_TYPE2_UNAVAILABLE) {
				id_valid_status = ID_INVALID;
				hwlog_info("%s: FSA9685_DEVICE_TYPE2_UNAVAILABLE_DETECTED\n", __func__);
			}

			if (reg_dev_type3 & FSA9685_CUSTOMER_ACCESSORY7) {
				usbswitch_common_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
				hwlog_info("%s:Enter FSA9685_CUSTOMER_ACCESSORY7\n", __func__);
			}

			if (reg_dev_type3 & FSA9685_CUSTOMER_ACCESSORY5) {
				hwlog_info("%s: FSA9685_CUSTOMER_ACCESSORY5, 365K\n", __func__);
				pedestal_attach = 1;
			}

			if (reg_dev_type3 & FSA9685_FM8_ACCESSORY) {
				hwlog_info("%s: FSA9685_FM8_DETECTED\n", __func__);
				usbswitch_common_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
			}

			if (reg_dev_type3 & FSA9685_DEVICE_TYPE3_UNAVAILABLE) {
				id_valid_status = ID_INVALID;
				if (reg_intrpt & FSA9685_VBUS_CHANGE)
					usbswitch_common_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
				hwlog_info("%s: FSA9685_DEVICE_TYPE3_UNAVAILABLE_DETECTED\n", __func__);
			}
		}

		if (reg_intrpt & FSA9685_RESERVED_ATTACH) {
			id_valid_status = ID_INVALID;
			if (reg_intrpt & FSA9685_VBUS_CHANGE)
				usbswitch_common_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
			hwlog_info("%s: FSA9685_RESERVED_ATTACH\n", __func__);
		}

		if (reg_intrpt & FSA9685_DETACH) {
			hwlog_info("%s: FSA9685_DETACH\n", __func__);
			rt8979_accp_enable(true);
			reg_ctl = fsa9685_read_reg(FSA9685_REG_CONTROL);
			reg_dev_type2 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_2);
			hwlog_info("%s: reg_ctl=0x%x\n", __func__, reg_ctl);
			if (reg_ctl < 0) {
				hwlog_err("%s: read FSA9685_REG_CONTROL error!!! reg_ctl=%d\n",
					__func__, reg_ctl);
				goto OUT;
			}

			if (((reg_ctl & FSA9685_MANUAL_SW) == 0) && !rt8979_is_in_fm8()) {
				reg_ctl |= FSA9685_MANUAL_SW;
				ret = fsa9685_write_reg(FSA9685_REG_CONTROL, reg_ctl);
				if (ret < 0) {
					hwlog_err("%s: write FSA9685_REG_CONTROL error\n", __func__);
					goto OUT;
				}
			}

			if ((otg_attach == 1) && di->usbid_enable) {
				hwlog_info("%s: FSA9685_USBOTG_DETACH\n", __func__);
				hisi_usb_id_change(ID_RISE_EVENT);
				otg_attach = 0;
			}

			if (pedestal_attach == 1) {
				hwlog_info("%s: FSA9685_CUSTOMER_ACCESSORY5_DETACH\n", __func__);
				pedestal_attach = 0;
			}

			if (reg_dev_type2 & FSA9685_JIG_UART)
				hwlog_info("%s: FSA9685_JIG_UART\n", __func__);
		}

		if (reg_intrpt & FSA9685_VBUS_CHANGE)
			hwlog_info("%s: FSA9685_VBUS_CHANGE\n", __func__);

		if (reg_intrpt & FSA9685_ADC_CHANGE) {
			reg_adc = fsa9685_read_reg(FSA9685_REG_ADC);
			hwlog_info("%s: FSA9685_ADC_CHANGE. reg_adc=%d\n", __func__, reg_adc);
			if (reg_adc < 0)
				hwlog_err("%s: read FSA9685_ADC_CHANGE error!!! reg_adc=%d\n",
					__func__, reg_adc);
		}
	}

	if ((id_valid_status == ID_INVALID) &&
				(reg_intrpt & (FSA9685_ATTACH | FSA9685_RESERVED_ATTACH))) {
		invalid_times++;
		hwlog_info("%s: invalid time:%d reset fsa9685 work\n", __func__, invalid_times);
		if (invalid_times < MAX_DETECTION_TIMES) {
			hwlog_info("%s: start schedule delayed work\n", __func__);
			schedule_delayed_work(&di->detach_delayed_work, msecs_to_jiffies(0));
		} else {
			invalid_times = 0;
		}
	} else if ((id_valid_status == ID_VALID) &&
				(reg_intrpt & (FSA9685_ATTACH | FSA9685_RESERVED_ATTACH))) {
		invalid_times = 0;
	}
OUT:
	if (!di->pd_support &&
		(usb_switch_wakelock_flag == USB_SWITCH_NEED_WAKE_UNLOCK) && (invalid_times == 0))
		fsa9685_usb_switch_wake_unlock();

	hwlog_info("%s: ------end\n", __func__);
	return;
}

static void fsa9685_update_nstd_chg_type(struct fsa9685_device_info *di,
	unsigned int chg_type)
{
	if (di->chg_type == CHARGER_TYPE_NON_STANDARD) {
		hwlog_info("%s: update chg_type %d to %d\n",
			__func__, di->chg_type, chg_type);
		di->chg_type = chg_type;
		if (chg_type == CHARGER_TYPE_USB) {
			hwlog_info("%s: if sdp recheck type\n", __func__);
			schedule_delayed_work(&di->update_type_work, msecs_to_jiffies(0));
		} else {
			rt8979_psy_chg_type_changed(di);
		}
	}
}

static void fsa9685_intb_work(struct work_struct *work)
{
	int reg_ctl, reg_intrpt, reg_adc, reg_dev_type1, reg_dev_type2, reg_dev_type3, vbus_status;
	int ret = -1;
	int ret2 = 0;
	int id_valid_status = ID_VALID;
	int usb_switch_wakelock_flag = USB_SWITCH_NEED_WAKE_UNLOCK;
	static int invalid_times;
	static int otg_attach;
	static int pedestal_attach;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if ((!di) || (!di->client)) {
		hwlog_err("error: di or client is null\n");
		return;
	}

	reg_intrpt = fsa9685_read_reg(FSA9685_REG_INTERRUPT);
	vbus_status = fsa9685_read_reg(FSA9685_REG_VBUS_STATUS);
	hwlog_info("%s: reg_intrpt=0x%x, vbus_status=0x%x\n",
		__func__, reg_intrpt, vbus_status);
	/* if support fcp ,disable fcp interrupt */
	if (!is_support_fcp() &&
		((fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT_MASK1) != 0xFF) ||
		(fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT_MASK2) != 0xFF))) {
		hwlog_info("disable fcp interrrupt again\n");
		ret2 |= fsa9685_write_reg_mask(FSA9685_REG_CONTROL2,
			FSA9685_ACCP_OSC_ENABLE, FSA9685_ACCP_OSC_ENABLE);
		ret2 |= fsa9685_write_reg_mask(FSA9685_REG_CONTROL2,
			di->dcd_timeout_force_enable, FSA9685_DCD_TIME_OUT_MASK);
		ret2 |= fsa9685_write_reg(FSA9685_REG_ACCP_INTERRUPT_MASK1, 0xFF);
		ret2 |= fsa9685_write_reg(FSA9685_REG_ACCP_INTERRUPT_MASK2, 0xFF);
		hwlog_info("%s : read ACCP interrupt,reg[0x59]=0x%x,reg[0x5A]=0x%x\n", __func__,
			fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1),
			fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2));
		if (ret2 < 0)
			hwlog_err("accp interrupt mask write failed\n");
	}

	if (unlikely(reg_intrpt < 0)) {
		hwlog_err("%s: read FSA9685_REG_INTERRUPT error\n", __func__);
	} else if (unlikely(reg_intrpt == 0)) {
		hwlog_err("%s: read FSA9685_REG_INTERRUPT, and no intr\n", __func__);
	} else {
		if (reg_intrpt & FSA9685_ATTACH) {
			hwlog_info("%s: FSA9685_ATTACH\n", __func__);
			reg_dev_type1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
			reg_dev_type2 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_2);
			reg_dev_type3 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_3);
			hwlog_info("%s: reg_dev_type1=0x%X, reg_dev_type2=0x%X, reg_dev_type3= 0x%X\n",
				__func__, reg_dev_type1, reg_dev_type2, reg_dev_type3);
			if (reg_dev_type1 & FSA9685_FC_USB_DETECTED)
				hwlog_info("%s: FSA9685_FC_USB_DETECTED\n", __func__);

			if (reg_dev_type1 & FSA9685_USB_DETECTED) {
				hwlog_info("%s: FSA9685_USB_DETECTED\n", __func__);
				if (FSA9685_USB2_ID_TO_IDBYPASS == get_swstate_value()) {
					switch_usb2_access_through_ap();
					hwlog_info("%s: fsa9685 switch to USB2 by setvalue\n", __func__);
				}
				fsa9685_update_nstd_chg_type(di, CHARGER_TYPE_USB);
			}
			if (reg_dev_type1 & FSA9685_UART_DETECTED)
				hwlog_info("%s: FSA9685_UART_DETECTED\n", __func__);

			if (reg_dev_type1 & FSA9685_MHL_DETECTED) {
				if (di->mhl_detect_disable == 1) {
					hwlog_info("%s: mhl detection is not enabled on this platform,
						regard as an invalid detection\n", __func__);
					id_valid_status = ID_INVALID;
				} else {
					hwlog_info("%s: FSA9685_MHL_DETECTED\n", __func__);
				}
			}
			if (reg_dev_type1 & FSA9685_CDP_DETECTED) {
				hwlog_info("%s: FSA9685_CDP_DETECTED\n", __func__);
				fsa9685_update_nstd_chg_type(di, CHARGER_TYPE_BC_USB);
			}

			if (reg_dev_type1 & FSA9685_DCP_DETECTED) {
				hwlog_info("%s: FSA9685_DCP_DETECTED\n", __func__);
				fsa9685_update_nstd_chg_type(di, CHARGER_TYPE_STANDARD);
			}

			if ((reg_dev_type1 & FSA9685_USBOTG_DETECTED) && di->usbid_enable) {
				hwlog_info("%s: FSA9685_USBOTG_DETECTED\n", __func__);
				otg_attach = 1;
				usb_switch_wakelock_flag = USB_SWITCH_NEED_WAKE_LOCK;
				hisi_usb_id_change(ID_FALL_EVENT);
			}

			if (reg_dev_type1 & FSA9685_DEVICE_TYPE1_UNAVAILABLE) {
				id_valid_status = ID_INVALID;
				hwlog_info("%s: FSA9685_DEVICE_TYPE1_UNAVAILABLE_DETECTED\n", __func__);
			}

			if (reg_dev_type2 & FSA9685_JIG_UART)
				hwlog_info("%s: FSA9685_JIG_UART\n", __func__);

			if (reg_dev_type2 & FSA9685_DEVICE_TYPE2_UNAVAILABLE) {
				id_valid_status = ID_INVALID;
				hwlog_info("%s: FSA9685_DEVICE_TYPE2_UNAVAILABLE_DETECTED\n", __func__);
			}

			if (reg_dev_type3 & FSA9685_CUSTOMER_ACCESSORY7) {
				usbswitch_common_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
				hwlog_info("%s:Enter FSA9685_CUSTOMER_ACCESSORY7\n", __func__);
			}

			if (reg_dev_type3 & FSA9685_CUSTOMER_ACCESSORY5) {
				hwlog_info("%s: FSA9685_CUSTOMER_ACCESSORY5, 365K\n", __func__);
				pedestal_attach = 1;
			}

			if (reg_dev_type3 & FSA9685_FM8_ACCESSORY) {
				hwlog_info("%s: FSA9685_FM8_DETECTED\n", __func__);
				usbswitch_common_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
			}

			if (reg_dev_type3 & FSA9685_DEVICE_TYPE3_UNAVAILABLE) {
				id_valid_status = ID_INVALID;
				if (reg_intrpt & FSA9685_VBUS_CHANGE)
					usbswitch_common_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);

				hwlog_info("%s: FSA9685_DEVICE_TYPE3_UNAVAILABLE_DETECTED\n", __func__);
			}
		}

		if (reg_intrpt & FSA9685_RESERVED_ATTACH) {
			id_valid_status = ID_INVALID;
			if (reg_intrpt & FSA9685_VBUS_CHANGE)
				usbswitch_common_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);

			hwlog_info("%s: FSA9685_RESERVED_ATTACH\n", __func__);
		}

		if (reg_intrpt & FSA9685_DETACH) {
			hwlog_info("%s: FSA9685_DETACH\n", __func__);
			/* check control register, if manual switch, reset to auto switch */
			reg_ctl = fsa9685_read_reg(FSA9685_REG_CONTROL);
			reg_dev_type2 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_2);
			hwlog_info("%s: reg_ctl=0x%x\n", __func__, reg_ctl);
			if (reg_ctl < 0) {
				hwlog_err("%s: read FSA9685_REG_CONTROL error!!! reg_ctl=%d\n",
					__func__, reg_ctl);
				goto OUT;
			}

			if ((reg_ctl & FSA9685_MANUAL_SW) == 0) {
				reg_ctl |= FSA9685_MANUAL_SW;
				ret = fsa9685_write_reg(FSA9685_REG_CONTROL, reg_ctl);
				if (ret < 0) {
					hwlog_err("%s: write FSA9685_REG_CONTROL error\n", __func__);
					goto OUT;
				}
			}

			if ((otg_attach == 1) && di->usbid_enable) {
				hwlog_info("%s: FSA9685_USBOTG_DETACH\n", __func__);
				hisi_usb_id_change(ID_RISE_EVENT);
				otg_attach = 0;
			}

			if (pedestal_attach == 1) {
				hwlog_info("%s: FSA9685_CUSTOMER_ACCESSORY5_DETACH\n", __func__);
				pedestal_attach = 0;
			}

			if (reg_dev_type2 & FSA9685_JIG_UART)
				hwlog_info("%s: FSA9685_JIG_UART\n", __func__);
		}

		if (reg_intrpt & FSA9685_VBUS_CHANGE)
			hwlog_info("%s: FSA9685_VBUS_CHANGE\n", __func__);

		if (reg_intrpt & FSA9685_ADC_CHANGE) {
			reg_adc = fsa9685_read_reg(FSA9685_REG_ADC);
			hwlog_info("%s: FSA9685_ADC_CHANGE. reg_adc=%d\n", __func__, reg_adc);
			if (reg_adc < 0)
				hwlog_err("%s: read FSA9685_ADC_CHANGE error!!! reg_adc=%d\n",
					__func__, reg_adc);

			/* do user specific handle */
		}
	}

	if ((id_valid_status == ID_INVALID) &&
				(reg_intrpt & (FSA9685_ATTACH | FSA9685_RESERVED_ATTACH))) {
		invalid_times++;
		hwlog_info("%s: invalid time:%d reset fsa9685 work\n", __func__, invalid_times);

		if (invalid_times < MAX_DETECTION_TIMES) {
			hwlog_info("%s: start schedule delayed work\n", __func__);
			schedule_delayed_work(&di->detach_delayed_work, msecs_to_jiffies(0));
		} else {
			invalid_times = 0;
		}
	} else if ((id_valid_status == ID_VALID) &&
				(reg_intrpt & (FSA9685_ATTACH | FSA9685_RESERVED_ATTACH))) {
		invalid_times = 0;
	}

OUT:
	if (!di->pd_support &&
		(usb_switch_wakelock_flag == USB_SWITCH_NEED_WAKE_UNLOCK) && (invalid_times == 0))
		fsa9685_usb_switch_wake_unlock();

	hwlog_info("%s: ------end\n", __func__);
	return;
}

static ssize_t fsa9685_dump_regs_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fsa9685_device_ops *ops = g_fsa9685_dev_ops;

	if ((!ops) || (!ops->dump_regs)) {
		hwlog_err("error: ops is null or dump_regs is null\n");
		return -1;
	}

	return ops->dump_regs(buf);
}

static DEVICE_ATTR(dump_regs, S_IRUGO, fsa9685_dump_regs_show, NULL);

static ssize_t jigpin_ctrl_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;
	struct fsa9685_device_ops *ops = g_fsa9685_dev_ops;
	int jig_val = JIG_PULL_DEFAULT_DOWN;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return -1;
	}

	if (!ops || !ops->jigpin_ctrl_store) {
		hwlog_err("ops is null or jigpin_ctrl_store is null\n");
		return -1;
	}

	if (sscanf(buf, "%d", &jig_val) != 1) {
		hwlog_err("unable to parse input %s\n", buf);
		return -1;
	}

	return ops->jigpin_ctrl_store(di->client, jig_val);
}

static ssize_t jigpin_ctrl_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct fsa9685_device_ops *ops = g_fsa9685_dev_ops;

	if ((!ops) || (!ops->jigpin_ctrl_show)) {
		hwlog_err("error: ops is null or jigpin_ctrl_show is null\n");
		return -1;
	}

	return ops->jigpin_ctrl_show(buf);
}

static DEVICE_ATTR(jigpin_ctrl, S_IRUGO | S_IWUSR, jigpin_ctrl_show, jigpin_ctrl_store);

static ssize_t switchctrl_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;
	struct fsa9685_device_ops *ops = g_fsa9685_dev_ops;
	int action = 0;

	if (!di || !di->client) {
		hwlog_err("error: di or client is null\n");
		return -1;
	}

	if (!ops || !ops->switchctrl_store) {
		hwlog_err("error: ops is null or switchctrl_store is null\n");
		return -1;
	}

	if (sscanf(buf, "%d", &action) != 1) {
		hwlog_err("error: unable to parse input:%s\n", buf);
		return -1;
	}

	return ops->switchctrl_store(di->client, action);
}

static bool rt8979_is_in_fm8(void)
{
	int status;

	status = fsa9685_read_reg(RT8979_REG_MUIC_STATUS1);
	if (status < 0)
		return true;

	if (status & RT8979_REG_MUIC_STATUS1_FMEN)
		return true;

	return false;
}

static ssize_t switchctrl_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct fsa9685_device_ops *ops = g_fsa9685_dev_ops;

	if (!ops || !ops->switchctrl_show) {
		hwlog_err("ops is null or switchctrl_show is null\n");
		return -1;
	}

	return ops->switchctrl_show(buf);
}

static DEVICE_ATTR(switchctrl, S_IRUGO | S_IWUSR, switchctrl_show, switchctrl_store);

int fcp_adapter_reset(void *dev_data)
{
	int ret;
	int val;

	if (is_rt8979()) {
		fsa9685_write_reg_mask(FSA9685_REG_ACCP_CNTL, 0,
			FAS9685_ACCP_CNTL_MASK);
		val = FSA9685_ACCP_MSTR_RST;
	} else {
		val = FSA9685_ACCP_MSTR_RST | FAS9685_ACCP_SENDCMD |
			FSA9685_ACCP_IS_ENABLE;
	}

	ret = fsa9685_write_reg_mask(FSA9685_REG_ACCP_CNTL, val,
		FAS9685_ACCP_CNTL_MASK);
	hwlog_info("%s send fcp adapter reset %s\n", __func__,
		ret < 0 ? "fail" : "sucess");
	return ret;
}

static int fcp_stop_charge_config(void *dev_data)
{
#ifdef CONFIG_BOOST_5V
	int ret;
	int disable;
#endif

#ifdef CONFIG_DIRECT_CHARGER
	if (!direct_charge_get_cutoff_normal_flag()) {
#endif /* CONFIG_DIRECT_CHARGER */
#ifdef CONFIG_BOOST_5V
		if (g_fsa9685_dev->power_by_5v) {
			ret = boost_5v_enable(DISABLE, BOOST_CTRL_FCP);
			disable = dc_set_bst_ctrl(DISABLE);
			if (ret || disable) {
				hwlog_err("%s 5v boost close fail\n", __func__);
				return BOOST_5V_CLOSE_FAIL;
			}
			hwlog_info("%s 5v boost close\n", __func__);
		}
#endif /* CONFIG_BOOST_5V */
#ifdef CONFIG_DIRECT_CHARGER
	}
#endif /* CONFIG_DIRECT_CHARGER */
	return 0;
}

int switch_chip_reset(void *dev_data)
{
	int ret = 0;
	int reg_ctl = 0;
	int gpio_value = 0;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if ((!di) || (!di->client)) {
		hwlog_err("error: di or client is null\n");
		return -1;
	}

	hwlog_info("%s++", __func__);

	if (is_rt8979())
		disable_irq(di->irq);
	ret = fsa9685_write_reg(0x19, 0x89);
	if (ret < 0)
		hwlog_err("reset fsa9688 failed\n");

	if (is_rt8979()) {
		msleep(1);
		rt8979_accp_enable(true);
		fsa9685_write_reg_mask(RT8979_REG_TIMING_SET_2,
			RT8979_REG_TIMING_SET_2_DCDTIMEOUT, RT8979_REG_TIMING_SET_2_DCDTIMEOUT);
		rt8979_dcd_timeout_enabled = false;
	}

	ret = fsa9685_write_reg(FSA9685_REG_DETACH_CONTROL, 1);
	if (ret < 0)
		hwlog_err("%s: write FSA9685_REG_DETACH_CONTROL error!!! ret=%d\n", __func__, ret);

	/* disable accp interrupt */
	ret |= fsa9685_write_reg_mask(FSA9685_REG_CONTROL2,
		FSA9685_ACCP_OSC_ENABLE, FSA9685_ACCP_OSC_ENABLE);
	if (!is_rt8979())
		ret |= fsa9685_write_reg_mask(FSA9685_REG_CONTROL2,
			di->dcd_timeout_force_enable, FSA9685_DCD_TIME_OUT_MASK);

	ret += fsa9685_write_reg(FSA9685_REG_ACCP_INTERRUPT_MASK1, 0xFF);
	ret += fsa9685_write_reg(FSA9685_REG_ACCP_INTERRUPT_MASK2, 0xFF);
	if (ret < 0)
		hwlog_err("accp interrupt mask write failed\n");

	/* clear INT MASK */
	reg_ctl = fsa9685_read_reg(FSA9685_REG_CONTROL);
	if (reg_ctl < 0) {
		hwlog_err("%s: read FSA9685_REG_CONTROL error!!! reg_ctl=%d\n", __func__, reg_ctl);
		if (is_rt8979())
			enable_irq(di->irq);
		return -1;
	}

	hwlog_info("%s: read FSA9685_REG_CONTROL. reg_ctl=0x%x\n", __func__, reg_ctl);

	reg_ctl &= (~FSA9685_INT_MASK);
	ret = fsa9685_write_reg(FSA9685_REG_CONTROL, reg_ctl);
	if (ret < 0) {
		hwlog_err("%s: write FSA9685_REG_CONTROL error!!! reg_ctl=%d\n", __func__, reg_ctl);
		if (is_rt8979())
			enable_irq(di->irq);
		return -1;
	}

	hwlog_info("%s: write FSA9685_REG_CONTROL. reg_ctl=0x%x\n", __func__, reg_ctl);

	ret = fsa9685_write_reg(FSA9685_REG_DCD, 0x0c);
	if (ret < 0) {
		hwlog_err("%s: write FSA9685_REG_DCD error!!! reg_DCD=0x%x\n", __func__, 0x08);
		if (is_rt8979())
			enable_irq(di->irq);
		return -1;
	}

	hwlog_info("%s: write FSA9685_REG_DCD. reg_DCD=0x%x\n", __func__, 0x0c);

	gpio_value = gpio_get_value(gpio);
	hwlog_info("%s: intb=%d after clear MASK\n", __func__, gpio_value);

	if (gpio_value == 0)
		schedule_work(&di->g_intb_work);

	if (is_rt8979()) {
		enable_irq(di->irq);
		rt8979_re_init(di);
	}
	hwlog_info("%s--", __func__);
	return 0;
}

int fsa9685_fcp_cmd_transfer_check(void)
{
	int reg_val1;
	int reg_val2;
	int i = 0;

	/* read accp interrupt registers until value is not zero */
	do {
		if (is_adp_plugout()) {
			hwlog_info("adp plugout\n");
			return -1;
		}

		usleep_range(30000, 31000); /* sleep 30ms */
		reg_val1 = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1);
		reg_val2 = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2);
		i++;
	} while ((i < MUIC_ACCP_INT_MAX_COUNT) && (reg_val1 == 0) && (reg_val2 == 0));

	if (reg_val1 < 0 || reg_val2 < 0) {
		hwlog_err("%s:read error reg_val1=%d,reg_val2=%d\n",
			__func__, reg_val1, reg_val2);
		return -1;
	}

	/* if something changed print reg info */
	if (reg_val2 & (FAS9685_PROSTAT | FAS9685_DSDVCSTAT))
		hwlog_info("%s:ACCP state changed,reg 0x59=0x%x,reg 0x5A=0x%x\n",
			__func__, reg_val1, reg_val2);

	/* judge if cmd transfer success */
	if ((reg_val1 & FAS9685_ACK) && (reg_val1 & FAS9685_CMDCPL) &&
		!(reg_val1 & FAS9685_CRCPAR) &&
		!(reg_val2 & (FAS9685_CRCRX | FAS9685_PARRX)))
		return 0;

	hwlog_err("%s:reg 0x59=0x%x,reg 0x5A=0x%x\n",
		__func__, reg_val1, reg_val2);
	return -1;
}

int rt8979_fcp_cmd_transfer_check(void)
{
	const int wait_time = RT8979_FCP_DM_TRANSFER_CHECK_WAIT_TIME;
	int reg_val1 = 0;
	int reg_val2 = 0;
	int i = -1;
	int scp_status;
	/* read accp interrupt registers until value is not zero */

	scp_status = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);
	if (scp_status == 0)
		return -1;
	do {
		if (is_adp_plugout()) {
			hwlog_info("adp plugout\n");
			return -1;
		}
		usleep_range(30000, 31000);
		reg_val1 = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1);
		reg_val2 = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2);
		i++;
	} while ((i < MUIC_ACCP_INT_MAX_COUNT) && (reg_val1 == 0) && (reg_val2 == 0));

	if (reg_val1 == 0 && reg_val2 == 0)
		hwlog_info("%s : read accp interrupt time out,total time is %d ms\n",
			__func__, wait_time);

	if (reg_val1 < 0 || reg_val2 < 0) {
		hwlog_err("%s: read  error!!! reg_val1=%d,reg_val2=%d\n",
			__func__, reg_val1, reg_val2);
		return -1;
	}

	/* if something  changed print reg info */
	if (reg_val2 & (FAS9685_PROSTAT | FAS9685_DSDVCSTAT))
		hwlog_info("%s : ACCP state changed  ,reg[0x59]=0x%x,reg[0x5A]=0x%x\n",
			__func__, reg_val1, reg_val2);

	if (reg_val2 == 0) {
		if (reg_val1 == FAS9685_CMDCPL) {
			/* increase OSC */
			hwlog_info("%s: increase OSC\n", __func__);
			if (rt8979_adjust_osc(-1) == 0) {
				return -RT8979_RETRY;
			}
		} else if (reg_val1	== (FAS9685_CMDCPL | FAS9685_CRCPAR)) {
			/* decrease OSC */
			hwlog_info("%s: decrease OSC\n", __func__);
			if (rt8979_adjust_osc(1) == 0) {
				return -RT8979_RETRY;
			}
		} else if (reg_val1	== (FAS9685_CMDCPL | FAS9685_ACK | FAS9685_CRCPAR)) {
			hwlog_info("%s: decrease OSC\n", __func__);
			if (rt8979_adjust_osc(1) == 0) {
				return -RT8979_FAIL;
			}
		}
	}

	if ((reg_val1 & FAS9685_CMDCPL) && (reg_val1 & FAS9685_ACK)
		&& !(reg_val1 & FAS9685_CRCPAR)
		&& !(reg_val2 & (FAS9685_CRCRX | FAS9685_PARRX)))
		return 0;

	hwlog_err("%s : reg[0x59]=0x%x,reg[0x5A]=0x%x\n", __func__, reg_val1, reg_val2);
	return -1;
}


/****************************************************************************
  Function:     fcp_cmd_transfer_check
  Description:  check cmd transfer success or fail
  Input:         NA
  Output:       NA
  Return:        0: success
                   -1: fail
***************************************************************************/
int fcp_cmd_transfer_check(void)
{
	return is_rt8979() ?
		rt8979_fcp_cmd_transfer_check() :
		fsa9685_fcp_cmd_transfer_check();
}

/****************************************************************************
  Function:     fcp_protocol_restart
  Description:  disable accp protocol and enable again
  Input:         NA
  Output:       NA
  Return:        0: success
                   -1: fail
***************************************************************************/
void fcp_protocol_restart(void)
{
	int reg_val = 0;
	int ret = 0;
	int slave_good, accp_status_mask;

	if (is_rt8979()) {
		slave_good = RT8979_ACCP_STATUS_SLAVE_GOOD;
		accp_status_mask =  RT8979_ACCP_STATUS_MASK;
		rt8979_sw_open(true);
		rt8979_regs_dump();
		/* RT8979 didn't support fcp_protocol_restart */
		return;
	} else {
		slave_good = FSA9688_ACCP_STATUS_SLAVE_GOOD;
		accp_status_mask = FSA9688_ACCP_STATUS_MASK;
	}
	/* disable accp protocol */
	fsa9685_write_reg_mask(FSA9685_REG_ACCP_CNTL, 0, FAS9685_ACCP_CNTL_MASK);
	usleep_range(100000, 101000);
	reg_val = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);

	if (slave_good == (reg_val & accp_status_mask))
		hwlog_err("%s : disable accp enable bit failed ,accp status [0x40]=0x%x\n",
			__func__, reg_val);

	/* enable accp protocol */
	fsa9685_write_reg_mask(FSA9685_REG_ACCP_CNTL,
		FSA9685_ACCP_IS_ENABLE, FAS9685_ACCP_CNTL_MASK);
	usleep_range(100000, 101000);
	reg_val = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);
	if (slave_good != (reg_val & accp_status_mask))
		hwlog_err("%s : enable accp enable bit failed, accp status [0x40]=0x%x\n",
			__func__, reg_val);

	/* disable accp interrupt */
	ret |= fsa9685_write_reg_mask(FSA9685_REG_CONTROL2,
		FSA9685_ACCP_OSC_ENABLE, FSA9685_ACCP_OSC_ENABLE);
	if (is_rt8979())
		ret |= fsa9685_write_reg_mask(FSA9685_REG_CONTROL2,
			FSA9685_DCD_TIME_OUT_MASK, FSA9685_ACCP_ENABLE | FSA9685_DCD_TIME_OUT_MASK);

	ret += fsa9685_write_reg(FSA9685_REG_ACCP_INTERRUPT_MASK1, 0xFF);
	ret += fsa9685_write_reg(FSA9685_REG_ACCP_INTERRUPT_MASK2, 0xFF);
	if (ret < 0)
		hwlog_err("accp interrupt mask write failed\n");

	hwlog_info("%s-%d :disable and enable accp protocol accp status is 0x%x\n",
		__func__, __LINE__, reg_val);
}
/****************************************************************************
  Function:     accp_adapter_reg_read
  Description:  read adapter register
  Input:        reg:register's num
                val:the value of register
  Output:       NA
  Return:        0: success
                -1: fail
***************************************************************************/
int accp_adapter_reg_read(int* val, int reg)
{
	int reg_val1 = 0;
	int reg_val2 = 0;
	int i = 0;
	int ret = 0;
	int adjust_osc_count = 0;
	int fcp_check_ret;

	fsa9685_accp_adaptor_reg_lock();
	for (i = 0; i < FCP_RETRY_MAX_TIMES &&
		adjust_osc_count < RT8979_ADJ_OSC_MAX_COUNT; i++) {
		if (is_adp_plugout()) {
			hwlog_info("adp plugout\n");
			fsa9685_accp_adaptor_reg_unlock();
			return -1;
		}
		/* before send cmd, read and clear accp interrupt registers */
		reg_val1 = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1);
		reg_val2 = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2);

		ret += fsa9685_write_reg(FSA9685_REG_ACCP_CMD, FSA9685_ACCP_CMD_SBRRD);
		ret += fsa9685_write_reg(FSA9685_REG_ACCP_ADDR, reg);
		ret |= fsa9685_write_reg_mask(FSA9685_REG_ACCP_CNTL,
			FSA9685_ACCP_IS_ENABLE | FAS9685_ACCP_SENDCMD, FAS9685_ACCP_CNTL_MASK);
		if (ret) {
			hwlog_err("%s: write error ret is %d\n", __func__, ret);
			fsa9685_accp_adaptor_reg_unlock();
			return -1;
		}

		/* check cmd transfer success or fail */
		fcp_check_ret = fcp_cmd_transfer_check();
		if (fcp_check_ret == 0) {
			/* recived data from adapter */
			*val = fsa9685_read_reg(FSA9685_REG_ACCP_DATA);
			break;
		} else if (-RT8979_RETRY == fcp_check_ret) {
			adjust_osc_count++;
			i--; /* do retry, so decrease the retry count */
		}
		/* if transfer failed, restart accp protocol */
		fcp_protocol_restart();
		hwlog_err("%s : adapter register read fail times=%d ,register=0x%x,data=0x%x,reg[0x59]=0x%x,reg[0x5A]=0x%x\n",
			__func__, i, reg, *val, reg_val1, reg_val2);
	}

	hwlog_debug("%s : adapter register retry times=%d ,register=0x%x,data=0x%x,reg[0x59]=0x%x,reg[0x5A]=0x%x\n",
		__func__, i, reg, *val, reg_val1, reg_val2);
	if (i == FCP_RETRY_MAX_TIMES) {
		hwlog_err("%s : ack error,retry %d times\n", __func__, i);
		ret = -1;
	} else {
		ret = 0;
	}
	fsa9685_accp_adaptor_reg_unlock();
	return ret;
}

/****************************************************************************
  Function:     accp_adapter_reg_write
  Description:  write value into the adapter register
  Input:        reg:register's num
                val:the value of register
  Output:       NA
  Return:        0: success
                -1: fail
***************************************************************************/
int accp_adapter_reg_write(int val, int reg)
{
	int reg_val1 = 0;
	int reg_val2 = 0;
	int i = 0;
	int ret = 0;
	int adjust_osc_count = 0;
	int fcp_check_ret;

	fsa9685_accp_adaptor_reg_lock();
	for (i = 0; i < FCP_RETRY_MAX_TIMES && adjust_osc_count < RT8979_ADJ_OSC_MAX_COUNT; i++) {
		if (is_adp_plugout()) {
			hwlog_info("adp plugout\n");
			fsa9685_accp_adaptor_reg_unlock();
			return -1;
		}

		/* before send cmd, clear accp interrupt registers */
		reg_val1 = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1);
		reg_val2 = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2);

		ret |= fsa9685_write_reg(FSA9685_REG_ACCP_CMD, FSA9685_ACCP_CMD_SBRWR);
		ret |= fsa9685_write_reg(FSA9685_REG_ACCP_ADDR, reg);
		ret |= fsa9685_write_reg(FSA9685_REG_ACCP_DATA, val);
		ret |= fsa9685_write_reg_mask(FSA9685_REG_ACCP_CNTL,
			FSA9685_ACCP_IS_ENABLE | FAS9685_ACCP_SENDCMD, FAS9685_ACCP_CNTL_MASK);
		if (ret < 0) {
			hwlog_err("%s: write error ret is %d\n", __func__, ret);
			fsa9685_accp_adaptor_reg_unlock();
			return -1;
		}

		/* check cmd transfer success or fail */
		fcp_check_ret = fcp_cmd_transfer_check();
		if (fcp_check_ret == 0) {
			break;
		} else if (-RT8979_RETRY == fcp_check_ret) {
			adjust_osc_count++;
			i--; /* do retry, so decrease the retry count */
		}
		/* if transfer failed, restart accp protocol */
		fcp_protocol_restart();
		hwlog_err("%s : adapter register write fail times=%d ,register=0x%x,data=0x%x,reg[0x59]=0x%x,reg[0x5A]=0x%x\n",
			__func__, i, reg, val, reg_val1, reg_val2);
	}
	hwlog_debug("%s : adapter register retry times=%d ,register=0x%x,data=0x%x,reg[0x59]=0x%x,reg[0x5A]=0x%x\n",
		__func__, i, reg, val, reg_val1, reg_val2);

	if (i == FCP_RETRY_MAX_TIMES) {
		hwlog_err("%s : ack error,retry %d times\n", __func__, i);
		ret = -1;
	} else {
		ret = 0;
	}
	fsa9685_accp_adaptor_reg_unlock();
	return ret;
}

int scp_adapter_reg_read(int *val, int reg)
{
	int ret;

	if (scp_error_flag) {
		hwlog_err("%s scp timeout happened, do not read reg = %d\n",
			__func__, reg);
		return -1;
	}
	ret = accp_adapter_reg_read(val, reg);
	if (ret) {
		hwlog_err("%s error reg = %d\n", __func__, reg);
		scp_error_flag = 1;
		return -1;
	}
	return 0;
}

int scp_adapter_reg_write(int val, int reg)
{
	int ret;

	if (scp_error_flag) {
		hwlog_err("%s scp timeout happened, do not write reg = %d\n",
			__func__, reg);
		return -1;
	}
	ret = accp_adapter_reg_write(val, reg);
	if (ret) {
		hwlog_err("%s error reg = %d\n", __func__, reg);
		scp_error_flag = 1;
		return -1;
	}
	return 0;
}

static void rt8979_regs_dump(void)
{
	const int reg_addr[] = {
		FSA9685_REG_CONTROL,
		RT8979_REG_MUIC_EXT1,
		RT8979_REG_MUIC_STATUS1,
		RT8979_REG_MUIC_STATUS2,
		FSA9685_REG_ACCP_STATUS,
		RT8979_REG_EXT3, /* 0xa0 */
		RT8979_REG_USBCHGEN, /* 0xa4 */
		RT8979_REG_MUIC_EXT2, /* 0xa7 */
		FSA9685_REG_CONTROL2, /* 0x0e */
		FSA9685_REG_DEVICE_TYPE_1, /* 0x08 */
		FSA9685_REG_DEVICE_TYPE_2, /* 0x09 */
		FSA9685_REG_DEVICE_TYPE_3, /* 0x0a */
		FSA9685_REG_DEVICE_TYPE_4, /* 0x0f */
	};
	int regval;
	int i;
	for (i = 0; i < ARRAY_SIZE(reg_addr); i++) {
		regval = fsa9685_read_reg(reg_addr[i]);
		hwlog_info("reg[0x%02x] = 0x%02x\n", reg_addr[i], regval);
	}
}
static int check_accp_ic_status(void)
{
	int check_times = 0;
	int reg_dev_type1 = 0;
#ifdef CONFIG_BOOST_5V
	int ret = 0;
	if (g_fsa9685_dev->power_by_5v) {
		ret = boost_5v_enable(ENABLE, BOOST_CTRL_FCP);
		ret |= dc_set_bst_ctrl(ENABLE);
		if (ret) {
			hwlog_err("[%s]:5v boost open fail\n", __func__);
			return ACCP_NOT_PREPARE_OK;
		}
		hwlog_info("[%s]:5v boost open\n", __func__);
	}
#endif
	for (check_times = 0; check_times < ADAPTOR_BC12_TYPE_MAX_CHECK_TIME; check_times++) {
		if (is_adp_plugout()) {
			hwlog_info("adp plugout\n");
			return ACCP_NOT_PREPARE_OK;
		}

		reg_dev_type1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
		if (reg_dev_type1 >= 0) {
			if (reg_dev_type1 & FSA9685_DCP_DETECTED) {
				hwlog_info("%s: FSA9685_DCP_DETECTED\n", __func__);
				break;
			}
		}
		hwlog_info("[%s]reg_dev_type1 = 0x%x,check_times = %d\n",
			__func__, reg_dev_type1, check_times);
		usleep_range(WAIT_FOR_BC12_DELAY * 1000, (WAIT_FOR_BC12_DELAY + 1) * 1000);
	}
	hwlog_info("[%s]:accp is ok,check_times = %d\n", __func__, check_times);

#ifdef CONFIG_BOOST_5V
	if (check_times >= ADAPTOR_BC12_TYPE_MAX_CHECK_TIME) {
		if (!g_fsa9685_dev)
			return ACCP_PREPARE_OK;

		if (g_fsa9685_dev->power_by_5v) {
			boost_5v_enable(DISABLE, BOOST_CTRL_FCP);
			dc_set_bst_ctrl(DISABLE);
		}
	}
#endif /* CONFIG_BOOST_5V */

	return ACCP_PREPARE_OK;
}

static int fsa9865_accp_adapter_detect(void)
{
	int reg_val1;
	int reg_val2;
	int i;
	int slave_good;
	int accp_status_mask;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return ACCP_ADAPTOR_DETECT_OTHER;
	}


	slave_good = FSA9688_ACCP_STATUS_SLAVE_GOOD;
	accp_status_mask = FSA9688_ACCP_STATUS_MASK;

	if (check_accp_ic_status() == ACCP_NOT_PREPARE_OK) {
		hwlog_err("check_accp_ic_status not prepare ok\n");
		return ACCP_ADAPTOR_DETECT_OTHER;
	}
	fsa9685_accp_detect_lock();
	/* check accp status */
	reg_val1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_4);
	reg_val2 = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);
	if ((reg_val1 & FSA9685_ACCP_CHARGER_DET) &&
		(slave_good == (reg_val2 & accp_status_mask))) {
		hwlog_info("accp adapter detect ok\n");
		fsa9685_accp_detect_unlock();
		return ACCP_ADAPTOR_DETECT_SUCC;
	}

	/* enable accp */
	reg_val1 = fsa9685_read_reg(FSA9685_REG_CONTROL2);
	reg_val1 |= FSA9685_ACCP_ENABLE;
	fsa9685_write_reg(FSA9685_REG_CONTROL2, reg_val1);

	hwlog_info("accp_adapter_detect 0x0e=0x%x\n", reg_val1);

	/* detect hisi acp charger */
	for (i = 0; i < ACCP_DETECT_MAX_COUT; i++) {
		if (is_adp_plugout()) {
			hwlog_info("adp plugout\n");
			fsa9685_accp_detect_unlock();
			return ACCP_ADAPTOR_DETECT_OTHER;
		}
		reg_val1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_4);
		reg_val2 = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);
		if ((reg_val1 & FSA9685_ACCP_CHARGER_DET) &&
			(slave_good == (reg_val2 & accp_status_mask)))
			break;
		accp_sleep();
	}
	/* clear accp interrupt */
	hwlog_info("%s:read ACCP interrupt reg 0x59=0x%x,reg 0x5A =0x%x\n",
		__func__, fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1),
		fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2));
	if (i == ACCP_DETECT_MAX_COUT) {
		fsa9685_accp_detect_unlock();
		hwlog_info("not accp adapter reg 0x0f=0x%x reg 0x40=0x%x\n",
			reg_val1, reg_val2);
		if (reg_val1 & FSA9685_ACCP_CHARGER_DET)
			return ACCP_ADAPTOR_DETECT_FAIL;

		return ACCP_ADAPTOR_DETECT_OTHER; /* not fcp adapter */
	}
	hwlog_info("accp adapter detect ok take %d ms\n", i * ACCP_POLL_TIME);
	fsa9685_accp_detect_unlock();
	return ACCP_ADAPTOR_DETECT_SUCC;
}

static int rt8979_accp_adapter_detect(void)
{
	int reg_val1;
	int reg_val2;
	int i;
	int j;
	bool vbus_present = false;
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return ACCP_ADAPTOR_DETECT_OTHER;
	}

	if (check_accp_ic_status() == ACCP_NOT_PREPARE_OK) {
		hwlog_err("check_accp_ic_status not prepare ok\n");
		return ACCP_ADAPTOR_DETECT_OTHER;
	}

	rt8979_regs_dump();
	fsa9685_accp_detect_lock();
	reg_val1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_4);
	reg_val2 = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);
	if ((reg_val1 & FSA9685_ACCP_CHARGER_DET) &&
		((reg_val2 & RT8979_ACCP_STATUS_MASK) ==
		RT8979_ACCP_STATUS_SLAVE_GOOD)) {
		hwlog_info("accp adapter detect ok\n");
		rt8979_sw_open(true);
		fsa9685_accp_detect_unlock();
		return ACCP_ADAPTOR_DETECT_SUCC;
	}
	rt8979_auto_restart_accp_det();
	rt8979_regs_dump();
	vbus_present = (fsa9685_read_reg(RT8979_REG_MUIC_STATUS1) &
		RT8979_REG_MUIC_STATUS1_DCDT) ? false : true;
	for (j = 0; j < ACCP_MAX_TRYCOUNT && vbus_present; j++) {
		for (i = 0; i < ACCP_DETECT_MAX_COUT; i++) {
			if (is_adp_plugout()) {
				hwlog_info("adp plugout\n");
				fsa9685_accp_detect_unlock();
				return ACCP_ADAPTOR_DETECT_OTHER;
			}
			reg_val1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_4);
			reg_val2 = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);
			if ((reg_val1 & FSA9685_ACCP_CHARGER_DET) &&
				((reg_val2 & RT8979_ACCP_STATUS_MASK) ==
				RT8979_ACCP_STATUS_SLAVE_GOOD))
				break;
			accp_sleep();
		}
		hwlog_info("%s:reg 0x59=0x%x,reg 0x5A=0x%x, reg 0xA7=0x%x\n",
			__func__, fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1),
			fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2),
		fsa9685_read_reg(RT8979_REG_MUIC_EXT2));
		if (i == ACCP_DETECT_MAX_COUT) {
			fsa9685_accp_detect_unlock();
			hwlog_info("not accp adapter reg 0xf=0x%x reg 0x40=0x%x\n",
				reg_val1, reg_val2);
			if (reg_val1 & FSA9685_ACCP_CHARGER_DET)
				return ACCP_ADAPTOR_DETECT_FAIL;
			rt8979_regs_dump();
			vbus_present = (fsa9685_read_reg(RT8979_REG_MUIC_STATUS1) &
				RT8979_REG_MUIC_STATUS1_DCDT) ? false : true;
			if (vbus_present)
				rt8979_force_restart_accp_det(true);

		} else {
			hwlog_info("accp adapter detect ok,take %d ms\n",
				i * ACCP_POLL_TIME);
			rt8979_sw_open(true);
			fsa9685_accp_detect_unlock();
			return ACCP_ADAPTOR_DETECT_SUCC;
		}
	}
	fsa9685_accp_detect_unlock();
	return ACCP_ADAPTOR_DETECT_OTHER; /* not fcp adapter */
}

int accp_adapter_detect(void *dev_data)
{
	return is_rt8979() ? rt8979_accp_adapter_detect() :
		fsa9865_accp_adapter_detect();
}

int is_support_fcp(void)
{
	int reg_val;
	static int flag_result = -EINVAL;
	struct fsa9685_device_info *di = g_fsa9685_dev;
	int version;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return -1;
	}

	if (!di->fcp_support)
		return -1;

	if (flag_result != -EINVAL)
		return flag_result;

	if (is_rt8979())
		return 0;

	reg_val = fsa9685_read_reg(FSA9685_REG_DEVICE_ID);
	version = reg_val & FAS9685_VERSION_ID_BIT_MASK;
	version = version >> FAS9685_VERSION_ID_BIT_SHIFT;
	if (version != FSA9688_VERSION_ID && version != FSA9688C_VERSION_ID) {
		hwlog_err("%s:not fsa9688 not support fcp reg 0x1=%d\n",
			__func__, reg_val);
		flag_result = -1;
	} else {
		flag_result = 0;
	}

	return flag_result;
}

int fcp_read_switch_status(void *dev_data)
{
	int reg_val;
	int slave_good;
	int accp_status_mask;

	if (is_rt8979()) {
		slave_good = RT8979_ACCP_STATUS_SLAVE_GOOD;
		accp_status_mask = RT8979_ACCP_STATUS_MASK;
	} else {
		slave_good = FSA9688_ACCP_STATUS_SLAVE_GOOD;
		accp_status_mask = FSA9688_ACCP_STATUS_MASK;
	}

	reg_val = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);
	if (reg_val < 0) {
		hwlog_err("reg_val fail\n");
		return -1;
	}

	if ((slave_good != (reg_val & accp_status_mask)))
		return -1;

	return 0;
}

#ifdef CONFIG_DIRECT_CHARGER
static int fsa9685_is_support_scp(void)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di || !di->client) {
		hwlog_err("di or client is null\n");
		return -1;
	}

	if (!di->scp_support)
		return -1;

	return 0;
}

static int fsa9685_scp_chip_reset(void *dev_data)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di || !di->client) {
		hwlog_err("error: di or client is null\n");
		return -1;
	}

	hwlog_info("%s\n", __func__);

	if (di->power_by_5v)
		return 0;

	return switch_chip_reset(dev_data);
}

static int fsa9685_scp_exit(void *dev_data)
{
	struct fsa9685_device_info *di = g_fsa9685_dev;

	if (!di || !di->client) {
		hwlog_err("error: di or client is null\n");
		return -1;
	}

	hwlog_info("%s\n", __func__);

	scp_error_flag = 0;

	if (di->power_by_5v)
		return 0;

#ifdef CONFIG_BOOST_5V
	if (di->power_by_5v) {
		boost_5v_enable(DISABLE, BOOST_CTRL_FCP);
		dc_set_bst_ctrl(DISABLE);
	}
#endif
	hwlog_info("%s:5v boost close\n", __func__);
	return 0;
}

static int fsa9685_scp_adaptor_reset(void *dev_data)
{
	return fcp_adapter_reset(dev_data);
}

#endif

#ifdef CONFIG_BOOST_5V
static int charge_notifier_call(struct notifier_block *usb_nb,
	unsigned long event, void *data)
{
	switch (event) {
	case VCHRG_START_CHARGING_EVENT:
		hwlog_info("%s:adp_plug in\n", __func__);
		adp_plugout = false;
		break;
	/* if STOP_CHARGING_EVENT coming, adp_plugout is false, go on next */
	/* fall-through */
	case VCHRG_STOP_CHARGING_EVENT:
		adp_plugout = true;
		scp_error_flag = 0;
		boost_5v_enable(DISABLE, BOOST_CTRL_FCP);
		dc_set_bst_ctrl(DISABLE);

		switch_chip_reset(g_fsa9685_dev);
		hwlog_info("%s:adp_plug out\n", __func__);
		break;
	default:
		hwlog_info("do nothing\n");
		break;
	}

	return NOTIFY_OK;
}
#endif

struct charge_switch_ops chrg_fsa9685_ops = {
	.get_charger_type = fsa9685_get_charger_type,
};

static struct water_detect_ops fsa9685_water_detect_ops = {
	.type_name = "usb_id",
	.is_water_intruded = fsa9685_is_water_intruded,
};

static int fsa9685_fcp_reg_read_block(int reg, int *val, int num,
	void *dev_data)
{
	int ret;
	int i;
	int data = 0;

	if (!val) {
		hwlog_err("val is null\n");
		return -1;
	}

	for (i = 0; i < num; i++) {
		ret = accp_adapter_reg_read(&data, reg + i);
		if (ret) {
			hwlog_err("fcp read failed(reg=0x%x)\n", reg + i);
			return -1;
		}

		val[i] = data;
	}

	return 0;
}

static int fsa9685_fcp_reg_write_block(int reg, const int *val, int num,
	void *dev_data)
{
	int ret;
	int i;

	if (!val) {
		hwlog_err("val is null\n");
		return -1;
	}

	for (i = 0; i < num; i++) {
		ret = accp_adapter_reg_write(val[i], reg + i);
		if (ret) {
			hwlog_err("fcp write failed(reg=0x%x)\n", reg + i);
			return -1;
		}
	}

	return 0;
}

static struct hwfcp_ops fsa9685_fcp_protocol_ops = {
	.chip_name = "fsa9685",
	.reg_read = fsa9685_fcp_reg_read_block,
	.reg_write = fsa9685_fcp_reg_write_block,
	.detect_adapter = accp_adapter_detect,
	.soft_reset_master = switch_chip_reset,
	.soft_reset_slave = fcp_adapter_reset,
	.get_master_status = fcp_read_switch_status,
	.stop_charging_config = fcp_stop_charge_config,
	.is_accp_charger_type = is_fcp_charger_type,
};

static int fsa9685_scp_reg_read_block(int reg, int *val, int num,
	void *dev_data)
{
	int ret;
	int i;
	int data = 0;

	if (!val) {
		hwlog_err("val is null\n");
		return -1;
	}

	scp_error_flag = 0;

	for (i = 0; i < num; i++) {
		ret = scp_adapter_reg_read(&data, reg + i);
		if (ret) {
			hwlog_err("scp read failed(reg=0x%x)\n", reg + i);
			return -1;
		}

		val[i] = data;
	}

	return 0;
}

static int fsa9685_scp_reg_write_block(int reg, const int *val, int num,
	void *dev_data)
{
	int ret;
	int i;

	if (!val) {
		hwlog_err("val is null\n");
		return -1;
	}

	scp_error_flag = 0;

	for (i = 0; i < num; i++) {
		ret = scp_adapter_reg_write(val[i], reg + i);
		if (ret) {
			hwlog_err("scp write failed(reg=0x%x)\n", reg + i);
			return -1;
		}
	}

	return 0;
}

static struct hwscp_ops fsa9685_scp_protocol_ops = {
	.chip_name = "fsa9685",
	.reg_read = fsa9685_scp_reg_read_block,
	.reg_write = fsa9685_scp_reg_write_block,
	.detect_adapter = accp_adapter_detect,
	.soft_reset_master = fsa9685_scp_chip_reset,
	.soft_reset_slave = fsa9685_scp_adaptor_reset,
	.post_exit = fsa9685_scp_exit,
};

/**********************************************************
*  Function:       fcp_mmi_show
*  Discription:    file node for mmi testing fast charge protocol
*  Parameters:     NA
*  return value:   0:success
*                  1:fail
*                  2:not support
**********************************************************/
static ssize_t fcp_mmi_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int result;
	unsigned int type = mt_get_charger_type();

	/* judge whether support fcp */
	if (adapter_test_check_protocol_type(ADAPTER_PROTOCOL_FCP)) {
		result = AT_RESULT_NOT_SUPPORT;
		return snprintf(buf, PAGE_SIZE, "%d\n", result);
	}
	/* to avoid the usb port cutoff when CTS test */
	if ((type == CHARGER_TYPE_USB) || (type == CHARGER_TYPE_BC_USB)) {
		result = AT_RESULT_FAIL;
		hwlog_err("fcp detect fail 1,charge type is %u\n", type);
		return snprintf(buf, PAGE_SIZE, "%d\n", result);
	}
	/* fcp adapter detect */
	if (adapter_test_check_support_mode(ADAPTER_PROTOCOL_FCP)) {
		/* 0:sdp 1:cdp 2:dcp 3:unknow 4:none */
		hwlog_err("fcp detect fail 2, charge type is %u\n", fsa9685_get_charger_type());
		result = AT_RESULT_FAIL;
	} else {
		result = AT_RESULT_SUCC;
	}
	hwlog_info("%s: fcp mmi result %d\n", __func__, result);
	return snprintf(buf, PAGE_SIZE, "%d\n", result);
}

static DEVICE_ATTR(fcp_mmi, S_IRUGO, fcp_mmi_show, NULL);

#ifdef CONFIG_OF
static const struct of_device_id switch_fsa9685_ids[] = {
	{ .compatible = "huawei,fairchild_fsa9685" },
	{ .compatible = "huawei,fairchild_fsa9683"},
	{ .compatible = "huawei,nxp_cbtl9689" },
	{ },
};
MODULE_DEVICE_TABLE(of, switch_fsa9685_ids);
#endif

static struct usbswitch_common_ops huawei_switch_extra_ops = {
	.manual_switch = fsa9685_manual_switch,
	.dcd_timeout_enable = fsa9685_dcd_timeout,
	.dcd_timeout_status = fsa9685_dcd_timeout_status,
	.manual_detach = fsa9685_manual_detach_work,
	.enable_chg_type_det = usb_switch_chg_type_det,
};

static int fsa9685_parse_dts(struct device_node* np, struct fsa9685_device_info *di)
{
	int ret = 0;

	if (!power_gpio_config_output(np, "switch_gpio", "switch_gpio_init",
		&g_switch_gpio, 1)) {
		gpio_set_value(g_switch_gpio, 1);
		ret = gpio_get_value(g_switch_gpio);
		hwlog_info("g_switch_gpio get value = %d\n", ret);
	}

	ret = of_property_read_u32(np, "usbid-enable", &(di->usbid_enable));
	if (ret) {
		di->usbid_enable = 1;
		hwlog_err("error: usbid-enable dts read failed\n");
	}
	hwlog_info("usbid-enable=%d\n", di->usbid_enable);

	ret = of_property_read_u32(np, "fcp_support", &(di->fcp_support));
	if (ret) {
		di->fcp_support = 0;
		hwlog_err("error: fcp_support dts read failed\n");
	}
	hwlog_info("fcp_support=%d\n", di->fcp_support);

	ret = of_property_read_u32(np, "scp_support", &(di->scp_support));
	if (ret) {
		di->scp_support = 0;
		hwlog_err("error: scp_support dts read failed\n");
	}
	hwlog_info("scp_support=%d\n", di->scp_support);

	ret = of_property_read_u32(np, "mhl_detect_disable", &(di->mhl_detect_disable));
	if (ret) {
		di->mhl_detect_disable = 0;
		hwlog_err("error: mhl_detect_disable dts read failed\n");
	}
	hwlog_info("mhl_detect_disable=%d\n", di->mhl_detect_disable);

	ret = of_property_read_u32(np, "two-switch-flag", &(di->two_switch_flag));
	if (ret) {
		di->two_switch_flag = 0;
		hwlog_err("error: two-switch-flag dts read failed\n");
	}
	hwlog_info("two-switch-flag=%d\n", di->two_switch_flag);

	ret = of_property_read_u32(of_find_compatible_node(NULL, NULL, "huawei,charger"),
		"pd_support", &(di->pd_support));
	if (ret) {
		di->pd_support = 0;
		hwlog_err("error: pd_support dts read failed\n");
	}
	hwlog_info("pd_support=%d\n", di->pd_support);

	ret = of_property_read_u32(np, "dcd_timeout_force_enable", &(di->dcd_timeout_force_enable));
	if (ret) {
		di->dcd_timeout_force_enable = 0;
		hwlog_err("error: dcd_timeout_force_enable dts read failed\n");
	}
	hwlog_info("dcd_timeout_force_enable=%d\n", di->dcd_timeout_force_enable);

	ret = of_property_read_u32(np, "power_by_5v", &(di->power_by_5v));
	if (ret)
		di->power_by_5v = 0;

	hwlog_info("power_by_5v=%d\n", di->power_by_5v);

	return 0;
}

static int rt8979_write_osc_pretrim(void)
{
	int retval = 0;

	retval = fsa9685_write_reg(RT8979_REG_TEST_MODE, RT8979_REG_TEST_MODE_VAL1);
	if (retval < 0)
		return retval;
	retval = fsa9685_write_reg(RT8979_REG_EFUSE_CTRL, RT8979_REG_EFUSE_CTRL_VAL);
	if (retval < 0) {
		fsa9685_write_reg(RT8979_REG_TEST_MODE, RT8979_REG_TEST_MODE_DEFAULT_VAL);
		return retval;
	}
	retval = fsa9685_write_reg(RT8979_REG_EFUSE_PRETRIM_DATA,
		(rt8979_osc_trim_code + rt8979_osc_trim_adjust) ^ RT8979_REG_EFUSE_PRETRIM_DATA_VAL);
	if (retval < 0) {
		fsa9685_write_reg(RT8979_REG_TEST_MODE, RT8979_REG_TEST_MODE_DEFAULT_VAL);
		return retval;
	}
	retval = fsa9685_write_reg(RT8979_REG_EFUSE_PRETRIM_ENABLE, RT8979_REG_EFUSE_PRETRIM_ENABLE_VAL);
	usleep_range(WRITE_OSC_PRETRIM_DELAY_MIN, WRITE_OSC_PRETRIM_DELAY_MAX);
	retval = fsa9685_read_reg(RT8979_REG_EFUSE_READ_DATA);
	hwlog_info("%s : trim code read = %d\n", __func__, retval);

	retval = fsa9685_write_reg(RT8979_REG_TEST_MODE, RT8979_REG_TEST_MODE_DEFAULT_VAL);
	return retval;
}

static int rt8979_adjust_osc(int8_t val)
{
	int8_t temp;

	temp = rt8979_osc_trim_code + rt8979_osc_trim_adjust + val;
	if (temp > rt8979_osc_upper_bound || temp < rt8979_osc_lower_bound) {
		hwlog_err("%s : reach to upper/lower bound %d / %d\n", __func__,
			rt8979_osc_trim_code, rt8979_osc_trim_adjust);
		return -RT8979_FAIL;
	}

	rt8979_osc_trim_adjust += val;
	hwlog_info("%s : adjust osc trim code %d / %d\n", __func__,
		rt8979_osc_trim_code, rt8979_osc_trim_adjust);

	return rt8979_write_osc_pretrim();
}

static int rt8979_init_osc_params(void)
{
	int retval = 0;

	hwlog_info("%s : entry\n", __func__);
	retval = fsa9685_write_reg(RT8979_REG_TEST_MODE, RT8979_REG_TEST_MODE_VAL1);
	if (retval < 0)
		return retval;

	retval = fsa9685_write_reg(RT8979_REG_EFUSE_CTRL, RT8979_REG_EFUSE_CTRL_VAL);
	if (retval < 0) {
		fsa9685_write_reg(RT8979_REG_TEST_MODE, RT8979_REG_TEST_MODE_DEFAULT_VAL);
		return retval;
	}

	retval = fsa9685_write_reg(RT8979_REG_EFUSE_PRETRIM_ENABLE, RT8979_REG_EFUSE_PRETRIM_ENABLE_VAL1);
	if (retval < 0) {
		fsa9685_write_reg(RT8979_REG_TEST_MODE, RT8979_REG_TEST_MODE_DEFAULT_VAL);
		return retval;
	}
	usleep_range(WRITE_OSC_PRETRIM_DELAY_MIN_DEFAULT, WRITE_OSC_PRETRIM_DELAY_MIN);
	retval = fsa9685_read_reg(RT8979_REG_EFUSE_READ_DATA);
	if (retval < 0) {
		fsa9685_write_reg(RT8979_REG_TEST_MODE, RT8979_REG_TEST_MODE_DEFAULT_VAL);
		return retval;
	}
	hwlog_info("%s : trim code read = %d\n", __func__, retval);
	rt8979_osc_trim_code = retval;
	rt8979_osc_lower_bound = RT8979_OSC_BOUND_MIN;
	rt8979_osc_upper_bound = RT8979_OSC_BOUND_MAX;
	rt8979_osc_trim_adjust = RT8979_OSC_TRIM_ADJUST_DEFAULT;

	retval = rt8979_osc_trim_code + rt8979_osc_trim_adjust;
	if (retval < rt8979_osc_lower_bound)
		retval = rt8979_osc_lower_bound;
	if (retval > rt8979_osc_upper_bound)
		retval = rt8979_osc_upper_bound;
	rt8979_osc_trim_default = retval - rt8979_osc_trim_code;
	rt8979_osc_trim_adjust = rt8979_osc_trim_default;

	retval = fsa9685_write_reg(RT8979_REG_EFUSE_PRETRIM_DATA,
		(retval) ^ RT8979_REG_EFUSE_PRETRIM_DATA_VAL);
	hwlog_info("%s : trim code = %d, rt8979_osc_trim_adjust = %d, %d\n",
			__func__, rt8979_osc_trim_code, rt8979_osc_trim_adjust,
			(rt8979_osc_trim_code + rt8979_osc_trim_adjust) ^ RT8979_REG_EFUSE_PRETRIM_DATA_VAL);
	if (retval < 0) {
		fsa9685_write_reg(RT8979_REG_TEST_MODE, RT8979_REG_TEST_MODE_DEFAULT_VAL);
		return retval;
	}
	fsa9685_write_reg(RT8979_REG_EFUSE_PRETRIM_ENABLE, RT8979_REG_EFUSE_PRETRIM_ENABLE_VAL);
	usleep_range(WRITE_OSC_PRETRIM_DELAY_MIN, WRITE_OSC_PRETRIM_DELAY_MAX);
	retval = fsa9685_read_reg(RT8979_REG_EFUSE_READ_DATA);
	hwlog_info("%s : trim code read = %d\n", __func__, retval);

	retval = fsa9685_write_reg(RT8979_REG_TEST_MODE, RT8979_REG_TEST_MODE_DEFAULT_VAL);
	return retval;
}

static void fsa9685_disable_fcp_intr(struct fsa9685_device_info *di)
{
	int ret = 0;
	int reg_ctl;

	if (is_support_fcp())
		return;

	/* if support fcp, disable fcp interrupt */
	ret += fsa9685_write_reg_mask(FSA9685_REG_CONTROL2, 0, FSA9685_ACCP_OSC_ENABLE);
	ret += fsa9685_write_reg(FSA9685_REG_ACCP_INTERRUPT_MASK1, 0xFF);
	ret += fsa9685_write_reg(FSA9685_REG_ACCP_INTERRUPT_MASK2, 0xFF);
	if (ret < 0)
		hwlog_err("accp interrupt mask write failed\n");

	reg_ctl = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
	hwlog_info("DEV_TYPE1 = 0x%x\n", reg_ctl);
	if (is_rt8979()) {
		fsa9685_write_reg(RT8979_REG_EXT3, RT8979_REG_EXT3_VAL);
		fsa9685_write_reg(RT8979_REG_MUIC_CTRL_3, RT8979_REG_MUIC_CTRL_3_DISABLEID_FUNCTION);
		reg_ctl = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
		hwlog_info("DEV_TYPE1 = 0x%x\n", reg_ctl);
		reg_ctl = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_4);
		hwlog_info("DEV_TYPE4 = 0x%x\n", reg_ctl);
	}
}

static int fsa9685_register_init(struct fsa9685_device_info *di)
{
	int ret;
	struct power_devices_info_data *power_dev_info = NULL;

#ifdef CONFIG_HUAWEI_CHARGER
	if (charge_switch_ops_register(&chrg_fsa9685_ops) == 0)
		hwlog_info("charge switch ops register success\n");
#endif /* CONFIG_HUAWEI_CHARGER */
	water_detect_ops_register(&fsa9685_water_detect_ops);

	/* if chip support fcp, register fcp adapter ops */
	if (is_support_fcp() == 0)
		hwfcp_ops_register(&fsa9685_fcp_protocol_ops);

#ifdef CONFIG_DIRECT_CHARGER
	/* if chip support scp, register scp adapter ops */
	if (fsa9685_is_support_scp() == 0)
		hwscp_ops_register(&fsa9685_scp_protocol_ops);
#endif /* CONFIG_DIRECT_CHARGER */

#ifdef CONFIG_HUAWEI_HW_DEV_DCT
	/* detect current device successful, set the flag as present */
	set_hw_dev_flag(DEV_I2C_USB_SWITCH);
#endif /* CONFIG_HUAWEI_HW_DEV_DCT */

	ret = usbswitch_common_ops_register(&huawei_switch_extra_ops);
	if (ret)
		hwlog_err("register extra switch ops failed\n");

#ifdef CONFIG_BOOST_5V
	if (di->power_by_5v)
		di->usb_nb.notifier_call = charge_notifier_call;
#endif /* CONFIG_BOOST_5V */

	power_dev_info = power_devices_info_register();
	if (power_dev_info) {
		power_dev_info->dev_name = di->dev->driver->name;
		power_dev_info->dev_id = di->device_id;
		power_dev_info->ver_id = 0;
	}

	return 0;
}

static bool fsa_init_done;
bool get_fsa_init_status(void)
{
	return fsa_init_done;
}

static int fsa9685_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	struct fsa9685_device_info *di = NULL;
	struct device_node *node = NULL;
	int ret = -ERR_NO_DEV;
	int reg_ctl;
	int gpio_value;
	int reg_vendor;
	bool is_dcp = false;
	struct class *switch_class = NULL;
	struct device *new_dev = NULL;

	hwlog_info("probe begin\n");

	if (!i2c_check_functionality(client->adapter,
		I2C_FUNC_SMBUS_BYTE_DATA)) {
		hwlog_err("i2c_check failed\n");
		goto fail_i2c_check_functionality;
	}

	if (g_fsa9685_dev) {
		hwlog_err("chip is already detected\n");
		return ret;
	}

	di = devm_kzalloc(&client->dev, sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	g_fsa9685_dev = di;
	di->dev = &client->dev;
	node = di->dev->of_node;
	di->client = client;
	i2c_set_clientdata(client, di);

	/* device idendify */
	di->device_id = fsa9685_get_device_id();
	if (di->device_id < 0)
		goto fail_i2c_check_functionality;

	fsa9685_select_device_ops(di->device_id);

	/* distingush the chip with different address */
	reg_vendor = fsa9685_read_reg(FSA9685_REG_DEVICE_ID);
	if (reg_vendor < 0) {
		hwlog_err("read FSA9685_REG_DEVICE_ID error\n");
		goto fail_i2c_check_functionality;
	}
	vendor_id = ((unsigned int)reg_vendor) & FAS9685_VENDOR_ID_BIT_MASK;
	if (is_rt8979()) {
		rt8979_accp_enable(true);
		reg_ctl = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
		hwlog_info("DEV_TYPE1 = 0x%x\n", reg_ctl);
		is_dcp = (reg_ctl & FSA9685_DCP_DETECTED) ? true : false;
		if (is_dcp) {
			hwlog_info("reset rt8979\n");
			fsa9685_write_reg(FSA9685_REG_RESET,
				FSA9685_REG_RESET_ENTIRE_IC);
			usleep_range(1000, 1100); /* sleep 1ms */
			fsa9685_write_reg_mask(FSA9685_REG_CONTROL,
				0, FSA9685_SWITCH_OPEN);
			rt8979_accp_enable(true);
		}
		fsa9685_write_reg_mask(RT8979_REG_TIMING_SET_2,
			RT8979_REG_TIMING_SET_2_DCDTIMEOUT,
			RT8979_REG_TIMING_SET_2_DCDTIMEOUT);
		rt8979_init_osc_params();
		ret = rt8979_init_early(di);
		if (ret < 0)
			goto fail_i2c_check_functionality;
	}

	atomic_set(&di->chg_type_det, 0);

	ret = device_create_file(&client->dev, &dev_attr_dump_regs);
	if (ret < 0) {
		hwlog_err("sysfs device_file create failed\n");
		goto fail_i2c_check_functionality;
	}

	/* create a node for phone-off current drain test */
	ret = device_create_file(&client->dev, &dev_attr_switchctrl);
	if (ret < 0) {
		hwlog_err("device_create_file error\n");
		goto fail_get_named_gpio;
	}

	ret = device_create_file(&client->dev, &dev_attr_jigpin_ctrl);
	if (ret < 0) {
		hwlog_err("device_create_file error\n");
		goto fail_create_jigpin_ctrl;
	}

	ret = device_create_file(&client->dev, &dev_attr_fcp_mmi);
	if (ret < 0) {
		hwlog_err("device_create_file error\n");
		goto fail_create_fcp_mmi;
	}

	switch_class = class_create(THIS_MODULE, "usb_switch");
	if (IS_ERR(switch_class)) {
		hwlog_err("create switch class failed\n");
		ret = PTR_ERR(switch_class);
		goto fail_create_link;
	}
	new_dev = device_create(switch_class, NULL, 0, NULL, "switch_ctrl");
	if (!new_dev) {
		hwlog_err("device create failed\n");
		ret = PTR_ERR(new_dev);
		goto fail_create_link;
	}
	ret = sysfs_create_link(&new_dev->kobj, &client->dev.kobj,
		"manual_ctrl");
	if (ret < 0) {
		hwlog_err("create link to switch failed\n");
		goto fail_create_link;
	}

	ret = fsa9685_parse_dts(node, di);
	if (ret) {
		hwlog_err("parse dts failed\n");
		goto fail_create_link;
	}
	/* init lock */
	mutex_init(&di->accp_detect_lock);
	mutex_init(&di->accp_adaptor_reg_lock);
	wakeup_source_init(&di->usb_switch_lock, "usb_switch_wakelock");

	/* init work */
	INIT_DELAYED_WORK(&di->detach_delayed_work, fsa9685_detach_work);
	INIT_DELAYED_WORK(&di->update_type_work, fsa9685_update_type_work);
	INIT_DELAYED_WORK(&di->chg_det_work, fsa9685_chg_det_work);
	INIT_WORK(&di->g_intb_work,
		is_rt8979() ? rt8979_intb_work : fsa9685_intb_work);

	/* create link end */
	gpio = of_get_named_gpio(node, "fairchild_fsa9685,gpio-intb", 0);
	if (gpio < 0) {
		hwlog_err("of_get_named_gpio error\n");
		ret = -EIO;
		goto fail_free_wakelock;
	}
	hwlog_info("get gpio-intb:%d\n", gpio);

	if (is_rt8979()) {
		ret = rt8979_init_later(di);
		if (ret < 0)
			goto fail_free_int_gpio;
	} else {
		client->irq = gpio_to_irq(gpio);
		if (client->irq < 0) {
			hwlog_err("gpio_to_irq error\n");
			ret = -EIO;
			goto fail_free_wakelock;
		}

		ret = gpio_request(gpio, "fsa9685_int");
		if (ret < 0) {
			hwlog_err("gpio_request error\n");
			goto fail_free_wakelock;
		}

		ret = gpio_direction_input(gpio);
		if (ret < 0) {
			hwlog_err("gpio_direction_input error\n");
			goto fail_free_int_gpio;
		}

		ret |= fsa9685_write_reg_mask(FSA9685_REG_CONTROL2, 0, FSA9685_DCD_TIME_OUT_MASK);
		if (ret < 0)
			hwlog_err("write FSA9685_REG_CONTROL2 error\n");
		ret |= fsa9685_write_reg_mask(FSA9685_REG_INTERRUPT_MASK,
			FSA9685_DEVICE_CHANGE, FSA9685_DEVICE_CHANGE);
		if (ret < 0)
			hwlog_err("write FSA9685_REG_INTERRUPT_MASK error\n");

		/* interrupt register */
		ret = request_irq(client->irq,
			fsa9685_irq_handler,
			IRQF_NO_SUSPEND | IRQF_TRIGGER_FALLING,
			"fsa9685_int", client);
		if (ret < 0) {
			hwlog_err("request_irq error\n");
			goto fail_free_int_gpio;
		}
	}

	fsa9685_disable_fcp_intr(di);

	/* clear INT MASK */
	reg_ctl = fsa9685_read_reg(FSA9685_REG_CONTROL);
	if (reg_ctl < 0) {
		hwlog_err("read FSA9685_REG_CONTROL error\n");
		ret = -EIO;
		goto fail_free_int_irq;
	}

	reg_ctl &= (~FSA9685_INT_MASK);
	ret = fsa9685_write_reg(FSA9685_REG_CONTROL, reg_ctl);
	if (ret < 0) {
		hwlog_err("write FSA9685_REG_CONTROL error\n");
		goto fail_free_int_irq;
	}

	ret = fsa9685_write_reg(FSA9685_REG_DCD, 0x0c);
	if (ret < 0) {
		hwlog_err("write FSA9685_REG_DCD error\n");
		goto fail_free_int_irq;
	}

	gpio_value = gpio_get_value(gpio);
	if (gpio_value == 0) {
		hwlog_info("get gpio value 0\n");
		schedule_work(&di->g_intb_work);
	}

	ret = fsa9685_register_init(di);
	if (ret < 0)
		goto fail_free_int_irq;
	fsa_init_done = true;

	hwlog_info("probe end\n");
	return 0;

fail_free_int_irq:
	free_irq(client->irq, client);
fail_free_int_gpio:
	gpio_free(gpio);
fail_free_wakelock:
	wakeup_source_trash(&di->usb_switch_lock);
fail_create_link:
	device_remove_file(&client->dev, &dev_attr_fcp_mmi);
fail_create_fcp_mmi:
	device_remove_file(&client->dev, &dev_attr_jigpin_ctrl);
fail_create_jigpin_ctrl:
	device_remove_file(&client->dev, &dev_attr_switchctrl);
fail_get_named_gpio:
	device_remove_file(&client->dev, &dev_attr_dump_regs);
fail_i2c_check_functionality:
	g_fsa9685_dev = NULL;
	return ret;
}

static void rt8979_remove(struct fsa9685_device_info *chip)
{
	hwlog_info("%s\n", __func__);

	disable_irq(chip->irq);
#ifdef CONFIG_MTK_EXTERNAL_CHARGER_TYPE_DETECT
	cancel_delayed_work_sync(&chip->psy_dwork);
	if (chip->psy)
		power_supply_put(chip->psy);
	kthread_stop(chip->bc12_en_kthread);
	wakeup_source_unregister(chip->bc12_en_ws);

	mutex_destroy(&chip->bc12_lock);
	mutex_destroy(&chip->bc12_en_lock);
#endif /* CONFIG_MTK_EXTERNAL_CHARGER_TYPE_DETECT */
	mutex_destroy(&chip->io_lock);
	mutex_destroy(&chip->hidden_mode_lock);
}

static int fsa9685_remove(struct i2c_client *client)
{
	struct fsa9685_device_info *di = i2c_get_clientdata(client);

	device_remove_file(&client->dev, &dev_attr_dump_regs);
	device_remove_file(&client->dev, &dev_attr_switchctrl);
	device_remove_file(&client->dev, &dev_attr_jigpin_ctrl);
	free_irq(client->irq, client);
	gpio_free(gpio);
	if (di) {
		wakeup_source_trash(&di->usb_switch_lock);
		if (is_rt8979()) {
			rt8979_remove(di);
		} else {
			cancel_delayed_work_sync(&di->chg_det_work);
			cancel_delayed_work_sync(&di->update_type_work);
		}
	}

	return 0;
}

static void rt8979_shutdown(struct fsa9685_device_info *chip)
{
	int ret;

	hwlog_info("%s\n", __func__);

	disable_irq(chip->irq);
#ifdef CONFIG_MTK_EXTERNAL_CHARGER_TYPE_DETECT
	kthread_stop(chip->bc12_en_kthread);
#endif /* CONFIG_MTK_EXTERNAL_CHARGER_TYPE_DETECT */

	ret = fsa9685_read_reg(RT8979_REG_MUIC_CTRL_4);
	if (ret < 0) {
		hwlog_err("fsa9685_read_reg fail\n");
		return;
	}
	ret = fsa9685_write_reg(RT8979_REG_MUIC_CTRL_4,
		ret & RT8979_REG_MUIC_CTRL_4_ENABLEID2_FUNCTION);
	if (ret < 0)
		hwlog_info("shutdown error\n");

	ret = rt8979_reset(chip);
	if (ret < 0)
		hwlog_err("reset rt8979 failed\n");
}

static void fsa9685_shutdown(struct i2c_client *client)
{
	struct fsa9685_device_info *di = i2c_get_clientdata(client);

	if (!di)
		return;

#ifdef CONFIG_BOOST_5V
	if (di->power_by_5v) {
		boost_5v_enable(DISABLE, BOOST_CTRL_FCP);
		dc_set_bst_ctrl(DISABLE);
	}
#endif /* CONFIG_BOOST_5V */
	if (is_rt8979())
		rt8979_shutdown(di);
}

static int rt8979_suspend(struct fsa9685_device_info *chip)
{
	hwlog_info("%s\n", __func__);
	if (device_may_wakeup(chip->dev))
		enable_irq_wake(chip->irq);
	disable_irq(chip->irq);

	return 0;
}

static int fsa9685_suspend(struct device *dev)
{
	struct fsa9685_device_info *di = dev_get_drvdata(dev);

	if (!di)
		return -1;

	if (is_rt8979())
		rt8979_suspend(di);

	return 0;
}

static int rt8979_resume(struct fsa9685_device_info *chip)
{
	hwlog_info("%s\n", __func__);
	enable_irq(chip->irq);
	if (device_may_wakeup(chip->dev))
		disable_irq_wake(chip->irq);

	return 0;
}

static int fsa9685_resume(struct device *dev)
{
	struct fsa9685_device_info *di = dev_get_drvdata(dev);

	if (!di)
		return -1;

	if (is_rt8979())
		rt8979_resume(di);

	return 0;
}

static SIMPLE_DEV_PM_OPS(fsa9685_pm_ops, fsa9685_suspend, fsa9685_resume);

static const struct i2c_device_id fsa9685_i2c_id[] = {
	{"fsa9685", 0},
	{}
};

static struct i2c_driver fsa9685_i2c_driver = {
	.probe = fsa9685_probe,
	.remove = fsa9685_remove,
	.shutdown = fsa9685_shutdown,
	.id_table = fsa9685_i2c_id,
	.driver = {
		.name = "fsa9685",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(switch_fsa9685_ids),
		.pm = &fsa9685_pm_ops,
	},
};

static __init int fsa9685_i2c_init(void)
{
	return i2c_add_driver(&fsa9685_i2c_driver);
}

static __exit void fsa9685_i2c_exit(void)
{
	i2c_del_driver(&fsa9685_i2c_driver);
}

device_initcall_sync(fsa9685_i2c_init);
module_exit(fsa9685_i2c_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("huawei switch module driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
