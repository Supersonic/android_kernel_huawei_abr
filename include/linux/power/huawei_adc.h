/*opyright (C) 2013 HUAWEI, Inc.
 *File Name: kernel/include/linux/hw_adc.h
 i*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __HW_ADC_H__
#define __HW_ADC_H__
#include <linux/version.h>
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
#include <linux/qpnp/qpnp-adc.h>
#endif

#define QPNP_VADC_LDO16_MIN_MICROVOLT               1800000
#define QPNP_VADC_LDO16_MAX_MICROVOLT               1800000
#define QPNP_VADC_LDO16_CONSUMER_SUPPLY             "custom_l16"
#define GET_PMI_SUB_VOLTAGE_RETURN_RATIO            1000
#define GPIO_UP             1
#define GPIO_DOWN           0

extern bool huawei_adc_init_done(struct device_node *np);
extern int get_adc_converse_voltage(struct device_node *np, int gpio_status);
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
extern int get_adc_converse_temp(struct device_node *np, int gpio_status,
	struct qpnp_vadc_map_pt *pts, uint32_t tablesize, int32_t *output);
#endif

#endif
