/*
 * zinitix_core.h
 *
 * header of zinitix driver core
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

#ifndef ZINITIX_CORE_H
#define ZINITIX_CORE_H

#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/firmware.h>
#include <linux/debugfs.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/dma-mapping.h>
#include <huawei_platform/log/hw_log.h>
#include <linux/ctype.h>

#include "huawei_ts_kit.h"
#include "huawei_ts_kit_api.h"

#define TX_NUM_MAX 8
#define RX_NUM_MAX 8

#define CAP_TEST_FAIL_CHAR 'F'
#define CAP_TEST_PASS_CHAR 'P'

#define CSV_CAP_RAW_DATA_MIN_ARRAY "threshold,cap_raw_data_min_array"
#define CSV_CAP_RAW_DATA_MAX_ARRAY "threshold,cap_raw_data_max_array"
#define CSV_CAP_NOISE_DATA_MIN_ARRAY "threshold,cap_noise_data_min_array"
#define CSV_CAP_NOISE_DATA_MAX_ARRAY "threshold,cap_noise_data_max_array"

#define CSV_CAP_RX_DATA_MIN_ARRAY "threshold,cap_rx_min_array"
#define CSV_CAP_RX_DATA_MAX_ARRAY "threshold,cap_rx_max_array"
#define CSV_CAP_TX_DATA_MIN_ARRAY "threshold,cap_tx_min_array"
#define CSV_CAP_TX_DATA_MAX_ARRAY "threshold,cap_tx_max_array"

#define CSV_OPEN_SHORT_MIN_ARRAY "threshold,open_short_min_array"
#define CSV_OPEN_SHORT_MAX_ARRAY "threshold,open_short_max_array"

#define CSV_CAP_SELF_DATA_MIN_ARRAY "threshold,cap_self_data_min_array"
#define CSV_CAP_SELF_DATA_MAX_ARRAY "threshold,cap_self_data_max_array"


struct zinitix_tcm_test_threshold {
	/* cap_raw_data saved raw date threshold */
	int32_t cap_raw_data_min_limits[TX_NUM_MAX * RX_NUM_MAX];
	int32_t cap_raw_data_max_limits[TX_NUM_MAX * RX_NUM_MAX];
	int32_t cap_noise_min_limits[TX_NUM_MAX*RX_NUM_MAX];
	int32_t cap_noise_max_limits[TX_NUM_MAX*RX_NUM_MAX];

	int32_t cap_rx_min_limits[TX_NUM_MAX * RX_NUM_MAX];
	int32_t cap_rx_max_limits[TX_NUM_MAX * RX_NUM_MAX];
	int32_t cap_tx_min_limits[TX_NUM_MAX * RX_NUM_MAX];
	int32_t cap_tx_max_limits[TX_NUM_MAX * RX_NUM_MAX];

	int32_t open_short_min_limits[TX_NUM_MAX*RX_NUM_MAX];
	int32_t open_short_max_limits[TX_NUM_MAX*RX_NUM_MAX];

	int32_t cap_self_data_min_limits[TX_NUM_MAX * RX_NUM_MAX];
	int32_t cap_self_data_max_limits[TX_NUM_MAX * RX_NUM_MAX];
};

#define RAW_DATA_BUFF 49
struct zinitix_test_params {
	struct zinitix_tcm_test_threshold threshold;
	int lcd_noise_data;
	int in_csv_file;
	int raw_data_buffer[RAW_DATA_BUFF];
	int point_by_point_judge;
	bool cap_thr_is_parsed;
	u32 raw_data_test;
	u32 noise_test;
	u32 open_short_test;
};

#define LCD_PANEL_INFO_MAX_LEN                     128
#define LCD_PANEL_TYPE_DEVICE_NODE_NAME            "huawei,lcd_panel_type"
#define TARGET_HUAWEI_WEARABLES                    1

#define ZINITIX_MAX_TOUCH_POINTS                   5
#define ZINITIX_PROJECT_ID_LEN                     11
#define ZINITIX_PANEL_ID_START_BIT                 6
#define ZINITIX_VENDOR_NAME_LEN                    10
#define ZINITIX_TP_COLOR_LEN                       3
#define TP_COLOR_SIZE                              15
#define ZINITIX_DETECT_I2C_RETRY_TIMES             3
#define ZINITIX_ESD_CHECK_TIMES                    (3)
#define ZINITIX_FW_UPGRADE_RETRY_TIMES             3
#define ZINITIX_FW_FILE_NAME_MAX_LEN               100
#define ZINITIX_FW_MANUAL_UPDATE_FILE_NAME         "ts/touch_screen_firmware.img"
#define ZINITIX_VENDOR_NAME_DEFAULT                "toptouch"
#define ZINITIX_VENDOR_ID_DEFAULT                  (0x5A49)

#define ZINITIX_CMD_REG_ENABLE                     (0x0001)
#define ZINITIX_CMD_REG_DISABLE                    (0x0000)
#define ZINITIX_ABS_PT_OFFSET                      (-1)
#define ZINITIX_READ_REG_LEN                       (1)
#define ZINITIX_GESTURE_EANBLE                     (1)
#define ZINITIX_SLEEP_TIME_100                     (100)
#define ZINITIX_SLEEP_TIME_220                     (220)
#define ZINITIX_SELF_CTRL_POWER                    1
#define ZINITIX_LDO_POWER                          1
#define ZINITIX_GPIO_POWER                         1
#define ZINITIX_VCI_LDO_VALUE_DFT                  (2800000)
#define ZINITIX_VDDIO_LDO_VALUE_DFT                (1800000)
#define ZINITIX_POWER_DOWN                         1
#define RESET_ENABLE_DELAY                         240
#define ZINITIX_RESET_KEEP_LOW_TIME                1500  // us
#define ZINITIX_RESET_KEEP_LOW_TIME_BEFORE_POWERON 500  // us
#define ZINITIX_EDGE_TOUCH_DATA_LEN                50
#define ZINITIX_GPIO_HIGH                          1
#define ZINITIX_GPIO_LOW                           0
#define ZINITIX_GPIO_RESET_HIGH_DELAY              20  // ms
#define ZINITIX_GPIO_RESET_LOW_DELAY               10 // ms
#define ZINITIX_GPIO_RESET_RELEASE_DELAY           100 // ms
#define ZINITIX_GPIO_GET_RETRY_TIMES 10

#define ZINITIX_MAX_SUPPORTED_FINGER_NUM           2 /* max 10 */
#define ZINITIX_CHIP_OFF_DELAY                     70 /* ms */
#define ZINITIX_CHIP_ON_DELAY                      50  // 100 /* ms */
#define ZINITIX_FIRMWARE_ON_DELAY                  150 /* ms */
#define ZINITIX_CHIP_EN_DELAY                      10  // us
#define ZINITIX_NVM_INIT_DELAY                     5  // ms
#define ZINITIX_IC_INIT_MAX_TIMES                  5 /* max 10 */
#define ZINITIX_REG_ADDRESS_SIZE                   2
#define ZINITIX_REG_VALUE_SIZE                     2
#define ZINITIX_GENERAL_DELAY_1MS                  1  // ms
#define ZINITIX_GENERAL_DELAY_10MS                 10  // ms
#define ZINITIX_GENERAL_DELAY_20MS                 20  // ms
#define ZINITIX_GENERAL_DELAY_30MS                 30  // ms
#define ZINITIX_GENERAL_DELAY_50MS                 50  // ms
#define ZINITIX_GENERAL_DELAY_100MS                100  // ms
#define ZINITIX_GENERAL_DELAY_150MS 150
#define ZINITIX_GENERAL_DELAY_400MS 400
#define ZINITIX_GENERAL_DELAY_1000MS               1000  // ms
#define ZINITIX_GENERAL_DELAY_100US                100  // us
#define ZINITIX_GENERAL_DELAY_10US 10
#define ZINITIX_GENERAL_RETRY_TIMES                3
#define ZINITIX_CALIBRAE_RETRY_TIMES               10
#define ZINITIX_WAIT_IRQ_TIMES                     500

#define ZINITIX_INIT_FLASH_MODE 0x0002
#define ZINITIX_UNGRADE_MODE_SIZE 0x0040
#define ZINITIX_NVM_DATA_ADDR 0x0018
#define ZINITIX_FW_CHECK_ADDR 0xb6fc
#define ZINITIX_FW_CHECK_VALUE 0xA5A5A5A5
#define ZINITIX_FW_CHECK_CMD 0xcc01
/* firmware upgrade */
#define ZINITIX_ZTW522_FLASH_SIZE                  (44 * 1024)
#define ZINITIX_ZTW523_FLASH_SIZE                  (48 * 1024)
#define ZINITIX_ZT7554_FLASH_SIZE                  (64 * 1024)
#define ZINITIX_BITS_PER_BYTE                      8
#define ZINITIX_FLASH_PAGE_SIZE                    64
#define ZINITIX_FLASH_SECTOR_SIZE                  8
#define ZINITIX_FW_MAIN_VER_OFFSET                 52
#define ZINITIX_FW_MINOR_VER_OFFSET                56
#define ZINITIX_FW_DATA_VER_OFFSET                 60
#define ZINITIX_FW_CHIP_ID_OFFSET                  64
#define ZINITIX_FW_HW_ID_OFFSET                    0x7528
#define ZINITIX_FW_CHECK_SUM                       0x55aa

/* raw data test */
#define ZINITIX_POINT_MODE                         (0)
#define ZINITIX_DND_MODE                           (6)
#define ZINITIX_CALIBRATE_MODE                     (7)
#define ZINITIX_PDND_MODE                          (11)
#define ZINITIX_SELF_MODE                          17
#define ZINITIX_DELTA_MODE                         3

#define ZINITIX_SELF_N_COUNT                       13
#define ZINITIX_SELF_U_COUNT                       1
#define ZINITIX_SELF_FREQUENCY                     60

#define ZINITIX_PDND_N_COUNT                       31
#define ZINITIX_PDND_U_COUNT                       9
#define ZINITIX_PDND_FREQUENCY                     240
#define ZINITIX_RAWDATA_DELAY_FOR_HOST             100
#define ZINITIX_RAWDATA_BUFFER_SIZE                98
/* short data test */
#define ZINITIX_SHORT_MODE                         14
#define ZINITIX_SHORT_DATA_NUM                     14
#define ZINITIX_SHORT_DATA_SIZE                    28
#define ZINITIX_SHORT_TEST_SKIP                    3
#define ZINITIX_NORMAL_SHORT_VALUE                 1000

#define ZINITIX_TX_NUM_MAX                         8
#define ZINITIX_RX_NUM_MAX                         8
#define ZINITIX_KEY_NUM_MAX                        0
#define ZINITIX_SENSOR_NUM_MAX  \
	(ZINITIX_TX_NUM_MAX * ZINITIX_RX_NUM_MAX + ZINITIX_KEY_NUM_MAX)
#define RAW_POINT_BY_POINT_JUDGE                   1

/* PALM: 1, PALM_REJECT: 0 */
#define ZINITIX_CMD_INT_VALUE			0x081F//0x081F

#define ROI_DATA_LENGTH 102
#define MAX_FRAME_BUFFER 4096

#define CHANNEL_X_NUM 7
#define CHANNEL_Y_NUM 7

#define ZINITIX_SINGLE_CLICK_CMD 0x0089
#define ZINITIX_DOUBLE_CLICK_CMD 0x0091
/* Interrupt & status register (0x00f0)flag bit
 *  -------------------------------------------------
 *  BIT_PT_CNT_CHANGE   0
 *  BIT_DOWN            1
 *  BIT_MOVE            2
 *  BIT_UP              3
 *  BIT_PALM            4
 *  BIT_PALM_REJECT     5
 *  RESERVED_0          6
 *  RESERVED_1          7
 *  BIT_WEIGHT_CHANGE   8
 *  BIT_PT_NO_CHANGE    9
 *  BIT_REJECT          10
 *  BIT_PT_EXIST        11
 *  RESERVED_2          12
 *  BIT_MUST_ZERO       13
 *  BIT_DEBUG           14
 *  BIT_ICON_EVENT      15
 */

#define zinitix_bit_test(val, n) ((val) & (1 << (n)))
#define zinitix_abs_operate(a, b) (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))

enum ZINITIX_FW_UPDATE_ERROR_CODE {
	ZINITIX_REQUEST_FW_ERROR = 0x1,
	ZINITIX_POWER_ON_ERROR,
	ZINITIX_POWER_ON_SEQUENCE_ERROR,
	ZINITIX_IC_VENDOR_COMMAND_ERROR,
	ZINITIX_CMD_INT_CLEAR_ERROR,
	ZINITIX_ENABLE_NVM_INIT_ERROR,
	ZINITIX_HW_CALIBRATION_ERROR,
	ZINITIX_ENABLE_NVM_VPP_ERROR,
	ZINITIX_DISABLE_NVM_VPP_ERROR,
	ZINITIX_ENABLE_NVM_WP_ERROR,
	ZINITIX_DISABLE_NVM_WP_ERROR,
	ZINITIX_WRITE_FLASH_ERROR,
	ZINITIX_INIT_FLASH_ERROR,
	ZINITIX_READ_FLASH_ERROR,
	ZINITIX_VERIFIED_FW_ERROR,
	ZINITIX_WAKEUP_NVM_ERROR,
	ZINITIX_ENTER_FU_MODE_ERROR,
	ZINITIX_ALLOC_MEM_ERROR,
};

/* Interrupt & status register (0x0080)flag bit */
enum ZINITIX_STATUS_REG_BIT {
	ZINITIX_BIT_PT_CNT_CHANGE,
	ZINITIX_BIT_DOWN,
	ZINITIX_BIT_MOVE,
	ZINITIX_BIT_UP,
	ZINITIX_BIT_PALM,
	ZINITIX_BIT_PALM_REJECT,
	ZINITIX_BIT_RESERVED_0,
	ZINITIX_BIT_RESERVED_1,
	ZINITIX_BIT_WEIGHT_CHANGE,
	ZINITIX_BIT_PT_NO_CHANGE,
	ZINITIX_BIT_REJECT,
	ZINITIX_BIT_PT_EXIST,
	ZINITIX_BIT_RESERVED_2,
	ZINITIX_BIT_MUST_ZERO,
	ZINITIX_BIT_DEBUG,
	ZINITIX_BIT_ICON_EVENT,
};

enum ZINITIX_SUB_STATUS_REG_BIT {
	ZINITIX_ZTW522_CHIP_ID = 0xE532,
	ZINITIX_ZT7538_CHIP_ID = 0xE538,
	ZINITIX_ZT7548_CHIP_ID = 0xE548,
	ZINITIX_ZTW523_CHIP_ID = 0xE628,
	ZINITIX_ZT7554_CHIP_ID = 0xE700,
};

enum ZINITIX_CHITP_TYPE {
	ZINITIX_SUB_BIT_PT_EXIST,
	ZINITIX_SUB_BIT_DOWN,
	ZINITIX_SUB_BIT_MOVE,
	ZINITIX_SUB_BIT_UP,
	ZINITIX_SUB_BIT_UPDATE,
	ZINITIX_SUB_BIT_WAIT,
	ZINITIX_SUB_BIT_CNT_FULL,
	ZINITIX_SUB_BIT_WIDTH_CHANGE,
};

/* touch reg map */
enum ZINITIX_REG_ADDR_ENUM {
	ZINITIX_SWRESET_CMD,
	ZINITIX_WAKEUP_CMD,
	ZINITIX_WAKEUP_NVM_CMD,
	ZINITIX_CLEAR_INT_CMD = 0x0003,
	ZINITIX_IDLE_CMD,
	ZINITIX_SLEEP_CMD,
	ZINITIX_CALIBRATE_CMD,
	ZINITIX_SAVE_CAL_STATUS_CMD,
	ZINITIX_SAVE_CALIBRATION_CMD,
	ZINITIX_POWER_CTL_BEGIN = 0x000a,
	ZINITIX_POWER_CTL_END,
	ZINITIX_RECALL_FACTORY_CMD = 0x000f,
	ZINITIX_TOUCH_MODE_REG = 0x0010,
	ZINITIX_CHIP_REVISION_REG,
	ZINITIX_FW_VER_REG,
	ZINITIX_DATA_VER_REG,
	ZINITIX_HW_ID_REG,
	ZINITIX_SUPPORTED_FINGER_NUM_REG,
	ZINITIX_MAX_Y_NUM_REG,
	ZINITIX_INTERNAL_FLAG_REG,
	ZINITIX_EEPROM_INFO_REG,
	ZINITIX_INITIAL_TOUCH_MODE_REG,
	ZINITIX_CHIP_HW_VER_REG,
	ZINITIX_CAL_N_DATA_NUM_REG,
	ZINITIX_IC_VENDOR_ID,
	ZINITIX_THRESHOLD = 0x0020,
	ZINITIX_TOTAL_NUMBER_OF_X = 0x0060,
	ZINITIX_TOTAL_NUMBER_OF_Y = 0x0061,
	ZINITIX_DELAY_RAW_FOR_HOST = 0x007f,
	ZINITIX_POINT_STATUS_REG = 0x0080,
	ZINITIX_ICON_STATUS_REG = 0x00aa,
	ZINITIX_BUTTON_SUPPORTED_NUM = 0x00b0,
	ZINITIX_BUTTON_SENSITIVITY = 0x00b2,
	ZINITIX_DUMMY_BUTTON_SENSITIVITY = 0X00c8,
	ZINITIX_X_RESOLUTION = 0x00c0,
	ZINITIX_Y_RESOLUTION = 0x00c1,
	ZINITIX_COOR_ORIENTATION = 0x00c2,
	ZINITIX_INT_ENABLE_FLAG = 0x00f0,
	ZINITIX_PERIODICAL_INT_INTERVAL = 0x00f1,
	ZINITIX_AFE_FREQUENCY = 0x0100,
	ZINITIX_DEBUG_REG = 0x0115,
	ZINITIX_USB_DETECT = 0x0116,
	ZINITIX_GESTURE_REG = 0x011d,
	ZINITIX_INTERNAL_FLAG_02 = 0x011e,
	ZINITIX_INTERNAL_FLAG_03 = 0x011f,
	ZINITIX_MINOR_FW_VERSION = 0x0121,
	ZINITIX_DND_N_COUNT = 0x0122,
	ZINITIX_WAKE_UP_GENSTRURE = 0x0126,
	ZINITIX_CHECKSUM_RESULT = 0x012c,
	ZINITIX_DND_U_COUNT = 0x0135,
	ZINITIX_BTN_WIDTH = 0x016d,
	ZINITIX_INIT_FLASH = 0x01d0,
	ZINITIX_WRITE_FLASH = 0x01d1,
	ZINITIX_READ_FLASH = 0x01d2,
	ZINITIX_UNGRADE_MODE_INIT = 0x01d3,
	ZINITIX_REPEAT_MODE_INIT = 0x01d4,
	ZINITIX_FU_MODE_ENTER = 0x01d5,
	ZINITIX_FW_FLUSH = 0x01dd,
	ZINITIX_UNGRADE_MODE_ENTER = 0x01de,
	ZINITIX_ERASE_FLASH = 0x01df,
	ZINITIX_RAWDATA_REG = 0x0200,
	ZINITIX_REG_VENDOR_EN = 0xc000,  // ic vendor cmd enable reg
	ZINITIX_REG_PROG_CTRL = 0xc001,  // firmware fun control reg
	ZINITIX_REG_NVM_CTRL = 0xc002,  // nvm init reg
	ZINITIX_REG_NVM_VPP_CTRL = 0xc003,  // nvm vpp reg
	ZINITIX_CMD_INT_CLEAR = 0xc004,  // intn clear
	ZINITIX_REG_NVM_WP_CTRL = 0xc104,  // nvm wp reg
	ZINITIX_REG_CHIP_ID = 0xcc00,  // chip id reg address
	ZINITIX_REG_UNDERWATER_DETECT_STATUS = 0x3d7,
	ZINITIX_REG_3DA = 0x3da,
	ZINITIX_WAKEUP_CMD_0XF8 = 0xf8,
};

#if defined(CONFIG_HUAWEI_DSM)
enum zinitix_fw_uptate_state {
	ZINITIX_ENTER_PARM_MODEL_FAIL = 0,
	ZINITIX_CHECK_FIRMAWARE_SIZE_IN_PARM_MODEL_FAIL,
	ZINITIX_AUTO_CONFIG_CLOCK_FAIL,
	ZINITIX_ENTER_PARM_UPDATE_MODEL_FROM_PARM_FAIL,
	ZINITIX_ERASURE_APP_AREA_FAIL,
	ZINITIX_WRITE_APP_DATA_FAIL,
	ZINITIX_READ_CHECK_SUM_FAIL,
	TS_UPDATE_STATE_UNDEFINE = 255,
};
#endif
struct ts_event {
	u16 position_x[ZINITIX_MAX_TOUCH_POINTS];
	u16 position_y[ZINITIX_MAX_TOUCH_POINTS];

	u16 pressure[ZINITIX_MAX_TOUCH_POINTS];
	u16 area[ZINITIX_MAX_TOUCH_POINTS];
	u16 ewx[ZINITIX_MAX_TOUCH_POINTS];
	u16 ewy[ZINITIX_MAX_TOUCH_POINTS];
	u16 xer[ZINITIX_MAX_TOUCH_POINTS];
	u16 yer[ZINITIX_MAX_TOUCH_POINTS];

	/* touch event: 0 -- down; 1-- up; 2 -- contact */
	u8 touch_event[ZINITIX_MAX_TOUCH_POINTS];
	/* touch ID */
	u8 finger_id[ZINITIX_MAX_TOUCH_POINTS];
	u8 touch_point;
	u8 touch_point_num;
	int touchs;
	unsigned int gesture_wakeup_value;
};

/* platform data that can be configured by extern */
struct zinitix_platform_data {
	struct platform_device *zinitix_platform_dev;
	struct ts_kit_device_data *zinitix_device_data;
	struct pinctrl *ts_pinctrl;
	struct pinctrl_state *pinctrl_state_active;
	struct pinctrl_state *pinctrl_state_suspend;
	struct pinctrl_state *pinctrl_state_release;
	unsigned int pram_projectid_addr;
	char project_id[ZINITIX_PROJECT_ID_LEN];
	char vendor_name[ZINITIX_VENDOR_NAME_LEN + 1];
	struct ts_event touch_data;
	u32 support_get_tp_color; /* for tp color */
	struct regulator *vddd;
	struct regulator *vdda;
	int self_ctrl_power;
	int power_down_ctrl;
	bool open_threshold_status;
	int only_open_once_captest_threshold;
	int projectid_length_control_flag;
	u32 enable_edge_touch;
	u32 edge_data_addr;
	u32 roi_pkg_num_addr;
	u32 flash_size;
	int need_distinguish_lcd;
	// 0 : fw depend on TP and others ,1 : fw only depend on lcd.
	int fw_only_depend_on_lcd;
	char lcd_panel_info[LCD_PANEL_INFO_MAX_LEN];
	char lcd_module_name[LCD_PANEL_INFO_MAX_LEN];
	u16 chip_id;
	u16 vendor_id;
	u16 ic_revision;
	u16 fw_version;
	u16 fw_minor_version;
	u16 reg_data_version;
	u16 multi_fingers;
	u16 channel_x_num;
	u16 channel_y_num;
	u16 key_num;
	u16 afe_frequency;
	u16 dnd_n_cnt;
	u16 dnd_u_cnt;
	u16 touch_mode;
	u8 touch_switch_game_reg;
	u8 power_on : 1;
	u8 suspend_state : 1;
	u8 firmware_updating : 1;
	u8 irq_is_disable : 1;
	u8 irq_processing : 1;
	u8 update_fw_force : 1;
	u8 driver_init_ok : 1;
	u8 unused : 1;
};

struct zinitix_touch_coord {
	u16 x;
	u16 y;
	u8 width;
	u8 sub_status;
};

struct zinitix_touch_point_info {
	u16 status;
	u8 finger_cnt;
	u8 time_stamp;
	struct zinitix_touch_coord coord[ZINITIX_MAX_SUPPORTED_FINGER_NUM];
};

struct zinitix_tcm_abnormal_info {
	union {
		struct {
			unsigned char gnd_connection:1;
			unsigned char tx_sns_ch:1;
			unsigned char rx_sns_ch:1;
			unsigned char pixel_sns:1;
			unsigned char display_noise:1;
			unsigned char charge_noise:1;
			unsigned char charge_noise_hop:1;
			unsigned char charge_noise_ex:1;
			unsigned char self_cap_noise:1;
			unsigned char mutual_cap_noise:1;
			unsigned char high_temp:1;
			unsigned char low_temp:1;
			unsigned char large_bending:1;
			unsigned char reserved:3;
		}__packed;
		unsigned char data[2];
	};
};

struct zinitix_tcm_helper {
	atomic_t task;
	struct work_struct work;
	struct workqueue_struct *workqueue;
};

struct zinitix_tcm_watchdog {
	bool run;
	unsigned char count;
	struct delayed_work work;
	struct workqueue_struct *workqueue;
};

struct zinitix_tcm_buffer {
	bool clone;
	unsigned char *buf;
	unsigned int buf_size;
	unsigned int data_length;
	struct mutex buf_mutex;
};

struct zinitix_tcm_report {
	unsigned char id;
	struct zinitix_tcm_buffer buffer;
};

struct zinitix_tcm_identification {
	unsigned char version;
	unsigned char mode;
	unsigned char part_number[16];
	unsigned char build_id[4];
	unsigned char max_write_size[2];
};

struct zinitix_tcm_board_data {
	bool x_flip;
	bool y_flip;
	int x_max;
	int y_max;
	int x_max_mt;
	int y_max_mt;
	int max_finger_objects;
	int support_vendor_ic_type;
	bool swap_axes;
	int irq_gpio;
	int irq_on_state;
	int power_gpio;
	int power_on_state;
	int reset_gpio;
	int reset_on_state;
	unsigned int spi_mode;
	unsigned int power_delay_ms;
	unsigned int reset_delay_ms;
	unsigned int reset_active_ms;
	unsigned int byte_delay_us;
	unsigned int block_delay_us;
	unsigned int ubl_i2c_addr;
	unsigned int ubl_max_freq;
	unsigned int ubl_byte_delay_us;
	unsigned long irq_flags;
	const char *pwr_reg_name;
	const char *bus_reg_name;
	const char *default_project_id;
	const char *chip_vendor_name;
};

struct zinitix_tcm_boot_info {
	unsigned char version;
	unsigned char status;
	unsigned char asic_id[2];
	unsigned char write_block_size_words;
	unsigned char erase_page_size_words[2];
	unsigned char max_write_payload_size[2];
	unsigned char last_reset_reason;
	unsigned char pc_at_time_of_last_reset[2];
	unsigned char boot_config_start_block[2];
	unsigned char boot_config_size_blocks[2];
	unsigned char display_config_start_block[4];
	unsigned char display_config_length_blocks[2];
	unsigned char backup_display_config_start_block[4];
	unsigned char backup_display_config_length_blocks[2];
	unsigned char custom_otp_start_block[2];
	unsigned char custom_otp_length_blocks[2];
};

struct zinitix_tcm_touch_info {
	unsigned char image_2d_scale_factor[4];
	unsigned char image_0d_scale_factor[4];
	unsigned char hybrid_x_scale_factor[4];
	unsigned char hybrid_y_scale_factor[4];
};

struct zinitix_tcm_message_header {
	unsigned char marker;
	unsigned char code;
	unsigned char length[2];
};

struct zinitix_tcm_esd_info {
	unsigned char esd_action;
	unsigned char esd_reason;
};

struct zinitix_tcm_hcd {
	struct ts_kit_device_data *zinitix_tcm_chip_data;
	struct regulator *zinitix_tcm_tp_vci;
	struct regulator *zinitix_tcm_tp_vddio;
	struct pinctrl *pctrl;
	struct pinctrl_state *pins_default;
	struct pinctrl_state *pins_idle;
	pid_t isr_pid;
	atomic_t command_status;
	atomic_t host_downloading;
	wait_queue_head_t hdl_wq;
	int irq;
	bool init_okay;
	bool do_polling;
	bool in_suspend;
	bool ud_sleep_status;
	bool irq_enabled;
	bool host_download_mode;
	bool htd;
	u16 roi_enable_status;
	unsigned char zinitix_tcm_roi_data[ROI_DATA_LENGTH];
	unsigned char fb_ready;
	unsigned char command;
	unsigned char async_report_id;
	unsigned char status_report_code;
	unsigned char response_code;
	unsigned int read_length;
	unsigned int payload_length;
	unsigned int packrat_number;
	unsigned int packrat_number_old;
	unsigned int rd_chunk_size;
	unsigned int wr_chunk_size;
	unsigned int app_status;
	unsigned int device_status;
	unsigned int tp_status_report_support;
	bool checking_abnormal_state;
	bool in_before_suspend;
	bool device_status_check;
	bool in_suspend_charge;
	bool after_resume_done;
	struct zinitix_tcm_abnormal_info ab_device_status;
	struct platform_device *pdev;
	struct regulator *pwr_reg;
	struct regulator *bus_reg;
	struct kobject *sysfs_dir;
	struct kobject *dynamnic_config_sysfs_dir;
	struct mutex extif_mutex;
	struct delayed_work polling_work;
	struct workqueue_struct *polling_workqueue;
	struct task_struct *notifier_thread;
#ifdef CONFIG_FB
	struct notifier_block fb_notifier;
#endif
	struct zinitix_tcm_buffer in;
	struct zinitix_tcm_buffer out;
	struct zinitix_tcm_buffer resp;
	struct zinitix_tcm_buffer temp;
	struct zinitix_tcm_buffer config;
	struct zinitix_tcm_report report;
	struct zinitix_tcm_boot_info boot_info;
	struct zinitix_tcm_touch_info touch_info;
	struct zinitix_tcm_identification id_info;
	struct zinitix_tcm_helper helper;
	struct zinitix_tcm_watchdog watchdog;
	struct zinitix_tcm_board_data *bdata;
	struct zinitix_tcm_esd_info esd_info;
	unsigned int esd_report_status;
	unsigned int use_esd_report;
	unsigned int use_dma_download_firmware;
	unsigned int downmload_firmware_frequency;
	unsigned int spi_comnunicate_frequency;
	unsigned int support_cold_patch;
	unsigned int zinitix_spec_delay_after_lcd;
	unsigned int baseline_status_flag;
	unsigned int irq_storm_sum;
	unsigned int support_esd_power_reset;
	const struct zinitix_tcm_hw_interface *hw_if;
	char fw_name[MAX_STR_LEN * 4];
	unsigned int aft_wxy_enable;
};

static inline unsigned int le2_to_uint(const unsigned char *src)
{
	return (unsigned int)src[0] +
			((unsigned int)src[1] << 8);
}

static inline unsigned int le4_to_uint(const unsigned char *src)
{
	return (unsigned int)src[0] +
			((unsigned int)src[1] << 8) +
			((unsigned int)src[2] << 16) +
			((unsigned int)src[3] << 24);
}

static inline unsigned int ceil_div(unsigned int dividend, unsigned divisor)
{
	if (!divisor) {
		TS_LOG_ERR("divisor is ZERO\n");
		return -EINVAL;
	}
	return (dividend + divisor - 1) / divisor;
}

int zts_read(u16 *addr, u16 addr_len, u8 *value, u16 values_size);
int zts_write(u16 *value, u16 values_size);
struct ts_kit_device_data *zinitix_get_device_data(void);
struct zinitix_platform_data *zinitix_get_platform_data(void);

#endif /* __ZINITIX_CORE_H__ */
