/*
* ufs_wb_dynamic_switch.c
* ufs write boster feature
*
* copyright (c) huawei technologies co., ltd. 2021. All rights reserved.
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/
#include "ufs_wb_dynamic_switch.h"
#include <linux/platform_device.h>
#include <linux/version.h>
#include <asm/unaligned.h>
#include "ufshcd.h"
#include "ufs_quirks.h"

#include <securec.h>

#ifdef CONFIG_HUAWEI_UFS_WB_RUNTIME
#define CREATE_TRACE_POINTS
#include <trace/events/ufs_wb_trace.h>

EXPORT_TRACEPOINT_SYMBOL_GPL(ufs_wb_runtime);
#endif

#define ufs_attribute(_name, _uname)                                           \
	static ssize_t _name##_show(struct device *dev,                \
			struct device_attribute *attr, char *buf)      \
	{                                                              \
		struct Scsi_Host *shost = class_to_shost(dev);         \
		struct ufs_hba *hba = shost_priv(shost);               \
		u32 value;                                             \
		pm_runtime_get_sync(hba->dev);			       \
		if (ufshcd_query_attr(hba, UPIU_QUERY_OPCODE_READ_ATTR,\
				QUERY_ATTR_IDN##_uname, 0, 0, &value)) \
			return -EINVAL;				       \
		pm_runtime_put_sync(hba->dev);			       \
		return sprintf(buf, "0x%08X\n", value);		       \
	}                                                              \
	static DEVICE_ATTR_RO(_name)

ufs_attribute(wb_flush_status, _WB_FLUSH_STATUS);
ufs_attribute(wb_available_buffer_size, _AVAIL_WB_BUFF_SIZE);
ufs_attribute(wb_buffer_lifetime_est, _WB_BUFF_LIFE_TIME_EST);
ufs_attribute(wb_current_buffer_size, _CURR_WB_BUFF_SIZE);

static int ufshcd_query_flag_common(struct ufs_hba *hba,
	enum query_opcode opcode, enum flag_idn idn, bool *flag_res)
{
	if (hba == NULL || flag_res == NULL)
		return -EINVAL;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(VERSION, PATCH_L, SUB_L)
	return ufshcd_query_flag(hba, opcode, idn, 0, flag_res);
#else
	return ufshcd_query_flag(hba, opcode, idn, flag_res);
#endif
}

#define ufs_flag(_name, _uname)                                                \
	static ssize_t _name##_show(struct device *dev,                \
		struct device_attribute *attr, char *buf)	       \
	{                                                              \
		bool flag;                                             \
		struct Scsi_Host *shost = class_to_shost(dev);         \
		struct ufs_hba *hba = shost_priv(shost);               \
		pm_runtime_get_sync(hba->dev);			       \
		if (ufshcd_query_flag_common(hba, UPIU_QUERY_OPCODE_READ_FLAG,\
			QUERY_FLAG_IDN##_uname, &flag))                \
			return -EINVAL;				       \
		pm_runtime_put_sync(hba->dev);			       \
		return sprintf(buf, "%s\n",                            \
			flag ? "true" : "false");		       \
	}                                                              \
	static DEVICE_ATTR_RO(_name)

ufs_flag(wb_en, _WB_EN);
ufs_flag(wb_flush_en, _WB_BUFF_FLUSH_EN);
ufs_flag(wb_flush_during_hibern8, _WB_BUFF_FLUSH_DURING_HIBERN8);

static ssize_t wb_permanent_disable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t len);
static ssize_t wb_permanent_disable_show(struct device *dev,
	struct device_attribute *attr, char *buf);
DEVICE_ATTR_RW(wb_permanent_disable);

#ifdef CONFIG_HUAWEI_UFS_WB_RUNTIME
static ssize_t wb_runtime_switch_disable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t len);
static ssize_t wb_runtime_switch_disable_show(struct device *dev,
	struct device_attribute *attr, char *buf);
DEVICE_ATTR_RW(wb_runtime_switch_disable);
#endif

struct device_attribute *ufs_host_attrs[] = {
	&dev_attr_wb_en,
	&dev_attr_wb_flush_en,
	&dev_attr_wb_flush_during_hibern8,
	&dev_attr_wb_flush_status,
	&dev_attr_wb_available_buffer_size,
	&dev_attr_wb_buffer_lifetime_est,
	&dev_attr_wb_current_buffer_size,
	&dev_attr_wb_permanent_disable,
#ifdef CONFIG_HUAWEI_UFS_WB_RUNTIME
	&dev_attr_wb_runtime_switch_disable,
#endif
	NULL
};

static int ufshcd_wb_exception_event_ctrl(struct ufs_hba *hba, bool enable)
{
	int ret;
	unsigned long flags;
	u32 val;

	/*
	 * Some device can alloc a new write buffer by toggle write booster.
	 * We apply a exception handler func for and only for those devices.
	 *
	 * If new strategy was to implement, the hba->wb_work would be the good
	 * place to put new function to, ufshcd_exception_event_handler would
	 * handle them the same way.
	 */
	if (hba->dev_info.w_manufacturer_id != UFS_VENDOR_TOSHIBA)
		return 0;

	if (enable)
		val = hba->ee_ctrl_mask | MASK_EE_WRITEBOOSTER_EVENT;
	else
		val = hba->ee_ctrl_mask & ~MASK_EE_WRITEBOOSTER_EVENT;
	val &= MASK_EE_STATUS;
	ret = ufshcd_query_attr(hba, UPIU_QUERY_OPCODE_WRITE_ATTR,
			QUERY_ATTR_IDN_EE_CONTROL, 0, 0, &val);
	if (ret) {
		dev_err(hba->dev,
			"%s: failed to enable wb exception event %d\n",
			__func__, ret);
		return ret;
	}

	spin_lock_irqsave(hba->host->host_lock, flags);
	hba->ee_ctrl_mask &= ~MASK_EE_WRITEBOOSTER_EVENT;
	hba->ufs_wb->wb_exception_enabled = enable;
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	dev_info(hba->dev, "%s: wb exception event %s\n",
			__func__, enable ? "enable" : "disable");

	return ret;
}

static inline struct ufs_hba *ufshcd_dev_to_hba(struct device *dev)
{
	if (!dev)
		return NULL;

	return shost_priv(class_to_shost(dev));
}

static inline int ufshcd_disable_wb_exception_event(struct ufs_hba *hba)
{
	return ufshcd_wb_exception_event_ctrl(hba, false);
}

int ufshcd_enable_wb_exception_event(struct ufs_hba *hba)
{
	return ufshcd_wb_exception_event_ctrl(hba, true);
}

bool ufshcd_wb_is_permanent_disabled(struct ufs_hba *hba)
{
	unsigned long flags;
	bool wb_permanent_disabled = false;

	spin_lock_irqsave(hba->host->host_lock, flags);
	wb_permanent_disabled = hba->ufs_wb->wb_permanent_disabled;
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	return wb_permanent_disabled;
}

static void ufshcd_set_wb_permanent_disable(struct ufs_hba *hba)
{
	unsigned long flags;

	spin_lock_irqsave(hba->host->host_lock, flags);
	hba->ufs_wb->wb_permanent_disabled = true;
	spin_unlock_irqrestore(hba->host->host_lock, flags);
}

static bool ufshcd_get_wb_work_sched(struct ufs_hba *hba)
{
	unsigned long flags;
	bool wb_work_sched = false;

	spin_lock_irqsave(hba->host->host_lock, flags);
	wb_work_sched = hba->ufs_wb->wb_work_sched;
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	return wb_work_sched;
}

static ssize_t wb_permanent_disable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t len)
{
	int ret = 0;
	struct ufs_hba *hba = ufshcd_dev_to_hba(dev);

	if (!hba)
		return -EFAULT;

	pm_runtime_get_sync(hba->dev);
	if (!ufshcd_wb_sup(hba)) {
		dev_info(hba->dev, "%s wb not support\n", __func__);
		goto out;
	}

	ret = ufshcd_disable_wb_exception_event(hba);
	if (ret) {
		dev_err(hba->dev, "%s wb disable exception fail\n", __func__);
		goto out;
	}

	/* when WRITEBOOSTER_EVENT is processing, caller will retry. */
	if (ufshcd_get_wb_work_sched(hba)) {
		ret = -EAGAIN;
		goto out;
	}

	ret = ufshcd_wb_ctrl(hba, false);
	if (ret) {
		dev_err(hba->dev, "%s wb disable fail\n", __func__);

		goto out;
	}
	ufshcd_set_wb_permanent_disable(hba);
	dev_info(hba->dev, "%s wb permanent disable succ\n", __func__);
out:
	pm_runtime_put_sync(hba->dev);

	return !ret ? len : ret;
}

static ssize_t wb_permanent_disable_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	bool flag = false;
	struct ufs_hba *hba = ufshcd_dev_to_hba(dev);

	if (!hba)
		return snprintf(buf, PAGE_SIZE,
			"wb permanent disable hba is null\n");

	if (!ufshcd_wb_sup(hba)) {
		dev_info(hba->dev, "%s wb not support\n", __func__);
		return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1,
			"donnot support wb\n");
	}

	flag = ufshcd_wb_is_permanent_disabled(hba);

	return snprintf(buf, PAGE_SIZE, "wb permanent disable : %s\n",
		flag ? "true" : "false");
}


static void ufshcd_wb_toggle_fn(struct work_struct *work)
{
	int ret;
	u32 current_buffer;
	u32 available;
	unsigned long flags;
	struct ufs_hba *hba;
	struct delayed_work *dwork = to_delayed_work(work);

	hba = ((struct ufs_wb_switch_info *)(container_of(dwork,
		struct ufs_wb_switch_info, wb_toggle_work)))->hba;

	dev_info(hba->dev, "%s: WB for Txxx toggle func", __func__);

	if (hba->dev_info.w_manufacturer_id != UFS_VENDOR_TOSHIBA)
		return;

	pm_runtime_get_sync(hba->dev);

	if (ufshcd_wb_ctrl(hba, false) || ufshcd_wb_ctrl(hba, true))
		goto out;
	/*
	 * Error handler doesn't need here. If we failed to check buffer
	 * size, following io would cause a new exception handler which
	 * let us try here again.
	 */
	ret = ufshcd_query_attr_retry(hba, UPIU_QUERY_OPCODE_READ_ATTR,
				      QUERY_ATTR_IDN_CURR_WB_BUFF_SIZE, 0,
				      0, &current_buffer);
	if (ret)
		goto out;

	ret = ufshcd_query_attr_retry(hba, UPIU_QUERY_OPCODE_READ_ATTR,
				      QUERY_ATTR_IDN_AVAIL_WB_BUFF_SIZE, 0,
				      0, &available);
	if (ret)
		goto out;

	/*
	 * Maybe, there are not enough space to alloc a new write buffer, if so,
	 * disable exception to avoid en endless notification.
	 */
	if (current_buffer < hba->ufs_wb->wb_shared_alloc_units ||
		available <= WB_T_BUFFER_THRESHOLD) {
		ret = ufshcd_wb_exception_event_ctrl(hba, false);
		if (!ret) {
			spin_lock_irqsave(hba->host->host_lock, flags);
			hba->ufs_wb->wb_exception_enabled = false;
			spin_unlock_irqrestore(hba->host->host_lock, flags);
		}
	}

out:
	pm_runtime_put_sync(hba->dev);
	spin_lock_irqsave(hba->host->host_lock, flags);
	hba->ufs_wb->wb_work_sched = false;
	spin_unlock_irqrestore(hba->host->host_lock, flags);
}

void ufshcd_wb_toggle_cancel(struct ufs_hba *hba)
{
	if (!hba)
		return;
	if (!hba->ufs_wb)
		return;

	flush_delayed_work(&hba->ufs_wb->wb_toggle_work);
}

void ufshcd_wb_exception_event_handler(struct ufs_hba *hba)
{
	unsigned long flags;

	if (!hba)
		return;

	spin_lock_irqsave(hba->host->host_lock, flags);

	if (!hba->ufs_wb->wb_exception_enabled)
		goto out;

	if (hba->ufs_wb->wb_work_sched)
		goto out;
	hba->ufs_wb->wb_work_sched = true;

	schedule_delayed_work(&hba->ufs_wb->wb_toggle_work,
			msecs_to_jiffies(0));

out:
	spin_unlock_irqrestore(hba->host->host_lock, flags);
}

static int ufs_get_wb_device_desc(struct ufs_hba *hba)
{
	int err;
	u8 *desc_buf = NULL;
	size_t buff_len;

	buff_len = max_t(size_t, hba->desc_size.dev_desc,
			QUERY_DESC_MAX_SIZE + 1);
	desc_buf = kmalloc(buff_len, GFP_KERNEL);
	if (!desc_buf)
		return -ENOMEM;

	err = ufshcd_read_device_desc(hba, desc_buf, hba->desc_size.dev_desc);
	if (err) {
		dev_err(hba->dev, "%s: Failed reading Device Desc. err = %d\n",
			__func__, err);
		goto out;
	}

	hba->ufs_wb->wb_shared_alloc_units = get_unaligned_be32(
		desc_buf + DEVICE_DESC_PARAM_WB_SHARED_ALLOC_UNITS);

out:
	kfree(desc_buf);
	return err;
}

#ifdef CONFIG_HUAWEI_UFS_WB_RUNTIME
static int ufshcd_wb_auto_flush_ctrl(struct ufs_hba *hba, bool enable)
{
	int val;
	u8 index;

	if (enable)
		val = UPIU_QUERY_OPCODE_SET_FLAG;
	else
		val = UPIU_QUERY_OPCODE_CLEAR_FLAG;

	hba->ufs_wb->wb_runtime.flush_is_open = enable;

	index = ufshcd_wb_get_query_index(hba);
	return ufshcd_query_flag_retry(hba, val,
				QUERY_FLAG_IDN_WB_BUFF_FLUSH_DURING_HIBERN8,
				index, NULL);
}

static ssize_t wb_runtime_switch_disable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t len)
{
	int ret = 0;
	unsigned long flags;
	struct ufs_hba *hba = ufshcd_dev_to_hba(dev);
	struct ufs_wb_runtime_switch *wb_runtime = NULL;

	if (hba == NULL || hba->ufs_wb == NULL)
		return -EFAULT;

	pm_runtime_get_sync(hba->dev);
	if (!ufshcd_wb_sup(hba)) {
		dev_info(hba->dev, "%s wb not support\n", __func__);
		goto out;
	}

	wb_runtime = &hba->ufs_wb->wb_runtime;
	spin_lock_irqsave(hba->host->host_lock, flags);
	if (strncmp(buf, "true", strlen("true")) == 0)
		wb_runtime->disable = 1;
	else if (strncmp(buf, "false", strlen("false")) == 0)
		wb_runtime->disable = 0;
	spin_unlock_irqrestore(hba->host->host_lock, flags);
	ufshcd_wb_ctrl(hba, true);
	ufshcd_wb_auto_flush_ctrl(hba, true);
out:
	pm_runtime_put_sync(hba->dev);
	return !ret ? len : ret;
}

static ssize_t wb_runtime_switch_disable_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct ufs_hba *hba = ufshcd_dev_to_hba(dev);
	struct ufs_wb_runtime_switch *wb_runtime = NULL;

	if (hba == NULL || hba->ufs_wb == NULL)
		return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1,
			"hba or ufs_wb is null\n");

	if (!ufshcd_wb_sup(hba)) {
		dev_info(hba->dev, "%s wb not support\n", __func__);
		return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1,
			"donnot support wb\n");
	}

	wb_runtime = &hba->ufs_wb->wb_runtime;

	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1,
		"wb_runtime disable : %s, wb_en is %d\n",
		wb_runtime->disable ? "true" : "false", hba->wb_enabled);
}

void ufshcd_wb_runtime_switch_suspend(struct ufs_hba *hba)
{
	if (hba == NULL || hba->ufs_wb == NULL)
		return;

	if (!ufshcd_wb_sup(hba))
		return;

	cancel_work_sync(&hba->ufs_wb->ondemad_wb_work);
}

static void ufshcd_wb_update_auto_flush_status(struct ufs_hba *hba)
{
	int ret;
	unsigned int available;
	struct ufs_wb_runtime_switch *wb_runtime = NULL;

	if (hba->ufs_wb == NULL)
		return;

	wb_runtime = &hba->ufs_wb->wb_runtime;
	if (!wb_runtime->is_samsung_v5 || !wb_runtime->wb_opt_state)
		return;

	ret = ufshcd_query_attr_retry(hba, UPIU_QUERY_OPCODE_READ_ATTR,
			QUERY_ATTR_IDN_AVAIL_WB_BUFF_SIZE, 0, 0,
			&available);
	if (ret) {
		dev_info(hba->dev, "get avail_wb_buff fail, ret %d\n", ret);
		return;
	}
	if (available <= FLUSH_LOW_LIMIT && !wb_runtime->flush_is_open)
		ufshcd_wb_auto_flush_ctrl(hba, true);
	if (available >= FLUSH_UP_LIMIT && wb_runtime->flush_is_open)
		ufshcd_wb_auto_flush_ctrl(hba, false);
}

static void ufshcd_wb_ondemand_switch(struct work_struct *work)
{
	struct ufs_hba *hba = NULL;
	struct ufs_wb_switch_info *ufs_wb =
		container_of(work, struct ufs_wb_switch_info, ondemad_wb_work);

	hba = ufs_wb->hba;

	pm_runtime_get_sync(hba->dev);
	if (ufshcd_wb_ctrl(hba, ufs_wb->wb_runtime.wb_opt_state))
		dev_err(hba->dev, "%s wb work fail\n", __func__);

	ufshcd_wb_update_auto_flush_status(hba);
	pm_runtime_put_sync(hba->dev);
}

static bool ufshcd_wb_is_need_close(struct ufs_hba *hba)
{
	struct ufs_wb_runtime_switch *wb_runtime = &hba->ufs_wb->wb_runtime;

	if (wb_runtime->avg_slot < CLOSE_WB_SLOT_MAX &&
		wb_runtime->low_speed_cnt >= COUNT_CLOSE_WB_MAX)
		return true;

	return false;
}

static bool ufshcd_wb_is_need_open(struct ufs_hba *hba)
{
	struct ufs_wb_runtime_switch *wb_runtime = &hba->ufs_wb->wb_runtime;

	if (wb_runtime->avg_slot >= OPEN_WB_SLOT_MAX ||
		wb_runtime->curr_bw >= wb_runtime->high_bw_threshold)
		return true;

	return false;
}

static void ufshcd_wb_calc_demand(struct ufs_hba *hba,
	unsigned char opcode, unsigned int blk_cnt)
{
	ktime_t time_us_delta;
	ktime_t now_time = ktime_get();
	unsigned int curr_bw;
	const unsigned short rate = 3906; /* for converting bytes/us to MB/s */
	struct ufs_wb_runtime_switch *wb_runtime = &hba->ufs_wb->wb_runtime;

	if (opcode == WRITE_6 || opcode == WRITE_10 || opcode == WRITE_16)
		wb_runtime->total_write = max_t(u64,
			wb_runtime->total_write + blk_cnt,
			wb_runtime->total_write);

	wb_runtime->slot_cnt += hweight_long(hba->lrb_in_use);
	wb_runtime->req_cnt++;

	time_us_delta = ktime_us_delta(now_time, wb_runtime->last_time);
	if (time_us_delta >= MONITOR_WB_US) {
		curr_bw = (wb_runtime->total_write * rate) / time_us_delta;
		/* calc bw */
		if (curr_bw < wb_runtime->high_bw_threshold)
			wb_runtime->low_speed_cnt = max_t(u64,
			wb_runtime->low_speed_cnt + 1,
			wb_runtime->low_speed_cnt);
		else
			wb_runtime->low_speed_cnt = 0;

		/* calc avg slot */
		wb_runtime->avg_slot = (unsigned short)(wb_runtime->slot_cnt /
						wb_runtime->req_cnt);
		wb_runtime->last_time = now_time;
		wb_runtime->curr_bw = curr_bw;
		wb_runtime->total_write = 0;
		wb_runtime->req_cnt = 0;
		wb_runtime->slot_cnt = 0;
	}
}

static bool ufshcd_wb_is_samsung_v5(struct ufs_hba *hba)
{
	unsigned short i = 0;
	struct ufs_dev_info *dev_info = NULL;
	const char *target_pnm[MAX_PNM_LEN] = {
		"KLUDG4UHDB-B2D1",
		"KLUEG8UHDB-C2D1",
		"KLUFG8RHDA-B2D1",
		"KLUDG4UHDB-B2E1",
		"KLUEG8UHDB-C2E1",
		"KLUFG8RHDA-B2E1",
		NULL
	};

	dev_info = &hba->dev_info;
	while (target_pnm[i] != NULL) {
		if (!strncmp(dev_info->model_name, target_pnm[i],
							strlen(target_pnm[i])))
			return true;
		i++;
	}

	return false;
}

void ufshcd_wb_runtime_config(struct scsi_cmnd *cmd, struct ufs_hba *hba)
{
	unsigned long flags;
	unsigned char opcode;
	unsigned int bytes;
	struct ufs_wb_runtime_switch *wb_runtime = NULL;

	if (cmd == NULL || hba == NULL || hba->ufs_wb == NULL)
		return;

	if (!ufshcd_wb_sup(hba))
		return;

	bytes = cmd->sdb.length;
	opcode = cmd->cmnd[0];
	wb_runtime = &hba->ufs_wb->wb_runtime;
	if (wb_runtime->disable || ufshcd_wb_is_permanent_disabled(hba))
		return;

	spin_lock_irqsave(hba->host->host_lock, flags);

	ufshcd_wb_calc_demand(hba, opcode, bytes >> PAGE_SHIFT);

	if (ufshcd_wb_is_need_open(hba)) {
		wb_runtime->wb_opt_state = true;
		wb_runtime->low_speed_cnt = 0;
	}

	if (ufshcd_wb_is_need_close(hba))
		wb_runtime->wb_opt_state = false;

	if (wb_runtime->wb_opt_state != hba->wb_enabled) {
		trace_ufs_wb_runtime(wb_runtime->avg_slot,
			wb_runtime->total_write, wb_runtime->low_speed_cnt,
			wb_runtime->curr_bw, hba->wb_enabled);

		if (!work_busy(&hba->ufs_wb->ondemad_wb_work))
			queue_work(system_freezable_wq, &hba->ufs_wb->ondemad_wb_work);
	}

	spin_unlock_irqrestore(hba->host->host_lock, flags);
}

static void ufshcd_ufs_wb_runtime_switch_init(struct ufs_hba *hba)
{
	int ret;
	struct ufs_wb_runtime_switch *wb_runtime = &hba->ufs_wb->wb_runtime;

	ret = memset_s(wb_runtime, sizeof(struct ufs_wb_runtime_switch),
		0x0, sizeof(struct ufs_wb_runtime_switch));
	if (ret) {
		pr_err("%s memset_s fail\n", __func__);
		return;
	}

	wb_runtime->disable = false;
	wb_runtime->wb_opt_state = true;
	wb_runtime->high_bw_threshold = 200; /* 200: is slc_randow_write bw */
	INIT_WORK(&hba->ufs_wb->ondemad_wb_work, ufshcd_wb_ondemand_switch);

	if (ufshcd_wb_is_samsung_v5(hba)) {
		wb_runtime->is_samsung_v5 = true;
		wb_runtime->flush_is_open = true;
	}
}
#endif

int ufs_dynamic_switching_wb_config(struct ufs_hba *hba)
{
	int ret = -EINVAL;
	if (!hba)
		return -1;

	if (!ufshcd_wb_sup(hba)) {
		dev_info(hba->dev, "%s wb not support\n", __func__);
		return -1;
	}
	if (hba->ufs_wb == NULL) {
		hba->ufs_wb = kzalloc(sizeof(struct ufs_wb_switch_info),
						GFP_KERNEL);
		if (!hba->ufs_wb) {
			dev_err(hba->dev, "%s: alloc ufs_wb_switch_info failed!\n",
					__func__);
			return -ENOMEM;
			}
	}

	hba->ufs_wb->hba = hba;
	if (ufshcd_wb_is_permanent_disabled(hba))
		goto out;

#ifdef CONFIG_HUAWEI_UFS_WB_RUNTIME
	ufshcd_ufs_wb_runtime_switch_init(hba);
#endif

	INIT_DELAYED_WORK(&hba->ufs_wb->wb_toggle_work, ufshcd_wb_toggle_fn);
	ret = ufs_get_wb_device_desc(hba);
	if (ret) {
		dev_err(hba->dev, "%s: alloc ufs_wb_switch_info failed!\n", __func__);
		goto out;
	}
	return ret;
out:
	kfree(hba->ufs_wb);
	hba->ufs_wb = NULL;
	return ret;
}

void ufs_dynamic_switching_wb_remove(struct ufs_hba *hba)
{
	if (!hba)
		return;
	if (!hba->ufs_wb)
		return;

	kfree(hba->ufs_wb);
	hba->ufs_wb = NULL;
}
