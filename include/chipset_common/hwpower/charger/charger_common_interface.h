/* SPDX-License-Identifier: GPL-2.0 */
/*
 * charger_common_interface.h
 *
 * common interface for charger module
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
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

#ifndef _CHARGER_COMMON_INTERFACE_H_
#define _CHARGER_COMMON_INTERFACE_H_

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/power_supply.h>

enum charge_charger_type {
	CHARGER_TYPE_BEGIN = 0,
	CHARGER_TYPE_USB = CHARGER_TYPE_BEGIN, /* 0:SDP */
	CHARGER_TYPE_BC_USB,                   /* 1:CDP */
	CHARGER_TYPE_NON_STANDARD,             /* 2:Unknown */
	CHARGER_TYPE_STANDARD,                 /* 3:DCP */
	CHARGER_TYPE_FCP,                      /* 4:FCP */
	CHARGER_REMOVED,                       /* 5:Not Connected */
	USB_EVENT_OTG_ID,                      /* 6 */
	CHARGER_TYPE_VR,                       /* 7:VR Charger */
	CHARGER_TYPE_TYPEC,                    /* 8:PD Charger */
	CHARGER_TYPE_PD,                       /* 9:PD Charger */
	CHARGER_TYPE_SCP,                      /* 10:SCP Charger */
	CHARGER_TYPE_WIRELESS,                 /* 11:Wireless Charger */
	CHARGER_TYPE_POGOPIN,                  /* 12:POGOPIN Charger */
	CHARGER_TYPE_APPLE_2_1A,               /* 13:2.1A Apple Charger */
	CHARGER_TYPE_APPLE_1_0A,               /* 14:1A Apple Charger */
	CHARGER_TYPE_APPLE_0_5A,               /* 15:0.5A Apple Charger */
	CHARGER_TYPE_BATTERY,                  /* 16 */
	CHARGER_TYPE_UPS,                      /* 17 */
	CHARGER_TYPE_MAINS,                    /* 18 */
	CHARGER_TYPE_ACA,                      /* 19:Accessory Charger Adapters */
	CHARGER_TYPE_PD_DRP,                   /* 20:PD Dual Role Port */
	CHARGER_TYPE_APPLE_BRICK_ID,           /* 21:Apple Charging Method */
	CHARGER_TYPE_HVDCP,                    /* 22:High Voltage DCP */
	CHARGER_TYPE_HVDCP_3,                  /* 23:Efficient High Voltage DCP */
	CHARGER_TYPE_BMS,                      /* 24:Battery Monitor System */
	CHARGER_TYPE_PARALLEL,                 /* 25:Parallel Path */
	CHARGER_TYPE_MAIN,                     /* 26:Main Path */
	CHARGER_TYPE_UFP,                      /* 27:Type-C UFP */
	CHARGER_TYPE_DFP,                      /* 28:Type-C DFP */
	CHARGER_TYPE_CHARGE_PUMP,              /* 29:Charge Pump */
	CHARGER_TYPE_END,
};

/* define hiz mode of charger */
#define HIZ_MODE_ENABLE                   1
#define HIZ_MODE_DISABLE                  0

/* define charging done of charger */
#define CHARGE_DONE_NON                   0
#define CHARGE_DONE                       1

/* define sleep state on charging done of charger */
#define CHARGE_DONE_SLEEP_DISABLED        0
#define CHARGE_DONE_SLEEP_ENABLED         1

/* define wakelock state of charger */
#define CHARGE_WAKELOCK_NEED              0
#define CHARGE_WAKELOCK_NO_NEED           1

/* define otg state of charger */
#define CHARGE_OTG_ENABLE                 1
#define CHARGE_OTG_DISABLE                0

/* define ic state of charger */
#define CHARGE_IC_GOOD                    0
#define CHARGE_IC_BAD                     1

/* define charging state of charger */
#define CHAGRE_STATE_NORMAL               0x00
#define CHAGRE_STATE_VBUS_OVP             0x01
#define CHAGRE_STATE_NOT_PG               0x02
#define CHAGRE_STATE_WDT_FAULT            0x04
#define CHAGRE_STATE_BATT_OVP             0x08
#define CHAGRE_STATE_CHRG_DONE            0x10
#define CHAGRE_STATE_INPUT_DPM            0x20
#define CHAGRE_STATE_NTC_FAULT            0x40
#define CHAGRE_STATE_CV_MODE              0x80

/* define watchdog timer of charger */
#define CHAGRE_WDT_DISABLE                0
#define CHAGRE_WDT_40S                    40
#define CHAGRE_WDT_80S                    80
#define CHAGRE_WDT_160S                   160
#define CHARGE_SYS_WDT_TIMEOUT            180

/* define charging current gears of charger */
#define CHARGE_CURRENT_0000_MA            0
#define CHARGE_CURRENT_0100_MA            100
#define CHARGE_CURRENT_0150_MA            150
#define CHARGE_CURRENT_0500_MA            500
#define CHARGE_CURRENT_0800_MA            800
#define CHARGE_CURRENT_1000_MA            1000
#define CHARGE_CURRENT_1200_MA            1200
#define CHARGE_CURRENT_1900_MA            1900
#define CHARGE_CURRENT_2000_MA            2000
#define CHARGE_CURRENT_4000_MA            4000
#define CHARGE_CURRENT_MAX_MA             32767

/* define charging voltage gears of charger */
#define CHARGE_VOLTAGE_4360_MV            4360
#define CHARGE_VOLTAGE_4520_MV            4520
#define CHARGE_VOLTAGE_4600_MV            4600
#define CHARGE_VOLTAGE_5000_MV            5000
#define CHARGE_VOLTAGE_6300_MV            6300
#define CHARGE_VOLTAGE_6500_MV            6500

/* define voltage gears of vbus */
#define VBUS_VOLTAGE_6500_MV              6500
#define VBUS_VOLTAGE_7000_MV              7000
#define VBUS_VOLTAGE_13400_MV             13400

/* define voltage gears of battery */
#define BATTERY_VOLTAGE_0000_MV           0
#define BATTERY_VOLTAGE_0200_MV           200
#define BATTERY_VOLTAGE_3200_MV           3200
#define BATTERY_VOLTAGE_3400_MV           3400
#define BATTERY_VOLTAGE_4100_MV           4100
#define BATTERY_VOLTAGE_4200_MV           4200
#define BATTERY_VOLTAGE_4350_MV           4350
#define BATTERY_VOLTAGE_4500_MV           4500
#define BATTERY_VOLTAGE_MIN_MV            (-32767)
#define BATTERY_VOLTAGE_MAX_MV            32767

/* define voltage gears of adapter */
#define ADAPTER_0V                        0
#define ADAPTER_5V                        5
#define ADAPTER_7V                        7
#define ADAPTER_9V                        9
#define ADAPTER_12V                       12
#define ADAPTER_15V                       15

/* state with monitor work of charger */
#define CHARGE_MONITOR_WORK_NEED_START    0
#define CHARGE_MONITOR_WORK_NEED_STOP     1

enum charge_reset_adapter_mode {
	RESET_ADAPTER_DIRECT_MODE,
	RESET_ADAPTER_SET_MODE,
	RESET_ADAPTER_CLEAR_MODE,
};

enum charge_reset_adapter_source {
	RESET_ADAPTER_SOURCE_BEGIN = 0,
	RESET_ADAPTER_SOURCE_SYSFS = RESET_ADAPTER_SOURCE_BEGIN,
	RESET_ADAPTER_SOURCE_WLTX,
	RESET_ADAPTER_SOURCE_END,
};

struct charge_extra_ops {
	bool (*check_ts)(void);
	bool (*check_otg_state)(void);
	unsigned int (*get_charger_type)(void);
	int (*set_state)(int state);
	int (*get_charge_current)(void);
	int (*get_batt_by_usb_id)(void);
};

struct charge_switch_ops {
	unsigned int (*get_charger_type)(void);
};

struct charge_init_data {
	unsigned int charger_type;
	int vbus;
};

const char *charge_get_charger_type_name(unsigned int type);

#if (defined(CONFIG_HUAWEI_CHARGER_AP) || defined(CONFIG_HUAWEI_CHARGER) || defined(CONFIG_FCP_CHARGER))
int charge_extra_ops_register(struct charge_extra_ops *ops);
int charge_switch_ops_register(struct charge_switch_ops *ops);
int charge_init_chip(struct charge_init_data *data);
int charge_check_charger_plugged(void);
int charge_set_hiz_enable(int enable);
bool charge_get_hiz_state(void);
int charge_get_ibus(void);
int charge_get_vbus(void);
int charge_get_vusb(void);
int charge_get_done_type(void);
unsigned int charge_get_charger_type(void);
void charge_set_charger_type(unsigned int type);
unsigned int charge_get_charger_source(void);
void charge_set_charger_source(unsigned int source);
unsigned int charge_get_charging_state(void);
int charge_set_vbus_vset(u32 volt);
int charge_get_charger_online(void);
void charge_set_charger_online(int online);
/* mivr: minimum input voltage regulation */
int charge_set_mivr(u32 volt);
int charge_set_batfet_disable(int val);
int charge_set_watchdog(int time);
int charge_reset_watchdog(void);
void charge_kick_watchdog(void);
void charge_disable_watchdog(void);
int charge_switch_get_charger_type(void);
unsigned int charge_convert_charger_type(unsigned int type);
int charge_set_buck_iterm(unsigned int value);
int charge_set_buck_fv_delta(unsigned int value);
int charge_get_battery_current_avg(void);
unsigned int charge_get_wakelock_flag(void);
void charge_set_wakelock_flag(unsigned int flag);
unsigned int charge_get_monitor_work_flag(void);
void charge_set_monitor_work_flag(unsigned int flag);
unsigned int charge_get_quicken_work_flag(void);
void charge_reset_quicken_work_flag(void);
void charge_update_quicken_work_flag(void);
unsigned int charge_get_run_time(void);
void charge_set_run_time(unsigned int time);
unsigned int charge_get_reset_adapter_source(void);
void charge_set_reset_adapter_source(unsigned int mode, unsigned int value);
bool charge_need_ignore_plug_event(void);
void charge_ignore_plug_event(bool state);
void charge_update_charger_remove_type(void);
void charge_update_buck_iin_thermal(void);
#else
static inline int charge_extra_ops_register(struct charge_extra_ops *ops)
{
	return -EINVAL;
}

static inline int charge_switch_ops_register(struct charge_switch_ops *ops)
{
	return -EINVAL;
}

static inline int charge_init_chip(struct charge_init_data *data)
{
	return -EINVAL;
}

static inline int charge_check_charger_plugged(void)
{
	return -EINVAL;
}

static inline int charge_set_hiz_enable(int enable)
{
	return 0;
}

static inline bool charge_get_hiz_state(void)
{
	return false;
}

static inline int charge_get_ibus(void)
{
	return 0;
}

static inline int charge_get_vbus(void)
{
	return 0;
}

static inline int charge_get_vusb(void)
{
	return -EINVAL;
}

static inline int charge_get_done_type(void)
{
	return CHARGE_DONE_NON;
}

static inline unsigned int charge_get_charger_type(void)
{
	return CHARGER_REMOVED;
}

static inline void charge_set_charger_type(unsigned int type)
{
}

static inline unsigned int charge_get_charger_source(void)
{
	return POWER_SUPPLY_TYPE_BATTERY;
}

static inline void charge_set_charger_source(unsigned int source)
{
}

static inline unsigned int charge_get_charging_state(void)
{
	return CHAGRE_STATE_NORMAL;
}

static inline int charge_set_vbus_vset(u32 volt)
{
	return -EINVAL;
}

static inline int charge_get_charger_online(void)
{
	return -EINVAL;
}

static inline void charge_set_charger_online(int online)
{
}

/* mivr: minimum input voltage regulation */
static inline int charge_set_mivr(u32 volt)
{
	return -EINVAL;
}

static inline int charge_set_batfet_disable(int val)
{
	return -EINVAL;
}

static inline int charge_set_watchdog(int time)
{
	return -EINVAL;
}

static inline int charge_reset_watchdog(void)
{
	return -EINVAL;
}

static inline void charge_kick_watchdog(void)
{
}

static inline void charge_disable_watchdog(void)
{
}

static inline int charge_switch_get_charger_type(void)
{
	return -EINVAL;
}

static inline unsigned int charge_convert_charger_type(unsigned int type)
{
	return CHARGER_REMOVED;
}

static inline int charge_set_buck_iterm(unsigned int value)
{
	return 0;
}

static inline int charge_set_buck_fv_delta(unsigned int value)
{
	return 0;
}

static inline int charge_get_battery_current_avg(void)
{
	return 0;
}

static inline unsigned int charge_get_wakelock_flag(void)
{
	return CHARGE_WAKELOCK_NO_NEED;
}

static inline void charge_set_wakelock_flag(unsigned int flag)
{
}

static inline unsigned int charge_get_monitor_work_flag(void)
{
	return CHARGE_MONITOR_WORK_NEED_START;
}

static inline void charge_set_monitor_work_flag(unsigned int flag)
{
}

static inline unsigned int charge_get_quicken_work_flag(void)
{
	return 0;
}

static inline void charge_reset_quicken_work_flag(void)
{
}

static inline void charge_update_quicken_work_flag(void)
{
}

static inline unsigned int charge_get_run_time(void)
{
	return 0;
}

static inline void charge_set_run_time(unsigned int time)
{
}

static inline unsigned int charge_get_reset_adapter_source(void)
{
	return 0;
}

static inline void charge_set_reset_adapter_source(unsigned int mode, unsigned int value)
{
}

static inline bool charge_need_ignore_plug_event(void)
{
	return false;
}

static inline void charge_ignore_plug_event(bool state)
{
}

static inline void charge_update_charger_remove_type(void)
{
}

static inline void charge_update_buck_iin_thermal(void)
{
}
#endif /* CONFIG_HUAWEI_CHARGER_AP || CONFIG_HUAWEI_CHARGER || CONFIG_FCP_CHARGER */

#endif /* _CHARGER_COMMON_INTERFACE_H_ */
