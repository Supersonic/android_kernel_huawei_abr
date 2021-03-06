/*
 * huawei_usb.h
 *
 * huawei usb header file
 *
 * Copyright (c) 2018-2019 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */
#ifndef __HUAWEI_USB_H__
#define __HUAWEI_USB_H__

#define COUNTRY_US         "us"
#define COUNTRY_JAPAN      "jp"
#define VENDOR_TMOBILE     "t-mobile"
#define VENDOR_EMOBILE     "emobile"
#define VENDOR_TRACFONE    "tracfone"
#define VENDOR_CC          "consumercellular"
#define VENDOR_SOFTBANK    "softbank"

#define USB_DEFAULT_SN     "0123456789ABCDEF"
#define USB_SERIAL_LEN     32
#define VENDOR_NAME_LEN    32
#define COUNTRY_NAME_LEN   32

/* support 3 luns, 1 for cdrom and 2 for udisk */
#define USB_MAX_LUNS       3

#define SC_REWIND          0x01
#define SC_REWIND_11       0x11

#define ORI_INDEX         0

/* READ_TOC command structure */
typedef struct _usbsdms_read_toc_cmd_type {
	u8 op_code;

	/*
	 * bit1 is MSF, 0: address format is LBA form
	 * 1: address format is MSF form
	 */
	u8 msf;

	/*
	 * bit3~bit0,   MSF Field   Track/Session Number
	 * 0000b:       Valid       Valid as a Track Number
	 * 0001b:       Valid       Ignored by Drive
	 * 0010b:       Ignored     Valid as a Session Number
	 * 0011b~0101b: Ignored     Ignored by Drive
	 * 0110b~1111b: Reserved
	 */
	u8 format;
	u8 reserved1;
	u8 reserved2;
	u8 reserved3;
	u8 session_num; /* a specific session or a track */
	u8 allocation_length_msb;
	u8 allocation_length_lsb;
	u8 control;
} usbsdms_read_toc_cmd_type;

#define USB_IF_CLASS_HW_PNP21       0xff
#define USB_IF_SUBCLASS_HW_PNP21    0x11

#define USB_IF_PROTOCOL_HW_MODEM    0x21
#define USB_IF_PROTOCOL_HW_PCUI     0x22
#define USB_IF_PROTOCOL_HW_DIAG     0x23
#define USB_IF_PROTOCOL_NOPNP       0xff

#define USB_LOGS_ERR 1
#define USB_LOGS_INFO 2
#define USB_LOGS_DBG 3

#define usb_logs_err(fmt, x...) \
do { \
	if ((KERNEL_HWFLOW) && (usb_debug >= USB_LOGS_ERR)) { \
		printk(KERN_ERR "USB_LOGS_ERR:" pr_fmt(fmt), ##x); \
	} \
} while (0)

#define usb_logs_info(fmt, x...) \
do { \
	if ((KERNEL_HWFLOW) && (usb_debug >= USB_LOGS_INFO)) { \
		printk(KERN_ERR "USB_LOGS_INFO:" pr_fmt(fmt), ##x); \
	} \
} while (0)

#define usb_logs_dbg(fmt, x...) \
do { \
	if ((KERNEL_HWFLOW) && (usb_debug >= USB_LOGS_DBG)) { \
		printk(KERN_ERR "USB_LOGS_DBG:" pr_fmt(fmt), ##x); \
	} \
} while (0)

#define usb_dev_err(dev, fmt, x...) \
do { \
	if ((KERNEL_HWFLOW) && (usb_debug >= USB_LOGS_ERR)) { \
		dev_printk(KERN_ERR, dev, "USB_LOGS_ERR:" fmt, ##x); \
	} \
} while (0)

#define usb_dev_info(dev, fmt, x...) \
do { \
	if ((KERNEL_HWFLOW) && (usb_debug >= USB_LOGS_INFO)) { \
		dev_printk(KERN_ERR, dev, "USB_LOGS_INFO:" fmt, ##x); \
	} \
} while (0)

#define usb_dev_dbg(dev, fmt, x...) \
do { \
	if ((KERNEL_HWFLOW) && (usb_debug >= USB_LOGS_DBG)) { \
		dev_printk(KERN_ERR, dev, "USB_LOGS_DBG:" fmt, ##x); \
	} \
} while (0)

void usb_port_switch_request(int usb_pid_index);
#endif /* __HUAWEI_USB_H__ */
