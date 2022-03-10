#ifndef LINUX_DSM_UFS_H
#define LINUX_DSM_UFS_H

#include <linux/time.h>
#include <dsm/dsm_pub.h>
#include "ufs.h"
#include "ufshcd.h"

#ifndef min
	#define min(x,y) ((x) < (y) ? x : y)
#endif

#define UFS_DSM_BUFFER_SIZE                4096
#define DDR_DSM_BUFFER_SIZE               2048
#define UFS_MSG_MAX_SIZE                   256
#define UIC_TRANSFER_TIME_LIMIT_MS         (600 * 1000)
#define UFS_ERR_REG_HIST_LENGTH 8

#define DSM_UFS_FASTBOOT_PWMODE_ERR     928008000
#define DSM_UFS_FASTBOOT_RW_ERR         928008001
#define DSM_UFS_SYSBUS_ERR              928008002
#define DSM_UFS_UIC_TRANS_ERR           928008003
#define DSM_UFS_UIC_CMD_ERR             928008004
#define DSM_UFS_CONTROLLER_ERR          928008005
#define DSM_UFS_DEV_ERR                 928008006
#define DSM_UFS_TIMEOUT_ERR             928008007
#define DSM_UFS_UTP_ERR                 928008008
#define DSM_UFS_ASSERT                  928008009
#define DSM_UFS_FFU_FAIL_ERR            928008010
#define DSM_UFS_LINKUP_ERR              928008011
#define DSM_UFS_ENTER_OR_EXIT_H8_ERR    928008012
#define DSM_UFS_LIFETIME_EXCCED_ERR     928008013
#define DSM_UFS_TIMEOUT_SERIOUS         928008014
#define DSM_UFS_DEV_INTERNEL_ERR        928008015
#define DSM_UFS_PA_INIT_ERR             928008016
#define DSM_UFS_INLINE_CRYPTO_ERR       928008017
#define DSM_UFS_HARDWARE_ERR            928008018
#define DSM_UFS_LINE_RESET_ERR          928008019
#define DSM_UFS_HI1861_INTERNEL_ERR     928008021
#define DSM_UFS_IO_TIMEOUT              928008027

#define FASTBOOTDMD_PWR_ERR  BIT(4) /*Power mode change err ,set bit4*/
#define FASTBOOTDMD_RW_ERR  BIT(15) /*read and write err, set bit15*/
#define TEN_BILLION_BITS 10000000000
#ifdef DSM_FACTORY_MODE
#define MIN_BER_SAMPLE_TIMES 3
#else
#define MIN_BER_SAMPLE_TIMES 10
#endif
#define UIC_ERR_HISTORY_LEN 10

#define DMD_UFSHCD_UIC_DL_PA_INIT_ERROR (1 << 0)

struct ufs_dsm_log {
	char dsm_log[UFS_DSM_BUFFER_SIZE];
	struct mutex lock; /*mutex*/
};
extern struct dsm_client *ufs_dclient;
extern unsigned int ufs_dsm_real_upload_size;

struct ufs_uic_err_history {
	unsigned long long transfered_bytes[UIC_ERR_HISTORY_LEN]; /* unit: bytes */
	u32 pos;
	unsigned long delta_err_cnt;
	unsigned long long delta_byte_cnt;
};


struct ufs_dsm_adaptor {
	unsigned long err_type;
#define UFS_FASTBOOT_PWMODE_ERR     0
#define UFS_FASTBOOT_RW_ERR         1
#define UFS_SYSBUS_ERR              2
#define UFS_UIC_TRANS_ERR           3
#define UFS_UIC_CMD_ERR             4
#define UFS_CONTROLLER_ERR          5
#define UFS_DEV_ERR                 6
#define UFS_TIMEOUT_ERR             7
#define UFS_UTP_TRANS_ERR           8
#define UFS_ASSERT                  9
#define UFS_FFU_FAIL_ERR            10
#define UFS_LINKUP_ERR              11
#define UFS_ENTER_OR_EXIT_H8_ERR    12
#define UFS_LIFETIME_EXCCED_ERR     13
#define UFS_TIMEOUT_SERIOUS         14
#define UFS_DEV_INTERNEL_ERR        15
#define UFS_PA_INIT_ERR             16
#define UFS_INLINE_CRYPTO_ERR       17
#define UFS_HARDWARE_ERR            18
#define UFS_LINE_RESET_ERR          19
#define UFS_HI1861_INTERNEL_ERR     21
#define UFS_SIZE_1GB (1024UL * 1024UL * 1024UL)

	/*for UIC Transfer Error*/
	unsigned long uic_disable;
	u32 uic_uecpa;
	u32 uic_uecdl;
	u32 uic_uecn;
	u32 uic_uect;
	u32 uic_uedme;
	/*for UIC Command Error*/
	u32 uic_info[4];
	/*for time out error*/
	int timeout_slot_id;
	unsigned long outstanding_reqs;
	u32 tr_doorbell;
	unsigned char timeout_scmd[MAX_CDB_SIZE];
	int scsi_status;
	int ocs;
	char sense_buffer[SCSI_SENSE_BUFFERSIZE];
	uint16_t manufacturer_id;
	unsigned long ice_outstanding;
	u32 ice_doorbell;
	u8 lifetime_a;
	u8 lifetime_b;
	u32 pwrmode;
	u32 tx_gear;
	u32 rx_gear;
	int temperature;
};
extern struct ufs_dsm_log g_ufs_dsm_log;

#define DPO_SHIFT 4
#define FUA_SHIFT 3
struct dsm_ufs_cmd_log_entry {
	u8 lun;
	u8 cmd_op;
	u8 dpo;
	u8 fua;
	sector_t lba;
	int transfer_len;
	unsigned int tag;
	ktime_t issue_time;
	ktime_t complete_time;
};

struct dsm_ufs_cmd_log {
	unsigned int pos;
	struct dsm_ufs_cmd_log_entry *cmd_log_entry;
};

struct ufs_dmd_info {
	struct ufs_hba *hba;
	struct mutex eh_mutex;
	struct delayed_work dsm_delay_work;
	struct dsm_ufs_cmd_log *cmd_log;
};

#ifdef CONFIG_HUAWEI_UFS_DSM
int dsm_ufs_get_log(struct ufs_hba *hba, unsigned long code, char *err_msg);
void dsm_ufs_enable_uic_err(struct ufs_hba *hba);
void ufshcd_error_need_queue_work(struct ufs_hba *hba);
void dsm_ufs_initialize(struct ufs_hba *hba);
void dsm_report_scsi_cmd_timeout_err(struct ufs_hba *hba, bool found, int tag,
	struct scsi_cmnd *scmd);
void dsm_report_one_lane_err(struct ufs_hba *hba);
void dsm_disable_uic_err(struct ufs_hba *hba);
void dsm_report_linereset_err(struct ufs_hba *hba, u32 line_reset_flag);
void dsm_report_uic_trans_err(struct ufs_hba *hba);
void dsm_report_pa_init_err(struct ufs_hba *hba);
void dsm_report_utp_err(struct ufs_hba *hba, int ocs);
void dsm_report_rsp_sense_data(struct ufs_hba *hba, int scsi_status, int result,
	struct ufshcd_lrb *lrbp);
void dsm_report_uic_cmd_err(struct ufs_hba *hba);
void dsm_timeout_serious_count_enable(void);
void dsm_ufs_restore_cmd(struct ufs_hba *hba, unsigned int tag);
void dsm_ufs_report_host_reset(struct ufs_hba *hba);
void ufshcd_count_data(struct scsi_cmnd *cmd, struct ufs_hba *hba);

#define DSM_UFS_LOG(hba, no, fmt, a...)\
	do {\
		char msg[UFS_MSG_MAX_SIZE];\
		snprintf(msg, UFS_MSG_MAX_SIZE-1, fmt, ## a);\
		mutex_lock(&g_ufs_dsm_log.lock);\
		if (dsm_ufs_get_log(hba, (no), (msg))) {\
			if (!dsm_client_ocuppy(ufs_dclient)) {\
				dsm_client_copy(ufs_dclient, \
						g_ufs_dsm_log.dsm_log, \
						ufs_dsm_real_upload_size+1);\
				dsm_client_notify(ufs_dclient, (no));\
			} \
		} \
		mutex_unlock(&g_ufs_dsm_log.lock);\
	} while (0) /* unsafe_function_ignore: snprintf */

#else
int dsm_ufs_get_log(struct ufs_hba *hba, unsigned long code, char *err_msg) {return 0; }
void dsm_ufs_enable_uic_err(struct ufs_hba *hba){return 0; };
void ufshcd_error_need_queue_work(struct ufs_hba *hba) {return 0;};
static inline  __attribute__((unused)) void
dmd_ufshcd_init(struct ufs_hba *hba
	__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
dmd_update_scsi_cmd_timeout_err(struct ufs_hba *hba, bool found, int tag,
	struct scsi_cmnd *scmd
	__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
dmd_update_linkup_err(struct ufs_hba *hba
	__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
dmd_disable_uic_err(struct ufs_hba *hba
	__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
dmd_update_linereset_err(struct ufs_hba *hba, u32 line_reset_flag
	__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
dmd_update_uic_trans_err(struct ufs_hba *hba
	__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
dmd_update_pa_init_err(struct ufs_hba *hba
	__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
dmd_update_utp_err(struct ufs_hba *hba, int ocs
	__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
dmd_update_rsp_sense_data(struct ufs_hba *hba, int scsi_status, int result,
	struct ufshcd_lrb *lrbp
	__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
dmd_update_uic_cmd_err(struct ufs_hba *hba
	__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
dsm_timeout_serious_count_enable(void
	__attribute__((unused)))
{
	return;
}

#define DSM_UFS_LOG(hba, no, fmt, a...)
#endif
#endif
