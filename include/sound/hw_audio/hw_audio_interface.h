/*
 * hw_audio_interface.h
 *
 * hw audio interface
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

#ifndef _HW_AUDIO_INTERFACE_H
#define _HW_AUDIO_INTERFACE_H

#ifdef CONFIG_HW_AUDIO_INFO
#include <sound/soc.h>

static const char * const g_hac_switch_text[] = { "OFF", "ON" };
static const char * const g_simple_pa_switch_text[] = { "OFF", "ON" };
static const char * const g_simple_pa_mode_text[] = { "ZERO", "ONE", "TWO",
	"THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN" };
static const char * const g_hs_switch_text[] = {"OFF", "ON"};

static const struct soc_enum g_hw_snd_priv_enum[] = {
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(g_hac_switch_text),
		g_hac_switch_text),
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(g_simple_pa_switch_text),
		g_simple_pa_switch_text),
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(g_simple_pa_mode_text),
		g_simple_pa_mode_text),
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(g_hs_switch_text),
		g_hs_switch_text),
};

enum smartpa_type {
	INVALID,
	CS35LXX,
	AW882XX,
	TFA98XX,
	SMARTPA_TYPE_MAX
};

enum hw_smartpa_num {
	SMARTPA_NUM_NONE = 0,
	SMARTPA_NUM_ONE,
	SMARTPA_NUM_TWO,
	SMARTPA_NUM_THREE,
	SMARTPA_NUM_FOUR,
	SMARTPA_NUM_FIVE,
	SMARTPA_NUM_SIX,
	SMARTPA_NUM_SEVEN,
	SMARTPA_NUM_EIGHT,
	SMARTPA_NUM_MAX,
};

void hw_set_smartpa_type(const char *buf, int len);
enum smartpa_type hw_get_smartpa_type(void);
int get_smartpa_num(void);

bool is_mic_differential_mode(void);
int hw_simple_pa_power_set(int gpio, int value);
int hac_switch_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol);
int hac_switch_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol);
int simple_pa_mode_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol);
int simple_pa_mode_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol);
int simple_pa_switch_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol);
int simple_pa_r_switch_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol);
int simple_pa_switch_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol);
int simple_pa_r_switch_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol);
int hs_switch_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol);
int hs_switch_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol);

#else
static inline bool is_mic_differential_mode(void) { return false; }
static inline int hw_simple_pa_power_set(int gpio, int value) { return 0; }
#endif // CONFIG_HW_AUDIO_INFO
#endif // _HW_AUDIO_INTERFACE_H_

