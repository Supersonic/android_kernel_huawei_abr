/*
  * h sensors_sysfs_hinge.h
  *
  * code for bal_poweroff_charging_fold
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

void report_fold_status_when_poweroff_charging(int status);
int get_fold_hinge_status(void);
