#ifndef _DEBUG_CORE_H_
#define _DEBUG_CORE_H_
#include <linux/list.h>
#include <chipset_common/camera_debug/debug_common.h>

struct dev_module {
	struct list_head node;
	int dev_id;
	int type;
	int state;
	char dev_name[DEV_NAME_MAX_LENGTH];
	void (*handle)(struct debug_msg *recv_data, struct debug_msg *send_data);
};

struct function_t {
	enum function_policy policy;
	enum dev_type type;
};

struct platform_adapter {
	struct function_t func_config[FUNCTION_MAX];
	void (*platform_adapter)(struct debug_msg *recv_data, struct debug_msg *send_data);
	struct list_head dev_list;
	struct mutex lock;
};
#endif