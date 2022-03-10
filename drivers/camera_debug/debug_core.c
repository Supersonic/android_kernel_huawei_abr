#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "debug_core.h"
#include <chipset_common/camera_debug/debug_common.h>
#include <chipset_common/camera_debug/debug_log.h>
#include <securec.h>
#define IOCTL_HAL_MSG  _IOWR('I', 0, struct debug_msg)
#define IOCTL_DEBUG_MSG _IOWR('I', 1, struct debug_msg)
struct debug_dev *debug_devp;
struct platform_adapter g_adapter;
struct debug_dev {
	struct miscdevice dev;
	wait_queue_head_t readq;
};

void updata_func_config(struct function_config func_config[], int size)
{
	int i;
	for (i = 0; i < size; i++) {
		g_adapter.func_config[func_config[i].func].policy =
			func_config[i].policy;
		g_adapter.func_config[func_config[i].func].type =
			func_config[i].type;
	}
}
EXPORT_SYMBOL(updata_func_config);

void updata_state_by_position(int position_id, int type, int state)
{
	struct dev_module *dev = NULL;
	debug_dbg("enter");
	mutex_lock(&g_adapter.lock);
	list_for_each_entry(dev, &g_adapter.dev_list, node)
		if (dev->dev_id == position_id && dev->type == type) {
			dev->state = state;
			debug_info("dev_id %d, type %d, state %d",
				dev->dev_id, dev->type, state);
		}
	mutex_unlock(&g_adapter.lock);
}
EXPORT_SYMBOL(updata_state_by_position);

void updata_dev_msg(struct dev_msg_t *dev)
{
	struct dev_module *dev_node = NULL;
	debug_dbg("enter!");
	mutex_lock(&g_adapter.lock);
	list_for_each_entry(dev_node, &g_adapter.dev_list, node) {
		if (dev_node->dev_id == dev->dev_id && dev_node->type == dev->type) {
			mutex_unlock(&g_adapter.lock);
			debug_err("dev id exist");
			return;
		}
	}
	dev_node = (struct dev_module *)kzalloc(sizeof(struct dev_module), GFP_KERNEL);
	if (!dev_node) {
		debug_err("dev kzalloc error");
		return;
	}
	dev_node->dev_id = dev->dev_id;
	dev_node->type = dev->type;
	dev_node->state = -1;
	strncpy_s(dev_node->dev_name, DEV_NAME_MAX_LENGTH, dev->dev_name, DEV_NAME_MAX_LENGTH);
	dev_node->handle = dev->handle;
	list_add(&dev_node->node, &g_adapter.dev_list);
	mutex_unlock(&g_adapter.lock);
}
EXPORT_SYMBOL(updata_dev_msg);

static int recv_hal_msg(struct debug_msg *msg, struct debug_msg *send_msg)
{
	if (!msg) {
		debug_err("hal msg null!");
		return DEBUG_ERR;
	}
	switch (msg->command) {
	// CAMERA_EXIST and SENSOR_STATE processed in adapter
	case CAMERA_EXIST:
		debug_info("CAMERA_EXIST");
	case SENSOR_STATE:
		debug_info("SENSOR_STATE");
		g_adapter.platform_adapter(msg, send_msg);
		break;
	default:
		debug_err("hal command err!");
		return DEBUG_ERR;
	}
	return 0;
}

static void get_camera_exist(struct debug_msg *send_data)
{
	unsigned int i = 0;
	struct dev_module *dev = NULL;
	mutex_lock(&g_adapter.lock);
	list_for_each_entry(dev, &g_adapter.dev_list, node) {
		debug_dbg("id %d, type %d, state %d", dev->dev_id, dev->type, dev->state);
		if (dev->state >= 0 && i < MAX_DEV_COUNT) {
			send_data->u.dev_info[i].dev_id = dev->dev_id;
			send_data->u.dev_info[i].type = dev->type;
			send_data->u.dev_info[i].state = dev->state;
			strncpy_s(send_data->u.dev_info[i].dev_name, DEV_NAME_MAX_LENGTH,
				dev->dev_name, DEV_NAME_MAX_LENGTH);
			i++;
		}
	}
	mutex_unlock(&g_adapter.lock);
}

static void get_dev_msg(struct debug_msg *recv_data, struct debug_msg *send_data)
{
	unsigned int i;
	switch (recv_data->command) {
	case SUPPORT_FUNCTION:
		for (i = 0; i < FUNCTION_MAX; i++) {
			send_data->u.func[i] = g_adapter.func_config[i].policy;
			debug_dbg("policy %d", send_data->u.func[i]);
		}
		break;
	case CAMERA_EXIST:
		get_camera_exist(send_data);
		break;
	default:
		debug_err("cmd error");
	}
}

static int debug_policy(struct debug_msg *recv_data, struct debug_msg *send_data)
{
	struct dev_module *dev = NULL;
	enum function_policy cmd;
	enum dev_type type;

	if (!recv_data || !send_data) {
		debug_err("msg is null!");
		return DEBUG_ERR;
	}

	cmd = g_adapter.func_config[recv_data->command].policy;
	type = g_adapter.func_config[recv_data->command].type;
	switch (cmd) {
	case GET_DEV_DATA: {
		debug_dbg("enter GET_DEV_DATA!");
		get_dev_msg(recv_data, send_data);
		break;
	}
	case GET_HAL_DATA: {
		debug_dbg("enter GET_HAL_DATA!");
		break;
	}
	case GET_DEBUG_DATA: {
		debug_dbg("enter GET_DEBUG_DATA!");
		mutex_lock(&g_adapter.lock);
		list_for_each_entry(dev, &g_adapter.dev_list, node) {
			if (dev->dev_id == recv_data->dev_id && dev->type == type) {
				debug_dbg("get dev!");
				dev->handle(recv_data, send_data);
			}
		}
		mutex_unlock(&g_adapter.lock);
	}
		break;
	default:
		debug_err("policy error!");
		return DEBUG_ERR;
	}

	return DEBUG_OK;
}

static long debug_ioctl(struct file *fd, unsigned int cmd, unsigned long arg)
{
	struct debug_msg recv_data;
	struct debug_msg send_data;
	memset_s(&send_data, sizeof(send_data), 0x00, sizeof(send_data));
	send_data.state = CAMERA_DEBUG_ERR;
	debug_dbg("misc in debug_ioctl");

	if (copy_from_user(&recv_data, (void __user *)arg, _IOC_SIZE(cmd))) {
		debug_err("%s: copy from arg failed", __FUNCTION__);
		return -EFAULT;
	}
	switch (cmd) {
	case IOCTL_HAL_MSG: {
		debug_dbg("hal msg!");
		recv_hal_msg(&recv_data, &send_data);
		break;
	}
	case IOCTL_DEBUG_MSG: {
		debug_dbg("debug msg!");
		debug_policy(&recv_data, &send_data);
		if (copy_to_user((void __user *)arg, &send_data, _IOC_SIZE(IOCTL_DEBUG_MSG))) {
			debug_err("%s: copy to arg failed", __FUNCTION__);
			return -EFAULT;
		}
		break;
	}
	default:
		debug_dbg("cmd error!");
		return 0;
	}
	return 0;
}

static int debug_open(struct inode *inode, struct file *fd)
{
	debug_dbg("misc in debug_open");
	return 0;
}
static int debug_release(struct inode *inode, struct file *filp)
{
	debug_dbg("misc debug_release");
	return 0;
}

static struct file_operations debug_fops =
{
	.owner = THIS_MODULE,
	.unlocked_ioctl = debug_ioctl,
#if CONFIG_COMPAT
	.compat_ioctl = debug_ioctl,
#endif
	.open = debug_open,
	.release = debug_release
};

static struct miscdevice camera_debug_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "camera_debug",
	.fops = &debug_fops,
};

static int __init dev_init(void)
{
	int ret = 0;
	debug_dbg("%s enter", __func__);
	debug_devp = kzalloc(sizeof(struct debug_dev), GFP_KERNEL);
	if (!debug_devp)
		return -ENOMEM;
	debug_devp->dev = camera_debug_misc;
	ret = misc_register(&debug_devp->dev);
	if (ret != 0) {
		debug_err("%s cannot register miscdev err=%d\n", __func__, ret);
		return ret;
	}
	mutex_init(&g_adapter.lock);
	init_waitqueue_head(&debug_devp->readq);
	INIT_LIST_HEAD(&g_adapter.dev_list);
	g_adapter.platform_adapter = NULL;
	debug_info("misc camera_debug initialized!");
	return ret;
}

static void __exit dev_exit(void)
{
	struct list_head *pos = NULL;
	struct list_head *q = NULL;
	struct dev_module *dev = NULL;
	debug_info("%s enter", __func__);
	misc_deregister(&camera_debug_misc);
	mutex_destroy(&g_adapter.lock);
	kfree(debug_devp);
	list_for_each_safe(pos, q, &g_adapter.dev_list) {
		dev = list_entry(pos, struct dev_module, node);
		list_del(pos);
		kfree(dev);
	}
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_DESCRIPTION("CAMERA DEBUG");
MODULE_LICENSE("GPL v2");