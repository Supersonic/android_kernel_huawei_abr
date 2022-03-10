#ifndef __UFSTT_H
#define __UFSTT_H

#include "ufshcd.h"

#ifdef CONFIG_HUAWEI_UFS_TURBO_TABLE

/* BYTE SHIFT */
#define ZERO_BYTE_SHIFT			0
#define ONE_BYTE_SHIFT			8
#define TWO_BYTE_SHIFT			16
#define THREE_BYTE_SHIFT		24
#define FOUR_BYTE_SHIFT			32
#define FIVE_BYTE_SHIFT			40
#define SIX_BYTE_SHIFT			48
#define SEVEN_BYTE_SHIFT		56

#define SHIFT_BYTE_0(num)		((num) << ZERO_BYTE_SHIFT)
#define SHIFT_BYTE_1(num)		((num) << ONE_BYTE_SHIFT)
#define SHIFT_BYTE_2(num)		((num) << TWO_BYTE_SHIFT)
#define SHIFT_BYTE_3(num)		((num) << THREE_BYTE_SHIFT)
#define SHIFT_BYTE_4(num)		((num) << FOUR_BYTE_SHIFT)
#define SHIFT_BYTE_5(num)		((num) << FIVE_BYTE_SHIFT)
#define SHIFT_BYTE_6(num)		((num) << SIX_BYTE_SHIFT)
#define SHIFT_BYTE_7(num)		((num) << SEVEN_BYTE_SHIFT)

#define GET_BYTE_0(num)			(((num) >> ZERO_BYTE_SHIFT) & 0xff)
#define GET_BYTE_1(num)			(((num) >> ONE_BYTE_SHIFT) & 0xff)
#define GET_BYTE_2(num)			(((num) >> TWO_BYTE_SHIFT) & 0xff)
#define GET_BYTE_3(num)			(((num) >> THREE_BYTE_SHIFT) & 0xff)
#define GET_BYTE_4(num)			(((num) >> FOUR_BYTE_SHIFT) & 0xff)
#define GET_BYTE_5(num)			(((num) >> FIVE_BYTE_SHIFT) & 0xff)
#define GET_BYTE_6(num)			(((num) >> SIX_BYTE_SHIFT) & 0xff)
#define GET_BYTE_7(num)			(((num) >> SEVEN_BYTE_SHIFT) & 0xff)

#define UFSTT_READ_BUFFER		0xF9
#define UFSTT_READ_BUFFER_ID		0x01
#define UFSTT_READ_BUFFER_CONTROL	0x00

#define UFSHCD_CAP_HPB_TURBO_TABLE (1 << 25)
#define VENDOR_FEATURE_TURBO_TABLE BIT(0)
/* Device descriptor parameters offsets in bytes */
enum ttunit_desc_param {
	TTUNIT_DESC_PARAM_LEN			= 0x0,
	TTUNIT_DESC_PARAM_DESCRIPTOR_IDN	= 0x1,
	TTUNIT_DESC_PARAM_TURBO_TABLE_EN	= 0x2,
	TTUNIT_DESC_PARAM_L2P_SIZE		= 0x3,
};

#define SAM_STAT_GOOD_NEED_UPDATE 0x80
#define UPIU_CMD_PRIO             0x04
#define QUERY_DESC_TTUNIT_MAX_SIZE 0x04
#define DEVICE_DESC_PARAM_FEATURE  0xF0
#define UPIU_QUERY_OPCODE_VENDOR_READ 0xF8
#define UPIU_QUERY_OPCODE_VENDOR_WRITE 0xF9


/* turbo table parameters offsets in bytes */
enum ufstt_desc_id {
	TURBO_TABLE_READ_BITMAP		= 0x4,
	VENDOR_IDN_TT_UNIT_RD		= 0x05,
	DEVICE_CAPABILITY_FLAG		= 0x06,
	VENDOR_IDN_TT_UNIT_WT		= 0x08,
	SET_TZ_STREAM_ID		= 0x0C,
};

struct ufstt_info {
	bool ufstt_enabled; 	/* turbo table workable */
	bool in_shutdown;
	bool in_suspend;
	bool ufstt_batch_mode;
	void *ufstt_private;
};

#define ufstt_hex_dump(prefix_str, buf, len)                                   \
	print_hex_dump(KERN_ERR, prefix_str, DUMP_PREFIX_OFFSET, 16, 4, buf,   \
		       len, false)

void ufstt_set_sdev(struct scsi_device *dev);
void ufstt_probe(struct ufs_hba *hba);
void ufstt_remove(struct ufs_hba *hba);
void ufstt_prep_fn(struct ufs_hba *hba, struct ufshcd_lrb *lrbp);
void ufstt_unprep_fn(struct ufs_hba *hba, struct ufshcd_lrb *lrbp);
void ufstt_idle_handler(struct ufs_hba *hba);
void ufstt_unprep_handler(struct ufs_hba *hba, struct ufshcd_lrb *lrbp);
void ufstt_node_update(void);
bool is_ufstt_batch_mode(void);
void ufstt_suspend(struct ufs_hba *hba, enum ufs_pm_op pm_op);
void ufstt_shutdown(struct ufs_hba *hba);
void ufstt_resume(struct ufs_hba *hba);

#else

#define UPIU_QUERY_OPCODE_VENDOR_READ 0xF8
#define UPIU_QUERY_OPCODE_VENDOR_WRITE 0xF9

static inline void ufstt_set_sdev(struct scsi_device *dev){};
static inline void ufstt_probe(struct ufs_hba *hba){};
static inline void ufstt_remove(struct ufs_hba *hba){};
static inline void ufstt_prep_fn(struct ufs_hba *hba, struct ufshcd_lrb *lrbp){};
static inline void ufstt_unprep_fn(struct ufs_hba *hba, struct ufshcd_lrb *lrbp){};
static inline void ufstt_idle_handler(struct ufs_hba *hba){};
static inline void ufstt_unprep_handler(struct ufs_hba *hba, struct ufshcd_lrb *lrbp){};
static inline void ufstt_node_update(void){};
static inline void ufstt_suspend(struct ufs_hba *hba, enum ufs_pm_op pm_op){};
static inline void ufstt_shutdown(struct ufs_hba *hba){};
static inline void ufstt_resume(struct ufs_hba *hba){};

#endif

#endif
