

#include "ufs_device_vendor_modify.h"
#include "unipro.h"
#include <linux/of.h>

int ufs_adapt_interface(struct ufs_hba *hba,
		struct ufs_pa_layer_attr *pwr_mode)
{
	int ret = 0;

	if (pwr_mode->gear_tx == UFS_HS_G4) {
		if (hba->dev_info.wmanufacturerid == UFS_VENDOR_SANDISK) {
			ufshcd_dme_set(hba, UIC_ARG_MIB(PA_TXHSADAPTTYPE), PA_NO_ADAPT);
		} else {
			if (hba->dev_info.wmanufacturerid != UFS_VENDOR_HISI) {
				u32 peer_rx_hs_adapt_initial_cap;
				ret = ufshcd_dme_peer_get(hba,
					UIC_ARG_MIB_SEL(RX_HS_ADAPT_INITIAL_CAPABILITY,
					UIC_ARG_MPHY_RX_GEN_SEL_INDEX(0)),
					&peer_rx_hs_adapt_initial_cap);
				if (ret) {
					dev_err(hba->dev,
						"%s: RX_HS_ADAPT_INITIAL_CAP get failed %d\n",
						__func__, ret);
					peer_rx_hs_adapt_initial_cap =
						PA_PEERRXHSADAPTINITIAL_Default;
				}
				ret = ufshcd_dme_set(hba, UIC_ARG_MIB(PA_PEERRXHSADAPTINITIAL),
					peer_rx_hs_adapt_initial_cap);
			}
			/* INITIAL ADAPT */
			ufshcd_dme_set(hba, UIC_ARG_MIB(PA_TXHSADAPTTYPE),
				PA_INITIAL_ADAPT);
			}
	} else {
		/* NO ADAPT */
		ufshcd_dme_set(hba, UIC_ARG_MIB(PA_TXHSADAPTTYPE), PA_NO_ADAPT);
	}
	return ret;
}

/*
 * ufs_qcom_parse_clkscaling - read from DTS whether clkscaling modes should be disabled.
 */
void ufs_qcom_parse_clkscaling(struct ufs_qcom_host *host)
{
	struct device_node *node = host->hba->dev->of_node;

	host->disable_clkscaling = of_property_read_bool(node, "qcom,disable-clkscaling");
	if (host->disable_clkscaling)
		dev_info(host->hba->dev, "(%s) All clkscaling is disabled\n",
			__func__);
}

