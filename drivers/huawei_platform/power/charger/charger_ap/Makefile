obj-$(CONFIG_HUAWEI_CHARGER) += huawei_charger/
obj-$(CONFIG_DIRECT_CHARGER) += direct_charger/
obj-$(CONFIG_WIRELESS_CHARGER) += wireless_charge/

ifeq ($(HLTHERM_RUNTEST), true)
    CFLAGS_KERNEL += -DCONFIG_HLTHERM_RUNTEST
endif
