/*
 * audio_interface.h
 *
 * huawei audio header file
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

#ifndef __AUDIO_INTERFACE_H__
#define __AUDIO_INTERFACE_H__

#define HUAWEI_USB_C_AUDIO_ADAPTER "HUAWEI USB-C TO 3.5MM AUDIO ADAPTER"

struct usb_headphone_ops {
	int (*otg_enable)(int en);
	int (*otg_switch_mode)(int en);
	int (*otg_need_gpio_switch)(int en);
	int8_t low_power_state;
	int8_t active;
};

void register_usb_low_power_otg(struct usb_headphone_ops *ops);
void usb_low_power_enable(int enable);

#endif /* __AUDIO_INTERFACE_H__ */
