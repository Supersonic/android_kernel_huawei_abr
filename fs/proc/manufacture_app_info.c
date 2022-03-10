/*
 * manufacture_app_info.c
 *
 * set app info API
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

#include <misc/app_info.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <asm-generic/errno-base.h>
#include <misc/ddr_info.h>
#include <linux/string.h>

struct info_node {
	char name[APP_INFO_NAME_LENTH];
	char value[APP_INFO_VALUE_LENTH];
	struct list_head entry;
};

static LIST_HEAD(app_info_list);
static DEFINE_SPINLOCK(app_info_list_lock);

int app_info_set(const char *name, const char *value)
{
	struct info_node *new_node = NULL;
	int name_lenth;
	int value_lenth;

	if (WARN_ON(!name || !value))
		return -EINVAL;

	name_lenth = strlen(name);
	value_lenth = strlen(value);

	new_node = kzalloc(sizeof(*new_node), GFP_KERNEL);
	if (new_node == NULL)
		return -EINVAL;

	memcpy(new_node->name, name,
		((name_lenth > (APP_INFO_NAME_LENTH - 1)) ?
		(APP_INFO_NAME_LENTH - 1) : name_lenth));
	memcpy(new_node->value, value,
		((value_lenth > (APP_INFO_VALUE_LENTH - 1)) ?
		(APP_INFO_VALUE_LENTH - 1) : value_lenth));

	spin_lock(&app_info_list_lock);
	list_add_tail(&new_node->entry, &app_info_list);
	spin_unlock(&app_info_list_lock);

	return 0;
}
EXPORT_SYMBOL(app_info_set);

static int app_info_proc_show(struct seq_file *m, void *v)
{
	struct info_node *node = NULL;

	if (WARN_ON(!m || !v))
		return -EINVAL;

	list_for_each_entry(node, &app_info_list, entry)
		seq_printf(m, "%-32s:%32s\n", node->name, node->value);
	return 0;
}

int app_info_proc_get(const char *name, char *value)
{
    char *q;
    unsigned int i = 0;
    struct info_node *node = NULL;

    if (name == NULL || value == NULL) {
        return -1;
    }

    list_for_each_entry(node, &app_info_list, entry) {
        q = strstr(node->name, name);
        if (q != NULL) {
            q = node->value;
            while (q[i] != '\0') {
                value[i] = q[i];
                i++;
            }
            value[i] = '\0';
            return 0;
        }
    }

    return -2;
}

static int app_info_proc_open(struct inode *inode, struct file *file)
{
	if (WARN_ON(!inode || !file))
		return -EINVAL;

	return single_open(file, app_info_proc_show, NULL);
}

static const struct file_operations app_info_proc_fops = {
	.open = app_info_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init proc_app_info_init(void)
{
	proc_create("app_info", 0644, NULL, &app_info_proc_fops);
	app_info_print_smem();
	return 0;
}

fs_initcall(proc_app_info_init);
