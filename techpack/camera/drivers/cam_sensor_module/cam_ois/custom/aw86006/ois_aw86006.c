/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor AW86006 OIS driver
 */

#include <linux/module.h>
#include <linux/string.h>
#include <linux/firmware.h>
#include <linux/dma-contiguous.h>
#include <cam_sensor_cmn_header.h>
#include <cam_ois_core.h>
#include <cam_ois_soc.h>
#include <cam_sensor_util.h>
#include <cam_debug_util.h>
#include <cam_res_mgr_api.h>
#include <cam_common_util.h>
#include <cam_packet_util.h>
#include "vendor_ois_core.h"
#include "ois_aw86006.h"
#include "securec.h"

#define AW_FLASH_BASE_ADDR             0x01000000
#define AW_FLASH_APP_ADDR              0x01002000
#define AW_FLASH_MOVE_LENGTH           (AW_FLASH_APP_ADDR - AW_FLASH_BASE_ADDR)
#define AW_FLASH_FULL_SIZE             0x10000
#define AW_FLASH_FIRMWARE_SIZE         0xA000
#define AW_FLASH_TOP_ADDR              (AW_FLASH_BASE_ADDR + AW_FLASH_FULL_SIZE)
#define AW_ERROR_LOOP                  5
#define AW_FLASH_ERASE_LEN             512
#define AW_FLASH_WRITE_LEN             64
#define AW_FLASH_READ_LEN              64
#define AW_SHUTDOWN_I2C_ADDR           (0xC2 >> 1)
#define AW_WAKEUP_I2C_ADDR             (0xD2 >> 1)
#define AW_FW_INFO_LENGTH              64
#define AW_FW_SHIFT_IDENTIFY           (AW_FLASH_MOVE_LENGTH)
#define AW_FW_SHIFT_CHECKSUM           (AW_FW_SHIFT_IDENTIFY + 8)
#define AW_FW_SHIFT_APP_CHECKSUM       (AW_FW_SHIFT_CHECKSUM + 4)
#define AW_FW_SHIFT_CHECKSUM_ADDR      (AW_FW_SHIFT_APP_CHECKSUM)
#define AW_FW_SHIFT_APP_LENGTH         (AW_FW_SHIFT_APP_CHECKSUM + 4)
#define AW_FW_SHIFT_APP_VERSION        (AW_FW_SHIFT_APP_LENGTH + 4)
#define AW_FW_SHIFT_APP_ID             (AW_FW_SHIFT_APP_VERSION + 4)
#define AW_FW_SHIFT_MOVE_CHECKSUM      (AW_FW_SHIFT_APP_ID + 4)
#define AW_FW_SHIFT_MOVE_VERSION       (AW_FW_SHIFT_MOVE_CHECKSUM + 4)
#define AW_FW_SHIFT_MOVE_LENGTH        (AW_FW_SHIFT_MOVE_VERSION + 4)
#define AW_FW_SHIFT_UPDATE_FLAG        (AW_FW_SHIFT_MOVE_LENGTH + 8)
#define AW_ARRAY_SHIFT_UPDATE_FLAG     (AW_FW_SHIFT_UPDATE_FLAG - AW_FLASH_MOVE_LENGTH)
#define AW86006_CHIP_ID_ADDRESS        0x0000
#define AW86006_VERSION_ADDRESS        0x0002
#define AW86006_OFFSET_0BIT            0
#define AW86006_OFFSET_8BIT            8
#define AW86006_OFFSET_16BIT           16
#define AW86006_OFFSET_24BIT           24
#define AW86006_ISP_WRITE_LOOP         5
#define AW_RESET_DELAY                 50
#define AW_JUMP_DELAY                  50
#define AW_SHUTDOWN_DELAY              2000 /* us */
#define AW_ISP_WRITE_ACK_LOOP          20
#define AW_ISP_RESET_DELAY             9
#define AW_SOC_WRITE_ACK_LOOP          20
#define AW_SOC_ACK_ERROR_LOOP          2
#define AW_SOC_RESET_DELAY             2
#define AW_SOC_ADDRESS_NONE            0x00
#define OIS_ERROR                      (-1)
#define OIS_SUCCESS                    0
#define TIME_50US                      50
#define AW_FLASH_READ_LENGTH_16BIT     16
#define AW_FLASH_READ_LENGTH_32BIT     32
#define AW_FLASH_READ_LENGTH_64BIT     64
#define AW_SOC_CONNECT_CHECK_LEN       14
#define AW_FLASH_HEAD_LEN              9

struct aw_fw_info {
	uint32_t checksum;
	uint32_t app_checksum;
	uint32_t app_length;
	uint32_t app_version;
	uint32_t app_id;
	uint32_t move_checksum;
	uint32_t move_version;
	uint32_t move_length;
	uint32_t update_flag;
	uint32_t size;
	uint8_t *data_p;
};

struct soc_protocol {
	unsigned char checksum;
	unsigned char protocol_ver;
	unsigned char addr;
	unsigned char module;
	unsigned char event;
	unsigned char len[2];
	unsigned char ack;
	unsigned char sum;
	unsigned char reserved[3];
	unsigned char ack_juge;
	unsigned char *p_data;
};

struct vendor_aw_ois_ctrl {
	bool already_download;
	struct aw_fw_info fw_info;
	uint8_t checkinfo_fw[64];
	uint8_t checkinfo_rd[64];
};

/****************this should be only global define!!!*****************/
static struct vendor_aw_ois_ctrl g_aw_ctrl;
/****************this should be only global define!!!*****************/
static int aw86006_runtime_check(struct cam_ois_ctrl_t *o_ctrl);
static int aw86006_reset(struct cam_ois_ctrl_t *o_ctrl);
static int32_t aw86006_mem_download(struct cam_ois_ctrl_t *o_ctrl, const struct firmware *fw);

static int aw_soc_flash_erase_check(struct cam_ois_ctrl_t *o_ctrl,
	unsigned int addr, unsigned int len);
static int aw_soc_flash_write_check(struct cam_ois_ctrl_t *o_ctrl,
	unsigned int addr, unsigned int block_num, unsigned char *bin_buf, unsigned int len);
static int aw_soc_flash_download_check(struct cam_ois_ctrl_t *o_ctrl,
	unsigned int addr, unsigned char *bin_buf, size_t len);
static int aw_soc_connect_check(struct cam_ois_ctrl_t *o_ctrl);
static int aw_soc_buf_build(struct cam_ois_ctrl_t *o_ctrl,
	unsigned char *buf, struct soc_protocol *soc_struct);
static int aw_soc_jump_boot(struct cam_ois_ctrl_t *o_ctrl);
static int aw_soc_flash_update(struct cam_ois_ctrl_t *o_ctrl,
	uint32_t addr, u8 *data_p, size_t fw_size);
static int aw_soc_flash_read_check(struct cam_ois_ctrl_t *o_ctrl,
	uint32_t addr, uint8_t *bin_buf, uint32_t len);
static int aw_isp_jump_move(struct cam_ois_ctrl_t *o_ctrl);

/*************** SOC ***************/
enum soc_status {
	SOC_OK = 0,
	SOC_ADDR_ERROR,
	SOC_PBUF_ERROR,
	SOC_HANK_ERROR,
	SOC_JUMP_ERROR,
	SOC_FLASH_ERROR,
	SOC_SPACE_ERROR,
};

enum soc_module {
	SOC_HANK = 0x01,
	SOC_SRAM = 0x02,
	SOC_FLASH = 0x03,
	SOC_END = 0x04,
};

enum soc_enum {
	SOC_VERSION = 0x01,
	SOC_CTL = 0x00,
	SOC_ACK = 0x01,
	SOC_ADDR = 0x84,
	SOC_READ_ADDR = 0x48,
	SOC_ERASE_STRUCT_LEN = 6,
	SOC_READ_STRUCT_LEN = 6,
	SOC_PROTOCAL_HEAD = 9,
	SOC_CONNECT_WRITE_LEN = 9,
	SOC_ERASE_WRITE_LEN = 15,
	SOC_READ_WRITE_LEN = 15,
	SOC_ERASE_BLOCK_DELAY = 8,
	SOC_WRITE_BLOCK_HEAD = 13,
	SOC_WRITE_BLOCK_DELAY = 1000, /* us */
	SOC_READ_BLOCK_DELAY = 2000, /* us */
	SOC_CONNECT_DELAY = 2000, /* us */
};

enum soc_flash_event {
	SOC_FLASH_WRITE = 0x01,
	SOC_FLASH_WRITE_ACK = 0x02,
	SOC_FLASH_READ = 0x11,
	SOC_FLASH_READ_ACK = 0x12,
	SOC_FLASH_ERASE_BLOCK = 0x21,
	SOC_FLASH_ERASE_BLOCK_ACK = 0x22,
	SOC_FLASH_ERASE_CHIP = 0x23,
	SOC_FLASH_ERASE_CHIP_ACK = 0x24,
};

enum soc_hank_event {
	SOC_HANK_CONNECT = 0x01,
	SOC_HANK_CONNECT_ACK = 0x02,
	SOC_HANK_CONNECT_ACK_LEN = 5,
	SOC_HANK_PROTICOL = 0x11,
	SOC_HANK_PROTICOL_ACK = 0x12,
	SOC_HANK_VERSION = 0x21,
	SOC_HANK_VERSION_ACK = 0x22,
	SOC_HANK_ID = 0x31,
	SOC_HANK_ID_ACK = 0x32,
	SOC_HANK_DATE = 0x33,
	SOC_HANK_DATA_ACK = 0x34,
};

/*************** ISP ***************/
enum isp_event_status {
	ISP_EVENT_IDLE = 0x00,
	ISP_EVENT_OK = 0x01,
	ISP_EVENT_ERR = 0x02,
};

enum isp_enum {
	ISP_FLASH_JUMP_DELAY = 2000, /* us */
	ISP_FLASH_HANK_DELAY = 2000, /* us */
	ISP_ERASE_BLOCK_DELAY = 8,
	ISP_FLASH_WRITE_DELAY = 600, /* us */
	ISP_FLASH_WRITE_HEAD_LEN = 8,
	ISP_JUMP_ACK_LEN = 1,
	ISP_HANK_ACK_LEN = 5,
	ISP_ERASE_ACK_LEN = 1,
	ISP_WRITE_ACK_LEN = 1,
	ISP_VERSION_ACK_LEN = 5,
	ISP_READ_VERSION_DELAY = 500, /* us */
};

enum isp_status {
	ISP_OK = 0,
	ISP_ADDR_ERROR,
	ISP_PBUF_ERROR,
	ISP_HANK_ERROR,
	ISP_JUMP_ERROR,
	ISP_FLASH_ERROR,
	ISP_SPACE_ERROR,
};

enum isp_boot_module {
	ISP_BOOT_VERS = 0x00,
	ISP_BOOT_SRAM = 0x20,
	ISP_BOOT_FLASH = 0x30,
	ISP_BOOT_REG = 0x40,
	ISP_BOOT_END = 0xF0,
};

enum isp_vers_event {
	ISP_VERS_VERSION = 0x00,
	ISP_VERS_CONNECT_ACK = 0x01,
};

enum isp_end_event {
	ISP_END_SUSPEND = 0x00,
	ISP_END_JUMP = 0x02,
	ISP_END_JUMP_FLASH = 0x04,
	ISP_END_JUMP_FLASH_ACK = 0x05,
	ISP_END_JUMP_RAM = 0x06,
	ISP_END_JUMP_RAM_ACK = 0x07,
	ISP_END_JUMP_UBOOT = 0x08,
	ISP_END_JUMP_SOC_ACK = 0x09,
};

enum isp_flash_event {
	ISP_FLASH_WRITE = 0x00,
	ISP_FLASH_WRITE_ACK = 0x01,
	ISP_FLASH_READ = 0x02,
	ISP_FLASH_READ_ACK = 0x03,
	ISP_FLASH_ERASE_BLOCK = 0x04,
	ISP_FLASH_ERASE_BLOCK_ACK = 0x05,
	ISP_FLASH_ERASE_CHIP = 0x06,
	ISP_FLASH_ERASE_CHIP_ACK = 0x07,
};

static int aw_cci_stand_read_seq(struct cam_ois_ctrl_t *o_ctrl,
	uint32_t addr, uint32_t addr_type, uint8_t *data, uint32_t num_byte)
{
	enum i2c_freq_mode temp_freq;
	int ret = -1;

	if (o_ctrl == NULL || data == NULL) {
		CAM_ERR(CAM_OIS, "Invalid Args o_ctrl: %pK, data: %pK",
			o_ctrl, data);
		return -EINVAL;
	}
	if (o_ctrl->cam_ois_state < CAM_OIS_CONFIG) {
		CAM_WARN(CAM_OIS, "Not in right state to start soc reads: %d",
			 o_ctrl->cam_ois_state);
		return -EINVAL;
	}
	temp_freq = o_ctrl->io_master_info.cci_client->i2c_freq_mode;

	/* Modify i2c freq to 100K */
	o_ctrl->io_master_info.cci_client->i2c_freq_mode = I2C_STANDARD_MODE;
	ret = camera_io_dev_read_seq(&(o_ctrl->io_master_info), addr, data,
		addr_type, CAMERA_SENSOR_I2C_TYPE_BYTE, num_byte);
	if (ret != OIS_SUCCESS) {
		CAM_ERR(CAM_OIS, "cci read failed!");
		ret = OIS_ERROR;
	}
	o_ctrl->io_master_info.cci_client->i2c_freq_mode = temp_freq;

	return ret;
}

static int aw_soc_buf_build(struct cam_ois_ctrl_t *o_ctrl, unsigned char *buf, struct soc_protocol *soc_struct)
{
	unsigned char *p_head = (unsigned char *)soc_struct;
	unsigned char i = 0;
	unsigned char checksum = 0;
	unsigned char data_sum = 0;
	int ret = OIS_ERROR;

	if ((buf == NULL) || ((soc_struct == NULL)))
		return ret;
	if (soc_struct->p_data == NULL)
		soc_struct->len[0] = 0;
	soc_struct->protocol_ver = SOC_VERSION;
	soc_struct->ack = SOC_ACK;
	soc_struct->addr = ((soc_struct->ack_juge == SOC_CTL) ? SOC_ADDR : SOC_READ_ADDR);
	for (i = 0; i < soc_struct->len[0]; i++) {
		data_sum += soc_struct->p_data[i];
		buf[i + SOC_PROTOCAL_HEAD] = soc_struct->p_data[i];
	}
	soc_struct->sum = data_sum;
	for (i = 1; i < SOC_PROTOCAL_HEAD; i++) {
		checksum += p_head[i];
		buf[i] = p_head[i];
	}
	soc_struct->checksum = checksum;
	buf[0] = p_head[0];
	ret = OIS_SUCCESS;
	return ret;
}

static int aw_soc_connect_check(struct cam_ois_ctrl_t *o_ctrl)
{
	struct soc_protocol soc_struct = {0};
	uint8_t w_buf[AW_FLASH_HEAD_LEN] = {0};
	uint8_t r_buf[AW_SOC_CONNECT_CHECK_LEN] = {0};
	uint8_t cmp_buf[5] = {0x00, 0x01, 0x00, 0x00, 0x00};
	int ret = OIS_ERROR;
	int loop = AW_SOC_ACK_ERROR_LOOP;

	soc_struct.module = SOC_HANK;
	do {
		soc_struct.event = SOC_HANK_CONNECT;
		soc_struct.len[0] = 0;
		soc_struct.p_data = NULL;
		soc_struct.ack_juge = SOC_CTL;
		aw_soc_buf_build(o_ctrl, w_buf, &soc_struct);

		ret = ois_i2c_block_write_addr8_value8(o_ctrl, w_buf[0],
			&w_buf[1], SOC_CONNECT_WRITE_LEN - 1, 0);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "cci write error:%d", ret);
			continue;
		}
		usleep_range(SOC_CONNECT_DELAY, SOC_CONNECT_DELAY + TIME_50US);
		ret = aw_cci_stand_read_seq(o_ctrl, AW_SOC_ADDRESS_NONE,
			CAMERA_SENSOR_I2C_TYPE_BYTE, &r_buf[0], AW_SOC_CONNECT_CHECK_LEN);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "cci read error:%d", ret);
			continue;
		}
		soc_struct.event = SOC_HANK_CONNECT_ACK;
		soc_struct.ack_juge = SOC_ACK;
		soc_struct.len[0] = SOC_HANK_CONNECT_ACK_LEN;
		soc_struct.p_data = cmp_buf;
		aw_soc_buf_build(o_ctrl, w_buf, &soc_struct);
		ret = memcmp(w_buf, r_buf, AW_SOC_CONNECT_CHECK_LEN);
		if (ret != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "soc read check error");
			continue;
		} else {
			break;
		}
	} while (loop--);
	if (loop == 0) {
		CAM_ERR(CAM_OIS, "flash connect error!");
		return OIS_ERROR;
	}
	return ret;
}

static int aw_soc_flash_erase_check(struct cam_ois_ctrl_t *o_ctrl,
	unsigned int addr, unsigned int len)
{
	struct soc_protocol soc_struct = {0};
	unsigned int erase_block = len / AW_FLASH_ERASE_LEN + ((len % AW_FLASH_ERASE_LEN) ? 1 : 0);
	unsigned char i = 0;
	unsigned char temp_buf[6] = {0}; /* 2bytes + 4bytes = 6bytes */
	unsigned char cmp_buf[1] = {0};
	unsigned char w_buf[15] = {0}; /* temp_buf + AW_FLASH_HEAD_LEN = 15 */
	unsigned char r_buf[10] = {0}; /* cmp_buf + AW_FLASH_HEAD_LEN = 10 */
	int ret = OIS_ERROR;
	int loop = AW_SOC_ACK_ERROR_LOOP;

	temp_buf[0] = (unsigned char)erase_block;
	temp_buf[1] = 0x00;
	for (i = 0; i < 4; i++)
		temp_buf[i + 2] = (unsigned char)(addr >> (i * 8));
	soc_struct.module = SOC_FLASH;
	do {
		soc_struct.event = SOC_FLASH_ERASE_BLOCK;
		soc_struct.len[0] = SOC_ERASE_STRUCT_LEN;
		soc_struct.p_data = temp_buf;
		soc_struct.ack_juge = SOC_CTL;
		aw_soc_buf_build(o_ctrl, w_buf, &soc_struct);
		ret = ois_i2c_block_write_addr8_value8(o_ctrl, w_buf[0],
			&w_buf[1], SOC_ERASE_WRITE_LEN - 1, 0);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "cci write error:%d", ret);
			continue;
		}
		msleep(erase_block * SOC_ERASE_BLOCK_DELAY);
		ret = aw_cci_stand_read_seq(o_ctrl, AW_SOC_ADDRESS_NONE, CAMERA_SENSOR_I2C_TYPE_BYTE,
			&r_buf[0], 10); /* cmp_buf + AW_FLASH_HEAD_LEN = 10 */
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "cci read error:%d", ret);
			continue;
		}
		soc_struct.event = SOC_FLASH_ERASE_BLOCK_ACK;
		soc_struct.len[0] = 1;
		soc_struct.ack_juge = SOC_ACK;
		soc_struct.p_data = cmp_buf;
		aw_soc_buf_build(o_ctrl, w_buf, &soc_struct);
		ret = memcmp(w_buf, r_buf, 10); /* cmp_buf + AW_FLASH_HEAD_LEN = 10 */
		if (ret != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "soc read check error");
			continue;
		} else {
			break;
		}
	} while (loop--);
	if (loop == 0) {
		CAM_ERR(CAM_OIS, "flash erase error!");
		return OIS_ERROR;
	}
	return ret;
}

static int aw_soc_flash_read_check(struct cam_ois_ctrl_t *o_ctrl,
		uint32_t addr, uint8_t *bin_buf, uint32_t len)
{
	struct soc_protocol soc_struct = {0};
	uint8_t temp_buf[SOC_READ_STRUCT_LEN] = {0};
	uint8_t w_buf[SOC_READ_WRITE_LEN] = {0};
	uint8_t r_buf[100] = {0};
	uint8_t checksum = 0;
	uint8_t i = 0;
	int ret = OIS_ERROR;
	int loop = AW_SOC_ACK_ERROR_LOOP;

	temp_buf[0] = len;
	temp_buf[1] = 0;
	for (i = 0; i < 4; i++)
		temp_buf[i + 2] = (uint8_t) (addr >> (i * AW86006_OFFSET_8BIT));
	soc_struct.module = SOC_FLASH;
	do {
		soc_struct.event = SOC_FLASH_READ;
		soc_struct.len[0] = SOC_READ_STRUCT_LEN;
		soc_struct.p_data = temp_buf;
		soc_struct.ack_juge = SOC_CTL;
		aw_soc_buf_build(o_ctrl, w_buf, &soc_struct);

		ret = ois_i2c_block_write_addr8_value8(o_ctrl, w_buf[0],
			&w_buf[1], SOC_READ_WRITE_LEN - 1, 0);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "cci write error:%d", ret);
			continue;
		}
		usleep_range(SOC_READ_BLOCK_DELAY, SOC_READ_BLOCK_DELAY + TIME_50US);

		ret = aw_cci_stand_read_seq(o_ctrl, AW_SOC_ADDRESS_NONE,
			CAMERA_SENSOR_I2C_TYPE_BYTE, &r_buf[0], AW_SOC_CONNECT_CHECK_LEN + len);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "cci read error:%d", ret);
			continue;
		}
		/* check error flag */
		if (r_buf[9] != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "error flag wrong:%d", r_buf[10]);
			continue;
		}
		checksum = 0;
		/* compute data checksum */
		for (i = 0; i < len + 5; i++)
			checksum += r_buf[AW_FLASH_HEAD_LEN + i];
		if (checksum != r_buf[8]) {
			CAM_ERR(CAM_OIS, "data checksum error:0x%02x != 0x%02x", checksum, r_buf[8]);
			continue;
		}
		checksum = 0;
		/* compute head checksum */
		for (i = 1; i < 9; i++)
			checksum += r_buf[i];
		if (checksum != r_buf[0]) {
			CAM_ERR(CAM_OIS, "head checksum error:0x%02x != 0x%02x", checksum, r_buf[0]);
			continue;
		} else {
			ret = memcpy_s(bin_buf, len, &r_buf[AW_SOC_CONNECT_CHECK_LEN], len);
			break;
		}
	} while (loop--);
	if (loop == 0) {
		CAM_ERR(CAM_OIS, "flash read error!");
		return OIS_ERROR;
	}
	return ret;
}

static int aw_soc_flash_write_check(struct cam_ois_ctrl_t *o_ctrl,
	unsigned int addr, unsigned int block_num, unsigned char *bin_buf, unsigned int len)
{
	struct soc_protocol soc_struct = {0};
	unsigned char temp_buf[68] = {0}; /* 64bytes + 32bits = 68 bytes */
	unsigned char w_buf[77] = {0}; /* 64 + 10 + 4 = 77 bytes */
	unsigned char r_buf[10] = {0}; /* cmp_buf + AW_FLASH_HEAD_LEN = 10 */
	unsigned char cmp_buf[1] = {0};
	unsigned char i = 0;
	int ret = OIS_ERROR;
	int loop = AW_SOC_ACK_ERROR_LOOP;

	for (i = 0; i < 4; i++)
		temp_buf[i] = (unsigned char)((addr + block_num * AW_FLASH_WRITE_LEN) >> (i * 8));
	for (i = 0; i < len; i++)
		temp_buf[i + 4] = (bin_buf + block_num * AW_FLASH_WRITE_LEN)[i];
	soc_struct.module = SOC_FLASH;
	do {
		soc_struct.event = SOC_FLASH_WRITE;
		soc_struct.len[0] = (unsigned char)(4 + len);
		soc_struct.p_data = temp_buf;
		soc_struct.ack_juge = SOC_CTL;
		aw_soc_buf_build(o_ctrl, w_buf, &soc_struct);

		ret = ois_i2c_block_write_addr8_value8(o_ctrl, w_buf[0],
			&w_buf[1], SOC_WRITE_BLOCK_HEAD - 1 + len, 0);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "cci write error:%d", ret);
			continue;
		}
		usleep_range(SOC_WRITE_BLOCK_DELAY, SOC_WRITE_BLOCK_DELAY + TIME_50US);
		ret = aw_cci_stand_read_seq(o_ctrl, AW_SOC_ADDRESS_NONE, CAMERA_SENSOR_I2C_TYPE_BYTE,
			&r_buf[0], 10);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "cci read error:%d", ret);
			continue;
		}
		soc_struct.event = SOC_FLASH_WRITE_ACK;
		soc_struct.len[0] = 1;
		soc_struct.p_data = cmp_buf;
		soc_struct.ack_juge = SOC_ACK;
		aw_soc_buf_build(o_ctrl, w_buf, &soc_struct);
		ret = memcmp(w_buf, r_buf, 10); /* cmp_buf + AW_FLASH_HEAD_LEN = 10 */
		if (ret != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "soc read check error");
			continue;
		} else {
			break;
		}
	} while (loop--);
	if (loop == 0) {
		CAM_ERR(CAM_OIS, "flash write error!");
		return OIS_ERROR;
	}
	return ret;
}

static int aw_soc_flash_download_check(struct cam_ois_ctrl_t *o_ctrl,
	unsigned int addr, unsigned char *bin_buf, size_t len)
{
	unsigned int flash_block = len / AW_FLASH_WRITE_LEN;
	unsigned int flash_tail = len % AW_FLASH_WRITE_LEN;
	unsigned int flash_checkinfo = 0;
	unsigned int i = 0;
	int ret = OIS_ERROR;
#ifdef AW_FLASH_READBACK_CHECK
	uint8_t temp[64] = {0}; /* max write 64 bytes */
#endif

	if (addr == AW_FLASH_BASE_ADDR) {
		flash_checkinfo = AW_FLASH_MOVE_LENGTH / AW_FLASH_WRITE_LEN;
		/* first erase app data */
		ret = aw_soc_flash_erase_check(o_ctrl, AW_FLASH_APP_ADDR, len - AW_FLASH_MOVE_LENGTH);
		if (ret != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "erase checkinfo error!");
			return OIS_ERROR;
		}
		/* then erase move data */
		ret = aw_soc_flash_erase_check(o_ctrl, AW_FLASH_BASE_ADDR, AW_FLASH_MOVE_LENGTH);
		if (ret != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "flash erase error!");
			return OIS_ERROR;
		}
		for (i = 0; i < flash_checkinfo; i++) {
			ret = aw_soc_flash_write_check(o_ctrl, addr, i, bin_buf, AW_FLASH_WRITE_LEN);
			if (ret != OIS_SUCCESS) {
				CAM_ERR(CAM_OIS, "flash write block:%d error!", i);
				return OIS_ERROR;
			}
#ifdef AW_FLASH_READBACK_CHECK
			ret = aw_soc_flash_read_check(o_ctrl,
				addr + i * AW_FLASH_WRITE_LEN, &temp[0], AW_FLASH_WRITE_LEN);
			if (ret != OIS_SUCCESS) {
				CAM_ERR(CAM_OIS, "flash read back block:%d error!", i);
				return OIS_ERROR;
			}
			ret = memcmp(temp, bin_buf + i * AW_FLASH_WRITE_LEN, AW_FLASH_WRITE_LEN);
			if (ret != OIS_SUCCESS) {
				CAM_ERR(CAM_OIS, "flash write date check error: %d", i);
				return OIS_ERROR;
			}
#endif
		}
		for (i = flash_checkinfo + 1; i < flash_block; i++) {
			ret = aw_soc_flash_write_check(o_ctrl, addr, i, bin_buf, AW_FLASH_WRITE_LEN);
			if (ret != OIS_SUCCESS) {
				CAM_ERR(CAM_OIS, "flash write block:%d error!", i);
				return OIS_ERROR;
			}
		}
		if (flash_tail != 0) {
			ret = aw_soc_flash_write_check(o_ctrl, addr, i, bin_buf, flash_tail);
			if (ret != OIS_SUCCESS) {
				CAM_ERR(CAM_OIS, "flash write tail error!");
				return OIS_ERROR;
			}
		}
	} else if (addr == AW_FLASH_APP_ADDR) {
		flash_checkinfo = 0;
		ret = aw_soc_flash_erase_check(o_ctrl, AW_FLASH_APP_ADDR, len);
		if (ret != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "erase app error!");
			return OIS_ERROR;
		}
		for (i = 1; i < flash_block; i++) {
			ret = aw_soc_flash_write_check(o_ctrl, addr, i, bin_buf, AW_FLASH_WRITE_LEN);
				if (ret != OIS_SUCCESS) {
					CAM_ERR(CAM_OIS, "flash write block:%d error!", i);
					return OIS_ERROR;
			}
		}
		if (flash_tail != 0) {
			ret = aw_soc_flash_write_check(o_ctrl, addr, i, bin_buf, flash_tail);
			if (ret != OIS_SUCCESS) {
				CAM_ERR(CAM_OIS, "flash write tail error!");
				return OIS_ERROR;
			}
		}
	} else {
		CAM_ERR(CAM_OIS, "wrong addr!");
		return OIS_ERROR;
	}

	/* Final flash write checkinfo data */
	ret = aw_soc_flash_write_check(o_ctrl, AW_FLASH_APP_ADDR,
		0, g_aw_ctrl.checkinfo_fw, AW_FLASH_WRITE_LEN);
	if (ret != OIS_SUCCESS) {
		CAM_ERR(CAM_OIS, "flash write checkinfo error!");
		ret = OIS_ERROR;
	}
	return ret;
}

static int aw_soc_jump_boot(struct cam_ois_ctrl_t *o_ctrl)
{
	uint8_t boot_cmd[3] = {0xAC, 0xAC, 0xAC};
	int ret = OIS_SUCCESS;
	int i;

	CAM_DBG(CAM_OIS, "enter");
	ret = aw86006_reset(o_ctrl);
	if (ret < 0) {
		CAM_ERR(CAM_OIS, "AW86006 reset error:%d", ret);
		return ret;
	}
	mdelay(AW_SOC_RESET_DELAY);
	for (i = 0; i < AW_SOC_WRITE_ACK_LOOP; i++)
		ois_i2c_block_write_addr8_value8(o_ctrl, boot_cmd[0],
			&boot_cmd[1], sizeof(boot_cmd) - 1, 0);
	ret = aw_soc_connect_check(o_ctrl);
	if (ret < 0) {
		CAM_ERR(CAM_OIS, "connect failed: %d", ret);
		ret = OIS_ERROR;
	} else {
		CAM_INFO(CAM_OIS, "connect success!");
		ret = OIS_SUCCESS;
	}
	return ret;
}

static int aw_soc_flash_update(struct cam_ois_ctrl_t *o_ctrl, uint32_t addr, u8 *data_p, size_t fw_size)
{
	int ret;
	int i;

	CAM_INFO(CAM_OIS, "update 0x%08x via soc", addr);
	if (!data_p) {
		CAM_ERR(CAM_OIS, "error allocating memory");
		return OIS_ERROR;
	}
	/* enter boot mode */
	for (i = 0; i < AW_ERROR_LOOP; i++) {
		ret = aw_soc_jump_boot(o_ctrl);
		if (ret != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "jump boot failed!,loop:%d", i);
		} else {
			CAM_DBG(CAM_OIS, "jump boot success");
			break;
		}
		if (i == (AW_ERROR_LOOP - 1))
			return OIS_ERROR;
	}
	ret = aw_soc_flash_download_check(o_ctrl, addr, data_p, fw_size);
	if (ret != OIS_SUCCESS) {
		CAM_ERR(CAM_OIS, "flash download failed!");
		return ret;
	}
	ret = aw86006_reset(o_ctrl);
	if (ret != OIS_SUCCESS) {
		CAM_ERR(CAM_OIS, "reset failed!");
		return ret;
	}
	msleep(AW_RESET_DELAY);
	CAM_DBG(CAM_OIS, "soc update success!");
	return ret;
}

static int aw86006_reset(struct cam_ois_ctrl_t *o_ctrl)
{
	uint8_t boot_cmd_1[10] = {0xFF, 0xF0, 0x20, 0x20, 0x02, 0x02, 0x19, 0x29, 0x19, 0x29};
	uint8_t boot_cmd_2[3] = {0xFF, 0xC4, 0xC4};
	uint8_t boot_cmd_3[3] = {0xFF, 0xC4, 0x00};
	uint8_t boot_cmd_4[3] = {0xFF, 0x10, 0xC3};
	uint16_t temp_addr;
	int ret;

	/* first: shutdown */
	temp_addr = o_ctrl->io_master_info.cci_client->sid;
	o_ctrl->io_master_info.cci_client->sid = AW_SHUTDOWN_I2C_ADDR;
	ret = ois_i2c_block_write_addr8_value8(o_ctrl, boot_cmd_1[0], &boot_cmd_1[1], 9, 0); /* 9, data length */
	if (ret < 0) {
		CAM_ERR(CAM_OIS, "write boot_cmd_1 error:%d", ret);
		goto err_exit;
	}
	ret = ois_i2c_block_write_addr8_value8(o_ctrl, boot_cmd_2[0],
		&boot_cmd_2[1], sizeof(boot_cmd_2) - 1, 0);
	if (ret < 0) {
		CAM_ERR(CAM_OIS, "write boot_cmd_2 error:%d", ret);
		goto err_exit;
	}
	usleep_range(AW_SHUTDOWN_DELAY, AW_SHUTDOWN_DELAY + TIME_50US);
	/* second: wake up */
	o_ctrl->io_master_info.cci_client->sid = AW_WAKEUP_I2C_ADDR;
	ret = ois_i2c_block_write_addr8_value8(o_ctrl, boot_cmd_3[0],
		&boot_cmd_3[1], sizeof(boot_cmd_3) - 1, 0);
	if (ret < 0) {
		CAM_ERR(CAM_OIS, "write boot_cmd_3 error:%d", ret);
		goto err_exit;
	}
	ret = ois_i2c_block_write_addr8_value8(o_ctrl, boot_cmd_4[0],
		&boot_cmd_4[1], sizeof(boot_cmd_4) - 1, 0);
	if (ret < 0) {
		CAM_ERR(CAM_OIS, "write boot_cmd_4 error:%d", ret);
		goto err_exit;
	}

	ret = OIS_SUCCESS;
err_exit:
	o_ctrl->io_master_info.cci_client->sid = temp_addr;
	return ret;
}

static int aw_isp_jump_move(struct cam_ois_ctrl_t *o_ctrl)
{
	uint8_t boot_cmd[3] = {0xF0, 0xF0, 0xF0};
	uint8_t version_cmd[2] = {0x00, 0x55};
	uint8_t version_ack[5] = {0x00};
	int ret = OIS_SUCCESS;
	uint8_t jump_loop = AW_ERROR_LOOP;
	uint32_t move_version;
	int i;

	do {
		ret = aw86006_reset(o_ctrl);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "software reset error:%d", jump_loop);
			continue;
		}
		mdelay(AW_ISP_RESET_DELAY); /* aw suggest mdelay 9ms */
		/* enter isp mode */
		for (i = 0; i < AW_ISP_WRITE_ACK_LOOP; i++) {
			ret = ois_i2c_block_write_addr8_value8(o_ctrl, boot_cmd[0],
					 &boot_cmd[1], sizeof(boot_cmd) - 1, 0);
			if (ret < 0)
				CAM_ERR(CAM_OIS, "write 0xF0 failed:%d", ret);
		}

		ret = ois_i2c_block_write_addr8_value8(o_ctrl, version_cmd[0],
				 &version_cmd[1], sizeof(version_cmd) - 1, 0);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "write version_cmd:%d", jump_loop);
			continue;
		}
		usleep_range(ISP_READ_VERSION_DELAY, ISP_READ_VERSION_DELAY + TIME_50US);
		ret = aw_cci_stand_read_seq(o_ctrl, ISP_VERS_CONNECT_ACK,
			CAMERA_SENSOR_I2C_TYPE_BYTE,
			&version_ack[0], ISP_VERSION_ACK_LEN);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "read version error:%d", jump_loop);
			continue;
		}
		if (jump_loop == 0)
			ret = OIS_ERROR;
		if (version_ack[0] != ISP_EVENT_OK) {
			CAM_ERR(CAM_OIS, "wrong version_ack:%d, loop:%d", version_ack[0], jump_loop);
			continue;
		} else {
			move_version = (version_ack[4] << AW86006_OFFSET_24BIT) |
				(version_ack[3] << AW86006_OFFSET_16BIT) |
				(version_ack[2] << AW86006_OFFSET_8BIT) | version_ack[1];
			if (move_version != g_aw_ctrl.fw_info.move_version) {
				CAM_ERR(CAM_OIS, "move_version check error: 0x%08X", move_version);
				continue;
			} else {
				CAM_DBG(CAM_OIS, "Jump move success!");
				ret = OIS_SUCCESS;
			}
			break;
		}
	} while (jump_loop--);
	return ret;
}

static int aw86006_runtime_check(struct cam_ois_ctrl_t *o_ctrl)
{
	uint16_t version = 0;
	uint16_t chip_id = 0;
	uint8_t reg_val[4] = {0};
	int ret;

	ret = aw_cci_stand_read_seq(o_ctrl, AW86006_CHIP_ID_ADDRESS,
		CAMERA_SENSOR_I2C_TYPE_WORD, &reg_val[0], 4);
	if (ret != OIS_SUCCESS) {
		CAM_ERR(CAM_OIS, "read reg failed!");
		return OIS_ERROR;
	}
	chip_id = (reg_val[0] << AW86006_OFFSET_8BIT) | (reg_val[1]);
	version = (reg_val[2] << AW86006_OFFSET_8BIT) | (reg_val[3]);
	CAM_INFO(CAM_OIS, "chip_id:0x%02X, version:0x%02X", chip_id, version);
	if ((chip_id != g_aw_ctrl.fw_info.app_id) || (version != g_aw_ctrl.fw_info.app_version))
		return OIS_ERROR;
	CAM_INFO(CAM_OIS, "pass!");
	return OIS_SUCCESS;
}


static int aw_checkinfo_analyse(struct cam_ois_ctrl_t *o_ctrl,
	uint8_t *checkinfo_rd, struct aw_fw_info *info)
{
	uint32_t temp = 0;
	int ret;
	int i = 0;
	uint32_t *check_ptr;
	char fw_check_str[] = {'A', 'W', 'I', 'N', 'I', 'C', 0, 0};

	if ((checkinfo_rd == NULL) || (info == NULL)) {
		CAM_ERR(CAM_OIS, "checkinfo empty!");
		return OIS_ERROR;
	}
	ret = memcmp(fw_check_str, (char *)checkinfo_rd, sizeof(fw_check_str));
	if (ret != OIS_SUCCESS) {
		CAM_ERR(CAM_OIS, "checkinfo not match!");
		return OIS_ERROR;
	}

	info->checksum = *(uint32_t *) (&checkinfo_rd[AW_FW_SHIFT_CHECKSUM - AW_FLASH_MOVE_LENGTH]);
	info->app_checksum = *(uint32_t *) (&checkinfo_rd[AW_FW_SHIFT_APP_CHECKSUM - AW_FLASH_MOVE_LENGTH]);
	info->app_length = *(uint32_t *) (&checkinfo_rd[AW_FW_SHIFT_APP_LENGTH - AW_FLASH_MOVE_LENGTH]) +
		AW_FW_INFO_LENGTH; /* add check_info size */
	info->app_version = *(uint32_t *) (&checkinfo_rd[AW_FW_SHIFT_APP_VERSION - AW_FLASH_MOVE_LENGTH]);
	info->app_id = (checkinfo_rd[AW_FW_SHIFT_APP_ID - AW_FLASH_MOVE_LENGTH] << AW86006_OFFSET_8BIT) |
		(checkinfo_rd[AW_FW_SHIFT_APP_ID - AW_FLASH_MOVE_LENGTH + 1]);
	info->move_checksum = *(uint32_t *) (&checkinfo_rd[AW_FW_SHIFT_MOVE_CHECKSUM - AW_FLASH_MOVE_LENGTH]);
	info->move_version = *(uint32_t *) (&checkinfo_rd[AW_FW_SHIFT_MOVE_VERSION - AW_FLASH_MOVE_LENGTH]);
	info->move_length = *(uint32_t *) (&checkinfo_rd[AW_FW_SHIFT_MOVE_LENGTH - AW_FLASH_MOVE_LENGTH]);

	CAM_INFO(CAM_OIS,
		"checkinfo: checksum:0x%08X, app_checksum:0x%08X, app_length:0x%04X, app_version:0x%04X, app_id:0x%04X",
		info->checksum, info->app_checksum, info->app_length, info->app_version, info->app_id);
	CAM_INFO(CAM_OIS, "checkinfo: move_checksum:0x%08X, move_version:0x%08X, move_length:0x%04X",
		info->move_checksum, info->move_version, info->move_length);

	/* info checksum check */
	check_ptr = (uint32_t *) &checkinfo_rd[AW_FW_SHIFT_CHECKSUM_ADDR - AW_FLASH_MOVE_LENGTH];
	for (i = 0; i < (AW_FW_INFO_LENGTH - 8 - 4) / 4; i++)
		temp += check_ptr[i];
	if (temp != info->checksum) {
		CAM_ERR(CAM_OIS, "checkinfo_rd checksum error:0x%08X != 0x%08X",
			info->checksum, temp);
		return OIS_ERROR;
	}
	CAM_INFO(CAM_OIS, "pass!");

	return OIS_SUCCESS;
}

static int32_t aw86006_mem_download(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw)
{
	int i, ret = OIS_ERROR;
	uint8_t *all_buf_ptr = (uint8_t *)fw->data;
	size_t all_buf_size = g_aw_ctrl.fw_info.app_length + AW_FLASH_MOVE_LENGTH;
	uint8_t *app_buf_ptr = (uint8_t *) fw->data + AW_FLASH_MOVE_LENGTH;
	size_t app_buf_size = g_aw_ctrl.fw_info.app_length;
	struct aw_fw_info info_rd;

	if (fw == NULL) {
		CAM_ERR(CAM_OIS, "FW Empty!");
		return OIS_ERROR;
	}

	ret = aw_soc_jump_boot(o_ctrl);
	if (ret != OIS_SUCCESS) {
		CAM_ERR(CAM_OIS, "jump boot error!");
		return OIS_ERROR;
	}
	ret = aw_soc_flash_read_check(o_ctrl, AW_FLASH_APP_ADDR,
		&g_aw_ctrl.checkinfo_rd[0], AW_FLASH_READ_LEN);
	if (ret != OIS_SUCCESS)
		CAM_ERR(CAM_OIS, "readback checkinfo error!");
	g_aw_ctrl.fw_info.update_flag = g_aw_ctrl.checkinfo_rd[AW_ARRAY_SHIFT_UPDATE_FLAG];
	if (g_aw_ctrl.fw_info.update_flag != 0x01) { /* update success last time */
		CAM_INFO(CAM_OIS, "update_flag error!");
		ret = aw_soc_flash_update(o_ctrl, AW_FLASH_BASE_ADDR, all_buf_ptr, all_buf_size);
		if (ret != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "flash update failed!");
			return ret;
		}
	} else {
		CAM_INFO(CAM_OIS, "update_flag ok!");
		ret = memcmp(g_aw_ctrl.checkinfo_rd, g_aw_ctrl.checkinfo_fw, AW_FW_INFO_LENGTH);
		if (ret != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "read checkinfo not match!");
			ret = aw_checkinfo_analyse(o_ctrl, g_aw_ctrl.checkinfo_rd, &info_rd);
			if ((ret != OIS_SUCCESS) || (info_rd.move_version != g_aw_ctrl.fw_info.move_version)) {
				CAM_ERR(CAM_OIS, "checkinfo or move not match, update all!");
				ret = aw_soc_flash_update(o_ctrl, AW_FLASH_BASE_ADDR,
					all_buf_ptr, all_buf_size);
				if (ret != OIS_SUCCESS) {
					CAM_ERR(CAM_OIS, "flash update failed!");
					return ret;
				}
			} else if ((info_rd.app_version != g_aw_ctrl.fw_info.app_version) ||
				(info_rd.app_id != g_aw_ctrl.fw_info.app_id)) {
				CAM_ERR(CAM_OIS, "app not match, update app!");
				ret = aw_soc_flash_update(o_ctrl, AW_FLASH_APP_ADDR,
					app_buf_ptr, app_buf_size);
				if (ret != OIS_SUCCESS) {
					CAM_ERR(CAM_OIS, "flash update failed!");
					return ret;
				}
			} else {
				CAM_ERR(CAM_OIS, "other errors, update all!");
				ret = aw_soc_flash_update(o_ctrl, AW_FLASH_BASE_ADDR,
					all_buf_ptr, all_buf_size);
				if (ret != OIS_SUCCESS) {
					CAM_ERR(CAM_OIS, "flash update failed!");
					return ret;
				}
			}
		} else {
			ret = aw_isp_jump_move(o_ctrl);
			if (ret != OIS_SUCCESS) {
				CAM_ERR(CAM_OIS, "aw_isp_jump_move fail, update all!");
				ret = aw_soc_flash_update(o_ctrl, AW_FLASH_BASE_ADDR,
					all_buf_ptr, all_buf_size);
				if (ret != OIS_SUCCESS) {
					CAM_ERR(CAM_OIS, "flash update failed!");
					return ret;
				}
			} else {
				ret = aw86006_reset(o_ctrl);
				if (ret != OIS_SUCCESS) {
					CAM_ERR(CAM_OIS, "reset failed!");
					return OIS_ERROR;
				}
				msleep(AW_RESET_DELAY);
			}
		}
	}

	for (i = 0; i <= AW_ERROR_LOOP; i++) {
		if (aw86006_runtime_check(o_ctrl) == OIS_SUCCESS) {
			CAM_DBG(CAM_OIS, "runtime_check pass, no need to update fw!");
			ret = OIS_SUCCESS;
			break;
		} else {
			CAM_ERR(CAM_OIS, "runtime_check failed! loop:%d", i);
		}
		if (i == AW_ERROR_LOOP) {
			CAM_ERR(CAM_OIS, "fw update failed!");
			ret = OIS_ERROR;
			break;
		}
		ret = aw_soc_flash_update(o_ctrl, AW_FLASH_APP_ADDR, app_buf_ptr, app_buf_size);
		if (ret != OIS_SUCCESS) {
			CAM_ERR(CAM_OIS, "flash update failed! loop:%d", i);
			continue;
		}
	}
	return ret;
}

static int32_t aw86006_download_ois_fw(struct cam_ois_ctrl_t *o_ctrl,
	const struct firmware *fw)
{
	int32_t ret = OIS_ERROR;

	if (g_aw_ctrl.already_download) {
		CAM_INFO(CAM_OIS, "fw already downloaded");
		aw86006_reset(o_ctrl);
		msleep(AW_RESET_DELAY);
		return OIS_SUCCESS;
	}
	ret = aw86006_mem_download(o_ctrl, fw);
	if (ret != OIS_SUCCESS) {
		CAM_ERR(CAM_OIS, "aw86006_mem_download fail:%d", ret);
		return ret;
	} else {
		g_aw_ctrl.already_download = false; /* true masked */
	}
	return OIS_SUCCESS;
}

static int aw86006_fw_analysis(const struct firmware *fw)
{
	uint32_t temp = 0;
	int i, ret;
	uint32_t *check_ptr;
	char fw_check_str[] = {'A', 'W', 'I', 'N', 'I', 'C', 0, 0};
	char *identify = (char *) fw->data + AW_FW_SHIFT_IDENTIFY;

	if (fw == NULL) {
		CAM_ERR(CAM_OIS, "FW Empty!!!");
		return OIS_ERROR;
	}
	ret = memcmp(fw_check_str, identify, 8);
	if (ret != OIS_SUCCESS) {
		CAM_ERR(CAM_OIS, "loaded wrong firmware!");
		return OIS_ERROR;
	}

	g_aw_ctrl.fw_info.size = fw->size;
	g_aw_ctrl.fw_info.checksum = *(uint32_t *) (&fw->data[AW_FW_SHIFT_CHECKSUM]);
	g_aw_ctrl.fw_info.app_checksum = *(uint32_t *) (&fw->data[AW_FW_SHIFT_APP_CHECKSUM]);
	g_aw_ctrl.fw_info.app_length = *(uint32_t *) (&fw->data[AW_FW_SHIFT_APP_LENGTH]) +
		AW_FW_INFO_LENGTH; /* add check_info size */
	g_aw_ctrl.fw_info.app_version = *(uint32_t *) (&fw->data[AW_FW_SHIFT_APP_VERSION]);
	g_aw_ctrl.fw_info.app_id = (fw->data[AW_FW_SHIFT_APP_ID] << 8) |
		(fw->data[AW_FW_SHIFT_APP_ID + 1]);
	g_aw_ctrl.fw_info.move_checksum = *(uint32_t *) (&fw->data[AW_FW_SHIFT_MOVE_CHECKSUM]);
	g_aw_ctrl.fw_info.move_version = *(uint32_t *) (&fw->data[AW_FW_SHIFT_MOVE_VERSION]);
	g_aw_ctrl.fw_info.move_length = *(uint32_t *) (&fw->data[AW_FW_SHIFT_MOVE_LENGTH]);

	CAM_INFO(CAM_OIS, "info_checksum:0x%08X, fw->size:0x%04X", g_aw_ctrl.fw_info.checksum,
		g_aw_ctrl.fw_info.size);
	CAM_INFO(CAM_OIS, "app_checksum:0x%08X, app_length:0x%04X, app_version:0x%04X, app_id:0x%04X",
		g_aw_ctrl.fw_info.app_checksum, g_aw_ctrl.fw_info.app_length,
		g_aw_ctrl.fw_info.app_version, g_aw_ctrl.fw_info.app_id);
	CAM_INFO(CAM_OIS, "move_checksum:0x%08X, move_version:0x%08X, move_length:0x%04X",
		g_aw_ctrl.fw_info.move_checksum, g_aw_ctrl.fw_info.move_version,
		g_aw_ctrl.fw_info.move_length);

	/* length check */
	temp = g_aw_ctrl.fw_info.move_length + g_aw_ctrl.fw_info.app_length;
	if (g_aw_ctrl.fw_info.size != temp) {
		CAM_ERR(CAM_OIS, "fw->size error: 0x%X != 0x%X", g_aw_ctrl.fw_info.size, temp);
		return OIS_ERROR;
	}
	/* move checksum check */
	check_ptr = (uint32_t *) &fw->data[0];
	for (i = temp = 0; i < g_aw_ctrl.fw_info.move_length / 4; i++)
		temp += check_ptr[i];
	if (temp != g_aw_ctrl.fw_info.move_checksum) {
		CAM_ERR(CAM_OIS, "move checksum error:0x%08X != 0x%08X",
			g_aw_ctrl.fw_info.move_checksum, temp);
		return OIS_ERROR;
	}

	/* info checksum check */
	check_ptr = (uint32_t *) &fw->data[AW_FW_SHIFT_CHECKSUM_ADDR];
	for (i = temp = 0; i < (AW_FW_INFO_LENGTH - 8 - 4) / 4; i++)
		temp += check_ptr[i];
	if (temp != g_aw_ctrl.fw_info.checksum) {
		CAM_ERR(CAM_OIS, "check_info checksum error:0x%08X != 0x%08X",
			g_aw_ctrl.fw_info.checksum, temp);
		return OIS_ERROR;
	}

	/* app checksum check */
	check_ptr = (uint32_t *) &fw->data[AW_FLASH_MOVE_LENGTH + AW_FW_INFO_LENGTH];
	for (i = temp = 0; i < (g_aw_ctrl.fw_info.app_length - AW_FW_INFO_LENGTH) / 4; i++)
		temp += check_ptr[i];
	if (temp != g_aw_ctrl.fw_info.app_checksum) {
		CAM_ERR(CAM_OIS, "app checksum error:0x%08X != 0x%08X",
			g_aw_ctrl.fw_info.app_checksum, temp);
		return OIS_ERROR;
	}
	ret = memcpy_s(&g_aw_ctrl.checkinfo_fw[0], AW_FW_INFO_LENGTH,
		fw->data + AW_FLASH_MOVE_LENGTH, AW_FW_INFO_LENGTH);
	CAM_INFO(CAM_OIS, "pass!");

	return OIS_SUCCESS;
}

int32_t aw86006_fw_download(struct cam_ois_ctrl_t *o_ctrl)
{
	int32_t                            rc = 0;
	const struct firmware             *fw = NULL;
	const char                        *fw_name = NULL;
	char                               name_buf[32] = {0};
	struct device                     *dev = &(o_ctrl->pdev->dev);

	if (!o_ctrl) {
		CAM_ERR(CAM_OIS, "Invalid Args");
		return -EINVAL;
	}

	if (strcmp(o_ctrl->opcode.fw_ver_name, "") == 0 ||
		strcmp(o_ctrl->opcode.fw_ver_name, "-") == 0)
			snprintf(name_buf, 32, "%s.prog", o_ctrl->ois_name);
	else
		snprintf(name_buf, 32, "%s_%s.prog", o_ctrl->ois_name,
			o_ctrl->opcode.fw_ver_name);
	fw_name = name_buf;
	CAM_DBG(CAM_OIS, "ois_name:%s, fw_ver_name:%s, fw:%s",
		o_ctrl->ois_name, o_ctrl->opcode.fw_ver_name, fw_name);
	/* Load FW */
	rc = request_firmware(&fw, fw_name, dev);
	if (rc) {
		CAM_ERR(CAM_OIS, "Failed to locate %s", fw_name);
		return rc;
	}

	/* check fw */
	rc = aw86006_fw_analysis(fw);
	if (rc != OIS_SUCCESS) {
		CAM_ERR(CAM_OIS, "aw86006_fw_analysis fail : %d", rc);
		release_firmware(fw);
		return rc;
	}

	rc = aw86006_download_ois_fw(o_ctrl, fw);
	if (rc != OIS_SUCCESS)
		CAM_ERR(CAM_OIS, "aw86006_download_ois_fw fail : %d", rc);

	release_firmware(fw);
	return rc;
}
