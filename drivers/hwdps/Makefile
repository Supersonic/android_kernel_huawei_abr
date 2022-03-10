
obj-$(CONFIG_HWDPS) += src/base/hwdps_utils.o

obj-$(CONFIG_HWDPS) += src/fek/hwdps_fs_callbacks.o

obj-$(CONFIG_HWDPS) += src/ioctl/hwdps_ioctl.o \
		       src/ioctl/hwdps_ioctl_impl.o

obj-$(CONFIG_HWDPS) += src/policy/hwdps_policy.o

obj-$(CONFIG_HWDPS) += src/data/hwdps_packages.o \
		       src/data/hwdps_data.o

obj-$(CONFIG_HWDPS) += kernel_hook/hwdps/hwdps_fs_hooks.o

EXTRA_CFLAGS := -I../../include/ \
		-Iinc/base \
		-Iinc/data \
		-Iinc/fek \
		-Iinc/ioctl \
		-Iinc/policy \
		-Idrivers/hisi/tzdriver \
		-I$(srctree)/lib/libc_sec/securec_v2/include

flags += -pipe -Wdate-time -Wfloat-equal -Wformat=2
