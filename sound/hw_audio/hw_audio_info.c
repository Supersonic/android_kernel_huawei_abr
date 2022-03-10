/*
 * hw_audio_info.c
 *
 * hw audio priv driver
 *
 * Copyright (c) 2017-2020 Huawei Technologies Co., Ltd.
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

#include "hw_audio_info.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/version.h>
#include <linux/regulator/consumer.h>
#include <linux/pinctrl/consumer.h>
#include <sound/hw_audio/hw_audio_interface.h>

const char *smartpa_name[SMARTPA_TYPE_MAX] = { "none", "cs35lxx", "aw882xx", "tfa98xx" };
const char *smartpa_num[SMARTPA_NUM_MAX] = { "0", "1", "2", "3", "4", "5", "6", "7", "8" };
static struct hw_audio_info g_hw_audio_priv;
int pa_gpio;

static int kcontrol_value_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol, int value)
{
	if (kcontrol == NULL || ucontrol == NULL) {
		pr_err("%s: input pointer is null\n", __func__);
		return -EINVAL;
	}

	ucontrol->value.integer.value[0] = value;
	return 0;
}

static int kcontrol_value_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol, int *value)
{
	if (kcontrol == NULL || ucontrol == NULL || value == NULL) {
		pr_err("%s: input pointer is null\n", __func__);
		return -EINVAL;
	}

	*value = ucontrol->value.integer.value[0];
	return 0;
}

static int hac_gpio_switch(int gpio, int value)
{
	if (!gpio_is_valid(gpio)) {
		pr_err("%s: invalid gpio:%d\n", __func__, gpio);
		return -EINVAL;
	}

	gpio_set_value(gpio, value);
	pr_info("%s:set gpio(%d):%d\n", __func__, gpio, value);
	return 0;
}

static struct hw_audio_info *get_audio_info_priv(void)
{
	return &g_hw_audio_priv;
}

int hac_switch_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	return kcontrol_value_get(kcontrol, ucontrol, priv->hac_switch);
}
EXPORT_SYMBOL_GPL(hac_switch_get);

int hac_switch_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	int ret;
	int value;
	struct hw_audio_info *priv = get_audio_info_priv();

	ret = kcontrol_value_put(kcontrol, ucontrol, &value);
	if (ret != 0)
		return ret;

	priv->hac_switch = value;
	return hac_gpio_switch(priv->hac_gpio, value);
}
EXPORT_SYMBOL_GPL(hac_switch_put);

static int switch_hs_gpio(int gpio, int value)
{
	if (!gpio_is_valid(gpio)) {
		pr_err("%s: invalid gpio:%d\n", __func__, gpio);
		return -EINVAL;
	}

	gpio_set_value(gpio, value);
	pr_info("%s:set gpio(%d):%d\n", __func__, gpio, value);
	return 0;
}

int hs_switch_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	return kcontrol_value_get(kcontrol, ucontrol, priv->hs_switch);
}
EXPORT_SYMBOL_GPL(hs_switch_get);

int hs_switch_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	int ret;
	int value;
	struct hw_audio_info *priv = get_audio_info_priv();

	ret = kcontrol_value_put(kcontrol, ucontrol, &value);
	if (ret != 0)
		return ret;

	priv->hs_switch = value;
	return switch_hs_gpio(priv->hs_gpio, value);
}
EXPORT_SYMBOL_GPL(hs_switch_put);

int simple_pa_mode_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	return kcontrol_value_get(kcontrol, ucontrol, priv->pa_mode);
}
EXPORT_SYMBOL_GPL(simple_pa_mode_get);

int simple_pa_mode_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	int ret;
	int value;
	struct hw_audio_info *priv = get_audio_info_priv();

	ret = kcontrol_value_put(kcontrol, ucontrol, &value);
	if (ret != 0)
		return ret;

	if (value < 0 || value >= ARRAY_SIZE(g_simple_pa_mode_text)) {
		pr_err("%s: pa mode is invalid: %d\n", __func__, value);
		return -EINVAL;
	}

	priv->pa_mode = (priv->pa_type == SIMPLE_PA_TI) ?
		SIMPLE_PA_DEFAULR_MODE : value;
	pr_info("%s: pa mode %d\n", __func__, priv->pa_mode);
	return 0;
}
EXPORT_SYMBOL_GPL(simple_pa_mode_put);

int hw_simple_pa_power_set(int gpio, int value)
{
	int i;
	struct hw_audio_info *priv = get_audio_info_priv();

	if (!gpio_is_valid(gpio)) {
		pr_err("%s: Invalid gpio: %d\n", __func__, gpio);
		return -EINVAL;
	}

	if (value == GPIO_PULL_UP) {
		gpio_set_value(gpio, GPIO_PULL_DOWN);
		msleep(1);
		for (i = 0; i < priv->pa_mode; i++) {
			gpio_set_value(gpio, GPIO_PULL_DOWN);
			udelay(2);
			gpio_set_value(gpio, GPIO_PULL_UP);
			udelay(2);
		}
	} else {
		gpio_set_value(gpio, GPIO_PULL_DOWN);
	}
	return 0;
}

int simple_pa_switch_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	return kcontrol_value_get(kcontrol, ucontrol, priv->pa_switch[SIMPLE_PA_LEFT]);
}
EXPORT_SYMBOL_GPL(simple_pa_switch_get);

int simple_pa_r_switch_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	return kcontrol_value_get(kcontrol, ucontrol, priv->pa_switch[SIMPLE_PA_RIGHT]);
}
EXPORT_SYMBOL_GPL(simple_pa_r_switch_get);

int simple_pa_switch_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	int ret;
	int value;
	struct hw_audio_info *priv = get_audio_info_priv();

	ret = kcontrol_value_put(kcontrol, ucontrol, &value);
	if (ret != 0)
		return ret;

	priv->pa_switch[SIMPLE_PA_LEFT] = value;
	pr_info("%s: pa switch %d\n", __func__, value);
	return hw_simple_pa_power_set(priv->pa_gpio[SIMPLE_PA_LEFT], value);
}
EXPORT_SYMBOL_GPL(simple_pa_switch_put);

int simple_pa_r_switch_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	int ret;
	int value;
	struct hw_audio_info *priv = get_audio_info_priv();

	ret = kcontrol_value_put(kcontrol, ucontrol, &value);
	if (ret != 0)
		return ret;

	priv->pa_switch[SIMPLE_PA_RIGHT] = value;
	pr_err("%s: pa right switch %d\n", __func__, value);
	return hw_simple_pa_power_set(priv->pa_gpio[SIMPLE_PA_RIGHT], value);
}
EXPORT_SYMBOL_GPL(simple_pa_r_switch_put);

bool is_mic_differential_mode(void)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	return priv->mic_differential_mode;
}
EXPORT_SYMBOL_GPL(is_mic_differential_mode);

static int gpio_output_init(int gpio, const char *label, int value)
{
	int ret;

	if (!gpio_is_valid(gpio)) {
		pr_err("%s: invalid gpio %d\n", __func__, gpio);
		return -EINVAL;
	}

	ret = gpio_request(gpio, label);
	if (ret != 0) {
		pr_err("%s: request gpio %d failed %d\n", __func__, gpio, ret);
		return -EINVAL;
	}

	gpio_direction_output(gpio, value);
	return 0;
}

static void hw_hac_init(struct hw_audio_info *priv, struct device_node *np)
{
	int gpio;

	pr_info("%s: enter\n", __func__);
	gpio = of_get_named_gpio_flags(np, "huawei,hac_gpio", 0, NULL);
	if (gpio < 0)
		return;

	if (gpio_output_init(gpio, "hac_gpio", GPIO_PULL_DOWN) != 0)
		return;

	priv->hac_gpio = gpio;
	pr_info("%s: init hac gpio(%d) success\n", __func__, gpio);
}

static void hw_hs_switch_init(struct hw_audio_info *priv, struct device_node *np)
{
	int gpio;

	pr_info("%s: enter\n", __func__);
	gpio = of_get_named_gpio_flags(np, "huawei,hs_gpio", 0, NULL);
	if (gpio < 0) {
		pr_err("%s: get gpio failed from dtsi\n", __func__);
		return;
	}

	if (gpio_output_init(gpio, "hs_gpio", GPIO_PULL_DOWN) != 0)
		return;

	priv->hs_gpio = gpio;
	pr_info("%s: init hs switch gpio(%d) success\n", __func__, gpio);
}

static void get_simple_pa_type(struct hw_audio_info *priv,
	struct platform_device *pdev, int gpio)
{
	int aw_pa_value;
	struct device_node *np = pdev->dev.of_node;
	struct pinctrl *pinctrl = NULL;
	struct pinctrl_state *pin_state = NULL;

	if (!of_property_read_bool(np, "need_identify_pa_vendor")) {
		pr_info("%s: not need identify pa type\n", __func__);
		goto exit;
	}

	if (of_property_read_u32(np, "simple_pa_type_aw", &aw_pa_value) != 0) {
		pr_err("%s: simple_pa_type_aw not config\n", __func__);
		goto exit;
	}

	pinctrl = devm_pinctrl_get(&pdev->dev);
	if (pinctrl == NULL) {
		pr_err("%s: get pinctrl fail\n", __func__);
		goto exit;
	}
	pin_state = pinctrl_lookup_state(pinctrl, "ext_pa_default");
	if (IS_ERR(pin_state)) {
		pr_err("%s: pinctrl_lookup_state fail %d\n",
			__func__, PTR_ERR(pin_state));
		goto exit;
	}
	if (pinctrl_select_state(pinctrl, pin_state) != 0) {
		pr_err("%s: pinctrl_select_state fail\n", __func__);
		goto exit;
	}

	if (gpio_direction_input(gpio) != 0) {
		pr_err("%s: set gpio to input direction fail\n", __func__);
		goto exit;
	}
	if (gpio_get_value(gpio) == aw_pa_value)
		priv->pa_type = SIMPLE_PA_AWINIC;
	else
		priv->pa_type = SIMPLE_PA_TI;

exit:
	pr_info("%s: pa_type is %s\n", __func__,
		priv->pa_type == SIMPLE_PA_AWINIC ? "aw" : "ti");
}

static void simple_pa_init(struct hw_audio_info *priv,
	struct platform_device *pdev)
{
	int gpio;
	struct device_node *np = pdev->dev.of_node;

	gpio = of_get_named_gpio(np, "spk-ext-simple-pa", 0);
	pa_gpio = gpio;
	if (gpio < 0) {
		pr_info("%s: missing spk-ext-simple-pa in dt node\n", __func__);
		return;
	}

	if (!gpio_is_valid(gpio)) {
		pr_err("%s: Invalid ext spk gpio: %d", __func__, gpio);
		return;
	}

	if (gpio_request(gpio, "simple_pa_gpio") != 0) {
		pr_err("%s: request gpio %d fail\n", __func__, gpio);
		return;
	}

	get_simple_pa_type(priv, pdev, gpio);
	gpio_direction_output(gpio, GPIO_PULL_DOWN);
	priv->pa_gpio[SIMPLE_PA_LEFT] = gpio;
	pr_info("%s: init simple pa gpio(%d) success\n", __func__, gpio);
}

int get_pa_en_pin(void)
{
	return pa_gpio;
}
EXPORT_SYMBOL_GPL(get_pa_en_pin);

static void simple_pa_r_init(struct hw_audio_info *priv,
	struct platform_device *pdev)
{
	int gpio;
	struct device_node *np = pdev->dev.of_node;

	gpio = of_get_named_gpio(np, "spk-ext-simple-r-pa", 0);
	if (gpio < 0) {
		pr_info("%s: missing spk-ext-simple-r-pa in dt node\n", __func__);
		return;
	}

	if (!gpio_is_valid(gpio)) {
		pr_err("%s: Invalid ext spk gpio: %d", __func__, gpio);
		return;
	}

	if (gpio_request(gpio, "simple_pa_gpio") != 0) {
		pr_err("%s: request gpio %d fail\n", __func__, gpio);
		return;
	}

	get_simple_pa_type(priv, pdev, gpio);
	gpio_direction_output(gpio, GPIO_PULL_DOWN);
	priv->pa_gpio[SIMPLE_PA_RIGHT] = gpio;
	pr_err("%s: init simple pa right gpio(%d) success\n", __func__, gpio);
}

static void hw_audio_info_priv(struct hw_audio_info *priv)
{
	priv->audio_prop = 0;
	priv->pa_type = SIMPLE_PA_TI;
	priv->pa_mode = SIMPLE_PA_DEFAULR_MODE;
	priv->pa_gpio[SIMPLE_PA_LEFT] = INVALID_GPIO;
	priv->pa_gpio[SIMPLE_PA_RIGHT] = INVALID_GPIO;
	priv->pa_switch[SIMPLE_PA_LEFT] = SWITCH_OFF;
	priv->pa_switch[SIMPLE_PA_RIGHT] = SWITCH_OFF;
	priv->hac_gpio = INVALID_GPIO;
	priv->hac_switch = SWITCH_OFF;
	priv->hs_gpio = INVALID_GPIO;
	priv->hs_switch = SWITCH_OFF;
	priv->mic_differential_mode = false;
	priv->is_init_smartpa_type = false;
}

static void hw_audio_property_init(struct hw_audio_info *priv,
	struct device_node *of_node)
{
	int i;

	if (of_node == NULL) {
		pr_err("hw_audio: %s failed,of_node is NULL\n", __func__);
		return;
	}

	for (i = 0; i < ARRAY_SIZE(audio_prop_table); i++) {
		if (of_property_read_bool(of_node, audio_prop_table[i].key))
			priv->audio_prop |= audio_prop_table[i].value;
	}
	if ((priv->audio_prop & audio_prop_table[0].value) == 0)
		pr_err("hw_audio: check mic config, no master mic found\n");
}

static void set_product_identifier(struct device_node *np, char *buf, int len)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	if (!of_property_read_bool(np, "separate_param_for_different_pa")) {
		pr_info("%s: not need to separate audio param for different pa\n", __func__);
		return;
	}

	if (of_property_read_bool(np, "need_identify_pa_vendor")) {
		if (len <= strlen(buf)) {
			pr_err("%s: buffer len not match\n", __func__);
			return;
		}
		switch (priv->pa_type) {
		case SIMPLE_PA_AWINIC:
			snprintf(buf + strlen(buf), len - strlen(buf), "_%s", "aw");
			break;
		case SIMPLE_PA_TI:
			snprintf(buf + strlen(buf), len - strlen(buf), "_%s", "ti");
			break;
		default:
			pr_err("%s: invalid simple pa type\n", __func__);
			break;
		}
	}
}

static void product_identifier_init(struct device_node *np, char *buf, int len)
{
	const char *string = NULL;

	memset(buf, 0, len);

	if (of_property_read_string(np, "product_identifier", &string) != 0)
		strncpy(buf, "default", strlen("default"));
	else
		strncpy(buf, string, strlen(string));
	set_product_identifier(np, buf, len);
	pr_info("%s:product_identifier %s", __func__, buf);
}

static void smartpa_type_init(struct device_node *np, char *buf, int len)
{
	const char *string = NULL;
	struct hw_audio_info *priv = get_audio_info_priv();

	if (priv->is_init_smartpa_type) {
		pr_info("%s:smartpa type is got", __func__);
		return;
	}
	memset(buf, 0, len);

	if (of_property_read_string(np, "smartpa_type", &string) != 0)
		strncpy(buf, "none", strlen("none"));
	else
		strncpy(buf, string, strlen(string));

	pr_info("%s:smartpa type %s", __func__, buf);
}

void hw_set_smartpa_type(const char *buf, int len)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	if (buf == NULL) {
		pr_err("hw_audio: get smartpa type failed, ptr is NULL.\n");
	}

	memset(priv->smartpa_type_str, 0, sizeof(priv->smartpa_type_str));
	memcpy(priv->smartpa_type_str, buf, len);
	priv->is_init_smartpa_type = true;
	return;
}
EXPORT_SYMBOL(hw_set_smartpa_type);

enum smartpa_type hw_get_smartpa_type(void)
{
	int i;
	struct hw_audio_info *priv = get_audio_info_priv();

	for (i = 0; i < SMARTPA_TYPE_MAX; i++) {
		if (!strncmp(priv->smartpa_type_str, smartpa_name[i], strlen(smartpa_name[i]) + 1)) {
			pr_info("%s: pa is %s", __func__, priv->smartpa_type_str);
			return (enum smartpa_type)i;
		}
	}
	return INVALID;
}
EXPORT_SYMBOL(hw_get_smartpa_type);

int get_smartpa_num(void)
{
	int i;
	struct hw_audio_info *priv = get_audio_info_priv();

	for (i = 0; i < SMARTPA_NUM_MAX; i++) {
		if (!strncmp(priv->smartpa_num_str, smartpa_num[i], strlen(smartpa_num[i]) + 1)) {
			pr_info("%s: pa num is %s", __func__, priv->smartpa_num_str);
			return i;
		}
	}
	return INVALID;
}
EXPORT_SYMBOL(get_smartpa_num);

static void smartpa_num_init(struct device_node *np, char *buf, int len)
{
	const char *string = NULL;
	const char *none_smartpa = "0";

	memset(buf, 0, len);
	if (of_property_read_string(np, "smartpa_num", &string) != 0) {
		strncpy(buf, none_smartpa, strlen(none_smartpa));
	} else {
		strncpy(buf, string, strlen(string));
	}

	pr_info("%s:smartpa_num %s", __func__, buf);
}

static int audio_info_probe(struct platform_device *pdev)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	if (pdev == NULL || pdev->dev.of_node == NULL) {
		pr_err("hw_audio: %s: prop failed, input is NULL\n", __func__);
		return -EINVAL;
	}

	hw_audio_info_priv(priv);
	hw_audio_property_init(priv, pdev->dev.of_node);
	hw_hac_init(priv, pdev->dev.of_node);
	hw_hs_switch_init(priv, pdev->dev.of_node);
	simple_pa_init(priv, pdev);
	simple_pa_r_init(priv, pdev);
	product_identifier_init(pdev->dev.of_node, priv->product_identifier,
		sizeof(priv->product_identifier));
	smartpa_type_init(pdev->dev.of_node, priv->smartpa_type_str,
		sizeof(priv->smartpa_type_str));
	smartpa_num_init(pdev->dev.of_node, priv->smartpa_num_str,
		sizeof(priv->smartpa_num_str));
	if (of_property_read_bool(pdev->dev.of_node, "mic_differential_mode"))
		priv->mic_differential_mode = true;
	return 0;
}

static ssize_t audio_property_show(struct device_driver *driver, char *buf)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	UNUSED(driver);
	if (buf == NULL) {
		pr_err("%s: buf is null", __func__);
		return 0;
	}
	return scnprintf(buf, PAGE_SIZE, "%08X\n", priv->audio_prop);
}
static DRIVER_ATTR_RO(audio_property);

static ssize_t codec_dump_show(struct device_driver *driver, char *buf)
{
	UNUSED(driver);
	UNUSED(buf);
	return 0;
}
static DRIVER_ATTR_RO(codec_dump);

static ssize_t product_identifier_show(struct device_driver *driver, char *buf)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	UNUSED(driver);
	if (buf == NULL) {
		pr_err("%s: buf is null", __func__);
		return 0;
	}
	return scnprintf(buf, PAGE_SIZE, "%s", priv->product_identifier);
}
static DRIVER_ATTR_RO(product_identifier);

static ssize_t smartpa_type_show(struct device_driver *driver, char *buf)
{
	struct hw_audio_info *priv = get_audio_info_priv();

	UNUSED(driver);
	if (buf == NULL) {
		pr_err("%s: buf is null", __func__);
		return 0;
	}
	return scnprintf(buf, PAGE_SIZE, "%s", priv->smartpa_type_str);
}
static DRIVER_ATTR_RO(smartpa_type);

static struct attribute *audio_attrs[] = {
	&driver_attr_audio_property.attr,
	&driver_attr_codec_dump.attr,
	&driver_attr_product_identifier.attr,
	&driver_attr_smartpa_type.attr,
	NULL,
};

static struct attribute_group audio_group = {
	.name = "hw_audio_info",
	.attrs = audio_attrs,
};

static const struct attribute_group *groups[] = {
	&audio_group,
	NULL,
};

static const struct of_device_id audio_info_match_table[] = {
	{ .compatible = "hw,hw_audio_info", },
	{ },
};

static struct platform_driver audio_info_driver = {
	.driver = {
		.name = "hw_audio_info",
		.owner = THIS_MODULE,
		.groups = groups,
		.of_match_table = audio_info_match_table,
	},

	.probe = audio_info_probe,
	.remove = NULL,
};

static int __init audio_info_init(void)
{
	return platform_driver_register(&audio_info_driver);
}

static void __exit audio_info_exit(void)
{
	platform_driver_unregister(&audio_info_driver);
}

module_init(audio_info_init);
module_exit(audio_info_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Huawei audio driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
