/*
 * huawei_charger.c
 *
 * huawei charger driver
 *
 * Copyright (c) 2012-2019 Huawei Technologies Co., Ltd.
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/usb/otg.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/power_supply.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/notifier.h>
#include <linux/mutex.h>
#include <linux/spmi.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>
#include <linux/power/huawei_charger.h>
#include <linux/power/huawei_battery.h>
#include <linux/version.h>
#include <chipset_common/hwpower/common_module/power_log.h>
#include <chipset_common/hwpower/common_module/power_sysfs.h>
#include <huawei_platform/power/huawei_charger_adaptor.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/common_module/power_interface.h>
#include <chipset_common/hwpower/common_module/power_supply_application.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <chipset_common/hwpower/common_module/power_cmdline.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
#include <huawei_platform/hwpower/common_module/power_glink.h>
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */
#include <huawei_platform/hwpower/common_module/power_platform.h>
#include <huawei_platform/hwpower/charger/charger_loginfo.h>
#include <chipset_common/hwpower/hardware_monitor/ship_mode.h>

#define DEFAULT_FCP_TEST_DELAY	6000
#define DEFAULT_IIN_CURRENT	1000
#define MAX_CURRENT	3000
#define MIN_CURRENT	100
#define RUNN_TEST_TEMP	250
#define DEFAULT_IIN_THL 2300
#define FCP_READ_DELAY  100
#define INPUT_NUMBER_BASE 10
#define DCP_INPUT_CURRENT 2000
#ifdef CONFIG_HLTHERM_RUNTEST
#define HLTHERM_CURRENT 2000
#endif

static struct charge_device_info *g_di;

struct device *charge_dev = NULL;
static struct charge_device_info *g_charger_device_para = NULL;

struct charge_device_info *get_charger_device_info(void)
{
	if (!g_charger_device_para)
		return NULL;
	return g_charger_device_para;
}

static struct kobject *g_sysfs_poll = NULL;

static ssize_t get_poll_charge_start_event(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct charge_device_info *chip = g_charger_device_para;

	if (!dev || !attr || !buf) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	if (chip)
		return snprintf(buf, PAGE_SIZE, "%d\n", chip->input_event);
	else
		return 0;
}

#define POLL_CHARGE_EVENT_MAX 3000
static ssize_t set_poll_charge_event(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t count)
{
	struct charge_device_info *chip = g_charger_device_para;
	long val = 0;

	if (!dev || !attr || !buf) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	if (chip) {
		if ((kstrtol(buf, INPUT_NUMBER_BASE, &val) < 0) ||
			(val < 0) || (val > POLL_CHARGE_EVENT_MAX))
			return -EINVAL;
		chip->input_event = val;
		power_event_notify_sysfs(g_sysfs_poll, NULL, "poll_charge_start_event");
	}
	return count;
}

static DEVICE_ATTR(poll_charge_start_event, (S_IWUSR | S_IRUGO),
				get_poll_charge_start_event,
				set_poll_charge_event);
static ssize_t get_poll_charge_done_event(struct device *dev,
					  struct device_attribute *attr,
					  char *buf)
{
	if (!dev || !attr || !buf) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static DEVICE_ATTR(poll_charge_done_event, (S_IWUSR | S_IRUGO),
		   get_poll_charge_done_event, NULL);
static ssize_t get_poll_fcc_learn_event(struct device *dev,
					  struct device_attribute *attr,
					  char *buf)
{
	if (!dev || !attr || !buf) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static DEVICE_ATTR(poll_fcc_learn_event, (S_IWUSR | S_IRUGO),
		   get_poll_fcc_learn_event, NULL);

static int charge_event_poll_register(struct device *dev)
{
	int ret;

	if (!dev) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	ret = sysfs_create_file(&dev->kobj, &dev_attr_poll_charge_start_event.attr);
	if (ret) {
		pr_err("fail to create poll node for %s\n", dev->kobj.name);
		return ret;
	}
	ret = sysfs_create_file(&dev->kobj, &dev_attr_poll_charge_done_event.attr);
	if (ret) {
		pr_err("fail to create poll node for %s\n", dev->kobj.name);
		return ret;
	}
	ret = sysfs_create_file(&dev->kobj, &dev_attr_poll_fcc_learn_event.attr);
	if (ret) {
		pr_err("fail to create poll node for %s\n", dev->kobj.name);
		return ret;
	}

	g_sysfs_poll = &dev->kobj;
	return ret;
}

void cap_learning_event_done_notify(void)
{
	struct charge_device_info *chip = g_charger_device_para;

	if (!chip) {
		pr_info("smb device is not init, do nothing!\n");
		return;
	}

	power_event_notify_sysfs(g_sysfs_poll, NULL, "poll_fcc_learn_event");
}

void cap_charge_done_event_notify(void)
{
	struct charge_device_info *chip = g_charger_device_para;

	if (!chip) {
		pr_info("smb device is not init, do nothing\n");
		return;
	}

	power_event_notify_sysfs(g_sysfs_poll, NULL, "poll_charge_done_event");
}

static void charge_event_notify(unsigned int event)
{
	struct charge_device_info *chip = g_charger_device_para;

	if (!chip) {
		pr_info("smb device is not init, do nothing!\n");
		return;
	}
	/* avoid notify charge stop event continuously without charger inserted */
	if ((chip->input_event != event) || (event == SMB_START_CHARGING)) {
		chip->input_event = event;
		power_event_notify_sysfs(g_sysfs_poll, NULL, "poll_charge_start_event");
	}
}

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
int huawei_charger_get_charge_enable(int *val)
{
	if (!val)
		return -EINVAL;

	return power_glink_get_property_value(POWER_GLINK_PROP_ID_SET_CHARGE_ENABLE, (u32 *)val, GLINK_DATA_ONE);
}

static int huawei_charger_set_rt_current(struct charge_device_info *di, int val)
{
	int iin_rt_curr;
	u32 data[GLINK_DATA_THREE];
#ifdef CONFIG_HLTHERM_RUNTEST
	int curr = HLTHERM_CURRENT;
#else
	int curr = MIN_CURRENT;
#endif

	if (val == 0)
		iin_rt_curr = DCP_INPUT_CURRENT;
	else if ((val <= MIN_CURRENT) && (val > 0))
		iin_rt_curr = curr;
	else
		iin_rt_curr = val;

	data[GLINK_DATA_ZERO] = ICL_AGGREGATOR_RT;
	data[GLINK_DATA_ONE] = AGGREGATOR_VOTE_ENABLE;
	data[GLINK_DATA_TWO] = iin_rt_curr;

	power_glink_set_property_value(POWER_GLINK_PROP_ID_SET_USB_ICL, data, GLINK_DATA_THREE);
	di->sysfs_data.iin_rt_curr = iin_rt_curr;
	pr_info("[%s]:set iin_rt_curr %d\n", __func__, iin_rt_curr);
	return 0;
}

static int huawei_charger_set_hz_mode(struct charge_device_info *di, int val)
{
	if (!di)
		return -EINVAL;

	charge_set_hiz_enable(val);
	di->sysfs_data.hiz_mode = val;
	return 0;
}

static int huawei_charger_set_iin_thermal_limit(unsigned int value)
{
	u32 id = POWER_GLINK_PROP_ID_SET_USB_ICL;
	u32 data[GLINK_DATA_THREE];

	data[GLINK_DATA_ZERO] = ICL_AGGREGATOR_HLOS_THERMAL;
	data[GLINK_DATA_ONE] = AGGREGATOR_VOTE_ENABLE;
	data[GLINK_DATA_TWO] = (value > 0) ? value : DCP_INPUT_CURRENT;

	return power_glink_set_property_value(id, data, GLINK_DATA_THREE);
}

static int get_iin_thermal_charge_type(void)
{
	int vol = 0;
	struct charge_device_info *di = g_charger_device_para;

	if (!di)
		return IIN_THERMAL_WCURRENT_5V;

	if (charge_get_charger_type() == CHARGER_TYPE_WIRELESS) {
		power_supply_get_int_property_value("wireless",
			POWER_SUPPLY_PROP_VOLTAGE_NOW, &vol);
		return (vol / POWER_UV_PER_MV / POWER_MV_PER_V < ADAPTER_7V) ?
			IIN_THERMAL_WLCURRENT_5V : IIN_THERMAL_WLCURRENT_9V;
	} else {
		power_supply_get_int_property_value("usb",
			POWER_SUPPLY_PROP_VOLTAGE_NOW, &vol);
		return (vol / POWER_UV_PER_MV / POWER_MV_PER_V < ADAPTER_7V) ?
			IIN_THERMAL_WCURRENT_5V : IIN_THERMAL_WCURRENT_9V;
	}
}
#else
int huawei_charger_get_charge_enable(int *val)
{
	if (!val)
		return -EINVAL;

	return power_supply_get_int_property_value("battery", POWER_SUPPLY_PROP_CHARGING_ENABLED, val);
}

static int huawei_charger_set_rt_current(struct charge_device_info *di, int val)
{
	int iin_rt_curr;

	if (!di || !power_supply_check_psy_available("huawei_charge", &di->chg_psy)) {
		pr_err("charge_device is not ready! cannot set runningtest\n");
		return -EINVAL;
	}

	if (val == 0) {
		iin_rt_curr = power_supply_return_int_property_value_with_psy(
			di->chg_psy, POWER_SUPPLY_PROP_INPUT_CURRENT_MAX);
		pr_info("%s  rt_curr =%d\n", __func__, iin_rt_curr);
	} else if ((val <= MIN_CURRENT) && (val > 0)) {
#ifndef CONFIG_HLTHERM_RUNTEST
		iin_rt_curr = MIN_CURRENT;
#else
		iin_rt_curr = HLTHERM_CURRENT;
#endif
	} else {
		iin_rt_curr = val;
	}

	power_supply_set_int_property_value_with_psy(di->chg_psy,
		POWER_SUPPLY_PROP_INPUT_CURRENT_MAX, iin_rt_curr);

	di->sysfs_data.iin_rt_curr = iin_rt_curr;
	pr_info("[%s]:set iin_rt_curr %d\n", __func__, iin_rt_curr);
	return 0;
}

static int huawei_charger_set_hz_mode(struct charge_device_info *di, int val)
{
	int hiz_mode;

	if (!di || !power_supply_check_psy_available("battery", &di->batt_psy)) {
		pr_err("charge_device is not ready! cannot set runningtest\n");
		return -EINVAL;
	}
	hiz_mode = !!val;
	power_supply_set_int_property_value_with_psy(di->batt_psy,
		POWER_SUPPLY_PROP_HIZ_MODE, hiz_mode);
	di->sysfs_data.hiz_mode = hiz_mode;
	pr_info("[%s]:set hiz_mode %d\n", __func__, hiz_mode);
	return 0;
}

static int huawei_charger_set_iin_thermal_limit(unsigned int value)
{
	struct charge_device_info *di = g_charger_device_para;

	if (!di)
		return -EINVAL;

	return power_supply_set_int_property_value_with_psy(di->chg_psy,
		POWER_SUPPLY_PROP_IIN_THERMAL, value);
}

static int get_iin_thermal_charge_type(void)
{
	int vol;
	struct charge_device_info *di = g_charger_device_para;

	if (!di)
		return IIN_THERMAL_WCURRENT_5V;

	vol = power_supply_app_get_usb_voltage_now();

	if (charge_get_charger_type() == CHARGER_TYPE_WIRELESS)
		return (vol / POWER_MV_PER_V < ADAPTER_7V) ?
			IIN_THERMAL_WLCURRENT_5V : IIN_THERMAL_WLCURRENT_9V;

	return (vol / POWER_MV_PER_V < ADAPTER_7V) ?
		IIN_THERMAL_WCURRENT_5V : IIN_THERMAL_WCURRENT_9V;
}
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */

static void update_iin_thermal(struct charge_device_info *di)
{
	int idx;

	if (!di || !power_supply_check_psy_available("huawei_charge", &di->chg_psy)) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return;
	}

	idx = get_iin_thermal_charge_type();
	di->sysfs_data.iin_thl = di->sysfs_data.iin_thl_array[idx];
	huawei_charger_set_iin_thermal_limit(di->sysfs_data.iin_thl);
	pr_info("update iin_thermal current: %d, type: %d",
		di->sysfs_data.iin_thl, idx);
}

static void smb_update_status(struct charge_device_info *di)
{
	unsigned int events = 0;
	int charging_enabled;
	int battery_present;

	if (!di || !power_supply_check_psy_available("battery", &di->batt_psy)) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return;
	}

	charging_enabled = power_supply_return_int_property_value_with_psy(
		di->batt_psy, POWER_SUPPLY_PROP_CHARGING_ENABLED);
	battery_present = power_supply_return_int_property_value_with_psy(
		di->batt_psy, POWER_SUPPLY_PROP_PRESENT);
	if (!battery_present)
		events = SMB_STOP_CHARGING;
	if (!events) {
		if (charging_enabled && battery_present)
			events = SMB_START_CHARGING;
	}
	charge_event_notify(events);
	update_iin_thermal(di);
}

static void smb_charger_work(struct work_struct *work)
{
	struct charge_device_info *chip = NULL;

	if (!work) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return;
	}

	chip = container_of(work, struct charge_device_info, smb_charger_work.work);
	if (!chip) {
		pr_err("%s: Cannot get chip, fatal error\n", __func__);
		return;
	}

	smb_update_status(chip);
	schedule_delayed_work(&chip->smb_charger_work,
				msecs_to_jiffies(QPNP_SMBCHARGER_TIMEOUT));
}

int huawei_handle_charger_event(unsigned long event)
{
	struct charge_device_info *di = NULL;
    /* give a chance for bootup with usb present case */
	static int smb_charge_work_flag = 1;
	bool flag = TRUE;

	di = g_charger_device_para;
	if (!di) {
		pr_err(" %s charge ic  is unregister !\n", __func__);
		return -EINVAL;
	}
	if ((event != POWER_NE_THIRDPLAT_CHARGING_STOP) && smb_charge_work_flag) {
		charge_event_notify(SMB_START_CHARGING);
		flag = schedule_delayed_work(&di->smb_charger_work, msecs_to_jiffies(0));
		if (!flag) {
			cancel_delayed_work_sync(&di->smb_charger_work);
			flag = schedule_delayed_work(&di->smb_charger_work, msecs_to_jiffies(0));
			pr_info("%s: flag = %d\n", __func__, flag);
		}
		smb_charge_work_flag = 0;
		pr_info("start charge work, event %lu\n", event);
	}
	if (event == POWER_NE_THIRDPLAT_CHARGING_STOP) {
		charge_event_notify(SMB_STOP_CHARGING);
		cancel_delayed_work_sync(&di->smb_charger_work);
		smb_charge_work_flag = 1;
		pr_info("stop charge work, event %lu\n", event);
	}
	return 0;
}

void huawei_charger_update_iin_thermal(void)
{
	struct charge_device_info *di = g_charger_device_para;

	if (!di)
		return;

	update_iin_thermal(di);
}

static int huawei_charger_set_runningtest(struct charge_device_info *di, int val)
{
	int iin_rt;

	if (!di || !power_supply_check_psy_available("battery", &di->batt_psy)) {
		pr_err("charge_device is not ready! cannot set runningtest\n");
		return -EINVAL;
	}

	power_supply_set_int_property_value_with_psy(di->batt_psy,
		POWER_SUPPLY_PROP_CHARGING_ENABLED, !!val);
	iin_rt = power_supply_return_int_property_value_with_psy(
		di->batt_psy, POWER_SUPPLY_PROP_CHARGING_ENABLED);
	di->sysfs_data.iin_rt = iin_rt;
	pr_info("[%s]:iin_rt = %d\n", __func__, val);
	return 0;
}

static int huawei_charger_set_adaptor_change(struct charge_device_info *di, int val)
{
	if (!di || !power_supply_check_psy_available("battery", &di->batt_psy)) {
		pr_err("charge_device is not ready! cannot set factory diag\n");
		return -EINVAL;
	}
	power_supply_set_int_property_value_with_psy(di->batt_psy,
		POWER_SUPPLY_PROP_ADAPTOR_VOLTAGE, val);
	pr_info("[%s]:set adpator volt %d\n", __func__, val);
	return 0;
}

#define charge_sysfs_field(_name, n, m, store) \
{ \
	.attr = __ATTR(_name, m, charge_sysfs_show, store), \
	.name = CHARGE_SYSFS_##n, \
}

#define charge_sysfs_field_rw(_name, n) \
	charge_sysfs_field(_name, n, S_IWUSR | S_IRUGO, \
		charge_sysfs_store)

#define charge_sysfs_field_ro(_name, n) \
	charge_sysfs_field(_name, n, S_IRUGO, NULL)

static ssize_t charge_sysfs_show(struct device *dev,
				 struct device_attribute *attr, char *buf);
static ssize_t charge_sysfs_store(struct device *dev,
				  struct device_attribute *attr, const char *buf, size_t count);

struct charge_sysfs_field_info {
	char name;
	struct device_attribute    attr;
};


static struct charge_sysfs_field_info charge_sysfs_field_tbl[] = {
	charge_sysfs_field_rw(iin_thermal,       IIN_THERMAL),
	charge_sysfs_field_rw(iin_runningtest,    IIN_RUNNINGTEST),
	charge_sysfs_field_rw(iin_rt_current,   IIN_RT_CURRENT),
	charge_sysfs_field_rw(enable_hiz, HIZ),
	charge_sysfs_field_rw(enable_charger,    ENABLE_CHARGER),
	charge_sysfs_field_rw(shutdown_watchdog, WATCHDOG_DISABLE),
	charge_sysfs_field_rw(factory_diag,    FACTORY_DIAG_CHARGER),
	charge_sysfs_field_rw(update_volt_now,    UPDATE_VOLT_NOW),
	charge_sysfs_field_ro(ibus,    IBUS),
	charge_sysfs_field_rw(adaptor_voltage, ADAPTOR_VOLTAGE),
	charge_sysfs_field_ro(chargerType, CHARGE_TYPE),

};

static struct attribute *charge_sysfs_attrs[ARRAY_SIZE(charge_sysfs_field_tbl) + 1];

static const struct attribute_group charge_sysfs_attr_group = {
	.attrs = charge_sysfs_attrs,
};

/* initialize charge_sysfs_attrs[] for charge attribute */
static void charge_sysfs_init_attrs(void)
{
	int i;
	int limit = ARRAY_SIZE(charge_sysfs_field_tbl);

	for (i = 0; i < limit; i++)
		charge_sysfs_attrs[i] = &charge_sysfs_field_tbl[i].attr.attr;

	charge_sysfs_attrs[limit] = NULL; /* Has additional entry for this */
}

/*
 * get the current device_attribute from charge_sysfs_field_tbl
 * by attr's name
 */
static struct charge_sysfs_field_info *charge_sysfs_field_lookup(const char *name)
{
	int i;
	int limit = ARRAY_SIZE(charge_sysfs_field_tbl);

	if (!name) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return NULL;
	}

	for (i = 0; i < limit; i++) {
		if (!strcmp(name, charge_sysfs_field_tbl[i].attr.attr.name))
			break;
	}

	if (i >= limit)
		return NULL;

	return &charge_sysfs_field_tbl[i];
}

/* show the value for all charge device's node */
static ssize_t charge_sysfs_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	int ret = 0;
	struct charge_sysfs_field_info *info = NULL;
	struct charge_device_info *di = NULL;
	int ibus = 0;

	if (!dev || !attr || !buf) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	di = dev_get_drvdata(dev);
	info = charge_sysfs_field_lookup(attr->attr.name);
	if (!di || !info)
		return -EINVAL;

	switch (info->name) {
	case CHARGE_SYSFS_IIN_THERMAL:
		ret = snprintf(buf, MAX_SIZE, "%u\n", di->sysfs_data.iin_thl);
		break;
	case CHARGE_SYSFS_IIN_RUNNINGTEST:
		ret = snprintf(buf, MAX_SIZE, "%u\n", di->sysfs_data.iin_rt);
		break;
	case CHARGE_SYSFS_IIN_RT_CURRENT:
		ret = snprintf(buf, MAX_SIZE, "%u\n", di->sysfs_data.iin_rt_curr);
		break;
	case CHARGE_SYSFS_ENABLE_CHARGER:
#ifndef CONFIG_HLTHERM_RUNTEST
		ret = snprintf(buf, MAX_SIZE, "%u\n", di->chrg_config);
		break;
#endif
	case CHARGE_SYSFS_WATCHDOG_DISABLE:
		return snprintf(buf, PAGE_SIZE, "%d\n", di->sysfs_data.wdt_disable);
	case CHARGE_SYSFS_FACTORY_DIAG_CHARGER:
		ret = snprintf(buf, MAX_SIZE, "%u\n", di->factory_diag);
		break;
	case CHARGE_SYSFS_UPDATE_VOLT_NOW:
		/* always return 1 when reading UPDATE_VOLT_NOW property */
		ret = snprintf(buf, MAX_SIZE, "%u\n", 1);
		break;
	case CHARGE_SYSFS_IBUS:
		if (power_supply_check_psy_available("battery", &di->batt_psy))
			ibus = power_supply_return_int_property_value_with_psy(di->usb_psy,
				POWER_SUPPLY_PROP_INPUT_CURRENT_NOW);
		return snprintf(buf, MAX_SIZE, "%d\n", ibus);
	case CHARGE_SYSFS_HIZ:
		ret = snprintf(buf, MAX_SIZE, "%u\n", di->sysfs_data.hiz_mode);
		break;
	case CHARGE_SYSFS_CHARGE_TYPE:
		return snprintf(buf, PAGE_SIZE, "%u\n", charge_get_charger_type());
		break;
	case CHARGE_SYSFS_ADAPTOR_VOLTAGE:
		if (charge_get_reset_adapter_source())
			return snprintf(buf, PAGE_SIZE, "%d\n", ADAPTER_5V);
		else
			return snprintf(buf, PAGE_SIZE, "%d\n", ADAPTER_9V);
		break;
	default:
		pr_err("(%s)NODE ERR!!HAVE NO THIS NODE:%d\n", __func__, info->name);
		break;
	}

	return ret;
}

/* set the value for charge_data's node which is can be written */
static ssize_t charge_sysfs_store(struct device *dev, struct device_attribute *attr,
					const char *buf, size_t count)
{
	long val = 0;
	struct charge_sysfs_field_info *info = NULL;
	struct charge_device_info *di = NULL;

	if (!dev || !attr || !buf) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	di = dev_get_drvdata(dev);
	if (!di) {
		pr_err("%s: Cannot get charge device info, fatal error\n", __func__);
		return -EINVAL;
	}

	info = charge_sysfs_field_lookup(attr->attr.name);
	if (!info)
		return -EINVAL;

	switch (info->name) {
	/* hot current limit function */
	case CHARGE_SYSFS_IIN_THERMAL:
		if ((kstrtol(buf, INPUT_NUMBER_BASE, &val) < 0) ||
			(val < 0) || (val > MAX_CURRENT))
				return -EINVAL;
		if (power_supply_check_psy_available("huawei_charge", &di->chg_psy)) {
			power_supply_set_int_property_value_with_psy(di->chg_psy,
				POWER_SUPPLY_PROP_IIN_THERMAL, val);
			di->sysfs_data.iin_thl = val;
			pr_info("THERMAL set input current = %ld\n", val);
		}
		break;
	/* running test charging/discharge function */
	case CHARGE_SYSFS_IIN_RUNNINGTEST:
		if ((kstrtol(buf, INPUT_NUMBER_BASE, &val) < 0) ||
			(val < 0) || (val > MAX_CURRENT))
				return -EINVAL;
		pr_info("THERMAL set running test val = %ld\n", val);
		huawei_charger_set_runningtest(di, val);
		pr_info("THERMAL set running test iin_rt = %d\n", di->sysfs_data.iin_rt);
		break;
	/* running test charging/discharge function */
	case CHARGE_SYSFS_IIN_RT_CURRENT:
		if ((kstrtol(buf, INPUT_NUMBER_BASE, &val) < 0) ||
			(val < 0) || (val > MAX_CURRENT))
				return -EINVAL;
		pr_info("THERMAL set rt test val = %ld\n", val);
		huawei_charger_set_rt_current(di, val);
		pr_info("THERMAL set rt test iin_rt = %d\n", di->sysfs_data.iin_rt_curr);
		break;
	/* charging/discharging function */
	case CHARGE_SYSFS_ENABLE_CHARGER:
		/* enable charger input must be 0 or 1 */
#ifndef CONFIG_HLTHERM_RUNTEST
		if ((kstrtol(buf, INPUT_NUMBER_BASE, &val) < 0) || (val < 0) || (val > 1))
			return -EINVAL;
		pr_info("ENABLE_CHARGER set enable charger val = %ld\n", val);
		if (power_supply_check_psy_available("battery", &di->batt_psy)) {
			power_supply_set_int_property_value_with_psy(di->batt_psy,
				POWER_SUPPLY_PROP_CHARGING_ENABLED, !!val);
			di->chrg_config = val;
			pr_info("ENABLE_CHARGER set chrg_config = %d\n", di->chrg_config);
		}
		break;
#endif
	case CHARGE_SYSFS_WATCHDOG_DISABLE:
		di->sysfs_data.wdt_disable = val;
		pr_info("RUNNINGTEST set wdt disable = %d\n",
			di->sysfs_data.wdt_disable);
		break;
	/* factory diag function */
	case CHARGE_SYSFS_FACTORY_DIAG_CHARGER:
		/* factory diag valid input is 0 or 1 */
		if ((kstrtol(buf, INPUT_NUMBER_BASE, &val) < 0) || (val < 0) || (val > 1))
			return -EINVAL;
#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
		if (power_glink_set_property_value(POWER_GLINK_PROP_ID_SET_CHARGE_ENABLE, (unsigned int *)&val, GLINK_DATA_ONE))
			return -EFAULT;
#else
		if (!power_supply_check_psy_available("battery", &di->batt_psy))
			return -EFAULT;
		power_supply_set_int_property_value_with_psy(di->batt_psy,
			POWER_SUPPLY_PROP_CHARGING_ENABLED, !!val);
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */
		di->factory_diag = val;
		pr_info("ENABLE_CHARGER set factory_diag = %d\n", di->factory_diag);
		break;
	case CHARGE_SYSFS_UPDATE_VOLT_NOW:
		/* update volt now valid input is 1 */
		if ((kstrtol(buf, INPUT_NUMBER_BASE, &val) < 0) || (val != 1))
			return -EINVAL;
		break;
	case CHARGE_SYSFS_HIZ:
		/* hiz valid input is 0 or 1 */
		if ((kstrtol(buf, INPUT_NUMBER_BASE, &val) < 0) || (val < 0) || (val > 1))
			return -EINVAL;
		pr_info("ENABLE_CHARGER set hz mode val = %ld\n", val);
		huawei_charger_set_hz_mode(di, val);
		break;
	case CHARGE_SYSFS_ADAPTOR_VOLTAGE:
		if ((kstrtol(buf, INPUT_NUMBER_BASE, &val) < 0) ||
			(val < 0) || (val > INPUT_NUMBER_BASE))
				return -EINVAL;
		if (val == ADAPTER_5V)
			charge_set_reset_adapter_source(RESET_ADAPTER_DIRECT_MODE, TRUE);
		else
			charge_set_reset_adapter_source(RESET_ADAPTER_DIRECT_MODE, FALSE);
		huawei_charger_set_adaptor_change(di, charge_get_reset_adapter_source());
		break;

	default:
		pr_err("(%s)NODE ERR!!HAVE NO THIS NODE:%d\n", __func__, info->name);
		break;
	}
	return count;
}

static void charge_sysfs_create_group(struct device *dev)
{
	charge_sysfs_init_attrs();
	power_sysfs_create_link_group("hw_power", "charger", "charge_data",
		dev, &charge_sysfs_attr_group);
}

static void charge_sysfs_remove_group(struct device *dev)
{
	power_sysfs_remove_link_group("hw_power", "charger", "charge_data",
		dev, &charge_sysfs_attr_group);
}

static int charger_plug_notifier_call(struct notifier_block *nb, unsigned long event, void *data)
{
	if (huawei_handle_charger_event(event))
		return NOTIFY_BAD;
	pr_info("charger plug in or out\n");
	return NOTIFY_OK;
}

static bool hw_charger_init_psy(struct charge_device_info *di)
{
	di->bk_batt_psy = power_supply_get_by_name("bk_battery");
	if (!di->bk_batt_psy) {
		pr_err("%s: bk_batt_psy is null\n", __func__);
		return false;
	}
	di->pc_port_psy = power_supply_get_by_name("pc_port");
	if (!di->pc_port_psy) {
		pr_err("%s: pc_port_psy is null\n", __func__);
		return false;
	}

	di->batt_psy = power_supply_get_by_name("battery");
	if (!di->batt_psy) {
		pr_err("%s: batt_psy is null\n", __func__);
		return false;
	}

	di->chg_psy = power_supply_get_by_name("huawei_charge");
	if (!di->chg_psy) {
		pr_err("%s: chg_psy is null\n", __func__);
		return false;
	}

	di->bms_psy = power_supply_get_by_name("bms");
	if (!di->bms_psy) {
		pr_err("%s: bms_psy is null\n", __func__);
		return false;
	}
	return true;
}

static void dcp_set_iin_thermal_limit(unsigned int value)
{
	(void)huawei_charger_set_iin_thermal_limit(value);
}

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
static int dcp_set_enable_charger(unsigned int val)
{
	u32 id = POWER_GLINK_PROP_ID_SET_CHARGE_ENABLE;
	int usb_in = 0;
	int wls_in = 0;

	if (!g_di)
		return -EINVAL;

	if (power_glink_set_property_value(id, &val, GLINK_DATA_ONE))
		return -EFAULT;

	(void)power_supply_get_int_property_value("usb",
		POWER_SUPPLY_PROP_ONLINE, &usb_in);
	(void)power_supply_get_int_property_value("Wireless",
		POWER_SUPPLY_PROP_ONLINE, &wls_in);

	pr_info("%s usb_in=%d, wls_in=%d\n", __func__, usb_in, wls_in);
	if (usb_in || wls_in) {
		if (val)
			power_event_bnc_notify(POWER_BNT_CHARGING,
				POWER_NE_CHARGING_START, NULL);
		else
			power_event_bnc_notify(POWER_BNT_CHARGING,
				POWER_NE_CHARGING_SUSPEND, NULL);
	} else {
		power_event_bnc_notify(POWER_BNT_CHARGING,
			POWER_NE_CHARGING_STOP, NULL);
	}

	g_di->chrg_config = val;
	pr_info("ENABLE_CHARGER set %d\n", g_di->chrg_config);
	return 0;
}
#else
static int dcp_set_enable_charger(unsigned int val)
{
	if (!g_di)
		return -EINVAL;

	if (power_supply_check_psy_available("battery", &g_di->batt_psy)) {
		power_supply_set_int_property_value_with_psy(g_di->batt_psy,
			POWER_SUPPLY_PROP_CHARGING_ENABLED, !!val);
		g_di->chrg_config = val;
		pr_info("ENABLE_CHARGER set %d\n", g_di->chrg_config);
	}

	return 0;
}
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */

#ifndef CONFIG_HLTHERM_RUNTEST
static int dcp_set_iin_limit_array(unsigned int idx, unsigned int val)
{
	struct charge_device_info *di = g_charger_device_para;

	if (!di) {
		pr_err("di is null\n");
		return -EINVAL;
	}

	/*
	 * set default iin when value is 0 or 1
	 * set 100mA for iin when value is between 2 and 100
	 */
	if ((val == 0) || (val == 1))
		di->sysfs_data.iin_thl_array[idx] = DEFAULT_IIN_THL;
	else if ((val > 1) && (val <= MIN_CURRENT))
		di->sysfs_data.iin_thl_array[idx] = MIN_CURRENT;
	else
		di->sysfs_data.iin_thl_array[idx] = val;

	pr_info("thermal send input current = %d, type: %u\n",
		di->sysfs_data.iin_thl_array[idx], idx);
	if (idx != get_iin_thermal_charge_type())
		return 0;

	di->sysfs_data.iin_thl = di->sysfs_data.iin_thl_array[idx];
	dcp_set_iin_thermal_limit(di->sysfs_data.iin_thl);
	pr_info("thermal set input current = %d\n", di->sysfs_data.iin_thl);
	return 0;
}
#else
static int dcp_set_iin_limit_array(unsigned int idx, unsigned int val)
{
	return 0;
}
#endif /* CONFIG_HLTHERM_RUNTEST */

static int dcp_set_iin_thermal_all(unsigned int value)
{
	int i;

	for (i = IIN_THERMAL_CHARGE_TYPE_BEGIN; i < IIN_THERMAL_CHARGE_TYPE_END; i++) {
		if (dcp_set_iin_limit_array(i, value))
			return -EINVAL;
	}
	return 0;
}

static int dcp_set_iin_thermal(unsigned int index, unsigned int value)
{
	if (index >= IIN_THERMAL_CHARGE_TYPE_END) {
		pr_err("error index: %u, out of boundary\n", index);
		return -EINVAL;
	}
	return dcp_set_iin_limit_array(index, value);
}

static int set_adapter_voltage(int val)
{
	struct charge_device_info *di = g_charger_device_para;

	if (!di || (val < 0)) {
		pr_err("di is null or val is invalid\n");
		return -EINVAL;
	}

	if (val == ADAPTER_5V)
		charge_set_reset_adapter_source(RESET_ADAPTER_DIRECT_MODE, TRUE);
	else
		charge_set_reset_adapter_source(RESET_ADAPTER_DIRECT_MODE, FALSE);

	huawei_charger_set_adaptor_change(di, charge_get_reset_adapter_source());
	return 0;
}

static int get_adapter_voltage(int *val)
{
	if (!val)
		return -EINVAL;

	if (charge_get_reset_adapter_source())
		*val = ADAPTER_5V;
	else
		*val = ADAPTER_9V;
	return 0;
}

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
static int dcp_set_vterm_dec(unsigned int val)
{
	return power_glink_set_property_value(POWER_GLINK_PROP_ID_SET_FV_FOR_BASP, &val, GLINK_DATA_ONE);
}
#else
static int dcp_set_vterm_dec(unsigned int val)
{
	struct charge_device_info *di = g_charger_device_para;
	int vdec = (int)val;
	int vol_max = 0;

	if (!di)
		return -EINVAL;

	if (power_supply_get_int_property_value_with_psy(di->batt_psy,
		POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN, &vol_max)) {
		pr_err("%s get voltage max design failed\n", __func__);
		return -ENODEV;
	}

	vdec *= POWER_UV_PER_MV;
	if (vdec >= vol_max)
		return -EINVAL;

	return power_supply_set_int_property_value_with_psy(di->chg_psy,
		POWER_SUPPLY_PROP_VOLTAGE_BASP, vol_max - vdec);
}
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */

static int dcp_set_ichg_limit(unsigned int val)
{
	struct charge_device_info *di = g_charger_device_para;

	if (!di)
		return -EINVAL;

	/* mA convert to uA */
	return power_supply_set_int_property_value_with_psy(di->chg_psy,
		POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX, (int)(val * POWER_UA_PER_MA));
}

static struct power_if_ops sdp_if_ops = {
	.type_name = "sdp",
};

static struct power_if_ops dcp_if_ops = {
	.set_enable_charger = dcp_set_enable_charger,
	.set_iin_thermal = dcp_set_iin_thermal,
	.set_iin_thermal_all = dcp_set_iin_thermal_all,
	.set_adap_volt = set_adapter_voltage,
	.get_adap_volt = get_adapter_voltage,
	.set_vterm_dec = dcp_set_vterm_dec,
	.set_ichg_limit = dcp_set_ichg_limit,
	.type_name = "dcp",
};

static struct power_if_ops fcp_if_ops = {
	.type_name = "hvc",
};

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
static void huawei_charger_set_entry_time(unsigned int time, void *dev_data)
{
	u32 id = POWER_GLINK_PROP_ID_SET_SHIP_MODE_TIMING;
	u32 value = time;

	if (!dev_data)
		return;

	(void)power_glink_set_property_value(id, &value, GLINK_DATA_ONE);
	pr_err("set_entry_time: value=%u\n", value);
}

static void huawei_charger_set_work_mode(unsigned int mode, void *dev_data)
{
	u32 id = POWER_GLINK_PROP_ID_SET_SHIP_MODE;
	u32 value;

	if (!dev_data)
		return;

	if (mode == SHIP_MODE_IN_SHIP)
		value = 0;
	else
		return;

	(void)power_glink_set_property_value(id, &value, GLINK_DATA_ONE);
	pr_err("set_work_mode: value=%u\n", value);
}

static struct ship_mode_ops huawei_charger_ship_mode_ops = {
	.ops_name = "huawei_charger",
	.set_entry_time = huawei_charger_set_entry_time,
	.set_work_mode = huawei_charger_set_work_mode,
};
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */

static int charge_probe(struct platform_device *pdev)
{
	int ret;
	struct charge_device_info *di = NULL;
	int usb_pre;

	if (!pdev)
		return -EINVAL;

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	ret = of_property_read_u32(pdev->dev.of_node, "huawei,fcp-test-delay", &di->fcp_test_delay);
	if (ret) {
		pr_info("There is no fcp test delay setting, use default time: 1s\n");
		di->fcp_test_delay = DEFAULT_FCP_TEST_DELAY;
	}

	INIT_DELAYED_WORK(&di->smb_charger_work, smb_charger_work);
	di->dev = &(pdev->dev);
	dev_set_drvdata(&(pdev->dev), di);

	/* defaultly charging is enabled */
	di->sysfs_data.iin_thl = DEFAULT_IIN_THL;
	di->sysfs_data.iin_thl_array[IIN_THERMAL_WCURRENT_5V] = DEFAULT_IIN_THL;
	di->sysfs_data.iin_thl_array[IIN_THERMAL_WCURRENT_9V] = DEFAULT_IIN_THL;
	di->sysfs_data.iin_thl_array[IIN_THERMAL_WLCURRENT_5V] = DEFAULT_IIN_THL;
	di->sysfs_data.iin_thl_array[IIN_THERMAL_WLCURRENT_9V] = DEFAULT_IIN_THL;
	di->sysfs_data.iin_rt = 1;
	di->sysfs_data.iin_rt_curr = DEFAULT_IIN_CURRENT;
	di->sysfs_data.hiz_mode = 0;
	di->chrg_config = 1;
	di->factory_diag = 1;
	charge_set_reset_adapter_source(RESET_ADAPTER_DIRECT_MODE, FALSE);
	g_di = di;
	di->nb.notifier_call = charger_plug_notifier_call;
	ret = power_event_bnc_register(POWER_BNT_THIRDPLAT_CHARGING, &di->nb);
	if (ret < 0)
		pr_err("usb charger register notify failed\n");
	charge_sysfs_create_group(di->dev);
	charge_event_poll_register(charge_dev);
	g_charger_device_para = di;

	if (power_supply_check_psy_available("usb", &di->usb_psy)) {
		usb_pre = power_supply_return_int_property_value_with_psy(
			di->usb_psy, POWER_SUPPLY_PROP_ONLINE);
		if (usb_pre) {
			ret = charger_plug_notifier_call(&di->nb, 0, NULL);
			if (ret != NOTIFY_OK)
				pr_err("charger plug notifier call failed\n");
		}
	}
	if (!hw_charger_init_psy(di))
		pr_err("psy init failed\n");
	power_if_ops_register(&dcp_if_ops);
	power_if_ops_register(&sdp_if_ops);
	power_if_ops_register(&fcp_if_ops);

	charger_loginfo_register(di);

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
	huawei_charger_ship_mode_ops.dev_data = di;
	ship_mode_ops_register(&huawei_charger_ship_mode_ops);
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */

	pr_info("huawei charger probe ok!\n");
	return 0;
}

static void charge_event_poll_unregister(struct device *dev)
{
	if (!dev) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return;
	}

	sysfs_remove_file(&dev->kobj, &dev_attr_poll_charge_start_event.attr);
	g_sysfs_poll = NULL;
}

static int charge_remove(struct platform_device *pdev)
{
	struct charge_device_info *di = NULL;

	if (!pdev)
		return -EINVAL;

	di = dev_get_drvdata(&pdev->dev);
	if (!di)
		return -EINVAL;

	power_event_bnc_unregister(POWER_BNT_THIRDPLAT_CHARGING, &di->nb);
	cancel_delayed_work_sync(&di->smb_charger_work);
	charge_event_poll_unregister(charge_dev);
	charge_sysfs_remove_group(di->dev);
	kfree(di);
	di = NULL;
	g_di = NULL;
	g_charger_device_para = NULL;

	return 0;
}

static void charge_shutdown(struct platform_device  *pdev)
{
	return;
}

#ifdef CONFIG_PM
static int charge_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

static int charge_resume(struct platform_device *pdev)
{
	return 0;
}
#endif

static struct of_device_id charge_match_table[] = {
	{
		.compatible = "huawei,charger",
		.data = NULL,
	},
	{
	},
};

static struct platform_driver charge_driver = {
	.probe = charge_probe,
	.remove = charge_remove,
#ifdef CONFIG_PM
	.suspend = charge_suspend,
	.resume = charge_resume,
#endif
	.shutdown = charge_shutdown,
	.driver = {
		.name = "huawei,charger",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(charge_match_table),
	},
};

static int __init charge_init(void)
{
	return platform_driver_register(&charge_driver);
}

static void __exit charge_exit(void)
{
	platform_driver_unregister(&charge_driver);
}

late_initcall(charge_init);
module_exit(charge_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("huawei charger module driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
