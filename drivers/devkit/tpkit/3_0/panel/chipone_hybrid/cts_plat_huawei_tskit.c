#define LOG_TAG         "Plat"

#include "cts_config.h"
#include "cts_platform.h"
#include "cts_core.h"
#include "cts_firmware.h"
#include "huawei_ts_kit.h"
#include "huawei_ts_kit_algo.h"

extern struct chipone_ts_data *cts_data;
extern void ts_i2c_error_dmd_report(u8* reg_addr);
/** Need to be replaced **/

size_t cts_plat_get_max_i2c_xfer_size(struct cts_platform_data *pdata)
{
	return CFG_CTS_MAX_I2C_XFER_SIZE;
}

u8 *cts_plat_get_i2c_xfer_buf(struct cts_platform_data *pdata,
			size_t xfer_size)
{
	return cts_data->i2c_fifo_buf;
}

int cts_plat_i2c_write(struct cts_platform_data *pdata, u8 i2c_addr,
		       const void *src, size_t len, int retry, int delay)
{
	int ret = 0, retries = 0;

	struct i2c_msg msg = {
		.flags = 0,
		.addr = i2c_addr,
		.buf = (u8 *)src,
		.len = len
	};

	do {
		ret = i2c_transfer(pdata->i2c_client->adapter, &msg, 1);
		if (ret != 1) {
			if (ret >= 0)
				ret = -EIO;
			if (delay)
				mdelay(delay);
			continue;
		} else {
			return 0;
		}
	} while (++retries < retry);

#if defined (CONFIG_HUAWEI_DSM)
	if (ret == -EIO) {
		ts_i2c_error_dmd_report((u8 *)&i2c_addr);
		TS_LOG_ERR("i2c write error\n");
	}
#endif
	return ret;
}

int cts_plat_i2c_read(struct cts_platform_data *pdata, u8 i2c_addr,
		      const u8 *wbuf, size_t wlen, void *rbuf, size_t rlen,
		      int retry, int delay)
{
	int num_msg = 0;
	int ret = 0;
	int retries = 0;

	struct i2c_msg msgs[2] = {
		{
		 .addr = i2c_addr,
		 .flags = 0,
		 .buf = (u8 *)wbuf,
		 .len = wlen
		},
		{
		 .addr = i2c_addr,
		 .flags = I2C_M_RD,
		 .buf = (u8 *)rbuf,
		 .len = rlen
		}
	};

	if (wbuf == NULL || wlen == 0)
		num_msg = 1;
	else
		num_msg = 2;

	do {
		ret = i2c_transfer(pdata->i2c_client->adapter,
				   &msgs[ARRAY_SIZE(msgs) - num_msg], num_msg);

		if (ret != num_msg) {
			if (ret >= 0)
				ret = -EIO;
			if (delay)
				mdelay(delay);
			continue;
		} else {
			return 0;
		}
	} while (++retries < retry);

#if defined (CONFIG_HUAWEI_DSM)
	if (ret == -EIO) {
		ts_i2c_error_dmd_report((u8 *)&i2c_addr);
		TS_LOG_ERR("i2c read error\n");
	}
#endif
	return ret;
}

int cts_plat_is_i2c_online(struct cts_platform_data *pdata, u8 i2c_addr)
{
	u8 dummy_bytes[2] = { 0x00, 0x00 };
	int ret;

	ret =
	    cts_plat_i2c_write(pdata, i2c_addr, dummy_bytes,
			       sizeof(dummy_bytes), 5, 2);
	if (ret) {
		TS_LOG_INFO("!!! I2C addr 0x%02x is offline !!!", i2c_addr);
		return false;
	} else {
		TS_LOG_INFO("I2C addr 0x%02x is online", i2c_addr);
		return true;
	}
}
