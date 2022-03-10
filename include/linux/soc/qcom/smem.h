/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __QCOM_SMEM_H__
#define __QCOM_SMEM_H__
#include <linux/types.h>

#define QCOM_SMEM_HOST_ANY -1
#define RTC_INFO_SIZE      16

enum {
	SMEM_ID_VENDOR1 = 135,
};

struct smem_exten_huawei_paramater {
 	unsigned int lpddr_id;    /* DDR ID */
 	unsigned int reserve; /* reserve for byte Alignment */
 	char rtc_offset_info[RTC_INFO_SIZE]; /* rtc offset */
 	unsigned int boot_stage;
};

int qcom_smem_alloc(unsigned host, unsigned item, size_t size);
void *qcom_smem_get(unsigned host, unsigned item, size_t *size);

int qcom_smem_get_free_space(unsigned host);

phys_addr_t qcom_smem_virt_to_phys(void *p);

#endif
