# SPDX-License-Identifier: GPL-2.0
# UFSHCD makefile
obj-$(CONFIG_SCSI_UFS_DWC_TC_PCI) += tc-dwc-g210-pci.o ufshcd-dwc.o tc-dwc-g210.o
obj-$(CONFIG_SCSI_UFS_DWC_TC_PLATFORM) += tc-dwc-g210-pltfrm.o ufshcd-dwc.o tc-dwc-g210.o
obj-$(CONFIG_SCSI_UFS_CDNS_PLATFORM) += cdns-pltfrm.o
obj-$(CONFIG_SCSI_UFS_QCOM) += ufs-qcom.o
obj-$(CONFIG_UFS_DEVICE_VENDOR_MODIFY) += ufs_device_vendor_modify.o
obj-$(CONFIG_SCSI_UFSHCD) += ufshcd-core.o
ufshcd-core-y				+= ufshcd.o ufs-sysfs.o
ufshcd-core-$(CONFIG_SCSI_UFS_BSG)	+= ufs_bsg.o
obj-$(CONFIG_SCSI_UFSHCD_PCI) += ufshcd-pci.o
obj-$(CONFIG_SCSI_UFSHCD_PLATFORM) += ufshcd-pltfrm.o
obj-$(CONFIG_SCSI_UFS_HISI) += ufs-hisi.o
obj-$(CONFIG_SCSI_UFS_MEDIATEK) += ufs-mediatek.o
ufshcd-core-$(CONFIG_SCSI_UFS_CRYPTO) += ufshcd-crypto.o
obj-$(CONFIG_SCSI_UFS_CRYPTO_QTI) += ufshcd-crypto-qti.o
obj-$(CONFIG_MAS_BLK)  += mas_ufs.o
obj-$(CONFIG_MAS_UFS_MANUAL_BKOPS) += mas-ufs-bkops.o mas_ufs_bkops_interface.o
obj-$(CONFIG_HUAWEI_UFS_VENDOR_MODE) += ufs-vendor-mode.o ufs-vendor-cmd.o ufs-vendor-extern.o
obj-$(CONFIG_HUAWEI_UFS_DSM)  += dsm_ufs.o
obj-$(CONFIG_SCSI_UFS_UNISTORE) += ufs_unistore_core.o
obj-$(CONFIG_SCSI_UFS_UNISTORE) += ufs_unistore_read.o
obj-$(CONFIG_SCSI_UFS_UNISTORE) += ufs_unistore_write.o
obj-$(CONFIG_SCSI_UFS_UNISTORE) += ufs_vcmd_proc.o
ifeq ($(TARGET_VERSION_MODE),factory)
    EXTRA_CFLAGS += -DHEALTH_INFO_FACTORY_MODE
endif
ifeq ($(TARGET_VERSION_MODE),factory)
    EXTRA_CFLAGS += -DDSM_FACTORY_MODE
endif
obj-$(CONFIG_HUAWEI_UFS_HPB) += ufs-hpb.o
obj-y += proc_ufs.o
obj-$(CONFIG_HUAWEI_UFS_DYNAMIC_SWITCH_WB)  += ufs_wb_dynamic_switch.o
obj-$(CONFIG_HUAWEI_UFS_TURBO_TABLE) += ufstt.o
obj-$(CONFIG_SCSI_UFS_HS_ERROR_RECOVER) += ufs-error-recovery.o