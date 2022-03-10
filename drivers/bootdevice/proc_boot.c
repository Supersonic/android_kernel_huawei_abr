#include <linux/bootdevice.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/printk.h>

#define MAX_NAME_LEN 32
#define MAX_PRL_LEN 5
#define MAX_DIEID_LEN 800

struct __bootdevice {
	enum bootdevice_type type;
	const struct device *dev;
	sector_t size;
	u32 cid[4];
	char product_name[MAX_NAME_LEN + 1];
	u8 pre_eol_info;
	u8 life_time_est_typ_a;
	u8 life_time_est_typ_b;
	unsigned int manfid;
	char fw_version[MAX_PRL_LEN + 1];
	char hufs_dieid[MAX_DIEID_LEN + 1];
};

static struct __bootdevice bootdevice;

void set_bootdevice_type(enum bootdevice_type type)
{
	bootdevice.type = type;
}

enum bootdevice_type get_bootdevice_type(void)
{
	return bootdevice.type;
}

void set_bootdevice_name(enum bootdevice_type type, struct device *dev)
{
	if (get_bootdevice_type() == type)
		bootdevice.dev = dev;
}

static int type_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", bootdevice.type);
	return 0;
}

static int type_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, type_proc_show, NULL);
}

static const struct file_operations type_proc_fops = {
	.open = type_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int name_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", dev_name(bootdevice.dev));
	return 0;
}

static int name_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, name_proc_show, NULL);
}

static const struct file_operations name_proc_fops = {
	.open = name_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

void set_bootdevice_size(enum bootdevice_type type, sector_t size)
{
	if (get_bootdevice_type() == type)
		bootdevice.size = size;
}

static int size_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%llu\n", (unsigned long long)bootdevice.size);
	return 0;
}

static int size_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, size_proc_show, NULL);
}

static const struct file_operations size_proc_fops = {
	.open = size_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

void set_bootdevice_cid(enum bootdevice_type type, u32 *cid)
{
	if (get_bootdevice_type() == type)
		memcpy(bootdevice.cid, cid, sizeof(bootdevice.cid));
}

static int cid_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%08x%08x%08x%08x\n", bootdevice.cid[0],
		   bootdevice.cid[1], bootdevice.cid[2], bootdevice.cid[3]);

	return 0;
}

static int cid_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cid_proc_show, NULL);
}

static const struct file_operations cid_proc_fops = {
	.open = cid_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

void set_bootdevice_fw_version(enum bootdevice_type type, char *fw_version)
{
	if (get_bootdevice_type() == type)
		strlcpy(bootdevice.fw_version, fw_version, sizeof(bootdevice.fw_version));
}

static int fw_version_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", bootdevice.fw_version);

	return 0;
}

static int fw_version_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fw_version_proc_show, NULL);
}

static const struct file_operations fw_version_proc_fops = {
	.open		= fw_version_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void set_bootdevice_hufs_dieid(enum bootdevice_type type, char *hufs_dieid)
{
	if (get_bootdevice_type() == type)
		strlcpy(bootdevice.hufs_dieid, hufs_dieid, sizeof(bootdevice.hufs_dieid));
}

static int hufs_dieid_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", bootdevice.hufs_dieid);

	return 0;
}

static int hufs_dieid_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, hufs_dieid_proc_show, NULL);
}

static const struct file_operations hufs_dieid_proc_fops = {
	.open		= hufs_dieid_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void set_bootdevice_product_name(enum bootdevice_type type, char *product_name)
{
	if (get_bootdevice_type() == type)
		strlcpy(bootdevice.product_name, product_name, sizeof(bootdevice.product_name));
}

void get_bootdevice_product_name(char *product_name, u32 len)
{
	u32 min_len = min_t(uint8_t, len, MAX_NAME_LEN);

	if (min_len == 0) {
		product_name[0] = '\0';
		return;
	}

	strlcpy(product_name, bootdevice.product_name, min_len);
	product_name[min_len - 1] = '\0';
}

static int product_name_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", bootdevice.product_name);

	return 0;
}

static int product_name_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, product_name_proc_show, NULL);
}

static const struct file_operations product_name_proc_fops = {
	.open		= product_name_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void set_bootdevice_pre_eol_info(enum bootdevice_type type, u8 pre_eol_info)
{
	if (get_bootdevice_type() == type)
		bootdevice.pre_eol_info = pre_eol_info;
}

static int pre_eol_info_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "0x%02X\n", bootdevice.pre_eol_info);

	return 0;
}

static int pre_eol_info_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, pre_eol_info_proc_show, NULL);
}

static const struct file_operations pre_eol_info_proc_fops = {
	.open		= pre_eol_info_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void set_bootdevice_life_time_est_typ_a(enum bootdevice_type type, u8 life_time_est_typ_a)
{
	if (get_bootdevice_type() == type)
		bootdevice.life_time_est_typ_a = life_time_est_typ_a;
}

static int life_time_est_typ_a_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "0x%02X\n", bootdevice.life_time_est_typ_a);

	return 0;
}

static int life_time_est_typ_a_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, life_time_est_typ_a_proc_show, NULL);
}

static const struct file_operations life_time_est_typ_a_proc_fops = {
	.open		= life_time_est_typ_a_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void set_bootdevice_life_time_est_typ_b(enum bootdevice_type type, u8 life_time_est_typ_b)
{
	if (get_bootdevice_type() == type)
		bootdevice.life_time_est_typ_b = life_time_est_typ_b;
}

static int life_time_est_typ_b_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "0x%02X\n", bootdevice.life_time_est_typ_b);

	return 0;
}

static int life_time_est_typ_b_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, life_time_est_typ_b_proc_show, NULL);
}

static const struct file_operations life_time_est_typ_b_proc_fops = {
	.open		= life_time_est_typ_b_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void set_bootdevice_manfid(enum bootdevice_type type, unsigned int manfid)
{
	if (get_bootdevice_type() == type)
		bootdevice.manfid = manfid;
}

unsigned int get_bootdevice_manfid(void)
{
	return bootdevice.manfid;
}

static int manfid_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "0x%06x\n", bootdevice.manfid);

	return 0;
}

static int manfid_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, manfid_proc_show, NULL);
}

static const struct file_operations manfid_proc_fops = {
	.open		= manfid_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_bootdevice_init(void)
{
	int init_value = 0;

	if (!proc_mkdir("bootdevice", NULL)) {
		pr_err("make proc dir bootdevice failed\n");
		return -EFAULT;
	}

	proc_create("bootdevice/type", init_value, NULL, &type_proc_fops);
	proc_create("bootdevice/name", init_value, NULL, &name_proc_fops);
	proc_create("bootdevice/size", init_value, NULL, &size_proc_fops);
	proc_create("bootdevice/cid", init_value, NULL, &cid_proc_fops);
	proc_create("bootdevice/rev",
		init_value, NULL, &fw_version_proc_fops);
	proc_create("bootdevice/product_name",
		init_value, NULL, &product_name_proc_fops);
	proc_create("bootdevice/pre_eol_info",
		init_value, NULL, &pre_eol_info_proc_fops);
	proc_create("bootdevice/life_time_est_typ_a", init_value, NULL,
		&life_time_est_typ_a_proc_fops);
	proc_create("bootdevice/life_time_est_typ_b", init_value, NULL,
		&life_time_est_typ_b_proc_fops);
	proc_create("bootdevice/manfid", init_value, NULL, &manfid_proc_fops);
	proc_create("bootdevice/hufs_dieid", init_value, NULL, &hufs_dieid_proc_fops);

	return 0;
}
module_init(proc_bootdevice_init);
