
#include <dsm/dsm_pub.h>

#define DSM_WIFI_BUF_SIZE (1024)
#define DSM_WIFI_MOD_NAME "dsm_wifi"

static struct dsm_dev dsm_wifi = {
	.name = DSM_WIFI_MOD_NAME,
	.device_name = NULL,
	.ic_name = "QCOM",
	.module_name = NULL,
	.fops = NULL,
	.buff_size = DSM_WIFI_BUF_SIZE,
};

static struct dsm_client *hw_wifi_dsm_client;

void hw_wlan_dsm_register_client(void)
{
	if (hw_wifi_dsm_client == NULL)
		pr_info("dsm_wifi register");
		hw_wifi_dsm_client = dsm_register_client(&dsm_wifi);
	return;
}

EXPORT_SYMBOL(hw_wlan_dsm_register_client);

void hw_wlan_dsm_unregister_client(void)
{
	if (hw_wifi_dsm_client != NULL) {
		pr_info("dsm_wifi unregister");
		hw_wifi_dsm_client = NULL;
	}
	return;
}

EXPORT_SYMBOL(hw_wlan_dsm_unregister_client);

void hw_wlan_dsm_client_notify(int dsm_id, const char *fmt, ...)
{
	char buf[DSM_WIFI_BUF_SIZE] = {0};
	va_list ap;
	int size;

	if (hw_wifi_dsm_client) {
		if (fmt != NULL) {
			va_start(ap, fmt);
			size = vsnprintf(buf, DSM_WIFI_BUF_SIZE, fmt, ap);
			va_end(ap);
			if (size < 0) {
				pr_err("dsm_wifi buf copy failed");
				return;
			}
		}
		if (!dsm_client_ocuppy(hw_wifi_dsm_client)) {
			dsm_client_record(hw_wifi_dsm_client, buf);
			dsm_client_notify(hw_wifi_dsm_client, dsm_id);
			pr_err("dsm_wifi client notify success, "
				"dsm_id=%d[%s]", dsm_id, buf);
		} else {
			dsm_client_unocuppy(hw_wifi_dsm_client);
			if (!dsm_client_ocuppy(hw_wifi_dsm_client)) {
				dsm_client_record(hw_wifi_dsm_client, buf);
				dsm_client_notify(hw_wifi_dsm_client, dsm_id);
				pr_err("dsm_wifi notify success, dsm_id=%d[%s]",
					dsm_id, buf);
			} else {
				pr_err("dsm_wifi client ocuppy, "
					"dsm notify failed, dsm_id=%d", dsm_id);
			}
		}
	} else {
		pr_err("dsm_wifi client is null, dsm notify failed, "
			"dsm_id=%d", dsm_id);
	}
	return;
}

EXPORT_SYMBOL(hw_wlan_dsm_client_notify);
