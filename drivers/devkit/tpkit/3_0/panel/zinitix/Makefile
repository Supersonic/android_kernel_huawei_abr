#drivers/input/touchscreen
#
#Makefile for the touch driver of Zinitix
obj-$(CONFIG_ZINITIX_TS_KIT_3_0) += zinitix_ts.o
zinitix_ts-objs += zinitix_core.o zinitix_dts.o #zinitix_test.o zinitix_flash.o 
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/tzdriver
EXTRA_CFLAGS += -I$(srctree)/drivers/tzdriver
EXTRA_CFLAGS += -I$(srctree)/drivers/devkit/lcdkit/lcdkit3.0/kernel/common/include
EXTRA_CFLAGS += -I$(srctree)/drivers/devkit/tpkit/3_0
