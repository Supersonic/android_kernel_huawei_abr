/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2021. All rights reserved.
 * Description:
 * Author: Huawei OS Kernel Lab
 * Create: Fri Jul 02 16:39:27 2021
 */
static int proc_vip_prio_show(struct seq_file *m, void *v)
{
	struct inode *inode = m->private;
	struct task_struct *p = NULL;

	p = get_proc_task(inode);
	if (!p)
		return -ESRCH;

	seq_printf(m, "%u\n", p->vip_prio);

	put_task_struct(p);

	return 0;
}

static int proc_vip_prio_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, proc_vip_prio_show, inode);
}

static ssize_t proc_vip_prio_write(struct file *file, const char __user *buf,
						size_t count, loff_t *offset)
{
	struct task_struct *p = NULL;
	unsigned int prio;
	int err;

	err = kstrtouint_from_user(buf, count, 0, &prio);
	if (err)
		return err;

	p = get_proc_task(file_inode(file));
	if (!p)
		return -ESRCH;

	err = set_vip_prio(p, prio);

	put_task_struct(p);

	return err < 0 ? err : count;
}

static const struct file_operations proc_vip_prio_operations = {
	.open       = proc_vip_prio_open,
	.read       = seq_read,
	.write      = proc_vip_prio_write,
	.llseek     = seq_lseek,
	.release    = single_release,
};
