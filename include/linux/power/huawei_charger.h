/*
 * drivers/power/huawei_charger.h
 *
 * huawei charger driver
 *
 * Copyright (C) 2012-2015 HUAWEI, Inc.
 * Author: HUAWEI, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#include <linux/version.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/notifier.h>
#include <linux/power_supply.h>
#include <chipset_common/hwpower/adapter/adapter_test.h>
#include <chipset_common/hwpower/charger/charger_common_interface.h>
#include <chipset_common/hwpower/common_module/power_dts.h>
#include <chipset_common/hwpower/common_module/power_dsm.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/common_module/power_ui_ne.h>
#include <huawei_platform/power/huawei_charger_common.h>
#include <chipset_common/hwpower/protocol/adapter_protocol.h>
#include <chipset_common/hwpower/protocol/adapter_protocol_scp.h>
#include <chipset_common/hwpower/protocol/adapter_protocol_fcp.h>
#include <chipset_common/hwpower/protocol/adapter_protocol_pd.h>
#include <chipset_common/hwpower/hardware_monitor/water_detect.h>


#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
#include <linux/qpnp/qpnp-adc.h>
#endif

#ifndef _HUAWEI_CHARGER
#define _HUAWEI_CHARGER

/* marco define area */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*
 * Running test result
 * And charge abnormal info
 */
#define CHARGE_STATUS_FAIL      (0 << 0)  /* Indicate running test charging status fail */
#define CHARGE_STATUS_PASS      (1 << 0)  /* Indicate running test charging status pass */
#define BATTERY_FULL            (1 << 1)
#define USB_NOT_PRESENT         (1 << 2)
#define REGULATOR_BOOST         (1 << 3)
#define CHARGE_LIMIT            (1 << 4)
#define BATTERY_HEALTH          (1 << 5)
#define CHARGER_OVP             (1 << 6)
#define OCP_ABNORML             (1 << 7)
#define BATTERY_VOL_ABNORML     (1 << 8)
#define BATTERY_TEMP_ABNORML    (1 << 9)
#define BATTERY_ABSENT          (1 << 10)

#define CHARGE_OCP_THR          (-4500000)    /* charge current abnormal threshold */
#define BATTERY_OCP_THR         6000000     /* discharge current abnormal threshold */
#define BATTERY_VOL_THR_HI      4500000     /* battery voltage abnormal high threshold */
#define BATTERY_VOL_THR_LO      2500000     /* battery voltage abnormal low threshold */
#define BATTERY_TEMP_HI         780         /* battery high temp threshold */
#define BATTERY_TEMP_LO         (-100)        /* battery low temp threshold */
#define WARM_VOL_BUFFER         100         /* warm_bat_mv need have a 100mV buffer */
#define WARM_TEMP_THR           390 /* battery warm temp threshold for running test */
#define HOT_TEMP_THR            600 /* battery overheat threshold for running test */
#define BATT_FULL               100         /* battery full capactiy */
#define COLD_HOT_TEMP_BUFFER    200         /* temp buffer */
#define PASS_MASK               0x1E        /* Running test pass mask */
#define FAIL_MASK               0x7E0       /* Running test fail mask */
#define WARM_VOL_THR            4100    /* battery regulation voltage in mV when warm */
#define HOT_TEMP                650     /* battery hot temp threshold */
#define COLD_TEMP               0       /* battery cold temp threshold */

#define MAX_SIZE                1024

#define SMB_START_CHARGING      0x40
#define SMB_STOP_CHARGING       0x60
#define QPNP_SMBCHARGER_TIMEOUT 30000

#define THERMAL_REASON_SIZE     16
#define ERR_NO_STRING_SIZE      256
#define EOC_DISABLE             FALSE

#define NO_EVENT                (-1)

enum battery_event_type {
	BATTERY_EVENT_HEALTH,
	BATTERY_EVETN_MAX,
};

enum charge_sysfs_type {
	CHARGE_SYSFS_IIN_THERMAL = 0,
	CHARGE_SYSFS_IIN_RUNNINGTEST,
	CHARGE_SYSFS_IIN_RT_CURRENT,
	CHARGE_SYSFS_ENABLE_CHARGER,
	CHARGE_SYSFS_FACTORY_DIAG_CHARGER,
	CHARGE_SYSFS_RUNNING_TEST_STATUS,
	CHARGE_SYSFS_UPDATE_VOLT_NOW,
	CHARGE_SYSFS_IBUS,
	CHARGE_SYSFS_VBUS,
	CHARGE_SYSFS_HIZ,
	CHARGE_SYSFS_CHARGE_TYPE,
	CHARGE_SYSFS_CHARGE_TERM_VOLT_DESIGN,
	CHARGE_SYSFS_CHARGE_TERM_CURR_DESIGN,
	CHARGE_SYSFS_CHARGE_TERM_VOLT_SETTING,
	CHARGE_SYSFS_CHARGE_TERM_CURR_SETTING,
	CHARGE_SYSFS_DBC_CHARGE_CONTROL,
	CHARGE_SYSFS_DBC_CHARGE_DONE,
	CHARGE_SYSFS_ADAPTOR_TEST,
	CHARGE_SYSFS_ADAPTOR_VOLTAGE,
	CHARGE_SYSFS_REGULATION_VOLTAGE,
	CHARGE_SYSFS_PLUGUSB,
	CHARGE_SYSFS_THERMAL_REASON,
	CHARGE_SYSFS_VTERM_DEC,
	CHARGE_SYSFS_ICHG_RATIO,
	CHARGE_SYSFS_CHARGER_ONLINE,
	CHARGE_SYSFS_WATCHDOG_DISABLE,
};

enum iin_thermal_charge_type {
	IIN_THERMAL_CHARGE_TYPE_BEGIN = 0,
	IIN_THERMAL_WCURRENT_5V = IIN_THERMAL_CHARGE_TYPE_BEGIN,
	IIN_THERMAL_WCURRENT_9V,
	IIN_THERMAL_WLCURRENT_5V,
	IIN_THERMAL_WLCURRENT_9V,
	IIN_THERMAL_CHARGE_TYPE_END,
};

enum charge_fault_type {
	CHARGE_FAULT_NON = 0,
	CHARGE_FAULT_BOOST_OCP,
	CHARGE_FAULT_VBAT_OVP,
	CHARGE_FAULT_SCHARGER,
	CHARGE_FAULT_I2C_ERR,
	CHARGE_FAULT_WEAKSOURCE,
	CHARGE_FAULT_CHARGE_DONE,
	CHARGE_FAULT_TOTAL,
};

enum charger_event_type {
	START_SINK = 0,
	STOP_SINK,
	START_SOURCE,
	STOP_SOURCE,
	START_SINK_WIRELESS,
	STOP_SINK_WIRELESS,
	CHARGER_MAX_EVENT,
};

#define CHIP_RESP_TIME	                200
#define FCP_DETECT_DELAY_IN_POWEROFF_CHARGE 2000
#define VBUS_VOL_READ_CNT               3
#define COOL_LIMIT                      11
#define WARM_LIMIT                      44
#define WARM_CUR_RATIO                  35
#define RATIO_BASE      100
#define IBIS_RATIO      120

#define VBUS_VOLTAGE_ABNORMAL_MAX_COUNT   2

enum disable_charger_type {
	CHARGER_SYS_NODE = 0,
	CHARGER_FATAL_ISC_TYPE,
	CHARGER_WIRELESS_TX,
	BATT_CERTIFICATION_TYPE,
	__MAX_DISABLE_CHAGER,
};

struct charger_event_queue {
	enum charger_event_type *event;
	unsigned int num_event;
	unsigned int max_event;
	unsigned int enpos, depos;
	unsigned int overlay, overlay_index;
};

struct charge_iin_regl_lut {
	int total_stage;
	int *iin_regl_para;
};

/* detected type-c protocol current when as a slave and in charge */
enum typec_input_current {
	TYPEC_DEV_CURRENT_DEFAULT = 0,
	TYPEC_DEV_CURRENT_MID,
	TYPEC_DEV_CURRENT_HIGH,
	TYPEC_DEV_CURRENT_NOT_READY,
};

enum hisi_charger_type {
	CHARGER_TYPE_SDP = 0,   /* Standard Downstreame Port */
	CHARGER_TYPE_CDP,       /* Charging Downstreame Port */
	CHARGER_TYPE_DCP,       /* Dedicate Charging Port */
	CHARGER_TYPE_UNKNOWN,   /* non-standard */
	CHARGER_TYPE_NONE,      /* not connected */

	/* other messages */
	PLEASE_PROVIDE_POWER,   /* host mode, provide power */
	CHARGER_TYPE_ILLEGAL,   /* illegal type */
};

struct charge_sysfs_data {
	int iin_rt;
	int iin_rt_curr;
	int hiz_mode;
	int ibus;
	int vbus;
	int inputcurrent;
	int voltage_sys;
	unsigned int adc_conv_rate;
	unsigned int iin_thl;
	unsigned int iin_thl_array[IIN_THERMAL_CHARGE_TYPE_END];
	unsigned int ichg_thl;
	unsigned int ichg_rt;
	unsigned int vterm_rt;
	unsigned int charge_limit;
	unsigned int wdt_disable;
	unsigned int charge_enable;
	unsigned int disable_charger[__MAX_DISABLE_CHAGER];
	unsigned int batfet_disable;
	unsigned int vr_charger_type;
	unsigned int dbc_charge_control;
	unsigned int support_ico;
	unsigned int water_intrused;
	int charge_done_status;
	int charge_done_sleep_status;
	int vterm;
	int iterm;
	int charger_cvcal_value;
	int charger_cvcal_clear;
	int charger_get_cvset;
};

struct charge_device_info {
	struct device *dev;
	struct notifier_block usb_nb;
	struct notifier_block fault_nb;
	struct notifier_block typec_nb;
	struct delayed_work charge_work;
	struct delayed_work plugout_uscp_work;
	struct delayed_work pd_voltage_change_work;
	struct work_struct usb_work;
	struct work_struct fault_work;
	struct charge_sysfs_data sysfs_data;
	struct charge_ops *ops;
	struct charge_core_data *core_data;
	struct hrtimer timer;
	struct mutex mutex_hiz;
#ifdef CONFIG_TCPC_CLASS
	struct notifier_block tcpc_nb;
	struct tcpc_device *tcpc;
	unsigned int tcpc_otg;
	struct mutex tcpc_otg_lock;
	struct pd_dpm_vbus_state *vbus_state;
#endif
	unsigned int pd_input_current;
	unsigned int pd_charge_current;
	enum typec_input_current typec_current_mode;
	enum charge_fault_type charge_fault;
	unsigned int charge_enable;
	unsigned int input_current;
	unsigned int charge_current;
	unsigned int input_typec_current;
	unsigned int charge_typec_current;
	unsigned int enable_current_full;
	unsigned int check_current_full_count;
	unsigned int check_full_count;
	unsigned int start_attemp_ico;
	unsigned int support_standard_ico;
	unsigned int ico_current_mode;
	unsigned int ico_all_the_way;
	unsigned int fcp_vindpm;
	unsigned int hiz_ref;
	unsigned int check_ibias_sleep_time;
	unsigned int need_filter_pd_event;
	u32 force_disable_dc_path;
	u32 scp_adp_normal_chg;
	u32 startup_iin_limit;
	u32 hota_iin_limit;
#ifdef CONFIG_DIRECT_CHARGER
	int support_scp_power;
#endif
	struct spmi_device *spmi;
	struct power_supply *usb_psy;
	struct power_supply *pc_port_psy;
	struct power_supply *batt_psy;
	struct power_supply *chg_psy;
	struct power_supply *bms_psy;
	struct power_supply *bk_batt_psy;
	int chrg_config;
	int factory_diag;
	unsigned int input_event;
	unsigned long event;
	struct delayed_work smb_charger_work;
	int fcp_test_delay;
	struct notifier_block nb;
	int weaksource_cnt;
	struct mutex event_type_lock;
	unsigned int charge_done_maintain_fcp;
	unsigned int term_exceed_time;
	struct work_struct event_work;
	spinlock_t event_spin_lock;
	struct charger_event_queue event_queue;
	unsigned int clear_water_intrused_flag_after_read;
	char thermal_reason[THERMAL_REASON_SIZE];
	int avg_iin_ma;
	int max_iin_ma;
	int current_full_status;
#ifdef CONFIG_HUAWEI_YCABLE
	struct notifier_block ycable_nb;
#endif
#ifdef CONFIG_POGO_PIN
	struct notifier_block pogopin_nb;
#endif
	int iin_regulation_enabled;
	int iin_regl_interval;
	int iin_now;
	int iin_target;
	struct mutex iin_regl_lock;
	struct charge_iin_regl_lut iin_regl_lut;
	struct delayed_work iin_regl_work;
	u32 increase_term_volt_en;
	int en_eoc_max_delay;
};

#ifdef CONFIG_DIRECT_CHARGER
void wired_connect_send_icon_uevent(int icon_type);
void wired_disconnect_send_icon_uevent(void);
#endif

extern int is_usb_ovp(void);
extern  int huawei_handle_charger_event(unsigned long event);
int battery_health_handler(void);
void cap_learning_event_done_notify(void);
#ifdef CONFIG_HUAWEI_CHARGER
struct charge_device_info *get_charger_device_info(void);
int adaptor_cfg_for_wltx_vbus(int vol, int cur);
void charge_set_adapter_voltage(int val,
	unsigned int type, unsigned int delay_time);
int charge_otg_mode_enable(int enable, unsigned int user);
int huawei_charger_get_charge_enable(int *val);
void huawei_charger_update_iin_thermal(void);
#else
static inline struct charge_device_info *get_charger_device_info(void)
{
	return NULL;
}

static inline int adaptor_cfg_for_wltx_vbus(int vol, int cur)
{
	return 0;
}

static inline void charge_set_adapter_voltage(int val,
	unsigned int type, unsigned int delay_time)
{
}

static inline int charge_otg_mode_enable(int enable, unsigned int user)
{
	return 0;
}

static inline int huawei_charger_get_charge_enable(int *val)
{
	return 0;
}

static inline void huawei_charger_update_iin_thermal(void)
{
}
#endif /* CONFIG_HUAWEI_CHARGER */
int get_first_insert(void);
void set_first_insert(int flag);
int get_eoc_max_delay_count(void);
#endif
