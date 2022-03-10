/*
 * kernel/power/sys_sync.c.c
 *
 * User space wakeup sources support.
 *
 * Copyright (C) 2012 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This code is based on the analogous interface allowing user space to
 * manipulate wakelocks on Android.
 */

#include <linux/syscalls.h> /* sys_sync */
#include <linux/suspend.h>
static int suspend_sys_sync_count;
static DEFINE_SPINLOCK(suspend_sys_sync_lock);
struct workqueue_struct *suspend_sys_sync_work_queue;
struct completion suspend_sys_sync_comp;

static void suspend_sys_sync(struct work_struct *work)
{

	pr_info("PM: Syncing filesystems...\n");
	sys_sync();
	pr_info("sync done.\n");

	spin_lock(&suspend_sys_sync_lock);
	suspend_sys_sync_count--;
	spin_unlock(&suspend_sys_sync_lock);
}
static DECLARE_WORK(suspend_sys_sync_work, suspend_sys_sync);

void suspend_sys_sync_queue(void)
{
	int ret;
	pr_debug("suspend_sys_sync_count=%d in suspend_sys_sync_queue\n",suspend_sys_sync_count);

	if(NULL == suspend_sys_sync_work_queue)
	{
		pr_info("queue is null in suspend_sys_sync_queue \n");
		suspend_sys_sync_work_queue = create_singlethread_workqueue("suspend_sys_sync");
		spin_lock(&suspend_sys_sync_lock);
		suspend_sys_sync_count = 0;
		spin_unlock(&suspend_sys_sync_lock);
	}

	spin_lock(&suspend_sys_sync_lock);
	if(NULL != suspend_sys_sync_work_queue)
	{
		ret = queue_work(suspend_sys_sync_work_queue, &suspend_sys_sync_work);
		if (ret)
			suspend_sys_sync_count++;
		spin_unlock(&suspend_sys_sync_lock);
	}
	else
	{
		suspend_sys_sync_count = 0;
		spin_unlock(&suspend_sys_sync_lock);
		pr_info("suspend_sys_sync_work_queue is NULL in suspend_sys_sync_queue\n");
		pr_info("begin sys_sync");
		sys_sync();
		pr_info("done sys_sync in suspend_sys_sync_queue");
	}

}

static bool suspend_sys_sync_abort;
static void suspend_sys_sync_handler(unsigned long);
static DEFINE_TIMER(suspend_sys_sync_timer, suspend_sys_sync_handler, 0, 0);
/* value should be less then half of input event wake lock timeout value
 * which is currently set to 5*HZ (see drivers/input/evdev.c)
 */
#define SUSPEND_SYS_SYNC_TIMEOUT (HZ/4)
static void suspend_sys_sync_handler(unsigned long arg)
{
	if (suspend_sys_sync_count == 0) {
		complete(&suspend_sys_sync_comp);
	} else if (pm_wakeup_pending()) {
		suspend_sys_sync_abort = true;
		complete(&suspend_sys_sync_comp);
	} else {
		mod_timer(&suspend_sys_sync_timer, jiffies +
				SUSPEND_SYS_SYNC_TIMEOUT);
	}
}

int suspend_sys_sync_wait(void)
{
	suspend_sys_sync_abort = false;

	if (suspend_sys_sync_count != 0) {
		mod_timer(&suspend_sys_sync_timer, jiffies +
				SUSPEND_SYS_SYNC_TIMEOUT);
		pr_debug("before wait signal in suspend_sys_sync_wait \n");
		wait_for_completion(&suspend_sys_sync_comp);
		pr_debug("wait success suspend_sys_sync_wait \n");
	}
	if (suspend_sys_sync_abort) {
		pr_info("suspend aborted....while waiting for sys_sync\n");
		return -EAGAIN;
	}

	return 0;
}

int suspend_sync_work_queue_init(void)
{
	init_completion(&suspend_sys_sync_comp);
	suspend_sys_sync_work_queue =
		create_singlethread_workqueue("suspend_sys_sync");
	if (suspend_sys_sync_work_queue == NULL) {
		return -ENOMEM;
	}

	return 0;
}

