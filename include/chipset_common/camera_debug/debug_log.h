#ifndef _CAMERA_DEBUG_LOG_H_
#define _CAMERA_DEBUG_LOG_H_

#define LOG_TAG "CAMERA_DEBUG"

#define PRINT_DEBUG 1
#define PRINT_INFO  1
#define PRINT_WARN  1
#define PRINT_ERR   1

#if PRINT_DEBUG
#define debug_dbg(fmt, ...) printk(KERN_INFO "[" LOG_TAG "]" "[D] %s:" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define debug_dbg(fmt, ...) ((void)0)
#endif

#if PRINT_INFO
#define debug_info(fmt, ...) printk(KERN_INFO "[" LOG_TAG "]" "[INFO] %s:" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define debug_info(fmt, ...) ((void)0)
#endif

#if PRINT_WARN
#define debug_warn(fmt, ...) printk(KERN_WARNING "[" LOG_TAG "]" "[WARN] %s:" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define debug_warn(fmt, ...) ((void)0)
#endif

#if PRINT_ERR
#define debug_err(fmt, ...) printk(KERN_ERR "[" LOG_TAG "]" "[ERR] %s:" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define debug_err(fmt, ...) ((void)0)
#endif

#endif

