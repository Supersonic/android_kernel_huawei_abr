#include "debug_adapter.h"
#include <securec.h>
#include <linux/list.h>
#include <linux/mutex.h>

static struct list_head camera_list;
static struct mutex lock;
struct camera_name {
	char vcm_name[DEV_NAME_MAX_LENGTH];
	char ois_name[DEV_NAME_MAX_LENGTH];
};

struct camera_adapter_t {
	struct list_head node;
	int slot_id;
	int vcm_id;
	int ois_id;
	int eeprom_id;
	struct cam_sensor_ctrl_t *s_ctrl;
	struct cam_actuator_ctrl_t *a_ctrl;
	struct cam_ois_ctrl_t *o_ctrl;
};

/* Configure the functions to be supported. */
static struct function_config g_function_policy[] = {
	{SUPPORT_FUNCTION,    GET_DEV_DATA,         NON_DEV},
	{CAMERA_EXIST,        GET_DEV_DATA,         NON_DEV},
	{SENSOR_I2C_READ,     GET_DEBUG_DATA,       SENSOR},
	{SENSOR_I2C_WRITE,    GET_DEBUG_DATA,       SENSOR},
	{VCM_I2C_READ,        GET_DEBUG_DATA,       VCM},
	{VCM_I2C_WRITE,       GET_DEBUG_DATA,       VCM},
	{OIS_I2C_READ,        GET_DEBUG_DATA,       OIS},
	{OIS_I2C_WRITE,       GET_DEBUG_DATA,       OIS},
};

void register_sensor_node(struct camera_module_adapter_t *dev)
{
	struct camera_adapter_t *camera = NULL;
	camera = (struct camera_adapter_t *)kzalloc(sizeof(struct camera_adapter_t), GFP_KERNEL);
	if (!camera) {
		debug_err("sensor_adapter kzalloc error!");
		return;
	}
	camera->slot_id = dev->slot_id;
	camera->s_ctrl = dev->s_ctrl;
	camera->vcm_id = dev->vcm_id;
	camera->ois_id = dev->ois_id;
	camera->eeprom_id = dev->eeprom_id;
	list_add(&camera->node, &camera_list);
}

int register_vcm_node(int vcm_id, struct cam_actuator_ctrl_t *a_ctrl)
{
	struct camera_adapter_t *camera = NULL;

	mutex_lock(&lock);
	list_for_each_entry(camera, &camera_list, node) {
		if (camera->vcm_id == vcm_id) {
			debug_dbg("enter! slot_id %d", camera->slot_id);
			camera->a_ctrl = a_ctrl;
			mutex_unlock(&lock);
			return camera->slot_id;
		}
	}
	mutex_unlock(&lock);
	return -1;
}

int register_ois_node(int ois_id, struct cam_ois_ctrl_t *o_ctrl)
{
	struct camera_adapter_t *camera = NULL;
	mutex_lock(&lock);
	list_for_each_entry(camera, &camera_list, node) {
		if (camera->ois_id == ois_id) {
			debug_dbg("enter! slot_id %d", camera->slot_id);
			camera->o_ctrl = o_ctrl;
			mutex_unlock(&lock);
			return camera->slot_id;
		}
	}
	mutex_unlock(&lock);
	return -1;
}

struct cam_sensor_ctrl_t* get_sensor_ctrl(int slot_id, int64_t device_type)
{
	struct camera_adapter_t *camera = NULL;
	int camera_slot_id;
	mutex_lock(&lock);
	list_for_each_entry(camera, &camera_list, node) {
		switch (device_type) {
		case CAM_SENSOR:
			camera_slot_id = camera->slot_id;
			break;
		case CAM_ACTUATOR:
			camera_slot_id = camera->vcm_id;
			break;
		case CAM_EEPROM:
			camera_slot_id = camera->eeprom_id;
			break;
		case CAM_OIS:
			camera_slot_id = camera->ois_id;
			break;
		default:
			debug_err("Not support device Type: %d", device_type);
			mutex_unlock(&lock);
			return NULL;
		}
		if (slot_id == camera_slot_id) {
			debug_dbg("get dev info");
			mutex_unlock(&lock);
			return camera->s_ctrl;
		}
	}
	mutex_unlock(&lock);
	return NULL;
}

struct cam_actuator_ctrl_t* get_vcm_ctrl(int slot_id)
{
	struct camera_adapter_t *camera = NULL;
	mutex_lock(&lock);
	list_for_each_entry(camera, &camera_list, node) {
		if (camera->slot_id == slot_id) {
			mutex_unlock(&lock);
			debug_dbg("get dev info");
			return camera->a_ctrl;
		}
	}
	mutex_unlock(&lock);
	return NULL;
}

struct cam_ois_ctrl_t* get_ois_ctrl(int slot_id)
{
	struct camera_adapter_t *camera = NULL;
	mutex_lock(&lock);
	list_for_each_entry(camera, &camera_list, node) {
		if (camera->slot_id == slot_id) {
			debug_dbg("get dev info");
			mutex_unlock(&lock);
			return camera->o_ctrl;
		}
	}
	mutex_unlock(&lock);
	return NULL;
}

static enum camera_sensor_i2c_type get_bit(unsigned int bit)
{
	switch (bit) {
	case 8:   // 8 bit
		return CAMERA_SENSOR_I2C_TYPE_BYTE;
	case 16:  // 16 bit
		return CAMERA_SENSOR_I2C_TYPE_WORD;
	case 24:  // 24 bit
		return CAMERA_SENSOR_I2C_TYPE_3B;
	case 32:  // 32 bit
		return CAMERA_SENSOR_I2C_TYPE_DWORD;
	default:
		return CAMERA_SENSOR_I2C_TYPE_INVALID;
	}
}

int i2c_read(struct debug_msg *recv_data, struct debug_msg *send_data,
			 struct camera_io_master *io_master_info)
{
	struct i2c_data *debug_data = NULL;
	struct i2c_data *read_data = NULL;
	int i;
	enum camera_sensor_i2c_type reg_bit;
	enum camera_sensor_i2c_type val_bit;
	int rc = 0;
	if (!recv_data || !send_data || !io_master_info) {
		debug_err("data or io_master_info is null.");
		return -1;
	}
	debug_data = &recv_data->u.i2c_debug_data;
	if (!debug_data) {
		debug_err("i2c_data is null.");
		return -1;
	}
	read_data = &send_data->u.i2c_debug_data;

	reg_bit = get_bit(debug_data->reg_bit);
	if (reg_bit == CAMERA_SENSOR_I2C_TYPE_INVALID)
		reg_bit = CAMERA_SENSOR_I2C_TYPE_WORD;
	val_bit = get_bit(debug_data->val_bit);
	if (val_bit == CAMERA_SENSOR_I2C_TYPE_INVALID)
		val_bit = CAMERA_SENSOR_I2C_TYPE_BYTE;
	debug_dbg("reg_bit %d, val_bit %d", reg_bit, val_bit);

	for (i = 0; i < debug_data->num; i++) {
		rc = camera_io_dev_read(
			io_master_info,
			debug_data->data[i].reg, &read_data->data[i].val, reg_bit, val_bit);
		if (rc < 0) {
			return -1;
		}
		debug_dbg("read iic reg 0x%x, val 0x%x", debug_data->data[i].reg, read_data->data[i].val);
		read_data->data[i].reg = debug_data->data[i].reg;
		mdelay(debug_data->data[i].delay_ms);
	}
	read_data->num = debug_data->num;
	return 0;
}

int i2c_write(struct debug_msg *recv_data,
			  struct camera_io_master *io_master_info)
{
	struct i2c_data *debug_data = NULL;
	struct cam_sensor_i2c_reg_setting write_setting;
	struct cam_sensor_i2c_reg_array i2c_array[MAX_I2C_SIZE] = {{0}};
	int i;
	enum camera_sensor_i2c_type reg_bit;
	enum camera_sensor_i2c_type val_bit;
	int rc = 0;

	if (!recv_data || !io_master_info) {
		debug_err("data or io_master_info is null.");
		return -1;
	}
	debug_data = &recv_data->u.i2c_debug_data;
	if (!debug_data) {
		debug_err("i2c_data is null.");
		return -1;
	}

	reg_bit = get_bit(debug_data->reg_bit);
	if (reg_bit == CAMERA_SENSOR_I2C_TYPE_INVALID)
		reg_bit = CAMERA_SENSOR_I2C_TYPE_WORD;
	val_bit = get_bit(debug_data->val_bit);
	if (val_bit == CAMERA_SENSOR_I2C_TYPE_INVALID)
		val_bit = CAMERA_SENSOR_I2C_TYPE_BYTE;
	debug_dbg("reg_bit %d, val_bit %d", reg_bit, val_bit);
	write_setting.size = debug_data->num;
	write_setting.addr_type = reg_bit;
	write_setting.data_type = val_bit;
	write_setting.delay = 0;
	write_setting.read_buff = NULL;
	write_setting.read_buff_len = 0;

	for (i = 0; i < debug_data->num; i++) {
		i2c_array[i].reg_addr = debug_data->data[i].reg;
		i2c_array[i].reg_data = debug_data->data[i].val;
		i2c_array[i].delay = debug_data->data[i].delay_ms;
		i2c_array[i].data_mask = 0;
		debug_dbg("write iic reg 0x%x, val 0x%x", debug_data->data[i].reg, debug_data->data[i].val);
	}
	write_setting.reg_setting = (struct cam_sensor_i2c_reg_array *)&i2c_array[0];
	rc = camera_io_dev_write(io_master_info, &write_setting);
	if (rc < 0) {
		debug_err("failed to read i2c");
		return -1;
	}
	return 0;
}

int __init debug_adapter_init(void)
{
	updata_func_config(g_function_policy, ARRAY_SIZE(g_function_policy));
	INIT_LIST_HEAD(&camera_list);
	mutex_init(&lock);
	return 0;
}

void __exit debug_adapter_exit(void)
{
	struct list_head *pos = NULL;
	struct list_head *q = NULL;
	struct camera_adapter_t *camera = NULL;
	mutex_destroy(&lock);
	list_for_each_safe(pos, q, &camera_list) {
		camera = list_entry(pos, struct camera_adapter_t, node);
		list_del(pos);
		kfree(camera);
	}
}