#ifndef _DEBUG_COMMON_H_
#define _DEBUG_COMMON_H_
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/mutex.h>
#define MAX_DEBUG_SIZE 40
#define MAX_MSG_LENGTH 400
#define DEV_NAME_MAX_LENGTH 20
#define MAX_DEV_COUNT       20
#define MAX_I2C_SIZE        30
#define DEBUG_ERR (1)
#define DEBUG_OK (0)

enum debug_state {
	CAMERA_DEBUG_OK = 0,
	CAMERA_DEBUG_ERR,
};

enum dev_type {
	NON_DEV = -1,
	SENSOR = 0,
	VCM,
	OIS,
	FLASH,
	LASER,
	EEPROM,
};

enum camera_state {
	SENSOR_DEV = -1,
	SENSOR_IS_EXIST = 0,
	SENSOR_POWER_ON,
	SENSOR_STREAM_ON,
	SENSOR_STREAM_OFF,
	SENSOR_POWER_DOWN,
};

enum debug_function {
	SUPPORT_FUNCTION = 0,
	CAMERA_EXIST,
	SENSOR_STATE,

	SENSOR_I2C_READ,
	SENSOR_I2C_WRITE,

	VCM_I2C_READ,
	VCM_I2C_WRITE,

	OIS_I2C_READ,
	OIS_I2C_WRITE,

	NCS_I2C_READ,
	NCS_I2C_WRITE,
	NSC_BASE_DEV_DEBUG,
	FUNCTION_MAX,
};

enum function_policy {
	NOT_SUPPORT,
	GET_HAL_DATA,
	GET_DEBUG_DATA,
	GET_DEV_DATA,
};

struct function_config {
	enum debug_function func;
	enum function_policy policy;
	enum dev_type type;
};

struct dev_info_t {
	int dev_id;
	int type;
	int state;
	char dev_name[DEV_NAME_MAX_LENGTH];
};

struct i2c_array {
	unsigned int reg;
	unsigned int val;
	unsigned int delay_ms;
};

struct i2c_data {
	unsigned int reg_bit;
	unsigned int val_bit;
	unsigned int num;
	struct i2c_array data[MAX_I2C_SIZE];
};

struct debug_msg {
	unsigned int msg_size;
	unsigned int msg_id;
	unsigned int dev_id;
	unsigned int command;
	unsigned int state;
	union {
		char adapter_msg[MAX_MSG_LENGTH];
		struct i2c_data i2c_debug_data;
		struct dev_info_t dev_info[MAX_DEV_COUNT];
		unsigned int func[FUNCTION_MAX];
	}u;
};

struct dev_msg_t {
	int dev_id;
	int type;
	char dev_name[DEV_NAME_MAX_LENGTH];
	void (*handle)(struct debug_msg *recv_data, struct debug_msg *send_data);
};

void updata_func_config(struct function_config func_config[], int size);
void updata_dev_msg(struct dev_msg_t *dev);
void updata_state_by_position(int position_id, int type, int state);
#endif