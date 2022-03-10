#include "huawei-rtc.h"
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/vmalloc.h>
#include <linux/soc/qcom/smem.h>

/* use to clear the offset when the rtc is outage*/
#define RTC_MIN_VALUE_TO_RESET_OFFSET 60
#define BSPINFO_RAW_PART_PATH "/dev/block/bootdevice/by-name/bsp_info"
#define PART_FULL_PATH_MAX_LEN 256
#define RTC_PART_OFFSET 0

#define OFFSET_TAG_SIZE 8
#define OFFSET_TAG "RTC"

#define CLEAR_OFFSET_DELAY_TIME 20000
#define WAIT_NODE_DELAY_TIME    1000
#define NODE_RETRY_TIMES        50

typedef struct {
	char off_tag[OFFSET_TAG_SIZE];
	uint64_t offset;
}Offset_Info;

static Offset_Info offset_info;
static const struct rtc_class_ops* qpnp_rtc_rw_ops;
static struct delayed_work write_offset_delay_work;  /* work to write offset */
struct workqueue_struct *rtc_off_work_queue;

static long write_data_to_file(int fd, char *buf, size_t buf_size)
{
	mm_segment_t old_fs;
	long bytes_total_to_rw = (long)buf_size;
	long bytes_total_rw = 0L;
	long bytes_this_time = 0L;
	char *ptemp = buf;

	if (unlikely((fd < 0) || (NULL == buf)))
	{
		pr_err("func [%s] invalid parameter, fd: %d buf: %p\n",__func__, fd, (void *)buf);
		return -1;
	}

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	while (bytes_total_to_rw > 0)
	{
		bytes_this_time = ksys_write(fd, ptemp, bytes_total_to_rw);
		if (bytes_this_time < 0)
		{
			pr_err("func [%s], Failed to write file! \n", __func__);
			break;
		}
		ptemp += bytes_this_time;
		bytes_total_to_rw -= bytes_this_time;
		bytes_total_rw += bytes_this_time;
	}
	set_fs(old_fs);

	return bytes_total_rw;
}

static int
write_emmc_raw_part(const char *dev_path,
	uint64_t offset,
	char *buf,
	uint64_t buf_size)
{
	int fd = -1;
	int ret = -1;
	off_t seek_result = (off_t)0;
	long bytes_written = 0L;
	mm_segment_t fs = 0;

	if (unlikely(NULL == dev_path) || unlikely(NULL == buf))
	{
		pr_err("func [%s] invalid parameter, dev_path:%p buf: %p\n",__func__, dev_path, buf);
		return -1;
	}

	fs = get_fs();
	set_fs(KERNEL_DS);

	/* 1. open file for writing */
	fd = ksys_open(dev_path, O_WRONLY, 0);
	if (fd < 0)
	{
		pr_err("func [%s] Open file [%s] failed! fd: %d\n",__func__, dev_path, fd);
		goto __out;
	}

	/* 2. seek file to the valid position */
	seek_result = ksys_lseek(fd, (off_t)offset, SEEK_SET);
	if ((off_t)offset != seek_result)
	{
		pr_err("func [%s] lseek file [%s] offset failed!seek_result: %ld, it should be: %ld\n",
			__func__, dev_path, (long)seek_result, (long)offset);
		goto __out;
	}

	/* 3. write data to file */
	bytes_written = write_data_to_file(fd, buf, buf_size);
	if ((long)buf_size != bytes_written)
	{
		pr_err("func [%s] write file [%s] failed!bytes_written: %ld, it shoule be: %ld\n",
			__func__, dev_path, (long)bytes_written, (long)buf_size);
		goto __out;
	}

	/* 4. write successfully, modify the value of ret */
	ret = 0;

__out:
	if (fd >= 0)
	{
		ksys_sync();
		ksys_close(fd);
	}

	set_fs(fs);

	return ret;
}

static void 
write_offset_to_raw_part(struct work_struct *work){
	int ret = -1;
	int fd = -1;
	int retry_times = NODE_RETRY_TIMES;

	while (retry_times) {
		fd = ksys_access(BSPINFO_RAW_PART_PATH, 0);
		if (fd < 0)
			pr_err("func [%s] Open [%s] failed! retry_times: %d\n",
				__func__, BSPINFO_RAW_PART_PATH, retry_times);
		else
			break;
		set_current_state(TASK_INTERRUPTIBLE);
		(void)schedule_timeout(msecs_to_jiffies(WAIT_NODE_DELAY_TIME));
		retry_times--;
	};
	pr_info("%s(): write the offset %llu to pad0 !\n", __func__, offset_info.offset);
	ret = write_emmc_raw_part(BSPINFO_RAW_PART_PATH, RTC_PART_OFFSET,
				(char *)&offset_info, sizeof(Offset_Info));
	if (0 != ret)
	{
		pr_err("write %s failed ret: %d\n", BSPINFO_RAW_PART_PATH, ret);
	}
}

static int
huawei_rtc_read_time(struct device *dev, struct rtc_time *tm){
	int ret = -1;
	uint64_t qpnp_rtc_secs, huawei_rtc_secs = 0;
	struct rtc_time qpnp_rtc_time;

	if(!qpnp_rtc_rw_ops){
		dev_err(dev, "func %s: qpnp_rtc_rw_ops is NULL !\n",__func__);
		return -1;
	}
	if(!qpnp_rtc_rw_ops->read_time){
		dev_err(dev, "func %s: qpnp_rtc_rw_ops->read_time is NULL !\n",__func__);
		return -1;
	}
	ret = qpnp_rtc_rw_ops->read_time(dev, &qpnp_rtc_time);
	if(0 != ret){
		dev_err(dev, "func %s: read rtc time from qpnp_rtc failed !\n",__func__);
		return ret;
	}
	rtc_tm_to_time(&qpnp_rtc_time, (unsigned long *)&qpnp_rtc_secs);

	huawei_rtc_secs = qpnp_rtc_secs + offset_info.offset;
	rtc_time_to_tm(huawei_rtc_secs, tm);

	ret = rtc_valid_tm(tm);
	if (ret) {
		dev_err(dev, "Invalid time read from Huawei RTC\n");
		return ret;
	}

	return ret;
}

static int
huawei_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	int ret = 0;
	uint64_t secs, qpnp_rtc_secs;
	struct rtc_time qpnp_rtc_time;

	ret = rtc_valid_tm(tm);
	if (ret) {
		dev_err(dev, "func %s: Invalid time used to set huawei rtc alarm\n", __func__);
		return ret;
	}

	rtc_tm_to_time(tm, (unsigned long *)&secs);

	dev_info(dev, "func %s: the setting time is h:m:s == %d:%d:%d, d/m/y = %d/%d/%d\n",
			__func__,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec,
			tm->tm_mday,
			tm->tm_mon + 1,
			tm->tm_year);

	if(!qpnp_rtc_rw_ops){
		dev_err(dev, "func %s: qpnp_rtc_rw_ops is NULL !\n",__func__);
		return -1;
	}
	if(!qpnp_rtc_rw_ops->read_time){
		dev_err(dev, "func %s: qpnp_rtc_rw_ops->read_time is NULL !\n",__func__);
		return -1;
	}

	qpnp_rtc_rw_ops->read_time(dev, &qpnp_rtc_time);
	rtc_tm_to_time(&qpnp_rtc_time, (unsigned long *)&qpnp_rtc_secs);

	if( offset_info.offset == (secs - qpnp_rtc_secs)){
		dev_info(dev, "func %s: the offset is not changed, need not to update offset !\n", __func__);
		return ret;
	}

	offset_info.offset = secs - qpnp_rtc_secs;
	if (delayed_work_pending(&write_offset_delay_work)){
		dev_info(dev, "func %s: There is a work pending in rtc_off_work_queue \n", __func__);
		if(!cancel_delayed_work(&write_offset_delay_work)){
			dev_err(dev, "func %s: cancel_delayed_work write_offset_delay_work failed ! \n", __func__);
		}
		flush_workqueue(rtc_off_work_queue);
	}
	queue_delayed_work(rtc_off_work_queue, &write_offset_delay_work, 0);

	return ret;
}

static int
huawei_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	int ret;
	uint64_t secs, secs_rtc;
	struct rtc_time* rtc_tm;

	rtc_tm = &alarm->time;
	ret = rtc_valid_tm(rtc_tm);
	if (ret) {
		dev_err(dev, "func %s: Invalid time used to set huawei rtc alarm\n", __func__);
		return ret;
	}

	dev_info(dev, "%s set for h:r:s=%d:%d:%d, d/m/y=%d/%d/%d\n", __func__,
			alarm->time.tm_hour, alarm->time.tm_min,
			alarm->time.tm_sec, alarm->time.tm_mday,
			alarm->time.tm_mon+1, alarm->time.tm_year);

	rtc_tm_to_time(rtc_tm, (unsigned long *)&secs);
	secs_rtc = secs - offset_info.offset;
	rtc_time_to_tm(secs_rtc, rtc_tm);

	ret = rtc_valid_tm(rtc_tm);
	if (ret) {
		dev_err(dev, "func %s: Invalid time used to set qpnp rtc alarm\n", __func__);
		return ret;
	}

	if(!qpnp_rtc_rw_ops){
		dev_err(dev, "func %s: qpnp_rtc_rw_ops is NULL !\n",__func__);
		return -1;
	}
	if(!qpnp_rtc_rw_ops->set_alarm){
		dev_err(dev, "func %s: qpnp_rtc_rw_ops->set_alarm is NULL !\n",__func__);
		return -1;
	}

	ret = qpnp_rtc_rw_ops->set_alarm(dev, alarm);
	if (ret) {
		dev_err(dev, "func %s: qpnp rtc set alarm failed !\n", __func__);
		return ret;
	}

	return ret;
}

static int
huawei_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	int ret;
	uint64_t secs, secs_rtc;
	struct rtc_time* rtc_tm;

	if(!qpnp_rtc_rw_ops){
		dev_err(dev, "func %s: qpnp_rtc_rw_ops is NULL !\n",__func__);
		return -1;
	}
	if(!qpnp_rtc_rw_ops->read_alarm){
		dev_err(dev, "func %s: qpnp_rtc_rw_ops->read_alarm is NULL !\n",__func__);
		return -1;
	}
	ret = qpnp_rtc_rw_ops->read_alarm(dev, alarm);
	if (ret) {
		dev_err(dev, "func %s: qpnp rtc read alarm failed !\n", __func__);
		return ret;
	}

	rtc_tm = &alarm->time;
	ret = rtc_valid_tm(rtc_tm);
	if (ret) {
		dev_err(dev, "func %s: Invalid time that read from qpnp rtc.\n", __func__);
		return ret;
	}

	rtc_tm_to_time(rtc_tm, (unsigned long *)&secs);
	secs_rtc = secs + offset_info.offset;
	rtc_time_to_tm(secs_rtc, rtc_tm);

	ret = rtc_valid_tm(rtc_tm);
	if (ret) {
		dev_err(dev, "func %s: Invalid time that read from huawei rtc\n", __func__);
		return ret;
	}

	return ret;
}

static struct rtc_class_ops huawei_rtc_rw_ops;

const struct rtc_class_ops* get_huawei_rtc_rw_ops(const struct rtc_class_ops *qpnp_rtc_rw_ops_ptr){

	struct rtc_class_ops *huawei_rtc_rw_ops_ptr;
	if(NULL == qpnp_rtc_rw_ops_ptr){
		pr_err("%s: the parameter qpnp_rtc_rw_ops_ptr is NULL\n", __func__);
		return NULL;
	}

	qpnp_rtc_rw_ops = qpnp_rtc_rw_ops_ptr;

	huawei_rtc_rw_ops_ptr = &huawei_rtc_rw_ops;
	memcpy(huawei_rtc_rw_ops_ptr, qpnp_rtc_rw_ops_ptr, sizeof(struct rtc_class_ops));
	huawei_rtc_rw_ops_ptr->read_time = huawei_rtc_read_time;
	huawei_rtc_rw_ops_ptr->set_time = huawei_rtc_set_time;
	huawei_rtc_rw_ops_ptr->set_alarm = huawei_rtc_set_alarm;
	huawei_rtc_rw_ops_ptr->read_alarm = huawei_rtc_read_alarm;

	pr_info("%s: get huawei rtc rw ops finished !\n", __func__);
	return huawei_rtc_rw_ops_ptr;
}
EXPORT_SYMBOL(get_huawei_rtc_rw_ops);

int huawei_rtc_init(struct device *dev){
	int ret = -1;
	uint64_t qpnp_rtc_secs;
	struct rtc_time qpnp_rtc_time;
	struct smem_exten_huawei_paramater *smem = NULL;
	size_t size = sizeof(*smem);

	if(NULL == dev){
		pr_err("%s: Device pointer is null !\n", __func__);
		return -1;
	}

	/*init the rtc_off_work_queue*/
	rtc_off_work_queue = create_workqueue("write_offset_delay_workqueue");
	INIT_DELAYED_WORK(&write_offset_delay_work, write_offset_to_raw_part);

	ret = qcom_smem_alloc(QCOM_SMEM_HOST_ANY, SMEM_ID_VENDOR1, size);
	if (ret < 0 && ret != -EEXIST) {
		pr_err("%s: qcom_smem_alloc failed, %d\n", __func__, ret);
		return ret;
	}
	smem = qcom_smem_get(QCOM_SMEM_HOST_ANY, SMEM_ID_VENDOR1, &size);
	if (NULL == smem)
	{
		pr_err("%s: SMEM Error, READING RTC OFFSET INFO \n", __func__);
		return -1;
	}

	memcpy(&offset_info, smem->rtc_offset_info, sizeof(Offset_Info));
	/* verify the off_tag in raw partition pad0*/
	if(strcmp(OFFSET_TAG, offset_info.off_tag)){
		pr_err("The OFFSET TAG verify failed ! off_tag is %s \n", offset_info.off_tag);
		memset(&offset_info, 0, sizeof(Offset_Info));
		strcpy(offset_info.off_tag, OFFSET_TAG);
		offset_info.offset = 0;
		return -1;
	}

	/* if the qpnp rtc is abnormal power off, clear the offset */
	if(!qpnp_rtc_rw_ops){
		dev_err(dev, "func %s: qpnp_rtc_rw_ops is NULL !\n",__func__);
		return -1;
	}
	if(!qpnp_rtc_rw_ops->read_time){
		dev_err(dev, "func %s: qpnp_rtc_rw_ops->read_time is NULL !\n",__func__);
		return -1;
	}
	ret = qpnp_rtc_rw_ops->read_time(dev, &qpnp_rtc_time);
	if(0 != ret){
		dev_err(dev, "func %s: read rtc time from qpnp_rtc failed !\n",__func__);
		return ret;
	}
	rtc_tm_to_time(&qpnp_rtc_time, (unsigned long *)&qpnp_rtc_secs);
	pr_info("func %s: The qpnp rtc seconds is %lld \n", __func__, qpnp_rtc_secs);
	if(qpnp_rtc_secs < RTC_MIN_VALUE_TO_RESET_OFFSET){
		memset(&offset_info, 0, sizeof(Offset_Info));
		strcpy(offset_info.off_tag, OFFSET_TAG);
		offset_info.offset = 0;

		queue_delayed_work(rtc_off_work_queue, &write_offset_delay_work, msecs_to_jiffies(CLEAR_OFFSET_DELAY_TIME));
		return -1;
	}

	pr_info("%s: the init rtc offset is %llu\n", __func__, offset_info.offset);

	return 0;
}
EXPORT_SYMBOL(huawei_rtc_init);

int huawei_rtc_remove(struct platform_device *pdev)
{
	cancel_delayed_work_sync(&write_offset_delay_work);
	flush_workqueue(rtc_off_work_queue);
	destroy_workqueue(rtc_off_work_queue);

	return 0;
}
EXPORT_SYMBOL(huawei_rtc_remove);
