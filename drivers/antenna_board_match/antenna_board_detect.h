/*
 * drivers/antenna_board_match/antenna_board_adc_detect.h
 *
 * huawei antenna board detect driver
 *
*/

#ifndef _ANTENNA_BOARD_ADC_DETECT
#define _ANTENNA_BOARD_ADC_DETECT
#include <linux/device.h>


struct antenna_device_info {
    struct device *dev;
	bool is_adc_mode;
    struct device_node *np_node;
	bool switch_mode;
};

enum antenna_type {
    ANTENNA_BOARD_STATUS_DETECT,
};

enum {
    STATUS_DOWN_DOWN = 0,    /* ID1 down, ID2 down, boardid version is AL00. */
    STATUS_DOWN_UP,          /* ID1 down, ID2 UP, boardid version is TL10. */
    STATUS_UP_DOWN,          /* ID1 up, ID2 down, boardid version is LX1. */
    STATUS_UP_UP,            /* ID1 up, ID2 up, boardid version is LX2. */
    STATUS_DOWN_DOWN_OTHER,  /* ID1 down, ID2 down, ID1 and ID2 connection, boardid version is LX3. */
    STATUS_UP_UP_OTHER,      /* ID1 up, ID2 up, ID1 and ID2 connection,reserved. */
    STATUS_ERROR,
};

#define ANATENNA_DETECT_SUCCEED 1
#define ANATENNA_DETECT_FAIL    0
#define DEFAULT_VOL    -50
#define DELAY_MS    1
#define DEFAULT_GPIO_CNT 1
#define DEFAULT_GPIO_VERSION -1

#endif
