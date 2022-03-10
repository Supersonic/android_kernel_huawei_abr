/* volume_key_event driver
*
* Copyright (c) 2021 Huawei Technologies Co., Ltd.
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

#ifndef _VOLUME_KEY_EVENT_H
#define _VOLUME_KEY_EVENT_H
#include <linux/notifier.h>

enum volume_key_press_staus {
	VOLUME_DOWN_KEY_PRESS_RELEASE,
	VOLUME_DOWN_KEY_PRESS_DOWN,
	VOLUME_KEY_MAX_STATUS
};

int volume_key_register_notifier(struct notifier_block *nb);
int volume_key_unregister_notifier(struct notifier_block *nb);
int volume_key_call_notifiers(unsigned long val,void *v);
void volume_key_status_distinguish(unsigned long pressed);
int volume_key_nb_init(void);

#endif

