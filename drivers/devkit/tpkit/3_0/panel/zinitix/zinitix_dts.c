/*
 * zinitix_dts.c
 *
 * parase dts config for zinitix driver
 *
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/of.h>

#include <linux/i2c.h>
#include <linux/delay.h>

#include "zinitix_core.h"
#include "zinitix_dts.h"

int touch_driver_get_vendor_name(struct zinitix_platform_data *zinitix_pdata)
{
	int ret;
	const char *producer = "zinitix";
	char comp_name[ZINITIX_VENDOR_COMP_NAME_LEN] = { 0 };
	struct device_node *np = NULL;

	if (zinitix_pdata == NULL) {
		TS_LOG_ERR("%s, param invalid\n", __func__);
		return -EINVAL;
	}

	ret = snprintf(comp_name, ZINITIX_VENDOR_COMP_NAME_LEN, "%s-%s",
			ZINITIX_CHIP_NAME, zinitix_pdata->project_id);
	if (ret >= ZINITIX_VENDOR_COMP_NAME_LEN) {
		TS_LOG_INFO("%s:%s, ret=%d, size=%u\n", __func__,
				"compatible_name out of range", ret,
				ZINITIX_VENDOR_COMP_NAME_LEN);
		return -EINVAL;
	}

	ret = of_property_read_string(np, "producer", &producer);
	if (ret) {
		TS_LOG_ERR("%s:find producer in dts fail, ret=%d\n",
				__func__, ret);
	}

	strlcpy(zinitix_pdata->vendor_name, producer, ZINITIX_VENDOR_NAME_LEN);
	zinitix_pdata->vendor_name[ZINITIX_VENDOR_NAME_LEN - 1] = '\0';
	strlcpy(zinitix_pdata->zinitix_device_data->module_name,
			zinitix_pdata->vendor_name,
			(ZINITIX_VENDOR_NAME_LEN < (MAX_STR_LEN - 1))
			? ZINITIX_VENDOR_NAME_LEN : (MAX_STR_LEN - 1));

	return 0;
}

static int touch_driver_parse_report_config_dts(struct device_node *np,
		struct ts_kit_device_data *dev_data)
{
	int ret;

	if (dev_data == NULL || np == NULL) {
		TS_LOG_ERR("%s, param invalid\n", __func__);
		return -EINVAL;
	}

	ret = of_property_read_u32(np, ZINITIX_ALGO_ID, &dev_data->algo_id);
	if (ret) {
		TS_LOG_ERR("%s:get algo id failed\n", __func__);
		return -ENODATA;
	}

	ret = of_property_read_u32(np, ZINITIX_X_MAX, &dev_data->x_max);
	if (ret) {
		TS_LOG_ERR("%s:get device x_max fail, ret=%d\n",
				__func__, ret);
		return -ENODATA;
	}

	ret = of_property_read_u32(np, ZINITIX_Y_MAX, &dev_data->y_max);
	if (ret) {
		TS_LOG_ERR("%s:get device y_max fail, ret=%d\n",
				__func__, ret);
		return -ENODATA;
	}

	ret = of_property_read_u32(np, ZINITIX_X_MAX_MT, &dev_data->x_max_mt);
	if (ret) {
		TS_LOG_ERR("%s:get device x_max fail, ret=%d\n",
				__func__, ret);
		return -ENODATA;
	}

	ret = of_property_read_u32(np, ZINITIX_Y_MAX_MT, &dev_data->y_max_mt);
	if (ret) {
		TS_LOG_ERR("%s:get device y_max fail, ret=%d\n",
				__func__, ret);
		return -ENODATA;
	}

	TS_LOG_INFO("%s:%s=%d, %s=%d, %s=%d, %s=%d, %s=%d\n", __func__,
			"algo_id", dev_data->algo_id,
			"x_max", dev_data->x_max,
			"y_max", dev_data->y_max,
			"x_mt", dev_data->x_max_mt,
			"y_mt", dev_data->y_max_mt);

	return 0;
}

static void touch_driver_of_property_read_u8_default(struct device_node *np,
		char *prop_name,
		u8 *out_value,
		u8 default_value)
{
	int ret;

	ret = of_property_read_u8(np, prop_name, out_value);
	if (ret) {
		TS_LOG_INFO("%s:%s not set in dts, use default\n",
				__func__, prop_name);
		*out_value = default_value;
	}
}

static void touch_driver_of_property_read_u32_default(struct device_node *np,
		char *prop_name,
		u32 *out_value,
		u32 default_value)
{
	int ret;

	ret = of_property_read_u32(np, prop_name, out_value);
	if (ret) {
		TS_LOG_INFO("%s:%s not set in dts, use default\n",
				__func__, prop_name);
		*out_value = default_value;
	}
}

static void zinitix_of_property_read_u16_default(struct device_node *np,
		char *prop_name,
		u16 *out_value,
		u16 default_value)
{
	int ret;

	ret = of_property_read_u16(np, prop_name, out_value);
	if (ret) {
		TS_LOG_INFO("%s:%s not set in dts, use default\n",
				__func__, prop_name);
		*out_value = default_value;
	}
}

int read_dts_default(struct device_node *np,
		struct ts_kit_device_data *dev_data,
		struct zinitix_platform_data *zinitix_pdata)
{
	int ret = 0;

	touch_driver_of_property_read_u32_default(np, ZINITIX_SLAVE_ADDR,
			&dev_data->slave_addr, ZINITIX_SLAVE_ADDR_DFT);

	touch_driver_of_property_read_u32_default(np, ZINITIX_POWER_SELF_CTRL,
			&zinitix_pdata->self_ctrl_power, 0);

	touch_driver_of_property_read_u32_default(np, ZINITIX_POWER_DOWN_CTRL,
			&zinitix_pdata->power_down_ctrl, 0);

	if (zinitix_pdata->self_ctrl_power == ZINITIX_SELF_CTRL_POWER) {
		touch_driver_of_property_read_u32_default(np,
				ZINITIX_VCI_REGULATOR_TYPE,
				&dev_data->vci_regulator_type, 0);
		if (dev_data->vci_regulator_type == ZINITIX_LDO_POWER)
			touch_driver_of_property_read_u32_default(np,
					ZINITIX_VCI_LDO_VALUE,
					&dev_data->regulator_ctr.vci_value,
					ZINITIX_VCI_LDO_VALUE_DFT);

		touch_driver_of_property_read_u32_default(np, ZINITIX_VCI_GPIO_TYPE,
				&dev_data->vci_gpio_type, 0);
		if (dev_data->vci_gpio_type == ZINITIX_GPIO_POWER) {
			dev_data->vci_gpio_ctrl = of_get_named_gpio(np,
					ZINITIX_VCI_GPIO_CTRL, 0);
			if (!gpio_is_valid(dev_data->vci_gpio_ctrl)) {
				TS_LOG_ERR("%s, Fail to get VCI control GPIO\n",
					__func__);
				return -EINVAL;
			}
			TS_LOG_INFO("get vci_gpio_ctrl = %d\n",
				dev_data->vci_gpio_ctrl);
		}

		touch_driver_of_property_read_u32_default(np,
				ZINITIX_VDDIO_REGULATOR_TYPE,
				&dev_data->vddio_regulator_type, 0);
		if (dev_data->vddio_regulator_type == ZINITIX_LDO_POWER)
			touch_driver_of_property_read_u32_default(np,
					ZINITIX_VDDIO_LDO_VALUE,
					&dev_data->regulator_ctr.vddio_value,
					ZINITIX_VDDIO_LDO_VALUE_DFT);

		touch_driver_of_property_read_u32_default(np,
				ZINITIX_VDDIO_GPIO_TYPE,
				&dev_data->vddio_gpio_type, 0);
		if (dev_data->vddio_gpio_type == ZINITIX_GPIO_POWER) {
			dev_data->vddio_gpio_ctrl = of_get_named_gpio(np,
					ZINITIX_VDDIO_GPIO_CTRL, 0);
			if (!gpio_is_valid(dev_data->vddio_gpio_ctrl)) {
				TS_LOG_ERR("%s, Fail to get VDDIO control GPIO\n",
					__func__);
				return -EINVAL;
			}
			TS_LOG_INFO("get vddio_gpio_ctrl = %d\n",
				dev_data->vddio_gpio_ctrl);
		}
	} else {
		TS_LOG_INFO("%s, power control by LCD, nothing to do\n",
			__func__);
	}
	return ret;
}

int touch_driver_parse_ic_config_dts(struct device_node *np,
		struct ts_kit_device_data *dev_data)
{
	int ret;
	struct zinitix_platform_data *zinitix_pdata = NULL;

	if (dev_data == NULL || np == NULL) {
		TS_LOG_ERR("%s, param invalid\n", __func__);
		return -EINVAL;
	}

	zinitix_pdata = zinitix_get_platform_data();
	if (zinitix_pdata == NULL) {
		TS_LOG_ERR("%s:zinitix_pdata is null pointer\n", __func__);
		return -EINVAL;
	}
	ret = read_dts_default(np, dev_data, zinitix_pdata);
	if (ret < 0)
		return ret;

	ret = of_property_read_u32(np, ZINITIX_IC_TYPES, &dev_data->ic_type);
	if (ret)
		TS_LOG_ERR("%s:get ic_type fail, ret=%d\n", __func__, ret);

	TS_LOG_INFO("%s:%s=%d, %s=%d, %s=%d, %s=%d, %s=%d, %s=%d, %s=%d,\
			%s=%d, %s=%d, %s=%d, %s=%d\n", __func__,
			"slave_addr", dev_data->slave_addr,
			"vci_regulator_type", dev_data->vci_regulator_type,
			"vddio_regulator_type", dev_data->vddio_regulator_type,
			"vci_value", dev_data->regulator_ctr.vci_value,
			"vddio_value", dev_data->regulator_ctr.vddio_value,
			"vci_gpio_type", dev_data->vci_gpio_type,
			"vddio_gpio_type", dev_data->vddio_gpio_type,
			"vci_gpio_ctrl", dev_data->vci_gpio_ctrl,
			"vddio_gpio_ctrl", dev_data->vddio_gpio_ctrl,
			"power_self_ctrl", zinitix_pdata->self_ctrl_power,
			"power_down_ctrl", zinitix_pdata->power_down_ctrl);

	return 0;
}

static int zinitix_get_lcd_panel_info(void)
{
	struct device_node *dev_node = NULL;
	char *lcd_type = NULL;
	struct zinitix_platform_data *zinitix_pdata = NULL;

	zinitix_pdata = zinitix_get_platform_data();
	if (zinitix_pdata == NULL) {
		TS_LOG_ERR("%s:zinitix_pdata is null pointer\n", __func__);
		return -EINVAL;
	}

	dev_node = of_find_compatible_node(NULL, NULL,
			LCD_PANEL_TYPE_DEVICE_NODE_NAME);
	if (!dev_node) {
		TS_LOG_ERR("%s: NOT found device node[%s]!\n", __func__,
				LCD_PANEL_TYPE_DEVICE_NODE_NAME);
		return -EINVAL;
	}

	lcd_type = (char *)of_get_property(dev_node, "lcd_panel_type", NULL);
	if (!lcd_type) {
		TS_LOG_ERR("%s: Get lcd panel type faile!\n", __func__);
		return -EINVAL;
	}

	strlcpy(zinitix_pdata->lcd_panel_info, lcd_type,
			LCD_PANEL_INFO_MAX_LEN - 1);
	TS_LOG_INFO("lcd_panel_info = %s.\n", zinitix_pdata->lcd_panel_info);

	return 0;
}

int parse_glove_roi_dts(struct device_node *np,
		struct zinitix_platform_data *zinitix_pdata)
{
	struct ts_wakeup_gesture_enable_info *gesture_info = NULL;
	struct ts_glove_info *glove_info = NULL;
	struct ts_holster_info *holster_info = NULL;
	struct ts_roi_info *roi_info = NULL;
	struct ts_kit_device_data *dev_data = zinitix_pdata->zinitix_device_data;
	/*
	 * 0 is cover without glass,
	 * 1 is cover with glass that need glove mode
	 * if not define in dtsi,set 0 to disable it
	 */
	glove_info = &(dev_data->ts_platform_data->feature_info.glove_info);
	touch_driver_of_property_read_u8_default(np, ZINITIX_GLOVE_SUPPORTED,
			&glove_info->glove_supported, 1);
	if (glove_info->glove_supported)
		zinitix_of_property_read_u16_default(np,
				ZINITIX_GLOVE_SWITCH_ADDR,
				&glove_info->glove_switch_addr, 0);

	holster_info = &dev_data->ts_platform_data->feature_info.holster_info;
	touch_driver_of_property_read_u8_default(np,
			ZINITIX_HOLSTER_SUPPORTED,
			&holster_info->holster_supported, 0);
	if (holster_info->holster_supported)
		zinitix_of_property_read_u16_default(np,
				ZINITIX_HOSTLER_SWITCH_ADDR,
				&holster_info->holster_switch_addr, 0);

	roi_info = &dev_data->ts_platform_data->feature_info.roi_info;
	touch_driver_of_property_read_u8_default(np,
			ZINITIX_ROI_SUPPORTED, &roi_info->roi_supported, 0);
	if (roi_info->roi_supported) {
		zinitix_of_property_read_u16_default(np,
				ZINITIX_ROI_SWITCH_ADDR,
				&roi_info->roi_control_addr, 0);
		touch_driver_of_property_read_u32_default(np,
				ZINITIX_ROI_PKG_NUM_ADDR,
				&zinitix_pdata->roi_pkg_num_addr, 0);
	} else {
		zinitix_pdata->roi_pkg_num_addr = 0;
	}
	TS_LOG_INFO("%s:%s=%d\n", __func__, "irq_config", dev_data->irq_config);

	touch_driver_of_property_read_u32_default(np,
		ZINITIX_PROJECTID_LEN_CTRL_FLAG,
		&zinitix_pdata->projectid_length_control_flag, 0);
	TS_LOG_INFO("%s:projectid_len_control =%d\n", __func__,
			zinitix_pdata->projectid_length_control_flag);

	gesture_info = &(dev_data->ts_platform_data->feature_info.wakeup_gesture_enable_info);
	touch_driver_of_property_read_u8_default(np,
		ZINITIX_GESTURE_SUPPORTED, &gesture_info->switch_value, 0);

	TS_LOG_ERR("%s:gesture_supported =%d\n", __func__,
			gesture_info->switch_value);

	TS_LOG_INFO("%s:glove_supported=%d,glove_switch_addr = 0x%04x,\
		holster_supported = %d,holster_switch_addr = 0x%04x,\
		touch_switch_flag = %d\n",
			__func__,
			glove_info->glove_supported,
			glove_info->glove_switch_addr,
			holster_info->holster_supported,
			holster_info->holster_switch_addr,
			zinitix_pdata->zinitix_device_data->touch_switch_flag);

	TS_LOG_INFO("%s: pram_projectid_addr is %x\n", __func__,
			zinitix_pdata->pram_projectid_addr);

	TS_LOG_INFO("%s:,roi_info=%d,roi_control_addr=0x%04x\n", __func__,
			roi_info->roi_supported,
			roi_info->roi_control_addr);
	return 0;
}

int parse_distinguishlcd_touchflags_dts(struct device_node *np,
		struct zinitix_platform_data *zinitix_pdata)
{
	int ret;

	if (zinitix_pdata->need_distinguish_lcd) {
		ret = zinitix_get_lcd_panel_info();
		if (ret)
			TS_LOG_ERR("%s get lcd panel info  failed, ret = %d.\n",
					__func__, ret);
	}

	ret = of_property_read_u32(np, ZINITIX_TOUCH_SWITCH_FLAG,
			&zinitix_pdata->zinitix_device_data->touch_switch_flag);
	if (ret) {
		TS_LOG_INFO("%s get touch_switch_flag failed, use default(0)\n",
				__func__);
		zinitix_pdata->zinitix_device_data->touch_switch_flag = 0;
	} else {
		if ((zinitix_pdata->zinitix_device_data->touch_switch_flag
			& TS_SWITCH_TYPE_GAME) == TS_SWITCH_TYPE_GAME) {
			ret = of_property_read_u8(np,
				ZINITIX_TOUCH_SWITCH_GAME_REG,
				&zinitix_pdata->touch_switch_game_reg);
			if (ret) {
				TS_LOG_INFO("%s get touch_switch_game_reg failed, use default(0)\n",
					__func__);
				zinitix_pdata->touch_switch_game_reg = 0;
			}
		}
	}
	return 0;
}

int parse_watchdog_lcdpower_dts(struct device_node *np,
	struct zinitix_platform_data *zinitix_pdata)
{
	int ret;
	struct ts_kit_device_data *dev_data = zinitix_pdata->zinitix_device_data;

	ret = of_property_read_u32(np, ZINITIX_CHECK_STATUS_WATCHDOG_TIMEOUT,
			&dev_data->check_status_watchdog_timeout);
	if (ret) {
		dev_data->check_status_watchdog_timeout = 0;
		TS_LOG_INFO("%s:get check_status_watchdog_timeout from dts failed,\
			use default FT8716 value\n", __func__);
	}

	ret = of_property_read_u32(np, ZINITIX_OPEN_ONCE_THRESHOLD,
			&zinitix_pdata->only_open_once_captest_threshold);
	if (ret) {
		zinitix_pdata->only_open_once_captest_threshold = 0;
		TS_LOG_INFO("%s:get only_open_once_captest_threshold failed\n",
			__func__);
	}
	TS_LOG_INFO("%s: get only_open_once_captest_threshold = %d\n", __func__,
			zinitix_pdata->only_open_once_captest_threshold);

	ret = of_property_read_u32(np, ZINITIX_ENABLE_EDGE_TOUCH,
			&zinitix_pdata->enable_edge_touch);
	if (ret) {
		zinitix_pdata->enable_edge_touch = 0;
		TS_LOG_INFO("%s:get enable_edge_touch failed\n", __func__);
	}
	TS_LOG_INFO("%s: get enable_edge_touch = %d\n", __func__,
			zinitix_pdata->enable_edge_touch);

	if (zinitix_pdata->enable_edge_touch) {
		ret = of_property_read_u32(np, ZINITIX_EDGE_DATA_ADDR,
				&zinitix_pdata->edge_data_addr);
		if (ret) {
			zinitix_pdata->edge_data_addr = 0;
			TS_LOG_INFO("%s:get edge_data_addr failed\n", __func__);
		}
		TS_LOG_INFO("%s: get edge_data_addr = %d\n", __func__,
				zinitix_pdata->edge_data_addr);
	} else {
		zinitix_pdata->edge_data_addr = 0;
	}

	ret = of_property_read_u32(np, ZINITIX_FW_NEED_DISTINGUISH_LCD,
			&zinitix_pdata->need_distinguish_lcd);
	if (ret) {
		zinitix_pdata->need_distinguish_lcd = 0;
		TS_LOG_INFO("%s: get need_distinguish_lcd failed, use default(0)\n",
				__func__);
	}

	return 0;
}

int parse_power_irq_tp_dts(struct device_node *np,
		struct zinitix_platform_data *zinitix_pdata)
{
	u32 need_wd_check_status = 0;
	int ret;
	struct ts_kit_device_data *dev_data = zinitix_pdata->zinitix_device_data;

	ret = touch_driver_parse_report_config_dts(np, dev_data);
	if (ret) {
		TS_LOG_ERR("%s:parse report config fail\n", __func__);
		return ret;
	}

	ret = of_property_read_u32(np, ZINITIX_IRQ_CFG, &dev_data->irq_config);
	if (ret) {
		TS_LOG_ERR("%s:get irq config fail, ret=%d\n", __func__, ret);
		return -ENODATA;
	}

	ret = of_property_read_u32(np, ZINITIX_WD_CHECK,
			&need_wd_check_status);
	dev_data->need_wd_check_status = need_wd_check_status ? true:false;
	if (ret) {
		dev_data->need_wd_check_status = 0;
		TS_LOG_INFO("%s:get need_wd_check_status failed,\
			use default FT8716 value\n", __func__);
	}
	return 0;
}

int zinitix_parse_dts(struct device_node *np,
		struct zinitix_platform_data *zinitix_pdata)
{
	int ret;
	struct ts_kit_device_data *dev_data = NULL;

	dev_data = zinitix_pdata->zinitix_device_data;
	if (dev_data == NULL || np == NULL || zinitix_pdata == NULL) {
		TS_LOG_ERR("%s, param invalid\n", __func__);
		return -EINVAL;
	}

	ret = parse_power_irq_tp_dts(np, zinitix_pdata);
	if (ret < 0)
		return ret;

	ret = parse_watchdog_lcdpower_dts(np, zinitix_pdata);
	if (ret < 0)
		return ret;

	ret = parse_distinguishlcd_touchflags_dts(np, zinitix_pdata);
	if (ret < 0)
		return ret;

	ret = parse_glove_roi_dts(np, zinitix_pdata);
	if (ret < 0)
		return ret;

	return NO_ERR;
}

int zinitix_parse_cap_test_config(struct zinitix_platform_data *pdata,
		struct zinitix_test_params *params)
{
	int ret;

	char comp_name[ZINITIX_VENDOR_COMP_NAME_LEN] = { 0 };
	struct device_node *np = NULL;
	struct zinitix_platform_data *zinitix_pdata = zinitix_get_platform_data();

	if (!zinitix_pdata || !params) {
		TS_LOG_ERR("%s:chip data null\n", __func__);
		return -EINVAL;
	}

	ret = snprintf(comp_name,
			ZINITIX_VENDOR_COMP_NAME_LEN,
			"%s-%s",
			ZINITIX_CHIP_NAME,
			zinitix_pdata->project_id);
	if (ret >= ZINITIX_VENDOR_COMP_NAME_LEN) {
		TS_LOG_INFO("%s:%s, ret=%d, size=%u\n", __func__,
				"compatible_name out of range", ret,
				ZINITIX_VENDOR_COMP_NAME_LEN);
		return -EINVAL;
	}

	TS_LOG_INFO("%s:compatible_name is: %s\n", __func__, comp_name);
	np = of_find_compatible_node(NULL, NULL, comp_name);
	if (!np) {
		TS_LOG_INFO("%s:find vendor node fail\n", __func__);
		return -ENODEV;
	}

	if (params->in_csv_file == ZINITIX_THRESHOLD_IN_CSV_FILE) {
		TS_LOG_INFO("%s: cap threshold in csv file\n", __func__);

	} else {
		TS_LOG_INFO("%s: cap threshold in dts file\n", __func__);
	}

	return 0;
}

