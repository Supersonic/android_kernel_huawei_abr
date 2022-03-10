#ifndef CTS_CONFIG_H
#define CTS_CONFIG_H

/** Driver version */
#define CFG_CTS_DRIVER_MAJOR_VERSION        1
#define CFG_CTS_DRIVER_MINOR_VERSION        3
#define CFG_CTS_DRIVER_PATCH_VERSION        3

#define CFG_CTS_DRIVER_VERSION              "v1.3.32"

#define CHIPONE_CHIP_NAME "chipone"
#define CFG_CTS_MAX_I2C_XFER_SIZE			48u

#define CFG_CTS_MAX_TOUCH				TS_MAX_FINGER

#define CFG_CTS_GET_DATA_RETRY             3
#define CFG_CTS_GET_FLAG_RETRY             20
#define CFG_CTS_GET_FLAG_DELAY             3

#define CFG_CTS_FIRMWARE_IN_FS
#ifdef CFG_CTS_FIRMWARE_IN_FS
#define CFG_CTS_FIRMWARE_FILEPATH       "/etc/firmware/ICNT8918.bin"
#endif

#ifdef CONFIG_PROC_FS
#define CONFIG_CTS_LEGACY_TOOL
#endif /* CONFIG_PROC_FS */

#ifdef CONFIG_SYSFS
    /* Sys FS for gesture report, debug feature etc. */
#define CONFIG_CTS_SYSFS
#endif				/* CONFIG_SYSFS */

#define CFG_CTS_MAX_TOUCH_NUM               (2)

/* Gesture wakeup */
#define CFG_CTS_GESTURE
#ifdef CFG_CTS_GESTURE
#define GESTURE_DOUBLE_TAP 0x50
#define GESTURE_SINGLE_TAP 0x7f

#endif				/* CFG_CTS_GESTURE */
#ifdef CONFIG_CTS_LEGACY_TOOL
#define CFG_CTS_TOOL_PROC_FILENAME      "cts_tool"
#endif /* CONFIG_CTS_LEGACY_TOOL */
#endif				/* CTS_CONFIG_H */
