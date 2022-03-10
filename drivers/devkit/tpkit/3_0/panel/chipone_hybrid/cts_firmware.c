#define LOG_TAG         "Firmware"

#include "cts_config.h"
#include "cts_platform.h"
#include "cts_core.h"
#include "cts_efctrl.h"
#include "cts_emb_flash.h"
#include "cts_firmware.h"
#include <linux/path.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/vmalloc.h>

unsigned int cts_fw_crc_calc(unsigned int crc_in, char *buf, int len)
{
	/*This polynomial (0x04c11db7) is used at: AUTODIN II, Ethernet, & FDDI */
	const static unsigned int crc32table[256] = {
		0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc,
		0x17c56b6b, 0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f,
		0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a,
		0x384fbdbd, 0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
		0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75, 0x6a1936c8,
		0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
		0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e,
		0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
		0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84,
		0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d, 0xd4326d90, 0xd0f37027,
		0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022,
		0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
		0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077,
		0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c,
		0x2e003dc5, 0x2ac12072, 0x128e9dcf, 0x164f8078, 0x1b0ca6a1,
		0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
		0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb,
		0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
		0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d,
		0x40d816ba, 0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
		0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692, 0x8aad2b2f,
		0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044,
		0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050, 0xe9362689,
		0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
		0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683,
		0xd1799b34, 0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59,
		0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c,
		0x774bb0eb, 0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
		0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53, 0x251d3b9e,
		0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
		0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48,
		0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
		0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2,
		0xe6ea3d65, 0xeba91bbc, 0xef68060b, 0xd727bbb6, 0xd3e6a601,
		0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604,
		0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
		0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6,
		0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad,
		0x81b02d74, 0x857130c3, 0x5d8a9099, 0x594b8d2e, 0x5408abf7,
		0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
		0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd,
		0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
		0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b,
		0x0fdc1bec, 0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
		0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 0xc5a92679,
		0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12,
		0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676, 0xea23f0af,
		0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
		0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5,
		0x9e7d9662, 0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06,
		0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03,
		0xb1f740b4
	};

	int i;
	unsigned int crc = crc_in;

	for (i = 0; i < len; i++)
		crc = (crc << 8) ^ crc32table[((crc >> 24) ^ *buf++) & 0xFF];

	TS_LOG_INFO("%s : crc = %0x ", __func__, crc);
	return crc;
}

#define FIRMWARE_CRC_SECTION_SIZE               (16)

#define CTS_FIRMWARE_MAX_FILE_SIZE              (0xC000-FIRMWARE_CRC_SECTION_SIZE)
#define CTS_SECTION_ENABLE_FLAG                 (0x0000C35A)

enum cts_firmware_section_offset {
	CTS_FIRMWARE_SECTION_OFFSET = 0x00000000,
	CTS_FIRMWARE_CRC_SECTION_OFFSET = CTS_FIRMWARE_MAX_FILE_SIZE,

};

enum cts_erase_flash_mode {
	CTS_CHIP_ERASE = 0,
	CTS_SECTOR_EARSE = 1,

};

static int calc_crc_in_flash(const struct cts_device *cts_dev,
			     u32 flash_addr, size_t size, u32 *crc)
{
	return cts_dev->hwdata->efctrl->ops->calc_flash_crc(cts_dev,
							    flash_addr, size,
							    crc);
}

static bool is_firmware_size_valid(const struct cts_firmware *firmware)
{
	return ((firmware->size > 0x0) &&
		(firmware->size <= CTS_FIRMWARE_CRC_SECTION_OFFSET) &&
		((firmware->size % 4) == 0));
}

static bool is_firmware_valid(const struct cts_firmware *firmware)
{
	return (firmware && firmware->data && is_firmware_size_valid(firmware));
}

#ifdef CFG_CTS_FIRMWARE_IN_FS
bool is_filesystem_mounted(const char *filepath)
{
	struct path root_path;
	struct path path;
	int ret;

	ret = kern_path("/", LOOKUP_FOLLOW, &root_path);
	if (ret)
		return false;

	ret = kern_path(filepath, LOOKUP_FOLLOW, &path);
	if (ret)
		goto err_put_root_path;

	if (path.mnt->mnt_sb == root_path.mnt->mnt_sb)
		/* not mounted */
		ret = false;
	else
		ret = true;

	path_put(&path);
 err_put_root_path:
	path_put(&root_path);

	return !!ret;
}

static int cts_wrap_kernel_request_firmware(struct cts_firmware *firmware,
					    const char *name,
					    struct device *device,
					    int curr_version)
{
	int ret;

	ret = request_firmware(&firmware->fw, name, device);
	if (ret) {
		TS_LOG_ERR("Could not load firmware from %s: %d", name, ret);
		return ret;
	}

	/* Map firmware structure to cts_firmware */
	firmware->data = (u8 *)firmware->fw->data;
	firmware->size = firmware->fw->size;

	/* Check firmware */
	if (!is_firmware_valid(firmware)) {
		TS_LOG_ERR("Request firmware file INVALID, size %zu",
			   firmware->size);
		ret = -EINVAL;
		goto err_release_firmware;
	}
	TS_LOG_INFO("curr_version = %04x\n", curr_version);
	TS_LOG_INFO("new_version = %04x\n", FIRMWARE_VERSION(firmware));
	/* Check firmware version */
	if (curr_version > 0 && curr_version == FIRMWARE_VERSION(firmware)) {
		TS_LOG_INFO
		    ("Request firmware file with version 0x%04x == 0x%04x",
		     FIRMWARE_VERSION(firmware), curr_version);
		ret = -EINVAL;
		goto err_release_firmware;
	}

	return 0;

 err_release_firmware:
	release_firmware(firmware->fw);

	return ret;
}

const struct cts_firmware *cts_request_newer_firmware_from_fs(const struct
							      cts_device
							      *cts_dev, const char
							      *filepath,
							      u16 curr_version)
{
	struct cts_firmware *firmware;
	int ret;

	TS_LOG_INFO("Request from file '%s' if version != %04x",
		    filepath, curr_version);

	if (filepath == NULL || filepath[0] == '\0') {
		TS_LOG_ERR("Request from file path INVALID %p", filepath);
		return NULL;
	}

	firmware = kzalloc(sizeof(*firmware), GFP_KERNEL);
	if (firmware == NULL) {
		TS_LOG_ERR("Request from file alloc struct firmware failed");
		return NULL;
	}

	TS_LOG_INFO
		("Filepath is only filename, use request_firmware()");
	ret = cts_wrap_kernel_request_firmware(firmware, filepath,
		container_of(cts_dev, struct chipone_ts_data, cts_dev)->device,
		curr_version);

	if (ret) {
		TS_LOG_ERR("Request from file '%s' failed %d", filepath, ret);
		kfree(firmware);
		return NULL;
	} else {
		return firmware;
	}
}

const struct cts_firmware *cts_request_firmware_from_fs(const struct cts_device
							*cts_dev,
							const char *filepath)
{
	TS_LOG_INFO("Request from file '%s'", filepath);

	return cts_request_newer_firmware_from_fs(cts_dev, filepath, 0);
}

int cts_update_firmware_from_file(struct cts_device *cts_dev,
				  const char *filepath, bool to_flash)
{
	const struct cts_firmware *firmware;
	int ret;

	TS_LOG_INFO("Update from file '%s' to %s", filepath,
		    to_flash ? "flash" : "sram");

	firmware = cts_request_firmware_from_fs(cts_dev, filepath);
	if (firmware == NULL) {
		TS_LOG_ERR("Request from file '%s' failed", filepath);
		return -EFAULT;
	}

	ret = cts_update_firmware(cts_dev, firmware, to_flash);
	if (ret) {
		TS_LOG_ERR("Update to %s from file failed %d",
			   to_flash ? "flash" : "sram", ret);
		goto err_release_firmware;
	}

	TS_LOG_INFO("Update from file success");

 err_release_firmware:
	cts_release_firmware(firmware);

	return ret;
}
#endif				/*CFG_CTS_FIRMWARE_IN_FS */

void cts_release_firmware(const struct cts_firmware *firmware)
{
	TS_LOG_INFO("Release firmware %p", firmware);

	if (firmware) {
		if (firmware->fw) {
			/* Use request_firmware() get struct firmware * */
			TS_LOG_INFO("Release firmware from request_firmware");
			release_firmware(firmware->fw);
			kfree(firmware);
		} else if (firmware->name == NULL) {
			/* Direct read from file */
			TS_LOG_INFO("Release firmware from direct-load");
			vfree(firmware->data);
			kfree(firmware);
		} else {
			/*Builtin firmware with non-NULL name, no need to free */
			TS_LOG_INFO("Release firmware from driver built-in");
		}
	}
}

static int cts_crc_write_check(struct cts_device *cts_dev, unsigned int crc_fw,
			       u32 Length)
{
	unsigned char temp[FIRMWARE_CRC_SECTION_SIZE],
	    temp_buf[FIRMWARE_CRC_SECTION_SIZE];
	int j = 0;
	int ret = 0;

	temp[0] = (unsigned char)crc_fw;
	temp[1] = (unsigned char)(crc_fw >> 8);
	temp[2] = (unsigned char)(crc_fw >> 16);
	temp[3] = (unsigned char)(crc_fw >> 24);

	temp[4] = (unsigned char)Length;
	temp[5] = (unsigned char)(Length >> 8);
	temp[6] = (unsigned char)(Length >> 16);
	temp[7] = (unsigned char)(Length >> 24);

	temp[8] = (unsigned char)CTS_SECTION_ENABLE_FLAG;
	temp[9] = (unsigned char)(CTS_SECTION_ENABLE_FLAG >> 8);
	temp[10] = (unsigned char)(CTS_SECTION_ENABLE_FLAG >> 16);
	temp[11] = (unsigned char)(CTS_SECTION_ENABLE_FLAG >> 24);

	temp[12] = (unsigned char)Length;
	temp[13] = (unsigned char)(Length >> 8);
	temp[14] = (unsigned char)(Length >> 16);
	temp[15] = (unsigned char)(Length >> 24);

	ret = cts_sram_writesb(cts_dev, Length, temp,
			       FIRMWARE_CRC_SECTION_SIZE);
	if (ret)
		TS_LOG_ERR("cts write crc to sram failed: %d", ret);

	ret = cts_program_flash_from_sram(cts_dev,
					  CTS_FIRMWARE_CRC_SECTION_OFFSET,
					  Length, FIRMWARE_CRC_SECTION_SIZE);
	if (ret) {
		TS_LOG_ERR("Program firmware crc section from sram failed %d",
			   ret);
		return ret;
	}

	ret =
	    cts_read_flash_to_sram_retry(cts_dev,
					 CTS_FIRMWARE_CRC_SECTION_OFFSET,
					 Length, FIRMWARE_CRC_SECTION_SIZE, 1);
	if (ret) {
		TS_LOG_ERR("read crc section to sram  failed %d", ret);
		return ret;
	}

	ret =
	    cts_sram_readsb(cts_dev, Length, temp_buf,
			    FIRMWARE_CRC_SECTION_SIZE);
	if (ret) {
		TS_LOG_ERR("read crc section from sram  failed %d", ret);
		return ret;
	}
	for (j = 0; j < 16; j++) {
		if (temp_buf[j] != temp[j]) {
			TS_LOG_ERR(" cts firmware check  error");
			return R_VERIFY_ERR;
		}
	}

	TS_LOG_INFO(" firmware  check  ok");
	return 0;
}

static int validate_flash_data(const struct cts_device *cts_dev,
			       u32 flash_addr, size_t size, u32 fw_crc)
{
	int ret;
	u32 crc_flash;

	TS_LOG_INFO("Validate flash data from 0x%06x size %zu by %s",
		flash_addr, size, fw_crc ? "check-crc" : "direct-readback");

	ret = calc_crc_in_flash(cts_dev, flash_addr, size, &crc_flash);
	if (ret) {
		TS_LOG_ERR("Calc data in flash from 0x%06x size %zu crc failed %d",
			flash_addr, size, ret);
	} else {
		if (crc_flash != fw_crc) { /**check crc**/
			TS_LOG_ERR("Crc in flash from 0x%06x size %zu mismatch 0x%08x != 0x%08x",
				flash_addr, size, crc_flash, fw_crc);
			return R_VERIFY_ERR;
		} else {
			TS_LOG_INFO("Flash data crc correct");
			return 0;
		}
	}

	return ret;
}

static int cts_program_firmware_from_sram_to_flash(const struct cts_device
						   *cts_dev, u32 Length)
{
	int ret;

	TS_LOG_INFO("Program firmware from sram to flash, size %zu", Length);

	ret = cts_program_flash_from_sram(cts_dev, 0, 0, Length);
	if (ret) {
		TS_LOG_ERR("Program firmware section from sram failed %d", ret);
		return ret;
	}

	return 0;
}

int cts_update_firmware(struct cts_device *cts_dev,
			const struct cts_firmware *firmware, bool to_flash)
{
	ktime_t start_time, end_time, delta_time;
	int ret, retries;
	unsigned int crc_fw = 0;

	TS_LOG_INFO("Update firmware to %s ver: %04x size: %zu",
		    to_flash ? "flash" : "sram",
		    FIRMWARE_VERSION(firmware), firmware->size);

	start_time = ktime_get();

	ret = cts_enter_program_mode(cts_dev);
	if (ret) {
		TS_LOG_ERR("Device enter program mode failed %d", ret);
		goto out;
	}
	ret = cts_prepare_flash_operation(cts_dev);
	if (ret) {
		TS_LOG_INFO("Prepare flash operation failed %d", ret);
		goto out;
	}
	cts_dev->rtdata.updating = true;
	/*** calc fw crc ***/
	crc_fw = cts_fw_crc_calc(crc_fw, firmware->data, firmware->size);
	TS_LOG_INFO("Update firmware , fw crc: 0x%08x, size:", crc_fw,
		    firmware->size);

	/***download to sram***/
	ret = cts_sram_writesb_check_crc_retry(cts_dev, 0,
					       firmware->data, firmware->size,
					       crc_fw, 3);
	if (ret) {
		TS_LOG_ERR("Write firmware to sram failed %d", ret);
		goto out;
	}

	if (!to_flash)
		goto out;
	/**** program flash***/
	retries = 0;
	do {
		retries++;

		TS_LOG_ERR("Erase firmware in flash");

		ret =
		    cts_erase_flash(cts_dev, CTS_FIRMWARE_SECTION_OFFSET,
				    firmware->size);
		if (ret) {
			TS_LOG_ERR
			    ("Erase firmware section failed %d retries %d", ret,
			     retries);
			continue;
		}

		ret =
		    cts_program_firmware_from_sram_to_flash(cts_dev,
							    firmware->size);
		if (ret) {
			TS_LOG_ERR
			    ("Program firmware & crc section failed %d retries %d",
			     ret, retries);
			continue;
		}

		ret = validate_flash_data(cts_dev, CTS_FIRMWARE_SECTION_OFFSET,
					  firmware->size, crc_fw);
		if (ret) {
			TS_LOG_ERR("Validate flash crc  failed %d", ret);
			continue;
		}

		ret =
		    cts_erase_flash(cts_dev, CTS_FIRMWARE_CRC_SECTION_OFFSET,
				    FIRMWARE_CRC_SECTION_SIZE);
		if (ret) {
			TS_LOG_ERR
			    ("Erase firmware crc section failed %d retries %d",
			     ret, retries);
			continue;
		}

		ret = cts_crc_write_check(cts_dev, crc_fw, firmware->size);
		if (ret) {
			TS_LOG_ERR(" crc write check failed %d ", ret);
			continue;
		}

	} while (ret && retries < 3);

 out:
	cts_dev->rtdata.updating = false;

	if (ret == 0) {
		if (firmware->size <= cts_dev->hwdata->efctrl->xchg_sram_base) {
			ret = cts_enter_normal_mode(cts_dev);
			if (ret)
				TS_LOG_ERR("Enter normal mode failed %d", ret);
		}
	}

	end_time = ktime_get();
	delta_time = ktime_sub(end_time, start_time);
	TS_LOG_INFO("Update firmware, ELAPSED TIME: %lldms",
		    ktime_to_ms(delta_time));

	return ret;
}

bool cts_is_firmware_updating(const struct cts_device *cts_dev)
{
	return cts_dev->rtdata.updating;
}
