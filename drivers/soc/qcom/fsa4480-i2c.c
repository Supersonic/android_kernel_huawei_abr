// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/regmap.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/usb/typec.h>
#include <linux/usb/ucsi_glink.h>
#include <linux/soc/qcom/fsa4480-i2c.h>
#include <linux/iio/consumer.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/pm_wakeup.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/init.h>
#include <securec.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>
#include <huawei_platform/log/hw_log.h>
#include <huawei_platform/power/huawei_charger.h>
#include <huawei_platform/usb/hw_pd_dev.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
#include <huawei_platform/hwpower/common_module/power_glink.h>
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */

#ifdef CONFIG_HUAWEI_DSM
#include <dsm/dsm_pub.h>
#endif

#define FSA4480_I2C_NAME	"fsa4480-driver"

#define FSA4480_SWITCH_SETTINGS 0x04
#define FSA4480_SWITCH_CONTROL  0x05
#define FSA4480_SWITCH_STATUS1  0x07
#define FSA4480_SLOW_L          0x08
#define FSA4480_SLOW_R          0x09
#define FSA4480_SLOW_MIC        0x0A
#define FSA4480_SLOW_SENSE      0x0B
#define FSA4480_SLOW_GND        0x0C
#define FSA4480_DELAY_L_R       0x0D
#define FSA4480_DELAY_L_MIC     0x0E
#define FSA4480_DELAY_L_SENSE   0x0F
#define FSA4480_DELAY_L_AGND    0x10
#define FSA4480_RESET           0x1E

#define FSA4480_REG_OVP_INT_MASK                                 0x01
#define FSA4480_REG_OVP_INT                                      0x02
#define FSA4480_REG_OVP_STATUS                                   0x03
#define FSA4480_REG_ENABLE                                       0x12
#define FSA4480_REG_PIN_SET                                      0x13
#define FSA4480_REG_RES_VAL                                      0x14
#define FSA4480_REG_RES_DET_THRD                                 0x15
#define FSA4480_REG_DET_INTERVAL                                 0x16
#define FSA4480_REG_RES_INT                                      0x18
#define FSA4480_REG_RES_INT_MASK                                 0x19

#define RES_ENABLE                                               0x02
#define RES_DISABLE                                              0x7D
#define RES_PREC_10K                                             0x20
#define RES_PREC_1K                                              0x5F
#define RES_INT_MASK_OPEN                                        0x07
#define RES_INT_MASK_CLEAR                                       0x05
#define RES_INT_THRESHOLD                                        0x19
#define SIGNAL_DETECT                                            0x00
#define LOOP_1S_DETECT                                           0x02
#define OVP_INT_MASK_OPEN                                        0x7F
#define OVP_INT_MASK_CLEAR                                       0x03
#define OVP_STATUS_VERIFY                                        0x3C
#define CHECK_TYPEC_DELAY                                        5000
#define DETECT_DELAY_DRY                                         30000
#define DETECT_DELAY_WATER                                       1000
#define DETECT_ONBOOT_DELAY                                      10000
#define WAKE_LOCK_TIMEOUT                                        200
#define WAIT_REG_TIMEOUT                                         100
#define RES_DETECT_DELAY                                         5
#define PIN_NUM                                                  4
#define DP                                                       0
#define DN                                                       1
#define SBU1                                                     2
#define SBU2                                                     3
#define STOP_OVP_DELAY                                           2000
#define NOTIFY_WATER_DELAY                                       3000
#define WATER_PIN_NUM                                            1

/* res detect */
#define ANA_FSA4480_AUDIO_JACK_DETECT_AND_CONFIG_IRQ_BIT         0x04
#define ANA_FSA4480_RES_BELOW_THRESHOLD_IRQ_BIT                  0x02
#define ANA_FSA4480_RES_DETECT_ACTION_IRQ_BIT                    0x01
#define ANA_FSA4480_RES_DETECT_ACTION_IRQ_MASK_BIT               0x01
#define ANA_FSA4480_RES_BELOW_THRESHOLD_IRQ_MASK_BIT             0x02
#define ANA_FSA4480_AUDIO_JACK_DETECT_AND_CONFIG_IRQ_MASK_BIT    0x04

/* ovp detec */
#define OVP_IRQ_MASK_BIT                                         0x40
#define FSA4480_DSM_BUF_SIZE_256                                 256
#ifdef CONFIG_HUAWEI_DSM
#define ERROR_NO_TYPEC_4480_OVP                 ERROR_NO_TYPEC_CC_OVP
#endif

#define SET_ANA_FSA4480_SWITCH_CONTROL_REG_VALUE    0x00
#define SET_ANA_FSA4480_SWITCH_ENABLE_REG_VALUE     0x98
#define CHANGE_ANA_FSA4480_MODE                     3
#define CHANGE_ANA_FSA4480_MODE_OFFSET              3

enum ana_fsa4480_res_irq_type {
	ANA_FSA4480_RES_DETECT_ACTION             = 0,
	ANA_FSA4480_RES_BELOW_THRESHOLD           = 1,
	ANA_FSA4480_AUDIO_JACK_DETECT_AND_CONFIG  = 2,
	ANA_FSA4480_IQR_MAX                       = 3,
};

enum ana_fsa4480_ovp_irq_type {
	ANA_FSA4480_OVP_IRQ_EXIST                 = 0,
	ANA_FSA4480_OVP_IRQ_MAX                   = 1,
};

enum ana_hs_typec_state {
	ANA_HS_TYPEC_DEVICE_ATTACHED              = 0,
	ANA_HS_TYPEC_DEVICE_DETACHED              = 1,
};

enum {
	ID_FSA4480_IRQ,
	ID_IRQ_END,
};

enum {
	ID_FSA4480_INIT,
	ID_INIT_END,
};

enum {
	ID_FSA4480_REMOVE,
	ID_REMOVE_END,
};

#define HWLOG_TAG fsa4480
HWLOG_REGIST();

#define IN_FUNCTION hwlog_info("%s comein\n", __func__)
#define OUT_FUNCTION hwlog_info("%s comeout\n", __func__)

static int g_dp_aux_state;

struct fsa4480_irq_handler {
	int gpio;
	int irq;
	int func_id;
	unsigned int irq_flag;
};

struct fsa4480_extern_ops_data {
	/* 4480 water detect data */
	struct wakeup_source *wake_lock;
	struct delayed_work notify_water_dw;
	struct delayed_work res_detect_dw;
	struct delayed_work stop_ovp_dw;
	struct delayed_work get_irq_type_dw;
	struct delayed_work detect_onboot_dw;
	int default_pin;
	bool water_intruded;
	bool typec_detach;
	bool report_when_detach;
	bool notified;
	unsigned int res_value_old[PIN_NUM];
};

struct fsa4480_priv {
	struct regmap *regmap;
	struct device *dev;
	struct power_supply *usb_psy;
	struct notifier_block nb;
	struct iio_channel *iio_ch;
	atomic_t usbc_mode;
	struct work_struct usbc_analog_work;
	struct blocking_notifier_head fsa4480_notifier;
	struct mutex notification_lock;
	u32 use_powersupply;
	int switch_control;
	int pogo_switch_control;
	int pogo_switch_settings;
	int pogo_state;
	struct fsa4480_irq_handler *handler;
	struct fsa4480_extern_ops_data *extern_pdata;
};

struct fsa4480_priv *g_fsa_data;

struct fsa4480_reg_val {
	u16 reg;
	u8 val;
};

static const struct regmap_config fsa4480_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = FSA4480_RESET,
};

static const struct fsa4480_reg_val fsa_reg_i2c_defaults[] = {
	{FSA4480_SLOW_L, 0x00},
	{FSA4480_SLOW_R, 0x00},
	{FSA4480_SLOW_MIC, 0x00},
	{FSA4480_SLOW_SENSE, 0x00},
	{FSA4480_SLOW_GND, 0x00},
	{FSA4480_DELAY_L_R, 0x00},
	{FSA4480_DELAY_L_MIC, 0x00},
	{FSA4480_DELAY_L_SENSE, 0x00},
	{FSA4480_DELAY_L_AGND, 0x09},
	{FSA4480_SWITCH_SETTINGS, 0x98},
};

static void fsa4480_usbc_update_settings(struct fsa4480_priv *fsa_priv,
		u32 switch_control, u32 switch_enable)
{
	u32 prev_control, prev_enable;

	if (!fsa_priv->regmap) {
		dev_err(fsa_priv->dev, "%s: regmap invalid\n", __func__);
		return;
	}

	regmap_read(fsa_priv->regmap, FSA4480_SWITCH_CONTROL, &prev_control);
	regmap_read(fsa_priv->regmap, FSA4480_SWITCH_SETTINGS, &prev_enable);

	if (prev_control == switch_control && prev_enable == switch_enable) {
		dev_dbg(fsa_priv->dev, "%s: settings unchanged\n", __func__);
		return;
	}

	regmap_write(fsa_priv->regmap, FSA4480_SWITCH_SETTINGS, 0x80);
	regmap_write(fsa_priv->regmap, FSA4480_SWITCH_CONTROL, switch_control);
	/* FSA4480 chip hardware requirement */
	usleep_range(50, 55);
	regmap_write(fsa_priv->regmap, FSA4480_SWITCH_SETTINGS, switch_enable);
}

static int fsa4480_usbc_event_changed_psupply(struct fsa4480_priv *fsa_priv,
				      unsigned long evt, void *ptr)
{
	struct device *dev = NULL;

	if (!fsa_priv)
		return -EINVAL;

	dev = fsa_priv->dev;
	if (!dev)
		return -EINVAL;
	dev_dbg(dev, "%s: queueing usbc_analog_work\n",
		__func__);
	pm_stay_awake(fsa_priv->dev);
	queue_work(system_freezable_wq, &fsa_priv->usbc_analog_work);

	return 0;
}

static int fsa4480_usbc_event_changed_ucsi(struct fsa4480_priv *fsa_priv,
				      unsigned long evt, void *ptr)
{
	struct device *dev;
	enum typec_accessory acc = ((struct ucsi_glink_constat_info *)ptr)->acc;

	if (!fsa_priv)
		return -EINVAL;

	dev = fsa_priv->dev;
	if (!dev)
		return -EINVAL;

	dev_dbg(dev, "%s: USB change event received, supply mode %d, usbc mode %ld, expected %d\n",
			__func__, acc, fsa_priv->usbc_mode.counter,
			TYPEC_ACCESSORY_AUDIO);

	switch (acc) {
	case TYPEC_ACCESSORY_AUDIO:
	case TYPEC_ACCESSORY_NONE:
		if (atomic_read(&(fsa_priv->usbc_mode)) == acc)
			break; /* filter notifications received before */
		atomic_set(&(fsa_priv->usbc_mode), acc);

		dev_dbg(dev, "%s: queueing usbc_analog_work\n",
			__func__);
		pm_stay_awake(fsa_priv->dev);
		queue_work(system_freezable_wq, &fsa_priv->usbc_analog_work);
		break;
	default:
		break;
	}

	return 0;
}

static int fsa4480_usbc_event_changed(struct notifier_block *nb_ptr,
				      unsigned long evt, void *ptr)
{
	struct fsa4480_priv *fsa_priv =
			container_of(nb_ptr, struct fsa4480_priv, nb);
	struct device *dev;

	if (!fsa_priv)
		return -EINVAL;

	dev = fsa_priv->dev;
	if (!dev)
		return -EINVAL;

	if (fsa_priv->use_powersupply)
		return fsa4480_usbc_event_changed_psupply(fsa_priv, evt, ptr);
	else
		return fsa4480_usbc_event_changed_ucsi(fsa_priv, evt, ptr);
}

static int fsa4480_usbc_analog_setup_switches_psupply(
						struct fsa4480_priv *fsa_priv)
{
	int rc = 0;
	union power_supply_propval mode;
	struct device *dev;

	if (!fsa_priv)
		return -EINVAL;
	dev = fsa_priv->dev;
	if (!dev)
		return -EINVAL;

	rc = iio_read_channel_processed(fsa_priv->iio_ch, &mode.intval);

	mutex_lock(&fsa_priv->notification_lock);
	/* get latest mode again within locked context */
	if (rc < 0) {
		dev_err(dev, "%s: Unable to read USB TYPEC_MODE: %d\n",
			__func__, rc);
		goto done;
	}

	dev_dbg(dev, "%s: setting GPIOs active = %d rcvd intval 0x%X\n",
		__func__, mode.intval != TYPEC_ACCESSORY_NONE, mode.intval);
	atomic_set(&(fsa_priv->usbc_mode), mode.intval);

	switch (mode.intval) {
	/* add all modes FSA should notify for in here */
	case TYPEC_ACCESSORY_AUDIO:
		/* activate switches */
		fsa4480_usbc_update_settings(fsa_priv, 0x00, 0x9F);

		/* notify call chain on event */
		blocking_notifier_call_chain(&fsa_priv->fsa4480_notifier,
		mode.intval, NULL);
		break;
	case TYPEC_ACCESSORY_NONE:
		/* notify call chain on event */
		blocking_notifier_call_chain(&fsa_priv->fsa4480_notifier,
				TYPEC_ACCESSORY_NONE, NULL);

		/* deactivate switches */
		if (!g_dp_aux_state) {
			if (fsa_priv->pogo_state) {
				regmap_read(fsa_priv->regmap, FSA4480_SWITCH_SETTINGS,
						&fsa_priv->pogo_switch_settings);

				dev_info(dev, "%s: deactivate switches reg: 0x%x value: 0x%x\n",
						__func__, FSA4480_SWITCH_SETTINGS,
						fsa_priv->pogo_switch_settings);

				fsa4480_usbc_update_settings(fsa_priv, 0x0,
						fsa_priv->pogo_switch_settings);
			} else {
				fsa4480_usbc_update_settings(fsa_priv, 0x18, 0x98);
			}
		}
		break;
	default:
		/* ignore other usb connection modes */
		break;
	}

done:
	mutex_unlock(&fsa_priv->notification_lock);
	return rc;
}

static int fsa4480_usbc_analog_setup_switches_ucsi(
						struct fsa4480_priv *fsa_priv)
{
	int rc = 0;
	int mode;
	struct device *dev;

	if (!fsa_priv)
		return -EINVAL;
	dev = fsa_priv->dev;
	if (!dev)
		return -EINVAL;

	mutex_lock(&fsa_priv->notification_lock);
	/* get latest mode again within locked context */
	mode = atomic_read(&(fsa_priv->usbc_mode));

	dev_dbg(dev, "%s: setting GPIOs active = %d\n",
		__func__, mode != TYPEC_ACCESSORY_NONE);

	switch (mode) {
	/* add all modes FSA should notify for in here */
	case TYPEC_ACCESSORY_AUDIO:
		/* activate switches */
		fsa4480_usbc_update_settings(fsa_priv, 0x00, 0x9F);

		/* notify call chain on event */
		blocking_notifier_call_chain(&fsa_priv->fsa4480_notifier,
					     mode, NULL);
		break;
	case TYPEC_ACCESSORY_NONE:
		/* notify call chain on event */
		blocking_notifier_call_chain(&fsa_priv->fsa4480_notifier,
				TYPEC_ACCESSORY_NONE, NULL);

		/* deactivate switches */
		if (!g_dp_aux_state) {
			if (fsa_priv->pogo_state) {
				regmap_read(fsa_priv->regmap, FSA4480_SWITCH_SETTINGS,
						&fsa_priv->pogo_switch_settings);

				dev_info(dev, "%s: deactivate switches reg: 0x%x value: 0x%x\n",
						__func__, FSA4480_SWITCH_SETTINGS,
						fsa_priv->pogo_switch_settings);

				fsa4480_usbc_update_settings(fsa_priv, 0x0,
						fsa_priv->pogo_switch_settings);
			} else {
				fsa4480_usbc_update_settings(fsa_priv, 0x18, 0x98);
			}
		}
		break;
	default:
		/* ignore other usb connection modes */
		break;
	}

	mutex_unlock(&fsa_priv->notification_lock);
	return rc;
}

static int fsa4480_usbc_analog_setup_switches(struct fsa4480_priv *fsa_priv)
{
	if (fsa_priv->use_powersupply)
		return fsa4480_usbc_analog_setup_switches_psupply(fsa_priv);
	else
		return fsa4480_usbc_analog_setup_switches_ucsi(fsa_priv);
}

/*
 * fsa4480_reg_notifier - register notifier block with fsa driver
 *
 * @nb - notifier block of fsa4480
 * @node - phandle node to fsa4480 device
 *
 * Returns 0 on success, or error code
 */
int fsa4480_reg_notifier(struct notifier_block *nb,
			 struct device_node *node)
{
	int rc = 0;
	struct i2c_client *client = of_find_i2c_device_by_node(node);
	struct fsa4480_priv *fsa_priv;

	if (!client)
		return -EINVAL;

	fsa_priv = (struct fsa4480_priv *)i2c_get_clientdata(client);
	if (!fsa_priv)
		return -EINVAL;

	rc = blocking_notifier_chain_register
				(&fsa_priv->fsa4480_notifier, nb);
	if (rc)
		return rc;

	/*
	 * as part of the init sequence check if there is a connected
	 * USB C analog adapter
	 */
	dev_dbg(fsa_priv->dev, "%s: verify if USB adapter is already inserted\n",
		__func__);
	rc = fsa4480_usbc_analog_setup_switches(fsa_priv);
	regmap_update_bits(fsa_priv->regmap, FSA4480_SWITCH_CONTROL, 0x07,
			   fsa_priv->switch_control);
	return rc;
}
EXPORT_SYMBOL(fsa4480_reg_notifier);

/*
 * fsa4480_unreg_notifier - unregister notifier block with fsa driver
 *
 * @nb - notifier block of fsa4480
 * @node - phandle node to fsa4480 device
 *
 * Returns 0 on pass, or error code
 */
int fsa4480_unreg_notifier(struct notifier_block *nb,
			     struct device_node *node)
{
	int rc = 0;
	struct i2c_client *client = of_find_i2c_device_by_node(node);
	struct fsa4480_priv *fsa_priv;

	if (!client)
		return -EINVAL;

	fsa_priv = (struct fsa4480_priv *)i2c_get_clientdata(client);
	if (!fsa_priv)
		return -EINVAL;

	mutex_lock(&fsa_priv->notification_lock);

	fsa4480_usbc_update_settings(fsa_priv, 0x18, 0x98);
	rc = blocking_notifier_chain_unregister
				(&fsa_priv->fsa4480_notifier, nb);
	mutex_unlock(&fsa_priv->notification_lock);

	return rc;
}
EXPORT_SYMBOL(fsa4480_unreg_notifier);

static int fsa4480_validate_display_port_settings(struct fsa4480_priv *fsa_priv)
{
	u32 switch_status = 0;

	regmap_read(fsa_priv->regmap, FSA4480_SWITCH_STATUS1, &switch_status);

	if ((switch_status != 0x23) && (switch_status != 0x1C)) {
		pr_err("AUX SBU1/2 switch status is invalid = %u\n",
				switch_status);
		return -EIO;
	}

	return 0;
}

static void change_fsa4480_status_pogo_plugin(struct fsa4480_priv *fsa_priv)
{
	if (!fsa_priv)
		return;

	fsa_priv->pogo_state = true;

	if (!(atomic_read(&(fsa_priv->usbc_mode)) == TYPEC_ACCESSORY_AUDIO)) {
		regmap_read(fsa_priv->regmap, FSA4480_SWITCH_CONTROL,
				&fsa_priv->pogo_switch_control);

		dev_info(fsa_priv->dev, "%s: switches old status reg: 0x%x value: 0x%x\n",
				__func__, FSA4480_SWITCH_CONTROL, fsa_priv->pogo_switch_control);

		regmap_read(fsa_priv->regmap, FSA4480_SWITCH_SETTINGS,
				&fsa_priv->pogo_switch_settings);
		fsa_priv->pogo_switch_control &=
					~(CHANGE_ANA_FSA4480_MODE_OFFSET << CHANGE_ANA_FSA4480_MODE);
		fsa4480_usbc_update_settings(fsa_priv, fsa_priv->pogo_switch_control,
				fsa_priv->pogo_switch_settings);
	}
}

static void change_fsa4480_status_pogo_plugout(struct fsa4480_priv *fsa_priv)
{
	if (!fsa_priv)
		return;

	fsa_priv->pogo_state = false;

	if (atomic_read(&(fsa_priv->usbc_mode)) == TYPEC_ACCESSORY_AUDIO) {
		fsa4480_usbc_update_settings(fsa_priv, SET_ANA_FSA4480_SWITCH_CONTROL_REG_VALUE,
				SET_ANA_FSA4480_SWITCH_ENABLE_REG_VALUE);
	} else {
		regmap_read(fsa_priv->regmap, FSA4480_SWITCH_CONTROL,
				&fsa_priv->pogo_switch_control);

		dev_info(fsa_priv->dev, "%s: switches old status reg: 0x%x value: 0x%x\n",
				__func__, FSA4480_SWITCH_CONTROL, fsa_priv->pogo_switch_control);

		regmap_read(fsa_priv->regmap, FSA4480_SWITCH_SETTINGS,
				&fsa_priv->pogo_switch_settings);
		fsa_priv->pogo_switch_control |=
				(CHANGE_ANA_FSA4480_MODE_OFFSET << CHANGE_ANA_FSA4480_MODE);
		fsa4480_usbc_update_settings(fsa_priv, fsa_priv->pogo_switch_control,
				fsa_priv->pogo_switch_settings);
	}
}

/*
 * fsa4480_switch_event - configure FSA switch position based on event
 *
 * @node - phandle node to fsa4480 device
 * @event - fsa_function enum
 *
 * Returns int on whether the switch happened or not
 */
int fsa4480_switch_event(struct device_node *node,
			 enum fsa_function event)
{
	struct i2c_client *client = of_find_i2c_device_by_node(node);
	struct fsa4480_priv *fsa_priv;
	int rc;

	if (!client)
		return -EINVAL;

	fsa_priv = (struct fsa4480_priv *)i2c_get_clientdata(client);
	if (!fsa_priv)
		return -EINVAL;
	if (!fsa_priv->regmap)
		return -EINVAL;

	switch (event) {
	case FSA_MIC_GND_SWAP:
		regmap_read(fsa_priv->regmap, FSA4480_SWITCH_CONTROL,
				&fsa_priv->switch_control);
		if ((fsa_priv->switch_control & 0x07) == 0x07)
			fsa_priv->switch_control = 0x0;
		else
			fsa_priv->switch_control = 0x7;
		fsa4480_usbc_update_settings(fsa_priv, fsa_priv->switch_control,
					     0x9F);
		break;
	case FSA_USBC_ORIENTATION_CC1:
		fsa4480_usbc_update_settings(fsa_priv, 0x18, 0xF8);
		rc = fsa4480_validate_display_port_settings(fsa_priv);
		g_dp_aux_state = ((rc == 0) ? 1 : 0);
		return rc;
	case FSA_USBC_ORIENTATION_CC2:
		fsa4480_usbc_update_settings(fsa_priv, 0x78, 0xF8);
		rc = fsa4480_validate_display_port_settings(fsa_priv);
		g_dp_aux_state = ((rc == 0) ? 1 : 0);
		return rc;
	case FSA_USBC_DISPLAYPORT_DISCONNECTED:
		fsa4480_usbc_update_settings(fsa_priv, 0x18, 0x98);
		g_dp_aux_state = 0;
		break;
	case FSA_POGO_IN:
		change_fsa4480_status_pogo_plugin(fsa_priv);
		break;
	case FSA_POGO_OUT:
		change_fsa4480_status_pogo_plugout(fsa_priv);
		break;
	default:
		break;
	}

	return 0;
}
EXPORT_SYMBOL(fsa4480_switch_event);

static void fsa4480_usbc_analog_work_fn(struct work_struct *work)
{
	struct fsa4480_priv *fsa_priv =
		container_of(work, struct fsa4480_priv, usbc_analog_work);

	if (!fsa_priv) {
		pr_err("%s: fsa container invalid\n", __func__);
		return;
	}
	fsa4480_usbc_analog_setup_switches(fsa_priv);
	pm_relax(fsa_priv->dev);
}

static void fsa4480_update_reg_defaults(struct regmap *regmap)
{
	u8 i;

	for (i = 0; i < ARRAY_SIZE(fsa_reg_i2c_defaults); i++)
		regmap_write(regmap, fsa_reg_i2c_defaults[i].reg,
				   fsa_reg_i2c_defaults[i].val);
}

static int fsa4480_is_water_intruded(void *dev_data)
{
	if (!g_fsa_data || !g_fsa_data->extern_pdata) {
		hwlog_err("%s extern_pdata is null\n", __func__);
		return false;
	}

	return g_fsa_data->extern_pdata->water_intruded;
}

static struct water_detect_ops fsa4480_extern_water_detect_ops = {
	.type_name = "audio_dp_dn",
	.is_water_intruded = fsa4480_is_water_intruded,
};

static void fsa4480_clear_res_detect(void)
{
	unsigned int reg_value = 0x00;

	regmap_read(g_fsa_data->regmap, FSA4480_REG_ENABLE, &reg_value);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_ENABLE, reg_value & RES_DISABLE);
}

static void fsa4480_clear_res_interrupt(void)
{
	unsigned int reg_value = 0x00;

	regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_INT, &reg_value);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_INT_MASK, &reg_value);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_RES_INT_MASK,
		reg_value | RES_INT_MASK_OPEN);
}

static void fsa4480_cancel_res_detect_work(void)
{
	if (!g_fsa_data || !g_fsa_data->extern_pdata)
		return;

	cancel_delayed_work_sync(&g_fsa_data->extern_pdata->res_detect_dw);
	cancel_delayed_work_sync(&g_fsa_data->extern_pdata->get_irq_type_dw);
	dev_info(g_fsa_data->dev, "%s: res detect work canceled\n", __func__);
}

static void fsa4480_cancel_notify_work(void)
{
	g_fsa_data->extern_pdata->water_intruded = false;
	cancel_delayed_work_sync(&g_fsa_data->extern_pdata->notify_water_dw);
	dev_info(g_fsa_data->dev, "%s water notify work canceled\n", __func__);
}

static void fsa4480_set_interrupt_pin(void)
{
	unsigned int reg_value = 0x00;

	IN_FUNCTION;

	regmap_write(g_fsa_data->regmap, FSA4480_REG_PIN_SET, g_fsa_data->extern_pdata->default_pin);
	hwlog_info("%s: default_pin is %d", __func__, g_fsa_data->extern_pdata->default_pin);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_DET_INTERVAL, LOOP_1S_DETECT);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_INT, &reg_value);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_INT_MASK, &reg_value);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_RES_INT_MASK, reg_value & RES_INT_MASK_CLEAR);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_ENABLE, &reg_value);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_ENABLE, reg_value | RES_ENABLE | RES_PREC_10K);

	OUT_FUNCTION;
}

void fsa4480_start_res_detect(bool new_state)
{
	if (!g_fsa_data || !g_fsa_data->extern_pdata)
		return;

	IN_FUNCTION;

	g_fsa_data->extern_pdata->typec_detach = new_state;
	g_fsa_data->extern_pdata->report_when_detach = true;

	if (g_fsa_data->extern_pdata->water_intruded) {
		hwlog_info("%s water_intruded is %d\n", __func__, g_fsa_data->extern_pdata->water_intruded);
		schedule_delayed_work(&g_fsa_data->extern_pdata->res_detect_dw,
			msecs_to_jiffies(DETECT_DELAY_WATER));
	} else {
		hwlog_info("%s water_intruded is %d\n", __func__, g_fsa_data->extern_pdata->water_intruded);
		schedule_delayed_work(&g_fsa_data->extern_pdata->res_detect_dw,
			msecs_to_jiffies(DETECT_DELAY_DRY));
	}

	OUT_FUNCTION;
}

void fsa4480_stop_res_detect(bool new_state)
{
	if (!g_fsa_data || !g_fsa_data->extern_pdata)
		return;

	IN_FUNCTION;

	g_fsa_data->extern_pdata->typec_detach = new_state;
	fsa4480_clear_res_detect();
	fsa4480_clear_res_interrupt();
	fsa4480_cancel_res_detect_work();
	if (g_fsa_data->extern_pdata->water_intruded && !g_fsa_data->extern_pdata->notified)
		fsa4480_cancel_notify_work();
	else
		g_fsa_data->extern_pdata->notified = false;

	OUT_FUNCTION;
}

#ifdef CONFIG_HUAWEI_DSM
static void ovp_dmd_report(void)
{
	int ret;
	char msg_buf[FSA4480_DSM_BUF_SIZE_256] = { 0 };

	ret = snprintf_s(msg_buf, FSA4480_DSM_BUF_SIZE_256,
		FSA4480_DSM_BUF_SIZE_256 - 1,
		"%s\n",
		"D+/D-/SBU ovp happened");
	if (ret < 0)
		dev_err(g_fsa_data->dev, "%s Fill fsa4480 ovp dmd msg fail\n", __func__);

	power_dsm_report_dmd(POWER_DSM_BATTERY, ERROR_NO_TYPEC_4480_OVP,
		(void *)msg_buf);
}
#endif

void fsa4480_start_ovp_detect(bool new_state)
{
	unsigned int opv_status = 0;
	unsigned int reg_value;

	if (!g_fsa_data || !g_fsa_data->extern_pdata)
		return;

	IN_FUNCTION;
	g_fsa_data->extern_pdata->typec_detach = new_state;
	regmap_read(g_fsa_data->regmap, FSA4480_REG_OVP_STATUS, &opv_status);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_OVP_INT, &reg_value);
	if (opv_status & OVP_STATUS_VERIFY) {
#ifdef CONFIG_HUAWEI_DSM
		hwlog_info("ovp happened, report ovp dmd\n");
		ovp_dmd_report();
#endif
	} else {
		hwlog_info("no ovp, enable ovp int\n");
		regmap_write(g_fsa_data->regmap, FSA4480_REG_OVP_INT_MASK, OVP_INT_MASK_CLEAR);
		regmap_read(g_fsa_data->regmap, FSA4480_REG_OVP_INT, &reg_value);
	}
	OUT_FUNCTION;
}

void fsa4480_stop_ovp_detect(bool new_state)
{
	if (!g_fsa_data || !g_fsa_data->extern_pdata)
		return;

	hwlog_info("%s wait %d ms to stop it", __func__, STOP_OVP_DELAY);
	g_fsa_data->extern_pdata->typec_detach = new_state;
	schedule_delayed_work(&g_fsa_data->extern_pdata->stop_ovp_dw,
		msecs_to_jiffies(STOP_OVP_DELAY));
}

static void fsa4480_stop_ovp_detect_work(struct work_struct *work)
{
	unsigned int reg_value;

	hwlog_info("%s stopped\n", __func__);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_OVP_INT, &reg_value);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_OVP_INT_MASK, OVP_INT_MASK_OPEN);
}


static void fsa4480_detect_onboot_work(struct work_struct *work)
{
	int event = PD_DPM_USB_TYPEC_NONE;

	IN_FUNCTION;

	pd_dpm_get_typec_state(&event);
	if (event == PD_DPM_USB_TYPEC_NONE ||
		event == PD_DPM_USB_TYPEC_DETACHED ||
		event == PD_DPM_USB_TYPEC_AUDIO_DETACHED) {
		hwlog_info("typec device detach, start res detect onboot\n");
		g_fsa_data->extern_pdata->typec_detach = ANA_HS_TYPEC_DEVICE_DETACHED;
		schedule_delayed_work(&g_fsa_data->extern_pdata->res_detect_dw,
			msecs_to_jiffies(0));
	} else {
		hwlog_info("typec device attach, start ovp detect onboot\n");
		g_fsa_data->extern_pdata->typec_detach = ANA_HS_TYPEC_DEVICE_ATTACHED;
		fsa4480_start_ovp_detect(g_fsa_data->extern_pdata->typec_detach);
	}

	OUT_FUNCTION;
}


static void fsa4480_update_pin_state(int i,
	unsigned int res_value, bool *pins_intruded_state)
{
	const static char *detected_pin[PIN_NUM] = {
		"DP", "DN", "SBU1", "SBU2" };

	if (res_value <= RES_INT_THRESHOLD) {
		pins_intruded_state[i] = true;
		hwlog_info("%s %s intruded flag is :%x\n", __func__,
			detected_pin[i], pins_intruded_state[i]);
	} else {
		pins_intruded_state[i] = false; /* pin is not watered */
	}
}

static void notify_water_work(struct work_struct *work)
{
	int event = WD_STBY_MOIST; /* watered */
	(void)work;

	hwlog_info("%s watered, report event\n", __func__);
	/* notify watered event */
	g_fsa_data->extern_pdata->notified = true;
	power_event_bnc_notify(POWER_BNT_WD, POWER_NE_WD_REPORT_UEVENT, &event);
	power_event_bnc_notify(POWER_BNT_WD, POWER_NE_WD_REPORT_DMD, "fsa4480");
}

static void fsa4480_update_watered_state(const bool pins_intruded_state[])
{
	int j;
	int event;
	int pin_index = 0;
	PM_TYPEC_PORT_ROLE_TYPE type;
	/* res_detect_pins is D+ D- SBU1 SBU2 */
	const static unsigned int res_detect_pins[PIN_NUM] = {
		0x01, 0x02, 0x03, 0x04 };

	if (!g_fsa_data->extern_pdata->water_intruded) {
		if (pins_intruded_state[DP] &&
			pins_intruded_state[DN] &&
			(pins_intruded_state[SBU1] ||
			pins_intruded_state[SBU2])) {
			g_fsa_data->extern_pdata->water_intruded = true;
			schedule_delayed_work(&g_fsa_data->extern_pdata->notify_water_dw,
				msecs_to_jiffies(NOTIFY_WATER_DELAY));
			type = TYPEC_PORT_ROLE_SNK;
			power_glink_set_property_value(POWER_GLINK_PROP_ID_SET_PORT_ROLE, &type, 1);
		} else {
			for (j = 0; j < PIN_NUM; j++) {
				if (!pins_intruded_state[j]) {
					g_fsa_data->extern_pdata->default_pin =
						res_detect_pins[j];
					hwlog_info("%s no watered, next detect 0x%x\n",
						__func__,
						g_fsa_data->extern_pdata->default_pin);
					break;
				}
			}
		}
	} else {
		if ((pins_intruded_state[DP] + pins_intruded_state[DN] +
			pins_intruded_state[SBU1] + pins_intruded_state[SBU2]) <= WATER_PIN_NUM) {
			while (pins_intruded_state[pin_index])
				pin_index++;
			g_fsa_data->extern_pdata->default_pin = res_detect_pins[pin_index];
			g_fsa_data->extern_pdata->water_intruded = false;
			hwlog_info("%s water dry, next detect 0x%x\n",
				__func__, g_fsa_data->extern_pdata->default_pin);
			/* notify dry event */
			event = WD_STBY_DRY; /* water dry */
			power_event_bnc_notify(POWER_BNT_WD, POWER_NE_WD_REPORT_UEVENT,
				&event);
			type = TYPEC_PORT_ROLE_DRP;
			power_glink_set_property_value(POWER_GLINK_PROP_ID_SET_PORT_ROLE, &type, 1);
		} else {
			hwlog_info("%s still watered\n", __func__);
			if (g_fsa_data->extern_pdata->report_when_detach) {
				hwlog_info("report when detach under watered\n");
				event = WD_STBY_MOIST; /* watered */
				power_event_bnc_notify(POWER_BNT_WD,
					POWER_NE_WD_REPORT_UEVENT, &event);
			}
		}
	}
}

static void fsa4480_select_detect_mode(void)
{
	if (g_fsa_data->extern_pdata->water_intruded) {
		hwlog_info("%s set %dms to recall\n", __func__,
			CHECK_TYPEC_DELAY);
		schedule_delayed_work(
			&g_fsa_data->extern_pdata->res_detect_dw,
			msecs_to_jiffies(CHECK_TYPEC_DELAY));
	} else {
		hwlog_info("%s restart res detect\n", __func__);
		fsa4480_set_interrupt_pin();
	}
}

static void fsa4480_res_detect_work(struct work_struct *work)
{
	int i;
	int ret;
	unsigned int res_value;
	unsigned int reg_value = 0x00;

	const static unsigned int res_detect_pins[PIN_NUM] = {
		0x01, 0x02, 0x03, 0x04 };
	const static char *detected_pin[PIN_NUM] = {
		"DP", "DN", "SBU1", "SBU2" };
	static bool pins_intruded_state[PIN_NUM] = {0};

	IN_FUNCTION;

	__pm_wakeup_event(g_fsa_data->extern_pdata->wake_lock, WAKE_LOCK_TIMEOUT);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_DET_INTERVAL, SIGNAL_DETECT);

	for (i = 0; i < PIN_NUM; i++) {
		if (!g_fsa_data->extern_pdata->typec_detach) {
			dev_info(g_fsa_data->dev, "%s typec attach, exit detect\n", __func__);
			__pm_relax(g_fsa_data->extern_pdata->wake_lock);
			return;
		} else {
			res_value = g_fsa_data->extern_pdata->res_value_old[i];
			regmap_write(g_fsa_data->regmap, FSA4480_REG_PIN_SET,
				res_detect_pins[i]);
			regmap_read(g_fsa_data->regmap, FSA4480_REG_ENABLE, &reg_value);
			regmap_write(g_fsa_data->regmap, FSA4480_REG_ENABLE,
				reg_value | RES_ENABLE | RES_PREC_10K);
			mdelay(RES_DETECT_DELAY);
			ret = regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_VAL, &res_value);
			if (ret == 0)
				g_fsa_data->extern_pdata->res_value_old[i] = res_value;
			regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_INT, &reg_value);
			dev_info(g_fsa_data->dev, "%s %s res value %d * 10K\n",
				__func__, detected_pin[i], res_value);
			fsa4480_update_pin_state(i,
				res_value, pins_intruded_state);
		}
	}
	fsa4480_update_watered_state(pins_intruded_state);
	g_fsa_data->extern_pdata->report_when_detach = false;
	fsa4480_select_detect_mode();
	__pm_relax(g_fsa_data->extern_pdata->wake_lock);
	OUT_FUNCTION;
}

static void get_ovp_irq_type(unsigned int value)
{
	int irq_type = ANA_FSA4480_OVP_IRQ_MAX;
	unsigned int mask = 0x00;

	__pm_wakeup_event(g_fsa_data->extern_pdata->wake_lock, WAKE_LOCK_TIMEOUT);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_OVP_INT_MASK, &mask);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_OVP_INT_MASK, OVP_INT_MASK_OPEN);

	if ((value & OVP_STATUS_VERIFY) && !(mask & OVP_IRQ_MASK_BIT))
		irq_type = ANA_FSA4480_OVP_IRQ_EXIST;
	hwlog_info("%s 0x02 is 0x%x, 0x01 is 0x%x, irq type is %d\n",
		__func__, value, mask, irq_type);

	switch (irq_type) {
	case ANA_FSA4480_OVP_IRQ_EXIST:
		hwlog_info("%s ANA_FSA4480_OVP_IRQ_EXIST\n", __func__);
#ifdef CONFIG_HUAWEI_DSM
		ovp_dmd_report();
#endif
		break;
	default:
		hwlog_info("%s ANA_FSA4480_OVP_IRQ_MAX\n", __func__);
		break;
	}
	__pm_relax(g_fsa_data->extern_pdata->wake_lock);
}

static void get_res_irq_type(unsigned int value)
{
	int irq_type = ANA_FSA4480_IQR_MAX;
	unsigned int mask = 0x00;

	__pm_wakeup_event(g_fsa_data->extern_pdata->wake_lock, WAKE_LOCK_TIMEOUT);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_INT_MASK, &mask);
	fsa4480_clear_res_interrupt();

	if ((value & ANA_FSA4480_AUDIO_JACK_DETECT_AND_CONFIG_IRQ_BIT) &&
		!(mask & ANA_FSA4480_AUDIO_JACK_DETECT_AND_CONFIG_IRQ_MASK_BIT))
		irq_type = ANA_FSA4480_AUDIO_JACK_DETECT_AND_CONFIG;
	else if ((value & ANA_FSA4480_RES_BELOW_THRESHOLD_IRQ_BIT) &&
		!(mask & ANA_FSA4480_RES_BELOW_THRESHOLD_IRQ_MASK_BIT))
		irq_type = ANA_FSA4480_RES_BELOW_THRESHOLD;
	else if ((value & ANA_FSA4480_RES_DETECT_ACTION_IRQ_BIT)  &&
		!(mask & ANA_FSA4480_RES_DETECT_ACTION_IRQ_MASK_BIT))
		irq_type = ANA_FSA4480_RES_DETECT_ACTION;

	hwlog_info("%s 0x18 is 0x%x, 0x19 is 0x%x, irq type is %d\n",
		__func__, value, mask, irq_type);

	switch (irq_type) {
	case ANA_FSA4480_AUDIO_JACK_DETECT_AND_CONFIG:
		hwlog_info("%s AUDIO_JACK_DETECT_AND_CONFIG\n", __func__);
		__pm_relax(g_fsa_data->extern_pdata->wake_lock);
		break;
	case ANA_FSA4480_RES_BELOW_THRESHOLD:
		hwlog_info("%s RES_BELOW_THRESHOLD\n", __func__);
		schedule_delayed_work(&g_fsa_data->extern_pdata->res_detect_dw, 0);
		break;
	case ANA_FSA4480_RES_DETECT_ACTION:
		hwlog_info("%s RES_DETECT_ACTION\n", __func__);
		__pm_relax(g_fsa_data->extern_pdata->wake_lock);
		break;
	default:
		hwlog_info("%s IQR_MAX\n", __func__);
		fsa4480_set_interrupt_pin();
		__pm_relax(g_fsa_data->extern_pdata->wake_lock);
		break;
	}
}

static void get_irq_type_work(struct work_struct *work)
{
	unsigned int ovp_int = 0;
	unsigned int ovp_mask = OVP_INT_MASK_OPEN;
	unsigned int res_int = 0;
	unsigned int res_mask = RES_INT_MASK_OPEN;

	IN_FUNCTION;

	/* res irq or ovp irq detect */
	regmap_read(g_fsa_data->regmap, FSA4480_REG_OVP_INT, &ovp_int);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_INT, &res_int);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_OVP_INT_MASK, &ovp_mask);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_INT_MASK, &res_mask);

	if ((ovp_int & OVP_STATUS_VERIFY) && (ovp_mask == OVP_INT_MASK_CLEAR))
		get_ovp_irq_type(ovp_int);
	if (res_int && (res_mask == RES_INT_MASK_CLEAR))
		get_res_irq_type(res_int);

	OUT_FUNCTION;
}

irqreturn_t fsa4480_irq_handler(int irq, void *data)
{
	IN_FUNCTION;

	__pm_wakeup_event(g_fsa_data->extern_pdata->wake_lock, WAKE_LOCK_TIMEOUT);
	schedule_delayed_work(&g_fsa_data->extern_pdata->get_irq_type_dw, WAIT_REG_TIMEOUT);

	OUT_FUNCTION;
	return IRQ_HANDLED;
}

static void fsa4480_reg_init(void)
{
	unsigned int reg_value = 0x00;

	regmap_write(g_fsa_data->regmap, FSA4480_REG_OVP_INT_MASK, OVP_INT_MASK_OPEN);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_OVP_INT, &reg_value);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_INT, &reg_value);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_RES_INT_MASK, &reg_value);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_RES_INT_MASK,
		reg_value | RES_INT_MASK_OPEN);
	regmap_read(g_fsa_data->regmap, FSA4480_REG_ENABLE, &reg_value);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_ENABLE, reg_value & RES_DISABLE);
	regmap_write(g_fsa_data->regmap, FSA4480_REG_RES_DET_THRD, RES_INT_THRESHOLD);
}

static void fsa4480_extern_init(void)
{
	int i;

	IN_FUNCTION;
	g_fsa_data->extern_pdata = kzalloc(sizeof(struct fsa4480_extern_ops_data), GFP_KERNEL);
	if (!g_fsa_data->extern_pdata)
		return;

	g_fsa_data->extern_pdata->default_pin = 0x01; /* 0x01 means D+ */
	g_fsa_data->extern_pdata->water_intruded = false;  /* default is false */
	g_fsa_data->extern_pdata->report_when_detach = false;
	g_fsa_data->extern_pdata->notified = false;
	g_fsa_data->extern_pdata->typec_detach = ANA_HS_TYPEC_DEVICE_DETACHED;
	for (i = 0; i < PIN_NUM; i++)
		g_fsa_data->extern_pdata->res_value_old[i] = 0xff; /* high res value */

	g_fsa_data->extern_pdata->wake_lock = wakeup_source_register(NULL, "ana_hs_extern_ops");

	INIT_DELAYED_WORK(&g_fsa_data->extern_pdata->notify_water_dw, notify_water_work);
	INIT_DELAYED_WORK(&g_fsa_data->extern_pdata->res_detect_dw,
		fsa4480_res_detect_work);
	INIT_DELAYED_WORK(&g_fsa_data->extern_pdata->stop_ovp_dw,
		fsa4480_stop_ovp_detect_work);
	INIT_DELAYED_WORK(&g_fsa_data->extern_pdata->get_irq_type_dw,
		get_irq_type_work);
	INIT_DELAYED_WORK(&g_fsa_data->extern_pdata->detect_onboot_dw,
		fsa4480_detect_onboot_work);
	fsa4480_reg_init();
	water_detect_ops_register(&fsa4480_extern_water_detect_ops);

	schedule_delayed_work(&g_fsa_data->extern_pdata->detect_onboot_dw,
		msecs_to_jiffies(DETECT_ONBOOT_DELAY));

	OUT_FUNCTION;
}

static void fsa4480_extern_remove(void)
{
	if (!g_fsa_data)
		return;

	fsa4480_clear_res_detect();
	fsa4480_clear_res_interrupt();
	fsa4480_cancel_res_detect_work();
	kfree(g_fsa_data->handler);
	g_fsa_data->handler = NULL;
	kfree(g_fsa_data->extern_pdata);
	g_fsa_data->extern_pdata = NULL;
	g_fsa_data = NULL;
	dev_info(g_fsa_data->dev, "%s: exit\n", __func__);
}

static int fsa4480_parse_dt_irq_pinctrl(struct device *dev)
{
	const char *fsa4480_irq_pinctrl = "fsa4480_irq_pinctrl";
	struct pinctrl *pinctrl = NULL;
	struct pinctrl_state *pin_default_state = NULL;
	const char *pin_ctrl_state_name = "default";
	bool support_irq_pinctrl = false;
	int ret = 0;

	support_irq_pinctrl = of_property_read_bool(dev->of_node,
		fsa4480_irq_pinctrl);
	if (support_irq_pinctrl == false) {
		hwlog_debug("%s: node:%s not existed, skip\n",
			__func__, fsa4480_irq_pinctrl);
		return ret;
	}

	pinctrl = devm_pinctrl_get(dev);
	if (IS_ERR_OR_NULL(pinctrl)) {
		hwlog_err("%s: get pinctrl fail %d\n",
			__func__, IS_ERR(pinctrl));
		ret = -EINVAL;
		return ret;
	}

	pin_default_state = pinctrl_lookup_state(pinctrl, pin_ctrl_state_name);
	if (IS_ERR_OR_NULL(pin_default_state)) {
		ret = -EINVAL;
		hwlog_err("%s: pinctrl_lookup_state fail %d\n",
			__func__, IS_ERR(pin_default_state));
		return ret;
	}

	if (pinctrl_select_state(pinctrl, pin_default_state) != 0) {
		ret = -EINVAL;
		hwlog_err("%s: pinctrl_select_state fail\n", __func__);
		return ret;
	}

	return ret;
}


static int fsa4480_i2c_irq_parse_handler(struct device_node *node,
	struct fsa4480_irq_handler **handler)
{
	struct fsa4480_irq_handler *handler_info = NULL;
	const char *gpio_irq_str = "gpio_irq";
	const char *irq_flag_str = "irq_flag";
	int ret;

	handler_info = kzalloc(sizeof(struct fsa4480_irq_handler), GFP_KERNEL);
	if (!handler_info)
		return -ENOMEM;

	handler_info->gpio = of_get_named_gpio(node, gpio_irq_str, 0);
	if (handler_info->gpio < 0) {
		hwlog_err("%s: get named irq gpio failed, %d\n", __func__,
			handler_info->gpio);
		ret = of_property_read_u32(node, gpio_irq_str,
			(u32 *)&handler_info->gpio);
		if (ret < 0) {
			hwlog_err("%s: read irq gpio failed, %d\n",
				__func__, ret);
			ret = -EFAULT;
			goto err_out;
		}
	}

	ret = of_property_read_u32(node, irq_flag_str,
		(u32 *)&handler_info->irq_flag);
	if (ret < 0) {
		hwlog_err("%s: get irq flag failed\n", __func__);
		ret = -EFAULT;
		goto err_out;
	}

	handler_info->irq_flag = IRQF_TRIGGER_FALLING;
	*handler = handler_info;
	return 0;

err_out:
	kfree(handler_info);
	handler_info = NULL;
	return ret;
}

static int fsa4480_i2c_irq_init(struct i2c_client *i2c,
	struct fsa4480_priv *i2c_priv)
{
	const char *irq_handler_str = "irq_handler";
	struct device_node *node = NULL;
	irq_handler_t handler_func = NULL;
	int ret;

	ret = fsa4480_parse_dt_irq_pinctrl(&i2c->dev);
	if (ret < 0)
		hwlog_err("%s: parse pinctrl fail", __func__);

	node = of_get_child_by_name(i2c->dev.of_node, irq_handler_str);
	if (!node) {
		dev_err(i2c_priv->dev, "%s: irq_handler not existed, skip\n", __func__);
		return -1;
	}

	ret = fsa4480_i2c_irq_parse_handler(node, &i2c_priv->handler);
	if (ret < 0)
		return ret;

	i2c_priv->handler->irq = gpio_to_irq((unsigned int)i2c_priv->handler->gpio);
	hwlog_info("%s i2c_priv->handler->irq is %d\n", __func__, i2c_priv->handler->irq);

	handler_func = fsa4480_irq_handler;
	ret = request_threaded_irq(i2c_priv->handler->irq, NULL,
		handler_func, i2c_priv->handler->irq_flag | IRQF_ONESHOT | IRQF_NO_SUSPEND,
		"fsa4480_i2c_irq", NULL);
	if (ret < 0) {
		dev_err(i2c_priv->dev, "fsa4480_i2c_irq request fail, ret = %d\n", ret);
		goto err_out;
	}

	return 0;
err_out:
	kfree(i2c_priv->handler);
	i2c_priv->handler = NULL;
	return ret;
}


static int fsa4480_probe(struct i2c_client *i2c,
			 const struct i2c_device_id *id)
{
	struct fsa4480_priv *fsa_priv;
	u32 use_powersupply = 0;
	int rc = 0;
	int ret = 0;

	fsa_priv = devm_kzalloc(&i2c->dev, sizeof(*fsa_priv),
				GFP_KERNEL);
	if (!fsa_priv)
		return -ENOMEM;

	memset(fsa_priv, 0, sizeof(struct fsa4480_priv));
	fsa_priv->dev = &i2c->dev;

	fsa_priv->regmap = devm_regmap_init_i2c(i2c, &fsa4480_regmap_config);
	if (IS_ERR_OR_NULL(fsa_priv->regmap)) {
		dev_err(fsa_priv->dev, "%s: Failed to initialize regmap: %d\n",
			__func__, rc);
		if (!fsa_priv->regmap) {
			rc = -EINVAL;
			goto err_data;
		}
		rc = PTR_ERR(fsa_priv->regmap);
		goto err_data;
	}

	fsa4480_update_reg_defaults(fsa_priv->regmap);

	fsa_priv->nb.notifier_call = fsa4480_usbc_event_changed;
	fsa_priv->nb.priority = 0;
	rc = of_property_read_u32(fsa_priv->dev->of_node,
			"qcom,use-power-supply", &use_powersupply);
	if (rc || use_powersupply == 0) {
		dev_dbg(fsa_priv->dev,
			"%s: Looking up %s property failed or disabled\n",
			__func__, "qcom,use-power-supply");

		fsa_priv->use_powersupply = 0;
		rc = register_ucsi_glink_notifier(&fsa_priv->nb);
		if (rc) {
			dev_err(fsa_priv->dev,
				"%s: ucsi glink notifier registration failed: %d\n",
				__func__, rc);
			goto err_data;
		}
	} else {
		fsa_priv->use_powersupply = 1;
		fsa_priv->usb_psy = power_supply_get_by_name("usb");
		if (!fsa_priv->usb_psy) {
			rc = -EPROBE_DEFER;
			dev_err(fsa_priv->dev,
				"%s: could not get USB psy info: %d\n",
				__func__, rc);
			goto err_data;
		}

		fsa_priv->iio_ch = iio_channel_get(fsa_priv->dev, "typec_mode");
		if (!fsa_priv->iio_ch) {
			dev_err(fsa_priv->dev,
				"%s: iio_channel_get failed for typec_mode\n",
				__func__);
			goto err_supply;
		}
		rc = power_supply_reg_notifier(&fsa_priv->nb);
		if (rc) {
			dev_err(fsa_priv->dev,
				"%s: power supply reg failed: %d\n",
				__func__, rc);
			goto err_supply;
		}
	}

	fsa_priv->pogo_state = false;
	mutex_init(&fsa_priv->notification_lock);
	i2c_set_clientdata(i2c, fsa_priv);

	INIT_WORK(&fsa_priv->usbc_analog_work,
		fsa4480_usbc_analog_work_fn);

	fsa_priv->fsa4480_notifier.rwsem =
		(struct rw_semaphore)__RWSEM_INITIALIZER
		((fsa_priv->fsa4480_notifier).rwsem);
	fsa_priv->fsa4480_notifier.head = NULL;

	ret = fsa4480_i2c_irq_init(i2c, fsa_priv);
	if (ret < 0) {
		dev_err(fsa_priv->dev, "%s: irq init fail\n", __func__);
	} else {
		g_fsa_data = fsa_priv;
		fsa4480_extern_init();
	}

	return 0;

err_supply:
	power_supply_put(fsa_priv->usb_psy);
err_data:
	devm_kfree(&i2c->dev, fsa_priv);
	return rc;
}

static int fsa4480_remove(struct i2c_client *i2c)
{
	struct fsa4480_priv *fsa_priv =
			(struct fsa4480_priv *)i2c_get_clientdata(i2c);

	if (!fsa_priv)
		return -EINVAL;

	if (fsa_priv->use_powersupply) {
		/* deregister from PMI */
		power_supply_unreg_notifier(&fsa_priv->nb);
		power_supply_put(fsa_priv->usb_psy);
	} else {
		unregister_ucsi_glink_notifier(&fsa_priv->nb);
	}

	fsa4480_usbc_update_settings(fsa_priv, 0x18, 0x98);
	cancel_work_sync(&fsa_priv->usbc_analog_work);
	pm_relax(fsa_priv->dev);
	mutex_destroy(&fsa_priv->notification_lock);
	fsa4480_extern_remove();
	dev_set_drvdata(&i2c->dev, NULL);

	return 0;
}

static const struct of_device_id fsa4480_i2c_dt_match[] = {
	{
		.compatible = "qcom,fsa4480-i2c",
	},
	{}
};

static struct i2c_driver fsa4480_i2c_driver = {
	.driver = {
		.name = FSA4480_I2C_NAME,
		.of_match_table = fsa4480_i2c_dt_match,
	},
	.probe = fsa4480_probe,
	.remove = fsa4480_remove,
};

static int __init fsa4480_init(void)
{
	int rc;

	rc = i2c_add_driver(&fsa4480_i2c_driver);
	if (rc)
		pr_err("fsa4480: Failed to register I2C driver: %d\n", rc);

	return rc;
}
module_init(fsa4480_init);

static void __exit fsa4480_exit(void)
{
	i2c_del_driver(&fsa4480_i2c_driver);
}
module_exit(fsa4480_exit);

MODULE_DESCRIPTION("FSA4480 I2C driver");
MODULE_LICENSE("GPL v2");
