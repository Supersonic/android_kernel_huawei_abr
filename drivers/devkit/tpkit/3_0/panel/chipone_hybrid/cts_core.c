#define LOG_TAG         "Core"

#include "cts_config.h"
#include "cts_platform.h"
#include "cts_core.h"
#include "cts_efctrl.h"
#include "cts_emb_flash.h"
#include "cts_firmware.h"

#include "cts_strerror.h"

extern struct chipone_ts_data *cts_data;

const static struct cts_efctrl icnt8918_efctrl = {
	.reg_base = 0x040600,
	.xchg_sram_base = (48 - 1) * 1024,
	.xchg_sram_size = 256,	/* For non firmware programming */
	.ops = &cts_efctrl_ops
};

const static struct cts_device_hwdata cts_device_hwdatas[] = {
	{
	 .name = "ICNT8918",
	 .hwid = CTS_DEV_HWID_ICNT8918,
	 .fwid = CTS_DEV_FWID_ICNT8918,
	 .num_row = 8,
	 .num_col = 8,
	 .sram_size = 48 * 1024,
	 .program_addr_width = 3,
	 .efctrl = &icnt8918_efctrl,
	}
};

static int cts_i2c_writeb(const struct cts_device *cts_dev,
			  u32 addr, u8 b, int retry, int delay)
{
	u8 buff[8];

	TS_LOG_DEBUG("Write to slave_addr: 0x%02x reg: 0x%0*x val: 0x%02x",
		     cts_dev->rtdata.slave_addr, cts_dev->rtdata.addr_width * 2,
		     addr, b);

	if (cts_dev->rtdata.addr_width == 2) {
		put_unaligned_be16(addr, buff);
	} else if (cts_dev->rtdata.addr_width == 3) {
		put_unaligned_be24(addr, buff);
	} else {
		TS_LOG_ERR("Writeb invalid address width %u",
			   cts_dev->rtdata.addr_width);
		return -EINVAL;
	}
	buff[cts_dev->rtdata.addr_width] = b;

	return cts_plat_i2c_write(cts_dev->pdata, cts_dev->rtdata.slave_addr,
				  buff, cts_dev->rtdata.addr_width + 1, retry,
				  delay);
}

static int cts_i2c_writew(const struct cts_device *cts_dev,
			  u32 addr, u16 w, int retry, int delay)
{
	u8 buff[8];

	TS_LOG_DEBUG("Write to slave_addr: 0x%02x reg: 0x%0*x val: 0x%04x",
		     cts_dev->rtdata.slave_addr, cts_dev->rtdata.addr_width * 2,
		     addr, w);

	if (cts_dev->rtdata.addr_width == 2) {
		put_unaligned_be16(addr, buff);
	} else if (cts_dev->rtdata.addr_width == 3) {
		put_unaligned_be24(addr, buff);
	} else {
		TS_LOG_ERR("Writew invalid address width %u",
			   cts_dev->rtdata.addr_width);
		return -EINVAL;
	}

	put_unaligned_le16(w, buff + cts_dev->rtdata.addr_width);

	return cts_plat_i2c_write(cts_dev->pdata, cts_dev->rtdata.slave_addr,
				  buff, cts_dev->rtdata.addr_width + 2, retry,
				  delay);
}

static int cts_i2c_writel(const struct cts_device *cts_dev,
			  u32 addr, u32 l, int retry, int delay)
{
	u8 buff[8];

	TS_LOG_DEBUG("Write to slave_addr: 0x%02x reg: 0x%0*x val: 0x%08x",
		     cts_dev->rtdata.slave_addr, cts_dev->rtdata.addr_width * 2,
		     addr, l);

	if (cts_dev->rtdata.addr_width == 2) {
		put_unaligned_be16(addr, buff);
	} else if (cts_dev->rtdata.addr_width == 3) {
		put_unaligned_be24(addr, buff);
	} else {
		TS_LOG_ERR("Writel invalid address width %u",
			   cts_dev->rtdata.addr_width);
		return -EINVAL;
	}

	put_unaligned_le32(l, buff + cts_dev->rtdata.addr_width);

	return cts_plat_i2c_write(cts_dev->pdata, cts_dev->rtdata.slave_addr,
				  buff, cts_dev->rtdata.addr_width + 4, retry,
				  delay);
}

static int cts_i2c_writesb(const struct cts_device *cts_dev, u32 addr,
			   const u8 *src, size_t len, int retry, int delay)
{
	int ret;
	u8 *data;
	size_t max_xfer_size;
	size_t payload_len;
	size_t xfer_len;

	TS_LOG_DEBUG("Write to slave_addr: 0x%02x reg: 0x%0*x len: %zu",
		     cts_dev->rtdata.slave_addr, cts_dev->rtdata.addr_width * 2,
		     addr, len);

	max_xfer_size = cts_plat_get_max_i2c_xfer_size(cts_dev->pdata);
	data = cts_plat_get_i2c_xfer_buf(cts_dev->pdata, len);
	while (len) {
		payload_len =
		    min((size_t) (max_xfer_size - cts_dev->rtdata.addr_width),
			len);
		xfer_len = payload_len + cts_dev->rtdata.addr_width;

		if (cts_dev->rtdata.addr_width == 2) {
			put_unaligned_be16(addr, data);
		} else if (cts_dev->rtdata.addr_width == 3) {
			put_unaligned_be24(addr, data);
		} else {
			TS_LOG_ERR("Writesb invalid address width %u",
				   cts_dev->rtdata.addr_width);
			return -EINVAL;
		}

		memcpy(data + cts_dev->rtdata.addr_width, src, payload_len);

		ret =
		    cts_plat_i2c_write(cts_dev->pdata,
				       cts_dev->rtdata.slave_addr, data,
				       xfer_len, retry, delay);
		if (ret) {
			TS_LOG_ERR("Platform i2c write failed %d", ret);
			return ret;
		}

		src += payload_len;
		len -= payload_len;
		addr += payload_len;
	}

	return 0;
}

static int cts_i2c_readb(const struct cts_device *cts_dev,
			 u32 addr, u8 *b, int retry, int delay)
{
	u8 addr_buf[4];

	TS_LOG_DEBUG("Readb from slave_addr: 0x%02x reg: 0x%0*x",
		     cts_dev->rtdata.slave_addr, cts_dev->rtdata.addr_width * 2,
		     addr);

	if (cts_dev->rtdata.addr_width == 2) {
		put_unaligned_be16(addr, addr_buf);
	} else if (cts_dev->rtdata.addr_width == 3) {
		put_unaligned_be24(addr, addr_buf);
	} else {
		TS_LOG_ERR("Readb invalid address width %u",
			   cts_dev->rtdata.addr_width);
		return -EINVAL;
	}

	return cts_plat_i2c_read(cts_dev->pdata, cts_dev->rtdata.slave_addr,
				 addr_buf, cts_dev->rtdata.addr_width, b, 1,
				 retry, delay);
}

static int cts_i2c_readw(const struct cts_device *cts_dev,
			 u32 addr, u16 *w, int retry, int delay)
{
	int ret;
	u8 addr_buf[4];
	u8 buff[2];

	TS_LOG_DEBUG("Readw from slave_addr: 0x%02x reg: 0x%0*x",
		     cts_dev->rtdata.slave_addr, cts_dev->rtdata.addr_width * 2,
		     addr);

	if (cts_dev->rtdata.addr_width == 2) {
		put_unaligned_be16(addr, addr_buf);
	} else if (cts_dev->rtdata.addr_width == 3) {
		put_unaligned_be24(addr, addr_buf);
	} else {
		TS_LOG_ERR("Readw invalid address width %u",
			   cts_dev->rtdata.addr_width);
		return -EINVAL;
	}

	ret = cts_plat_i2c_read(cts_dev->pdata, cts_dev->rtdata.slave_addr,
				addr_buf, cts_dev->rtdata.addr_width, buff, 2,
				retry, delay);
	if (ret == 0)
		*w = get_unaligned_le16(buff);

	return ret;
}

static int cts_i2c_readl(const struct cts_device *cts_dev,
			 u32 addr, u32 *l, int retry, int delay)
{
	int ret;
	u8 addr_buf[4];
	u8 buff[4];

	TS_LOG_DEBUG("Readl from slave_addr: 0x%02x reg: 0x%0*x",
		     cts_dev->rtdata.slave_addr, cts_dev->rtdata.addr_width * 2,
		     addr);

	if (cts_dev->rtdata.addr_width == 2) {
		put_unaligned_be16(addr, addr_buf);
	} else if (cts_dev->rtdata.addr_width == 3) {
		put_unaligned_be24(addr, addr_buf);
	} else {
		TS_LOG_ERR("Readl invalid address width %u",
			   cts_dev->rtdata.addr_width);
		return -EINVAL;
	}

	ret = cts_plat_i2c_read(cts_dev->pdata, cts_dev->rtdata.slave_addr,
				addr_buf, cts_dev->rtdata.addr_width, buff, 4,
				retry, delay);
	if (ret == 0)
		*l = get_unaligned_le32(buff);

	return ret;
}

static int cts_i2c_readsb(const struct cts_device *cts_dev,
			  u32 addr, void *dst, size_t len, int retry, int delay)
{
	int ret;
	int retry_times = 0;
	u8 addr_buf[4];
	size_t max_xfer_size, xfer_len;

	TS_LOG_DEBUG("Readsb from slave_addr: 0x%02x reg: 0x%0*x len: %zu",
		     cts_dev->rtdata.slave_addr, cts_dev->rtdata.addr_width * 2,
		     addr, len);

	max_xfer_size = cts_plat_get_max_i2c_xfer_size(cts_dev->pdata);
	while (len) {
		xfer_len = min(max_xfer_size, len);

		if (cts_dev->rtdata.addr_width == 2) {
			put_unaligned_be16(addr, addr_buf);
		} else if (cts_dev->rtdata.addr_width == 3) {
			put_unaligned_be24(addr, addr_buf);
		} else {
			TS_LOG_ERR("Readsb invalid address width %u",
				   cts_dev->rtdata.addr_width);
			return -EINVAL;
		}

		retry_times = 0;
		while (retry_times < RW_RETRY_TIMES) {
			ret = cts_plat_i2c_read(cts_dev->pdata,
				cts_dev->rtdata.slave_addr, addr_buf,
				cts_dev->rtdata.addr_width, dst, xfer_len,
				retry, delay);
			if (ret) {
				TS_LOG_ERR("Platform i2c read failed %d", ret);
				retry_times++;
				msleep(CTS_DELAY_10MS);
				continue;
			} else {
				break;
			}
		}
		if (retry_times == RW_RETRY_TIMES) {
			TS_LOG_ERR("i2c read failed retry 3 times %d", ret);
			return ret;
		}

		dst += xfer_len;
		len -= xfer_len;
		addr += xfer_len;
	}

	return 0;
}

static inline int cts_dev_writeb(const struct cts_device *cts_dev,
				 u32 addr, u8 b, int retry, int delay)
{
	return cts_i2c_writeb(cts_dev, addr, b, retry, delay);
}

static inline int cts_dev_writew(const struct cts_device *cts_dev,
				 u32 addr, u16 w, int retry, int delay)
{
	return cts_i2c_writew(cts_dev, addr, w, retry, delay);
}

static inline int cts_dev_writel(const struct cts_device *cts_dev,
				 u32 addr, u32 l, int retry, int delay)
{
	return cts_i2c_writel(cts_dev, addr, l, retry, delay);
}

static inline int cts_dev_writesb(const struct cts_device *cts_dev, u32 addr,
				  const u8 *src, size_t len, int retry,
				  int delay)
{
	return cts_i2c_writesb(cts_dev, addr, src, len, retry, delay);
}

static inline int cts_dev_readb(const struct cts_device *cts_dev,
				u32 addr, u8 *b, int retry, int delay)
{
	return cts_i2c_readb(cts_dev, addr, b, retry, delay);
}

static inline int cts_dev_readw(const struct cts_device *cts_dev,
				u32 addr, u16 *w, int retry, int delay)
{
	return cts_i2c_readw(cts_dev, addr, w, retry, delay);
}

static inline int cts_dev_readl(const struct cts_device *cts_dev,
				u32 addr, u32 *l, int retry, int delay)
{
	return cts_i2c_readl(cts_dev, addr, l, retry, delay);
}

static inline int cts_dev_readsb(const struct cts_device *cts_dev,
				 u32 addr, void *dst, size_t len, int retry,
				 int delay)
{
	return cts_i2c_readsb(cts_dev, addr, dst, len, retry, delay);
}

static inline int cts_dev_readsb_delay_idle(const struct cts_device *cts_dev,
					    u32 addr, void *dst, size_t len,
					    int retry, int delay, int idle)
{
	return cts_i2c_readsb(cts_dev, addr, dst, len, retry, delay);
}

static int cts_write_sram_normal_mode(const struct cts_device *cts_dev,
				      u32 addr, const void *src, size_t len,
				      int retry, int delay)
{
	int i, ret;
	u8 buff[5];

	for (i = 0; i < len; i++) {
		put_unaligned_le32(addr, buff);
		buff[4] = *(u8 *)src;

		addr++;
		src++;

		ret = cts_dev_writesb(cts_dev,
				      CTS_DEVICE_FW_REG_DEBUG_INTF, buff, 5,
				      retry, delay);
		if (ret) {
			TS_LOG_ERR("Write rDEBUG_INTF len=5B failed %d", ret);
			return ret;
		}
	}

	return 0;
}

int cts_sram_writeb_retry(const struct cts_device *cts_dev,
			  u32 addr, u8 b, int retry, int delay)
{
	if (cts_dev->rtdata.program_mode) {
		return cts_dev_writeb(cts_dev, addr, b, retry, delay);
	} else {
		return cts_write_sram_normal_mode(cts_dev, addr, &b, 1, retry,
						  delay);
	}
}

int cts_sram_writew_retry(const struct cts_device *cts_dev,
			  u32 addr, u16 w, int retry, int delay)
{
	u8 buff[2];

	if (cts_dev->rtdata.program_mode) {
		return cts_dev_writew(cts_dev, addr, w, retry, delay);
	} else {
		put_unaligned_le16(w, buff);

		return cts_write_sram_normal_mode(cts_dev, addr, buff, 2, retry,
						  delay);
	}
}

int cts_sram_writel_retry(const struct cts_device *cts_dev,
			  u32 addr, u32 l, int retry, int delay)
{
	u8 buff[4];

	if (cts_dev->rtdata.program_mode) {
		return cts_dev_writel(cts_dev, addr, l, retry, delay);
	} else {
		put_unaligned_le32(l, buff);

		return cts_write_sram_normal_mode(cts_dev, addr, buff, 4, retry,
						  delay);
	}
}

int cts_sram_writesb_retry(const struct cts_device *cts_dev,
			   u32 addr, const void *src, size_t len, int retry,
			   int delay)
{
	if (cts_dev->rtdata.program_mode) {
		return cts_dev_writesb(cts_dev, addr, src, len, retry, delay);
	} else {
		return cts_write_sram_normal_mode(cts_dev, addr, src, len,
						  retry, delay);
	}
}

static int cts_calc_sram_crc(const struct cts_device *cts_dev,
			     u32 sram_addr, size_t size, u32 *crc)
{
	TS_LOG_INFO("Calc crc from sram 0x%06x size %zu", sram_addr, size);

	return cts_dev->hwdata->efctrl->ops->calc_sram_crc(cts_dev,
							   sram_addr, size,
							   crc);
}

int cts_sram_writesb_check_crc_retry(const struct cts_device *cts_dev,
				     u32 addr, const void *src, size_t len,
				     u32 crc, int retry)
{
	int ret, retries;

	retries = 0;
	do {
		u32 crc_sram;

		retries++;
		ret = cts_sram_writesb(cts_dev, 0, src, len);
		if (ret) {
			TS_LOG_ERR("SRAM writesb failed %d", ret);
			continue;
		}
		ret = cts_calc_sram_crc(cts_dev, 0, len, &crc_sram);
		if (ret) {
			TS_LOG_ERR
			    ("Get CRC for sram writesb failed %d retries %d",
			     ret, retries);
			continue;
		}

		if (crc == crc_sram)
			return 0;

		TS_LOG_ERR
		    ("Check CRC for sram writesb mismatch %x != %x retries %d",
		     crc, crc_sram, retries);
		ret = -EFAULT;
	} while (retries < retry);

	return ret;
}

static int cts_read_sram_normal_mode(const struct cts_device *cts_dev,
				     u32 addr, void *dst, size_t len, int retry,
				     int delay)
{
	int i, ret;

	for (i = 0; i < len; i++) {
		ret = cts_dev_writel(cts_dev,
				     CTS_DEVICE_FW_REG_DEBUG_INTF, addr, retry,
				     delay);
		if (ret) {
			TS_LOG_ERR("Write addr to rDEBUG_INTF failed %d", ret);
			return ret;
		}

		ret = cts_dev_readb(cts_dev,
				    CTS_DEVICE_FW_REG_DEBUG_INTF + 4,
				    (u8 *)dst, retry, delay);
		if (ret) {
			TS_LOG_ERR("Read value from rDEBUG_INTF + 4 failed %d",
				   ret);
			return ret;
		}

		addr++;
		dst++;
	}

	return 0;
}

int cts_sram_readb_retry(const struct cts_device *cts_dev,
			 u32 addr, u8 *b, int retry, int delay)
{
	if (cts_dev->rtdata.program_mode) {
		return cts_dev_readb(cts_dev, addr, b, retry, delay);
	} else {
		return cts_read_sram_normal_mode(cts_dev, addr, b, 1, retry,
						 delay);
	}
}

int cts_sram_readw_retry(const struct cts_device *cts_dev,
			 u32 addr, u16 *w, int retry, int delay)
{
	int ret;
	u8 buff[2];

	if (cts_dev->rtdata.program_mode) {
		return cts_dev_readw(cts_dev, addr, w, retry, delay);
	} else {
		ret =
		    cts_read_sram_normal_mode(cts_dev, addr, buff, 2, retry,
					      delay);
		if (ret) {
			TS_LOG_ERR("SRAM readw in normal mode failed %d", ret);
			return ret;
		}

		*w = get_unaligned_le16(buff);

		return 0;
	}
}

int cts_sram_readl_retry(const struct cts_device *cts_dev,
			 u32 addr, u32 *l, int retry, int delay)
{
	int ret;
	u8 buff[4];

	if (cts_dev->rtdata.program_mode) {
		return cts_dev_readl(cts_dev, addr, l, retry, delay);
	} else {
		ret = cts_read_sram_normal_mode(cts_dev, addr, buff, 4, retry,
			delay);
		if (ret) {
			TS_LOG_ERR("SRAM readl in normal mode failed %d", ret);
			return ret;
		}

		*l = get_unaligned_le32(buff);
		return 0;
	}
}

int cts_sram_readsb_retry(const struct cts_device *cts_dev,
			  u32 addr, void *dst, size_t len, int retry, int delay)
{
	if (cts_dev->rtdata.program_mode) {
		return cts_dev_readsb(cts_dev, addr, dst, len, retry, delay);
	} else {
		return cts_read_sram_normal_mode(cts_dev, addr, dst, len, retry,
						 delay);
	}
}

int cts_fw_reg_writeb_retry(const struct cts_device *cts_dev,
			    u32 reg_addr, u8 b, int retry, int delay)
{
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Writeb to fw reg 0x%04x under program mode",
			   reg_addr);
		return -ENODEV;
	}

	return cts_dev_writeb(cts_dev, reg_addr, b, retry, delay);
}

int cts_fw_reg_writew_retry(const struct cts_device *cts_dev,
			    u32 reg_addr, u16 w, int retry, int delay)
{
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Writew to fw reg 0x%04x under program mode",
			   reg_addr);
		return -ENODEV;
	}

	return cts_dev_writew(cts_dev, reg_addr, w, retry, delay);
}

int cts_fw_reg_writel_retry(const struct cts_device *cts_dev,
			    u32 reg_addr, u32 l, int retry, int delay)
{
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Writel to fw reg 0x%04x under program mode",
			   reg_addr);
		return -ENODEV;
	}

	return cts_dev_writel(cts_dev, reg_addr, l, retry, delay);
}

int cts_fw_reg_writesb_retry(const struct cts_device *cts_dev,
			     u32 reg_addr, const void *src, size_t len,
			     int retry, int delay)
{
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Writesb to fw reg 0x%04x under program mode",
			   reg_addr);
		return -ENODEV;
	}

	return cts_dev_writesb(cts_dev, reg_addr, src, len, retry, delay);
}

int cts_fw_reg_readb_retry(const struct cts_device *cts_dev,
			   u32 reg_addr, u8 *b, int retry, int delay)
{
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Readb from fw reg under program mode");
		return -ENODEV;
	}

	return cts_dev_readb(cts_dev, reg_addr, b, retry, delay);
}

int cts_fw_reg_readw_retry(const struct cts_device *cts_dev,
			   u32 reg_addr, u16 *w, int retry, int delay)
{
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Readw from fw reg under program mode");
		return -ENODEV;
	}

	return cts_dev_readw(cts_dev, reg_addr, w, retry, delay);
}

int cts_fw_reg_readl_retry(const struct cts_device *cts_dev,
			   u32 reg_addr, u32 *l, int retry, int delay)
{
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Readl from fw reg under program mode");
		return -ENODEV;
	}

	return cts_dev_readl(cts_dev, reg_addr, l, retry, delay);
}

int cts_fw_reg_readsb_retry(const struct cts_device *cts_dev,
			    u32 reg_addr, void *dst, size_t len, int retry,
			    int delay)
{
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Readsb from fw reg under program mode");
		return -ENODEV;
	}

	return cts_dev_readsb(cts_dev, reg_addr, dst, len, retry, delay);
}

int cts_fw_reg_readsb_retry_delay_idle(const struct cts_device *cts_dev,
				       u32 reg_addr, void *dst, size_t len,
				       int retry, int delay, int idle)
{
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Readsb from fw reg under program mode");
		return -ENODEV;
	}

	return cts_dev_readsb_delay_idle(cts_dev, reg_addr, dst, len, retry,
					 delay, idle);
}

int cts_hw_reg_writeb_retry(const struct cts_device *cts_dev,
			    u32 reg_addr, u8 b, int retry, int delay)
{
	return cts_sram_writeb_retry(cts_dev, reg_addr, b, retry, delay);
}

int cts_hw_reg_writew_retry(const struct cts_device *cts_dev,
			    u32 reg_addr, u16 w, int retry, int delay)
{
	return cts_sram_writew_retry(cts_dev, reg_addr, w, retry, delay);
}

int cts_hw_reg_writel_retry(const struct cts_device *cts_dev,
			    u32 reg_addr, u32 l, int retry, int delay)
{
	return cts_sram_writel_retry(cts_dev, reg_addr, l, retry, delay);
}

int cts_hw_reg_writesb_retry(const struct cts_device *cts_dev,
			     u32 reg_addr, const void *src, size_t len,
			     int retry, int delay)
{
	return cts_sram_writesb_retry(cts_dev, reg_addr, src, len, retry,
				      delay);
}

int cts_hw_reg_readb_retry(const struct cts_device *cts_dev,
			   u32 reg_addr, u8 *b, int retry, int delay)
{
	return cts_sram_readb_retry(cts_dev, reg_addr, b, retry, delay);
}

int cts_hw_reg_readw_retry(const struct cts_device *cts_dev,
			   u32 reg_addr, u16 *w, int retry, int delay)
{
	return cts_sram_readw_retry(cts_dev, reg_addr, w, retry, delay);
}

int cts_hw_reg_readl_retry(const struct cts_device *cts_dev,
			   u32 reg_addr, u32 *l, int retry, int delay)
{
	return cts_sram_readl_retry(cts_dev, reg_addr, l, retry, delay);
}

int cts_hw_reg_readsb_retry(const struct cts_device *cts_dev,
			    u32 reg_addr, void *dst, size_t len, int retry,
			    int delay)
{
	return cts_sram_readsb_retry(cts_dev, reg_addr, dst, len, retry, delay);
}

static int cts_init_device_hwdata(struct cts_device *cts_dev,
				  u16 hwid, u16 fwid)
{
	int i;
	int ret = NO_ERR;

	TS_LOG_DEBUG("Init hardware data hwid: %04x fwid: %04x", hwid, fwid);
	if (cts_dev == NULL) {
		TS_LOG_ERR("%s:cts_dev is null\n", __func__);
		return -EINVAL;
	}

	for (i = 0; i < ARRAY_SIZE(cts_device_hwdatas); i++) {
		if (hwid == cts_device_hwdatas[i].hwid ||
		    fwid == cts_device_hwdatas[i].fwid) {
			cts_dev->hwdata = &cts_device_hwdatas[i];
			break;
		}
	}
	TS_LOG_INFO("Init hardware data name: %s hwid: %04x fwid: %04x ",
		    cts_dev->hwdata->name, cts_dev->hwdata->hwid,
		    cts_dev->hwdata->fwid);
	return ret;
}

void cts_lock_device(struct cts_device *cts_dev)
{
	TS_LOG_DEBUG("*** Lock ***");

	rt_mutex_lock(&cts_dev->dev_lock);
}

void cts_unlock_device(struct cts_device *cts_dev)
{
	TS_LOG_DEBUG("### Un-Lock ###");

	rt_mutex_unlock(&cts_dev->dev_lock);
}

int cts_set_work_mode(const struct cts_device *cts_dev, u8 mode)
{
	TS_LOG_INFO("Set work mode to %u", mode);

	return cts_fw_reg_writeb(cts_dev, CTS_DEVICE_FW_REG_WORK_MODE, mode);
}

int cts_get_work_mode(const struct cts_device *cts_dev, u8 *mode)
{
	return cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_WORK_MODE, mode);
}

int cts_get_firmware_version(const struct cts_device *cts_dev, u16 *version)
{
	int ret =
	    cts_fw_reg_readw(cts_dev, CTS_DEVICE_FW_REG_FW_VERSION, version);

	if (ret)
		*version = 0;
	else
		*version = be16_to_cpup(version);

	return ret;
}

int cts_get_short_test_status(const struct cts_device *cts_dev, u8 *status)
{
	return cts_fw_reg_readb(cts_dev,
				CTS_DEVICE_FW_REG_GET_SHORT_TEST_STATUS,
				status);
}

int cts_get_data_ready_flag(const struct cts_device *cts_dev, u8 *flag)
{
	return cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_DATA_READY, flag);
}

int cts_clr_data_ready_flag(const struct cts_device *cts_dev)
{
	return cts_fw_reg_writeb(cts_dev, CTS_DEVICE_FW_REG_DATA_READY, 0);
}

int cts_send_command(const struct cts_device *cts_dev, u8 cmd)
{
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_INFO("Send command 0x%x while chip in program mode",
			    cmd);
		return -ENODEV;
	}

	return cts_fw_reg_writeb_retry(cts_dev, CTS_DEVICE_FW_REG_CMD, cmd, 3,
				       0);
}

int cts_get_touchinfo(const struct cts_device *cts_dev,
		      struct cts_device_touch_info *touch_info)
{
	TS_LOG_DEBUG("Get touch info");

	if (cts_dev->rtdata.program_mode) {
		TS_LOG_INFO("Get touch info in program mode");
		return -ENODEV;
	}

	if (cts_dev->rtdata.suspended)
		TS_LOG_INFO("Get touch info while is suspended");

	return cts_fw_reg_readsb(cts_dev, CTS_DEVICE_FW_REG_TOUCH_INFO,
				 touch_info, sizeof(*touch_info));
}

int cts_get_pannel_id(const struct cts_device *cts_dev, u16 *pannelid)
{
	return cts_fw_reg_readw(cts_dev, CTS_DEVICE_FW_REG_GET_PANNEL_ID,
				pannelid);
}

int cts_get_panel_param(const struct cts_device *cts_dev,
			void *param, size_t size)
{
	TS_LOG_INFO("Get panel parameter");

	if (cts_dev->rtdata.program_mode) {
		TS_LOG_INFO("Get panel parameter in program mode");
		return -ENODEV;
	}

	return cts_fw_reg_readsb(cts_dev,
				 CTS_DEVICE_FW_REG_PANEL_PARAM, param, size);
}

int cts_set_panel_param(const struct cts_device *cts_dev,
			const void *param, size_t size)
{
	TS_LOG_INFO("Set panel parameter");

	if (cts_dev->rtdata.program_mode) {
		TS_LOG_INFO("Set panel parameter in program mode");
		return -ENODEV;
	}
	return cts_fw_reg_writesb(cts_dev,
				  CTS_DEVICE_FW_REG_PANEL_PARAM, param, size);
}

int cts_get_x_resolution(const struct cts_device *cts_dev, u16 *resolution)
{
	return cts_fw_reg_readw(cts_dev, CTS_DEVICE_FW_REG_X_RESOLUTION,
				resolution);
}

int cts_get_y_resolution(const struct cts_device *cts_dev, u16 *resolution)
{
	return cts_fw_reg_readw(cts_dev, CTS_DEVICE_FW_REG_Y_RESOLUTION,
				resolution);
}

int cts_get_num_rows(const struct cts_device *cts_dev, u8 *num_rows)
{
	return cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_NUM_TX, num_rows);
}

int cts_get_num_cols(const struct cts_device *cts_dev, u8 *num_cols)
{
	return cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_NUM_RX, num_cols);
}

int cts_enable_get_rawdata(const struct cts_device *cts_dev)
{
	int i, ret;

	TS_LOG_INFO("Enable get rawdata");

	ret = cts_send_command(cts_dev, CTS_CMD_ENABLE_READ_RAWDATA);
	if (ret) {
		TS_LOG_ERR("Send CMD_ENABLE_READ_RAWDATA failed %d(%s)",
			   ret, cts_strerror(ret));
		return ret;
	}

	for (i = 0; i < CFG_CTS_GET_FLAG_RETRY; i++) {
		u8 enabled = 0;

		ret =
		    cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_GET_RAW_CFG,
				     &enabled);
		if (ret) {
			TS_LOG_ERR
			    ("Get RawDataAndCfgInNormal flag failed %d(%s)",
			     ret, cts_strerror(ret));
			goto delay_and_retry;
		}

		if (enabled) {
			TS_LOG_DEBUG("get RawDataAndCfgInNormal flag has set");
			return 0;
		}

 delay_and_retry:
		mdelay(CFG_CTS_GET_FLAG_DELAY);
	}

	return -EIO;
}

int cts_disable_get_rawdata(const struct cts_device *cts_dev)
{
	int i, ret;

	TS_LOG_INFO("Disable get touch data");

	ret = cts_send_command(cts_dev, CTS_CMD_DISABLE_READ_RAWDATA);
	if (ret) {
		TS_LOG_ERR("Send CMD_DISABLE_READ_RAWDATA failed %d(%s)",
			   ret, cts_strerror(ret));
		return ret;
	}

	if (cts_clr_data_ready_flag(cts_dev)) {
		TS_LOG_ERR("Clear data ready flag failed");
		ret = -ENODEV;
	}

	for (i = 0; i < CFG_CTS_GET_FLAG_RETRY; i++) {
		u8 enabled = 0;

		ret =
		    cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_DATA_READY,
				     &enabled);
		if (ret) {
			TS_LOG_ERR("Enable get touch data flag failed %d(%s)",
				   ret, cts_strerror(ret));
			goto delay_and_retry;
		}

		if (enabled) {
			TS_LOG_DEBUG
			    ("Enable get touch data flag STILL set, try again");
		} else {
			return 0;
		}

 delay_and_retry:
		mdelay(CFG_CTS_GET_FLAG_DELAY);
	}

	return ret;
}

int cts_start_fw_short_opentest(const struct cts_device *cts_dev)
{
	int i, ret;

	TS_LOG_INFO("start fw short and open test");

	ret = cts_send_command(cts_dev, CTS_CMD_SHORT_OPEN_TEST);
	if (ret) {
		TS_LOG_ERR("Send CTS_CMD_SHORT_OPEN_TEST failed %d(%s)",
			   ret, cts_strerror(ret));
		return ret;
	}

	for (i = 0; i < CFG_CTS_GET_FLAG_RETRY; i++) {
		u8 enabled = 0;

		ret =
		    cts_fw_reg_readb(cts_dev,
				     CTS_DEVICE_FW_REG_GET_SHORT_OPEN_TEST_START_FLAG,
				     &enabled);
		if (ret) {
			TS_LOG_ERR
			    ("Get short and open test flag failed %d(%s)",
			     ret, cts_strerror(ret));
			goto delay_and_retry;
		}

		if (enabled) {
			TS_LOG_DEBUG(" short and open test flag  has set");
			return 0;
		}

 delay_and_retry:
		mdelay(CFG_CTS_GET_FLAG_DELAY);
	}

	return -EIO;
}

int cts_get_shortdata(const struct cts_device *cts_dev, void *buf)
{
	int i, ret;
	u8 status;
	u8 retries = 5;

	TS_LOG_INFO("Get shortdata");
    /** - Wait data ready flag set */
	for (i = 0; i < 2000; i++) {
		mdelay(1);
		ret = cts_get_short_test_status(cts_dev, &status);
		if (ret) {
			TS_LOG_ERR("Get data ready flag failed %d", ret);
			continue;
		}
		if (status == 0x03) {
			TS_LOG_INFO("Get short data status:%d", status);
			break;
		}
	}
	if (i == 2000) {
		ret = -ENODEV;
		return ret;
	}

	do {
		ret =
		    cts_fw_reg_readsb_delay_idle(cts_dev,
						 CTS_DEVICE_FW_REG_SHORT_DATA,
						 buf,
						 (cts_dev->fwdata.rows +
						  cts_dev->fwdata.cols) * 4,
						 500);
		if (ret)
			TS_LOG_ERR("Read short failed %d", ret);
	} while (--retries > 0 && ret != 0);

	return ret;
}

int cts_get_opendata(const struct cts_device *cts_dev, void *buf)
{
	int i, ret;
	u8 status;

	TS_LOG_INFO("Get shortdata");
    /** - Wait data ready flag set */
	for (i = 0; i < 200; i++) {
		mdelay(10);
		ret = cts_get_short_test_status(cts_dev, &status);
		if (ret) {
			TS_LOG_ERR("Get data ready flag failed %d", ret);
			continue;
		}
		if (status == 0x03) {
			TS_LOG_INFO("Get open data status:%d", status);
			break;
		}
	}
	if (i == 200) {
		ret = -ENODEV;
		return ret;
	}

	ret =
	    cts_fw_reg_readsb(cts_dev,
			      CTS_DEVICE_FW_REG_OPEN_DATA,
			      buf,
			      (cts_dev->fwdata.rows *
			       cts_dev->fwdata.cols) * 2);
	if (ret)
		TS_LOG_ERR("Read open data failed %d", ret);

	return ret;
}

int cts_get_rawdata(const struct cts_device *cts_dev, void *buf)
{
	int i, ret;
	u8 ready;

	TS_LOG_INFO("Get rawdata");
    /** - Wait data ready flag set */
	for (i = 0; i < 1000; i++) {
		mdelay(1);
		ret = cts_get_data_ready_flag(cts_dev, &ready);
		if (ret) {
			TS_LOG_ERR("Get data ready flag failed %d", ret);
			continue;
		}
		if (ready) {
			TS_LOG_INFO("Get data ready flag,ready = %d", ready);
			break;
		}
	}
	if (i == 1000) {
		ret = -ENODEV;
		TS_LOG_ERR("Get data ready flag timeout, ret= %d", ret);
		goto get_raw_exit;
	}

	ret = cts_fw_reg_readsb(cts_dev, CTS_DEVICE_FW_REG_RAW_DATA,
				buf,
				cts_dev->fwdata.rows * cts_dev->fwdata.cols *
				2);
	if (ret)
		TS_LOG_ERR("Read mutual rawdata failed %d", ret);

	ret = cts_fw_reg_readsb(cts_dev,
				CTS_DEVICE_FW_REG_RAW_DATA_SELF_CAP,
				buf +
				cts_dev->fwdata.rows * cts_dev->fwdata.cols * 2,
				(cts_dev->fwdata.rows +
				 cts_dev->fwdata.cols) * 2);
	if (ret)
		TS_LOG_ERR("Read self rawdata failed %d", ret);

	if (cts_clr_data_ready_flag(cts_dev)) {
		TS_LOG_ERR("Clear data ready flag failed");
		ret = -ENODEV;
	}
 get_raw_exit:
	return ret;
}

int cts_get_diffdata(const struct cts_device *cts_dev, void *buf)
{
	int i, j, ret;
	u8 ready;
	u8 retries = 5;
	u8 *cache_buf;

	TS_LOG_INFO("Get diffdata");
	cache_buf = kzalloc((cts_dev->fwdata.rows + 2) * (cts_dev->fwdata.cols +
							  2) * 2, GFP_KERNEL);
	if (cache_buf == NULL) {
		TS_LOG_ERR("Get diffdata: malloc error");
		ret = -ENOMEM;
		goto get_diff_exit;
	}
    /** - Wait data ready flag set */
	for (i = 0; i < 1000; i++) {
		mdelay(1);
		ret = cts_get_data_ready_flag(cts_dev, &ready);
		if (ret) {
			TS_LOG_ERR("Get data ready flag failed %d", ret);
			goto get_diff_free_buf;
		}
		if (ready)
			break;
	}
	if (i == 1000) {
		ret = -ENODEV;
		goto get_diff_free_buf;
	}
	do {
		ret =
		    cts_fw_reg_readsb_delay_idle(cts_dev,
						 CTS_DEVICE_FW_REG_DIFF_DATA,
						 cache_buf,
						 (cts_dev->fwdata.rows +
						  2) * (cts_dev->fwdata.cols +
							2) * 2, 500);
		if (ret)
			TS_LOG_ERR("Read diffdata failed %d", ret);
	} while (--retries > 0 && ret != 0);

	for (i = 0; i < cts_dev->fwdata.rows; i++) {
		for (j = 0; j < cts_dev->fwdata.cols; j++) {
			((u8 *)buf)[2 * (i * cts_dev->fwdata.cols + j)] =
			    cache_buf[2 *
				      ((i + 1) * (cts_dev->fwdata.cols + 2) +
				       j + 1)];
			((u8 *)buf)[2 * (i * cts_dev->fwdata.cols + j) + 1] =
			    cache_buf[2 *
				      ((i + 1) * (cts_dev->fwdata.cols + 2) +
				       j + 1) + 1];
		}
	}

	if (cts_clr_data_ready_flag(cts_dev)) {
		TS_LOG_ERR("Clear data ready flag failed");
		ret = -ENODEV;
	}
 get_diff_free_buf:
	kfree(cache_buf);
 get_diff_exit:
	return ret;
}

int cts_init_fwdata(struct cts_device *cts_dev)
{
	struct cts_device_fwdata *fwdata = &cts_dev->fwdata;
	int ret;

	TS_LOG_INFO("Init firmware data");

	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Init firmware data while in program mode");
		return -EINVAL;
	}

	ret = cts_get_firmware_version(cts_dev, &fwdata->version);
	if (ret) {
		TS_LOG_ERR("Read firmware version failed %d", ret);
		return ret;
	}
	TS_LOG_INFO("  %-24s: %04x", "Firmware version", fwdata->version);

	ret = cts_get_pannel_id(cts_dev, &fwdata->pannel_id);
	if (ret) {
		TS_LOG_ERR("Read firmware pannel_id failed %d", ret);
		return ret;
	}
	TS_LOG_INFO("  %-24s: %x", "pannel_id", fwdata->pannel_id);

	ret = cts_get_x_resolution(cts_dev, &fwdata->res_x);
	if (ret) {
		TS_LOG_ERR("Read firmware X resoltion failed %d", ret);
		return ret;
	}
	TS_LOG_INFO("  %-24s: %u", "X resolution", fwdata->res_x);

	ret = cts_get_y_resolution(cts_dev, &fwdata->res_y);
	if (ret) {
		TS_LOG_ERR("Read firmware Y resolution failed %d", ret);
		return ret;
	}
	TS_LOG_INFO("  %-24s: %u", "Y resolution", fwdata->res_y);

	ret = cts_get_num_rows(cts_dev, &fwdata->rows);
	if (ret) {
		TS_LOG_ERR("Read firmware num TX failed %d", ret);
	} else {
		if (fwdata->rows > cts_dev->hwdata->num_row) {
			TS_LOG_ERR
			    ("Read firmware num TX %u > device supported %u",
			     fwdata->rows, cts_dev->hwdata->num_row);
			return -EINVAL;
		}
	}
	TS_LOG_INFO("  %-24s: %u", "Num rows", fwdata->rows);

	ret = cts_get_num_cols(cts_dev, &fwdata->cols);
	if (ret) {
		TS_LOG_ERR("Read firmware num RX failed %d", ret);
	} else {
		if (fwdata->cols > cts_dev->hwdata->num_col) {
			TS_LOG_ERR
			    ("Read firmware num RX %u > device supported %u",
			     fwdata->cols, cts_dev->hwdata->num_col);
			return -EINVAL;
		}
	}
	TS_LOG_INFO("  %-24s: %u", "Num cols", fwdata->cols);

	ret =
	    cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_X_Y_SWAP,
			     &fwdata->swap_axes);
	if (ret) {
		TS_LOG_ERR("Read FW_REG_SWAP_AXES failed %d", ret);
		return ret;
	}

	TS_LOG_INFO("  %-24s: %u", "Swap axes", fwdata->swap_axes);

	ret = cts_fw_reg_readb(cts_dev,
			       CTS_DEVICE_FW_REG_INT_MODE, &fwdata->int_mode);
	if (ret) {
		TS_LOG_ERR("Read firmware Int mode failed %d", ret);
		return ret;
	}
	TS_LOG_INFO("  %-24s: %s", "Int polarity",
		    (fwdata->int_mode == 0) ? "LOW" : "HIGH");

	ret = cts_fw_reg_readw(cts_dev,
			       CTS_DEVICE_FW_REG_INT_KEEP_TIME,
			       &fwdata->int_keep_time);
	if (ret) {
		TS_LOG_ERR("Read firmware Int keep time failed %d", ret);
		return ret;
	}
	TS_LOG_INFO("  %-24s: %d", "Int keep time", fwdata->int_keep_time);

	ret = cts_normal_get_project_id(cts_dev);
	if (ret) {
		TS_LOG_ERR("Read firmware project id failed %d\n", ret);
		return ret;
	}

	return 0;
}

bool cts_is_device_suspended(const struct cts_device *cts_dev)
{
	return cts_dev->rtdata.suspended;
}

int cts_suspend_device(struct cts_device *cts_dev)
{
	int ret;

	TS_LOG_INFO("Suspend device");

	if (cts_dev->rtdata.suspended) {
		TS_LOG_INFO("Suspend device while already suspended");
		return 0;
	}
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_INFO("Quit programming mode before suspend");
		ret = cts_enter_normal_mode(cts_dev);
		if (ret) {
			TS_LOG_ERR
			    ("Failed to exit program mode before suspend:%d",
			     ret);
			return ret;
		}
	}
	ret = cts_send_command(cts_dev,
			       cts_dev->rtdata.gesture_wakeup_enabled ?
			       CTS_CMD_SUSPEND_WITH_GESTURE : CTS_CMD_SUSPEND);

	if (ret) {
		TS_LOG_ERR("Suspend device failed %d", ret);

		return ret;
	}
	if (cts_data->tskit_data->easy_wakeup_info.sleep_mode) {
		ret = cts_send_command(cts_dev, CTS_DOUBLE_CLICK_CMD);
		TS_LOG_INFO("sleep mode");
	}
	if (cts_data->tskit_data->easy_wakeup_info.aod_mode) {
		ret = cts_send_command(cts_dev, CTS_SINGLE_CLICK_CMD);
		TS_LOG_INFO("aod mode");
	}
	if (ret) {
		TS_LOG_ERR("send gesture cmd failed %d", ret);
		return ret;
	}

	TS_LOG_INFO("Device suspended ...");
	cts_dev->rtdata.suspended = true;

	return 0;
}

int cts_resume_device(struct cts_device *cts_dev)
{
	struct chipone_ts_data *cts_data;
	int ret = 0;
	int retries = 1;

	TS_LOG_INFO("Resume device");

	cts_data = container_of(cts_dev, struct chipone_ts_data, cts_dev);

	/* Check whether device is in normal mode */
	while (--retries >= 0) {
		cts_plat_reset_device(cts_dev->pdata);

		cts_set_normal_addr(cts_dev);
		if (cts_plat_is_i2c_online
		    (cts_dev->pdata, CTS_DEV_NORMAL_MODE_I2CADDR)) {
			goto config_firmware;
		}
	}

	TS_LOG_INFO("Need update firmware when resume");

	if (cts_data->cached_firmware == NULL) {
		TS_LOG_INFO("Firmware is NULL, re-request from system");
		if (cts_data->boot_firmware_filename[0] == '\0') {
			TS_LOG_ERR
			    ("Firmware filename is nul, nothing can be done");
			goto err_set_program_mode;
		}
		cts_data->cached_firmware =
			cts_request_firmware_from_fs(&cts_data->cts_dev,
				cts_data->boot_firmware_filename);
		if (cts_data->cached_firmware == NULL) {
			TS_LOG_ERR("Request firmware from file '%s' failed",
				   cts_data->boot_firmware_filename);
			ret = -EIO;
			goto err_set_program_mode;
		}
	}

 config_firmware:
	cts_dev->rtdata.suspended = false;
	return 0;

 err_set_program_mode:
	cts_dev->rtdata.program_mode = true;
	cts_dev->rtdata.slave_addr = CTS_DEV_PROGRAM_MODE_I2CADDR;
	cts_dev->rtdata.addr_width = CTS_DEV_PROGRAM_MODE_ADDR_WIDTH;

	return ret;
}

bool cts_is_device_program_mode(const struct cts_device *cts_dev)
{
	return cts_dev->rtdata.program_mode;
}

static inline void cts_init_rtdata_with_normal_mode(struct cts_device *cts_dev)
{
	memset(&cts_dev->rtdata, 0, sizeof(cts_dev->rtdata));

	cts_set_normal_addr(cts_dev);
	cts_dev->rtdata.suspended = false;
	cts_dev->rtdata.updating = false;
	cts_dev->rtdata.testing = false;
	cts_dev->rtdata.fw_log_redirect_enabled = false;
	cts_dev->rtdata.glove_mode_enabled = false;
}

void cts_get_boot_status(struct cts_device *cts_dev, u8 *status)
{
	u8 ret;
	u8 regread[3];

	put_unaligned_be24(CTS_DEV_HW_REG_BOOT_STATUS, regread);
	ret = cts_plat_i2c_read(cts_dev->pdata,
				CTS_DEV_PROGRAM_MODE_I2CADDR, regread, 3,
				status, 1, 5, 10);
	if (ret) {
		TS_LOG_ERR("get boot status : 0x%02x failed %d",
			   CTS_DEV_HW_REG_BOOT_STATUS, ret);
	}
}

int cts_disable_lowpower(struct cts_device *cts_dev)
{
	u8 ret = NO_ERR;
	u8 i;
	u8 retries = 0;

	TS_LOG_INFO("cts_disable_lowpower");
	do {
		ret = cts_send_command(cts_dev, CTS_CMD_LOW_POWER_OFF);
		if (ret) {
			TS_LOG_ERR
			    ("Send command CTS_CMD_LOW_POWER_OFF failed: ret = %d",
			     ret);
		}

		for (i = 0; i < 10; i++) {
			u8 enabled = 0xff;

			ret =
			    cts_fw_reg_readb(cts_dev,
					     CTS_DEVICE_FW_REG_LOW_POWER_EN,
					     &enabled);
			if (ret) {
				TS_LOG_ERR
				    ("Get disable low_power flag failed %d",
				     ret);
				goto delay_and_retry;
			}

			if (!enabled) {
				TS_LOG_DEBUG
				    ("get disable low_power flag  flag has set");
				return 0;
			}
 delay_and_retry:
			mdelay(5);
		}
	} while (++retries < 3);

	if (retries >= 3) {
		TS_LOG_ERR("set disable low_power flag error");
		return -EINVAL;
	}
	return ret;
}

int cts_enter_program_mode(struct cts_device *cts_dev)
{
	const static u8 magic_num[] = { 0xCC, 0x33, 0x55, 0x5A };
	u8 ret;
	u8 bootstatus;
	u8 retries = 0;
	struct chipone_ts_data *cts_data;

	cts_data = container_of(cts_dev, struct chipone_ts_data, cts_dev);

	TS_LOG_INFO("Enter program mode");

	if (cts_dev->rtdata.program_mode) {
		TS_LOG_INFO("Enter program mode while alredy in");
		return 0;
	}
	do {
		ret = cts_plat_reset_device(cts_dev->pdata);
		if (ret < 0)
			TS_LOG_ERR("Reset device failed %d", ret);

		ret = cts_send_command(cts_dev, CTS_CMD_LOW_POWER_OFF);
		if (ret) {
			TS_LOG_ERR
			    ("Send command CTS_CMD_LOW_POWER_OFF failed: ret = %d",
			     ret);
		}

		ret = cts_send_command(cts_dev, CTS_CMD_MONITOR_OFF);
		if (ret) {
			TS_LOG_ERR
			    ("Send command CTS_CMD_LOW_POWER_OFF failed: ret = %d",
			     ret);
		}

		mdelay(60);
		ret = cts_plat_i2c_write(cts_dev->pdata,
					 CTS_DEV_PROGRAM_MODE_I2CADDR,
					 magic_num, 4, 5, 10);
		if (ret) {
			TS_LOG_ERR
			    ("Write magic number to i2c_dev: 0x%02x failed: ret = %d",
			     CTS_DEV_PROGRAM_MODE_I2CADDR, ret);
			continue;
		}
		mdelay(1);
		cts_get_boot_status(cts_dev, &bootstatus);
		if (bootstatus == 0x02)
			break;

	} while (++retries < 2);

	TS_LOG_INFO("bootmode: 0x%02x", bootstatus);

	if (retries >= 2)
		return -EINVAL;

	cts_set_program_addr(cts_dev);

	return ret;
}

int cts_enter_normal_mode(struct cts_device *cts_dev)
{
	int ret = 0;

	if (!cts_dev->rtdata.program_mode) {
		TS_LOG_INFO("Enter normal mode while already in");
		return 0;
	}

	TS_LOG_INFO("Enter normal mode");

	cts_set_program_addr(cts_dev);

	ret =
	    cts_hw_reg_writeb_retry(cts_dev, CTS_DEV_HW_REG_BOOT_MODE,
				    CTS_DEV_BOOT_MODE_SRAM, 5, 5);

	if (ret) {
		TS_LOG_ERR("Enter normal mode  ICNT8918 failed %d(%s)",
			   ret, cts_strerror(ret));
	}
	TS_LOG_INFO("Enter normal mode  ICNT8918 suc");
	mdelay(30);
	cts_set_normal_addr(cts_dev);
	return ret;
}

bool cts_is_device_enabled(const struct cts_device *cts_dev)
{
	return cts_dev->enabled;
}

int cts_start_device(struct cts_device *cts_dev)
{
	TS_LOG_INFO("Start device...");

	if (cts_is_device_enabled(cts_dev)) {
		TS_LOG_INFO("Start device while already started");
		return 0;
	}

	cts_dev->enabled = true;

	TS_LOG_INFO("Start device successfully");

	return 0;
}

int cts_stop_device(struct cts_device *cts_dev)
{
	int ret;

	TS_LOG_INFO("Stop device...");

	if (!cts_is_device_enabled(cts_dev)) {
		TS_LOG_INFO("Stop device while halted");
		return 0;
	}

	if (cts_is_firmware_updating(cts_dev)) {
		TS_LOG_INFO
		    ("Stop device while firmware updating, please try again");
		return -EAGAIN;
	}

	cts_dev->enabled = false;

	ret = cts_plat_release_all_touch(cts_dev->pdata);
	if (ret) {
		TS_LOG_ERR("Release all touch failed %d", ret);
		return ret;
	}
	return ret;
}

int cts_prog_get_project_id(struct cts_device *cts_dev)
{
	int ret;
	int len;
	char project_id[CTS_INFO_PROJECT_ID_LEN + 4] = { 0 };
	struct chipone_ts_data *cts_data =
	    container_of(cts_dev, struct chipone_ts_data, cts_dev);
	TS_LOG_INFO("prog get project id\n");

	/*  size MUST 4 aligned */
	len = roundup(CTS_INFO_PROJECT_ID_LEN, 4);

	ret = cts_prepare_flash_operation(cts_dev);
	if (ret) {
		TS_LOG_INFO("prepare flash operation failed %d\n", ret);
		ret = -ENODEV;
	}

	ret = cts_read_flash(cts_dev, CTS_INFO_PROJECT_ID_ADDR,
			     project_id, len);
	if (ret) {
		TS_LOG_INFO("Read flash data failed %d\n", ret);
		ret = -ENODEV;
	}

	strncpy(cts_data->project_id, &project_id[2],
		sizeof(cts_data->project_id) - 1);
	TS_LOG_INFO("Device hardware id: %s", project_id);
	return ret;
}

int cts_normal_get_project_id(struct cts_device *cts_dev)
{
	int ret;
	char project_id[CTS_INFO_PROJECT_ID_LEN + 1] = { 0 };
	struct chipone_ts_data *cts_data =
		container_of(cts_dev, struct chipone_ts_data, cts_dev);

	TS_LOG_INFO("normal get project id\n");
	ret = cts_fw_reg_readsb(cts_dev, CTS_DEVICE_FW_REG_PROJECT_ID,
		project_id, CTS_INFO_PROJECT_ID_LEN);
	if (ret) {
		TS_LOG_ERR("Read firmware project id failed %d\n", ret);
		return ret;
	}
	strncpy(cts_data->project_id, project_id,
		sizeof(cts_data->project_id) - 1);
	TS_LOG_INFO("Device firmware project id: %s\n", project_id);
	return ret;
}

bool cts_is_fwid_valid(u16 fwid)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(cts_device_hwdatas); i++) {
		if (cts_device_hwdatas[i].fwid == fwid)
			return true;
	}

	return false;
}

static bool cts_is_hwid_valid(u32 hwid)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(cts_device_hwdatas); i++) {
		if (cts_device_hwdatas[i].hwid == hwid)
			return true;
	}

	return false;
}

int cts_get_fwid(struct cts_device *cts_dev, u16 *fwid)
{
	int ret = NO_ERR;

	TS_LOG_INFO("Get device firmware id");

	if (cts_dev->hwdata) {
		*fwid = cts_dev->hwdata->fwid;
		return ret;
	}

	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("Get device firmware id while in program mode");
		ret = -ENODEV;
		goto err_out;
	}

	ret = cts_fw_reg_readw_retry(cts_dev,
				     CTS_DEVICE_FW_REG_CHIP_TYPE, fwid, 5, 1);
	if (ret)
		goto err_out;

	*fwid = be16_to_cpu(*fwid);

	TS_LOG_INFO("Device firmware id: %04x", *fwid);

	if (!cts_is_fwid_valid(*fwid)) {
		TS_LOG_INFO("Get invalid firmware id %04x", *fwid);
		ret = -EINVAL;
		goto err_out;
	}

	return ret;

 err_out:
	*fwid = CTS_DEV_FWID_INVALID;
	return ret;
}

int cts_get_hwid(struct cts_device *cts_dev, u16 *hwid)
{
	int ret;

	TS_LOG_INFO("Get device hardware id");

	if (cts_dev->hwdata) {
		*hwid = cts_dev->hwdata->hwid;
		return 0;
	}

	TS_LOG_INFO
	    ("Device hardware data not initialized, try to read from register");

	if (!cts_dev->rtdata.program_mode) {
		ret = cts_enter_program_mode(cts_dev);
		if (ret) {
			TS_LOG_ERR("Enter program mode failed %d", ret);
			goto err_out;
		}
	}

	ret =
	    cts_hw_reg_readw_retry(cts_dev, CTS_DEV_HW_REG_HARDWARE_ID, hwid, 5,
				   10);
	if (ret)
		goto err_out;

	*hwid = le16_to_cpu(*hwid);
	TS_LOG_INFO("Device hardware id: %04x", *hwid);

	if (!cts_is_hwid_valid(*hwid)) {
		TS_LOG_INFO("Device hardware id %04x invalid", *hwid);
		ret = -EINVAL;
		goto err_out;
	}

	return 0;

 err_out:
	*hwid = CTS_DEV_HWID_INVALID;
	return ret;
}

int cts_probe_device(struct cts_device *cts_dev)
{
	int ret, retries = 0;
	u16 fwid = CTS_DEV_FWID_INVALID;
	u16 hwid = CTS_DEV_HWID_INVALID;

	TS_LOG_INFO("Probe device");
	if (!cts_dev) {
		TS_LOG_ERR("%s:cts_dev is null\n", __func__);
		return -EINVAL;
	}

	rt_mutex_init(&cts_dev->dev_lock);

	cts_init_rtdata_with_normal_mode(cts_dev);

	do {
		ret = cts_get_fwid(cts_dev, &fwid);
		if (ret || fwid == CTS_DEV_HWID_INVALID)
			TS_LOG_ERR("Get firmware id failed %d,fwid 0x%x retries %d", ret, fwid, retries);
		else {
			TS_LOG_INFO("Get firmware id: %x, retries %d", fwid, retries);
			break;
		}

		 /** - Try to read hardware id, it will enter program mode as normal */
		ret = cts_get_hwid(cts_dev, &hwid);
		if (ret || hwid == CTS_DEV_HWID_INVALID) {
			TS_LOG_ERR("Get hardware id failed %d retries %d", ret, retries);
		} else {
			TS_LOG_INFO("Get hwid: %d, retries %d", hwid, retries);
			break;
		}

		ret = cts_plat_reset_device(cts_dev->pdata);
		if (ret < 0)
			TS_LOG_ERR("Reset device failed %d", ret);
	}  while (++retries < 3);

	if (retries >= 3) {
		TS_LOG_ERR("Get fwid and hwid failed %d retries %d", ret, retries);
		return -ENODEV;
	}

	ret = cts_init_device_hwdata(cts_dev, hwid, fwid);
	if (ret) {
		TS_LOG_ERR("Device hwid: %06x fwid: %04x not found", hwid,
			   fwid);
		return -ENODEV;
	}
	if (hwid == cts_dev->hwdata->hwid) {
		ret = cts_prog_get_project_id(cts_dev);
		if (ret)
			TS_LOG_ERR("Get project id failed");
	}
	if (fwid == cts_dev->hwdata->hwid) {
		ret = cts_get_firmware_version(cts_dev, &(cts_dev->fwdata.version));
		if (ret) {
			TS_LOG_ERR("Read firmware version failed %d", ret);
			return ret;
		}
		TS_LOG_INFO("  %-24s: %04x", "Firmware version", cts_dev->fwdata.version);

		ret = cts_normal_get_project_id(cts_dev);
		if (ret) {
			TS_LOG_ERR("Read firmware project id failed %d\n", ret);
			return ret;
		}

		if (cts_dev->fwdata.version < 0x0a00) {
			ret = cts_prog_get_project_id(cts_dev);
			if (ret)
				TS_LOG_ERR("Get project id failed");
			ret = cts_enter_normal_mode(cts_dev);
			if (ret)
				TS_LOG_ERR("Enter normal mode failed %d", ret);
		}
	}
	return ret;
}

void cts_enable_gesture_wakeup(struct cts_device *cts_dev)
{
	TS_LOG_INFO("Enable gesture wakeup");
	cts_dev->rtdata.gesture_wakeup_enabled = true;
}

void cts_disable_gesture_wakeup(struct cts_device *cts_dev)
{
	TS_LOG_INFO("Disable gesture wakeup");
	cts_dev->rtdata.gesture_wakeup_enabled = false;
}

bool cts_is_gesture_wakeup_enabled(const struct cts_device *cts_dev)
{
	return cts_dev->rtdata.gesture_wakeup_enabled;
}

int cts_get_gesture_info(const struct cts_device *cts_dev,
			 void *gesture_info, bool trace_point)
{
	int ret = 0;

	TS_LOG_INFO("get gesture info");
	return ret;
}

