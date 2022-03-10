#ifdef CONFIG_SERIAL_MSM_CONSOLE

#include <linux/string.h>
#include <linux/printk.h>

extern char *saved_command_line;
bool is_serial_log_enabled(void)
{
	if ((strstr(saved_command_line, "console=ttyHSL0,115200,n8") != NULL) || (strstr(saved_command_line, "console=ttyMSM0,115200,n8") != NULL)) {
		pr_info("serial log enable \n");
		return true;
	}

	pr_info("serial log disable \n");

	return false;
}

#endif
