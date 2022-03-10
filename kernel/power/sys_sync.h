
#ifndef LINUX_POWER_SYSSYNC_H
#define LINUX_POWER_SYSSYNC_H

extern int suspend_sys_sync_wait(void);
extern void suspend_sys_sync_queue(void);
extern int suspend_sync_work_queue_init(void);

#endif