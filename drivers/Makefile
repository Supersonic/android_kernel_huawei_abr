# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the Linux kernel device drivers.
#
# 15 Sep 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
#

obj-y				+= irqchip/
obj-y				+= bus/

obj-$(CONFIG_GENERIC_PHY)	+= phy/

# GPIO must come after pinctrl as gpios may need to mux pins etc
obj-$(CONFIG_PINCTRL)		+= pinctrl/
obj-$(CONFIG_GPIOLIB)		+= gpio/
obj-y				+= pwm/

obj-y				+= pci/

obj-$(CONFIG_PARISC)		+= parisc/
obj-$(CONFIG_RAPIDIO)		+= rapidio/
obj-y				+= video/
obj-y				+= idle/

# IPMI must come before ACPI in order to provide IPMI opregion support
obj-y				+= char/ipmi/

obj-$(CONFIG_ACPI)		+= acpi/
obj-$(CONFIG_SFI)		+= sfi/
# PnP must come after ACPI since it will eventually need to check if acpi
# was used and do nothing if so
obj-$(CONFIG_PNP)		+= pnp/
obj-y				+= amba/

obj-y				+= clk/
# Many drivers will want to use DMA so this has to be made available
# really early.
obj-$(CONFIG_DMADEVICES)	+= dma/

# SOC specific infrastructure drivers.
obj-y				+= soc/

obj-$(CONFIG_VIRTIO)		+= virtio/
obj-$(CONFIG_XEN)		+= xen/

# regulators early, since some subsystems rely on them to initialize
obj-$(CONFIG_REGULATOR)		+= regulator/

# reset controllers early, since gpu drivers might rely on them to initialize
obj-$(CONFIG_RESET_CONTROLLER)	+= reset/

# tty/ comes before char/ so that the VT console is the boot-time
# default.
obj-y				+= tty/
obj-y				+= char/

# iommu/ comes before gpu as gpu are using iommu controllers
obj-y				+= iommu/

# gpu/ comes after char for AGP vs DRM startup and after iommu
obj-y				+= gpu/

obj-$(CONFIG_CONNECTOR)		+= connector/

# i810fb and intelfb depend on char/agp/
obj-$(CONFIG_FB_I810)           += video/fbdev/i810/
obj-$(CONFIG_FB_INTEL)          += video/fbdev/intelfb/

obj-$(CONFIG_PARPORT)		+= parport/
obj-$(CONFIG_NVM)		+= lightnvm/
obj-y				+= base/ block/ misc/ mfd/ nfc/
obj-$(CONFIG_LIBNVDIMM)		+= nvdimm/
obj-$(CONFIG_DAX)		+= dax/
obj-$(CONFIG_DMA_SHARED_BUFFER) += dma-buf/
obj-$(CONFIG_NUBUS)		+= nubus/
obj-y				+= macintosh/
obj-$(CONFIG_IDE)		+= ide/
obj-y				+= scsi/
obj-y				+= nvme/
obj-$(CONFIG_ATA)		+= ata/
obj-$(CONFIG_TARGET_CORE)	+= target/
obj-$(CONFIG_MTD)		+= mtd/
obj-$(CONFIG_SPI)		+= spi/
obj-$(CONFIG_SPMI)		+= spmi/
obj-$(CONFIG_HSI)		+= hsi/
obj-$(CONFIG_SLIMBUS)		+= slimbus/
obj-y				+= net/
obj-$(CONFIG_ATM)		+= atm/
obj-$(CONFIG_FUSION)		+= message/
obj-y				+= firewire/
obj-$(CONFIG_UIO)		+= uio/
obj-$(CONFIG_VFIO)		+= vfio/
obj-y				+= cdrom/
obj-y				+= auxdisplay/
obj-$(CONFIG_PCCARD)		+= pcmcia/
obj-$(CONFIG_DIO)		+= dio/
obj-$(CONFIG_SBUS)		+= sbus/
obj-$(CONFIG_ZORRO)		+= zorro/
obj-$(CONFIG_ATA_OVER_ETH)	+= block/aoe/
obj-$(CONFIG_PARIDE) 		+= block/paride/
obj-$(CONFIG_TC)		+= tc/
obj-$(CONFIG_USB_PHY)		+= usb/
obj-$(CONFIG_USB)		+= usb/
obj-$(CONFIG_USB_SUPPORT)	+= usb/
obj-$(CONFIG_PCI)		+= usb/
obj-$(CONFIG_USB_GADGET)	+= usb/
obj-$(CONFIG_OF)		+= usb/
obj-$(CONFIG_SERIO)		+= input/serio/
obj-$(CONFIG_GAMEPORT)		+= input/gameport/
obj-$(CONFIG_INPUT)		+= input/
obj-$(CONFIG_RTC_LIB)		+= rtc/
obj-y				+= i2c/ i3c/ media/
obj-$(CONFIG_PPS)		+= pps/
obj-y				+= ptp/
obj-$(CONFIG_W1)		+= w1/
obj-y				+= power/
obj-$(CONFIG_HWMON)		+= hwmon/
obj-$(CONFIG_THERMAL)		+= thermal/
obj-$(CONFIG_WATCHDOG)		+= watchdog/
obj-$(CONFIG_MD)		+= md/
obj-$(CONFIG_BT)		+= bluetooth/
obj-$(CONFIG_ACCESSIBILITY)	+= accessibility/
obj-$(CONFIG_ISDN)		+= isdn/
obj-$(CONFIG_EDAC)		+= edac/
obj-$(CONFIG_EISA)		+= eisa/
obj-$(CONFIG_PM_OPP)		+= opp/
obj-$(CONFIG_CPU_FREQ)		+= cpufreq/
obj-$(CONFIG_CPU_IDLE)		+= cpuidle/
obj-y				+= mmc/
obj-$(CONFIG_HUAWEI_DSM_IOMT_HOST)     += iomt_host/
obj-$(CONFIG_MEMSTICK)		+= memstick/
obj-$(CONFIG_NEW_LEDS)		+= leds/
obj-$(CONFIG_INFINIBAND)	+= infiniband/
obj-y				+= firmware/
obj-$(CONFIG_CRYPTO)		+= crypto/
obj-$(CONFIG_SUPERH)		+= sh/
ifndef CONFIG_ARCH_USES_GETTIMEOFFSET
obj-y				+= clocksource/
endif
obj-$(CONFIG_DCA)		+= dca/
obj-$(CONFIG_HID)		+= hid/
obj-$(CONFIG_PPC_PS3)		+= ps3/
obj-$(CONFIG_OF)		+= of/
obj-$(CONFIG_SSB)		+= ssb/
obj-$(CONFIG_BCMA)		+= bcma/
obj-$(CONFIG_VHOST_RING)	+= vhost/
obj-$(CONFIG_VHOST)		+= vhost/
obj-$(CONFIG_VLYNQ)		+= vlynq/
obj-$(CONFIG_GREYBUS)		+= greybus/
obj-$(CONFIG_STAGING)		+= staging/
obj-y				+= platform/

obj-$(CONFIG_MAILBOX)		+= mailbox/
obj-$(CONFIG_HWSPINLOCK)	+= hwspinlock/
obj-$(CONFIG_REMOTEPROC)	+= remoteproc/
obj-$(CONFIG_RPMSG)		+= rpmsg/
obj-$(CONFIG_SOUNDWIRE)		+= soundwire/

# Virtualization drivers
obj-$(CONFIG_VIRT_DRIVERS)	+= virt/
obj-$(CONFIG_HYPERV)		+= hv/

obj-$(CONFIG_HWDPS)		+= hwdps/

obj-$(CONFIG_PM_DEVFREQ)	+= devfreq/
obj-$(CONFIG_EXTCON)		+= extcon/
obj-$(CONFIG_MEMORY)		+= memory/
obj-$(CONFIG_IIO)		+= iio/
obj-$(CONFIG_VME_BUS)		+= vme/
obj-$(CONFIG_IPACK_BUS)		+= ipack/
obj-$(CONFIG_NTB)		+= ntb/
obj-$(CONFIG_POWERCAP)		+= powercap/
obj-$(CONFIG_MCB)		+= mcb/
obj-$(CONFIG_PERF_EVENTS)	+= perf/
obj-$(CONFIG_RAS)		+= ras/
obj-$(CONFIG_THUNDERBOLT)	+= thunderbolt/
obj-$(CONFIG_CORESIGHT)		+= hwtracing/coresight/
obj-y				+= hwtracing/intel_th/
obj-$(CONFIG_STM)		+= hwtracing/stm/
obj-$(CONFIG_ANDROID)		+= android/
obj-$(CONFIG_NVMEM)		+= nvmem/
obj-$(CONFIG_FPGA)		+= fpga/
obj-$(CONFIG_FSI)		+= fsi/
obj-$(CONFIG_TEE)		+= tee/
obj-$(CONFIG_MULTIPLEXER)	+= mux/
obj-$(CONFIG_UNISYS_VISORBUS)	+= visorbus/
obj-$(CONFIG_SIOX)		+= siox/
obj-$(CONFIG_GNSS)		+= gnss/
obj-$(CONFIG_HW_GADGET)		+= hwusb/
obj-$(CONFIG_LOGGER_EXTEND)	+= log/
obj-$(CONFIG_HUAWEI_DSM)	+= dsm/
obj-y                    	+= camera_debug/
obj-y                           += dmd/
obj-$(CONFIG_AUDIOKIT)          += devkit/audiokit/
obj-y                           += hwpower/
obj-y                           += huawei_platform/
obj-$(CONFIG_LCD_KIT_DRIVER)    += devkit/lcdkit/
obj-y                           += devkit/tpkit/
obj-y                           += irda/
obj-y                           += dp/
obj-$(CONFIG_INTERCONNECT)	+= interconnect/
obj-$(CONFIG_COUNTER)		+= counter/
obj-$(CONFIG_HW_RECLAIM_ACCT) += reclaim_acct/
obj-$(CONFIG_HW_PAGE_TRACKER)	+= hw_page_tracker/
obj-$(CONFIG_HW_MEMORY_MONITOR)	+= mmonitor/
obj-$(CONFIG_HW_MEMORY_MONITOR)	+= allocpages_delayacct/
obj-$(CONFIG_RUNMODE_TYPE_PARSE) += hwmanufac/
obj-$(CONFIG_MODEM_RESET) += modem_reset/
obj-y                           += vibrator/
obj-$(CONFIG_HW_BOOSTER) += booster/
obj-y                           += bootdevice/
obj-$(CONFIG_HW_MEMCHECK)		+= hwmemcheck/
obj-m		+= rphone/
obj-$(CONFIG_CHR_NETLINK_MODULE) += chr/
obj-$(CONFIG_HW_DPIMARK_MODULE) += hw_dpi_mark/
obj-y += hwril/
obj-$(CONFIG_HW_WIFIPRO) += hwnet/
obj-$(CONFIG_MEM_INSPECT)  += rmeminspect/
obj-$(CONFIG_HW_NETWORK_AWARE) += network_aware/
obj-$(CONFIG_HW_ZEROHUNG) += hwzerohung/
obj-$(CONFIG_RFFE_ANTENNA) += antenna_cable/
obj-$(CONFIG_HUAWEI_EMCOM) += emcom/
obj-$(CONFIG_HW_CPULOAD_NOTI) += cpuload/
obj-y                           += hwsensor/
obj-y                           += hwsensors/
obj-$(CONFIG_CARD_TRAY_GPIO_DETECT) += card_tray/
obj-$(CONFIG_HUAWEI_BOOT_TIME) += boottime/
obj-y += hwril_sim_sd/
obj-y                           += blpwm/

obj-y                           += cfi/
obj-$(CONFIG_HUAWEI_WIFI) += hw_wlan/
obj-$(CONFIG_HUAWEI_BT_FM) += hw_btfm/
obj-$(CONFIG_HUAWEI_PERF_CTRL) += perf_ctrl/
obj-$(CONFIG_HW_PERFHUB) += perfhub/
