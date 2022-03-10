/*
 * hw_sensor_info.c
 *
 * set sensor app info API
 *
 * Copyright (c) 2019-2019 Huawei Technologies Co., Ltd.
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

#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/list.h>
#include <linux/export.h>
#include <misc/app_info.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/kobject.h>
#include <linux/kthread.h>

#define HW_FUNC_RET_FAIL     (-1)
#define HW_FUNC_RET_SUCC     1
#define APP_NAME_VALUE_SPLIT_CHAR  "*"
#define SENSOR_MAX              20
#define HW_APP_INFO_1_VAL       1
#define HW_APP_INFO_5_VAL       5
#define APP_INFO_NAME_LEN       32
#define APP_INFO_VALUE_LEN      64

struct info_node {
	char name[APP_INFO_NAME_LEN];
	char value[APP_INFO_VALUE_LEN];
	struct list_head entry;
};

struct sensor_node {
	char name[APP_INFO_NAME_LEN];
};

struct sensor_node sensor_type[SENSOR_MAX];

static struct kobject *set_appinfo_node_kobject;
static LIST_HEAD(app_info_list);

static ssize_t hw_set_app_info_node_store(struct kobject *dev,
	struct kobj_attribute *attr, const char *buf, size_t size)
{
	char *app_str = NULL;
	char *strtok = NULL;
	char *temp_ptr = NULL;
	char app_name[APP_INFO_NAME_LEN] = { '\0' };
	char app_value[APP_INFO_VALUE_LEN] = { '\0' };
	int name_len, value_len, buf_len;
	int ret;
	int i;
	bool flag = false;

	if (buf == NULL) {
		pr_err("%s: buf is null\n", __func__);
		return HW_FUNC_RET_FAIL;
	}

	buf_len = strlen(buf);
	app_str = kmalloc(buf_len + HW_APP_INFO_5_VAL, GFP_KERNEL);
	if (app_str == NULL) {
		pr_err("%s: kmalloc fail\n",__func__);
		return HW_FUNC_RET_FAIL;
	}
	temp_ptr = app_str;

	memset(app_str, 0, buf_len + HW_APP_INFO_5_VAL);
	snprintf(app_str, buf_len + HW_APP_INFO_5_VAL - HW_APP_INFO_1_VAL, "%s", buf);
	strtok = strsep(&app_str, APP_NAME_VALUE_SPLIT_CHAR);
	if (strtok != NULL) {
		name_len = strlen(strtok);
		memcpy(app_name, strtok,
			((name_len > (APP_INFO_NAME_LEN - HW_APP_INFO_1_VAL)) ?
			(APP_INFO_NAME_LEN - HW_APP_INFO_1_VAL) : name_len));
	} else {
		pr_err("%s: buf name Invalid: %s", __func__, buf);
		goto split_fail_exit;
	}
	strtok = strsep(&app_str, APP_NAME_VALUE_SPLIT_CHAR);
	if (strtok != NULL) {
		value_len = strlen(strtok);
		memcpy(app_value, strtok,
			((value_len > (APP_INFO_VALUE_LEN - HW_APP_INFO_1_VAL)) ?
			(APP_INFO_VALUE_LEN - HW_APP_INFO_1_VAL) : value_len));
	} else {
		pr_err("%s: buf value Invalid: %s", __func__, buf);
		goto split_fail_exit;
	}

	for (i = 0; i < SENSOR_MAX; i++) {
		if (strncmp(sensor_type[i].name, app_name,sizeof(app_name)) == 0) {
			flag = true;
			break;
		} else if (strncmp(sensor_type[i].name, "", sizeof(app_name)) == 0) {
			strncpy(sensor_type[i].name, app_name,
				sizeof(app_name) - HW_APP_INFO_1_VAL);
			break;
		}
	}

	if (!flag) {
		ret = app_info_set(app_name, app_value);
		if (ret < 0) {
			pr_err("%s: app_info_set fail", __func__);
			goto split_fail_exit;
		}
	}

split_fail_exit:
	if (temp_ptr != NULL) {
		kfree(temp_ptr);
		temp_ptr = NULL;
	}
	return size;
}

static struct kobj_attribute sys_set_appinfo_init = {
	.attr = {.name = "set_app_info_node", .mode = (S_IRUGO | S_IWUSR | S_IWGRP)},
	.show = NULL,
	.store = hw_set_app_info_node_store,
};

static int app_info_node_init()
{
	int ret;

	set_appinfo_node_kobject = kobject_create_and_add("set_app_info", NULL);
	if (set_appinfo_node_kobject == NULL) {
		pr_err("%s: create set_app_info folder error", __func__);
		return HW_FUNC_RET_FAIL;
	}

	ret = sysfs_create_file(set_appinfo_node_kobject, &sys_set_appinfo_init.attr);
	if (ret != 0) {
		pr_err("%s: init set_appinfo_node_kobject file fail", __func__);
		return HW_FUNC_RET_FAIL;
	}

	return HW_FUNC_RET_SUCC;
}

static int app_info_proc_show(struct seq_file *m, void *v)
{
	struct info_node *node = NULL;

	list_for_each_entry(node, &app_info_list, entry) {
		seq_printf(m, "%-32s:%32s\n", node->name, node->value);
	}

	return 0;
}

static int sensor_info_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, app_info_proc_show, NULL);
}

static const struct file_operations sensor_info_proc_fops =
{
	.open = sensor_info_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init proc_sensor_info_init(void)
{
	proc_create("hw_sensor_info", 0, NULL, &sensor_info_proc_fops);
	app_info_node_init();
	memset(sensor_type, '\0', sizeof(struct sensor_node) * SENSOR_MAX);
	return 0;
}

fs_initcall(proc_sensor_info_init);

