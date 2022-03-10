#ifndef BOOTDEVICE_H
#define BOOTDEVICE_H
#include <linux/device.h>

#define SERIAL_NUM_SIZE 12
#define UFS_CID_SIZE 4
#define QUERY_DESC_GEOMETRY_MAX_SIZE    0x44

enum bootdevice_type { BOOT_DEVICE_EMMC = 0, BOOT_DEVICE_UFS = 1 };

void set_bootdevice_type(enum bootdevice_type type);
enum bootdevice_type get_bootdevice_type(void);
unsigned int get_bootdevice_manfid(void);
void set_bootdevice_name(enum bootdevice_type type, struct device *dev);
void set_bootdevice_size(enum bootdevice_type type, sector_t size);
void set_bootdevice_cid(enum bootdevice_type type, u32 *cid);
void set_bootdevice_fw_version(enum bootdevice_type type, char *fw_version);
void set_bootdevice_product_name(enum bootdevice_type type, char *product_name);
void set_bootdevice_hufs_dieid(enum bootdevice_type type, char *dieid);
void set_bootdevice_pre_eol_info(enum bootdevice_type type, u8 pre_eol_info);
void set_bootdevice_life_time_est_typ_a(enum bootdevice_type type, u8 life_time_est_typ_a);
void set_bootdevice_life_time_est_typ_b(enum bootdevice_type type, u8 life_time_est_typ_b);
void set_bootdevice_manfid(enum bootdevice_type type, unsigned int manfid);
void set_bootdevice_rev_handler(
	int (*get_rev_func)(const struct device *, char *));
#endif
