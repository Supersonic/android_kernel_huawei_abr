# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the kernel block layer
#

obj-$(CONFIG_BLOCK) := bio.o elevator.o blk-core.o blk-sysfs.o \
			blk-flush.o blk-settings.o blk-ioc.o blk-map.o \
			blk-exec.o blk-merge.o blk-softirq.o blk-timeout.o \
			blk-lib.o blk-mq.o blk-mq-tag.o blk-stat.o \
			blk-mq-sysfs.o blk-mq-cpumap.o blk-mq-sched.o ioctl.o \
			genhd.o partition-generic.o ioprio.o \
			badblocks.o partitions/ blk-rq-qos.o
obj-$(CONFIG_MAS_BLK) += mas_blk_core.o mas_blk_dump.o mas_blk_latency.o mas_blk_busy_idle_notify.o \
					mas_blk_flush.o mas_blk_iosched_ufs_mq.o mas_blk_iosched_interface.o \
					mas_blk_busy_idle_interface.o mas_blk_core_interface.o mas_blk_dump_interface.o \
					mas_blk_flush_interface.o mas_blk_latency_interface.o mas_blk_mq_tag.o \
					mas_blk_unistore_interface.o
obj-$(CONFIG_MAS_UNISTORE_PRESERVE) += mas_blk_unistore.o mas_blk_unistore_register.o mas_blk_unistore_sysfs.o
obj-$(CONFIG_MAS_BLK_DEBUG) += mas_blk_ft.o
obj-$(CONFIG_MAS_BKOPS) += mas_bkops_core.o mas_bkops_sysfs.o mas_bkops_core_interface.o \
					mas_bkops_sysfs_interface.o
obj-$(CONFIG_BOUNCE)		+= bounce.o
obj-$(CONFIG_BLK_SCSI_REQUEST)	+= scsi_ioctl.o
obj-$(CONFIG_BLK_DEV_BSG)	+= bsg.o
obj-$(CONFIG_BLK_DEV_BSGLIB)	+= bsg-lib.o
obj-$(CONFIG_BLK_CGROUP)	+= blk-cgroup.o
obj-$(CONFIG_BLK_CGROUP_IOSMART)	+= blk_iosmart.o
obj-$(CONFIG_BLK_DEV_THROTTLING)	+= blk-throttle.o
obj-$(CONFIG_BLK_CGROUP_IOLATENCY)	+= blk-iolatency.o
obj-$(CONFIG_BLK_CGROUP_IOCOST)	+= blk-iocost.o
obj-$(CONFIG_MQ_IOSCHED_DEADLINE)	+= mq-deadline.o
obj-$(CONFIG_MQ_IOSCHED_KYBER)	+= kyber-iosched.o
bfq-y				:= bfq-iosched.o bfq-wf2q.o bfq-cgroup.o
obj-$(CONFIG_IOSCHED_BFQ)	+= bfq.o

obj-$(CONFIG_BLOCK_COMPAT)	+= compat_ioctl.o
obj-$(CONFIG_BLK_CMDLINE_PARSER)	+= cmdline-parser.o
obj-$(CONFIG_BLK_DEV_INTEGRITY) += bio-integrity.o blk-integrity.o t10-pi.o
obj-$(CONFIG_BLK_MQ_PCI)	+= blk-mq-pci.o
obj-$(CONFIG_BLK_MQ_VIRTIO)	+= blk-mq-virtio.o
obj-$(CONFIG_BLK_MQ_RDMA)	+= blk-mq-rdma.o
obj-$(CONFIG_BLK_DEV_ZONED)	+= blk-zoned.o
obj-$(CONFIG_BLK_WBT)		+= blk-wbt.o
obj-$(CONFIG_BLK_DEBUG_FS)	+= blk-mq-debugfs.o
obj-$(CONFIG_BLK_DEBUG_FS_ZONED)+= blk-mq-debugfs-zoned.o
obj-$(CONFIG_BLK_SED_OPAL)	+= sed-opal.o
obj-$(CONFIG_BLK_PM)		+= blk-pm.o
obj-$(CONFIG_BLK_INLINE_ENCRYPTION)	+= keyslot-manager.o bio-crypt-ctx.o \
					   blk-crypto.o
obj-$(CONFIG_BLK_INLINE_ENCRYPTION_FALLBACK)	+= blk-crypto-fallback.o
#obj-$(CONFIG_HUAWEI_DSM)	+= dsm_block.o
ifeq ($(TARGET_VERSION_MODE),factory)
	EXTRA_CFLAGS += -DBYPASS_AUTHORITY_VERIFY
endif
