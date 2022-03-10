#include "cts_config.h"
#include "cts_platform.h"
#include "cts_core.h"
#include "cts_firmware.h"
#include "cts_test.h"
#include "cts_strerror.h"
#include "cts_dts.h"
#include <huawei_platform/log/log_jank.h>
#include "huawei_ts_kit.h"
#include "huawei_ts_kit_algo.h"
#include "huawei_ts_kit_api.h"
#if defined(CONFIG_TEE_TUI)
#include "tui.h"
#endif
#if defined(CONFIG_HUAWEI_DSM)
#include <dsm/dsm_pub.h>
#endif

struct chipone_ts_data *cts_data = NULL;

static void parse_dts_project_basic(const struct device_node *of_node)
{
	const char *producer = NULL;

	int ret = NO_ERR;

	if (of_node == NULL || cts_data->tskit_data == NULL ) {
		TS_LOG_ERR ("%s,Parse dts config with of_node/ts_kit_device_data = NULL\n",
			__func__);
		return ;
	}

	TS_LOG_INFO("basic param:\n");

	ret = of_property_read_string(of_node, CTS_PRODUCER, &producer);
	if (ret) {
		TS_LOG_INFO("Get producer from dts failed %d(%s)\n",
			    ret, cts_strerror(ret));
	}
	if (producer) {
		strncpy(cts_data->tskit_data->module_name, producer,
			sizeof(cts_data->tskit_data->module_name) - 1);
	}

	ret = of_property_read_u8_array(of_node, CTS_BOOT_RESET_TIMING,
			cts_data->boot_hw_reset_timing,
			ARRAY_SIZE(cts_data->boot_hw_reset_timing));
	if (ret) {
		TS_LOG_INFO
		    ("Get 'boot-hw-reset-timing' from dts failed %d(%s)\n", ret,
		     cts_strerror(ret));
		cts_data->boot_hw_reset_timing[0] = 2;
		cts_data->boot_hw_reset_timing[1] = 5;
		cts_data->boot_hw_reset_timing[2] = 40;
	}
	TS_LOG_INFO(" - %-32s: [H:%u, L:%u, H:%u]\n", "Boot hw reset timing",
		    cts_data->boot_hw_reset_timing[0],
		    cts_data->boot_hw_reset_timing[1],
		    cts_data->boot_hw_reset_timing[2]);

	ret = of_property_read_u8_array(of_node, CTS_RESUME_RESET_TIMING,
		cts_data->resume_hw_reset_timing,
		ARRAY_SIZE(cts_data->resume_hw_reset_timing));
	if (ret) {
		TS_LOG_INFO("Get 'resume-hw-reset-timing' from dts failed %d(%s)\n",
			ret, cts_strerror(ret));
		cts_data->resume_hw_reset_timing[0] = 2;
		cts_data->resume_hw_reset_timing[1] = 5;
		cts_data->resume_hw_reset_timing[2] = 40;
	}
	TS_LOG_INFO(" - %-32s: [H:%u, L:%u, H:%u]\n", CTS_RESUME_RESET_TIMING,
		    cts_data->boot_hw_reset_timing[0],
		    cts_data->boot_hw_reset_timing[1],
		    cts_data->boot_hw_reset_timing[2]);

	ret = of_property_read_u32(of_node,
			CTS_IC_TYPE, &cts_data->tskit_data->ic_type);
	if (ret) {
		TS_LOG_ERR("Get ic-type from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		cts_data->tskit_data->ic_type = CTS_DEV_HWID_ICNT8918;
	}
	TS_LOG_INFO(" - %-32s: 0x%06x\n", "IC type",
		    cts_data->tskit_data->ic_type);

	ret = of_property_read_u32(of_node,
				   CTS_ALGO_ID, &cts_data->tskit_data->algo_id);
	if (ret) {
		TS_LOG_ERR("Get algo_id from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		cts_data->tskit_data->algo_id = 0;
	}
	TS_LOG_INFO(" - %-32s: %u\n", "Algo id", cts_data->tskit_data->algo_id);

	ret = of_property_read_u32(of_node,
				   CTS_IRQ_CONFIG,
				   &cts_data->tskit_data->irq_config);
	if (ret) {
		TS_LOG_ERR("Get irq_config from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		cts_data->tskit_data->irq_config = 0;
	}
	TS_LOG_INFO(" - %-32s: %u\n", "IRQ config",
		    cts_data->tskit_data->irq_config);
	ret = of_property_read_u32(of_node, TEST_CAPACITANCE_VIA_CSVFILE,
		&cts_data->test_capacitance_via_csvfile);
	if (ret) {
		TS_LOG_INFO("test_capacitance_via_csvfile is 0.\n");
		cts_data->test_capacitance_via_csvfile = 0;
		ret = 0;
	} else {
		TS_LOG_INFO("test_capacitance_via_csvfile = %d\n",
			    cts_data->test_capacitance_via_csvfile);
	}

	ret =
	    of_property_read_u32(of_node, CSVFILE_USE_PRODUCT_SYSTEM_TYPE,
				 &cts_data->csvfile_use_product_system);
	if (ret) {
		TS_LOG_INFO("csvfile use product system is 0.\n");
		cts_data->csvfile_use_product_system = 0;
		ret = 0;
	} else {
		TS_LOG_INFO("csvfile_use_product_system = %d\n",
			    cts_data->csvfile_use_product_system);
	}
}

static void parse_dts_input_config(const struct device_node *of_node)
{
	struct ts_kit_device_data *tskit_dev_data;
	int ret = NO_ERR;

    if (of_node == NULL || cts_data == NULL || cts_data->tskit_data == NULL ) {
		TS_LOG_ERR
		    ("%s,Parse dts config with of_node/ts_kit_device_data = NULL\n",__func__);
		return;
	}

	tskit_dev_data = cts_data->tskit_data;

	TS_LOG_INFO("* Project input config:\n");

	ret = of_property_read_u32(of_node, CTS_X_MAX,
				   (u32 *) &tskit_dev_data->x_max);
	if (ret) {
		TS_LOG_ERR("Get x_max from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		tskit_dev_data->x_max = 340;
	}
	TS_LOG_INFO(" - %-32s: %u\n", "X Max", tskit_dev_data->x_max);

	ret = of_property_read_u32(of_node, CTS_Y_MAX,
				   (u32 *) &tskit_dev_data->y_max);
	if (ret) {
		TS_LOG_ERR("Get y_max from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		tskit_dev_data->y_max = 340;
	}
	TS_LOG_INFO(" - %-32s: %u\n", "Y Max", tskit_dev_data->y_max);

	ret = of_property_read_u32(of_node, CTS_X_MAX_MT,
				   (u32 *) &tskit_dev_data->x_max_mt);
	if (ret) {
		TS_LOG_ERR("Get x_max_mt from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		tskit_dev_data->x_max_mt = 340;
	}
	TS_LOG_INFO(" - %-32s: %u\n", "X Max MT", tskit_dev_data->x_max_mt);

	ret = of_property_read_u32(of_node, CTS_Y_MAX_MT,
				   (u32 *) &tskit_dev_data->y_max_mt);
	if (ret) {
		TS_LOG_ERR("Get y_max_mt from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		tskit_dev_data->y_max_mt = 340;
	}
	TS_LOG_INFO(" - %-32s: %u\n", "Y Max MT", tskit_dev_data->y_max_mt);

}

static void parse_dts_project_facotry_test(const struct device_node *proj_of_node)
{
	const char *str = NULL;
	int ret = NO_ERR;

	TS_LOG_INFO("* Project factory test:\n");

	ret = of_property_read_u32(proj_of_node, CTS_DUMP_TEST_DATA_TO_CONSOLE,
	    	(u32 *)&cts_data->dump_factory_test_data_to_console);
	TS_LOG_INFO(" - %-32s: %c\n", "Dump test data to console",
		    cts_data->dump_factory_test_data_to_console ? 'Y' : 'N');

	ret = of_property_read_u32(proj_of_node, CTS_DUMP_TEST_DATA_TO_BUF,
	    	(u32 *)&cts_data->dump_factory_test_data_to_buffer);
	TS_LOG_INFO(" - %-32s: %c\n", "Dump test data to buffer",
		    cts_data->dump_factory_test_data_to_buffer ? 'Y' : 'N');

	ret = of_property_read_u32(proj_of_node, CTS_DUMP_TEST_DATA_TO_FILE,
	    	(u32 *)&cts_data->dump_factory_test_data_to_file);
	TS_LOG_INFO(" - %-32s: %c\n", "Dump test data to file",
		    cts_data->dump_factory_test_data_to_file ? 'Y' : 'N');

	ret = of_property_read_u32(proj_of_node, CTS_PRINT_TEST_DATA_ONLY_FAIL,
	    	(u32 *)&cts_data->print_factory_test_data_only_fail);
	TS_LOG_INFO(" - %-32s: %c\n", "Dump test data only fail",
		    cts_data->print_factory_test_data_only_fail ? 'Y' : 'N');

	ret = of_property_read_string(proj_of_node,
	     CTS_TEST_DATA_DIRECTORY, &str);
	 if (ret) {
		TS_LOG_ERR
		    ("Get factory-test-data-directory from dts failed %d(%s)\n",
		     ret, cts_strerror(ret));
		strcpy(cts_data->factory_test_data_directory,
		       "/sdcard/chipone/test/");
	} else {
		strncpy(cts_data->factory_test_data_directory, str,
			sizeof(cts_data->factory_test_data_directory) - 1);
	}
	TS_LOG_INFO(" - %-32s: %s\n", "Test data directory",
		    cts_data->factory_test_data_directory);
}

static int init_pinctrl(void)
{
	int ret = NO_ERR;

	if (!cts_data->use_pinctrl) {
		TS_LOG_INFO("Init pinctrl while NOT use\n");
		return 0;
	}

	TS_LOG_INFO("Init pinctrl\n");

	cts_data->pinctrl = devm_pinctrl_get(cts_data->device);
	if (IS_ERR_OR_NULL(cts_data->pinctrl)) {
		ret = PTR_ERR(cts_data->pinctrl);
		TS_LOG_ERR("Get pinctrl failed %d(%s)\n",
			   ret, cts_strerror(ret));
		cts_data->pinctrl = NULL;
		return ret;
	}
#ifdef CONFIG_HUAWEI_DEVKIT_MTK_3_0
#define PINCTRL_STATE_RESET_HIGH_NAME   "state_rst_output1"
#define PINCTRL_STATE_RESET_LOW_NAME    "state_rst_output0"
#define PINCTRL_STATE_INT_HIGH_NAME     "state_eint_output1"
#define PINCTRL_STATE_INT_LOW_NAME      "state_eint_output0"
#define PINCTRL_STATE_AS_INT_NAME       "state_eint_as_int"

	cts_data->pinctrl_state_reset_high =
	    pinctrl_lookup_state(cts_data->pinctrl,
				 PINCTRL_STATE_RESET_HIGH_NAME);
	if (IS_ERR_OR_NULL(cts_data->pinctrl_state_reset_high)) {
		ret = -PTR_ERR(cts_data->pinctrl_state_reset_high);
		TS_LOG_ERR("Lookup pinctrl state '"
			   PINCTRL_STATE_RESET_HIGH_NAME "' failed %d(%s)\n",
			   ret, cts_strerror(ret));
		goto put_pinctrl;
	}
	cts_data->pinctrl_state_reset_low =
	    pinctrl_lookup_state(cts_data->pinctrl,
				 PINCTRL_STATE_RESET_LOW_NAME);
	if (IS_ERR_OR_NULL(cts_data->pinctrl_state_reset_low)) {
		ret = -PTR_ERR(cts_data->pinctrl_state_reset_low);
		TS_LOG_ERR("Lookup pinctrl state '"
			   PINCTRL_STATE_RESET_LOW_NAME "' failed %d(%s)\n",
			   ret, cts_strerror(ret));
		goto put_pinctrl;
	}
	cts_data->pinctrl_state_as_int =
	    pinctrl_lookup_state(cts_data->pinctrl, PINCTRL_STATE_AS_INT_NAME);
	if (IS_ERR_OR_NULL(cts_data->pinctrl_state_as_int)) {
		ret = -PTR_ERR(cts_data->pinctrl_state_as_int);
		TS_LOG_ERR("Lookup pinctrl state '"
			   PINCTRL_STATE_AS_INT_NAME "' failed %d(%s)\n",
			   ret, cts_strerror(ret));
		goto put_pinctrl;
	}
	cts_data->pinctrl_state_int_high =
	    pinctrl_lookup_state(cts_data->pinctrl,
				 PINCTRL_STATE_INT_HIGH_NAME);
	if (IS_ERR_OR_NULL(cts_data->pinctrl_state_int_high)) {
		ret = -PTR_ERR(cts_data->pinctrl_state_int_high);
		TS_LOG_ERR("Lookup pinctrl state '"
			   PINCTRL_STATE_INT_HIGH_NAME "' failed %d(%s)\n",
			   ret, cts_strerror(ret));
		goto put_pinctrl;
	}
	cts_data->pinctrl_state_int_low =
	    pinctrl_lookup_state(cts_data->pinctrl, PINCTRL_STATE_INT_LOW_NAME);
	if (IS_ERR_OR_NULL(cts_data->pinctrl_state_int_low)) {
		ret = -PTR_ERR(cts_data->pinctrl_state_int_low);
		TS_LOG_ERR("Lookup pinctrl state '"
			   PINCTRL_STATE_INT_LOW_NAME "' failed %d(%s)\n",
			   ret, cts_strerror(ret));
		goto put_pinctrl;
	}
#undef PINCTRL_STATE_RESET_HIGH
#undef PINCTRL_STATE_RESET_LOW
#undef PINCTRL_STATE_INT_HIGH
#undef PINCTRL_STATE_INT_LOW
#undef PINCTRL_STATE_AS_INT

#else				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */
	cts_data->pinctrl_state_default =
	    pinctrl_lookup_state(cts_data->pinctrl, "default");
	if (IS_ERR(cts_data->pinctrl_state_default)) {
		ret = PTR_ERR(cts_data->pinctrl_state_default);
		TS_LOG_ERR("Lookup active pinctrl state failed %d(%s)\n",
			   ret, cts_strerror(ret));
		goto put_pinctrl;
	}

	cts_data->pinctrl_state_idle =
	    pinctrl_lookup_state(cts_data->pinctrl, "idle");
	if (IS_ERR(cts_data->pinctrl_state_idle)) {
		ret = PTR_ERR(cts_data->pinctrl_state_default);
		TS_LOG_ERR("Lookup suspend pinctrl state failed %d(%s)\n",
			   ret, cts_strerror(ret));
		goto put_pinctrl;
	}
#endif				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */

	return ret;

 put_pinctrl:
	if (cts_data->pinctrl) {
		devm_pinctrl_put(cts_data->pinctrl);
		cts_data->pinctrl = NULL;
	}
#ifdef CONFIG_HUAWEI_DEVKIT_MTK_3_0
	cts_data->pinctrl_state_reset_high = NULL;
	cts_data->pinctrl_state_reset_low = NULL;
	cts_data->pinctrl_state_release = NULL;
	cts_data->pinctrl_state_int_high = NULL;
	cts_data->pinctrl_state_int_low = NULL;
	cts_data->pinctrl_state_as_int = NULL;
#else				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */
	cts_data->pinctrl_state_default = NULL;
	cts_data->pinctrl_state_idle = NULL;
#endif				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */

	return ret;
}

static int release_pinctrl(void)
{
	if (!cts_data->use_pinctrl) {
		TS_LOG_INFO("Release pinctrl with use_pinctrl = 0\n");
		return 0;
	}

	if (cts_data->pinctrl == NULL) {
		TS_LOG_INFO("Release pinctrl with pinctrl = NULL\n");
		return 0;
	}

	TS_LOG_INFO("Release pinctrl\n");

	devm_pinctrl_put(cts_data->pinctrl);
	cts_data->pinctrl = NULL;
#ifdef CONFIG_HUAWEI_DEVKIT_MTK_3_0
	cts_data->pinctrl_state_reset_high = NULL;
	cts_data->pinctrl_state_int_high = NULL;
	cts_data->pinctrl_state_int_low = NULL;
	cts_data->pinctrl_state_reset_low = NULL;
	cts_data->pinctrl_state_as_int = NULL;
#else				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */
	cts_data->pinctrl_state_default = NULL;
	cts_data->pinctrl_state_idle = NULL;
#endif				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */

	return 0;
}

static int pinctrl_select_default_state(void)
{
	int ret = NO_ERR;
	if (!cts_data->use_pinctrl) {
		TS_LOG_INFO
		    ("Select default pinctrl state with use_pinctrl = 0\n");
		return 0;
	}
#ifdef CONFIG_HUAWEI_DEVKIT_MTK_3_0
	ret = pinctrl_select_state(cts_data->pinctrl,
				   cts_data->pinctrl_state_reset_high);
	if (ret < 0) {
		TS_LOG_ERR("Select reset default pinctrl state failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}
	ret = pinctrl_select_state(cts_data->pinctrl,
				   cts_data->pinctrl_state_as_int);
	if (ret < 0) {
		TS_LOG_ERR("Select int default pinctrl state failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}
#else				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */
	if (IS_ERR_OR_NULL(cts_data->pinctrl) ||
	    IS_ERR_OR_NULL(cts_data->pinctrl_state_default)) {
		TS_LOG_ERR("Select default pinctrl state with "
			   "pinctrl = NULL or pinctrl_state_default = NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Select default pinctrl state\n");

	ret = pinctrl_select_state(cts_data->pinctrl,
				   cts_data->pinctrl_state_default);
	if (ret < 0) {
		TS_LOG_ERR("Select default pinctrl state failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}
#endif				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */

	return 0;
}

static int pinctrl_select_idle_state(void)
{
	int ret = NO_ERR;

	if (!cts_data->use_pinctrl) {
		TS_LOG_INFO("Select idle pinctrl state with use_pinctrl = 0\n");
		return -ENODEV;
	}
#ifdef CONFIG_HUAWEI_DEVKIT_MTK_3_0
	/* TODO:*/
#else				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */
	if (IS_ERR_OR_NULL(cts_data->pinctrl) ||
	    IS_ERR_OR_NULL(cts_data->pinctrl_state_idle)) {
		TS_LOG_ERR("Select ilde pinctrl state with "
			   "pinctrl = NULL or pinctrl_state_idle = NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Select default pinctrl state\n");

	ret = pinctrl_select_state(cts_data->pinctrl,
				   cts_data->pinctrl_state_idle);
	if (ret < 0) {
		TS_LOG_ERR("Select idle pinctrl state failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}
#endif				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */

	return ret;
}

static int set_reset_gpio_value(int value)
{
#ifdef CONFIG_HUAWEI_DEVKIT_MTK_3_0
	struct pinctrl_state *pinctrl_state_reset;
	int ret;
	TS_LOG_INFO("Set reset pin to %s\n", (!!value) ? "HIGH" : "LOW");

	pinctrl_state_reset = (! !value) ? cts_data->pinctrl_state_reset_high :
	    cts_data->pinctrl_state_reset_low;
	if (cts_data->pinctrl == NULL || pinctrl_state_reset == NULL) {
		TS_LOG_ERR("Set reset gpio value with "
			   "pinctrl = NULL or reset pinctrl_state = NULL\n");
		return -EINVAL;
	}
	ret = pinctrl_select_state(cts_data->pinctrl, pinctrl_state_reset);
	if (ret) {
		TS_LOG_ERR("Select reset pinctrl state failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}

	return 0;
#else				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */
	int ret = NO_ERR;

	TS_LOG_INFO("Set reset pin to %s\n", (!!value) ? "HIGH" : "LOW");

	if (cts_data->tskit_data->ts_platform_data->reset_gpio <= 0) {
		TS_LOG_INFO("Set reset gpio value with reset gpio %d\n",
			    cts_data->tskit_data->ts_platform_data->reset_gpio);
		return -EFAULT;
	}

	ret =
	    gpio_direction_output(cts_data->tskit_data->ts_platform_data->
				  reset_gpio, (!!value) ? 1 : 0);
	if (ret) {
		TS_LOG_ERR("Set reset gpio value use "
			   "gpio_direction_output() failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}

	return ret;
#endif				/* CONFIG_HUAWEI_DEVKIT_MTK_3_0 */
}

static int do_hw_reset(const u8 *hw_reset_timing)
{
	int ret = NO_ERR;

	if (NULL == hw_reset_timing) {
		TS_LOG_ERR("%s:Invalid reset_timing\n", __func__);
		return -EINVAL;
		}
	TS_LOG_INFO("Do harware reset, timing: [H:%u, L:%u, H:%u]\n",
		    hw_reset_timing[0], hw_reset_timing[1], hw_reset_timing[2]);

	ret = set_reset_gpio_value(1);
	if (ret) {
		TS_LOG_ERR("Set reset gpio to HIGH failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}
	mdelay(hw_reset_timing[0]);

	ret = set_reset_gpio_value(0);
	if (ret) {
		TS_LOG_ERR("Set reset gpio to LOW failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}
	mdelay(hw_reset_timing[1]);

	ret = set_reset_gpio_value(1);
	if (ret) {
		TS_LOG_ERR("Set reset gpio to HIGH failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}
	mdelay(hw_reset_timing[2]);

	return ret;
}

static int chipone_chip_init(void)
{
	int ret = NO_ERR;

	if (cts_data == NULL) {
		TS_LOG_ERR("Chip init with cts_data = NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Chip init\n");
	ret = cts_tool_init(cts_data);
    if (ret < 0) {
        TS_LOG_ERR("Add proc entry for device failed %d", ret);
    }
	return ret;
}

static int chipone_parse_config(struct device_node *of_node,
				struct ts_kit_device_data *data)
{
	int ret = NO_ERR;

	if (of_node == NULL || data == NULL) {
		TS_LOG_ERR
		    ("Parse dts config with of_node/ts_kit_device_data = NULL\n");
		return EINVAL;
	}

	if (cts_data == NULL) {
		TS_LOG_ERR("Parse dts config with cts_data = NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Parse dts config\n");

	ret = of_property_read_u32(of_node, CTS_USE_PINCTRL,
		(u32 *)&cts_data->use_pinctrl);
	TS_LOG_INFO(" - %-32s: %c\n", "Use pinctrl",
		    cts_data->use_pinctrl ? 'Y' : 'N');

	ret = of_property_read_u32(of_node, CTS_USE_CHARER_DETECT,
	    	(u32 *)&cts_data->use_huawei_charger_detect);
	TS_LOG_INFO(" - %-32s: %c\n", "Use huawei charger detect",
		    cts_data->use_huawei_charger_detect ? 'Y' : 'N');

	ret = of_property_read_u32(of_node, CTS_RAWDATA_PROC_PRINTF,
				   (u32 *) &data->is_ic_rawdata_proc_printf);
	if (ret) {
		TS_LOG_ERR
		    ("Get is_ic_rawdata_proc_printf from dts failed %d(%s)\n",
		     ret, cts_strerror(ret));
		data->is_ic_rawdata_proc_printf = 0;
	}
	TS_LOG_INFO(" - %-32s: %c\n", "IC rawdata proc printf",
		    data->is_ic_rawdata_proc_printf ? 'Y' : 'N');

	ret = of_property_read_u32(of_node, CTS_HIDE_FW_NAME,
				   (u32 *) &cts_data->hide_fw_name);
	if (ret) {
		TS_LOG_ERR("Get hide_fw_name from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		cts_data->hide_fw_name = 0;
	}
	TS_LOG_INFO(" - %-32s: %c\n", CTS_HIDE_FW_NAME,
		    cts_data->hide_fw_name ? 'Y' : 'N');
	ret = of_property_read_string(of_node, CTS_TP_TEST_TYPE,
				      (const char **)&data->tp_test_type);
	if (ret) {
		TS_LOG_ERR("Get tp_test_type from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
	}
	TS_LOG_INFO(" - %-32s: %s\n", "TP test type", data->tp_test_type);

	ret = of_property_read_u32(of_node, CTS_RAWDATA_NEWFORMAT,
				   (u32 *) &data->rawdata_newformatflag);
	if (ret) {
		TS_LOG_ERR("Get rawdata_newformatflag from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		data->rawdata_newformatflag = 0;
	}
	TS_LOG_INFO(" - %-32s: %c\n", "Rawdata new format",
		    data->rawdata_newformatflag ? 'Y' : 'N');

	ret = of_property_read_u32(of_node, CTS_TOUCH_SWITCH_FLAG,
				   (u32 *) &data->touch_switch_flag);
	if (ret) {
		TS_LOG_ERR("Get touch_switch_flag from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		data->touch_switch_flag = 0;
	}
	TS_LOG_INFO(" - %-32s: %c\n", "Touch switch flag",
		    data->touch_switch_flag ? 'Y' : 'N');

	ret = of_property_read_u32(of_node, CTS_WD_CHECK_STATUS,
				   (u32 *) &data->need_wd_check_status);
	if (ret) {
		TS_LOG_ERR("Get need_wd_check_status from dts failed %d(%s)\n",
			   ret, cts_strerror(ret));
		data->need_wd_check_status = 0;
	}
	TS_LOG_INFO(" - %-32s: %c\n", "Need WDT check status",
		    data->need_wd_check_status ? 'Y' : 'N');

	ret = of_property_read_u32(of_node, CTS_CHECK_STATUS_TIMEOUT,
				   (u32 *) &data->check_status_watchdog_timeout);
	if (ret) {
		TS_LOG_ERR
		    ("Get check_status_watchdog_timeout from dts failed %d(%s)\n",
		     ret, cts_strerror(ret));
		data->check_status_watchdog_timeout = 0;
	}
	TS_LOG_INFO(" - %-32s: %u\n", "WDT check status timeout",
		    data->check_status_watchdog_timeout);

	parse_dts_input_config(of_node);
	parse_dts_project_basic(of_node);
	parse_dts_project_facotry_test(of_node);

	return ret;
}

static int chipone_power_on_gpio_set(void)
{
	int ret = NO_ERR;
	ret = set_reset_gpio_value(1);
	if (ret) {
		TS_LOG_ERR("Set reset gpio to HIGH failed %d(%s)\n",
						ret, cts_strerror(ret));
		return ret;
	}
		return ret;
}
static int chipone_power_off_gpio_set(void)
{
	int ret = NO_ERR;
	ret = set_reset_gpio_value(0);
	if (ret) {
		TS_LOG_ERR("Set reset gpio to low failed %d(%s)\n",
					ret, cts_strerror(ret));
		return ret;
	}
	return ret ;
}


static int chipone_power_init(void)
{
	int ret = NO_ERR;
	ret = ts_kit_power_supply_get(TS_KIT_IOVDD);
	if(ret)
		return ret;
	ret = ts_kit_power_supply_get(TS_KIT_VCC);
	return ret;
}

static int chipone_power_release(void)
{
	int ret = NO_ERR;
	TS_LOG_INFO("cts_power_off called\n");
	ret = ts_kit_power_supply_put(TS_KIT_IOVDD);
	if(ret)
		return ret;
	ret = ts_kit_power_supply_put(TS_KIT_VCC);
	return ret;
}

static int chipone_power_on(void)
{
	int ret = NO_ERR;
	TS_LOG_INFO("cts_power_on called\n");

	ret = ts_kit_power_supply_ctrl(TS_KIT_VCC, TS_KIT_POWER_ON, 0);
	if(ret)
		return ret;
	udelay(5);
	ret = ts_kit_power_supply_ctrl(TS_KIT_IOVDD, TS_KIT_POWER_ON, 15);
	if(ret)
		return ret;

	ret = chipone_power_on_gpio_set();
	if (ret) {
		TS_LOG_ERR("powef on gpio set failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}
	return ret;
}
static int  chipone_power_off(void)
{
	int ret = NO_ERR;
	ret = ts_kit_power_supply_ctrl(TS_KIT_IOVDD, TS_KIT_POWER_OFF, 0);
	if(ret)
		return ret;
	udelay(2);
	ret = ts_kit_power_supply_ctrl(TS_KIT_VCC, TS_KIT_POWER_OFF, 0);
	if(ret)
		return ret;
	udelay(2);

    ret = chipone_power_off_gpio_set();
	if (ret) {
		TS_LOG_ERR("powef off gpio set failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}

	return ret;
}
static int chipone_power_switch(int on)
{
	int ret = 0;
	if (on) {
		ret = chipone_power_on();
	} else {
		ret = chipone_power_off();
	}
	return ret ;
}

static int cts_chip_detect(struct ts_kit_platform_data *data)
{
	int ret = NO_ERR;

	if (data == NULL) {
		TS_LOG_ERR("Detect chip with ts_kit_platform_data = NULL\n");
		return -EINVAL;
	}

	if (!data->chip_data || !data->chip_data->cnode) {
		TS_LOG_ERR("Detect chip with tskit_data.cnode = NULL\n");
		return -EFAULT;
	}

	if (cts_data == NULL) {
		TS_LOG_ERR("Detect chip with cts_data = NULL\n");
		return -EINVAL;
	}
	TS_LOG_INFO("chip_detect enter\n");

	if (dev_get_drvdata(&data->ts_dev->dev)) {
		TS_LOG_ERR("Detect chip with drvdata != NULL");
		return -EBUSY;
	}
	TS_LOG_INFO("%s:Driver Version: %s\n", __func__,
		    CFG_CTS_DRIVER_VERSION);
	TS_LOG_INFO("Chip detect");
	cts_data->tskit_data->ts_platform_data = data;
	cts_data->device = &data->ts_dev->dev;
	cts_data->device->of_node = cts_data->tskit_data->cnode;

	dev_set_drvdata(&data->ts_dev->dev, cts_data);
	cts_data->pdata->i2c_client = data->client;
	cts_data->pdata->int_gpio = data->irq_gpio;
	cts_data->pdata->rst_gpio = data->reset_gpio;
	cts_data->cts_dev.pdata = cts_data->pdata;

	chipone_parse_config(data->chip_data->cnode, data->chip_data);

	ret = chipone_power_init();
	if (ret) {
		TS_LOG_ERR("Chip power init error");
		goto exit;
	}

	ret = chipone_power_switch(SWITCH_ON);
 	if (ret) {
		TS_LOG_ERR("Chip power on error");
		goto exit;
	}

	init_pinctrl();
	pinctrl_select_default_state();
	do_hw_reset(cts_data->boot_hw_reset_timing);

	ret = cts_probe_device(&cts_data->cts_dev);
	if (ret) {
		TS_LOG_ERR("Probe device failed %d(%s)\n",
			ret, cts_strerror(ret));
		goto err_power_off;
	}

	strncpy(cts_data->tskit_data->chip_name,
		cts_data->cts_dev.hwdata->name,
		sizeof(cts_data->tskit_data->chip_name) - 1);

	cts_data->cts_dev.pdata = cts_data->pdata;
	cts_data->chip_detect_ok = true;
	TS_LOG_INFO("Chip detect success %d\n", ret);
	return ret;

err_power_off:
	chipone_power_switch(SWITCH_OFF);
	chipone_power_release();
	release_pinctrl();
exit:
	cts_data->chip_detect_ok = false;
	kfree(cts_data->tskit_data);
	cts_data->tskit_data = NULL;
	kfree(cts_data->pdata);
	cts_data->pdata = NULL;
	kfree(cts_data);
	cts_data = NULL;

	TS_LOG_INFO("Chip detect returns %d\n", ret);
	return ret;
}

static int chipone_input_config(struct input_dev *input_dev)
{
	int ret = NO_ERR;

	if (cts_data == NULL) {
		TS_LOG_ERR("Config input device with cts_data = NULL\n");
		return -EFAULT;
	}

	if (input_dev == NULL) {
		TS_LOG_ERR("Config input device with input_dev = NULL\n");
		return -EINVAL;
	}

    TS_LOG_INFO("Config input device, X: %u, Y: %u, N: %u\n",
        cts_data->tskit_data->x_max, cts_data->tskit_data->y_max,
        cts_data->tskit_data->ts_platform_data->max_fingers);

	input_dev->evbit[0] = BIT_MASK(EV_SYN) |
	    BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	input_set_capability(input_dev, EV_KEY, BTN_TOUCH);
	__set_bit(INPUT_PROP_DIRECT, input_dev->propbit);

	input_set_abs_params(input_dev, ABS_X,
			     0, (cts_data->tskit_data->x_max - 1), 0, 0);
	input_set_abs_params(input_dev, ABS_Y,
			     0, (cts_data->tskit_data->y_max - 1), 0, 0);
	input_set_abs_params(input_dev, ABS_PRESSURE, 0, 255, 0, 0);

	input_set_abs_params(input_dev, ABS_MT_POSITION_X,
			     0, (cts_data->tskit_data->x_max - 1), 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y,
			     0, (cts_data->tskit_data->y_max - 1), 0, 0);
	input_set_abs_params(input_dev, ABS_MT_PRESSURE, 0, 255, 0, 0);

    input_set_abs_params(input_dev, ABS_MT_TRACKING_ID,
        0, cts_data->tskit_data->ts_platform_data->max_fingers * 2,
        0, 0);
	__set_bit(TS_DOUBLE_CLICK, input_dev->keybit);
	__set_bit(TS_SINGLE_CLICK, input_dev->keybit);
#ifdef CONFIG_CTS_GESTURE
	input_set_capability(input_dev, EV_KEY, TS_DOUBLE_CLICK);
#endif				/* CONFIG_CTS_GESTURE */

#ifdef TYPE_B_PROTOCOL
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,7,0)
    input_mt_init_slots(input_dev,
        cts_data->tskit_data->ts_platform_data->max_fingers,
        INPUT_MT_DIRECT);
#else
    input_mt_init_slots(input_dev,
        cts_data->tskit_data->ts_platform_data->max_fingers);
#endif
#endif				/* TYPE_B_PROTOCOL */

	return ret;
}

static int chipone_irq_top_half(struct ts_cmd_node *cmd)
{
	cmd->command = TS_INT_PROCESS;

	return NO_ERR;
}

static int process_touch_msg(struct ts_fingers *info,
			     struct cts_device_touch_msg *msgs, int num)
{
	int i;

	TS_LOG_DEBUG("Process touch %d msgs\n", num);
	if ((info == NULL) || (msgs == NULL)) {
		TS_LOG_ERR("process_touch_msg with input NULL\n");
		return -EINVAL;
	}

	memset(info, 0, sizeof(*info));

	for (i = 0; i < num; i++) {
		u16 x, y;

		x = le16_to_cpu(msgs[i].x);
		y = le16_to_cpu(msgs[i].y);
		if ((x > X_MAX) || (y > Y_MAX)) {
			TS_LOG_ERR("%s:invalid coord, x = %d y = %d\n", __func__, x, y);
			x = X_MAX;
			y = Y_MAX;
		}

		TS_LOG_DEBUG
		    ("  Process touch msg[%d]: id[%u] ev=%u p=%u\n",
		     i, msgs[i].id, msgs[i].event, msgs[i].pressure);

		/* ts_finger_events_reformat() will OR
		 * TS_FINGER_PRESS/TS_FINGER_RELEASE to status.
		 */
		switch (msgs[i].event) {
		case CTS_DEVICE_TOUCH_EVENT_DOWN:
		case CTS_DEVICE_TOUCH_EVENT_MOVE:
		case CTS_DEVICE_TOUCH_EVENT_STAY:
			info->fingers[msgs[i].id].x = (X_MAX - x);
			info->fingers[msgs[i].id].y = (Y_MAX - y);
			info->fingers[msgs[i].id].major = 1;
			info->fingers[msgs[i].id].minor = 1;
			info->fingers[msgs[i].id].pressure = msgs[i].pressure;
			info->fingers[msgs[i].id].status = TP_FINGER;
			info->cur_finger_number++;
			break;
		case CTS_DEVICE_TOUCH_EVENT_UP:
			info->fingers[msgs[i].id].status = TP_NONE_FINGER;
			break;
		default:
			TS_LOG_ERR
			    ("Process touch msg with unknown event %u id %u\n",
			     msgs[i].event, msgs[i].id);
			break;
		}
	}

	return 0;
}

int process_gesture_info(struct ts_fingers *info, u8 gesture_id)
{
	TS_LOG_ERR("Process gesture info, ID: 0x%02x\n", gesture_id);

	switch (gesture_id) {
	case GESTURE_DOUBLE_TAP:
		mutex_lock(&cts_data->wrong_touch_lock);
		if (cts_data->tskit_data->easy_wakeup_info.off_motion_on)
			info->gesture_wakeup_value = TS_DOUBLE_CLICK;
		mutex_unlock(&cts_data->wrong_touch_lock);
		break;
	case GESTURE_SINGLE_TAP:
		mutex_lock(&cts_data->wrong_touch_lock);
		if (cts_data->tskit_data->easy_wakeup_info.off_motion_on)
			info->gesture_wakeup_value = TS_SINGLE_CLICK;
		mutex_unlock(&cts_data->wrong_touch_lock);
		break;
	default:
		TS_LOG_ERR("Get unknown gesture code 0x%02x\n", gesture_id);
		break;
	}

	return 0;
}

static int chipone_irq_bottom_half(struct ts_cmd_node *in_cmd,
				   struct ts_cmd_node *out_cmd)
{
	struct ts_fingers *info = NULL;
	struct cts_device *cts_dev = NULL;
	struct cts_device_touch_info *touch_info;
	int ret;

	if (cts_data == NULL) {
		TS_LOG_ERR("Irq bottom half with cts_data = NULL\n");
		return -EINVAL;
	}

	if (out_cmd == NULL) {
		TS_LOG_ERR("Irq bottom half with out_cmd = NULL\n");
		return -EINVAL;
	}
	cts_dev = &cts_data->cts_dev;
	if (cts_dev->rtdata.program_mode) {
		TS_LOG_ERR("IRQ triggered in program mode\n");
		return -EINVAL;
	}

	TS_LOG_DEBUG("Irq bottom half\n");

	out_cmd->command = TS_INPUT_ALGO;
	out_cmd->cmd_param.pub_params.algo_param.algo_order =
	    cts_data->tskit_data->algo_id;
	info = &out_cmd->cmd_param.pub_params.algo_param.info;
	memset(info, 0, sizeof(*info));

	touch_info = &cts_data->touch_info;

	ret = cts_get_touchinfo(cts_dev, touch_info);
	if (ret) {
		TS_LOG_ERR("Get touch info failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}
	TS_LOG_DEBUG("palm:%u gesture_id=0x%x num_msg=%u", touch_info->palm,
		touch_info->gesture, touch_info->num_msg);
	if (unlikely(cts_dev->rtdata.suspended)) {
		if (cts_dev->rtdata.gesture_wakeup_enabled) {
			ret = process_gesture_info(info, touch_info->gesture);
			if (ret) {
				TS_LOG_ERR
				    ("Process gesture info failed %d(%s)\n",
				     ret, cts_strerror(ret));
				return ret;
			}
		} else {
			TS_LOG_ERR("IRQ triggered while device suspended "
				   "without gesture wakeup enable\n");
		}
	} else {
		ret = process_touch_msg(info,
					touch_info->msgs, touch_info->num_msg);
		if (ret) {
			TS_LOG_ERR("Process touch msg failed %d(%s)\n",
				   ret, cts_strerror(ret));
			return ret;
		}
	}

	return NO_ERR;
}

int chipone_chip_reset(void)
{
	if (cts_data == NULL) {
		TS_LOG_ERR("Chipe reset with cts_data = NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Chip reset\n");

	return do_hw_reset(cts_data->resume_hw_reset_timing);
}

static void chipone_chip_shutdown(void)
{
	if (cts_data == NULL) {
		TS_LOG_ERR("Chip shutdown with cts_data = NULL\n");
		return;
	}

	TS_LOG_INFO("Chip shutdown\n");
	chipone_power_switch(SWITCH_OFF);
	chipone_power_release();
	pinctrl_select_idle_state();
	return;
}

static int cts_chip_get_info(struct ts_chip_info_param *info)
{
	int len;

	if (info == NULL) {
		TS_LOG_ERR("Chip get info with info = NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("%s: enter\n", __func__);

	memset(info->ic_vendor, 0, sizeof(info->ic_vendor));
	memset(info->mod_vendor, 0, sizeof(info->mod_vendor));
	memset(info->fw_vendor, 0, sizeof(info->fw_vendor));

	if ((!cts_data->tskit_data->ts_platform_data->hide_plain_id) ||
		ts_is_factory()) {
		len = (sizeof(info->ic_vendor) - 1) > sizeof(CHIPONE_CHIP_NAME) ?
			sizeof(CHIPONE_CHIP_NAME) : (sizeof(info->ic_vendor) - 1);
		strncpy(info->ic_vendor, CHIPONE_CHIP_NAME, len);
	} else {
		len = (sizeof(info->ic_vendor) - 1) > sizeof(cts_data->project_id) ?
			sizeof(cts_data->project_id) : (sizeof(info->ic_vendor) - 1);
		strncpy(info->ic_vendor, cts_data->project_id, len);
	}
	len = (sizeof(info->mod_vendor) - 1) > strlen(cts_data->project_id) ?
		strlen(cts_data->project_id) : (sizeof(info->mod_vendor) - 1);
	strncpy(info->mod_vendor, cts_data->project_id, len);
	snprintf(info->fw_vendor, sizeof(info->fw_vendor),
		"%04x", cts_data->cts_dev.fwdata.version);

	if (cts_data->tskit_data->ts_platform_data->hide_plain_id &&
		(!ts_is_factory()))
		/*
		 * The length of ic_vendor is CHIP_INFO_LENGTH * 2
		 * no overwriting occurs
		 */
		snprintf(info->ic_vendor, CHIP_INFO_LENGTH * 2,
			"TP-%s-%s", info->mod_vendor, info->fw_vendor);
	TS_LOG_INFO("mod_vendor:%s\n", info->mod_vendor);
	TS_LOG_INFO("fw_vendor:%s\n", info->fw_vendor);

	return 0;
}

static int get_boot_firmware_filename(void)
{
	struct ts_kit_device_data *tskit_dev_data = cts_data->tskit_data;
	int ret = 0;

	if (cts_data->hide_fw_name) {
		ret = snprintf(cts_data->boot_firmware_filename,
			       sizeof(cts_data->boot_firmware_filename),
			       "ts/%s.bin", cts_data->project_id);
	} else {
		if (tskit_dev_data->ts_platform_data->product_name[0] == '\0' ||
		    tskit_dev_data->chip_name[0] == '\0' ||
		    cts_data->project_id[0] == '\0' ||
		    tskit_dev_data->module_name[0] == '\0') {
			TS_LOG_ERR("Get boot firmware filename with "
				   "product_name/chip_name/project_id/module_name = nul\n");
		}
		ret = snprintf(cts_data->boot_firmware_filename,
			sizeof(cts_data->boot_firmware_filename),
			"ts/%s_%s.img",
			tskit_dev_data->ts_platform_data->product_name,
			cts_data->project_id);
	}

	if (ret >= sizeof(cts_data->boot_firmware_filename)) {
		TS_LOG_ERR("Get boot firmware filename length %d too large\n",
			   ret);
		return -ENOMEM;
	}

	TS_LOG_INFO("Boot firmware filename: '%s'\n",
		    cts_data->boot_firmware_filename);

	return 0;
}

static int chipone_fw_update_boot(char *file_name)
{
	int ret;

	if (cts_data == NULL) {
		TS_LOG_ERR("Firmware update boot with cts_data = NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Firmware update boot\n");

	ret = get_boot_firmware_filename();
	if (ret) {
		TS_LOG_ERR("Get boot firmware filename failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}

	cts_data->cached_firmware = cts_request_newer_firmware_from_fs(&cts_data->cts_dev,
		cts_data->boot_firmware_filename,
		cts_data->cts_dev.fwdata.version);

	if (cts_data->cached_firmware == NULL) {
		TS_LOG_ERR("no need update or request firmware error: ret =%d", ret);
	} else {
		ret = cts_update_firmware(&cts_data->cts_dev,
			cts_data->cached_firmware, true);
		if (ret) {
			TS_LOG_ERR("Update to %s from file failed %d",
				   "flash", ret);
			cts_release_firmware(cts_data->cached_firmware);
		}
	}
	ret = cts_init_fwdata(&cts_data->cts_dev);
	if (ret) {
		TS_LOG_ERR("Device init firmware data failed %d", ret);
		return ret;
	}

	return ret;
}

#define CHIPONE_FW_NAME_SD "ts/touch_screen_firmware.img"
static int chipone_fw_update_sd(void)
{
	int ret;

	if (cts_data == NULL) {
		TS_LOG_ERR("Firmware update manual with cts_data = NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Firmware update manual\n");

	ret = cts_update_firmware_from_file(&cts_data->cts_dev,
					    CHIPONE_FW_NAME_SD, true);
	if (ret) {
		TS_LOG_ERR("Update firmware manual failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}

	ret = cts_init_fwdata(&cts_data->cts_dev);
	if (ret) {
		TS_LOG_ERR("Device init firmware data failed %d", ret);
		return ret;
	}

	return 0;
}

static int alloc_test_data_mem(struct cts_test_param *test_param,
			       int test_data_buf_size)
{
	const char *test_item_str = cts_test_item_str(test_param->test_item);

	if (test_param->test_data_buf) {
		TS_LOG_INFO("Alloc %s data mem with test_data_buf != NULL\n",
			    test_item_str);
		/* Reuse previous memory */
		return 0;
	}

	TS_LOG_INFO("* Alloc %s data mem, size: %d\n",
		    test_item_str, test_data_buf_size);

	test_param->test_data_buf = vzalloc(test_data_buf_size);
	if (test_param->test_data_buf == NULL) {
		TS_LOG_ERR("Alloc %s data mem failed\n", test_item_str);
		test_param->test_data_buf_size = 0;
		return -ENOMEM;
	}
	test_param->test_data_buf_size = test_data_buf_size;

	return 0;
}

static int alloc_test_data_filepath(struct cts_test_param *test_param,
				    const char *test_data_filename)
{
	const char *test_item_str = cts_test_item_str(test_param->test_item);
	char touch_data_filepath[512];
	int ret;

	if (test_param->test_data_filepath) {
		TS_LOG_ERR
		    ("Alloc %s data filepath with test_data_filepath != NULL\n",
		     test_item_str);
		kfree(test_param->test_data_filepath);
	}

	ret = snprintf(touch_data_filepath, sizeof(touch_data_filepath),
		       "%s%s",
		       cts_data->factory_test_data_directory,
		       test_data_filename);
	if (ret >= sizeof(touch_data_filepath)) {
		TS_LOG_ERR("Alloc %s data filepath too long %d\n",
			   test_item_str, ret);
		return -EINVAL;
	}

	TS_LOG_INFO("* Alloc %s data filepath: '%s'\n",
		    test_item_str, touch_data_filepath);

	test_param->test_data_filepath =
	    kstrdup(touch_data_filepath, GFP_KERNEL);
	if (test_param->test_data_filepath == NULL) {
		TS_LOG_ERR("Alloc %s data filepath failed\n", test_item_str);
		return -ENOMEM;
	}

	return 0;
}

int cts_init_testlimits_from_csvfile(void)
{
	char file_path[CTS_CSV_FILE_PATH_LENS] = { 0 };
	char file_name[CTS_CSV_FILE_NAME_LENS] = { 0 };
	int data[RX_MAX_CHANNEL_NUM * TX_MAX_CHANNEL_NUM + RX_MAX_CHANNEL_NUM +
		 TX_MAX_CHANNEL_NUM] = { 0 };
	int columns = 0;
	int rows = 0;
	int ret = 0;
	int i = 0;
	unsigned int chipone_current_test_mode = 0;
	static int chipone_last_test_mode;

	TS_LOG_INFO("%s called", __func__);
	if (cts_data == NULL) {
		TS_LOG_ERR("Rawdata test with cts_data is NULL");
		return -EINVAL;
	}

	columns = cts_data->cts_dev.fwdata.cols;
	rows = cts_data->cts_dev.fwdata.rows;

	if (columns <= 0 || rows <= 0) {
		TS_LOG_INFO("tx num = %d, rx num = %d", columns, rows);
		return -EINVAL;
	}
	if (!strnlen
	    (cts_data->tskit_data->ts_platform_data->product_name,
	     MAX_STR_LEN - 1)
	    || !strnlen(cts_data->tskit_data->chip_name, MAX_STR_LEN - 1)
	    || !strnlen(cts_data->project_id, CTS_INFO_PROJECT_ID_LEN)) {
		TS_LOG_ERR("csv file name is not detected");
		return -EINVAL;
	}

	snprintf(file_name, sizeof(file_name), "%s_%s_limits.csv",
		 cts_data->tskit_data->ts_platform_data->product_name,
		 cts_data->project_id);

	TS_LOG_INFO("%s called", __func__);
	if (CTS_CSV_FILE_IN_PRODUCT == cts_data->csvfile_use_product_system) {
		snprintf(file_path, sizeof(file_path), "%s%s",
			 CTS_CSV_PATH_PERF_PRODUCT, file_name);
	} else if (CTS_CSV_FILE_IN_ODM == cts_data->csvfile_use_product_system) {
		snprintf(file_path, sizeof(file_path), "%s%s",
			 CTS_CSV_PATH_PERF_ODM, file_name);
	} else {
		TS_LOG_ERR
		    ("csvfile path is not supported, csvfile_use_product_system = %d",
		     cts_data->csvfile_use_product_system);
		return -EINVAL;
	}
	TS_LOG_INFO("threshold file name:%s", file_path);

	chipone_current_test_mode = CAP_TEST_NORMAL;

	TS_LOG_INFO("%s cts_last_test_mode=%d cts_current_test_mode=%d\n",
		__func__, chipone_last_test_mode, chipone_current_test_mode);
	if (chipone_last_test_mode == chipone_current_test_mode)
		return 0;
	else
		chipone_last_test_mode = chipone_current_test_mode;

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_INVALID_NODE_NUM, data,
				 1, 1);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read mutual raw data limit max from csvfile:%d",
		     ret);
		return ret;
	} else {
			cts_data->invalid_node_num = (u8) (data[0]);
			TS_LOG_INFO("invalid_node_num:%u", data[0]);
	}
	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_INVALID_NODES, data,
				 2, cts_data->invalid_node_num);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read mutual raw data limit max from csvfile:%d",
		     ret);
		return ret;
	} else {
		for (i = 0; i < cts_data->invalid_node_num; i++) {
			cts_data->invalid_nodes[i] = MAKE_INVALID_NODE(data[i], data[i + cts_data->invalid_node_num]);
			TS_LOG_INFO("invalid_nodes:%d,%d", data[i],data[i + cts_data->invalid_node_num]);
		}
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_RESET_TEST_ENABLE, data, 1,
				 1);
	if (ret) {
		TS_LOG_ERR("Failed to read reset_test_enable from csvfile:%d",
			   ret);
		return ret;
	} else {
		cts_data->test_reset_pin = (u8) (data[0]);
		TS_LOG_INFO("reset_test_enable:%u", data[0]);
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_INT_TEST_ENABLE, data, 1,
				 1);
	if (ret) {
		TS_LOG_ERR("Failed to read int_test_enable from csvfile:%d",
			   ret);
		return ret;
	} else {
		cts_data->test_int_pin = (u8) (data[0]);
		TS_LOG_INFO("int_test_enable:%u", data[0]);
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_RAW_TEST_ENABLE, data, 1,
				 1);
	if (ret) {
		TS_LOG_ERR("Failed to read raw_test_enable from csvfile:%d",
			   ret);
		return ret;
	} else {
		cts_data->test_rawdata = (u8) (data[0]);
		TS_LOG_INFO("raw_test_enable:%u", data[0]);
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_RAW_TEST_FRAME, data, 1, 1);
	if (ret) {
		TS_LOG_ERR("Failed to read raw_test_frame from csvfile:%d",
			   ret);
		return ret;
	} else {
		cts_data->rawdata_test_priv_param.frames = (u8) (data[0]);
		TS_LOG_INFO("rawdata_test_priv_param.frames:%u", data[0]);
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_MUTUAL_RAW_LIMIT_MAX, data,
				 rows, columns);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read mutual raw data limit max from csvfile:%d",
		     ret);
		return ret;
	} else {
		for (i = 0; i < rows * columns; i++) {
			cts_data->rawdata_test_param.max_limits[i] =
			    (u16) (data[i]);
		}
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_MUTUAL_RAW_LIMIT_MIN, data,
				 rows, columns);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read mutual raw data limit min from csvfile:%d",
		     ret);
		return ret;
	} else {
		for (i = 0; i < rows * columns; i++) {
			cts_data->rawdata_test_param.min_limits[i] =
			    (u16) (data[i]);
		}
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_SELF_RAW_LIMIT_MAX, data, 1,
				 rows + columns);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read self raw data limit max from csvfile:%d",
		     ret);
		return ret;
	} else {
		cts_data->test_rawdata = 1;
		for (i = 0; i < rows + columns; i++) {
			cts_data->rawdata_test_param.max_limits[rows * columns +
								i] =
			    (u16) (data[i]);
		}
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_SELF_RAW_LIMIT_MIN, data, 1,
				 rows + columns);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read self raw data limit min from csvfile:%d",
		     ret);
		return ret;
	} else {
		for (i = 0; i < rows + columns; i++) {
			cts_data->rawdata_test_param.min_limits[rows * columns +
								i] =
			    (u16) (data[i]);
		}
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_OPEN_TEST_ENABLE, data, 1,
				 1);
	if (ret) {
		TS_LOG_ERR("Failed to read open drv from csvfile:%d", ret);
		return ret;
	} else {
		cts_data->test_open = (u8) (data[0]);
		TS_LOG_INFO("test_open :%u", data[0]);
	}
	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_OPEN_LIMIT_MAX, data,
				 rows, columns);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read open data limit max from csvfile:%d",
		     ret);
		return ret;
	} else {
		for (i = 0; i < rows * columns; i++) {
			cts_data->open_test_param.max_limits[i] =
			    (u16) (data[i]);
		}
	}
	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_OPEN_LIMIT_MIN, data,
				 rows, columns);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read open data limit min from csvfile:%d",
		     ret);
		return ret;
	} else {
		for (i = 0; i < rows * columns; i++) {
			cts_data->open_test_param.min_limits[i] =
			    (u16) (data[i]);
		}
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_DEVIATION_TEST_ENABLE, data,
				 1, 1);
	if (ret) {
		TS_LOG_ERR("Failed to read deviation enable from csvfile:%d",
			   ret);
		return ret;
	} else {
		cts_data->test_deviation = (u8) (data[0]);
		TS_LOG_INFO("test_deviation drv :%u", data[0]);
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_DEVIATION_TEST_FRAME, data,
				 1, 1);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read deviation_test_frame from csvfile:%d",
		     ret);
		return ret;
	} else {
		cts_data->deviation_test_priv_param.frames = (u8) (data[0]);
		TS_LOG_INFO("noise_test_priv_param.frames:%u", data[0]);
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_MUTUAL_DEVIATION_LIMIT,
				 data, rows, columns);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read mutual deviation data limit max from csvfile:%d",
		     ret);
		return ret;
	} else {
		for (i = 0; i < rows * columns; i++) {
			cts_data->deviation_test_param.max_limits[i] =
			    (u16) (data[i]);
		}
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_NOISE_TEST_ENABLE, data, 1,
				 1);
	if (ret) {
		TS_LOG_ERR("Failed to read noise_test_enable from csvfile:%d",
			   ret);
		return ret;
	} else {
		cts_data->test_noise = (u8) (data[0]);
		TS_LOG_INFO("noise_test_enable:%u", data[0]);
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_NOISE_TEST_FRAME, data, 1,
				 1);
	if (ret) {
		TS_LOG_ERR("Failed to read noise_test_frame from csvfile:%d",
			   ret);
		return ret;
	} else {
		cts_data->noise_test_priv_param.frames = (u8) (data[0]);
		TS_LOG_INFO("noise_test_priv_param.frames:%u", data[0]);
	}
	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_MUTUAL_NOISE_LIMIT, data,
				 rows, columns);
	if (ret) {
		TS_LOG_ERR("Failed to read noise limit from csvfile:%d", ret);
		return ret;
	} else {
		for (i = 0; i < rows * columns; i++) {
			cts_data->noise_test_param.max_limits[i] =
			    (u16) (data[i]);
		}
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_SELF_NOISE_LIMIT, data, 1,
				 rows + columns);
	if (ret) {
		TS_LOG_ERR("Failed to read self noise limit from csvfile:%d",
			   ret);
		return ret;
	} else {
		for (i = 0; i < rows + columns; i++) {
			cts_data->noise_test_param.max_limits[rows * columns + i] =
			    (u16)(data[i]);
		}
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_SHORT_TEST_ENABLE, data, 1,
				 1);
	if (ret) {
		TS_LOG_ERR("Failed to read short enable from csvfile:%d", ret);
		return ret;
	} else {
		cts_data->test_short = (u8) (data[0]);
		TS_LOG_INFO("short enable:%u", data[0]);
	}

	ret =
	    ts_kit_parse_csvfile(file_path, CTS_CSV_SHORT_THRESHOLD, data, 1,
				 rows + columns);
	if (ret) {
		TS_LOG_ERR
		    ("Failed to read shortcircut threshold from csvfile:%d",
		     ret);
		return ret;
	} else {
		for (i = 0; i < rows + columns; i++) {
			cts_data->short_test_param.min_limits[i] = (u16)(data[i]);
			TS_LOG_INFO("short:%u\n", data[i]);
		}
	}

	TS_LOG_INFO("get threshold from %s ok", file_path);
	return 0;
}

static int init_factory_test_param(void)
{
	struct cts_test_param *test_param;
	u8 rows, cols;
	int nodes;
	int ret = 0;

	TS_LOG_INFO("Init factory test param\n");

	rows = cts_data->cts_dev.fwdata.rows;
	cols = cts_data->cts_dev.fwdata.cols;
	nodes = rows * cols + rows + cols;

	if (cts_data->test_capacitance_via_csvfile) {
		/* get test limits from csvfile */
		ret = cts_init_testlimits_from_csvfile();
		if (ret < 0) {
			TS_LOG_ERR("Failed to init testlimits from csvfile:%d",
				   ret);
			return ret;
		}
	} else {
		/* get test limits from dt */
		//parse_dts_project(const struct device_node *proj_of_node);
		TS_LOG_INFO("need get init testlimits from dt:%d", ret);
	}

	if (cts_data->test_reset_pin) {
		test_param = &cts_data->reset_pin_test_param;
		test_param->test_item = CTS_TEST_RESET_PIN;
		test_param->test_result = &cts_data->reset_pin_test_result;
	}
	if (cts_data->test_int_pin) {
		test_param = &cts_data->int_pin_test_param;
		test_param->test_item = CTS_TEST_INT_PIN;
		test_param->test_result = &cts_data->int_pin_test_result;
	}
	if (cts_data->test_rawdata) {
		test_param = &cts_data->rawdata_test_param;
		test_param->test_item = CTS_TEST_RAWDATA;
		test_param->flags =
		    CTS_TEST_FLAG_VALIDATE_DATA |
		    CTS_TEST_FLAG_VALIDATE_MIN |
		    CTS_TEST_FLAG_VALIDATE_MAX |
		    CTS_TEST_FLAG_VALIDATE_PER_NODE |
		    CTS_TEST_FLAG_STOP_TEST_IF_VALIDATE_FAILED;

		test_param->min = test_param->min_limits;
		test_param->max = test_param->max_limits;
		test_param->self_min = &test_param->min_limits[rows*cols];
		test_param->self_max = &test_param->max_limits[rows*cols];
		test_param->num_invalid_node =  cts_data->invalid_node_num;
		test_param->invalid_nodes = cts_data->invalid_nodes;
		if (cts_data->dump_factory_test_data_to_console) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_CONSOLE;
		}
		if (cts_data->dump_factory_test_data_to_buffer) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_USERSPACE;
			ret = alloc_test_data_mem(test_param, 2 * nodes *
						  cts_data->rawdata_test_priv_param.frames);
			if (ret)
				return ret;
			test_param->test_data_wr_size =
			    &cts_data->rawdata_test_data_size;
		}
		if (cts_data->dump_factory_test_data_to_file) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE;
			ret =
			    alloc_test_data_filepath(test_param,
						     "rawdata-test-data.txt");
			if (ret)
				return ret;
		}

		test_param->test_result = &cts_data->rawdata_test_result;

		test_param->priv_param = &cts_data->rawdata_test_priv_param;
		test_param->priv_param_size =
		    sizeof(cts_data->rawdata_test_priv_param);
	}

	if (cts_data->test_deviation) {
		test_param = &cts_data->deviation_test_param;
		test_param->test_item = CTS_TEST_DEVIATION;
		test_param->flags =
		    CTS_TEST_FLAG_VALIDATE_DATA |
		    CTS_TEST_FLAG_VALIDATE_MAX |
		    CTS_TEST_FLAG_VALIDATE_PER_NODE |
		    CTS_TEST_FLAG_STOP_TEST_IF_VALIDATE_FAILED;

		test_param->max = test_param->max_limits;
		test_param->self_max = &test_param->max_limits[rows*cols];
		test_param->num_invalid_node =  cts_data->invalid_node_num;
		test_param->invalid_nodes = cts_data->invalid_nodes;
		if (cts_data->dump_factory_test_data_to_console) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_CONSOLE;
		}
		if (cts_data->dump_factory_test_data_to_buffer) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_USERSPACE;
			ret = alloc_test_data_mem(test_param, 2 * nodes *
						  cts_data->deviation_test_priv_param.frames);
			if (ret)
				return ret;
			test_param->test_data_wr_size =
			    &cts_data->deviation_test_data_size;
		}
		if (cts_data->dump_factory_test_data_to_file) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE;
			ret =
			    alloc_test_data_filepath(test_param,
						     "deviation-test-data.txt");
			if (ret)
				return ret;
		}

		test_param->test_result = &cts_data->deviation_test_result;

		test_param->priv_param = &cts_data->deviation_test_priv_param;
		test_param->priv_param_size =
		    sizeof(cts_data->deviation_test_priv_param);
	}

	if (cts_data->test_noise) {
		test_param = &cts_data->noise_test_param;
		test_param->test_item = CTS_TEST_NOISE;
		test_param->flags =
		    CTS_TEST_FLAG_VALIDATE_DATA |
		    CTS_TEST_FLAG_VALIDATE_MAX |
		    CTS_TEST_FLAG_VALIDATE_PER_NODE |
		    CTS_TEST_FLAG_STOP_TEST_IF_VALIDATE_FAILED;

		test_param->max = test_param->max_limits;
		test_param->self_max = &(test_param->max_limits[rows*cols]);
		test_param->num_invalid_node =  cts_data->invalid_node_num;
		test_param->invalid_nodes = cts_data->invalid_nodes;
		if (cts_data->dump_factory_test_data_to_console) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_CONSOLE;
		}
		if (cts_data->dump_factory_test_data_to_buffer) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_USERSPACE;
			ret = alloc_test_data_mem(test_param, 2 * nodes *
						  (cts_data->
						  noise_test_priv_param.frames+3));
			if (ret)
				return ret;
			test_param->test_data_wr_size =
			    &cts_data->noise_test_data_size;
		}
		if (cts_data->dump_factory_test_data_to_file) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE;
			ret =
			    alloc_test_data_filepath(test_param,
						     "noise-test-data.txt");
			if (ret)
				return ret;
		}

		test_param->test_result = &cts_data->noise_test_result;

		test_param->priv_param = &cts_data->noise_test_priv_param;
		test_param->priv_param_size =
		    sizeof(cts_data->noise_test_priv_param);
	}

	if (cts_data->test_open) {
		test_param = &cts_data->open_test_param;
		test_param->test_item = CTS_TEST_OPEN;
		test_param->flags =
		    CTS_TEST_FLAG_VALIDATE_DATA |
		    CTS_TEST_FLAG_VALIDATE_MAX |
		    CTS_TEST_FLAG_VALIDATE_PER_NODE |
		    CTS_TEST_FLAG_STOP_TEST_IF_VALIDATE_FAILED;

		test_param->min = test_param->min_limits;
		test_param->max = test_param->max_limits;
		test_param->num_invalid_node =  cts_data->invalid_node_num;
		test_param->invalid_nodes = cts_data->invalid_nodes;
		if (cts_data->dump_factory_test_data_to_console) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_CONSOLE;
		}
		if (cts_data->dump_factory_test_data_to_buffer) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_USERSPACE;
			ret = alloc_test_data_mem(test_param, 2 * nodes);
			if (ret)
				return ret;
			test_param->test_data_wr_size =
			    &cts_data->open_test_data_size;
		}
		if (cts_data->dump_factory_test_data_to_file) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE;
			ret =
			    alloc_test_data_filepath(test_param,
						     "open-test-data.txt");
			if (ret)
				return ret;
		}

		test_param->test_result = &cts_data->open_test_result;
	}

	if (cts_data->test_short) {
		test_param = &cts_data->short_test_param;
		test_param->test_item = CTS_TEST_SHORT;
		test_param->flags =
		    CTS_TEST_FLAG_VALIDATE_DATA |
		    CTS_TEST_FLAG_VALIDATE_MIN |
		    CTS_TEST_FLAG_VALIDATE_PER_NODE |
		    CTS_TEST_FLAG_STOP_TEST_IF_VALIDATE_FAILED;

		test_param->min = test_param->min_limits;

		if (cts_data->dump_factory_test_data_to_console) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_CONSOLE;
		}
		if (cts_data->dump_factory_test_data_to_buffer) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_USERSPACE;
			ret = alloc_test_data_mem(test_param, 2 * nodes * 7);
			if (ret)
				return ret;
			test_param->test_data_wr_size =
			    &cts_data->short_test_data_size;
		}
		if (cts_data->dump_factory_test_data_to_file) {
			test_param->flags |=
			    CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE;
			ret =
			    alloc_test_data_filepath(test_param,
						     "short-test-data.txt");
			if (ret)
				return ret;
		}

		test_param->test_result = &cts_data->short_test_result;
	}

	return 0;
}

static void free_test_param_mem(struct cts_test_param *test_param)
{
	if (test_param->test_data_buf) {
		TS_LOG_INFO("* Free %s data mem\n",
			    cts_test_item_str(test_param->test_item));
		kfree(test_param->test_data_buf);
		test_param->test_data_buf = NULL;
	}
	if (test_param->test_data_filepath) {
		TS_LOG_INFO("* Free %s data filepath\n",
			    cts_test_item_str(test_param->test_item));
		kfree(test_param->test_data_filepath);
		test_param->test_data_filepath = NULL;
	}
}

static void free_factory_test_mem(void)
{
	TS_LOG_INFO("Free factory test data mem\n");
	if (cts_data->test_rawdata)
		free_test_param_mem(&cts_data->rawdata_test_param);
	if (cts_data->test_deviation)
		free_test_param_mem(&cts_data->deviation_test_param);
	if (cts_data->test_noise)
		free_test_param_mem(&cts_data->noise_test_param);
	if(cts_data->test_open)
		free_test_param_mem(&cts_data->open_test_param);
	if(cts_data->test_short)
		free_test_param_mem(&cts_data->short_test_param);

}

static void print_test_result_to_console(const char *test_desc, int test_result)
{
	if (test_result > 0) {
		TS_LOG_INFO("%-10s test: FAIL %d nodes\n", test_desc,
			    test_result);
	} else if (test_result < 0) {
		TS_LOG_INFO("%-10s test: FAIL %d(%s)\n", test_desc,
			    test_result, cts_strerror(test_result));
	} else {
		TS_LOG_INFO("%-10s test: PASS\n", test_desc);
	}
}

static void print_factory_test_result_to_console(void)
{
	if (cts_data->test_reset_pin) {
		print_test_result_to_console("Reset-Pin",
					     cts_data->reset_pin_test_result);
	}
	if (cts_data->test_int_pin) {
		print_test_result_to_console("Int-Pin",
					     cts_data->int_pin_test_result);
	}
	if (cts_data->test_rawdata) {
		print_test_result_to_console("Rawdata",
					     cts_data->rawdata_test_result);
		print_test_result_to_console("self Rawdata",
					     cts_data->selfrawdata_test_result);
	}
	if (cts_data->test_deviation)
		print_test_result_to_console("Deviation",
			cts_data->deviation_test_result);
	if (cts_data->test_noise) {
		print_test_result_to_console("Noise",
					     cts_data->noise_test_result);
		print_test_result_to_console("self Noise",
					     cts_data->selfnoise_test_result);
	}
	if (cts_data->test_open) {
		print_test_result_to_console("Open",
					     cts_data->open_test_result);
	}
	if (cts_data->test_short) {
		print_test_result_to_console("Short",
					     cts_data->short_test_result);
	}
}

static void do_factory_test(void)
{
	ktime_t start_time, end_time, delta_time;
	int ret = NO_ERR;

	TS_LOG_INFO("Factory test start...\n");

	start_time = ktime_get();

	if (cts_data->test_reset_pin) {
		cts_data->reset_pin_test_result =
		    cts_test_reset_pin(&cts_data->cts_dev,
				       &cts_data->reset_pin_test_param);
	}

	if (cts_data->test_int_pin) {
		cts_data->int_pin_test_result =
		    cts_test_int_pin(&cts_data->cts_dev,
				     &cts_data->int_pin_test_param);
	}
	if (cts_data->test_rawdata) {
		cts_data->rawdata_test_result = 0;
		cts_data->selfrawdata_test_result = 0;
		ret = cts_test_rawdata(&cts_data->cts_dev,
				     &cts_data->rawdata_test_param);
		if (ret){
			cts_data->rawdata_test_result = cts_data->rawdata_test_param.mul_failed_nodes;
			cts_data->selfrawdata_test_result = cts_data->rawdata_test_param.self_failed_nodes;
		}
	}
	if (cts_data->test_deviation) {
		cts_data->deviation_test_result = 0;
		ret = cts_test_deviation(&cts_data->cts_dev,
				       &cts_data->deviation_test_param);
		if (ret)
			cts_data->deviation_test_result = cts_data->deviation_test_param.mul_failed_nodes;
	}
	if (cts_data->test_noise) {
		cts_data->noise_test_result = 0;
		cts_data->selfnoise_test_result = 0;
		ret = cts_test_noise(&cts_data->cts_dev,
				   &cts_data->noise_test_param);
		if (ret){
			cts_data->noise_test_result = cts_data->noise_test_param.mul_failed_nodes;
			cts_data->selfnoise_test_result = cts_data->noise_test_param.self_failed_nodes;
		}
	}

	if (cts_data->test_open) {
		cts_data->open_test_result =
		    cts_test_open(&cts_data->cts_dev,
				  &cts_data->open_test_param);
	}
	if (cts_data->test_short) {
		cts_data->short_test_result =
		    cts_test_short(&cts_data->cts_dev,
				   &cts_data->short_test_param);
	}

	end_time = ktime_get();
	delta_time = ktime_sub(end_time, start_time);
	TS_LOG_INFO("Factory test complete, ELAPSED TIME: %lldms\n",
		    ktime_to_ms(delta_time));
}

static int chipone_alloc_newnode(struct ts_rawdata_newnodeinfo ** pts_node,int bufsize)
{
	int rc = NO_ERR;
	*pts_node = (struct ts_rawdata_newnodeinfo *)kzalloc(sizeof(struct ts_rawdata_newnodeinfo), GFP_KERNEL);
	if (!(*pts_node)) {
		TS_LOG_ERR("malloc failed \n");
		return -EINVAL;
	}
	if (0 == bufsize)
		return rc;
	(*pts_node)->values = kzalloc((bufsize)*sizeof(int), GFP_KERNEL);
	if (!(*pts_node)->values) {
		TS_LOG_ERR("malloc failed  for values \n");
		kfree(*pts_node);
		*pts_node = NULL;
		return -EINVAL;
	}
	return rc;
}

static void cts_put_test_result_newformat(struct ts_rawdata_info_new *info)
{
	int i = 0;
	int ret = NO_ERR;
	u16 *data = NULL;
	u32 *short_data = NULL;
	int single_frame_size = 0;
	struct ts_rawdata_newnodeinfo *pts_node = NULL;

	TS_LOG_INFO("%s :\n", __func__);
	if ((cts_data == NULL) || (info == NULL) || (!cts_data->cts_dev.fwdata.rows) ||
		(!cts_data->cts_dev.fwdata.cols)) {
		TS_LOG_ERR("put_test_result_newformat NULL\n");
		return;
	}

	info->tx = cts_data->cts_dev.fwdata.rows;
	info->rx = cts_data->cts_dev.fwdata.cols;
	single_frame_size = info->tx * info->rx + info->tx + info->rx;

	if (cts_data->test_reset_pin) {
		/* 1p 1f */
		ret = chipone_alloc_newnode(&pts_node, 0);
		if(ret < 0) {
			TS_LOG_ERR("multual raw malloc failed node\n");
			return ;
		}
		pts_node->size = 0;
		pts_node->testresult = cts_data->reset_pin_test_result ? 'F' : 'P';
		pts_node->typeindex = 1;
		strncpy(pts_node->test_name, "reset test",
				sizeof(pts_node->test_name) - 1);
		if (pts_node->testresult == CAP_TEST_FAIL_CHAR)
			strncpy(pts_node->tptestfailedreason, cts_data->failedreason,
				TS_RAWDATA_FAILED_REASON_LEN - 1);
		list_add_tail(&pts_node->node, &info->rawdata_head);
		info->listnodenum++;
	}
	if (cts_data->test_int_pin) {
		/* 2p 2f not test */
		ret = chipone_alloc_newnode(&pts_node, 0);
		if(ret < 0) {
			TS_LOG_ERR("int malloc failed node\n");
			return ;
		}
		pts_node->size = 0;
		pts_node->testresult = cts_data->int_pin_test_result ? 'F' : 'P';
		pts_node->typeindex =  2;
		strncpy(pts_node->test_name, "int test",
				sizeof(pts_node->test_name) - 1);
		if (pts_node->testresult == CAP_TEST_FAIL_CHAR)
			strncpy(pts_node->tptestfailedreason, cts_data->failedreason,
				TS_RAWDATA_FAILED_REASON_LEN - 1);
		list_add_tail(&pts_node->node, &info->rawdata_head);
		info->listnodenum++;
	}

	if (cts_data->test_rawdata) {
		/* mul raw, 3p 3f */
		ret = chipone_alloc_newnode(&pts_node, info->tx * info->rx);
		if(ret < 0) {
			TS_LOG_ERR("multual raw malloc failed node\n");
			return ;
		}
		if (cts_data->rawdata_test_data_size) {
			data = (s16 *)cts_data->rawdata_test_param.test_data_buf;
			for (i = 0; i < info->tx * info->rx; i++) /*only 1 frame*/
				pts_node->values[i] = data[i];
		}
		pts_node->size = info->tx * info->rx;
		pts_node->testresult = cts_data->rawdata_test_result ? 'F' : 'P';
		pts_node->typeindex =  3;
		strncpy(pts_node->test_name, "Rawdata test",
				sizeof(pts_node->test_name) - 1);
		if (pts_node->testresult == CAP_TEST_FAIL_CHAR)
			strncpy(pts_node->tptestfailedreason, cts_data->failedreason,
				TS_RAWDATA_FAILED_REASON_LEN - 1);
		list_add_tail(&pts_node->node, &info->rawdata_head);
		info->listnodenum++;

		/* self cap 4p 4f */
		ret = chipone_alloc_newnode(&pts_node, info->tx + info->rx);
		if(ret < 0) {
			TS_LOG_ERR("self raw malloc failed node\n");
			return ;
		}
		if (cts_data->rawdata_test_data_size) {
			data = (s16 *)cts_data->rawdata_test_param.test_data_buf;
			for (i = 0; i < info->tx + info->rx; i++)  /*only 1 frame*/
				pts_node->values[i] = data[info->tx * info->tx + i];
		}
		pts_node->size = info->tx + info->rx;
		pts_node->testresult = cts_data->selfrawdata_test_result ? 'F' : 'P';
		pts_node->typeindex = 4;
		strncpy(pts_node->test_name, "self Rawdata test",
				sizeof(pts_node->test_name) - 1);
		if (pts_node->testresult == CAP_TEST_FAIL_CHAR)
			strncpy(pts_node->tptestfailedreason, cts_data->failedreason,
				TS_RAWDATA_FAILED_REASON_LEN - 1);
		list_add_tail(&pts_node->node, &info->rawdata_head);
		info->listnodenum++;
	}
	if (cts_data->test_deviation) {
		/* 5p 5f */
		ret = chipone_alloc_newnode(&pts_node, info->tx * info->rx);
		if(ret < 0) {
			TS_LOG_ERR(" deviation  malloc failed node\n");
			return ;
		}
		if (cts_data->deviation_test_data_size) {
			data = (s16 *)cts_data->deviation_test_param.test_data_buf;
			for (i = 0; i < info->tx * info->rx; i++)  /*only 1 frame*/
				pts_node->values[i] = data[i];
		}
		pts_node->size = info->tx * info->rx;
		pts_node->testresult = cts_data->deviation_test_result ? 'F' : 'P';
		pts_node->typeindex = 5;
		strncpy(pts_node->test_name, "deviation test",
				sizeof(pts_node->test_name) - 1);
		if (pts_node->testresult == CAP_TEST_FAIL_CHAR)
			strncpy(pts_node->tptestfailedreason, cts_data->failedreason,
				TS_RAWDATA_FAILED_REASON_LEN - 1);
		list_add_tail(&pts_node->node, &info->rawdata_head);
		info->listnodenum++;
	}
	if (cts_data->test_noise) {
		/* mul noise, 6p 6f */
		ret = chipone_alloc_newnode(&pts_node, info->tx * info->rx);
		if(ret < 0) {
			TS_LOG_ERR("noise malloc failed node\n");
			return ;
		}
		if (cts_data->noise_test_data_size) {
			data = (s16 *)cts_data->noise_test_param.test_data_buf;
			for (i = 0; i < info->tx * info->rx; i++) /*only 1 frame*/
				pts_node->values[i] = data[cts_data->noise_test_priv_param.frames * single_frame_size + i];
		}
		pts_node->size = info->tx * info->rx;
		pts_node->testresult = cts_data->noise_test_result ? 'F' : 'P';
		pts_node->typeindex = 6;
		strncpy(pts_node->test_name, "Noise test",
				sizeof(pts_node->test_name) - 1);
		if (pts_node->testresult == CAP_TEST_FAIL_CHAR)
			strncpy(pts_node->tptestfailedreason, cts_data->failedreason,
				TS_RAWDATA_FAILED_REASON_LEN - 1);
		list_add_tail(&pts_node->node, &info->rawdata_head);
		info->listnodenum++;

		/* self noise, 7p 7f */
		ret = chipone_alloc_newnode(&pts_node, info->tx + info->rx);
		if(ret < 0) {
			TS_LOG_ERR("self noise malloc failed node\n");
			return ;
		}
		if (cts_data->noise_test_data_size) {
			data = (s16 *)cts_data->noise_test_param.test_data_buf;
			for (i = 0; i < info->tx + info->rx; i++) /*only 1 frame*/
				pts_node->values[i] = data[cts_data->noise_test_priv_param.frames * single_frame_size + info->tx + info->rx + i];
		}
		pts_node->size = info->tx + info->rx;
		pts_node->testresult = cts_data->selfnoise_test_result ? 'F' : 'P';
		pts_node->typeindex =  7;
		strncpy(pts_node->test_name, "self Noise test",
				sizeof(pts_node->test_name) - 1);
		if (pts_node->testresult == CAP_TEST_FAIL_CHAR)
			strncpy(pts_node->tptestfailedreason, cts_data->failedreason,
				TS_RAWDATA_FAILED_REASON_LEN - 1);
		list_add_tail(&pts_node->node, &info->rawdata_head);
		info->listnodenum++;
	}
	if (cts_data->test_open) {
		/* open, 8p 8f */
		ret = chipone_alloc_newnode(&pts_node, info->tx * info->rx);
		if(ret < 0) {
			TS_LOG_ERR("open malloc failed node\n");
			return ;
		}
		if (cts_data->open_test_data_size) {
			data = (s16 *)cts_data->open_test_param.test_data_buf;
			for (i = 0; i < info->tx * info->rx; i++) /*only 1 frame*/
				pts_node->values[i] = data[i];
		}
		pts_node->size = info->tx * info->rx;
		pts_node->testresult = cts_data->open_test_result ? 'F' : 'P';
		pts_node->typeindex =  8;
		strncpy(pts_node->test_name, "open test",
				sizeof(pts_node->test_name) - 1);
		if (pts_node->testresult == CAP_TEST_FAIL_CHAR)
			strncpy(pts_node->tptestfailedreason, cts_data->failedreason,
				TS_RAWDATA_FAILED_REASON_LEN - 1);
		list_add_tail(&pts_node->node, &info->rawdata_head);
		info->listnodenum++;
	}
	if (cts_data->test_short) {
		/* short 9p 9f */
		ret = chipone_alloc_newnode(&pts_node, info->tx + info->rx);
		if(ret < 0) {
			TS_LOG_ERR("short malloc failed node\n");
			return ;
		}
		if (cts_data->short_test_data_size) {
			short_data = (u32 *)cts_data->noise_test_param.test_data_buf;
			for (i = 0; i < info->tx + info->rx; i++) /*only 1 frame*/
				pts_node->values[i] = short_data[i];
		}
		pts_node->size = info->tx + info->rx;
		pts_node->testresult = cts_data->short_test_result ? 'F' : 'P';
		pts_node->typeindex = 9;
		strncpy(pts_node->test_name, "short test",
				sizeof(pts_node->test_name) - 1);
		if (pts_node->testresult == CAP_TEST_FAIL_CHAR)
			strncpy(pts_node->tptestfailedreason, cts_data->failedreason,
				TS_RAWDATA_FAILED_REASON_LEN - 1);
		list_add_tail(&pts_node->node, &info->rawdata_head);
		info->listnodenum++;
	}
	return ;
}

static int chipone_i2c_communication_test(
	struct ts_rawdata_info_new *info)
{
	if (!cts_data->chip_detect_ok) {
		snprintf(info->i2cinfo, TS_RAWDATA_RESULT_CODE_LEN,
			"%d%c", RAW_DATA_TYPE_IC, CAP_TEST_FAIL_CHAR);
		strncat(info->i2cerrinfo, "software reason ",
			sizeof(info->i2cerrinfo));
		return -ENODEV;
	}
	snprintf(info->i2cinfo, TS_RAWDATA_RESULT_CODE_LEN,
		"%d%c", RAW_DATA_TYPE_IC, CAP_TEST_PASS_CHAR);
	return 0;
}

static void chipone_put_device_info(struct ts_rawdata_info_new *info)
{
	char buf_fw_ver[CHIP_INFO_LENGTH] = {0};
	unsigned long len;

	strncpy(info->deviceinfo, cts_data->project_id,
		strlen(cts_data->project_id));

	if (sizeof(info->deviceinfo) > strlen(info->deviceinfo))
		len = sizeof(info->deviceinfo) - strlen(info->deviceinfo) - 1;
	else
		len = 0;
	strncat(info->deviceinfo, "-", len);

	snprintf(buf_fw_ver, CHIP_INFO_LENGTH,
		 "%04x", cts_data->cts_dev.fwdata.version);
	TS_LOG_INFO("buf_fw_ver = %s", buf_fw_ver);
	if (sizeof(info->deviceinfo) > strlen(info->deviceinfo))
		len = sizeof(info->deviceinfo) - strlen(info->deviceinfo) - 1;
	else
		len = 0;
	strncat(info->deviceinfo, buf_fw_ver, len);

	if (sizeof(info->deviceinfo) > strlen(info->deviceinfo))
		len = sizeof(info->deviceinfo) - strlen(info->deviceinfo) - 1;
	else
		len = 0;
	strncat(info->deviceinfo, ";", len);
}

static int chipone_get_capacitance_test_type(struct ts_test_type_info *info)
{
	int ret = 0;

	if (!info || (cts_data == NULL)) {
		TS_LOG_ERR("%s:info is Null\n", __func__);
		return -ENOMEM;
	}

	switch (info->op_action) {
	case TS_ACTION_READ:
		memcpy(info->tp_test_type,
			cts_data->tskit_data->tp_test_type,
			TS_CAP_TEST_TYPE_LEN);
		TS_LOG_INFO("%s:test_type=%s\n", __func__,
			info->tp_test_type);
		break;
	case TS_ACTION_WRITE:
		break;
	default:
		TS_LOG_ERR("%s:invalid op action:%d\n",
			__func__, info->op_action);
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int cts_chip_get_rawdata(struct ts_rawdata_info *info,
			       struct ts_cmd_node *out_cmd)
{
	int ret = NO_ERR;
	const char *fail_reason_str = NULL;
	struct ts_rawdata_info_new *new_info =
		(struct ts_rawdata_info_new *)info;

	if ((cts_data == NULL) || (!cts_data->cts_dev.fwdata.rows) ||
		(!cts_data->cts_dev.fwdata.cols) ||
		(cts_data->cts_dev.fwdata.rows > RX_MAX_CHANNEL_NUM) ||
		(cts_data->cts_dev.fwdata.cols > TX_MAX_CHANNEL_NUM)) {
		TS_LOG_ERR("Get rawdata with NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Get rawdata\n");

	ret = chipone_i2c_communication_test(new_info);
	if (ret < 0) {
		TS_LOG_ERR("%s i2c_communication failed\n", __func__);
		return  -ENODEV;
	}

	ret = init_factory_test_param();
	if (ret) {
		TS_LOG_ERR("Init factory test param failed %d(%s)\n",
			   ret, cts_strerror(ret));
		fail_reason_str = "init test param fail:-";
		goto output_result_str;
	}
	chipone_put_device_info(new_info);
	do_factory_test();
	print_factory_test_result_to_console();
	if (cts_data->tskit_data->rawdata_newformatflag == TS_RAWDATA_NEWFORMAT) {
		cts_put_test_result_newformat(new_info);
		return ret;
	}
	 info->buff[0] = cts_data->cts_dev.fwdata.rows;
	 info->buff[1] = cts_data->cts_dev.fwdata.cols;

	if (cts_data->reset_pin_test_result ||
	    cts_data->int_pin_test_result ||
	    cts_data->rawdata_test_result ||
	    cts_data->selfrawdata_test_result ||
	    cts_data->deviation_test_result ||
	    cts_data->noise_test_result ||
	    cts_data->selfnoise_test_result ||
	    cts_data->open_test_result || cts_data->short_test_result) {
		fail_reason_str = "panel_reason-";
	}

output_result_str:
	snprintf(info->result, TS_RAWDATA_RESULT_MAX,
		 "0%c-1%c-2%c-3%c-4%c-5%c-6%c-7%c-8%c-%s-%s-%s\n",
		 cts_data->reset_pin_test_result ? 'F' : 'P',
		 cts_data->int_pin_test_result ? 'F' : 'P',
		 cts_data->rawdata_test_result ? 'F' : 'P',
		 cts_data->selfrawdata_test_result ? 'F' : 'P',
		 cts_data->deviation_test_result ? 'F' : 'P',
		 cts_data->noise_test_result ? 'F' : 'P',
		 cts_data->selfnoise_test_result ? 'F' : 'P',
		 cts_data->open_test_result ? 'F' : 'P',
		 cts_data->short_test_result ? 'F' : 'P',
		 fail_reason_str ? fail_reason_str : "",
		 cts_data->cts_dev.hwdata->name, cts_data->project_id);

	TS_LOG_INFO("Factory test result: %s\n", info->result);

	return ret;
}

static int print_touch_data_row_to_buffer(char *buf, size_t size,
					  const u16 *data, int cols,
					  const char *prefix,
					  const char *suffix, char seperator)
{
	int c, count = 0;

	if (prefix)
		count += scnprintf(buf, size, "%s", prefix);

	for (c = 0; c < cols; c++) {
		count += scnprintf(buf + count, size - count,
				   "%4u%c ", data[c], seperator);
	}

	if (suffix)
		count += scnprintf(buf + count, size - count, "%s", suffix);

	return count;
}

static void print_touch_data_to_seq_file(struct seq_file *m,
					 const u16 *data, int frames, int rows,
					 int cols)
{
	int i, r;

	for (i = 0; i < frames; i++) {
		for (r = 0; r < rows; r++) {
			char linebuf[256];
			int len;

			len =
			    print_touch_data_row_to_buffer(linebuf,
							   sizeof(linebuf),
							   data, cols, NULL,
							   "\n", ',');
			seq_puts(m, linebuf);

			data += cols;
		}
		seq_putc(m, '\n');
	}
}

static void print_test_result_to_seq_file(struct seq_file *m,
					  const char *test_desc,
					  int test_result)
{
	if (test_result > 0) {
		seq_printf(m, "%-10s test: FAIL %d nodes\n", test_desc,
			   test_result);
	} else if (test_result < 0) {
		seq_printf(m, "%-10s test: FAIL %d(%s)\n", test_desc,
			   test_result, cts_strerror(test_result));
	} else {
		seq_printf(m, "%-10s test: PASS\n", test_desc);
	}
}

int chipone_rawdata_proc_printf(struct seq_file *m,
				struct ts_rawdata_info *info, int range_size,
				int row_size)
{
	int rows, cols;

	if ((cts_data == NULL) || (!cts_data->cts_dev.fwdata.rows) ||
		(!cts_data->cts_dev.fwdata.cols)) {
		TS_LOG_ERR("Print touch data to seq file with cts_data = NULL\n");
		return -EFAULT;
	}

	TS_LOG_INFO("Print touch data to seq file, range: %d, row: %d\n",
		    range_size, row_size);

	rows = cts_data->cts_dev.fwdata.rows;
	cols = cts_data->cts_dev.fwdata.cols;
	seq_printf(m, "rows %04x, cols = %04d \n\n", rows ,cols);
	seq_printf(m, "FW Version %04x!\n\n", cts_data->cts_dev.fwdata.version);

	if (cts_data->test_reset_pin) {
		print_test_result_to_seq_file(m, "Reset-Pin",
					      cts_data->reset_pin_test_result);
	}

	if (cts_data->test_int_pin) {
		print_test_result_to_seq_file(m, "Int-Pin",
					      cts_data->int_pin_test_result);
	}

	if (cts_data->test_rawdata) {
		print_test_result_to_seq_file(m,
					      "Rawdata",
					      cts_data->rawdata_test_result);
		print_test_result_to_seq_file(m,
					      "self Rawdata",
					      cts_data->selfrawdata_test_result);
		if (cts_data->dump_factory_test_data_to_buffer &&
		    (cts_data->rawdata_test_result > 0 ||
		     !cts_data->print_factory_test_data_only_fail)) {
			print_touch_data_to_seq_file(m,
				cts_data->rawdata_test_param.test_data_buf,
				cts_data->rawdata_test_priv_param.frames, rows + 2, cols);
		}
	}

	if (cts_data->test_deviation) {
		print_test_result_to_seq_file(m, "Deviation",
					      cts_data->deviation_test_result);
		if (cts_data->dump_factory_test_data_to_buffer &&
		    (cts_data->deviation_test_result > 0 ||
		     !cts_data->print_factory_test_data_only_fail)) {
			print_touch_data_to_seq_file(m,
				cts_data->deviation_test_param.test_data_buf,
				cts_data->deviation_test_priv_param.frames, rows + 2, cols);
		}
	}

	if (cts_data->test_noise) {
		print_test_result_to_seq_file(m,
				"Noise", cts_data->noise_test_result);
		print_test_result_to_seq_file(m,
				"self Noise", cts_data->selfnoise_test_result);
		if (cts_data->dump_factory_test_data_to_buffer &&
		    (cts_data->noise_test_result > 0 ||
		     !cts_data->print_factory_test_data_only_fail)) {
			print_touch_data_to_seq_file(m,
				cts_data->noise_test_param.test_data_buf,
				cts_data->noise_test_priv_param.frames + 3, rows + 2, cols);
		}
	}
	if (cts_data->test_open) {
		print_test_result_to_seq_file(m, "Open",
					      cts_data->open_test_result);
		if (cts_data->dump_factory_test_data_to_buffer &&
		    (cts_data->open_test_result > 0 ||
		     !cts_data->print_factory_test_data_only_fail))
			print_touch_data_to_seq_file(m,
				cts_data->open_test_param.test_data_buf, 1, rows, cols);
	}

	if (cts_data->test_short) {
		print_test_result_to_seq_file(m, "Short",
					cts_data->short_test_result);
		if (cts_data->dump_factory_test_data_to_buffer &&
		    (cts_data->short_test_result > 0 ||
		     !cts_data->print_factory_test_data_only_fail)) {
			print_touch_data_to_seq_file(m,
				cts_data->short_test_param.test_data_buf, 1, 2, cols);
		}
	}

	return 0;
}

static int chipone_wakeup_gesture_enable_switch(struct
						ts_wakeup_gesture_enable_info
						*info)
{
	int ret = NO_ERR;

	TS_LOG_INFO("Gesture wakeup switch\n");

	if (info == NULL) {
		TS_LOG_ERR("Gesture wakeup switch with info = NULL\n");
		ret = -EINVAL;
	}

	TS_LOG_INFO("Gesture wakeup switch returns %d\n", ret);
	return ret;
}

static int cts_chip_suspend(void)
{
	struct ts_kit_device_data *tskit_dev_data = NULL;
	struct ts_kit_platform_data *ts_plat_data = NULL;
	struct ts_easy_wakeup_info *wakeup_info = NULL;
	int ret = NO_ERR;

	if (cts_data == NULL) {
		TS_LOG_ERR("Chip suspend with cts_data = NULL\n");
		return -EINVAL;
	}

	tskit_dev_data = cts_data->tskit_data;
	ts_plat_data = tskit_dev_data->ts_platform_data;
	wakeup_info = &tskit_dev_data->easy_wakeup_info;

	TS_LOG_INFO("Chip suspend with %s  wakeup\n",
		    wakeup_info->sleep_mode == TS_GESTURE_MODE ? "" : "out");

	switch ((int)(wakeup_info->sleep_mode ||
		wakeup_info->aod_mode))  {
	case TS_POWER_OFF_MODE:
		TS_LOG_INFO("%s: enter power_off mode\n", __func__);
		chipone_power_switch(SWITCH_OFF);
		wakeup_info->easy_wakeup_flag = false;
		wakeup_info->off_motion_on = false;
		break;

	case TS_GESTURE_MODE:
		cts_enable_gesture_wakeup(&cts_data->cts_dev);
		wakeup_info->easy_wakeup_flag = true;
		wakeup_info->off_motion_on = true;
		break;

	default:
		TS_LOG_ERR("Suspend with unknown sleep mode %d\n",
			   wakeup_info->sleep_mode);
		break;
	}

	ret = cts_stop_device(&cts_data->cts_dev);
	if (ret) {
		TS_LOG_ERR("Stop device failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}
	/* Enable IRQ wake if gesture wakeup enabled */
	if (cts_is_gesture_wakeup_enabled(&cts_data->cts_dev)) {
		TS_LOG_INFO("%s: enter sleep mode\n", __func__);
		cts_lock_device(&cts_data->cts_dev);
		ret = cts_suspend_device(&cts_data->cts_dev);
		cts_unlock_device(&cts_data->cts_dev);
		if (ret) {
			TS_LOG_ERR("Suspend device failed %d(%s)\n",
				   ret, cts_strerror(ret));
		}
	}

	return ret;
}

static int chipone_chip_resume(void)
{
	struct ts_easy_wakeup_info *wakeup_info = NULL;
	int ret = NO_ERR;

	TS_LOG_INFO("resume enter\n");

	if (cts_data == NULL) {
		TS_LOG_ERR("Chip resume with cts_data = NULL\n");
		return -EINVAL;
	}

	wakeup_info = &cts_data->tskit_data->easy_wakeup_info;

	TS_LOG_INFO("Chip resume with%s gesture wakeup\n",
		    wakeup_info->sleep_mode == TS_GESTURE_MODE ? "" : "out");

	wakeup_info->easy_wakeup_flag = false;

	switch ((int)(wakeup_info->sleep_mode ||
		wakeup_info->aod_mode))  {
	case TS_POWER_OFF_MODE:
		chipone_power_switch(SWITCH_ON);
		break;
	 /* gesture mode  needn't do anything */
	case TS_GESTURE_MODE:
          	TS_LOG_INFO("Resume with gesture mode %d\n",
			   wakeup_info->sleep_mode);
		  break;
	default:
		TS_LOG_ERR("resume with unknown sleep mode %d\n",
			   wakeup_info->sleep_mode);
		break;
	}

	cts_lock_device(&cts_data->cts_dev);
	ret = cts_resume_device(&cts_data->cts_dev);
	cts_unlock_device(&cts_data->cts_dev);
	if (ret) {
		TS_LOG_ERR("Resume device failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}

	ret = cts_start_device(&cts_data->cts_dev);
	if (ret) {
		TS_LOG_ERR("Start device failed %d(%s)\n",
			   ret, cts_strerror(ret));
		return ret;
	}

	return ret;
}

static int chipone_chip_test(struct ts_cmd_node *in_cmd,
			     struct ts_cmd_node *out_cmd)
{
	return 0;
}

static int chipone_hw_reset(void)
{
	int ret = NO_ERR;

	if (cts_data == NULL) {
		TS_LOG_ERR("Chip hw reset with cts_data = NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Chip hw reset\n");

	ret = do_hw_reset(cts_data->resume_hw_reset_timing);
	if (ret) {
		TS_LOG_ERR("Do hardware reset failed %d(%s)\n",
			   ret, cts_strerror(ret));
	}

	TS_LOG_INFO("Chip hw reset returns %d\n", ret);

	return ret;
}

static int chipone_wrong_touch(void)
{
	if (cts_data == NULL) {
		TS_LOG_ERR("Wrong touch switch with cts_data = NULL\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Wrong touch switch ON\n");

	mutex_lock(&cts_data->wrong_touch_lock);
	cts_data->tskit_data->easy_wakeup_info.off_motion_on = true;
	mutex_unlock(&cts_data->wrong_touch_lock);

	return 0;
}

/* Platform ops needed */
int cts_plat_enable_irq(struct cts_platform_data *pdata)
{
	return 0;
}

int cts_plat_disable_irq(struct cts_platform_data *pdata)
{
	return 0;
}
static int chipone_charger_switch(struct ts_charger_info *info)
{
	int ret;

	if (info == NULL) {
		TS_LOG_ERR("Charger switch with info = NULL\n");
		return -EINVAL;
	}

	if (!cts_data->use_huawei_charger_detect) {
		TS_LOG_ERR("Charger switch with use_huawei_charger_detect = false\n");
		return -EINVAL;
	}

	if (!cts_data->tskit_data->ts_platform_data->feature_info.charger_info.charger_supported) {
		TS_LOG_ERR("Charger switch with charger_supported = false\n");
		return -EINVAL;
	}

	TS_LOG_INFO("Charger switch, action: %d\n", info->op_action);

	switch (info->op_action) {
	case TS_ACTION_READ:
		ret = cts_fw_reg_readb(&cts_data->cts_dev,
			CTS_DEVICE_FW_REG_CHARGER_STATUS, &info->charger_switch);
		if (ret) {
			TS_LOG_ERR("Get charger status from device failed %d(%s)\n",
				ret, cts_strerror(ret));
		}
		return ret;
	case TS_ACTION_WRITE:
		TS_LOG_INFO("Charger switch write charger_switch: %u\n", info->charger_switch);
		cts_lock_device(&cts_data->cts_dev);
		ret = cts_send_command(&cts_data->cts_dev,
			info->charger_switch ? CTS_CMD_CHARGER_ATTACHED :
			CTS_CMD_CHARGER_DETACHED);
		cts_unlock_device(&cts_data->cts_dev);
		if (ret) {
			TS_LOG_ERR("Send %s to device failed %d(%s)\n",
				info->charger_switch ? "CMD_CHARGER_ATTACHED" :
				"CMD_CHARGER_DETACHED", ret, cts_strerror(ret));
		}
		return ret;
	default:
		TS_LOG_INFO("Charger switch with action: %d unknown\n", info->op_action);
		 return -EINVAL;
	}
}

int chipone_chip_check_status(void)
{
	int ret;
	u16 fwid;

	if (!cts_data->tskit_data->need_wd_check_status) {
		TS_LOG_ERR("Check status with need_wd_check_status = false\n");
		return -EINVAL;
	}

	TS_LOG_DEBUG("Check chip status\n");
	cts_lock_device(&cts_data->cts_dev);
    	ret = cts_fw_reg_readw(&cts_data->cts_dev, CTS_DEVICE_FW_REG_CHIP_TYPE, &fwid);
	cts_unlock_device(&cts_data->cts_dev);
	if (ret) {
		TS_LOG_ERR("Check status by get fwid failed %d(%s)\n", ret, cts_strerror(ret));
		return ret;
	}

	if (be16_to_cpu(fwid) != cts_data->cts_dev.hwdata->fwid) {
		TS_LOG_ERR("Check status get fwid 0x%04x != 0x%04x\n",
			be16_to_cpu(fwid), cts_data->cts_dev.hwdata->fwid);
		ret = chipone_hw_reset();
		if (ret)
			TS_LOG_ERR("hw reset failed\n");
	}
	return ret;
}

int chipone_regs_operate(struct ts_regs_info * info)
{
	int ret;

	if (info == NULL) {
	TS_LOG_ERR("Regs operate with info = NULL\n");
	return -EINVAL;
	}

	if (info->addr > 0xFFFF) {
		TS_LOG_ERR("Regs operate with addr 0x%x > 0xFFFF\n", info->addr);
		return -EINVAL;
	}

	if (info->num > TS_MAX_REG_VALUE_NUM) {
		TS_LOG_ERR("Regs operate with num %d > %d\n",
			 info->addr, TS_MAX_REG_VALUE_NUM);
		return -EINVAL;
	}

	TS_LOG_INFO("Regs operate, action: %d, addr: 0x%04x, num: %d\n",
		info->op_action, info->addr, info->num);

	cts_lock_device(&cts_data->cts_dev);
	 switch (info->op_action) {
	case TS_ACTION_READ:
		ret = cts_fw_reg_readsb(&cts_data->cts_dev,
		info->addr, info->values, (size_t)info->num);
		break;
	case TS_ACTION_WRITE:
		ret = cts_fw_reg_writesb(&cts_data->cts_dev,
		info->addr, info->values, (size_t)info->num);
		break;
	default:
		ret = -EINVAL;
	}
	cts_unlock_device(&cts_data->cts_dev);
	if (ret) {
		TS_LOG_ERR("Regs operate failed %d(%s)\n", ret, cts_strerror(ret));
	}
	return ret;
}

int cts_plat_reset_device(struct cts_platform_data *pdata)
{
	return chipone_hw_reset();
}

int cts_plat_release_all_touch(struct cts_platform_data *pdata)
{
	return 0;
}

int cts_plat_set_reset(struct cts_platform_data *pdata, int val)
{
	return set_reset_gpio_value(val);
}

int cts_plat_get_int_pin(struct cts_platform_data *pdata)
{
	return gpio_get_value(cts_data->tskit_data->ts_platform_data->irq_gpio);
}

struct ts_device_ops chipone_ts_device_ops = {
	.chip_detect = cts_chip_detect,
	.chip_init = chipone_chip_init,
	.chip_parse_config = chipone_parse_config,
	.chip_input_config = chipone_input_config,
	.chip_irq_top_half = chipone_irq_top_half,
	.chip_irq_bottom_half = chipone_irq_bottom_half,
	.chip_reset = chipone_chip_reset,
	.chip_shutdown = chipone_chip_shutdown,
	.chip_get_info = cts_chip_get_info,
	.chip_fw_update_boot = chipone_fw_update_boot,
	.chip_fw_update_sd = chipone_fw_update_sd,
	.chip_get_rawdata = cts_chip_get_rawdata,
	.chip_get_capacitance_test_type = chipone_get_capacitance_test_type,
	.chip_special_rawdata_proc_printf = chipone_rawdata_proc_printf,
	.chip_wakeup_gesture_enable_switch = chipone_wakeup_gesture_enable_switch,
	.chip_suspend = cts_chip_suspend,
	.chip_resume = chipone_chip_resume,
	.chip_test = chipone_chip_test,
	.chip_hw_reset = chipone_hw_reset,
	.chip_wrong_touch = chipone_wrong_touch,
	.chip_charger_switch = chipone_charger_switch,
	.chip_check_status = chipone_chip_check_status,
	.chip_regs_operate = chipone_regs_operate,
};

static int __init cts_ts_module_init(void)
{
	struct device_node *tskit_of_node = NULL;
	struct device_node *chipone_of_node = NULL;
	int ret = NO_ERR;
	bool found = false;

	TS_LOG_INFO("Cts touch module init\n");

	cts_data = kzalloc(sizeof(struct chipone_ts_data), GFP_KERNEL);
	if (cts_data == NULL) {
		TS_LOG_ERR("Alloc chipone_ts_data failed\n");
		return -ENOMEM;
	}

	cts_data->tskit_data =
	    kzalloc(sizeof(struct ts_kit_device_data), GFP_KERNEL);
	if (cts_data->tskit_data == NULL) {
		TS_LOG_ERR("Alloc ts_kit_device_data failed\n");
		ret = -ENOMEM;
		goto out;
	}

	cts_data->pdata = kzalloc(sizeof(struct cts_platform_data), GFP_KERNEL);
	if (cts_data->pdata == NULL) {
		TS_LOG_ERR("Alloc cts_platform_data failed\n");
		ret = -ENOMEM;
		goto out;
	}

	tskit_of_node = of_find_compatible_node(NULL, NULL, HUAWEI_TS_KIT);
	if (tskit_of_node == NULL) {
		TS_LOG_ERR("Huawei ts_kit of node NOT found in DTS\n");
		ret = -ENODEV;
		goto out;
	}

	for_each_child_of_node(tskit_of_node, chipone_of_node) {
		if (of_device_is_compatible(chipone_of_node, "chipone")) {
			TS_LOG_ERR("found device tree node\n");
			found = true;
		break;
		}
	}
	 if (!found) {
		TS_LOG_ERR(" not found chip chip child node  !\n");
		ret = -EINVAL;
		goto out;
	}
	mutex_init(&cts_data->wrong_touch_lock);
	cts_data->tskit_data->cnode = chipone_of_node;
	cts_data->tskit_data->ops = &chipone_ts_device_ops;
	TS_LOG_INFO("%s childnode found: %p\n", __func__, chipone_of_node);
	ret = huawei_ts_chip_register(cts_data->tskit_data);
	if (ret) {
		TS_LOG_ERR("Register chipone chip to ts_kit failed %d(%s)!\n",
			   ret, cts_strerror(ret));
		goto out;
	}

	return 0;

 out:
	kfree(cts_data->tskit_data);
	cts_data->tskit_data = NULL;
	kfree(cts_data->pdata);
	cts_data->pdata = NULL;
	kfree(cts_data);
	cts_data = NULL;

	TS_LOG_INFO("Chipone touch module init returns %d\n", ret);
	return ret;
}

static void __exit chipone_ts_module_exit(void)
{
	if (cts_data == NULL) {
		TS_LOG_ERR("Module exit with cts_data = NULL\n");
		return;
	}

	TS_LOG_INFO("Chipone touch module exit");

	pinctrl_select_idle_state();
	release_pinctrl();

	if (cts_data->cached_firmware) {
		TS_LOG_INFO("Release cached firmware");
		cts_release_firmware(cts_data->cached_firmware);
		cts_data->cached_firmware = NULL;
	}
	cts_tool_deinit(cts_data);

	free_factory_test_mem();

	if (cts_data->tskit_data != NULL) {
		kfree(cts_data->tskit_data);
		cts_data->tskit_data = NULL;
	}

	if (cts_data->pdata != NULL) {
		kfree(cts_data->pdata);
		cts_data->pdata = NULL;
	}

	kfree(cts_data);
	cts_data = NULL;
}

late_initcall(cts_ts_module_init);
module_exit(chipone_ts_module_exit);
MODULE_AUTHOR("Miao Defang <dfmiao@chiponeic.com>");
MODULE_DESCRIPTION("Chipone touch driver for Huawei tpkit");
MODULE_LICENSE("GPL");
