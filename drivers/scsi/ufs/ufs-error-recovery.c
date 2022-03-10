/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
 * Description: ufs error recovery function
 * Author: lida
 * Create: 2021-06-05
 */

#include "ufs-error-recovery.h"
#include <linux/pm_wakeup.h>

static void ufs_recover_hs_mode_reinit(struct ufs_hba *hba)
{
	unsigned long flags;

	spin_lock_irqsave(hba->host->host_lock, flags);
	hba->ufshcd_state = UFSHCD_STATE_EH_SCHEDULED_NON_FATAL;
	hba->force_host_reset = true;
	if (!queue_work(hba->eh_wq, &hba->eh_work))
		dev_err(hba->dev, "%s: queue hba->eh_work\n", __func__);
	spin_unlock_irqrestore(hba->host->host_lock, flags);
}

static void set_hs_mode(struct ufs_hba *hba)
{
	int ret, val;

	ret = ufshcd_get_max_pwr_mode(hba);
	if (ret) {
		dev_err(hba->dev,
			"%s: Failed getting max supported power mode\n",
			__func__);
		goto reinit;
	}

	ret = ufshcd_config_pwr_mode(hba, &hba->max_pwr_info.info);
	if (ret) {
		dev_err(hba->dev, "%s: Failed setting power mode, err = %d\n",
				__func__, ret);
		goto reinit;
	}

	/* confirm ufs works well after switch to hs mode */
	ret = ufshcd_verify_dev_init(hba);
	if (ret) {
		dev_err(hba->dev, "%s: Failed nop out, err = %d\n",
				__func__, ret);
		goto reinit;
	}

	ret = ufshcd_dme_get(hba, UIC_ARG_MIB(PA_RXPWRSTATUS), &val);
	dev_err(hba->dev, "%s: after change_pwr_mode PA_RXPWRSTATUS: %d\n",__func__, val);
	ufshcd_print_pwr_info(hba);
	return;

reinit:
	ufs_recover_hs_mode_reinit(hba);
	return;
}

static void ufs_recover_hs_mode(struct work_struct *work)
{
	unsigned long flags;
	u32 tm_doorbell;
	u32 tr_doorbell;
	int ret;
	struct ufs_error_recovery *error_recovery;
	struct ufs_hba *hba;
	unsigned long timeout;

	error_recovery = container_of(work, struct ufs_error_recovery, recover_hs_work);
	hba = error_recovery->hba;

	if (error_recovery->in_progress) {
		dev_err(hba->dev, "%s: another work is running, error_recovery->in_progress = %d\n",
				__func__, error_recovery->in_progress);
		return;
	}
	ufshcd_hold(hba, false);
	error_recovery->in_progress = true;
	__pm_stay_awake(error_recovery->recover_wake_lock);
	ret = pm_runtime_get_sync(hba->dev);
	if (ret < 0) {
		goto wake_unlock;
	}

	/* block request*/
	ufshcd_scsi_block_requests(hba);
	/* confirm no request */
	timeout = jiffies + msecs_to_jiffies(DOORBELL_TIMEOUT_MS);
	do {
		if (hba->ufshcd_state != UFSHCD_STATE_OPERATIONAL)
			goto fault_state;
		if (time_after(jiffies, timeout)) {
			dev_err(hba->dev, "%s: wait ufs host free timeout, lrb_in_use 0x%lx\n",
					__func__, hba->lrb_in_use);
			break;
		}
		spin_lock_irqsave(hba->host->host_lock, flags);
		if (hba->is_sys_suspended || hba->pm_op_in_progress) {
			spin_unlock_irqrestore(hba->host->host_lock, flags);
			msleep(5);
			continue;
		}
		tm_doorbell = ufshcd_readl(hba, REG_UTP_TASK_REQ_DOOR_BELL);
		tr_doorbell = ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_DOOR_BELL);
		if (hba->outstanding_reqs || tm_doorbell || tr_doorbell) {
			spin_unlock_irqrestore(hba->host->host_lock, flags);
			msleep(5);
			continue;
		} else {
			spin_unlock_irqrestore(hba->host->host_lock, flags);
			break;
		}
	} while(1);

	set_hs_mode(hba);

fault_state:
	pm_runtime_allow(hba->dev);
	dev_err(hba->dev, "enable ufs pm runtime in hs\n");
	ufshcd_scsi_unblock_requests(hba);

wake_unlock:
	pm_runtime_put_sync(hba->dev);
	__pm_relax(error_recovery->recover_wake_lock);
	error_recovery->in_progress = false;
	ufshcd_release(hba);
	return;
}

static void check_link_status_handler(struct work_struct *work)
{
	struct ufs_hba *hba;
	struct ufs_error_recovery *error_recovery;
	int ret, val;

	error_recovery = container_of(work, struct ufs_error_recovery, check_link_status);
	hba = error_recovery->hba;

	if (error_recovery->check_link_count == 0)
		return;

	ret = pm_runtime_get_sync(hba->dev);
	if (ret < 0) {
		dev_err(hba->dev, "%s: pm_runtime_get_sync failed: %d\n",
			__func__, ret);
		pm_runtime_put_sync(hba->dev);
		return;
	}
	mutex_lock(&error_recovery->check_init_status_mutex);
	ret = ufshcd_dme_get(hba, UIC_ARG_MIB(PA_RXPWRSTATUS), &val);
	dev_err(hba->dev, "%s: line_reset PA_RXPWRSTATUS: %d\n",
			__func__, val);

	if (val == SLOW_STATE) {
		dev_err(hba->dev, "ufs pwrmode = 0x%x after line_reset\n", val);
		ufshcd_init_pwr_info(hba);
		error_recovery->check_link_count = 0;
		if (!work_busy(&error_recovery->recover_hs_work))
			schedule_work(&error_recovery->recover_hs_work);
		else
			dev_err(hba->dev, "%s:recover_hs_work is running \n", __func__);
	} else {
		error_recovery->check_link_count--;
		dev_err(hba->dev, "check link status, %d times remain\n", error_recovery->check_link_count);
	}
	mutex_unlock(&error_recovery->check_init_status_mutex);
	pm_runtime_put_sync(hba->dev);

	return;
}

void update_check_link_count(struct ufs_hba *hba)
{
	hba->error_recovery->check_link_count = CHECK_LINK_COUNT;
	return;
}

void ufs_error_recovery_init(struct ufs_hba *hba)
{
	hba->error_recovery = kzalloc(sizeof(struct ufs_error_recovery), GFP_KERNEL);
	if (!hba->error_recovery) {
		dev_err(hba->dev, "%s: alloc ufs_error_recovery failed!\n", __func__);
	}
	hba->error_recovery->hba = hba;
	hba->error_recovery->check_link_count = 0;
	hba->error_recovery->in_progress = false;
	hba->error_recovery->recover_wake_lock = wakeup_source_register(NULL, "ufs_recover");
	mutex_init(&hba->error_recovery->check_init_status_mutex);
	INIT_WORK(&hba->error_recovery->recover_hs_work, ufs_recover_hs_mode);
	INIT_WORK(&hba->error_recovery->check_link_status, check_link_status_handler);

	return;
}

void check_link_status(struct ufs_hba *hba)
{
	if (hba->ufshcd_state != UFSHCD_STATE_OPERATIONAL)
		goto out;
	queue_work(system_freezable_wq, &hba->error_recovery->check_link_status);
out:
	return;
}

