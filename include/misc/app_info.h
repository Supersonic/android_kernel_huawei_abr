#ifndef __APP_INFO_H_
#define __APP_INFO_H_
#define APP_INFO_NAME_LENTH 32
#define APP_INFO_VALUE_LENTH 64

extern int app_info_set(const char * name, const char * value);
int app_info_proc_get(const char *name, char *value);
#endif
