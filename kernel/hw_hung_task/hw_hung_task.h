#ifndef _HW_HUNG_TASK_H
#define _HW_HUNG_TASK_H
int hw_hungtask_check(struct task_struct *t);
void hw_hungtask_findpname(struct task_struct *t);
unsigned int hw_hung_task_flag(void);
int hw_hungtask_create_proc(void);
#endif