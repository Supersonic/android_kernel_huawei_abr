#ifndef CTS_FIRMWARE_H
#define CTS_FIRMWARE_H

#include "cts_config.h"
#include <linux/firmware.h>

struct cts_firmware {
	const char *name;	/* MUST set to non-NULL if driver builtin firmware */
	u16 hwid;
	u16 fwid;
	u8 *data;
	size_t size;
	u16 ver_offset;
	bool is_fw_in_fs;
	const struct firmware *fw;
};

#define FIRMWARE_VERSION_OFFSET     0x114
#define FIRMWARE_VERSION(firmware)  \
	get_unaligned_le16((firmware)->data + FIRMWARE_VERSION_OFFSET)

enum e_upgrade_err_type {
	R_OK = 0,
	R_FILE_ERR,
	R_STATE_ERR,
	R_ERASE_ERR,
	R_PROGRAM_ERR,
	R_VERIFY_ERR,
};

struct cts_device;

const struct cts_firmware *cts_request_firmware(const struct cts_device
						*cts_dev, u32 hwid,
						u16 fwid, u16 device_fw_ver);
void cts_release_firmware(const struct cts_firmware *firmware);

int cts_update_firmware(struct cts_device *cts_dev,
			const struct cts_firmware *firmware, bool to_flash);
bool cts_is_firmware_updating(const struct cts_device *cts_dev);

bool cts_is_firmware_updating(const struct cts_device *cts_dev);

int cts_update_firmware(struct cts_device *cts_dev,
			const struct cts_firmware *firmware, bool to_flash);

#ifdef CFG_CTS_FIRMWARE_IN_FS
const struct cts_firmware *cts_request_firmware_from_fs(const struct cts_device
							*cts_dev,
							const char *filepath);
int cts_update_firmware_from_file(struct cts_device *cts_dev,
				  const char *filepath, bool to_flash);
const struct cts_firmware *cts_request_newer_firmware_from_fs(const struct
							      cts_device
							      *cts_dev,
							      const char
							      *filepath,
							      u16 curr_version);

#else				/* CFG_CTS_FIRMWARE_IN_FS */
static inline const struct cts_firmware *cts_request_firmware_from_fs(const
								      struct
								      cts_device
								      *cts_dev,
								      const char
								      *filepath)
{
	return NULL;
}

static inline int cts_update_firmware_from_file(struct cts_device *cts_dev,
						const char *filepath,
						bool to_flash)
{
	return -ENOTSUPP;
}
#endif				/* CFG_CTS_FIRMWARE_IN_FS */

#endif				/* CTS_FIRMWARE_H */
