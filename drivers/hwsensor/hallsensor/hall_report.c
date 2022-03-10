

#include <linux/input.h>
#include <linux/device.h>
#include <linux/of.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <misc/app_info.h>
#include "../sensors_class.h"
#include "hall_report.h"
#include <huawei_platform/ap_hall/ext_hall_event.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "ap_hall"

#define log_info(fmt, args...)  pr_info(LOG_TAG " %s: " fmt, __func__, ##args)
#define log_debug(fmt, args...) pr_debug(LOG_TAG " %s: " fmt, __func__, ##args)
#define log_err(fmt, args...)   pr_err(LOG_TAG " %s: " fmt, __func__, ##args)
static unsigned int support_coil_switch;
static unsigned int is_support_hall_keyboard; // default no supply
static unsigned int kb_hall_value = 0x04;     // default: 0000 0100b

static unsigned int is_support_hall_pen;      // default no supply
static unsigned int pen_hall_value = 0x10;    // default: 0001 0000b
static unsigned int is_support_hall_lightstrap; // default no supply
static unsigned int hall_lightstrap_value = 0x04;     // default: 0000 0100b
static unsigned int is_support_ext_hall_notify;

static unsigned int ext_bit2_comp_val = 2; // default bit2 compare val
static unsigned int ext_bit1_comp_val = 1; // default bit2 compare val

static struct input_dev *hall_input = NULL;

/* define ext_hall data[0] every bit func */
#define BIT0_FOR_SLIDE_TYPE       0x01
#define BIT1_FOR_FOLD_TYPE        0x02
#define BIT2_FOR_NFC_THEME_SHELL  0x04

/* define ext_hall data every bit compare val */
#define BIT_COMPARE_FOR_EVERY_BIT       0x01
static int ext_hall_status[MAX_EXT_HALL_TYPE] = {0};

static struct sensors_classdev hall_cdev = {
	.name = "ak8789-hall",
	.vendor = "AKMMicroelectronics",
	.version = 1,
	.handle = SENSORS_HALL_HANDLE,
	.type = SENSOR_TYPE_HALL,
	.max_range = "3",
	.resolution = "1",
	.sensor_power = "0.002",
	.min_delay = 0,
	.delay_msec = 200,
	.fifo_reserved_event_count = 0,
	.fifo_max_event_count = 0,
	.enabled = 0,
	.sensors_enable = NULL,
	.sensors_poll_delay = NULL,
};

static void init_ext_hall_notify_config()
{
	unsigned int i;

	support_coil_switch = 0;
	is_support_hall_pen = 0;
	is_support_hall_keyboard = 0;
	is_support_hall_lightstrap = 0;
	is_support_ext_hall_notify = 0;
	for (i = 0; i < MAX_EXT_HALL_TYPE; i++)
		ext_hall_status[i] = -1;
}

void get_ext_hall_bit_config(struct device_node *node)
{
	if (of_property_read_u32(node, "ext_bit2_comp_val", &ext_bit2_comp_val))
		log_info("read ext_bit2_comp_val fail, use default\n");
	else
		log_info("read ext_bit2_comp_val = %d\n", ext_bit2_comp_val);

	if (of_property_read_u32(node, "ext_bit1_comp_val", &ext_bit1_comp_val))
		log_info("read ext_bit1_comp_val fail, use default\n");
	else
		log_info("read ext_bit1_comp_val = %d\n", ext_bit1_comp_val);
}

void get_hall_fature_config(struct device_node *node)
{
	unsigned int tmp;

	log_info("enter\n");
	if (!node)
		return;
	init_ext_hall_notify_config();
	get_ext_hall_bit_config(node);
	if (!of_property_read_u32(node, "support_coil_switch", &tmp)) {
		support_coil_switch = tmp;
		log_info("support_coil_switch = %d\n", support_coil_switch);
	}
	if (!of_property_read_u32(node, "is_support_hall_pen", &tmp)) {
		is_support_hall_pen = tmp;
		log_info("is_support_hall_pen = %d\n", is_support_hall_pen);
	}

	if (!of_property_read_u32(node, "hall_pen_value", &tmp)) {
		pen_hall_value = tmp;
		log_info("pen_hall_value = %d\n", pen_hall_value);
	}

	if (!of_property_read_u32(node, "is_support_hall_keyboard", &tmp)) {
		is_support_hall_keyboard = tmp;
		log_info("is_support_hall_keyboard = %d\n", is_support_hall_keyboard);
	}

	if (!of_property_read_u32(node, "hall_keyboard_value", &tmp)) {
		kb_hall_value = tmp;
		log_info("kb_hall_value = %d\n", kb_hall_value);
	}

	if (!of_property_read_u32(node, "is_support_hall_lightstrap", &tmp)) {
		is_support_hall_lightstrap = tmp;
		log_info("is_support_hall_lightstrap = %d\n", is_support_hall_lightstrap);
	}

	if (!of_property_read_u32(node, "hall_lightstrap_value", &tmp)) {
		hall_lightstrap_value = tmp;
		log_info("hall_lightstrap_value = %d\n", hall_lightstrap_value);
	}

	if (!of_property_read_u32(node, "is_support_ext_hall_notify", &tmp)) {
		is_support_ext_hall_notify = tmp;
		log_info("is_support_ext_hall_notify = %d\n",
			is_support_ext_hall_notify);
	}

}

int hall_report_register(struct device *dev)
{
	int ret;
	struct input_dev *input_ptr = NULL;

	if ((dev == NULL) || (dev->of_node == NULL)) {
		log_err("dev or of_node is NULL\n");
		return -EINVAL;
	}

	get_hall_fature_config(dev->of_node);

	input_ptr = input_allocate_device();
	if (IS_ERR_OR_NULL(input_ptr)) {
		ret = PTR_ERR(input_ptr);
		log_err("input alloc fail, ret = %ld\n", ret);
		return ret;
	}

	input_ptr->name = "hall";
	set_bit(EV_MSC, input_ptr->evbit);
	set_bit(MSC_SCAN, input_ptr->mscbit);
	ret = input_register_device(input_ptr);
	if (ret) {
		log_err("input register fail, ret = %d\n", ret);
		goto free_input_dev;
	}

	ret = sensors_classdev_register(&input_ptr->dev, &hall_cdev);
	if (ret) {
		log_err("sensor class register fail\n");
		goto unregister_input;
	}

	ret = app_info_set("Hall", "AKM8789");
	if (ret) {
		log_err("app info set fail\n");
		goto unregister_classdev;
	}

	hall_input = input_ptr;
	return ret;

unregister_classdev:
	sensors_classdev_unregister(&hall_cdev);

unregister_input:
	input_unregister_device(input_ptr);

free_input_dev:
	input_free_device(input_ptr);
	hall_input = NULL;

	return ret;
}

void hall_report_unregister()
{
	sensors_classdev_unregister(&hall_cdev);
	if (hall_input != NULL) {
		input_unregister_device(hall_input);
		input_free_device(hall_input);
		hall_input = NULL;
	}
}

static void check_hall_pen_state(unsigned int value)
{
	static unsigned int state = 0;
	unsigned int temp;

	temp = value & pen_hall_value;
	if (temp != state) {
		if (temp) {
			if (support_coil_switch) {
				power_event_bnc_notify(POWER_BNT_WLTX_AUX, POWER_NE_WLTX_AUX_PEN_HALL_APPROACH, NULL);
				log_info("open wireless charging\n");
			} else {
				power_event_bnc_notify(POWER_BNT_WLTX_AUX, POWER_NE_WLTX_HALL_APPROACH, NULL);
				log_info("close wireless charging\n");
			}
		} else {
			if (support_coil_switch) {
				power_event_bnc_notify(POWER_BNT_WLTX_AUX, POWER_NE_WLTX_AUX_PEN_HALL_AWAY_FROM, NULL);
				log_info("open wireless charging\n");
			} else {
				power_event_bnc_notify(POWER_BNT_WLTX_AUX, POWER_NE_WLTX_HALL_AWAY_FROM, NULL);
				log_info("close wireless charging\n");
			}
		}
		state = temp;
	}
}

#ifdef CONFIG_WIRELESS_LIGHTSTRAP
static uint32_t pre_lightstrap_state;
static void check_hall_lightstrap_state(uint32_t value)
{
	uint32_t light_state;

	light_state = value & hall_lightstrap_value;
	if (pre_lightstrap_state != light_state) {
		if (light_state) {
			log_info("approach, open wireless tx\n", __func__);
			power_event_bnc_notify(POWER_BNT_LIGHTSTRAP,
				POWER_NE_LIGHTSTRAP_ON, NULL);
		} else {
			log_info("away, close wireless tx\n", __func__);
			power_event_bnc_notify(POWER_BNT_LIGHTSTRAP,
				POWER_NE_LIGHTSTRAP_OFF, NULL);
		}
		pre_lightstrap_state = light_state;
	}
}
#endif

uint32_t get_hall_lightstrap_value(void)
{
	return hall_lightstrap_value;
}

static void check_hall_keyboard_state(unsigned int value)
{
	static unsigned int state = 0;
	unsigned int temp;

	temp = value & kb_hall_value;
	if (temp != state) {
		if (temp) {
			if (support_coil_switch) {
				power_event_bnc_notify(POWER_BNT_WLTX_AUX, POWER_NE_WLTX_AUX_KB_HALL_APPROACH, NULL);
				log_info("open wireless charging\n");
			} else {
				power_event_bnc_notify(POWER_BNT_WLTX, POWER_NE_WLTX_HALL_APPROACH, NULL);
				log_info("close wireless charging\n");
			}
		} else {
			if (support_coil_switch) {
				power_event_bnc_notify(POWER_BNT_WLTX_AUX, POWER_NE_WLTX_AUX_KB_HALL_AWAY_FROM, NULL);
				log_info("open wireless charging\n");
			} else {
				power_event_bnc_notify(POWER_BNT_WLTX, POWER_NE_WLTX_HALL_AWAY_FROM, NULL);
				log_info("close wireless charging\n");
			}
		}
		state = temp;
	}
}

int hall_report_value(int value)
{
	struct custom_sensor_data_t hall_data;

#ifdef INPUT_HALL_DATA_REPORT
	if (hall_input == NULL) {
		log_err("ptr is NULL\n");
		return -EINVAL;
	}

	input_event(hall_input, EV_MSC, MSC_SCAN, value);
	input_sync(hall_input);
#else
	hall_data.sensor_data_head.sensor_type = SENSOR_TYPE_AP_HALL;
	hall_data.sensor_data_head.action = STD_SENSOR_DATA_REPORT;
	memset(&hall_data.sensor_data[0], 0, 16 * sizeof(int));
	hall_data.sensor_data[0] = value;
	log_err("report hall state = %d\n", hall_data.sensor_data[0]);
	return ap_sensor_route_write((char*)&hall_data,
		sizeof(struct custom_sensor_data_t));
#endif

	log_info("value = %d\n", value);
	return 0;
}

static int ext_hall_data_compare(int hall_val, unsigned int comp_val)
{
	int bit_tmp;

	bit_tmp = (int)(((unsigned int)hall_val >> comp_val) &
		BIT_COMPARE_FOR_EVERY_BIT);
	return bit_tmp;
}

int get_ext_hall_status(unsigned int ext_hall_type)
{
	if (ext_hall_type >= MAX_EXT_HALL_TYPE)
		return -1;

	return ext_hall_status[ext_hall_type];
}

static void check_ext_hall_notify_state(int ext_hall_type,
	unsigned int value)
{
	if (((unsigned int)ext_hall_type & BIT1_FOR_FOLD_TYPE) > 0) {
		ext_hall_status[EXT_HALL_FOLD_TYPE] =
			!ext_hall_data_compare(value, ext_bit1_comp_val);
		ext_fold_status_distinguish(ext_hall_status[EXT_HALL_FOLD_TYPE]);
	}

	if (((unsigned int)ext_hall_type & BIT2_FOR_NFC_THEME_SHELL) > 0)
		ext_hall_status[EXT_HALL_NFC_TYPE] =
			ext_hall_data_compare(value, ext_bit2_comp_val);
}

void ext_hall_notify_event(int ext_hall_type, int value)
{
	if (is_support_hall_pen)
		check_hall_pen_state((unsigned int)value);

	if (is_support_hall_keyboard)
		check_hall_keyboard_state((unsigned int)value);

	if (is_support_ext_hall_notify)
		check_ext_hall_notify_state(ext_hall_type, (unsigned int)value);

#ifdef CONFIG_WIRELESS_LIGHTSTRAP
	if (is_support_hall_lightstrap)
		check_hall_lightstrap_state((unsigned int)value);
#endif
}

void ext_hall_report_value(int ext_hall_type, int value)
{
	struct custom_sensor_data_t hall_data;

	if (ext_hall_type <= 0)
		return;

	hall_data.sensor_data_head.sensor_type = SENSOR_TYPE_EXT_HALL;
	hall_data.sensor_data_head.action = STD_SENSOR_DATA_REPORT;
	memset(&hall_data.sensor_data[0], 0, 16 * sizeof(int));
	hall_data.sensor_data[0] = ext_hall_type;
	if (((unsigned int)ext_hall_type & BIT1_FOR_FOLD_TYPE) > 0) {
		hall_data.sensor_data[1] =
			!ext_hall_data_compare(value, ext_bit1_comp_val);
		log_info("report ext hall bit1 state = %d\n",
			hall_data.sensor_data[1]);
	}
	if (((unsigned int)ext_hall_type & BIT2_FOR_NFC_THEME_SHELL) > 0) {
		hall_data.sensor_data[2] =
			ext_hall_data_compare(value, ext_bit2_comp_val);
		log_info("report ext hall bit2 state = %d\n",
			hall_data.sensor_data[2]);
	}

	ap_sensor_route_write((char*)&hall_data,
		sizeof(struct custom_sensor_data_t));
}

