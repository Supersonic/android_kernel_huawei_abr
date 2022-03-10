

#ifndef __UFS_DEVICE_VENDOR_MODIFY_H__
#define __UFS_DEVICE_VENDOR_MODIFY_H__
#include "ufshcd.h"
#include "ufs-qcom.h"

#ifdef CONFIG_UFS_DEVICE_VENDOR_MODIFY
int ufs_adapt_interface(struct ufs_hba *hba,
		struct ufs_pa_layer_attr *pwr_mode);
void ufs_qcom_parse_clkscaling(struct ufs_qcom_host *host);
#else
static inline  __attribute__((unused)) int
ufs_adapt_interface(struct ufs_hba *hba, struct ufs_pa_layer_attr *pwr_mode
		__attribute__((unused)))
{
	return 0;
}
static inline  __attribute__((unused)) void
ufs_qcom_parse_clkscaling(struct ufs_qcom_host *host
		__attribute__((unused)))
{
	return;
}
#endif /* CONFIG_UFS_DEVICE_VENDOR_MODIFY */
#endif
