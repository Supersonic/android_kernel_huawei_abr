#define LOG_TAG "Test"

#include "cts_config.h"
#include "cts_platform.h"
#include "cts_core.h"
#include "cts_strerror.h"
#include "cts_test.h"

const char *cts_test_item_str(int test_item)
{
#define case_test_item(item) \
	case CTS_TEST_ ## item: return #item "-TEST"

	switch (test_item) {
		case_test_item(RESET_PIN);
		case_test_item(INT_PIN);
		case_test_item(RAWDATA);
		case_test_item(NOISE);
		case_test_item(OPEN);
		case_test_item(SHORT);
		case_test_item(COMPENSATE_CAP);

		default: return "INVALID";
	}
#undef case_test_item
}

#define CTS_FIRMWARE_WORK_MODE_NORMAL (0x00)
#define CTS_FIRMWARE_WORK_MODE_FACTORY (0x01)
#define CTS_FIRMWARE_WORK_MODE_CONFIG (0x02)
#define CTS_FIRMWARE_WORK_MODE_TEST (0x03)

#define CTS_TEST_SHORT (0x01)
#define CTS_TEST_OPEN (0x02)

#define CTS_SHORT_TEST_UNDEFINED (0x00)
#define CTS_SHORT_TEST_BETWEEN_COLS (0x01)
#define CTS_SHORT_TEST_BETWEEN_ROWS (0x02)
#define CTS_SHORT_TEST_BETWEEN_GND (0x03)

#define TEST_RESULT_BUFFER_SIZE(cts_dev) \
	(cts_dev->fwdata.rows * cts_dev->fwdata.cols * 2)

#define RAWDATA_BUFFER_SIZE(cts_dev) \
		(cts_dev->fwdata.rows * cts_dev->fwdata.cols * 2)

static int disable_fw_monitor_mode(struct cts_device *cts_dev)
{
	int ret;
	u8 value;

	ret = cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_FLAG_BITS, &value);
	if (ret) {
		return ret;
	}

	if (value & BIT(0)) {
		return cts_fw_reg_writeb(cts_dev,
			CTS_DEVICE_FW_REG_FLAG_BITS, value & (~BIT(0)));
	}

	return 0;
}

static int disable_fw_auto_compensate(struct cts_device *cts_dev)
{
	return cts_fw_reg_writeb(cts_dev,
		CTS_DEVICE_FW_REG_AUTO_CALIB_COMP_CAP_ENABLE, 0);
}

static int set_fw_work_mode(struct cts_device *cts_dev, u8 mode)
{
	int ret, retries;
	u8  pwr_mode;

	TS_LOG_INFO("Set firmware work mode to %u", mode);

	ret = cts_fw_reg_writeb(cts_dev, CTS_DEVICE_FW_REG_WORK_MODE, mode);
	if (ret) {
		TS_LOG_ERR("Write firmware work mode register failed %d", ret);
		return ret;
	}

	ret = cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_POWER_MODE,
		&pwr_mode);
	if (ret) {
		TS_LOG_ERR("Read firmware power mode register failed %d", ret);
		return ret;
	}

	if (pwr_mode == 1) {
		ret = cts_send_command(cts_dev, CTS_CMD_QUIT_GESTURE_MONITOR);
		if (ret) {
			TS_LOG_ERR("Send cmd QUIT_GESTURE_MONITOR failed %d", ret);
			return ret;
		}

		msleep(50);
	}

	retries = 0;
	do {
		u8 sys_busy, curr_mode;

		msleep(10);

		ret = cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_SYS_BUSY,
			&sys_busy);
		if (ret) {
			TS_LOG_ERR("Read firmware system busy register failed %d", ret);
			//return ret;
			continue;
		}
		if (sys_busy)
			continue;

		ret = cts_fw_reg_readb(cts_dev, CTS_DEVICE_FW_REG_GET_WORK_MODE,
			&curr_mode);
		if (ret) {
			TS_LOG_ERR("Read firmware current work mode failed %d", ret);
			//return ret;
			continue;
		}

		if (curr_mode == mode /*|| curr_mode == 0xFF*/) {
			break;
		}
	} while (retries++ < 1000);

	return (retries >= 1000 ? -ETIMEDOUT : 0);
}

static int set_display_state(struct cts_device *cts_dev, bool active)
{
	int ret;
	u8  access_flag;

	TS_LOG_INFO("Set display state to %s", active ? "ACTIVE" : "SLEEP");

	ret = cts_hw_reg_readb(cts_dev, 0x3002C, &access_flag);
	if (ret) {
		TS_LOG_ERR("Read display access flag failed %d", ret);
		return ret;
	}

	ret = cts_hw_reg_writeb(cts_dev, 0x3002C, access_flag | 0x01);
	if (ret) {
		TS_LOG_ERR("Write display access flag %02x failed %d", access_flag, ret);
		return ret;
	}

	if (active) {
		ret = cts_hw_reg_writeb(cts_dev, 0x3C044, 0x55);
		if (ret) {
			TS_LOG_ERR("Write DCS-CMD11 fail");
			return ret;
		}

		msleep(100);

		ret = cts_hw_reg_writeb(cts_dev, 0x3C0A4, 0x55);
		if (ret) {
			TS_LOG_ERR("Write DCS-CMD29 fail");
			return ret;
		}

		msleep(100);
	} else {
		ret = cts_hw_reg_writeb(cts_dev, 0x3C0A0, 0x55);
		if (ret) {
			TS_LOG_ERR("Write DCS-CMD28 fail");
			return ret;
		}

		msleep(100);

		ret = cts_hw_reg_writeb(cts_dev, 0x3C040, 0x55);
		if (ret) {
			TS_LOG_ERR("Write DCS-CMD10 fail");
			return ret;
		}

		msleep(100);
	}

	ret = cts_hw_reg_writeb(cts_dev, 0x3002C, access_flag);
	if (ret) {
		TS_LOG_ERR("Restore display access flag %02x failed %d", access_flag, ret);
		return ret;
	}

	return 0;
}

static int wait_test_complete(struct cts_device *cts_dev, int skip_frames)
{
	int ret, i, j;

	TS_LOG_INFO("Wait test complete skip %d frames", skip_frames);

	for (i = 0; i < (skip_frames + 1); i++) {
		u8 ready;

		for (j = 0; j < 1000; j++) {
			mdelay(1);

			ready = 0;
			ret = cts_get_data_ready_flag(cts_dev, &ready);
			if (ret) {
				TS_LOG_ERR("Get data ready flag failed %d", ret);
				return ret;
			}

			if (ready) {
				break;
			}
		}

		if (ready == 0) {
			TS_LOG_ERR("Wait test complete timeout");
			return -ETIMEDOUT;
		}
		if (i < skip_frames) {
			ret = cts_clr_data_ready_flag(cts_dev);
			if (ret) {
				TS_LOG_ERR("Clr data ready flag failed %d", ret);
				return ret;
			}
		}
	}

	return 0;
}

static int get_test_result(struct cts_device *cts_dev, u16 *result)
{
	int ret;

	ret = cts_fw_reg_readsb(cts_dev, CTS_DEVICE_FW_REG_RAW_DATA, result,
			TEST_RESULT_BUFFER_SIZE(cts_dev));
	if (ret) {
		TS_LOG_ERR("Get test result data failed %d", ret);
		return ret;
	}

	ret = cts_clr_data_ready_flag(cts_dev);
	if (ret) {
		TS_LOG_ERR("Clear data ready flag failed %d", ret);
		return ret;
	}

	return 0;
}

static int set_fw_test_type(struct cts_device *cts_dev, u8 type)
{
	int ret, retries = 0;
	u8  sys_busy;
	u8  type_readback;

	TS_LOG_INFO("Set test type %d", type);

	ret = cts_fw_reg_writeb(cts_dev, 0x34, type);
	if (ret) {
		TS_LOG_ERR("Write test type register to failed %d", ret);
		return ret;
	}

	do {
		msleep(1);

		ret = cts_fw_reg_readb(cts_dev, 0x01, &sys_busy);
		if (ret) {
			TS_LOG_ERR("Read system busy register failed %d", ret);
			return ret;
		}
	} while (sys_busy && retries++ < 1000);

	if (retries >= 1000) {
		TS_LOG_ERR("Wait system ready timeout");
		return -ETIMEDOUT;
	}

	ret = cts_fw_reg_readb(cts_dev, 0x34, &type_readback);
	if (ret) {
		TS_LOG_ERR("Read test type register failed %d", ret);
		return ret;
	}

	if (type != type_readback) {
		TS_LOG_ERR("Set test type %u != readback %u", type, type_readback);
		return -EFAULT;
	}

	return 0;
}

static bool set_short_test_type(struct cts_device *cts_dev, u8 type)
{
	static struct fw_short_test_param {
		u8  type;
		u32 col_pattern[2];
		u32 row_pattern[2];
	} param = {
		.type = CTS_SHORT_TEST_BETWEEN_COLS,
		.col_pattern = {0, 0},
		.row_pattern = {0, 0}
	};
	int i, ret;

	TS_LOG_INFO("Set short test type to %u", type);

	param.type = type;
	for (i = 0; i < 5; i++) {
		u8 type_readback;

		ret = cts_fw_reg_writesb(cts_dev, 0x5000, &param, sizeof(param));
		if (ret) {
			TS_LOG_ERR("Set short test type to %u failed %d", type, ret);
			continue;
		}
		ret = cts_fw_reg_readb(cts_dev, 0x5000, &type_readback);
		if (ret) {
			TS_LOG_ERR("Get short test type failed %d", ret);
			continue;
		}
		if (type == type_readback) {
			return 0;
		} else {
			TS_LOG_ERR("Set test type %u != readback %u", type, type_readback);
			continue;
		}
	}

	return ret;
}

int cts_write_file(struct file *filp, const void *data, size_t size)
{
	loff_t  pos;
	ssize_t ret;

	pos = filp->f_pos;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	ret = kernel_write(filp, data, size, &pos);
#else
	ret = kernel_write(filp, data, size, pos);
#endif

	if (ret >= 0) {
		filp->f_pos += ret;
	}

	return ret;
}

/* Make directory for filepath
 * If filepath = "/A/B/C/D.file", it will make dir /A/B/C recursive
 * like userspace mkdir -p
 */
int cts_mkdir_for_file(const char *filepath, umode_t mode)
{
	char *dirname = NULL;
	int   dirname_len;
	char *s;
	int   ret;
	mm_segment_t fs;

	if (filepath == NULL) {
		TS_LOG_ERR("Create dir for file with filepath = NULL");
		return -EINVAL;
	}

	if (filepath[0] == '\0' || filepath[0] != '/') {
		TS_LOG_ERR("Create dir for file with invalid filepath[0]: %c",
			filepath[0]);
		return -EINVAL;
	}

	dirname_len = strrchr(filepath, '/') - filepath;
	if (dirname_len == 0) {
		TS_LOG_ERR("Create dir for file '%s' in root dir", filepath);
		return 0;
	}

	dirname = kstrndup(filepath, dirname_len, GFP_KERNEL);
	if (dirname == NULL) {
		TS_LOG_ERR("Create dir alloc mem for dirname failed");
		return -ENOMEM;
	}

	TS_LOG_INFO("Create dir '%s' for file '%s'", dirname, filepath);

	fs = get_fs();
	set_fs(KERNEL_DS);

	s = dirname + 1;   /* Skip leading '/' */
	while (1) {
		char c = '\0';

		/* Bypass leading non-'/'s and then subsequent '/'s */
		while (*s) {
			if (*s == '/') {
				do {
					++s;
				} while (*s == '/');
				c = *s;	 /* Save current char */
				*s = '\0';  /* and replace it with nul */
				break;
			}
			++s;
		}

		TS_LOG_DEBUG(" - Create dir '%s'", dirname);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
		ret = ksys_mkdir(dirname, 0660);
#else
		ret = sys_mkdir(dirname, 0660);
#endif
		if (ret < 0 && ret != -EEXIST) {
			TS_LOG_INFO("Create dir '%s' failed %d(%s)",
				dirname, ret, cts_strerror(ret));
			/* Remove any inserted nul from the path */
			*s = c;
			break;
		}
		/* Reset ret to 0 if return -EEXIST */
		ret = 0;

		if (c) {
			/* Remove any inserted nul from the path */
			*s = c;
		} else {
			break;
		}
	}

	set_fs(fs);

	if (dirname) {
		kfree(dirname);
	}

	return ret;
}

struct file *cts_test_data_filp = NULL;
int cts_start_dump_test_data_to_file(const char *filepath, bool append_to_file)
{
	int ret;

#define START_BANNER \
		">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"

	TS_LOG_INFO("Start dump test data to file '%s'", filepath);

	ret = cts_mkdir_for_file(filepath, 0777);
	if (ret) {
		TS_LOG_ERR("Create dir for test data file failed %d", ret);
		return ret;
	}

	cts_test_data_filp = filp_open(filepath,
		O_WRONLY | O_CREAT | (append_to_file ? O_APPEND : O_TRUNC),
		S_IRUGO | S_IWUGO);
	if (IS_ERR(cts_test_data_filp)) {
		ret = PTR_ERR(cts_test_data_filp);
		cts_test_data_filp = NULL;
		TS_LOG_ERR("Open file '%s' for test data failed %d",
			cts_test_data_filp, ret);
		return ret;
	}

	cts_write_file(cts_test_data_filp, START_BANNER, strlen(START_BANNER));

	return 0;
#undef START_BANNER
}

void cts_stop_dump_test_data_to_file(void)
{
#define END_BANNER \
	"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"
	int r;

	TS_LOG_INFO("Stop dump test data to file");

	if (cts_test_data_filp) {
		cts_write_file(cts_test_data_filp,
			END_BANNER, strlen(END_BANNER));
		r = filp_close(cts_test_data_filp, NULL);
		if (r) {
			TS_LOG_ERR("Close test data file failed %d", r);
		}
		cts_test_data_filp = NULL;
	} else {
		TS_LOG_ERR("Stop dump tsdata to file with filp = NULL");
	}
#undef END_BANNER
}

static void cts_dump_tsdata(struct cts_device *cts_dev,
		const char *desc, const u16 *data, bool to_console)
{
#define SPLIT_LINE_STR \
	"---------------------------------------------------------------------------------------------------------------"
#define ROW_NUM_FORMAT_STR "%2d | "
#define COL_NUM_FORMAT_STR "%-5u "
#define DATA_FORMAT_STR "%-5u "

	int r, c;
	u32 max, min, sum, average;
	int max_r, max_c, min_r, min_c;
	char line_buf[128];
	int count = 0;

	max = min = data[0];
	sum = 0;
	max_r = max_c = min_r = min_c = 0;
	for (r = 0; r < cts_dev->fwdata.rows; r++) {
		for (c = 0; c < cts_dev->fwdata.cols; c++) {
			u16 val = data[r * cts_dev->fwdata.cols + c];

			sum += val;
			if (val > max) {
				max = val;
				max_r = r;
				max_c = c;
			} else if (val < min) {
				min = val;
				min_r = r;
				min_c = c;
			}
		}
	}
	average = sum / (cts_dev->fwdata.rows * cts_dev->fwdata.cols);

	count = 0;
	count += scnprintf(line_buf + count, sizeof(line_buf) - count,
		" %s test data MIN: [%u][%u]=%u, MAX: [%u][%u]=%u, AVG=%u",
		desc, min_r, min_c, min, max_r, max_c, max, average);
	if (to_console) {
		TS_LOG_INFO(SPLIT_LINE_STR);
		TS_LOG_INFO("%s", line_buf);
		TS_LOG_INFO(SPLIT_LINE_STR);
	}
	if (cts_test_data_filp) {
		cts_write_file(cts_test_data_filp, SPLIT_LINE_STR, strlen(SPLIT_LINE_STR));
		cts_write_file(cts_test_data_filp, "\n", 1);
		cts_write_file(cts_test_data_filp, line_buf, count);
		cts_write_file(cts_test_data_filp, "\n", 1);
		cts_write_file(cts_test_data_filp, SPLIT_LINE_STR, strlen(SPLIT_LINE_STR));
		cts_write_file(cts_test_data_filp, "\n", 1);
	}

	count = 0;
	count += scnprintf(line_buf + count, sizeof(line_buf) - count, "   |  ");
	for (c = 0; c < cts_dev->fwdata.cols; c++) {
		count += scnprintf(line_buf + count, sizeof(line_buf) - count,
			COL_NUM_FORMAT_STR, c);
	}
	if (to_console) {
		TS_LOG_INFO("%s", line_buf);
		TS_LOG_INFO(SPLIT_LINE_STR);
	}
	if (cts_test_data_filp) {
		cts_write_file(cts_test_data_filp, line_buf, count);
		cts_write_file(cts_test_data_filp, "\n", 1);
		cts_write_file(cts_test_data_filp, SPLIT_LINE_STR, strlen(SPLIT_LINE_STR));
		cts_write_file(cts_test_data_filp, "\n", 1);
	}

	for (r = 0; r < cts_dev->fwdata.rows; r++) {
		count = 0;
		count += scnprintf(line_buf + count, sizeof(line_buf) - count,
			ROW_NUM_FORMAT_STR, r);
		for (c = 0; c < cts_dev->fwdata.cols; c++) {
			count +=
				scnprintf(line_buf + count, sizeof(line_buf) - count,
					DATA_FORMAT_STR,
					data[r * cts_dev->fwdata.cols + c]);
		}
		if (to_console) {
			TS_LOG_INFO("%s", line_buf);
		}
		if (cts_test_data_filp) {
			cts_write_file(cts_test_data_filp, line_buf, count);
			cts_write_file(cts_test_data_filp, "\n", 1);
		}
	}
	if (to_console) {
		TS_LOG_INFO(SPLIT_LINE_STR);
	}
	if (cts_test_data_filp) {
		cts_write_file(cts_test_data_filp, SPLIT_LINE_STR, strlen(SPLIT_LINE_STR));
		cts_write_file(cts_test_data_filp, "\n", 1);
	}

#undef SPLIT_LINE_STR
#undef ROW_NUM_FORMAT_STR
#undef COL_NUM_FORMAT_STR
#undef DATA_FORMAT_STR
}

static bool is_invalid_node(u32 *invalid_nodes, u32 num_invalid_nodes,
	u16 row, u16 col)
{
	int i;

	for (i = 0; i < num_invalid_nodes; i++) {
		if (MAKE_INVALID_NODE(row,col)== invalid_nodes[i]) {
			return true;
		}
	}

	return false;
}

static int validate_tsdata(struct cts_device *cts_dev,
	const char *desc, u16 *data,
	u32 *invalid_nodes, u32 num_invalid_nodes,
	bool per_node, int *min, int *max)
{
#define SPLIT_LINE_STR \
	"------------------------------"

	int r, c;
	int failed_cnt = 0;

	TS_LOG_INFO("%s validate data: %s, num invalid node: %u, thresh[0]=[%d, %d]",
		desc, per_node ? "Per-Node" : "Uniform-Threshold",
		num_invalid_nodes, min ? min[0] : INT_MIN, max ? max[0] : INT_MAX);

	for (r = 0; r < cts_dev->fwdata.rows; r++) {
		for (c = 0; c < cts_dev->fwdata.cols; c++) {
			int offset = r * cts_dev->fwdata.cols + c;

			if (num_invalid_nodes &&
				is_invalid_node(invalid_nodes, num_invalid_nodes, r,c)) {
				continue;
			}

			if ((min != NULL && data[offset] < min[per_node ? offset : 0]) ||
				(max != NULL && data[offset] > max[per_node ? offset : 0])) {
				if (failed_cnt == 0) {
					TS_LOG_INFO(SPLIT_LINE_STR);
					TS_LOG_INFO("%s failed nodes:", desc);
				}
				failed_cnt++;

				TS_LOG_INFO("  %3d: [%-2d][%-2d] = %u",
					failed_cnt, r, c, data[offset]);
			}
		}
	}

	if (failed_cnt) {
		TS_LOG_INFO(SPLIT_LINE_STR);
		TS_LOG_INFO("%s test %d node total failed", desc, failed_cnt);
	}

	return failed_cnt;

#undef SPLIT_LINE_STR
}

static int validate_comp_cap(struct cts_device *cts_dev,
	const char *desc, u8 *cap,
	u32 *invalid_nodes, u32 num_invalid_nodes,
	bool per_node, int *min, int *max)
{
#define SPLIT_LINE_STR \
	"------------------------------"

	int r, c;
	int failed_cnt = 0;

	TS_LOG_INFO("Validate %s data: %s, num invalid node: %u, thresh[0]=[%d, %d]",
		desc, per_node ? "Per-Node" : "Uniform-Threshold",
		num_invalid_nodes, min ? min[0] : INT_MIN, max ? max[0] : INT_MAX);

	for (r = 0; r < cts_dev->fwdata.rows; r++) {
		for (c = 0; c < cts_dev->fwdata.cols; c++) {
			int offset = r * cts_dev->fwdata.cols + c;

			if (num_invalid_nodes &&
				is_invalid_node(invalid_nodes, num_invalid_nodes, r,c)) {
				continue;
			}

			if ((min != NULL && cap[offset] < min[per_node ? offset : 0]) ||
				(max != NULL && cap[offset] > max[per_node ? offset : 0])) {
				if (failed_cnt == 0) {
					TS_LOG_INFO(SPLIT_LINE_STR);
					TS_LOG_INFO("%s failed nodes:", desc);
				}
				failed_cnt++;

				TS_LOG_INFO("  %3d: [%-2d][%-2d] = %u",
					failed_cnt, r, c, cap[offset]);
			}
		}
	}

	if (failed_cnt) {
		TS_LOG_INFO(SPLIT_LINE_STR);
		TS_LOG_INFO("%s test %d node total failed", desc, failed_cnt);
	}

	return failed_cnt;

#undef SPLIT_LINE_STR
}

static int wait_fw_to_normal_work(struct cts_device *cts_dev)
{
	int i = 0;
	int ret;

	TS_LOG_INFO ("Wait fw to normal work");

	do {
		u8 work_mode;

		ret = cts_fw_reg_readb(cts_dev,
			CTS_DEVICE_FW_REG_GET_WORK_MODE, &work_mode);
		if (ret) {
			TS_LOG_ERR("Get fw curr work mode failed %d", work_mode);
			continue;
		} else {
			if (work_mode == CTS_FIRMWARE_WORK_MODE_NORMAL) {
				return 0;
			}
		}

		mdelay (10);
	} while (++i < 100);

	return -ETIMEDOUT;
}

static int prepare_test(struct cts_device *cts_dev)
{
	int ret;

	TS_LOG_INFO("Prepare test");

	cts_plat_reset_device(cts_dev->pdata);

	ret = cts_set_dev_esd_protection(cts_dev, false);
	if (ret) {
		TS_LOG_ERR("Disable firmware ESD protection failed %d", ret);
		return ret;
	}

	ret = disable_fw_monitor_mode(cts_dev);
	if (ret) {
		TS_LOG_ERR("Disable firmware monitor mode failed %d", ret);
		return ret;
	}

	ret = disable_fw_auto_compensate(cts_dev);
	if (ret) {
		TS_LOG_ERR("Disable firmware auto compensate failed %d", ret);
		return ret;
	}

	ret = set_fw_work_mode(cts_dev, CTS_FIRMWARE_WORK_MODE_CONFIG);
	if (ret) {
		TS_LOG_ERR("Set firmware work mode to WORK_MODE_CONFIG failed %d", ret);
		return ret;
	}

	cts_dev->rtdata.testing = true;

	return 0;
}

static void post_test(struct cts_device *cts_dev)
{
	int ret;

	TS_LOG_INFO("Post test");

	cts_plat_reset_device(cts_dev->pdata);

	ret = set_fw_work_mode(cts_dev, CTS_FIRMWARE_WORK_MODE_NORMAL);
	if (ret) {
		TS_LOG_ERR("Set firmware work mode to WORK_MODE_NORMAL failed %d", ret);
	}

	ret = wait_fw_to_normal_work(cts_dev);
	if (ret) {
		TS_LOG_ERR("Wait fw to normal work failed %d", ret);
		//return ret;
	}

	cts_dev->rtdata.testing = false;
}

/* Return 0 success
	negative value while error occurs
	positive value means how many nodes fail */
int cts_test_short(struct cts_device *cts_dev,
	struct cts_test_param *param)
{
	bool driver_validate_data = false;
	bool validate_data_per_node = false;
	bool stop_if_failed = false;
	bool dump_test_data_to_user = false;
	bool dump_test_data_to_console = false;
	bool dump_test_data_to_file = false;
	int  num_nodes;
	int  tsdata_frame_size;
	int  loopcnt;
	int  ret;
	u16 *test_result = NULL;
	bool recovery_display_state = false;
	u8   need_display_on;
	u8   feature_ver;
	ktime_t start_time, end_time, delta_time;

	if (cts_dev == NULL || param == NULL) {
		TS_LOG_ERR("Short test with invalid param: cts_dev: %p test param: %p",
			cts_dev, param);
		return -EINVAL;
	}

	num_nodes = cts_dev->fwdata.rows * cts_dev->fwdata.cols;
	tsdata_frame_size = 2 * num_nodes;

	driver_validate_data =
		!!(param->flags & CTS_TEST_FLAG_VALIDATE_DATA);
	validate_data_per_node =
		!!(param->flags & CTS_TEST_FLAG_VALIDATE_PER_NODE);
	dump_test_data_to_user =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_USERSPACE);
	dump_test_data_to_console =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_CONSOLE);
	dump_test_data_to_file =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE);
	stop_if_failed =
		!!(param->flags & CTS_TEST_FLAG_STOP_TEST_IF_VALIDATE_FAILED);

	TS_LOG_INFO("Short test, flags: 0x%08x,"
			   "num invalid node: %u, "
			   "test data file: '%s' buf size: %d, "
			   "drive log file: '%s' buf size: %d",
		param->flags, param->num_invalid_node,
		param->test_data_filepath, param->test_data_buf_size,
		param->driver_log_filepath, param->driver_log_buf_size);

	start_time = ktime_get();

	if (dump_test_data_to_user) {
		test_result = (u16 *)param->test_data_buf;
	} else {
		test_result = (u16 *)kmalloc(tsdata_frame_size, GFP_KERNEL);
		if (test_result == NULL) {
			TS_LOG_ERR("Allocate test result buffer failed");
			ret = -ENOMEM;
			goto show_test_result;
		}
	}

	ret = cts_stop_device(cts_dev);
	if (ret) {
		TS_LOG_ERR("Stop device failed %d", ret);
		goto free_mem;
	}

	cts_lock_device(cts_dev);

	ret = prepare_test(cts_dev);
	if (ret) {
		TS_LOG_ERR("Prepare test failed %d", ret);
		goto unlock_device;
	}

	TS_LOG_INFO("Test short to GND");

	ret = cts_sram_readb(cts_dev, 0xE8, &feature_ver);
	if (ret) {
		TS_LOG_ERR("Read firmware feature version failed %d", ret);
		goto post_test;
	}
	TS_LOG_INFO("Feature version: %u", feature_ver);

	if (feature_ver > 0) {
		ret = set_short_test_type(cts_dev, CTS_SHORT_TEST_UNDEFINED);
		if (ret) {
			TS_LOG_ERR("Set short test type to UNDEFINED failed %d", ret);
			goto post_test;
		}

		ret = set_fw_test_type(cts_dev, CTS_TEST_SHORT);
		if (ret) {
			TS_LOG_ERR("Set test type to SHORT failed %d", ret);
			goto post_test;
		}

		ret = set_fw_work_mode(cts_dev, CTS_FIRMWARE_WORK_MODE_TEST);
		if (ret) {
			TS_LOG_ERR("Set firmware work mode to WORK_MODE_TEST failed %d",
				ret);
			goto post_test;
		}

		if (feature_ver <= 3) {
			u8 val;

			TS_LOG_INFO("Patch short test issue");

			ret = cts_hw_reg_readb(cts_dev, 0x350E2, &val);
			if (ret) {
				TS_LOG_ERR("Read 0x350E2 failed %d", ret);
				goto post_test;
			}
			if ((val & (BIT(2) | BIT(5))) != 0) {
				ret = cts_hw_reg_writeb(cts_dev, 0x350E2, val & 0xDB);
				if (ret) {
					TS_LOG_ERR("Write 0x350E2 failed %d", ret);
					goto post_test;
				}
			}
		}

		ret = set_short_test_type(cts_dev, CTS_SHORT_TEST_BETWEEN_GND);
		if (ret) {
			TS_LOG_ERR("Set short test type to SHORT_TO_GND failed %d", ret);
			goto post_test;
		}

		ret = wait_test_complete(cts_dev, 0);
		if (ret) {
			TS_LOG_ERR("Wait test complete failed %d", ret);
			goto post_test;
		}
	} else {
		ret = cts_send_command(cts_dev, CTS_CMD_RECOVERY_TX_VOL);
		if (ret) {
			TS_LOG_ERR("Send command RECOVERY_TX_VOL failed %d", ret);
			goto post_test;
		}

		ret = wait_test_complete(cts_dev, 2);
		if (ret) {
			TS_LOG_ERR("Wait test complete failed %d", ret);
			goto post_test;
		}

		// TODO: In factory mode
	}

	ret = get_test_result(cts_dev, test_result);
	if (ret) {
		TS_LOG_ERR("Read test result failed %d", ret);
		goto post_test;
	}

	if (dump_test_data_to_user) {
		*param->test_data_wr_size += tsdata_frame_size;
	}

	if (dump_test_data_to_file) {
		int r = cts_start_dump_test_data_to_file(param->test_data_filepath,
			!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE_APPEND));
		if (r) {
			TS_LOG_ERR("Start dump test data to file failed %d", r);
		}
	}

	if (dump_test_data_to_console || dump_test_data_to_file) {
		cts_dump_tsdata(cts_dev, "GND-short", test_result,
			dump_test_data_to_console);
	}

	if (driver_validate_data) {
		ret = validate_tsdata(cts_dev, "GND-short",
			test_result, param->invalid_nodes, param->num_invalid_node,
			validate_data_per_node, param->min, param->max);
		if (ret) {
			TS_LOG_ERR("Short to GND test failed %d", ret);
			if (stop_if_failed) {
				goto stop_dump_test_data_to_file;
			}
		}
	}

	if (dump_test_data_to_user) {
		test_result += num_nodes;
	}

	ret = cts_fw_reg_readb(cts_dev,
		CTS_DEVICE_FW_REG_TEST_WITH_DISPLAY_ON, &need_display_on);
	if (ret) {
		TS_LOG_ERR("Read need display on register failed %d", ret);
		goto stop_dump_test_data_to_file;
	}

	if (need_display_on == 0) {
		ret = set_display_state(cts_dev, false);
		if (ret) {
			TS_LOG_ERR("Set display state to SLEEP failed %d", ret);
			goto stop_dump_test_data_to_file;
		}
		recovery_display_state = true;
	}

	/*
	 * Short between colums
	 */
	TS_LOG_INFO("Test short between columns");

#if 0
	ret = set_fw_test_type(cts_dev, CTS_TEST_SHORT);
	if (ret) {
		TS_LOG_ERR("Set test type to SHORT failed %d", ret);
		return ret;
	}
#endif

	ret = set_short_test_type(cts_dev, CTS_SHORT_TEST_BETWEEN_COLS);
	if (ret) {
		TS_LOG_ERR("Set short test type to BETWEEN_COLS failed %d", ret);
		goto recovery_display_state;
	}

#if 0
	ret = set_fw_work_mode(cts_dev, CTS_FIRMWARE_WORK_MODE_TEST);
	if (ret) {
		TS_LOG_ERR("Set firmware work mode to WORK_MODE_TEST failed %d",
			ret);
		return ret;
	}
#endif

	if (need_display_on == 0) {
		TS_LOG_INFO("Skip first frame data");

		ret = wait_test_complete(cts_dev, 0);
		if (ret) {
			TS_LOG_ERR("Wait test complete failed %d", ret);
			goto recovery_display_state;
		}

		ret = get_test_result(cts_dev, test_result);
		if (ret) {
			TS_LOG_ERR("Read skip test result failed %d", ret);
			goto recovery_display_state;
		}

		ret = set_short_test_type(cts_dev, CTS_SHORT_TEST_BETWEEN_COLS);
		if (ret) {
			TS_LOG_ERR("Set short test type to BETWEEN_COLS failed %d",
				ret);
			goto recovery_display_state;
		}
	}

	ret = wait_test_complete(cts_dev, 0);
	if (ret) {
		TS_LOG_ERR("Wait test complete failed %d", ret);
		goto recovery_display_state;
	}

	ret = get_test_result(cts_dev, test_result);
	if (ret) {
		TS_LOG_ERR("Read test result failed %d", ret);
		goto recovery_display_state;
	}

	if (dump_test_data_to_user) {
		*param->test_data_wr_size += tsdata_frame_size;
	}

	if (dump_test_data_to_console || dump_test_data_to_file) {
		cts_dump_tsdata(cts_dev, "Col-short", test_result,
			dump_test_data_to_console);
	}

	if (driver_validate_data) {
		ret = validate_tsdata(cts_dev, "Col-short",
			test_result, param->invalid_nodes, param->num_invalid_node,
			validate_data_per_node, param->min, param->max);
		if (ret) {
			TS_LOG_ERR("Short between columns test failed %d", ret);
			if (stop_if_failed) {
				goto recovery_display_state;
			}
		}
	}

	if (dump_test_data_to_user) {
		test_result += num_nodes;
	}

	/*
	 * Short between colums
	 */
	TS_LOG_INFO("Test short between rows");

	ret = set_short_test_type(cts_dev, CTS_SHORT_TEST_BETWEEN_ROWS);
	if (ret) {
		TS_LOG_ERR("Set short test type to BETWEEN_ROWS failed %d", ret);
		goto recovery_display_state;
	}

	loopcnt = cts_dev->hwdata->num_row;
	while (loopcnt > 1) {
		ret = wait_test_complete(cts_dev, 0);
		if (ret) {
			TS_LOG_ERR("Wait test complete failed %d", ret);
			goto recovery_display_state;
		}

		ret = get_test_result(cts_dev, test_result);
		if (ret) {
			TS_LOG_ERR("Read test result failed %d", ret);
			goto recovery_display_state;
		}

		if (dump_test_data_to_user) {
			*param->test_data_wr_size += tsdata_frame_size;
		}

		if (dump_test_data_to_console || dump_test_data_to_file) {
			cts_dump_tsdata(cts_dev, "Row-short", test_result,
				dump_test_data_to_console);
		}

		if (driver_validate_data) {
			ret = validate_tsdata(cts_dev, "Row-short",
				test_result, param->invalid_nodes, param->num_invalid_node,
				validate_data_per_node, param->min, param->max);
			if (ret) {
				TS_LOG_ERR("Short between columns test failed %d", ret);
				if (stop_if_failed) {
					goto recovery_display_state;
				}
			}
		}

		if (dump_test_data_to_user) {
			test_result += num_nodes;
		}

		loopcnt += loopcnt % 2;
		loopcnt = loopcnt >> 1;
	}

recovery_display_state:
	if (recovery_display_state) {
		int r = set_display_state(cts_dev, true);
		if (r) {
			TS_LOG_ERR("Set display state to ACTIVE failed %d", r);
		}
	}

stop_dump_test_data_to_file:
	if (dump_test_data_to_file) {
		cts_stop_dump_test_data_to_file();
	}

post_test:
	post_test(cts_dev);

unlock_device:
	cts_unlock_device(cts_dev);

	cts_start_device(cts_dev);

free_mem:
	if (!dump_test_data_to_user && test_result) {
		kfree(test_result);
	}

show_test_result:
	end_time = ktime_get();
	delta_time = ktime_sub(end_time, start_time);
	if (ret > 0) {
		TS_LOG_INFO("Short test has %d nodes FAIL, ELAPSED TIME: %lldms",
			ret, ktime_to_ms(delta_time));
	} else if (ret < 0) {
		TS_LOG_INFO("Short test FAIL %d(%s), ELAPSED TIME: %lldms",
			ret, cts_strerror(ret), ktime_to_ms(delta_time));
	} else {
		TS_LOG_INFO("Short test PASS, ELAPSED TIME: %lldms",
			ktime_to_ms(delta_time));
	}


	return ret;
}

/* Return 0 success
	negative value while error occurs
	positive value means how many nodes fail */
int cts_test_open(struct cts_device *cts_dev,
	struct cts_test_param *param)
{
	bool driver_validate_data = false;
	bool validate_data_per_node = false;
	bool dump_test_data_to_user = false;
	bool dump_test_data_to_console = false;
	bool dump_test_data_to_file = false;
	int  num_nodes;
	int  tsdata_frame_size;
	int  ret;
	u16 *test_result = NULL;
	bool recovery_display_state = false;
	u8   need_display_on;
	ktime_t start_time, end_time, delta_time;

	if (cts_dev == NULL || param == NULL) {
		TS_LOG_ERR("Open test with invalid param: cts_dev: %p test param: %p",
			cts_dev, param);
		return -EINVAL;
	}

	num_nodes = cts_dev->fwdata.rows * cts_dev->fwdata.cols;
	tsdata_frame_size = 2 * num_nodes;

	driver_validate_data =
		!!(param->flags & CTS_TEST_FLAG_VALIDATE_DATA);
	validate_data_per_node =
		!!(param->flags & CTS_TEST_FLAG_VALIDATE_PER_NODE);
	dump_test_data_to_user =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_USERSPACE);
	dump_test_data_to_console =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_CONSOLE);
	dump_test_data_to_file =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE);

	TS_LOG_INFO("Open test, flags: 0x%08x,"
			   "num invalid node: %u, "
			   "test data file: '%s' buf size: %d, "
			   "drive log file: '%s' buf size: %d",
		param->flags, param->num_invalid_node,
		param->test_data_filepath, param->test_data_buf_size,
		param->driver_log_filepath, param->driver_log_buf_size);

	start_time = ktime_get();

	if (dump_test_data_to_user) {
		test_result = (u16 *)param->test_data_buf;
	} else {
		test_result = (u16 *) kmalloc(tsdata_frame_size, GFP_KERNEL);
		if (test_result == NULL) {
			TS_LOG_ERR("Allocate memory for test result faild");
			ret = -ENOMEM;
			goto show_test_result;
		}
	}

	ret = cts_stop_device(cts_dev);
	if (ret) {
		TS_LOG_ERR("Stop device failed %d", ret);
		goto free_mem;
	}

	cts_lock_device(cts_dev);

	ret = prepare_test(cts_dev);
	if (ret) {
		TS_LOG_ERR("Prepare test failed %d", ret);
		goto unlock_device;
	}

	ret = cts_fw_reg_readb(cts_dev,
		CTS_DEVICE_FW_REG_TEST_WITH_DISPLAY_ON, &need_display_on);
	if (ret) {
		TS_LOG_ERR("Read need display on register failed %d", ret);
		goto post_test;
	}

	if (need_display_on == 0) {
		ret = set_display_state(cts_dev, false);
		if (ret) {
			TS_LOG_ERR("Set display state to SLEEP failed %d", ret);
			goto post_test;
		}
		recovery_display_state = true;
	}

	ret = cts_send_command(cts_dev, CTS_CMD_RECOVERY_TX_VOL);
	if (ret) {
		TS_LOG_ERR("Recovery tx voltage failed %d", ret);
		goto recovery_display_state;
	}

	ret = set_fw_test_type(cts_dev, CTS_TEST_OPEN);
	if (ret) {
		TS_LOG_ERR("Set test type to OPEN_TEST failed %d", ret);
		goto recovery_display_state;
	}

	ret = set_fw_work_mode(cts_dev, CTS_FIRMWARE_WORK_MODE_TEST);
	if (ret) {
		TS_LOG_ERR("Set firmware work mode to WORK_MODE_TEST failed %d",
			ret);
		goto recovery_display_state;
	}

	ret = wait_test_complete(cts_dev, 2);
	if (ret) {
		TS_LOG_ERR("Wait test complete failed %d", ret);
		goto recovery_display_state;
	}

	ret = get_test_result(cts_dev, test_result);
	if (ret) {
		TS_LOG_ERR("Read test result failed %d", ret);
		goto recovery_display_state;
	}

	if (dump_test_data_to_user) {
		*param->test_data_wr_size += tsdata_frame_size;
	}

	if (dump_test_data_to_file) {
		int r = cts_start_dump_test_data_to_file(param->test_data_filepath,
			!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE_APPEND));
		if (r) {
			TS_LOG_ERR("Start dump test data to file failed %d", r);
		}
	}

	if (dump_test_data_to_console || dump_test_data_to_file) {
		cts_dump_tsdata(cts_dev, "Open-circuit", test_result,
			dump_test_data_to_console);
	}

	if (dump_test_data_to_file) {
		cts_stop_dump_test_data_to_file();
	}

	if (driver_validate_data) {
		ret = validate_tsdata(cts_dev, "Open-circuit",
			test_result, param->invalid_nodes, param->num_invalid_node,
			validate_data_per_node, param->min, param->max);
	}

recovery_display_state:
	if (recovery_display_state) {
		int r = set_display_state(cts_dev, true);
		if (r) {
			TS_LOG_ERR("Set display state to ACTIVE failed %d", r);
		}
	}

post_test:
	post_test(cts_dev);

unlock_device:
	cts_unlock_device(cts_dev);

	cts_start_device(cts_dev);

free_mem:
	if (!dump_test_data_to_user && test_result) {
		kfree(test_result);
	}

show_test_result:
	end_time = ktime_get();
	delta_time = ktime_sub(end_time, start_time);
	if (ret > 0) {
		TS_LOG_INFO("Open test has %d nodes FAIL, ELAPSED TIME: %lldms",
			ret, ktime_to_ms(delta_time));
	} else if (ret < 0) {
		TS_LOG_INFO("Open test FAIL %d(%s), ELAPSED TIME: %lldms",
			ret, cts_strerror(ret), ktime_to_ms(delta_time));
	} else {
		TS_LOG_INFO("Open test PASS, ELAPSED TIME: %lldms",
			ktime_to_ms(delta_time));
	}

	return ret;
}

#ifdef CFG_CTS_HAS_RESET_PIN
int cts_test_reset_pin(struct cts_device *cts_dev, struct cts_test_param *param)
{
	ktime_t start_time, end_time, delta_time;
	int ret;

	if (cts_dev == NULL || param == NULL) {
		TS_LOG_ERR("Reset-pin test with invalid param: cts_dev: %p test param: %p",
			cts_dev, param);
		return -EINVAL;
	}

	TS_LOG_INFO("Reset-Pin test, flags: 0x%08x, "
			   "drive log file: '%s' buf size: %d",
		param->flags,
		param->driver_log_filepath, param->driver_log_buf_size);

	start_time = ktime_get();

	ret = cts_stop_device(cts_dev);
	if (ret) {
		TS_LOG_ERR("Stop device failed %d", ret);
		goto show_test_result;
	}

	cts_lock_device(cts_dev);

	cts_plat_set_reset(cts_dev->pdata, 0);
	mdelay(50);
#ifdef CONFIG_CTS_I2C_HOST
	/* Check whether device is in normal mode */
	if (cts_plat_is_i2c_online(cts_dev->pdata,
		CTS_DEV_NORMAL_MODE_I2CADDR)) {
#else
	if (cts_plat_is_normal_mode(cts_dev->pdata)) {
#endif /* CONFIG_CTS_I2C_HOST */
		ret = -EIO;
		TS_LOG_ERR("Device is alive while reset is low");
	}
	cts_plat_set_reset(cts_dev->pdata, 1);
	mdelay(50);

	{
		int r = wait_fw_to_normal_work(cts_dev);
		if (r) {
			TS_LOG_ERR("Wait fw to normal work failed %d", r);
		}
	}

#ifdef CONFIG_CTS_I2C_HOST
	/* Check whether device is in normal mode */
	if (!cts_plat_is_i2c_online(cts_dev->pdata,
		CTS_DEV_NORMAL_MODE_I2CADDR)) {
#else
	if (!cts_plat_is_normal_mode(cts_dev->pdata)) {
#endif /* CONFIG_CTS_I2C_HOST */
		ret = -EIO;
		TS_LOG_ERR("Device is offline while reset is high");
	}

#ifdef CONFIG_CTS_CHARGER_DETECT
	if (cts_is_charger_exist(cts_dev)) {
		int r = cts_set_dev_charger_attached(cts_dev, true);
		if (r) {
			TS_LOG_ERR("Set dev charger attached failed %d", r);
		}
	}
#endif /* CONFIG_CTS_CHARGER_DETECT */

#ifdef CONFIG_CTS_EARJACK_DETECT
	if (cts_dev->fwdata.supp_headphone_cable_reject &&
		cts_is_earjack_exist(cts_dev)) {
		int r = cts_set_dev_earjack_attached(cts_dev, true);
		if (r) {
			TS_LOG_ERR("Set dev earjack attached failed %d", r);
		}
	}
#endif /* CONFIG_CTS_EARJACK_DETECT */

#ifdef CONFIG_CTS_GLOVE
	if (cts_is_glove_enabled(cts_dev)) {
		cts_enter_glove_mode(cts_dev);
	}
#endif

#ifdef CFG_CTS_FW_LOG_REDIRECT
	if (cts_is_fw_log_redirect(cts_dev)) {
		cts_enable_fw_log_redirect(cts_dev);
	}
#endif

	cts_unlock_device(cts_dev);

	{
		int r = cts_start_device(cts_dev);
		if (r) {
			TS_LOG_ERR("Start device failed %d", r);
		}
	}

	if (!cts_dev->rtdata.program_mode) {
		cts_set_normal_addr(cts_dev);
	}

show_test_result:
	end_time = ktime_get();
	delta_time = ktime_sub(end_time, start_time);
	if (ret) {
		TS_LOG_INFO("Reset-Pin test FAIL %d(%s), ELAPSED TIME: %lldms",
			ret, cts_strerror(ret), ktime_to_ms(delta_time));
	} else {
		TS_LOG_INFO("Reset-Pin test PASS, ELAPSED TIME: %lldms",
			ktime_to_ms(delta_time));
	}

	return ret;
}
#endif

int cts_test_int_pin(struct cts_device *cts_dev, struct cts_test_param *param)
{
	ktime_t start_time, end_time, delta_time;
	int ret;

	if (cts_dev == NULL || param == NULL) {
		TS_LOG_ERR("Int-pin test with invalid param: cts_dev: %p test param: %p",
			cts_dev, param);
		return -EINVAL;
	}

	TS_LOG_INFO("Int-Pin test, flags: 0x%08x, "
			 "drive log file: '%s' buf size: %d",
		param->flags,
		param->driver_log_filepath, param->driver_log_buf_size);

	start_time = ktime_get();

	ret = cts_stop_device(cts_dev);
	if (ret) {
		TS_LOG_ERR("Stop device failed %d", ret);
		goto show_test_result;
	}

	cts_lock_device(cts_dev);

	ret = cts_send_command(cts_dev, CTS_CMD_WRTITE_INT_HIGH);
	if (ret) {
		TS_LOG_ERR("Send command WRTITE_INT_HIGH failed %d", ret);
		goto unlock_device;
	}
	mdelay(10);
	if (cts_plat_get_int_pin(cts_dev->pdata) == 0) {
		TS_LOG_ERR("INT pin state != HIGH");
		ret = -EFAULT;
		goto exit_int_test;
	}

	ret = cts_send_command(cts_dev, CTS_CMD_WRTITE_INT_LOW);
	if (ret) {
		TS_LOG_ERR("Send command WRTITE_INT_LOW failed %d", ret);
		goto exit_int_test;
	}
	mdelay(10);
	if (cts_plat_get_int_pin(cts_dev->pdata) != 0) {
		TS_LOG_ERR("INT pin state != LOW");
		ret = -EFAULT;
		goto exit_int_test;
	}

exit_int_test:
	{
		int r = cts_send_command(cts_dev, CTS_CMD_RELASE_INT_TEST);
		if (r) {
			TS_LOG_ERR("Send command RELASE_INT_TEST failed %d", r);
		}
	}
	mdelay(10);

unlock_device:
	cts_unlock_device(cts_dev);

	{
		int r = cts_start_device(cts_dev);
		if (r) {
			TS_LOG_ERR("Start device failed %d", r);
		}
	}

show_test_result:
	end_time = ktime_get();
	delta_time = ktime_sub(end_time, start_time);
	if (ret) {
		TS_LOG_INFO("Int-Pin test FAIL %d(%s), ELAPSED TIME: %lldms",
			ret, cts_strerror(ret), ktime_to_ms(delta_time));
	} else {
		TS_LOG_INFO("Int-Pin test PASS, ELAPSED TIME: %lldms",
			ktime_to_ms(delta_time));
	}

	return ret;
}

void cts_dump_comp_cap(struct cts_device *cts_dev, u8 *cap, bool to_console)
{
#define SPLIT_LINE_STR \
	"--------------------------------------"
#define ROW_NUM_FORMAT_STR "%2d | "
#define COL_NUM_FORMAT_STR "%3u "
#define DATA_FORMAT_STR "%4d"

	int r, c;
	u32 max, min, sum, average;
	int max_r, max_c, min_r, min_c;
	char line_buf[128];
	int count;

	max = min = cap[0];
	sum = 0;
	max_r = max_c = min_r = min_c = 0;
	for (r = 0; r < cts_dev->fwdata.rows; r++) {
		for (c = 0; c < cts_dev->fwdata.cols; c++) {
			u16 val = cap[r * cts_dev->fwdata.cols + c];

			sum += val;
			if (val > max) {
				max = val;
				max_r = r;
				max_c = c;
			} else if (val < min) {
				min = val;
				min_r = r;
				min_c = c;
			}
		}
	}
	average = sum / (cts_dev->fwdata.rows * cts_dev->fwdata.cols);

	count = 0;
	count += scnprintf(line_buf + count, sizeof(line_buf) - count,
			  " Compensate Cap MIN: [%u][%u]=%u, MAX: [%u][%u]=%u, AVG=%u",
			  min_r, min_c, min, max_r, max_c, max, average);
	if (to_console) {
		TS_LOG_INFO(SPLIT_LINE_STR);
		TS_LOG_INFO("%s", line_buf);
		TS_LOG_INFO(SPLIT_LINE_STR);
	}
	if (cts_test_data_filp) {
		cts_write_file(cts_test_data_filp, SPLIT_LINE_STR, strlen(SPLIT_LINE_STR));
		cts_write_file(cts_test_data_filp, "\n", 1);
		cts_write_file(cts_test_data_filp, line_buf, count);
		cts_write_file(cts_test_data_filp, "\n", 1);
		cts_write_file(cts_test_data_filp, SPLIT_LINE_STR, strlen(SPLIT_LINE_STR));
		cts_write_file(cts_test_data_filp, "\n", 1);
	}

	count = 0;
	count += scnprintf(line_buf + count, sizeof(line_buf) - count, "	  ");
	for (c = 0; c < cts_dev->fwdata.cols; c++) {
		count += scnprintf(line_buf + count, sizeof(line_buf) - count,
				  COL_NUM_FORMAT_STR, c);
	}
	if (to_console) {
		TS_LOG_INFO("%s", line_buf);
		TS_LOG_INFO(SPLIT_LINE_STR);
	}
	if (cts_test_data_filp) {
		cts_write_file(cts_test_data_filp, line_buf, count);
		cts_write_file(cts_test_data_filp, "\n", 1);
		cts_write_file(cts_test_data_filp, SPLIT_LINE_STR, strlen(SPLIT_LINE_STR));
		cts_write_file(cts_test_data_filp, "\n", 1);
	}

	for (r = 0; r < cts_dev->fwdata.rows; r++) {
		count = 0;
		count += scnprintf(line_buf + count, sizeof(line_buf) - count,
				  ROW_NUM_FORMAT_STR, r);
		for (c = 0; c < cts_dev->fwdata.cols; c++) {
			count += scnprintf(line_buf + count,
					  sizeof(line_buf) - count,
					  DATA_FORMAT_STR,
					  cap[r * cts_dev->fwdata.cols + c]);
		}
		if (to_console) {
			TS_LOG_INFO("%s", line_buf);
		}
		if (cts_test_data_filp) {
			cts_write_file(cts_test_data_filp, line_buf, count);
			cts_write_file(cts_test_data_filp, "\n", 1);
		}
	}

	if (to_console) {
		TS_LOG_INFO(SPLIT_LINE_STR);
	}
	if (cts_test_data_filp) {
		cts_write_file(cts_test_data_filp, SPLIT_LINE_STR, strlen(SPLIT_LINE_STR));
		cts_write_file(cts_test_data_filp, "\n", 1);
	}
#undef SPLIT_LINE_STR
#undef ROW_NUM_FORMAT_STR
#undef COL_NUM_FORMAT_STR
#undef DATA_FORMAT_STR
}

int cts_test_compensate_cap(struct cts_device *cts_dev,
	struct cts_test_param *param)
{
	bool driver_validate_data = false;
	bool validate_data_per_node = false;
	bool dump_test_data_to_user = false;
	bool dump_test_data_to_console = false;
	bool dump_test_data_to_file = false;
	int  num_nodes;
	u8 * cap = NULL;
	int  ret = 0;
	ktime_t start_time, end_time, delta_time;

	if (cts_dev == NULL || param == NULL) {
		TS_LOG_ERR("Compensate cap test with invalid param: cts_dev: %p test param: %p",
			cts_dev, param);
		return -EINVAL;
	}

	num_nodes = cts_dev->hwdata->num_row * cts_dev->hwdata->num_col;

	driver_validate_data =
		!!(param->flags & CTS_TEST_FLAG_VALIDATE_DATA);
	if (driver_validate_data) {
		validate_data_per_node =
			!!(param->flags & CTS_TEST_FLAG_VALIDATE_PER_NODE);
	}
	dump_test_data_to_user =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_USERSPACE);
	dump_test_data_to_console =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_CONSOLE);
	dump_test_data_to_file =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE);

	TS_LOG_INFO("Compensate cap test, flags: 0x%08x "
			   "num invalid node: %u, "
			   "test data file: '%s' buf size: %d, "
			   "drive log file: '%s' buf size: %d",
		param->flags, param->num_invalid_node,
		param->test_data_filepath, param->test_data_buf_size,
		param->driver_log_filepath, param->driver_log_buf_size);

	start_time = ktime_get();

	if (dump_test_data_to_user) {
		cap = (u8 *)param->test_data_buf;
	} else {
		cap = (u8 *)kzalloc(num_nodes, GFP_KERNEL);
		if (cap == NULL) {
			TS_LOG_ERR("Alloc mem for compensate cap failed");
			ret = -ENOMEM;
			goto show_test_result;
		}
	}

	/* Stop device to avoid un-wanted interrrupt */
	ret = cts_stop_device(cts_dev);
	if (ret) {
		TS_LOG_ERR("Stop device failed %d", ret);
		goto free_mem;
	}

	cts_lock_device(cts_dev);
	ret = cts_get_compensate_cap(cts_dev, cap);
	cts_unlock_device(cts_dev);
	if (ret) {
		TS_LOG_ERR("Get compensate cap failed %d", ret);
		goto start_device;
	}

	if (dump_test_data_to_user) {
		*param->test_data_wr_size = num_nodes;
	}

	if (dump_test_data_to_file) {
		int r = cts_start_dump_test_data_to_file(param->test_data_filepath,
			!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE_APPEND));
		if (r) {
			TS_LOG_ERR("Start dump test data to file failed %d", r);
		}
	}

	if (dump_test_data_to_console || dump_test_data_to_file) {
		cts_dump_comp_cap(cts_dev, cap,
			dump_test_data_to_console);
	}

	if (dump_test_data_to_file) {
		cts_stop_dump_test_data_to_file();
	}

	if (driver_validate_data) {
		ret = validate_comp_cap(cts_dev, "Compensate-Cap",
			cap, param->invalid_nodes, param->num_invalid_node,
			validate_data_per_node, param->min, param->max);
	}

start_device:
	{
		int r = cts_start_device(cts_dev);
		if (r) {
			TS_LOG_ERR("Start device failed %d", r);
		}
	}

free_mem:
	if (!dump_test_data_to_user && cap) {
		kfree(cap);
	}

show_test_result:
	end_time = ktime_get();
	delta_time = ktime_sub(end_time, start_time);
	if (ret > 0) {
		TS_LOG_INFO("Compensate-Cap test has %d nodes FAIL, ELAPSED TIME: %lldms",
			ret, ktime_to_ms(delta_time));
	} else if (ret < 0) {
		TS_LOG_INFO("Compensate-Cap test FAIL %d(%s), ELAPSED TIME: %lldms",
			ret, cts_strerror(ret), ktime_to_ms(delta_time));
	} else {
		TS_LOG_INFO("Compensate-Cap test PASS, ELAPSED TIME: %lldms",
			ktime_to_ms(delta_time));
	}

	return ret;
}

int cts_test_rawdata(struct cts_device *cts_dev,
	struct cts_test_param *param)
{
	struct cts_rawdata_test_priv_param *priv_param;
	bool driver_validate_data = false;
	bool validate_data_per_node = false;
	bool stop_test_if_validate_fail = false;
	bool dump_test_data_to_user = false;
	bool dump_test_data_to_console = false;
	bool dump_test_data_to_file = false;
	int  num_nodes;
	int  tsdata_frame_size;
	int  frame;
	u16 *rawdata = NULL;
	ktime_t start_time, end_time, delta_time;
	int  i;
	int  ret;

	if (cts_dev == NULL || param == NULL ||
		param->priv_param_size != sizeof(*priv_param) ||
		param->priv_param == NULL) {
		TS_LOG_ERR("Rawdata test with invalid param: priv param: %p size: %d",
			param->priv_param, param->priv_param_size);
		return -EINVAL;
	}

	priv_param = param->priv_param;
	if (priv_param->frames <= 0) {
		TS_LOG_INFO("Rawdata test with too little frame %u",
			priv_param->frames);
		return -EINVAL;
	}

	num_nodes = cts_dev->fwdata.rows * cts_dev->fwdata.cols;
	tsdata_frame_size = 2 * num_nodes;

	driver_validate_data =
		!!(param->flags & CTS_TEST_FLAG_VALIDATE_DATA);
	validate_data_per_node =
		!!(param->flags & CTS_TEST_FLAG_VALIDATE_PER_NODE);
	dump_test_data_to_user =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_USERSPACE);
	dump_test_data_to_console =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_CONSOLE);
	dump_test_data_to_file =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE);
	stop_test_if_validate_fail =
		!!(param->flags & CTS_TEST_FLAG_STOP_TEST_IF_VALIDATE_FAILED);

	TS_LOG_INFO("Rawdata test, flags: 0x%08x, frames: %d, "
		"num invalid node: %u, "
		"test data file: '%s' buf size: %d, "
		"drive log file: '%s' buf size: %d",
		param->flags, priv_param->frames, param->num_invalid_node,
		param->test_data_filepath, param->test_data_buf_size,
		param->driver_log_filepath, param->driver_log_buf_size);

	start_time = ktime_get();

	if (dump_test_data_to_user) {
		rawdata = (u16 *)param->test_data_buf;
	} else {
		rawdata = (u16 *)kmalloc(tsdata_frame_size, GFP_KERNEL);
		if (rawdata == NULL) {
			TS_LOG_ERR("Allocate memory for rawdata failed");
			ret = -ENOMEM;
			goto show_test_result;
		}
	}

	/* Stop device to avoid un-wanted interrrupt */
	ret = cts_stop_device(cts_dev);
	if (ret) {
		TS_LOG_ERR("Stop device failed %d", ret);
		goto free_mem;
	}

	cts_lock_device(cts_dev);

	for (i = 0; i < 5; i++) {
		int r;
		u8 val;
		r = cts_enable_get_rawdata(cts_dev);
		if (r) {
			TS_LOG_ERR("Enable get tsdata failed %d", r);
			continue;
		}
		mdelay(1);
		r = cts_fw_reg_readb(cts_dev, 0x12, &val);
		if (r) {
			TS_LOG_ERR("Read enable get tsdata failed %d", r);
			continue;
		}
		if (val != 0) {
			break;
		}
	}

	if (i >= 5) {
		TS_LOG_ERR("Enable read tsdata failed");
		ret = -EIO;
		goto unlock_device;
	}

	if (dump_test_data_to_file) {
		int r = cts_start_dump_test_data_to_file(param->test_data_filepath,
			!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE_APPEND));
		if (r) {
			TS_LOG_ERR("Start dump test data to file failed %d", r);
		}
	}

	for (frame = 0; frame < priv_param->frames; frame++) {
		bool data_valid = false;
		int  r;

		//if (cts_dev.fwdata.monitor_mode) {
			r = cts_send_command(cts_dev, CTS_CMD_QUIT_GESTURE_MONITOR);
			if (r) {
				TS_LOG_ERR("Send CMD_QUIT_GESTURE_MONITOR failed %d", r);
			}
		//}

		for (i = 0; i < 3; i++) {
			r = cts_get_rawdata(cts_dev, rawdata);
			if (r) {
				TS_LOG_ERR("Get rawdata failed %d", r);
				mdelay(30);
			} else {
				data_valid = true;
				break;
			}
		}

		if (!data_valid) {
			ret = -EIO;
			break;
		}

		if (dump_test_data_to_user) {
			*param->test_data_wr_size += tsdata_frame_size;
		}

		if (dump_test_data_to_console || dump_test_data_to_file) {
			cts_dump_tsdata(cts_dev, "Rawdata", rawdata,
				dump_test_data_to_console);
		}

		if (driver_validate_data) {
			ret = validate_tsdata(cts_dev,
				"Rawdata", rawdata,
				param->invalid_nodes, param->num_invalid_node,
				validate_data_per_node, param->min, param->max);
			if (ret) {
				TS_LOG_ERR("Rawdata test failed %d", ret);
				if (stop_test_if_validate_fail) {
					break;
				}
			}
		}

		if (dump_test_data_to_user) {
			rawdata += num_nodes;
		}
	}

	if (dump_test_data_to_file) {
		cts_stop_dump_test_data_to_file();
	}

	for (i = 0; i < 5; i++) {
		int r = cts_disable_get_rawdata(cts_dev);
		if (r) {
			TS_LOG_ERR("Disable get rawdata failed %d", r);
			continue;
		} else {
			break;
		}
	}

unlock_device:
	cts_unlock_device(cts_dev);

	{
		int r = cts_start_device(cts_dev);
		if (r) {
			TS_LOG_ERR("Start device failed %d", r);
		}
	}

free_mem:
	if (!dump_test_data_to_user && rawdata != NULL) {
		kfree(rawdata);
	}

show_test_result:
	end_time = ktime_get();
	delta_time = ktime_sub(end_time, start_time);
	if (ret > 0) {
		TS_LOG_INFO("Rawdata test has %d nodes FAIL, ELAPSED TIME: %lldms",
			ret, ktime_to_ms(delta_time));
	} else if (ret < 0) {
		TS_LOG_INFO("Rawdata test FAIL %d(%s), ELAPSED TIME: %lldms",
			ret, cts_strerror(ret), ktime_to_ms(delta_time));
	} else {
		TS_LOG_INFO("Rawdata test PASS, ELAPSED TIME: %lldms",
			ktime_to_ms(delta_time));
	}

	return ret;
}

int cts_test_noise(struct cts_device *cts_dev,
		struct cts_test_param *param)
{
	struct cts_noise_test_priv_param *priv_param;
	bool driver_validate_data = false;
	bool validate_data_per_node = false;
	bool dump_test_data_to_user = false;
	bool dump_test_data_to_console = false;
	bool dump_test_data_to_file = false;
	int  num_nodes;
	int  tsdata_frame_size;
	int  frame;
	u16 *buffer = NULL;
	int  buf_size = 0;
	u16 *curr_rawdata = NULL;
	u16 *max_rawdata = NULL;
	u16 *min_rawdata = NULL;
	u16 *noise = NULL;
	bool first_frame = true;
	bool data_valid = false;
	ktime_t start_time, end_time, delta_time;
	int  i;
	int  ret;

	if (cts_dev == NULL || param == NULL ||
		param->priv_param_size != sizeof(*priv_param) ||
		param->priv_param == NULL) {
		TS_LOG_ERR("Noise test with invalid param: priv param: %p size: %d",
			param->priv_param, param->priv_param_size);
		return -EINVAL;
	}

	priv_param = param->priv_param;
	if (priv_param->frames < 2) {
		TS_LOG_ERR("Noise test with too little frame %u",
			priv_param->frames);
		return -EINVAL;
	}

	num_nodes = cts_dev->fwdata.rows * cts_dev->fwdata.cols;
	tsdata_frame_size = 2 * num_nodes;

	driver_validate_data =
		!!(param->flags & CTS_TEST_FLAG_VALIDATE_DATA);
	validate_data_per_node =
		!!(param->flags & CTS_TEST_FLAG_VALIDATE_PER_NODE);
	dump_test_data_to_user =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_USERSPACE);
	dump_test_data_to_console =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_CONSOLE);
	dump_test_data_to_file =
		!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE);

	TS_LOG_INFO("Noise test, flags: 0x%08x, frames: %d, "
			   "num invalid node: %u, "
			   "test data file: '%s' buf size: %d, "
			   "drive log file: '%s' buf size: %d",
		param->flags, priv_param->frames, param->num_invalid_node,
		param->test_data_filepath, param->test_data_buf_size,
		param->driver_log_filepath, param->driver_log_buf_size);

	start_time = ktime_get();

	buf_size = (driver_validate_data ? 4 : 1) * tsdata_frame_size;
	buffer = (u16 *)kmalloc(buf_size, GFP_KERNEL);
	if (buffer == NULL) {
		TS_LOG_ERR("Alloc mem for touch data failed");
		ret = -ENOMEM;
		goto show_test_result;
	}

	curr_rawdata = buffer;
	if (driver_validate_data) {
		max_rawdata = curr_rawdata + 1 * num_nodes;
		min_rawdata = curr_rawdata + 2 * num_nodes;
		noise	   = curr_rawdata + 3 * num_nodes;
	}

	/* Stop device to avoid un-wanted interrrupt */
	ret = cts_stop_device(cts_dev);
	if (ret) {
		TS_LOG_ERR("Stop device failed %d", ret);
		goto free_mem;
	}

	cts_lock_device(cts_dev);

	for (i = 0; i < 5; i++) {
		int r;
		u8 val;
		r = cts_enable_get_rawdata(cts_dev);
		if (r) {
			TS_LOG_ERR("Enable get ts data failed %d", r);
			continue;
		}
		mdelay(1);
		r = cts_fw_reg_readb(cts_dev, 0x12, &val);
		if (r) {
			TS_LOG_ERR("Read enable get ts data failed %d", r);
			continue;
		}
		if (val != 0) {
			break;
		}
	}

	if (i >= 5) {
		TS_LOG_ERR("Enable read tsdata failed");
		ret = -EIO;
		goto unlock_device;
	}

	if (dump_test_data_to_file) {
		int r = cts_start_dump_test_data_to_file(param->test_data_filepath,
			!!(param->flags & CTS_TEST_FLAG_DUMP_TEST_DATA_TO_FILE_APPEND));
		if (r) {
			TS_LOG_ERR("Start dump test data to file failed %d", r);
		}
	}

	msleep(50);

	for (frame = 0; frame < priv_param->frames; frame++) {
		int r;

		r = cts_send_command(cts_dev, CTS_CMD_QUIT_GESTURE_MONITOR);
		if (r) {
			TS_LOG_ERR("send quit gesture monitor failed %d", r);
			// Ignore this error
		}

		for (i = 0; i < 3; i++) {
			r = cts_get_rawdata(cts_dev, curr_rawdata);
			if (r) {
				TS_LOG_ERR("Get rawdata failed %d", r);
				mdelay(30);
			} else {
				break;
			}
		}

		if (i >= 3) {
			TS_LOG_ERR("Read rawdata failed");
			ret = -EIO;
			goto disable_get_tsdata;
		}

		if (dump_test_data_to_console || dump_test_data_to_file) {
			cts_dump_tsdata(cts_dev, "Noise-rawdata", curr_rawdata,
				dump_test_data_to_console);
		}

		if (dump_test_data_to_user) {
			memcpy(param->test_data_buf + frame * tsdata_frame_size,
				curr_rawdata, tsdata_frame_size);
			*param->test_data_wr_size += tsdata_frame_size;
		}

		if (driver_validate_data) {
			if (unlikely(first_frame)) {
				memcpy(max_rawdata, curr_rawdata, tsdata_frame_size);
				memcpy(min_rawdata, curr_rawdata, tsdata_frame_size);
				first_frame = false;
			} else {
				for (i = 0; i < num_nodes; i++) {
					if (curr_rawdata[i] > max_rawdata[i]) {
						max_rawdata[i] = curr_rawdata[i];
					} else if (curr_rawdata[i] < min_rawdata[i]) {
						min_rawdata[i] = curr_rawdata[i];
					}
				}
			}
		}
	}

	data_valid = true;

disable_get_tsdata:
	for (i = 0; i < 5; i++) {
		int r = cts_disable_get_rawdata(cts_dev);
		if (r) {
			TS_LOG_ERR("Disable get rawdata failed %d\n", r);
			continue;
		} else {
			break;
		}
	}

	if (driver_validate_data && data_valid) {
		for (i = 0; i < num_nodes; i++) {
			noise[i] = max_rawdata[i] - min_rawdata[i];
		}
		memcpy(param->test_data_buf + (frame + 0) * tsdata_frame_size,
			noise, tsdata_frame_size);
		memcpy(param->test_data_buf + (frame + 1) * tsdata_frame_size,
			max_rawdata, tsdata_frame_size);
		memcpy(param->test_data_buf + (frame + 2) * tsdata_frame_size,
			min_rawdata, tsdata_frame_size);
		if (dump_test_data_to_console || dump_test_data_to_file) {
			cts_dump_tsdata(cts_dev, "Noise", noise,
				dump_test_data_to_console);
			cts_dump_tsdata(cts_dev, "Rawdata MAX", max_rawdata,
				dump_test_data_to_console);
			cts_dump_tsdata(cts_dev, "Rawdata MIN", min_rawdata,
				dump_test_data_to_console);
		}

		ret = validate_tsdata(cts_dev, "Noise test",
			noise, param->invalid_nodes, param->num_invalid_node,
			validate_data_per_node, param->min, param->max);
	}
	if (dump_test_data_to_file) {
		cts_stop_dump_test_data_to_file();
	}

unlock_device:
	cts_unlock_device(cts_dev);

	{
		int r = cts_start_device(cts_dev);
		if (r) {
		TS_LOG_ERR("Start device failed %d\n", r);
		}
	}

free_mem:
	if (buffer) {
		kfree(buffer);
	}

show_test_result:
	end_time = ktime_get();
	delta_time = ktime_sub(end_time, start_time);
	if (ret > 0) {
		TS_LOG_INFO("Noise test has %d nodes FAIL, ELAPSED TIME: %lldms",
			ret, ktime_to_ms(delta_time));
	} else if (ret < 0) {
		TS_LOG_INFO("Noise test FAIL %d(%s), ELAPSED TIME: %lldms",
			ret, cts_strerror(ret), ktime_to_ms(delta_time));
	} else {
		TS_LOG_INFO("Noise test PASS, ELAPSED TIME: %lldms",
			ktime_to_ms(delta_time));
	}

	return ret;
}

