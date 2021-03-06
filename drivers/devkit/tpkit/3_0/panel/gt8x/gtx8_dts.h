#ifndef __GTX8_DTS_H__
#define __GTX8_DTS_H__

#define GTP_GTX8_CHIP_NAME		"gtx8"
#define HUAWEI_TS_KIT			"huawei,ts_kit"


#define GTP_IRQ_CFG			"irq_config"
#define GTP_ALGO_ID			"algo_id"
#define GTP_IC_TYPES			"ic_type"
#define GTP_WD_CHECK			"need_wd_check_status"
#define GTP_WD_TIMEOUT			"check_status_watchdog_timeout"

#define GTP_X_MAX_MT			"x_max_mt"
#define GTP_Y_MAX_MT			"y_max_mt"
#define GTP_VCI_GPIO_TYPE		"vci_gpio_type"
#define GTP_VCI_REGULATOR_TYPE		"vci_regulator_type"
#define GTP_VDDIO_GPIO_TYPE		"vddio_gpio_type"
#define GTP_VDDIO_REGULATOR_TYPE	"vddio_regulator_type"
#define GTP_COVER_FORCE_GLOVE		"force_glove_in_smart_cover"
#define GTP_TEST_TYPE			"tp_test_type"

#define GTP_HOSTLER_SWITCH_ADDR		"holster_switch_addr"
#define GTP_HOSTLER_SWITCH_BIT		"holster_switch_bit"

#define GTP_GLOVE_SWITCH_ADDR		"glove_switch_addr"
#define GTP_GLOVE_SWITCH_BIT		"glove_switch_bit"

#define GTP_ROI_SWITCH_ADDR		"roi_switch_addr"
#define GTP_ROI_DATA_SIZE		"roi_data_size"
#define GTP_GESTURE_SUPPORTED		"gesture_supported"
#define GTP_PEN_SUPPORTED		"pen_supported"
#define GTP_PRAM_PROJECTID_ADDR		"pram_projectid_addr"
#define GTP_PROJECT_ID			"project_id"
#define GTP_IC_TYPE			"ic_type"
#define GTP_EASY_WAKE_SUPPORTED		"easy_wakeup_supported"

#define GTP_TOOL_SUPPORT		"tools_support"
#define GTP_STATIC_PID_SUPPORT		"gtx8_static_pid_support"
#define GTP_CHARGER_SUPPORT		"charger_supported"
#define GTP_TP_TEST_TYPE		"tp_test_type"
#define GTP_CHIP_NAME			"chip_name"
#define GTP_MODULE_VENDOR		"module_vendor"
#define GTP_EDGE_ADD			"gtx8_edge_add"
#define GTP_ROI_DATA_ADDR		"gtx8_roi_data_add"
#define GTP_ROI_FW_SUPPORTED		"gtx8_roi_fw_supported"
#define GTP_PANEL_ID			"panel_id"

#define GTP_FW_UPDATE_LOGIC		"fw_update_logic"
#define GTX8_SLAVE_ADDR			"slave_address"

#define GTX8_SUPPORT_TP_COLOR		"support_get_tp_color"
#define GTX8_SUPPORT_READ_PROJECTID	"support_read_projectid"
#define GTX8_PROVIDE_PANEL_ID_SUPPORT	"provide_panel_id_suppot"
#define GTX8_FILENAME_CONTAIN_PROJECTID	"support_filename_contain_projectid"
#define GTX8_CHECKED_ESD_RESET_CHIP "support_checked_esd_reset_chip"
#define GTX8_DELETE_INSIGNIFICANT_INFO "support_deleted_insignificant_info"
#define GTX8_WAKE_LOCK_SUSPEND		"support_wake_lock_suspend"
#define GTX8_SUPPORT_PRIORITY_READ_SENSORID "support_priority_read_sensorid"
#define GTX8_DOZE_FAILED_NOT_SEND_CFG "support_doze_failed_not_send_cfg"
#define GTX8_SUPPORT_READ_MORE_DEUGMSG "support_read_more_debug_msg"
#define GTX8_FILENAME_CONTAIN_LCD_MOUDULE "support_filename_contain_lcd_module"
#define GTX8_SUPPORT_IC_USE_MAX_RESOLUTION "support_ic_use_max_resolution"
#define GTX8_SUPPORT_READ_GESTURE_WITHOUT_CHECKSUM "support_read_gesture_without_checksum"
#define GTX8_SUPPORT_PEN_USE_MAX_RESOLUTION "support_pen_use_max_resolution"
#define GTX8_SUPPORT_RETRY_READ_GESTURE "support_retry_read_gesture"
#define GTP_BOOT_PROJ_CODE_ADDR2	0x20
#define GTX8_DEFAULT_SLAVE_ADDR		0x5D

#endif
