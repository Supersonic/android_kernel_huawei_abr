/*
 * radio_tps6381x.c
 *
 * radio_tps6381x driver
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

#include "radio_tps6381x.h"
#include <huawei_platform/log/hw_log.h>

#define BUCKBOOST_MODE 1
#define NO_BUCKBOOST_MODE 0
#define HWLOG_TAG radio_tps6381x
HWLOG_REGIST();

static struct tps63810_device_info *g_tps63810_di;

static bool tps63810_set_enable(unsigned int value, struct tps63810_device_info *tps63810_di)
{
	struct tps63810_device_info *di = g_tps63810_di;

	if (!di) {
		hwlog_err("di is null\n");
		return false;
	}
	if (!gpio_is_valid(di->gpio_no)) {
		hwlog_err("gpio_no is invalid\n");
		return false;
	}
	gpio_direction_output(di->gpio_no, 0);
	gpio_set_value(di->gpio_no, value);
	return true;
}

static int tps63810_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct device_node *np = NULL;
	struct tps63810_device_info *di = NULL;
	int buckboost_valid;
	int ret;

	if (!client || !client->dev.of_node)
		return -ENODEV;

	di = devm_kzalloc(&client->dev, sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	g_tps63810_di = di;
	di->dev = &client->dev;
	np = di->dev->of_node;
	di->client = client;
	i2c_set_clientdata(client, di);
	di->gpio_no = of_get_named_gpio(np, "en_pin", 0);
	if (!gpio_is_valid(di->gpio_no)) {
		hwlog_err("No GPIO provided, will not HW reset device\n");
		return -EINVAL;
	}
	ret = of_property_read_u32(np, "buckboost_state", &buckboost_valid);
	if (ret) {
		hwlog_err("can't get buckboost_state config\n");
		return -EINVAL;
	}
	if (buckboost_valid == BUCKBOOST_MODE) {
		hwlog_info("buckboost mode\n");
		tps63810_set_enable(1, g_tps63810_di);
	}
	if (buckboost_valid == NO_BUCKBOOST_MODE) {
		hwlog_info("no buckboost mode\n");
		tps63810_set_enable(0, g_tps63810_di);
	}
	return 0;
}

static int tps63810_remove(struct i2c_client *client)
{
	struct tps63810_device_info *di = i2c_get_clientdata(client);

        if (!di)
                return -ENODEV;

        if (gpio_is_valid(di->gpio_no))
                gpio_free(di->gpio_no);

        i2c_set_clientdata(client, NULL);
        g_tps63810_di = NULL;
        return 0;
}

static const struct of_device_id tps63810_of_match[] = {
	{
		.compatible = "qcom, radio_tps63810",
		.data = NULL,
	},
	{},
};

static const struct i2c_device_id tps63810_i2c_id[] = {
	{ "radio_tps63810", 0 }, {}
};

MODULE_DEVICE_TABLE(i2c, tps63810);
static struct i2c_driver tps63810_driver = {
	.probe = tps63810_probe,
	.remove = tps63810_remove,
	.id_table = tps63810_i2c_id,
	.driver = {
		.owner = THIS_MODULE,
		.name = "radio_tps63810",
		.of_match_table = of_match_ptr(tps63810_of_match),
	},
};

static int __init tps63810_init(void)
{
    return i2c_add_driver(&tps63810_driver);
}

static void __exit tps63810_exit(void)
{
    i2c_del_driver(&tps63810_driver);
}

fs_initcall_sync(tps63810_init);
module_exit(tps63810_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("tps63810 for radio driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
