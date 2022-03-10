/*
 * Copyright (C) 2013 Huawei Device Co.Ltd
 * License terms: GNU General Public License (GPL) version 2
 *
 */
#include "pil_q6v5_mss_log.h"
#include <linux/aio.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/time.h>
#include <linux/timekeeping.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
#include <linux/uio.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0)
#include <uapi/linux/fs.h>
#endif
#include <uapi/linux/uio.h>
#include <soc/qcom/subsystem_restart.h>
#include "pil_q6v5_mss_ultils.h"

#define DATA_LOG_DIR "/data/log/modem_crash/"
#define MODEM_EXCEPTION "modem_exception"
#define MODEM_CRASH_LOG "modem_crash_log"
#define MODEM_F3_TRACE "modem_f3_trace"
#define HUAWEI_STR "huawei"
#define HUAWEI_REQUEST_STR "cm_hw_request_modem_reset"
#define SSR_REASON_LEN (250U)
#define ERR_DATA_MAX_SIZE (0x4000U)
#define DIAG_F3_TRACE_BUFFER_SIZE (0x4000U)
#define MAX_PATH_LEN (255)
#define MAX_CRASH_RECORD_NUM 50
#define TIMEBUF_SIZE 21
#define MAX_RECORD_NUM 50
#define MAX_RECORD_LENGTH 512
#define LEFT_NUMBER_FOR_NEWLINE 4

/* define work data sturct*/
struct work_data{
    struct work_struct log_modem_work; //WORK
    struct workqueue_struct *log_modem_work_queue; //WORK QUEUE
    char reset_reason[SSR_REASON_LEN];
    char crash_log_valid;
    char crash_log[ERR_DATA_MAX_SIZE];
    char f3_trace_log_valid;
    char f3_trace_log[DIAG_F3_TRACE_BUFFER_SIZE + 4];
};

//struct work_struct log_modem_work; //used to fill the temp work
static struct work_data *g_work_data = NULL;
static DEFINE_MUTEX(lock);

char *get_current_timestamp(char *buf, int len)
{
    struct tm tm;
    int length;
    if (buf == NULL || len < TIMEBUF_SIZE) {
        pr_err("Invalid timestamp buffer");
        goto get_timestamp_error;
    }
    time64_t now = ktime_get_real_seconds();
    time64_to_tm(now, 0, &tm);
    // default datae is from 1900 years
    length =snprintf(buf, TIMEBUF_SIZE,
        "%04d-%02d-%02d_%02d-%02d-%02d ", tm.tm_year + 1900,
        tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
        tm.tm_sec);
    pr_err("get time length %d,time %s", length, buf);
    return buf;

get_timestamp_error:
    return NULL;
}

/**
 *  name: the name of this command
 *  msg: concrete command string to write to /dev/log/exception
 *  return: on success return the bytes writed successfully, on error return -1
 *
 */
static int store_exception(char *path, char* name, char* msg, int len)
{
    static int count = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
    struct iov_iter iter;
    unsigned long vcount = 0;
    struct iovec vec[2];
#else
    mm_segment_t oldfs;
    struct iovec vec;
#endif
    struct file *file;
    int ret = 0;
    char log_path[MAX_PATH_LEN] = {0};
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0)
    int flags = 0;
    flags |= RWF_SYNC;
#endif
    char data[MAX_RECORD_LENGTH];
    char* line = "\n";
    int data_length;

    if(NULL == path || NULL == name || NULL == msg)
    {
        pr_err("store_exception: the arguments invalidate\n");
        return -1;
    }

    if (get_current_timestamp(data, TIMEBUF_SIZE) == NULL) {
        pr_err("store_exception: the arguments invalidate\n");
        strlcpy(data, msg, MAX_RECORD_LENGTH - LEFT_NUMBER_FOR_NEWLINE);
    } else {
        strlcat(data, msg, MAX_RECORD_LENGTH - LEFT_NUMBER_FOR_NEWLINE);
    }
    strlcat(data, line, MAX_RECORD_LENGTH);
    snprintf(log_path, sizeof(log_path), "%s%s", path, name);
    pr_info("store_exception: log_path:%s  msg:%s\n", log_path, data);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0))
    oldfs = get_fs();
    set_fs(KERNEL_DS);
#endif
    if (count >= MAX_CRASH_RECORD_NUM) {
        // 0664 represetnt (420)(420)(400), rw-rw-r--
        file = filp_open(log_path, O_CREAT | O_RDWR | O_TRUNC, 0664);
        count = 0;
        pr_info("count exceed limit %d\n", count);
    } else {
        file = filp_open(log_path, O_CREAT | O_RDWR | O_APPEND, 0664);
        pr_info("count beyond limit %d\n", count);
    }
    if(!file || IS_ERR(file))
    {
        pr_err("store_exception: Failed to access\n");
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0))
        set_fs(oldfs);
#endif
        return -1;
    }

    mutex_lock(&lock);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
    vec[vcount].iov_base  = data;
    vec[vcount++].iov_len = strlen(data);
#elif
    vfs_truncate(&file->f_path,  0);
    vec.iov_base = data;
    vec.iov_len = strlen(data);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
    iov_iter_init(&iter, WRITE, vec, vcount, iov_length(vec, vcount));
    ret = vfs_iter_write(file, &iter, &file->f_pos, flags);
#elif INUX_VERSION_CODE >= KERNEL_VERSION(4,9,0)
    ret = vfs_writev(file, &vec, 1, &file->f_pos, flags);
#else
    ret = vfs_writev(file, &vec, 1, &file->f_pos);
#endif
    mutex_unlock(&lock);

    if(ret < 0)
    {
        pr_err("store_exception: Failed to write to /dev/log/\n");
        filp_close(file, NULL);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0))
        set_fs(oldfs);
#endif
        return -1;
    }
    count++;
    pr_info("store_exception: success\n");
    filp_close(file, NULL);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0))
    set_fs(oldfs);
#endif
    return ret;
}

/*the queue work handle, store the modem reason to the exception file*/
static void log_modem_work_func(struct work_struct *work)
{
    struct work_data *work_data_self = container_of(work, struct work_data, log_modem_work);
    if(NULL == work_data_self)
    {
        pr_err("[log_modem_reset]work_data_self is NULL!\n");
        return;
    }
    store_exception(DATA_LOG_DIR, MODEM_EXCEPTION, work_data_self->reset_reason, strlen(work_data_self->reset_reason));
    if(g_work_data->crash_log_valid){
        store_exception(DATA_LOG_DIR, MODEM_CRASH_LOG, work_data_self->crash_log, sizeof(work_data_self->crash_log));
        g_work_data->crash_log_valid = false;
    }

    if(g_work_data->f3_trace_log_valid){
        store_exception(DATA_LOG_DIR, MODEM_F3_TRACE, work_data_self->f3_trace_log, sizeof(work_data_self->f3_trace_log));
        g_work_data->f3_trace_log_valid = false;
    }

    pr_info("[log_modem_reset]log_modem_reset_work after write exception inode work_data_self->reset_reason=%s \n",work_data_self->reset_reason);
    g_work_data->reset_reason[0] = '\0';
}

static void InsertModemLogQueue(void)
{
    if(NULL == g_work_data->log_modem_work_queue){
        pr_err("[log_modem_reset]log_modem_reset_work_queue is NULL!\n");
        return;
    }

    pr_info("[log_modem_reset]modem reset reason inserted the log_modem_reset_queue \n");
    queue_work(g_work_data->log_modem_work_queue, &(g_work_data->log_modem_work));
    return;
}

static void save_modem_err_log(int type)
{
    size_t size;
    char *modem_log;

    modem_log = qcom_smem_get(QCOM_SMEM_HOST_ANY, type, &size);
    if (IS_ERR(modem_log) || !size) {
        pr_err("[log_modem_reset]log_modem_log %d failure reason: (unknown, smem_get_entry_no_rlock failed).\n", type);
        return;
    }

    if (!modem_log[0]) {
        pr_err("[log_modem_reset]log_modem_log %d failure reason: (unknown, empty string found).\n", type);
        return;
    }

    if (type == SMEM_ERR_F3_TRACE_LOG) {
        memcpy(g_work_data->f3_trace_log, modem_log, DIAG_F3_TRACE_BUFFER_SIZE - 1);
        g_work_data->f3_trace_log_valid = true;
    }
    if (type == SMEM_ERR_CRASH_LOG) {
        memcpy(g_work_data->crash_log, modem_log, ERR_DATA_MAX_SIZE - 1);
        g_work_data->crash_log_valid = true;
    }

    modem_log[0] = '\0';
    wmb();
}

void save_modem_reset_log(char reason[], int reasonLength)
{
    int is_workqueue_pending;

    if (strstr(reason, HUAWEI_STR) || strstr(reason, HUAWEI_REQUEST_STR)) {
        pr_err("reset modem subsystem by huawei\n");
        return;
    }

    is_workqueue_pending = work_pending(&g_work_data->log_modem_work);
    if (is_workqueue_pending) {
        pr_err("log modem reset work queue is pending, ignore current ones\n");
        return;
    }

    strncpy(g_work_data->reset_reason,reason,SSR_REASON_LEN - 1);

    save_modem_err_log(SMEM_ERR_F3_TRACE_LOG);
    save_modem_err_log(SMEM_ERR_CRASH_LOG);

    InsertModemLogQueue();
    pr_info("[log_modem_reset]put done \n");
    return;
}


/*creat the queue that log the modem reset reason*/
int create_modem_log_queue(void)
{
    int error = 0;
    int workDataSize;

    workDataSize = sizeof(struct work_data);

    g_work_data = kzalloc(workDataSize,GFP_KERNEL);
    if (NULL == g_work_data) {
        error = -ENOMEM;
        pr_err("[log_modem_reset]work_data_temp is NULL, Don't log this!\n");
        return error;
    }

    memset(g_work_data, 0, workDataSize);
    INIT_WORK(&(g_work_data->log_modem_work), log_modem_work_func);

    g_work_data->log_modem_work_queue = create_singlethread_workqueue("log_modem_reset");
    if (NULL == g_work_data->log_modem_work_queue ) {
        error = -ENOMEM;
        pr_err("[log_modem_reset]log modem reset queue created failed!\n");
        kfree(g_work_data);
        return error;
    }

    pr_info("[log_modem_reset]log modem reset queue created success \n");
    return error;
}

void destroy_modem_log_queue(void)
{
    if (g_work_data) {
        kfree(g_work_data);
        g_work_data = NULL;
    }
}
