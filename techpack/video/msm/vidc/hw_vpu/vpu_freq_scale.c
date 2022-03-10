/*
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
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

#include "vpu_freq_scale.h"

#include <linux/sysfs.h>
#include <linux/device.h>

#include <../techpack/video/msm/vidc/msm_vidc_internal.h>
#include <../techpack/video/msm/vidc/msm_vidc_common.h>
#include <../techpack/video/msm/vidc/msm_vidc_resources.h>
#include <../techpack/video/msm/vidc/msm_vidc.h>

static int msm_hw_vpu_voting = !1;

bool is_vpu_freq_scale(void)
{
	if (msm_hw_vpu_voting < 0)
		msm_hw_vpu_voting = 0;
	return msm_hw_vpu_voting;
}

unsigned long get_vpu_freq_scale(void)
{
	return (unsigned long)msm_hw_vpu_voting;
}

static ssize_t available_frequencies_show(struct device *dev,
					  struct device_attribute *attr,
					  char *buf)
{
	int i;
	ssize_t count = 0;
	struct msm_vidc_core *core;
	struct allowed_clock_rates_table *allowed_clks_tbl = NULL;

	core = get_vidc_core(MSM_VIDC_CORE_VENUS);
	if (core == NULL)
		return -EINVAL;
	allowed_clks_tbl = core->resources.allowed_clks_tbl;
	for (i = core->resources.allowed_clks_tbl_size - 1; i >= 0; i--) {
		count += scnprintf(&buf[count], (PAGE_SIZE - count),
				  "%u ", allowed_clks_tbl[i].clock_rate);
	}

	count += scnprintf(&buf[count], PAGE_SIZE - count, "\n");

	return count;
}

static DEVICE_ATTR_RO(available_frequencies);

static ssize_t vpu_freq_scale_show(struct device *dev,
				   struct device_attribute *attr,
				   char *buf)
{
	struct msm_vidc_core *core;
	core = get_vidc_core(MSM_VIDC_CORE_VENUS);
	if (core == NULL)
		return -EINVAL;

	return scnprintf(buf, PAGE_SIZE, "%lu\n", core->curr_freq);
}

static ssize_t vpu_freq_scale_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf,
				    size_t count)
{
	int rc;

	rc = kstrtoint(buf, 0, &msm_hw_vpu_voting);
	if (rc || msm_hw_vpu_voting < 0) {
		pr_err("Invalid freq scale value :%s\n", buf);
		return -EINVAL;
	}

	return count;
}

static DEVICE_ATTR_RW(vpu_freq_scale);

static struct attribute *vpu_freq_scale_attrs[] = {
	&dev_attr_vpu_freq_scale.attr,
	&dev_attr_available_frequencies.attr,
	NULL,
};

static struct attribute_group dev_vpu_freq_scale_group = {
	.name = "vpu_freq",
	.attrs = vpu_freq_scale_attrs,
};

int vpu_create_group(struct kobject *kobj)
{
	int rc;

	rc = sysfs_create_group(kobj, &dev_vpu_freq_scale_group);
	if (rc) {
		pr_err("Failed to create vpu freq scale attributes\n");
		return -EINVAL;
	}
	return 0;
}

void vpu_remove_group(struct kobject *kobj)
{
	sysfs_remove_group(kobj, &dev_vpu_freq_scale_group);
	return;
}
