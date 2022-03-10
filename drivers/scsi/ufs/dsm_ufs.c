#include <dsm/dsm_pub.h>
#include <linux/async.h>
#include <linux/of_gpio.h>
#include "ufs.h"
#include "ufshcd.h"
#include "unipro.h"
#include "dsm_ufs.h"

#include <securec.h>

/*lint -e747 -e713 -e715 -e732 -e835*/
struct ufs_dsm_log  g_ufs_dsm_log;
static struct ufs_dsm_adaptor g_ufs_dsm_adaptor;
struct dsm_client *ufs_dclient;
struct dsm_client *ddr_dclient;
unsigned int ufs_dsm_real_upload_size;
static int dsm_ufs_enable;
struct ufs_uic_err_history uic_err_history;

struct dsm_dev dsm_ufs = {
	.name = "dsm_ufs",
	.buff_size = UFS_DSM_BUFFER_SIZE,
};

struct dsm_dev dsm_ddr = {
	.name = "dsm_ddr",
	.device_name = "ddr",
	.ic_name = "NNN",
	.module_name = "NNN",
	.fops = NULL,
	.buff_size = DDR_DSM_BUFFER_SIZE,
};

static int ufs_timeout_lock = 0;
static int ufs_timeout_count = 0;
#define UFS_TIMEOUT_SERIOUS_THRESHOLD  3

EXPORT_SYMBOL(ufs_dclient);
EXPORT_SYMBOL(ddr_dclient);

static unsigned long dsm_ufs_if_err(void)
{
	return g_ufs_dsm_adaptor.err_type;
}

static int dsm_ufs_update_upiu_info(struct ufs_hba *hba, int tag, int err_code)
{
	struct scsi_cmnd *scmd;
	int i;

	if (!dsm_ufs_enable)
		return -1;

	if (err_code == DSM_UFS_TIMEOUT_ERR) {
		if (test_and_set_bit_lock(UFS_TIMEOUT_ERR, &g_ufs_dsm_adaptor.err_type))
			return -1;
	} else
		return -1;
	if ((tag != -1) && (hba->lrb[tag].cmd)) {
	    scmd = hba->lrb[tag].cmd;
	    for (i = 0; i < COMMAND_SIZE(scmd->cmnd[0]); i++)
		g_ufs_dsm_adaptor.timeout_scmd[i] = scmd->cmnd[i];
	} else {
	    for (i = 0; i < MAX_CDB_SIZE; i++)
		g_ufs_dsm_adaptor.timeout_scmd[i] = 0;
	}
	g_ufs_dsm_adaptor.timeout_slot_id = tag;
	g_ufs_dsm_adaptor.tr_doorbell = 0;
	g_ufs_dsm_adaptor.outstanding_reqs = hba->outstanding_reqs;

	return 0;

}
#define UFS_DMD_INDEX_SIZE    32
static u32 crypto_conf_index[UFS_DMD_INDEX_SIZE];
static int dsm_ufs_updata_ice_info(struct ufs_hba *hba)
{
	unsigned int i;
	if(!dsm_ufs_enable)
		return -1;
	if(test_and_set_bit_lock(UFS_INLINE_CRYPTO_ERR, &g_ufs_dsm_adaptor.err_type))
		return -1;
	g_ufs_dsm_adaptor.ice_outstanding = hba->outstanding_reqs;
	g_ufs_dsm_adaptor.ice_doorbell = ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_DOOR_BELL);
	for(i = 0; i < UFS_DMD_INDEX_SIZE; i++)
	{
		if(!((g_ufs_dsm_adaptor.ice_doorbell >> i)&0x1))
		{
			continue;
		}
		/*
		if (ufshcd_is_hisi_ufs_hc(hba))
			crypto_conf_index[i] =
				(hba->lrb[i].hisi_utr_descriptor_ptr)
					->header.dword_0;
		else
		*/
		crypto_conf_index[i] = (hba->lrb[i].utr_descriptor_ptr)
						       ->header.dword_0;
	}
	return 0;
}
static int dsm_ufs_update_scsi_info(struct ufshcd_lrb *lrbp, int scsi_status, int err_code)
{
	struct scsi_cmnd *scmd;
	int i;

	if (!dsm_ufs_enable)
		return -1;

	switch (err_code) {
	case DSM_UFS_DEV_INTERNEL_ERR:
		if (test_and_set_bit_lock(UFS_DEV_INTERNEL_ERR, &g_ufs_dsm_adaptor.err_type))
			return -1;
		break;
	case DSM_UFS_HARDWARE_ERR:
		if (test_and_set_bit_lock(UFS_HARDWARE_ERR, &g_ufs_dsm_adaptor.err_type))
			return -1;
		break;
	default:
		return -1;
	}

	if (lrbp->cmd)
	    scmd = lrbp->cmd;
	else
	    return -1;
	g_ufs_dsm_adaptor.scsi_status = scsi_status;
	for (i = 0; i < COMMAND_SIZE(scmd->cmnd[0]); i++)
		g_ufs_dsm_adaptor.timeout_scmd[i] = scmd->cmnd[i];

	for (i = 0; i < SCSI_SENSE_BUFFERSIZE; i++)
		g_ufs_dsm_adaptor.sense_buffer[i] = lrbp->sense_buffer[i];

	return 0;
}

static int dsm_ufs_update_ocs_info(struct ufs_hba *hba, int err_code, int ocs)
{
	if (!dsm_ufs_enable)
		return -1;

	if (test_and_set_bit_lock(UFS_UTP_TRANS_ERR, &g_ufs_dsm_adaptor.err_type))
		return -1;
	g_ufs_dsm_adaptor.ocs = ocs;
	return 0;
}

static int dsm_ufs_update_fastboot_info(struct ufs_hba *hba)
{
	char *pstr;
	unsigned int err_code;

	if (!dsm_ufs_enable)
		return -1;

	pstr = strstr(saved_command_line, "fastbootdmd=");
	if (!pstr) {
		pr_err("No fastboot dmd info\n");
		return -EINVAL;
	}
	if (1 != sscanf(pstr, "fastbootdmd=%d", &err_code)) {
		pr_err("Failed to get err_code\n");
		return -EINVAL;
	}

	if (err_code) {
		if (err_code & FASTBOOTDMD_PWR_ERR) {
			if (test_and_set_bit_lock(UFS_FASTBOOT_PWMODE_ERR,
				&g_ufs_dsm_adaptor.err_type))
				return -1;
		}
		if (err_code & FASTBOOTDMD_RW_ERR) {
			if (test_and_set_bit_lock(UFS_FASTBOOT_RW_ERR,
				&g_ufs_dsm_adaptor.err_type))
				return -1;
		}
	} else
		return -EINVAL;

	return 0;
}

static int dsm_ufs_enabled(void)
{
	return dsm_ufs_enable;
}

static void dsm_ufs_fastboot_async(void *data, async_cookie_t cookie)
{
	int ret;
	struct ufs_hba *hba = (struct ufs_hba *)data;

	ret = dsm_ufs_update_fastboot_info(hba);
	if (!ret) {
		pr_info("Ufs get fastboot dmd err info\n");
		if (dsm_ufs_enabled())
			queue_delayed_work(system_freezable_wq, &hba->ufs_dmd->dsm_delay_work, msecs_to_jiffies(0));
	}
}

void dsm_ufs_enable_uic_err(struct ufs_hba *hba)
{
	unsigned long flags;

	clear_bit_unlock( 0x0, &(g_ufs_dsm_adaptor.uic_disable));
	spin_lock_irqsave(hba->host->host_lock, flags);/*lint !e550*/
	ufshcd_enable_intr(hba, UIC_ERROR);
	spin_unlock_irqrestore(hba->host->host_lock, flags);/*lint !e550*/
	return;
}

static int dsm_ufs_disable_uic_err(void)
{
	return (test_and_set_bit_lock( 0x0, &(g_ufs_dsm_adaptor.uic_disable)));
}

static int dsm_ufs_if_uic_err_disable(void)
{
	return (test_bit( 0x0, &(g_ufs_dsm_adaptor.uic_disable)));
}

static int dsm_ufs_update_uic_info(struct ufs_hba *hba, int err_code)
{
	int pos = 0;

	if (!dsm_ufs_enable)
	    return -1;
	if (dsm_ufs_if_uic_err_disable())
		return 0;
	if (test_and_set_bit_lock(UFS_UIC_TRANS_ERR, &g_ufs_dsm_adaptor.err_type))
	    return -1;
	pos = (hba->ufs_stats.pa_err.pos - 1) % UFS_ERR_REG_HIST_LENGTH;
	g_ufs_dsm_adaptor.uic_uecpa = hba->ufs_stats.pa_err.reg[pos];
	pos = (hba->ufs_stats.dl_err.pos - 1) % UFS_ERR_REG_HIST_LENGTH;
	g_ufs_dsm_adaptor.uic_uecdl = hba->ufs_stats.dl_err.reg[pos];
	pos = (hba->ufs_stats.nl_err.pos - 1) % UFS_ERR_REG_HIST_LENGTH;
	g_ufs_dsm_adaptor.uic_uecn = hba->ufs_stats.nl_err.reg[pos];
	pos = (hba->ufs_stats.tl_err.pos - 1) % UFS_ERR_REG_HIST_LENGTH;
	g_ufs_dsm_adaptor.uic_uect = hba->ufs_stats.tl_err.reg[pos];
	pos = (hba->ufs_stats.dme_err.pos - 1) % UFS_ERR_REG_HIST_LENGTH;
	g_ufs_dsm_adaptor.uic_uedme = hba->ufs_stats.dme_err.reg[pos];

	if (g_ufs_dsm_adaptor.uic_uecpa & UIC_PHY_ADAPTER_LAYER_GENERIC_ERROR) {
		if (test_and_set_bit_lock(UFS_LINE_RESET_ERR, &g_ufs_dsm_adaptor.err_type))
		    return -1;
	}
	if (g_ufs_dsm_adaptor.uic_uecdl & UIC_DATA_LINK_LAYER_ERROR_PA_INIT) {
		if (test_and_set_bit_lock(UFS_PA_INIT_ERR, &g_ufs_dsm_adaptor.err_type))
		return -1;
	}

	return 0;
}

static int dsm_ufs_update_error_info(struct ufs_hba *hba, int code)
{

	if (!dsm_ufs_enable)
		return -1;

	switch (code) {
	case DSM_UFS_LINKUP_ERR:
		(void)test_and_set_bit_lock(UFS_LINKUP_ERR, &g_ufs_dsm_adaptor.err_type);
		break;
	case DSM_UFS_UIC_CMD_ERR:
		if (test_and_set_bit_lock(UFS_UIC_CMD_ERR, &g_ufs_dsm_adaptor.err_type))
			break;
		g_ufs_dsm_adaptor.uic_info[0] = ufshcd_readl(hba, REG_UIC_COMMAND);
		g_ufs_dsm_adaptor.uic_info[1] = ufshcd_readl(hba, REG_UIC_COMMAND_ARG_1);
		g_ufs_dsm_adaptor.uic_info[2] = ufshcd_readl(hba, REG_UIC_COMMAND_ARG_2);
		g_ufs_dsm_adaptor.uic_info[3] = ufshcd_readl(hba, REG_UIC_COMMAND_ARG_3);
		break;
		case DSM_UFS_SYSBUS_ERR:
		case DSM_UFS_CONTROLLER_ERR:
		case DSM_UFS_PA_INIT_ERR:
		case DSM_UFS_DEV_ERR:
		case DSM_UFS_ENTER_OR_EXIT_H8_ERR:
		case DSM_UFS_LINE_RESET_ERR:
			if (code == DSM_UFS_SYSBUS_ERR) {
				if (test_and_set_bit_lock(UFS_SYSBUS_ERR, &g_ufs_dsm_adaptor.err_type))
					break;
			} else if (code == DSM_UFS_CONTROLLER_ERR) {
				if (test_and_set_bit_lock(UFS_CONTROLLER_ERR, &g_ufs_dsm_adaptor.err_type))
					break;
			} else if (code == DSM_UFS_PA_INIT_ERR) {
				if (test_and_set_bit_lock(UFS_PA_INIT_ERR, &g_ufs_dsm_adaptor.err_type))
					break;
			} else if (code == DSM_UFS_LINE_RESET_ERR) {
				if (test_and_set_bit_lock(UFS_LINE_RESET_ERR, &g_ufs_dsm_adaptor.err_type))
					break;
			} else if (code == DSM_UFS_DEV_ERR) {
			if (test_and_set_bit_lock(UFS_DEV_ERR, &g_ufs_dsm_adaptor.err_type))
				break;
			} else if (code == DSM_UFS_ENTER_OR_EXIT_H8_ERR) {
			if (test_and_set_bit_lock(UFS_ENTER_OR_EXIT_H8_ERR, &g_ufs_dsm_adaptor.err_type))
				break;
		}
		break;
	case DSM_UFS_LIFETIME_EXCCED_ERR:
		(void)test_and_set_bit_lock(UFS_LIFETIME_EXCCED_ERR, &g_ufs_dsm_adaptor.err_type);
		break;
	default:
		return 0;
	}
	return 0;
}

/*put error message into buffer*/
void get_bootdevice_product_name(char* product_name, u32 len);
static void schedule_delayed_dsm_work(struct ufs_hba *hba, unsigned long delay);

static void set_dsm_dev_ic_module_name(char *product_name, char *rev)
{
	int ret = 0;
	dsm_ufs.ic_name = (const char *)product_name;
	dsm_ufs.module_name = (const char *)rev;
	ret = dsm_update_client_vendor_info(&dsm_ufs);
	if (ret)
		pr_err("update dsm ufs ic_name and module_name failed!\n");
	dsm_ufs.ic_name = NULL;
	dsm_ufs.module_name = NULL;
}

static void dsm_ufs_host_reset_history_cmd(struct ufs_hba *hba,
	char *dsm_log_buff, unsigned int *buff_size)
{
	int ret;
	int pos;
	int loop_cnt = 0;
	struct ufs_dmd_info *ufs_dmd = hba->ufs_dmd;
	struct dsm_ufs_cmd_log *cmd_log = NULL;
	struct dsm_ufs_cmd_log_entry *entry = NULL;
	if (ufs_dmd == NULL)
		return;
	cmd_log = ufs_dmd->cmd_log;
	if (cmd_log == NULL)
		return;

	if (!(g_ufs_dsm_adaptor.err_type & (1 << UFS_TIMEOUT_SERIOUS))) {
		pr_err("UFS DSM Error! Err Num: %lu, %lu\n",
			UFS_TIMEOUT_SERIOUS, g_ufs_dsm_adaptor.err_type);
		return;
	}

	pos = cmd_log->pos;
	while(loop_cnt < hba->nutrs && pos < hba->nutrs) {
		loop_cnt++;

		entry = &cmd_log->cmd_log_entry[pos];
		pos = ++pos >= hba->nutrs ? 0 : pos;

		if (!entry->cmd_op)
			continue;

		dev_err(hba->dev, "[%d], cmd_op 0x%x, lba %llu, tag %u, "
			"trans_len %d, DPO %u, FUA %u, issue_time %llu,"
			"complete_time delta %llu\n", loop_cnt, entry->cmd_op,
			entry->lba, entry->tag, entry->transfer_len,
			entry->dpo, entry->fua, entry->issue_time,
			ktime_us_delta(entry->complete_time, entry->issue_time));

		ret = snprintf_s(dsm_log_buff, *buff_size, *buff_size - 1,
			"%d,0x%x,%llu,%u,%d,%u,%u,%llu,%llu;",
			loop_cnt, entry->cmd_op, entry->lba, entry->tag,
			entry->transfer_len, entry->dpo, entry->fua, entry->issue_time,
			ktime_us_delta(entry->complete_time, entry->issue_time));

		if (ret < 0)
			break;

		dsm_log_buff += ret;
		*buff_size -= ret;
	}
}

static int dsm_ufs_update_host_reset_info(struct ufs_hba *hba, int err_code)
{
	if (!dsm_ufs_enable || err_code != DSM_UFS_TIMEOUT_SERIOUS)
		return -1;

	if (test_and_set_bit_lock(UFS_TIMEOUT_SERIOUS,
				&g_ufs_dsm_adaptor.err_type)) {
		dev_err(hba->dev, "dsm host_reset bit already be set.\n");
		return -1;
	}

	return 0;
}

void dsm_ufs_report_host_reset(struct ufs_hba *hba)
{
	int ret;

	ret = dsm_ufs_update_host_reset_info(hba, DSM_UFS_TIMEOUT_SERIOUS);
	if (ret)
		return;

	schedule_delayed_dsm_work(hba, msecs_to_jiffies(0));

}

static void dsm_ufs_restore_cmd_init(struct ufs_hba *hba)
{
	struct device *dev = hba->dev;
	struct ufs_dmd_info *ufs_dmd = hba->ufs_dmd;

	if (ufs_dmd == NULL)
		return;

	ufs_dmd->cmd_log = devm_kzalloc(dev,
		sizeof(struct dsm_ufs_cmd_log), GFP_KERNEL);
	if (ufs_dmd->cmd_log == NULL) {
		dev_err(dev,"alloc dsm_cmd_log fail in %s\n", __func__);
		return;
	}

	ufs_dmd->cmd_log->cmd_log_entry = devm_kzalloc(dev,
		sizeof(struct dsm_ufs_cmd_log_entry) * hba->nutrs, GFP_KERNEL);
	if (ufs_dmd->cmd_log->cmd_log_entry == NULL) {
		dev_err(dev,"alloc cmd_log_entry fail in %s\n", __func__);
		devm_kfree(dev, ufs_dmd->cmd_log);
		ufs_dmd->cmd_log = NULL;
	}
}

void dsm_ufs_restore_cmd(struct ufs_hba *hba, unsigned int tag)
{
	struct ufshcd_lrb *lrbp = &hba->lrb[tag];
	struct scsi_cmnd *cmd = lrbp->cmd;
	struct ufs_dmd_info *ufs_dmd = hba->ufs_dmd;
	struct dsm_ufs_cmd_log_entry *log_entry = NULL;
	struct dsm_ufs_cmd_log *cmd_log = NULL;

	if (!cmd || !ufs_dmd || !ufs_dmd->cmd_log)
		return;

	cmd_log = ufs_dmd->cmd_log;
	cmd_log->pos = ++cmd_log->pos >= hba->nutrs ? 0 : cmd_log->pos;

	log_entry = &cmd_log->cmd_log_entry[cmd_log->pos];
	log_entry->cmd_op = cmd->cmnd[0];
	log_entry->dpo = (cmd->cmnd[1] >> DPO_SHIFT) & 0x01;
	log_entry->fua = (cmd->cmnd[1] >> FUA_SHIFT) & 0x01;
	log_entry->lun = lrbp->lun;
	log_entry->tag = tag;
	log_entry->issue_time = lrbp->issue_time_stamp;
	log_entry->complete_time = ktime_get();

	if (!cmd->request || !cmd->request->bio) {
		log_entry->lba = 0;
		log_entry->transfer_len = 0;
		return;
	}

	log_entry->lba = cmd->request->bio->bi_iter.bi_sector;
	log_entry->transfer_len =
		be32_to_cpu(lrbp->ucd_req_ptr->sc.exp_data_transfer_len);
}

#define PRODUCT_NAME_SIZE   32
int dsm_ufs_get_log(struct ufs_hba *hba, unsigned long code, char *err_msg)
{
	int i, ret = 0;
	int buff_size = sizeof(g_ufs_dsm_log.dsm_log);
	char *dsm_log_buff = g_ufs_dsm_log.dsm_log;
	char product_name[PRODUCT_NAME_SIZE] = {0};
	char rev[5] = {0};
	int j;

	memset((void*)product_name, 0, PRODUCT_NAME_SIZE);

	if (!dsm_ufs_enable)
		return 0;

	pr_err("enter dsm_ufs_get_log\n");
	g_ufs_dsm_adaptor.manufacturer_id = hba->manufacturer_id;
	memset(g_ufs_dsm_log.dsm_log, 0, buff_size);/**/
	ret = snprintf(dsm_log_buff, buff_size, "0x%04x,", g_ufs_dsm_adaptor.manufacturer_id);

	dsm_log_buff += ret;
	buff_size -= ret;

	/*depend on bootdevice*/
	get_bootdevice_product_name( product_name, PRODUCT_NAME_SIZE);
	ret = snprintf(dsm_log_buff, buff_size, "%s ", product_name);

	dsm_log_buff += ret;
	buff_size -= ret;

	if ((hba->sdev_ufs_device != NULL)
		&& (hba->sdev_ufs_device->rev != NULL))
		snprintf(rev, 5, "%.4s", hba->sdev_ufs_device->rev);

	ret = snprintf(dsm_log_buff, buff_size, "%s\n", rev);
	dsm_log_buff += ret;
	buff_size -= ret;

	ret = snprintf(dsm_log_buff, buff_size, "%04x,", hba->manufacturer_date);
	dsm_log_buff += ret;
	buff_size -= ret;

	for (j = 0; j < strlen(g_ufs_dsm_log.dsm_log); j++)
		g_ufs_dsm_log.dsm_log[j] -= 1;

	ret = snprintf(dsm_log_buff, buff_size, "\nError Num:%lu, %s\n", code, err_msg);
	dsm_log_buff += ret;
	buff_size -= ret;

	set_dsm_dev_ic_module_name(product_name, rev);

	/*print vendor info*/
	switch (code) {
	case DSM_UFS_FASTBOOT_PWMODE_ERR:
		if (!(g_ufs_dsm_adaptor.err_type&(1 << UFS_FASTBOOT_PWMODE_ERR))) {
			pr_err("UFS DSM Error! Err Num:%lu err_type:%lu\n",
				code, g_ufs_dsm_adaptor.err_type);
			break;
		}
		ret = snprintf(dsm_log_buff, buff_size,
				"Fastboot Power Mode Change Err\n");
		dsm_log_buff += ret;
		buff_size -= ret;
		break;
	case DSM_UFS_FASTBOOT_RW_ERR:
		if (!(g_ufs_dsm_adaptor.err_type&(1 << UFS_FASTBOOT_RW_ERR))) {
			pr_err("UFS DSM Error! Err Num:%lu err_type:%lu\n",
				code, g_ufs_dsm_adaptor.err_type);
			break;
		}
		ret = snprintf(dsm_log_buff, buff_size, "Fastboot Read Write Err\n");
		dsm_log_buff += ret;
		buff_size -= ret;
		break;
	case DSM_UFS_UIC_TRANS_ERR:
	case DSM_UFS_LINE_RESET_ERR:
	case DSM_UFS_PA_INIT_ERR:
		if (!(g_ufs_dsm_adaptor.err_type&((1<<UFS_UIC_TRANS_ERR) |
			(1<<UFS_LINE_RESET_ERR) | (1 << UFS_PA_INIT_ERR)))) {
			pr_err("UFS DSM Error! Err Num: %lu, %lu\n",
				code, g_ufs_dsm_adaptor.err_type);
			break;
		}

		ret = snprintf(dsm_log_buff, buff_size, "UECPA:%08x,UECDL:%08x,UECN:%08x,UECT:%08x,UEDME:%08x\n",
			g_ufs_dsm_adaptor.uic_uecpa, g_ufs_dsm_adaptor.uic_uecdl, g_ufs_dsm_adaptor.uic_uecn,
			g_ufs_dsm_adaptor.uic_uect, g_ufs_dsm_adaptor.uic_uedme);
		dsm_log_buff += ret;
		buff_size -= ret;
		break;
	case DSM_UFS_UIC_CMD_ERR:
		if (!(g_ufs_dsm_adaptor.err_type&(1<<UFS_UIC_CMD_ERR))) {
			pr_err("UFS DSM Error! Err Num: %lu, %ld\n",
				code, g_ufs_dsm_adaptor.err_type);
			break;
		}
		ret = snprintf(dsm_log_buff, buff_size, "UIC_CMD:%08x,ARG1:%08x,ARG2:%08x,ARG3:%08x\n",
				g_ufs_dsm_adaptor.uic_info[0], g_ufs_dsm_adaptor.uic_info[1],
				g_ufs_dsm_adaptor.uic_info[2], g_ufs_dsm_adaptor.uic_info[3]);
		dsm_log_buff += ret;
		buff_size -= ret;
		break;
	case DSM_UFS_SYSBUS_ERR:
	case DSM_UFS_CONTROLLER_ERR:
	case DSM_UFS_DEV_ERR:
	case DSM_UFS_ENTER_OR_EXIT_H8_ERR:
	case DSM_UFS_INLINE_CRYPTO_ERR:
		if (!(g_ufs_dsm_adaptor.err_type & ((1 << UFS_SYSBUS_ERR) |
			(1 << UFS_CONTROLLER_ERR) | (1 << UFS_INLINE_CRYPTO_ERR) |
			(1 << UFS_DEV_ERR) | (1 << UFS_ENTER_OR_EXIT_H8_ERR)))) {
			pr_err("UFS DSM Error! Err Num: %lu, %ld\n",
				code, g_ufs_dsm_adaptor.err_type);
			break;
		}
		if (g_ufs_dsm_adaptor.err_type & (1 << UFS_INLINE_CRYPTO_ERR)) {
			ret = snprintf(dsm_log_buff, buff_size, "outstanding:0x%lx, doorbell: 0x%x\n",
				g_ufs_dsm_adaptor.ice_outstanding, g_ufs_dsm_adaptor.ice_doorbell);
			dsm_log_buff += ret;
			buff_size-= ret;
			for(i = 0; i < UFS_DMD_INDEX_SIZE; i++) {
				if(crypto_conf_index[i]) {
					ret = snprintf(dsm_log_buff, buff_size, "UTP_DESC[%d]: 0x%x\n", i, crypto_conf_index[i]);
					dsm_log_buff += ret;
					buff_size-= ret;
					crypto_conf_index[i] = 0;
				}
			}
		}
		if ((g_ufs_dsm_adaptor.err_type & (1 << UFS_SYSBUS_ERR))) {
			ret = snprintf(dsm_log_buff, buff_size, "SYSBUS error\n");
			dsm_log_buff += ret;
			buff_size -= ret;
		}
		if ((g_ufs_dsm_adaptor.err_type & (1 << UFS_CONTROLLER_ERR))) {
			ret = snprintf(dsm_log_buff, buff_size, "controller error\n");
			dsm_log_buff += ret;
			buff_size-= ret;
		}

		ret = snprintf(dsm_log_buff, buff_size, "UECDL:%08x,UECN:%08x\n", g_ufs_dsm_adaptor.uic_uecdl, g_ufs_dsm_adaptor.uic_uecn);
		dsm_log_buff += ret;
		buff_size -= ret;
		ret = snprintf(dsm_log_buff, buff_size, "UECT:%08x,UEDME:%08x\n", g_ufs_dsm_adaptor.uic_uect, g_ufs_dsm_adaptor.uic_uedme);
		dsm_log_buff += ret;
		buff_size -= ret;
		break;
	case DSM_UFS_UTP_ERR:
		if (!(g_ufs_dsm_adaptor.err_type&(1 << UFS_UTP_TRANS_ERR))) {
			pr_err("UFS DSM Error! Err Num: %lu, %lu\n",
				code, g_ufs_dsm_adaptor.err_type);
			break;
		}
		ret = snprintf(dsm_log_buff, buff_size,
				"UTP_OCS_ERR:%d\n", g_ufs_dsm_adaptor.ocs);
		dsm_log_buff += ret;
		buff_size -= ret;
		break;
	case DSM_UFS_TIMEOUT_ERR:
		if (!(g_ufs_dsm_adaptor.err_type&((1 << UFS_TIMEOUT_ERR) |
				(1 << UFS_TIMEOUT_SERIOUS)))) {
			pr_err("UFS DSM Error! Err Num: %lu, %ld\n",
				code, g_ufs_dsm_adaptor.err_type);
			break;
		}
		ret = snprintf(dsm_log_buff, buff_size,
				"UTP_tag:%02x,doorbell:%08x,outstandings:%lu, pwrmode:0x%x, tx_gear:%d, rx_gear:%d, scsi_cdb:\n",
				g_ufs_dsm_adaptor.timeout_slot_id,
				g_ufs_dsm_adaptor.tr_doorbell,
				g_ufs_dsm_adaptor.outstanding_reqs,
				g_ufs_dsm_adaptor.pwrmode,
				g_ufs_dsm_adaptor.tx_gear,
				g_ufs_dsm_adaptor.rx_gear);
		dsm_log_buff += ret;
		buff_size -= ret;
		for (i = 0; i < COMMAND_SIZE(g_ufs_dsm_adaptor.timeout_scmd[0]); i++) {
			ret = snprintf(dsm_log_buff, buff_size, "%02x", g_ufs_dsm_adaptor.timeout_scmd[i]);/*[false alarm]: buff_size is nomal*/
			dsm_log_buff += ret;
			buff_size -= ret;
		}
		break;
		case DSM_UFS_HARDWARE_ERR:
		case DSM_UFS_DEV_INTERNEL_ERR:
			if (!(g_ufs_dsm_adaptor.err_type & ((1 << UFS_HARDWARE_ERR) |
					(1 << UFS_DEV_INTERNEL_ERR)))) {
			pr_err("UFS DSM Error! Err Num: %lu, %ld\n",
				code, g_ufs_dsm_adaptor.err_type);
			break;
		}
		ret = snprintf(dsm_log_buff, buff_size,
				"scsi_status:%x scsi_cdb:\n",
				g_ufs_dsm_adaptor.scsi_status);
		dsm_log_buff += ret;
		buff_size -= ret;
		for (i = 0; i < COMMAND_SIZE(g_ufs_dsm_adaptor.timeout_scmd[0]); i++) {
			ret = snprintf(dsm_log_buff, buff_size, "%02x", g_ufs_dsm_adaptor.timeout_scmd[i]);/*[false alarm]: buff_size is nomal*/
			dsm_log_buff += ret;
			buff_size -= ret;
		}
		ret = snprintf(dsm_log_buff, buff_size,
				"\t\tResponse_code:%02x, Sense_key:%02x, ASC:%02x, ASCQ:%x\n",
				g_ufs_dsm_adaptor.sense_buffer[0], g_ufs_dsm_adaptor.sense_buffer[2],
				g_ufs_dsm_adaptor.sense_buffer[12],g_ufs_dsm_adaptor.sense_buffer[13]);
		dsm_log_buff += ret;
		buff_size -= ret;
		break;
	case DSM_UFS_LINKUP_ERR:
		if (!(g_ufs_dsm_adaptor.err_type&(1 << UFS_LINKUP_ERR))) {
			pr_err("UFS DSM Error! Err Num: %lu, %lu\n",
				code, g_ufs_dsm_adaptor.err_type);
			break;
		}
		ret = snprintf(dsm_log_buff, buff_size,
				"UFS Linkup Line not correct,"
				"lane_rx:%d, lane_tx:%d\n",
				hba->max_pwr_info.info.lane_rx,
				hba->max_pwr_info.info.lane_tx);
		dsm_log_buff += ret;
		buff_size -= ret;
		break;
	case DSM_UFS_LIFETIME_EXCCED_ERR:
		if (!(g_ufs_dsm_adaptor.err_type&(1 << UFS_LIFETIME_EXCCED_ERR))) {
			pr_err("UFS DSM Error! Err Num: %lu, %lu\n",
				code, g_ufs_dsm_adaptor.err_type);
			break;
		}
		ret = snprintf(dsm_log_buff, buff_size,
				"life_time_a:%u, life_time_b:%u\n",
				g_ufs_dsm_adaptor.lifetime_a,
				g_ufs_dsm_adaptor.lifetime_b);
		dsm_log_buff += ret;
		buff_size -= ret;
		break;
	case DSM_UFS_IO_TIMEOUT:
		break;
	case DSM_UFS_TIMEOUT_SERIOUS:
		/* This DMD is temporarily used to record ufs host reset. */
		dsm_ufs_host_reset_history_cmd(hba, dsm_log_buff, &buff_size);
#ifdef HEALTH_INFO_FACTORY_MODE
		return 0; // we donnot want to report dmd red screen in factory mode
#endif
		break;
	default:
		pr_err("unknown error code: %lu\n", code);
		return 0;
	}
	ufs_dsm_real_upload_size = sizeof(g_ufs_dsm_log.dsm_log)-buff_size;
	if (buff_size <= 0) {
		ufs_dsm_real_upload_size = sizeof(g_ufs_dsm_log.dsm_log)-1;
		pr_err("dsm log buff overflow!\n");
	}
	pr_err("dsm_log:\n %s\n", g_ufs_dsm_log.dsm_log);
	pr_err("buff_size = %d\n", buff_size);
	return 1;
}

/*lint -e648 -e845*/
#if defined(CONFIG_HISI_RPMB_UFS)
extern int get_rpmb_key_status(void);
#endif
static int dsm_ufs_utp_err_need_report(struct ufs_hba *hba)
{
	int key_status = 0;

	/* if (unlikely(hba->host->is_emulator))
		return 0; */
#if defined(CONFIG_HISI_RPMB_UFS)
	key_status = get_rpmb_key_status();
#endif
	if ((0 == key_status) && (OCS_FATAL_ERROR == g_ufs_dsm_adaptor.ocs))
		return 0;

	return 1;
}

static int dsm_ufs_uic_cmd_err_need_report(void)
{
	u32 uic_cmd = g_ufs_dsm_adaptor.uic_info[0];
	if((UIC_CMD_DME_HIBER_ENTER == uic_cmd) || (UIC_CMD_DME_HIBER_EXIT == uic_cmd))
		return 0;
	return 1;
}

static void print_uic_err_history(void)
{
	int i;

	for (i = 0; i < UIC_ERR_HISTORY_LEN; i++) {
		pr_info("transfered_bytes[%d] = %llu bytes\n", i, uic_err_history.transfered_bytes[i]);
	}
	pr_info("Pos: %u, delta_err_cnt: %lu, delta_byte_cnt: %llu\n",
		uic_err_history.pos,
		uic_err_history.delta_err_cnt,
		uic_err_history.delta_byte_cnt);
}

void ufshcd_count_data(struct scsi_cmnd *cmd, struct ufs_hba *hba)
{
	unsigned char opcode;
	unsigned int bytes;
	unsigned long long total_write = 0;
	unsigned long long total_read = 0;

	if (cmd == NULL || hba == NULL)
		return;

	bytes = cmd->sdb.length;
	opcode = cmd->cmnd[0];

	if (opcode == WRITE_6 || opcode == WRITE_10 || opcode == WRITE_16)
		total_write = max_t(u64, total_write + bytes, total_write);

	if (opcode == READ_6 || opcode == READ_10 || opcode == READ_16)
		total_read = max_t(u64, total_read + bytes, total_read);

	hba->total_count = max_t(u64, hba->total_count + total_read + total_write, hba->total_count);
}

static int dsm_ufs_uic_err_need_report(struct ufs_hba *hba)
{
	static unsigned long uic_err_cnt = 0;

	uic_err_cnt++;
	uic_err_history.transfered_bytes[uic_err_history.pos] = hba->total_count;
	uic_err_history.pos = (uic_err_history.pos + 1) % UIC_ERR_HISTORY_LEN;
	uic_err_history.delta_err_cnt = min(uic_err_cnt, (unsigned long)UIC_ERR_HISTORY_LEN);
	uic_err_history.delta_byte_cnt = hba->total_count - uic_err_history.transfered_bytes[uic_err_history.pos];

	print_uic_err_history();
	pr_info("%s:uic_err_cnt= %lu, hba->total_count = %llu\n",
		   __func__,
		   uic_err_cnt,
		   hba->total_count);

	if ((uic_err_cnt >= MIN_BER_SAMPLE_TIMES) &&
		(uic_err_history.delta_err_cnt * UFS_SIZE_1GB > uic_err_history.delta_byte_cnt))
		return 1;

	pr_info("%s: UECPA:%08x,UECDL:%08x,UECN:%08x,UECT:%08x,UEDME:%08x\n", __func__,
		   g_ufs_dsm_adaptor.uic_uecpa,
		   g_ufs_dsm_adaptor.uic_uecdl,
		   g_ufs_dsm_adaptor.uic_uecn,
		   g_ufs_dsm_adaptor.uic_uect,
		   g_ufs_dsm_adaptor.uic_uedme);
	pr_info("%s: UIC error cnt below threshold, do not report!\n", __func__);
	return 0;
}
/*lint +e648 +e845*/
static void dsm_ufs_handle_work(struct work_struct *work)
{
	struct ufs_hba *hba;
	struct delayed_work *dwork = to_delayed_work(work);

	hba = ((struct ufs_dmd_info *)(container_of(dwork, struct ufs_dmd_info, dsm_delay_work)))->hba;

	mutex_lock(&hba->ufs_dmd->eh_mutex);

	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_FASTBOOT_PWMODE_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_FASTBOOT_PWMODE_ERR, "Fastboot PwrMode Error");/* !e713*/
		clear_bit_unlock((unsigned int)UFS_FASTBOOT_PWMODE_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_FASTBOOT_RW_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_FASTBOOT_RW_ERR, "Fastboot RW Error");/* !e713*/
		clear_bit_unlock((unsigned int)UFS_FASTBOOT_RW_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_SYSBUS_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_SYSBUS_ERR, "SYSBUS  Error"); /* !e713*/
		clear_bit_unlock((unsigned int)UFS_SYSBUS_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_UIC_TRANS_ERR)) {
		if (dsm_ufs_uic_err_need_report(hba)) {
			DSM_UFS_LOG(hba, DSM_UFS_UIC_TRANS_ERR, "UIC No Fatal Error");
		}
		clear_bit_unlock(UFS_UIC_TRANS_ERR, &g_ufs_dsm_adaptor.err_type);

	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_UIC_CMD_ERR)) {
		if (dsm_ufs_uic_cmd_err_need_report()) {
			DSM_UFS_LOG(hba, DSM_UFS_UIC_CMD_ERR, "UIC Cmd Error");
		}
		clear_bit_unlock(UFS_UIC_CMD_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_CONTROLLER_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_CONTROLLER_ERR, "UFS Controller Error");
		clear_bit_unlock(UFS_CONTROLLER_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_DEV_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_DEV_ERR, "UFS Device Error");
		clear_bit_unlock(UFS_DEV_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_ENTER_OR_EXIT_H8_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_ENTER_OR_EXIT_H8_ERR, "UFS Enter/Exit H8 Fail");
		clear_bit_unlock(UFS_DEV_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_UTP_TRANS_ERR)) {
		if (dsm_ufs_utp_err_need_report(hba)) {
			DSM_UFS_LOG(hba, DSM_UFS_UTP_ERR, "UTP Transfer Error");
		}
		clear_bit_unlock(UFS_UTP_TRANS_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_TIMEOUT_ERR)) {
		/*lint -e845*/
		/*lint +e845*/
		DSM_UFS_LOG(hba, DSM_UFS_TIMEOUT_ERR, "UFS TimeOut Error");
		clear_bit_unlock(UFS_TIMEOUT_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_TIMEOUT_SERIOUS)) {
		DSM_UFS_LOG(hba, DSM_UFS_TIMEOUT_SERIOUS, "UFS_TIMEOUT_SERIOUS");
		clear_bit_unlock(UFS_TIMEOUT_SERIOUS, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_HARDWARE_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_HARDWARE_ERR, "UFS HARD WARE Error");
		clear_bit_unlock(UFS_HARDWARE_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_PA_INIT_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_PA_INIT_ERR, "UFS PA_INIT Error");
		clear_bit_unlock(UFS_PA_INIT_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_DEV_INTERNEL_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_DEV_INTERNEL_ERR, "UFS_DEV_INTERNEL_ERR");
		clear_bit_unlock(UFS_DEV_INTERNEL_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_HI1861_INTERNEL_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_HI1861_INTERNEL_ERR, "DSM_UFS_HI1861_INTERNEL_ERR");
		clear_bit_unlock(UFS_DEV_INTERNEL_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_LINKUP_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_LINKUP_ERR, "UFS Linkup Error");
		clear_bit_unlock(UFS_LINKUP_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_INLINE_CRYPTO_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_INLINE_CRYPTO_ERR, "UFS inline crypto error");
		clear_bit_unlock(UFS_INLINE_CRYPTO_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_LIFETIME_EXCCED_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_LIFETIME_EXCCED_ERR, "UFS Life time exceed");
		clear_bit_unlock(UFS_LIFETIME_EXCCED_ERR, &g_ufs_dsm_adaptor.err_type);
	}
	if (g_ufs_dsm_adaptor.err_type & (1 << UFS_LINE_RESET_ERR)) {
		DSM_UFS_LOG(hba, DSM_UFS_LINE_RESET_ERR, "UFS occur Line reset");
		clear_bit_unlock(UFS_LINE_RESET_ERR, &g_ufs_dsm_adaptor.err_type);
	}

	mutex_unlock(&hba->ufs_dmd->eh_mutex);
}

static void schedule_delayed_dsm_work(struct ufs_hba *hba, unsigned long delay)
{
	if ((dsm_ufs_enabled())&&(dsm_ufs_if_err()))
		queue_delayed_work(system_freezable_wq, &hba->ufs_dmd->dsm_delay_work, delay);
}

/**
 *uhshcd_rsp_sense_data - check rsp sense_data
 * @hba: per adapter instance
 * @lrb: pointer to local reference block of completed command
 * @scsi_status: the result based on SCSI status response
 */
static void
uhshcd_rsp_sense_data(struct ufs_hba *hba, struct ufshcd_lrb *lrbp, int scsi_status)
{
	if ((lrbp->ucd_rsp_ptr->sr.sense_data[2] & 0xf) == HARDWARE_ERROR) {
		dsm_ufs_update_scsi_info(lrbp, scsi_status, DSM_UFS_HARDWARE_ERR);
		schedule_delayed_dsm_work(hba, msecs_to_jiffies(0));
	} else if ((lrbp->ucd_rsp_ptr->sr.sense_data[2] & 0xf) == MEDIUM_ERROR) {
		dsm_ufs_update_scsi_info(lrbp, scsi_status, DSM_UFS_DEV_INTERNEL_ERR);
		schedule_delayed_dsm_work(hba, msecs_to_jiffies(0));
	}
}
void ufshcd_error_need_queue_work(struct ufs_hba *hba)
{
	bool queue_dsm_work = false;

	if (hba->errors & INT_FATAL_ERRORS ||
			(hba->errors &
				(UIC_HIBERNATE_ENTER | UIC_HIBERNATE_EXIT))) {
		queue_dsm_work = true;
		if (hba->errors &
				(UIC_HIBERNATE_ENTER | UIC_HIBERNATE_EXIT))
			dsm_ufs_update_error_info(
				hba, DSM_UFS_ENTER_OR_EXIT_H8_ERR);
		if (hba->errors & CONTROLLER_FATAL_ERROR)
			dsm_ufs_update_error_info(hba, DSM_UFS_CONTROLLER_ERR);
		if (hba->errors & DEVICE_FATAL_ERROR)
			dsm_ufs_update_error_info(hba, DSM_UFS_DEV_ERR);
		if (hba->errors & SYSTEM_BUS_FATAL_ERROR)
			dsm_ufs_update_error_info(hba, DSM_UFS_SYSBUS_ERR);
		if (hba->errors & CRYPTO_ENGINE_FATAL_ERROR)
			dsm_ufs_updata_ice_info(hba);
	}
	if (queue_dsm_work) {
		schedule_delayed_dsm_work(hba, msecs_to_jiffies(0));
	}

	return;
}

static void dsm_ufs_init(struct ufs_hba *hba)
{
	memset((void*)&uic_err_history, 0, sizeof(uic_err_history));

	ufs_dclient = dsm_register_client(&dsm_ufs);
	if (!ufs_dclient) {
		pr_err("ufs dclient init error");
		return;
	}

	ddr_dclient = dsm_register_client(&dsm_ddr);
	if (!ddr_dclient) {
		pr_err("ddr dclient init error");
		return;
	}

	dsm_ufs_restore_cmd_init(hba);

	INIT_DELAYED_WORK(&hba->ufs_dmd->dsm_delay_work, dsm_ufs_handle_work);
	mutex_init(&g_ufs_dsm_log.lock);
	dsm_ufs_enable = 1;
	async_schedule(dsm_ufs_fastboot_async, hba);
}

void dsm_timeout_serious_count_enable()
{
	ufs_timeout_lock = 0;
}

void dsm_report_uic_cmd_err(struct ufs_hba *hba)
{
	unsigned long flags;

	spin_lock_irqsave(hba->host->host_lock, flags);
	dsm_ufs_update_error_info(hba, DSM_UFS_UIC_CMD_ERR);
	spin_unlock_irqrestore(hba->host->host_lock, flags);
	schedule_delayed_dsm_work(hba, msecs_to_jiffies(0));
}

void dsm_report_rsp_sense_data(struct ufs_hba *hba, int scsi_status, int result,
	struct ufshcd_lrb *lrbp)
{
	if ((scsi_status != SAM_STAT_GOOD) && (result != DID_ERROR << 16))
		uhshcd_rsp_sense_data(hba, lrbp, scsi_status);
}

void dsm_report_utp_err(struct ufs_hba *hba, int ocs)
{
	int ret;

	ret = dsm_ufs_update_ocs_info(hba, DSM_UFS_UTP_ERR, ocs);
	if (!ret)
		schedule_delayed_dsm_work(hba, msecs_to_jiffies(0));
}

void dsm_report_linereset_err(struct ufs_hba *hba, u32 line_reset_flag)
{
	if (line_reset_flag)
		dsm_ufs_update_error_info(hba, DSM_UFS_LINE_RESET_ERR);
	schedule_delayed_dsm_work(hba, msecs_to_jiffies(0));
}

void dsm_report_uic_trans_err(struct ufs_hba *hba)
{
	dsm_ufs_update_uic_info(hba, DSM_UFS_UIC_TRANS_ERR);
	schedule_delayed_dsm_work(hba, msecs_to_jiffies(0));
}

void dsm_report_pa_init_err(struct ufs_hba *hba)
{
	if (hba->uic_error & DMD_UFSHCD_UIC_DL_PA_INIT_ERROR)
		dsm_ufs_update_error_info(hba, DSM_UFS_PA_INIT_ERR);
	schedule_delayed_dsm_work(hba, msecs_to_jiffies(0));
}

void dsm_disable_uic_err(struct ufs_hba *hba)
{
	if (dsm_ufs_disable_uic_err())
		dev_err(hba->dev, "dsm uic disable bit already be set.\n");
}

void dsm_report_one_lane_err(struct ufs_hba *hba)
{
	u32 avail_lane_rx, avail_lane_tx;
	unsigned long flags;

	ufshcd_dme_peer_get(hba, UIC_ARG_MIB(PA_AVAILRXDATALANES),
			&avail_lane_rx);
	ufshcd_dme_peer_get(hba, UIC_ARG_MIB(PA_AVAILTXDATALANES),
			&avail_lane_tx);
	if ((hba->max_pwr_info.info.lane_rx < avail_lane_rx) ||
		(hba->max_pwr_info.info.lane_tx < avail_lane_tx)) {
		dev_err(hba->dev, "ufs line number is less than avail "
			"rx=%d, tx=%d, avail_rx=%d, avail_tx=%d\n",
			hba->max_pwr_info.info.lane_rx,
			hba->max_pwr_info.info.lane_tx,
			avail_lane_rx, avail_lane_tx);
		spin_lock_irqsave(hba->host->host_lock, flags);
		dsm_ufs_update_error_info(hba, DSM_UFS_LINKUP_ERR);
		spin_unlock_irqrestore(hba->host->host_lock, flags);
		schedule_delayed_dsm_work(hba, msecs_to_jiffies(60000));
	}
}

void dsm_report_scsi_cmd_timeout_err(struct ufs_hba *hba, bool found, int tag,
	struct scsi_cmnd *scmd)
{
	if (found == true) {
		dsm_ufs_update_upiu_info(hba, tag, DSM_UFS_TIMEOUT_ERR);
		/*
		* Report DSM_UFS_TIMEOUT_SERIOUS when doorbell timeout has
		* been recovered by ufshcd_host_reset_and_restore over
		* UFS_TIMEOUT_SERIOUS_THRESHOLD times. ufs_timeout_count
		* will not increase if ufshcd_host_reset_and_restore not
		* called.
		*/
		if (ufs_timeout_lock != 1) {
			ufs_timeout_lock = 1;
			ufs_timeout_count++;
		}
		schedule_delayed_dsm_work(hba, msecs_to_jiffies(0));
	} else {
		dev_warn(hba->dev,\
			"scsi cmd[%x] with tag[%x] is timeout which can't be found.",
			scmd->cmnd[0], scmd->request->tag);
	}
}

void dsm_ufs_initialize(struct ufs_hba *hba)
{
	hba->ufs_dmd = kzalloc(sizeof(struct ufs_dmd_info), GFP_KERNEL);
	if (!hba->ufs_dmd)
		return;
	hba->ufs_dmd->hba = hba;

	mutex_init(&hba->ufs_dmd->eh_mutex);
	dsm_ufs_init(hba);
}

/*lint +e747 +e713 +e715 +e732 +e835*/
