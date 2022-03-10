/*
 * hw_audio_info.h
 *
 * hw audio info definition
 *
 * Copyright (c) 2020-2020 Huawei Technologies Co., Ltd.
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

#ifndef _HW_AUDIO_INFO_H
#define _HW_AUDIO_INFO_H

#include <linux/kernel.h>

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

struct audio_prop_strategy {
	char *key;
	int value;
};

/*
 * Audio property is an unsigned 32-bit integer stored in the variable of
 * audio_prop.The meaning of audio_property is defined as following MACROs
 * in groups of 4 bits
 *
 * Bit4 ~ bit7:
 * Actually existing mics on the phone, it's NOT relevant to fluence. Using
 * one bit to denote the existence of one kind of mic, possible mics are:
 * master mic: the basic mic used for call and record, if it doesn't exist
 *             means the config or software is wrong.
 * secondary mic: auxiliary mic, usually used for fluence or paired with
 *             speaker in handsfree mode, it's possible that this mic
 *                  exist but fluence isn't enabled.
 * error mic: used in handset ANC.
 *
 * Bit12 ~ bit15:
 * Denote which mic would be used in hand held mode, please add as needed
 *
 * Bit16 ~ bit19:
 * Denote which mic would be used in loud speaker mode, please add as needed
 */
static struct audio_prop_strategy audio_prop_table[] = {
	{ "builtin-primary-mic-exist", 0x00000010 },
	{ "builtin-second-mic-exist", 0x00000020 },
	{ "builtin-error-mic-exist", 0x00000040 },
	{ "hand_held_primary_mic_strategy", 0x00001000 },
	{ "hand_held_dual_mic_strategy", 0x00002000 },
	{ "hand_held_aanc_mic_strategy", 0x00004000 },
	{ "loud_speaker_primary_mic_strategy", 0x00010000 },
	{ "loud_speaker_second_mic_strategy", 0x00020000 },
	{ "loud_speaker_error_mic_strategy", 0x00040000 },
	{ "loud_speaker_dual_mic_strategy", 0x00080000 }
};

#define SIMPLE_PA_DEFAULR_MODE 1

#define INVALID_GPIO           (-1)
#define GPIO_PULL_UP           1
#define GPIO_PULL_DOWN         0

#define SWITCH_ON              1
#define SWITCH_OFF             0

#define ELECTRIC_LEVEL_LOW     0
#define ELECTRIC_LEVEL_HIGH    1

#define PRODUCT_IDENTIFIER_BUFF_SIZE  32
#define SMARTPA_TYPE_BUFF_SIZE  32
#define SMARTPA_NUM_BUFF_SIZE 32

enum simple_type {
	SIMPLE_PA_AWINIC,
	SIMPLE_PA_TI,
};

enum pa_num {
	SIMPLE_PA_LEFT,
	SIMPLE_PA_RIGHT,
	SIMPLE_PA_NUMBER,
};

struct hw_audio_info {
	unsigned int audio_prop;
	enum simple_type pa_type;
	int pa_mode;
	int pa_gpio[SIMPLE_PA_NUMBER];
	int pa_switch[SIMPLE_PA_NUMBER];
	int hac_gpio;
	int hac_switch;
	int hs_gpio;
	int hs_switch;
	bool mic_differential_mode;
	bool is_init_smartpa_type;
	char product_identifier[PRODUCT_IDENTIFIER_BUFF_SIZE];
	char smartpa_type_str[SMARTPA_TYPE_BUFF_SIZE];
	char smartpa_num_str[SMARTPA_NUM_BUFF_SIZE];
};

#endif

