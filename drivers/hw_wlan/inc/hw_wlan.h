
#ifndef HW_WLAN_H
#define HW_WLAN_H
#include <linux/types.h> /* for size_t */

#ifndef DSM_WIFI_MODULE_INIT_ERROR
#define DSM_WIFI_MODULE_INIT_ERROR (909030001)
#endif
#ifndef DSM_WIFI_FIRMWARE_DL_ERROR
#define DSM_WIFI_FIRMWARE_DL_ERROR (909030007)
#endif
#ifndef DSM_WIFI_OPEN_ERROR
#define DSM_WIFI_OPEN_ERROR (909030019)
#endif
#ifndef DSM_WIFI_FIRMWARE_RESET_ERROR
#define DSM_WIFI_FIRMWARE_RESET_ERROR (909030041)
#endif

void hw_wlan_dsm_register_client(void);
void hw_wlan_dsm_unregister_client(void);
void hw_wlan_dsm_client_notify(int dsm_id, const char *fmt, ...);
int hw_wlan_get_cust_ini_path(char *ini_path, size_t len);
bool hw_wlan_is_driver_match(const char *driver_module_name);
const void *hw_wlan_get_board_name(void);
#endif //HW_WLAN_H
