/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * Description: laser driver
 *
 * Author: lvyali
 *
 * Create: 2021-05-06
 */

#include <sensors_class.h>
#include "cam_laser_dev.h"
#include "cam_laser_soc.h"
#include "cam_debug_util.h"
#include "tof8801_driver.h"
#include "stmvl53l1.h"
#include "vi5300.h"

static laser_module_intf_t laser_devices[] = {
	{ "tof8805", &tof_probe, &tof_remove },
	{ "stmvl53l1", &stmvl53l1_probe, &stmvl53l1_remove },
	{ "vi5300", &vi5300_probe, &vi5300_remove },
};

static int laser_index = -1;

static int laser_data_remove(struct i2c_client *client)
{
	int rc = 0;

	if (!client) {
		CAM_ERR(CAM_LASER, "parameter error");
		return -EINVAL;
	}

	if (laser_index == -1)
		return -1;
	rc = laser_devices[laser_index].data_remove(client);

	return rc;
}

static int cam_laser_i2c_driver_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int rc = 0;
	struct cam_laser_ctrl_t *o_ctrl = NULL;
	struct cam_laser_soc_private *soc_private = NULL;
	struct regulator *vreg = NULL;
	struct device *dev = NULL;
	struct pinctrl *pctrl = NULL;
	struct pinctrl_state *pinctrl_state_laser_int = NULL;
	struct pinctrl_state *pinctrl_state_laser_en = NULL;

	if (client == NULL || id == NULL) {
		CAM_ERR(CAM_LASER, "Invalid Args client: %pK id: %pK", client, id);
		return -EINVAL;
	}

	CAM_INFO(CAM_LASER, "I2C Address: %#04x\n", client->addr);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		CAM_ERR(CAM_LASER, "I2C check functionality failed");
		return -ENXIO;
	}

	o_ctrl = kzalloc(sizeof(*o_ctrl), GFP_KERNEL);
	if (!o_ctrl) {
		CAM_ERR(CAM_LASER, "kzalloc failed");
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, o_ctrl);

	o_ctrl->soc_info.dev = &client->dev;
	o_ctrl->soc_info.dev_name = client->name;
	o_ctrl->laser_device_type = MSM_CAMERA_I2C_DEVICE;
	o_ctrl->io_master_info.master_type = I2C_MASTER;
	o_ctrl->io_master_info.client = client;

	soc_private = kzalloc(sizeof(struct cam_laser_soc_private),
		GFP_KERNEL);
	if (!soc_private) {
		rc = -ENOMEM;
		goto octrl_free;
	}

	o_ctrl->soc_info.soc_private = soc_private;
	/* vdd regulator get*/
	vreg = devm_regulator_get(o_ctrl->soc_info.dev, "vdd_ana");
	if (IS_ERR(vreg)) {
		CAM_ERR(CAM_LASER, "devm_regulator_get fail");
		goto soc_free;
	}
	rc = regulator_set_load(vreg, DEVICE_LOAD_CURRENT);
	if (rc) {
		CAM_ERR(CAM_LASER, "regulator_set_load fail");
		goto soc_free;
	}
	rc = regulator_enable(vreg);
	if (rc) {
		CAM_ERR(CAM_LASER, "regulator_enable fail");
		goto soc_free;
	}

	if (&client->dev == NULL) {
		CAM_ERR(CAM_LASER, "&client->dev == NULL");
		goto soc_free;
	}
	dev = &client->dev;
	pctrl = devm_pinctrl_get(dev);
	if (IS_ERR_OR_NULL(pctrl)) {
		rc = -EINVAL;
		CAM_ERR(CAM_LASER, "pctrl == NULL \n");
		goto soc_free;
	}

	pinctrl_state_laser_int = pinctrl_lookup_state(pctrl, "laser_int_active");
	if (IS_ERR_OR_NULL(pinctrl_state_laser_int)) {
		rc = -EINVAL;
		CAM_ERR(CAM_LASER, "pinctrl_lookup_state INT == NULL \n");
		goto err_pinctrl_select_state;
	}
	rc = pinctrl_select_state(pctrl, pinctrl_state_laser_int);
	if(rc) {
		rc = -EINVAL;
		CAM_ERR(CAM_LASER, "pinctrl_select_state INT == NULL \n");
		goto err_pinctrl_select_state;
	}

	pinctrl_state_laser_en = pinctrl_lookup_state(pctrl, "laser_en_active");
	if (IS_ERR_OR_NULL(pinctrl_state_laser_en)) {
		rc = -EINVAL;
		CAM_ERR(CAM_LASER, "pinctrl_lookup_state EN == NULL \n");
		goto err_pinctrl_select_state;
	}
	rc = pinctrl_select_state(pctrl, pinctrl_state_laser_en);
	if(rc) {
		rc = -EINVAL;
		CAM_ERR(CAM_LASER, "INT pinctrl_select_state EN == NULL \n");
		goto err_pinctrl_select_state;
	}

	rc = cam_laser_driver_soc_init(o_ctrl);
	if (rc) {
		CAM_ERR(CAM_LASER, "failed: cam_sensor_parse_dt");
		goto regulator_err;
	}

	laser_index = o_ctrl->soc_info.index;
	if (laser_index == -1)
		return -1;

	rc = laser_devices[laser_index].data_init(client, id);
	if (rc) {
		CAM_ERR(CAM_LASER, "cell index %d probe fail", o_ctrl->soc_info.index);
		goto regulator_err;
	}

	CAM_INFO(CAM_LASER, "enter laser_probe");
	rc = laser_probe(client, id);
	if (rc) {
		goto regulator_err;
	}

	o_ctrl->cam_laser_state = CAM_LASER_INIT;
	o_ctrl->open_cnt = 0;

	return rc;

regulator_err:
	regulator_disable(vreg);
err_pinctrl_select_state:
	pr_err("err_pinctrl_select_state fail\n");
	devm_pinctrl_put(pctrl);
soc_free:
	kfree(soc_private);
octrl_free:
	kfree(o_ctrl);
probe_failure:
	return rc;
}

static int cam_laser_i2c_driver_remove(struct i2c_client *client)
{
	struct cam_laser_ctrl_t          *o_ctrl = i2c_get_clientdata(client);
	int rc = 0;

	if (!o_ctrl) {
		CAM_ERR(CAM_LASER, "laser device is NULL");
		return -EINVAL;
	}

	CAM_INFO(CAM_LASER, "i2c driver remove invoked");

	rc = laser_data_remove(client);
	if (rc == 0) {
		CAM_INFO(CAM_LASER, "laser_data_remove success");
		rc = laser_remove(client);
	}

	kfree(o_ctrl->soc_info.soc_private);
	kfree(o_ctrl);

	return 0;
}

static const struct i2c_device_id cam_laser_i2c_id[] = {
	{ "laser", (kernel_ulong_t)NULL},
	{ }
};
MODULE_DEVICE_TABLE(i2c, cam_laser_i2c_id);

static const struct of_device_id laser_of_match[] = {
  { .compatible = "huawei,laser" },
  { }
};
MODULE_DEVICE_TABLE(of, laser_of_match);

static struct i2c_driver cam_laser_i2c_driver = {
	.driver = {
		.name = "camera-laser",
		.of_match_table = of_match_ptr(laser_of_match),
	},
	.id_table = cam_laser_i2c_id,
	.probe  = cam_laser_i2c_driver_probe,
	.remove = cam_laser_i2c_driver_remove,
};

int cam_laser_driver_init(void)
{
	int rc = 0;

	rc = i2c_add_driver(&cam_laser_i2c_driver);
	if (rc) {
		CAM_ERR(CAM_LASER, "i2c_add_driver failed rc = %d", rc);
		return rc;
	}

	return rc;
}

void cam_laser_driver_exit(void)
{
	i2c_del_driver(&cam_laser_i2c_driver);
}
