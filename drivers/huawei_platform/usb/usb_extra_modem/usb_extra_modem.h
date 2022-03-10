/*
 * usb_extra_modem.h
 *
 * header file for usb_extra_modem driver
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

#ifndef _USB_EXTRA_MODEM_H_
#define _USB_EXTRA_MODEM_H_

#define UEM_BASE_DEC                      10

#define UEM_LOADSWITCH_GPIO_DISABLE       0
#define UEM_LOADSWITCH_GPIO_ENABLE        1

enum uem_loadswitch_gpio {
	LOADSWITCH_GPIO_Q4,
	LOADSWITCH_GPIO_Q5,
};

struct uem_dev_info {
	struct device *dev;
	int gpio_q4;
	int gpio_q5;
};

struct uem_dev_info *uem_get_dev_info(void);

#endif /* _USB_EXTRA_MODEM_H_ */
