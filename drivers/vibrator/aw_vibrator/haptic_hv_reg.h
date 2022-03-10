 /*
  * haptic_hv_reg.h
  *
  * code for vibrator
  *
  * Copyright (c) 2020 Huawei Technologies Co., Ltd.
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


#ifndef _HAPTIC_HV_REG_H_
#define _HAPTIC_HV_REG_H_

/*********************************************************
 *
 * Register Access
 *
 *********************************************************/
#define REG_NONE_ACCESS					(0)
#define REG_RD_ACCESS					(1 << 0)
#define REG_WR_ACCESS					(1 << 1)
#define AW_HAPTIC_REG_MAX				(0xff)

#endif
