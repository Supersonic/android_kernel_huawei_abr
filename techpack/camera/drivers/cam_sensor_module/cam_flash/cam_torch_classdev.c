/*
 * cam_torch_classdev.c
 *
 * Copyright (c) 2020-2020 Huawei Technologies Co., Ltd.
 *
 * fled regulator interface
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

#include "cam_torch_classdev.h"
#include "cam_flash_dev.h"
#include "cam_res_mgr_api.h"

struct cam_torch_dt_data {
	uint32_t torch_index_array[CAM_FLASH_MAX_LED_TRIGGERS];
	const char *torch_name_array[CAM_FLASH_MAX_LED_TRIGGERS];
	uint32_t torch_num;
};

static void cam_torch_set_brightness(struct led_classdev *cdev,
			enum led_brightness value)
{
	struct torch_classdev_data *data = container_of(cdev,
			struct torch_classdev_data, cdev_torch);

	if (!data->torch_trigger) {
		pr_err("%s, Null\n", __func__);
		return;
	}

	pr_info("%s, value=%d", __func__, value);

	mutex_lock(&data->lock);
	cam_res_mgr_led_trigger_event(data->torch_trigger, value);

	if (!data->switch_trigger) {
		pr_err("%s, No switch_trigger\n", __func__);
		mutex_unlock(&data->lock);
		return;
	}

	if (value > 0)
		cam_res_mgr_led_trigger_event(data->switch_trigger,
			(enum led_brightness)LED_SWITCH_ON);
	else
		cam_res_mgr_led_trigger_event(data->switch_trigger,
			(enum led_brightness)LED_SWITCH_OFF);
	mutex_unlock(&data->lock);
}

static int cam_torch_classdev_parse_dt(const struct device_node *of_node,
	struct cam_torch_dt_data *dt_data)
{
	int rc;
	uint32_t torch_num = 0;
	int count;

	pr_info("%s, Enter\n", __func__);
	if (!of_node || !dt_data) {
		pr_err("%s, Null ptr\n", __func__);
		return -EINVAL;
	}

	if (!of_get_property(of_node, "torch-light-num", &torch_num)) {
		pr_info("%s, No torch\n", __func__);
		return -ENODEV;
	}
	torch_num /= sizeof(uint32_t);
	if (torch_num > CAM_FLASH_MAX_LED_TRIGGERS) {
		pr_err("%s, torch_num is too large: %d\n", __func__, torch_num);
		return -EINVAL;
	}

	count = of_property_count_strings(of_node, "torch-light-name");
	if (count != torch_num) {
		pr_err("%s, invalid count=%d, torch_num=%d\n", __func__, count, torch_num);
		return -EINVAL;
	}

	rc = of_property_read_string_array(of_node, "torch-light-name",
		dt_data->torch_name_array, count);
	if (rc < 0) {
		pr_warning("%s, read string failed\n", __func__);
		return -ENODEV;
	}

	rc = of_property_read_u32_array(of_node, "torch-light-num",
		dt_data->torch_index_array, torch_num);
	if (rc < 0) {
		pr_warning("%s, read index failed\n", __func__);
		return -ENODEV;
	}

	dt_data->torch_num = torch_num;

	pr_info("%s, torch_num:%d\n", __func__, dt_data->torch_num);
	pr_info("%s, Exit\n", __func__);
	return 0;
}

static int cam_torch_classdev_get_data(struct torch_classdev_data *torch_data,
	struct cam_flash_ctrl *fctrl,
	struct cam_torch_dt_data *dt_data,
	unsigned int index)
{
	uint32_t torch_id;

	if (index >= CAM_FLASH_MAX_LED_TRIGGERS) {
		pr_err("%s, invalid index: %d", __func__, index);
		return -EINVAL;
	}

	torch_data->cdev_torch.name = dt_data->torch_name_array[index];
	if (!torch_data->cdev_torch.name) {
		pr_err("%s, get name failed\n", __func__);
		return -ENODEV;
	}

	torch_id = dt_data->torch_index_array[index];
	if (torch_id >= CAM_FLASH_MAX_LED_TRIGGERS) {
		pr_err("%s, invalid torch_id: %d", __func__, torch_id);
		return -EINVAL;
	}

	if (!fctrl->torch_trigger[torch_id]) {
		pr_err("%s, no trigger\n", __func__);
		return -ENODEV;
	}
	torch_data->torch_trigger = fctrl->torch_trigger[torch_id];
	torch_data->cdev_torch.brightness = LED_OFF;
	torch_data->cdev_torch.brightness_set = cam_torch_set_brightness;
	torch_data->switch_trigger = fctrl->switch_trigger;
	return 0;
}

int cam_torch_classdev_register(struct platform_device *pdev, void *data)
{
	int ret;
	int i;
	struct torch_classdev_data *torch_data = NULL;
	struct cam_flash_ctrl *fctrl = data;
	struct cam_torch_dt_data *dt_data = NULL;

	if (!pdev || !data || !pdev->dev.of_node) {
		pr_err("%s, Null ptr\n", __func__);
		return -EINVAL;
	}

	dt_data = devm_kzalloc(&pdev->dev, sizeof(*dt_data), GFP_KERNEL);
	if (!dt_data) {
		pr_err("%s, no mem\n", __func__);
		return -ENOMEM;
	}
	ret = cam_torch_classdev_parse_dt(pdev->dev.of_node, dt_data);
	if (ret < 0) {
		pr_err("%s, no dt\n", __func__);
		devm_kfree(&pdev->dev, dt_data);
		return -ENODEV;
	}

	for (i = 0; i < dt_data->torch_num; i++) {
		torch_data = devm_kzalloc(&pdev->dev,
			sizeof(*torch_data), GFP_KERNEL);
		if (!torch_data) {
			pr_err("%s, i = %d, no mem\n", __func__, i);
			devm_kfree(&pdev->dev, dt_data);
			return -ENOMEM;
		}

		ret = cam_torch_classdev_get_data(torch_data,
			fctrl, dt_data, i);
		if (ret < 0)
			goto exit;

		torch_data->dev = &pdev->dev;

		pr_info("%s, register %s\n", __func__, torch_data->cdev_torch.name);
		ret = devm_led_classdev_register(&pdev->dev,
			&torch_data->cdev_torch);
		if (ret < 0)
			goto exit;
	}

	devm_kfree(&pdev->dev, dt_data);
	return 0;

exit:
	pr_err("%s, %dth register failed\n", __func__, i);
	devm_kfree(&pdev->dev, torch_data);
	devm_kfree(&pdev->dev, dt_data);
	return ret;
}
EXPORT_SYMBOL(cam_torch_classdev_register);
