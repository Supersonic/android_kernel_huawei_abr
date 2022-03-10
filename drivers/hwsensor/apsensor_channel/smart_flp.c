/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: Contexthub flp driver.
 * Create: 2017-02-18
 */
#include "smart_flp.h"
#include <clocksource/arm_arch_timer.h>
#include <linux/delay.h>
#include <linux/compat.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/init.h>
#include <net/genetlink.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/pm_wakeup.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/workqueue.h>
#include <securec.h>
#include "smart_softtimer.h"
#include <ap_sensor.h>
#include <ap_sensor_route.h>

/*lint -e750 -esym(750,*) */
#define FLP_BATCHING        (BIT(0))
#define FLP_GEOFENCE        (BIT(1))
#define FLP_CELLFENCE       (BIT(2))
#define FLP_CELLTRAJECTORY  (BIT(3))
#define FLP_OFFLINEDB       (BIT(4))
#define FLP_WIFIFENCE       (BIT(5))
#define FLP_CELLBATCHING    (BIT(7))
#define FLP_DIAG            (BIT(9))
#define FLP_ERR_TIMEOUT     222

#define IOMCU_APP_FLP       (FLP_BATCHING|FLP_GEOFENCE|FLP_CELLFENCE|FLP_CELLTRAJECTORY|FLP_WIFIFENCE|FLP_CELLBATCHING|FLP_DIAG)
#define FLP_MAX_GET_DATA   (1024 * 200)
#define CONFIG_SMART_FLP_DIAG
#ifdef CONFIG_SMART_FLP_DIAG
#define FLP_DIAG_MAX_CMD_LEN 64
#endif

#if defined(DDR_SHMEMEXT_ADDR_AP) && defined(DDR_SHMEMEXT_SIZE)
#define OFFLINE_INDOOR_MMAP_ADDR  (DDR_SHMEMEXT_ADDR_AP + DDR_SHMEMEXT_SIZE - OFFLINE_INDOOR_MMAP_SIZE)
#define OFFLINE_INDOOR_MMAP_SIZE  (0x80000)
#define OFFLINE_CELL_MMAP_SIZE    (0x100000)
#define MMAP_OFFSET_CHECK_BIT     (8)
#endif
enum {
	OFFLINE_TYPE_INDOOR = 1,
	OFFLINE_TYPE_CELL,
	OFFLINE_TYPE_INVALID
};

enum {
	LOC_STATUS_STOP,
	LOC_STATUS_START,
	LOC_STATUS_OFF,
	LOC_STATUS_ON
};

/************************* piling function start ************************/
#define FLP_WRITE_MAX                         1024
#define FLP_IOCTL_MAX                         960
#define FLP_IOCTL_PKG_HEADER                  4
#define FLP_CMD_REQUIRE_RESP                  1
#define CONFIG_SMART_FLP_GEOFENCE
#define CONFIG_SMART_FLP_BATCHING
#define CONFIG_SMART_FLP_CELLFENCE
#define CONFIG_SMART_FLP_WIFIFENCE

/* inputhub_wrapper_pack_and_send_cmd max data len
 * contains only the data length, excluding the header information and subcommands
 */
#define INPUTHUB_WRAPPER_MAX_LEN (FLP_IOCTL_MAX - sizeof(struct pkt_header) - sizeof(u32))

enum obj_cmd {
    CMD_CMN_OPEN_REQ = 0x01,
    CMD_CMN_OPEN_RESP,
    CMD_CMN_CLOSE_REQ,
    CMD_CMN_CLOSE_RESP,
    CMD_CMN_SET_INTERVAL_REQ,
    CMD_CMN_SET_INTERVAL_RESP,
    CMD_CMN_CONFIG_REQ,
    CMD_CMN_CONFIG_RESP,
    CMD_CMN_FLUSH_REQ,
    CMD_CMN_FLUSH_RESP,
    CMD_CMN_RECOVERY_RESP,

    CMD_DATA_REQ = 0x1f,
    CMD_DATA_RESP,

    CMD_SET_FAULT_TYPE_REQ,//0x21
    CMD_SET_FAULT_TYPE_RESP,
    CMD_SET_FAULT_ADDR_REQ,
    CMD_SET_FAULT_ADDR_RESP,

    /*SPI*/
    CMD_SPI_BAUD_REQ,//0x25
    CMD_SPI_BAUD_RESP,
    CMD_SPI_TRANS_REQ,
    CMD_SPI_TRANS_RESP,

    /*I2C*/
    CMD_I2C_TRANS_REQ,//0x29
    CMD_I2C_TRANS_RESP,

    /*system status*/
    CMD_SYS_STATUSCHANGE_REQ,//0x2b
    CMD_SYS_STATUSCHANGE_RESP,
    CMD_SYS_DYNLOAD_REQ,
    CMD_SYS_DYNLOAD_RESP,
    CMD_SYS_HEARTBEAT_REQ,
    CMD_SYS_HEARTBEAT_RESP,
    CMD_SYS_LOG_LEVEL_REQ,
    CMD_SYS_LOG_LEVEL_RESP,
    CMD_SYS_CTS_RESTRICT_MODE_REQ,
    CMD_SYS_CTS_RESTRICT_MODE_RESP,
    CMD_SYS_STATUSREADY_REQ,
    CMD_SYS_STATUSREADY_RESP,

    /*LOG*/
    CMD_LOG_REPORT_REQ, //0x37
    CMD_LOG_REPORT_RESP,
    CMD_LOG_CONFIG_REQ,
    CMD_LOG_CONFIG_RESP,
    CMD_LOG_POWER_REQ,
    CMD_LOG_POWER_RESP,

    /*SHAREMEM*/
    CMD_SHMEM_AP_RECV_REQ,//0x3d
    CMD_SHMEM_AP_RECV_RESP,
    CMD_SHMEM_AP_SEND_REQ,
    CMD_SHMEM_AP_SEND_RESP,

    /* SHELL_DBG */
    CMD_SHELL_DBG_REQ,//0x41
    CMD_SHELL_DBG_RESP,

    /* LoadMonitor */
    CMD_READ_AO_MONITOR_SENSOR,//0x43
    CMD_READ_AO_MONITOR_SENSOR_RESP,

    /* DATA_COMM */
    CMD_DATA_REPLAY_DATA_READY_REQ,//0x45
    CMD_DATA_REPLAY_DATA_READY_RESP,
    CMD_DATA_REPLAY_DATA_HANDLED_REQ,
    CMD_DATA_REPLAY_DATA_HANDLED_RESP,

    /* BIG DATA */
    CMD_BIG_DATA_REQUEST_DATA,//0x49
    CMD_BIG_DATA_REQUEST_DATA_RESP,
    CMD_BIG_DATA_SEND_TO_AP,
    CMD_BIG_DATA_SEND_TO_AP_RESP,  //0x4c

    /* tag sys for hall status*/
    CMD_SYS_HALL_STATUS_REQ = 0x53,//0x53
    CMD_SYS_HALL_STATUS_RESP = 0x54,//0x54

    /* tag modem for cell info*/
    CMD_MODEM_CELL_INFO_REQ = 0x3f,
    CMD_MODEM_CELL_INFO_RESP,
    CMD_MODEM_REBOOT_NOTIFY_REQ,
    CMD_MODEM_REBOOT_NOTIFY_RESP,

    /* tag modem for phonecall status*/
    CMD_MODEM_PHONECALL_INFO_REQ = 0x55,//0x55
    CMD_MODEM_PHONECALL_INFO_RESP = 0x56,//0x56

    /*I3C*/
    CMD_I3C_TRANS_REQ = 0x57,
    CMD_I3C_TRANS_RESP,

    /* tag als for ud*/
    CMD_ALS_RUN_STOP_PARA_REQ = 0x59,
    CMD_ALS_RUN_STOP_PARA_REsp = 0x5A,

    /*log buff*/
    CMD_LOG_SER_REQ = 0xf1,
    CMD_LOG_USEBUF_REQ,
    CMD_LOG_BUFF_ALERT,
    CMD_LOG_BUFF_FLUSH,
    CMD_LOG_BUFF_FLUSHP,
    CMD_EXT_LOG_FLUSH,
    CMD_EXT_LOG_FLUSHP,

    CMD_SYS_SYNC_TIME_REQ = 0xf8,
    CMD_SYS_SYNC_TIME_RESP,

    /*max cmd*/
    CMD_ERR_RESP = 0xfe,
};

enum obj_sub_cmd {
    SUB_CMD_NULL_REQ = 0x0,
    SUB_CMD_SELFCALI_REQ = 0x01, //01,
    SUB_CMD_SET_PARAMET_REQ,//02
    SUB_CMD_SET_OFFSET_REQ,//03
    SUB_CMD_SELFTEST_REQ,//04
    SUB_CMD_CALIBRATE_DATA_REQ,//05
    SUB_CMD_SET_SLAVE_ADDR_REQ,//06
    SUB_CMD_SET_RESET_PARAM_REQ,//07
    SUB_CMD_ADDITIONAL_INFO,//08
    SUB_CMD_FW_DLOAD_REQ,//09
    SUB_CMD_FLUSH_REQ,//10
    SUB_CMD_SET_ADD_DATA_REQ,//11
    SUB_CMD_SET_DATA_TYPE_REQ,//12
    SUB_CMD_SET_DATA_MODE,//13
    SUB_CMD_SET_TP_COORDINATE,//14
    SUB_CMD_SET_WRITE_NV_ATTER_SALE = SUB_CMD_SET_TP_COORDINATE,

    /*als*/
    SUB_CMD_SET_ALS_PA = 0x20,
    SUB_CMD_UPDATE_BL_LEVEL,
    SUB_CMD_UPDATE_RGB_DATA,
    SUB_CMD_GET_FACTORY_PARA,
    SUB_CMD_CHANGE_DC_STATUS,
    SUB_CMD_CHANGE_ALWAYS_ON_STATUS,
    SUB_CMD_UPDATE_NOISE_DATA,
    SUB_CMD_ALS_LCD_FREQ_REQ,
    SUB_CMD_SET_ALS_UD_CALIB_DATA,
    SUB_CMD_UPDATE_AOD_STATUS,

    //gyro
    SUB_CMD_GYRO_OIS_REQ = 0x20,
    SUB_CMD_GYRO_TMP_OFFSET_REQ,

    /*vibrator*/
    SUB_CMD_VIBRATOR_SINGLE_REQ = 0x20,
    SUB_CMD_VIBRATOR_REPEAT_REQ,
    SUB_CMD_VIBRATOR_ON_REQ,
    SUB_CMD_VIBRATOR_SET_AMPLITUDE_REQ,

    /*motion*/
    SUB_CMD_MOTION_HORIZONTAL_PICKUP_REQ = 0x23,

    //fingerprint
    SUB_CMD_FINGERPRINT_OPEN_REQ = 0x20,
    SUB_CMD_FINGERPRINT_CLOSE_REQ,
    SUB_CMD_FINGERPRINT_CONFIG_SENSOR_DATA_REQ,

    //finger sense
    SUB_CMD_ACCEL_FINGERSENSE_ENABLE_REQ = 0x20,
    SUB_CMD_ACCEL_FINGERSENSE_CLOSE_REQ,
    SUB_CMD_ACCEL_FINGERSENSE_REQ_DATA_REQ,
    SUB_CMD_ACCEL_FINGERSENSE_DATA_REPORT,
    SUB_CMD_TOUCH_FINGERSENSE_MAIN_SCREEN,
    SUB_CMD_TOUCH_FINGERSENSE_ASSIT_SCREEN,

    /*tag pdr*/
    SUB_CMD_FLP_PDR_START_REQ = 0x20,
    SUB_CMD_FLP_PDR_STOP_REQ,
    SUB_CMD_FLP_PDR_WRITE_REQ,
    SUB_CMD_FLP_PDR_UPDATE_REQ,
    SUB_CMD_FLP_PDR_FLUSH_REQ,
    SUB_CMD_FLP_PDR_UNRELIABLE_REQ,
    SUB_CMD_FLP_PDR_STEPCFG_REQ,

    /*tag ar*/
    SUB_CMD_FLP_AR_START_REQ = 0x20,
    SUB_CMD_FLP_AR_CONFIG_REQ,
    SUB_CMD_FLP_AR_STOP_REQ,
    SUB_CMD_FLP_AR_UPDATE_REQ,
    SUB_CMD_FLP_AR_FLUSH_REQ,
    SUB_CMD_FLP_AR_GET_STATE_REQ,
    SUB_CMD_FLP_AR_GET_STATE_RESP,
    SUB_CMD_CELL_INFO_DATA_REQ = 0x29,

    /*tag environment*/
    SUB_CMD_ENVIRONMENT_INIT_REQ = 0x20,
    SUB_CMD_ENVIRONMENT_ENABLE_REQ,
    SUB_CMD_ENVIRONMENT_DISABLE_REQ,
    SUB_CMD_ENVIRONMENT_EXIT_REQ,
    SUB_CMD_ENVIRONMENT_DATABASE_REQ,
    SUB_CMD_ENVIRONMENT_GET_STATE_REQ,


    /*tag flp*/
    SUB_CMD_FLP_START_BATCHING_REQ = 0x20,
    SUB_CMD_FLP_STOP_BATCHING_REQ,
    SUB_CMD_FLP_UPDATE_BATCHING_REQ,
    SUB_CMD_FLP_GET_BATCHED_LOCATION_REQ,
    SUB_CMD_FLP_FLUSH_LOCATION_REQ,
    SUB_CMD_FLP_INJECT_LOCATION_REQ, //0x25-37
    SUB_CMD_FLP_RESET_REQ,
    SUB_CMD_FLP_RESET_RESP,
    SUB_CMD_FLP_GET_BATCH_SIZE_REQ,
    SUB_CMD_FLP_BATCH_PUSH_GNSS_REQ,

    SUB_CMD_FLP_ADD_GEOF_REQ,   //0x2A-42
    SUB_CMD_FLP_REMOVE_GEOF_REQ,
    SUB_CMD_FLP_MODIFY_GEOF_REQ,
    SUB_CMD_FLP_LOCATION_UPDATE_REQ,
    SUB_CMD_FLP_GEOF_TRANSITION_REQ,
    SUB_CMD_FLP_GEOF_MONITOR_STATUS_REQ, //0x2F-47
    SUB_CMD_FLP_GEOF_GET_TRANSITION_REQ,

    SUB_CMD_FLP_CELLFENCE_ADD_REQ,
    SUB_CMD_FLP_CELLFENCE_OPT_REQ,
    SUB_CMD_FLP_CELLFENCE_TRANSITION_REQ,
    SUB_CMD_FLP_CELLFENCE_INJECT_RESULT_REQ,
    SUB_CMD_FLP_CELLTRAJECTORY_CFG_REQ, // 0x35
    SUB_CMD_FLP_CELLTRAJECTORY_REQUEST_REQ,
    SUB_CMD_FLP_CELLTRAJECTORY_REPORT_REQ,
    SUB_CMD_FLP_CELLDB_LOCATION_REPORT_REQ,

    SUB_CMD_FLP_COMMON_STOP_SERVICE_REQ, // 0x39
    SUB_CMD_FLP_COMMON_WIFI_CFG_REQ,

    SUB_CMD_FLP_GEOF_GET_LOCATION_REQ, // 0x3b
    SUB_CMD_FLP_GEOF_GET_LOCATION_REPORT_REQ,
    SUB_CMD_FLP_ADD_WIFENCE_REQ,
    SUB_CMD_FLP_REMOVE_WIFENCE_REQ,
    SUB_CMD_FLP_PAUSE_WIFENCE_REQ,
    SUB_CMD_FLP_RESUME_WIFENCE_REQ,  // 0x40
    SUB_CMD_FLP_GET_WIFENCE_STATUS_REQ,
    SUB_CMD_FLP_WIFENCE_TRANSITION_REQ,
    SUB_CMD_FLP_WIFENCE_STATUS_REQ,

    SUB_CMD_FLP_COMMON_DEBUG_CONFIG_REQ,
    SUB_CMD_FLP_CELL_CELLBATCHING_CFG_REQ,
    SUB_CMD_FLP_CELL_CELLBATCHING_REQ,
    SUB_CMD_FLP_CELL_CELLBATCHING_REPORT_REQ,
    SUB_CMD_FLP_DIAG_SEND_CMD_REQ,
    SUB_CMD_FLP_DIAG_DATA_REPORT_REQ,
    SUB_CMD_FLP_BATCHING_MULT_LASTLOCATION_REQ, // 0x4a
    SUB_CMD_FLP_BATCHING_MULT_FLUSH_REQ,

    SUB_CMD_FLP_GEOF_GET_SIZE_REQ,
    SUB_CMD_FLP_GEOF_GET_SIZE_REPORT_REQ,
    SUB_CMD_FLP_CELLFENCE_GET_SIZE_REQ,
    SUB_CMD_FLP_CELLFENCE_GET_SIZE_REPORT_REQ,
    SUB_CMD_FLP_WIFENCE_GET_SIZE_REQ,
    SUB_CMD_FLP_WIFENCE_GET_SIZE_REPORT_REQ,
    SUB_CMD_FLP_WIFIDB_LOCATION_REPORT_REQ,
    SUB_CMD_FLP_REQUEST_GPS_REQ,

    //Always On Display
    SUB_CMD_AOD_START_REQ = 0x20,
    SUB_CMD_AOD_STOP_REQ,
    SUB_CMD_AOD_START_UPDATING_REQ,
    SUB_CMD_AOD_END_UPDATING_REQ,
    SUB_CMD_AOD_SET_TIME_REQ,
    SUB_CMD_AOD_SET_DISPLAY_SPACE_REQ,
    SUB_CMD_AOD_SETUP_REQ,
    SUB_CMD_AOD_DSS_ON_REQ,
    SUB_CMD_AOD_DSS_OFF_REQ,

    // rpc

    SUB_CMD_RPC_START_REQ = 0x20,
    SUB_CMD_RPC_STOP_REQ,
    SUB_CMD_RPC_UPDATE_REQ,
    SUB_CMD_RPC_LIBHAND_REQ,

    /* KB */
    CMD_KB_OPEN_REQ = 0x20,
    CMD_KB_OPEN_RESP,
    CMD_KB_CLOSE_REQ,
    CMD_KB_CLOSE_RESP,
    CMD_KB_REPORT_REQ,
    CMD_KB_REPORT_RESP,
    CMD_KB_EVENT_REQ,
    CMD_KB_EVENT_RESP,
    CMD_KB_HUB_MCU_READY,

    // posture
    SUB_CMD_POSTURE_START_REQ = 0x20,

    SUB_CMD_MAX = 0xff,

};

/*lint +e750 +esym(750,*) */

struct flp_device_t {
	struct list_head        list;
	unsigned int            service_type;
	struct mutex            lock;
	unsigned int            denial_sevice;
	struct mutex            recovery_lock;
	struct completion       shmem_completion;
	int                     shmem_resp_errno;
	struct workqueue_struct *wq;
};

struct flp_data_buf_t {
	char                  *data_buf;
	unsigned int          buf_size;
	unsigned int          read_index;
	unsigned int          write_index;
	unsigned int          data_count;
};

/*struct write_info {
	int tag;
 	int cmd;
 	const void *wr_buf;
 	int wr_len;
 	int app_tag;
};*/

struct flp_port_t {
	struct list_head        list;
	unsigned int            channel_type;
	struct softtimer_list   sleep_timer;
	struct batching_config_t       gps_batching_config;
	unsigned int            portid;
	unsigned long           total_count;
	struct work_struct      work;
	unsigned int            work_para;
	unsigned int            need_awake;
	unsigned int            need_report;
	struct wakeup_source    wlock;
	unsigned int            need_hold_wlock;
	struct loc_source_status       location_status;
};

struct flp_report_data_handle_work {
	struct work_struct worker;
	void *data;
};

struct flp_device_t  g_flp_dev;
static unsigned int g_flp_shmem_transid;
struct read_info g_rd_info = {0};
bool g_has_rd = false;

#if (defined CONFIG_SMART_FLP_GEOFENCE) || (defined CONFIG_SMART_FLP_WIFIFENCE) || (defined CONFIG_SMART_FLP_CELLFENCE)
static int flp_fence_shmem(struct flp_port_t *flp_port, struct hal_config_t *config,
	unsigned short int shmem_cmd, unsigned int type);
#endif

/*lint -e785 */
static struct genl_family flp_genl_family = {
#if (KERNEL_VERSION(4, 10, 0) > LINUX_VERSION_CODE)
	.id         = GENL_ID_GENERATE,
#endif
	.name       = FLP_GENL_NAME,
	.version    = TASKFLP_GENL_VERSION,
	.maxattr    = FLP_GENL_ATTR_MAX,
};
/*lint +e785 */
/*lint +e655*/
static int flp_genlink_checkin(struct flp_port_t *flp_port, unsigned int count, unsigned char cmd_type)
{
	if (flp_port == NULL) {
		pr_err("[%s] struct flp_port NULL\n", __func__);
		return -EINVAL;
	}

	if (flp_port->portid == 0) {
		pr_err("[%s]no portid error\n", __func__);
		return -EBUSY;
	}

	return 0;
}

/*lint -e826 -e834 -e776*/

/*
 * Generate netlink data packet and send to HAL
 */
static int flp_generate_netlink_packet(struct flp_port_t *flp_port, const char *buf,
		unsigned int count, unsigned char cmd_type)
{
	struct sk_buff *skb = NULL;
	struct nlmsghdr *nlh = NULL;
	void *msg_header = NULL;
	char *data = NULL;
	int result;
	static unsigned int flp_event_seqnum;

	result = flp_genlink_checkin(flp_port, count, cmd_type);
	if (result != 0) {
		pr_err("[%s]flp_genlink_checkin[%d]\n", __func__, result);
		return result;
	}

	skb = genlmsg_new((size_t)count, GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	/* add the genetlink message header */
	msg_header = genlmsg_put(skb, 0, flp_event_seqnum++,
	&flp_genl_family, 0, cmd_type);
	if (msg_header == NULL) {
		nlmsg_free(skb);
		return -ENOMEM;
	}

	/* fill the data */
	data = nla_reserve_nohdr(skb, (int)count);
	if (data == NULL) {
		nlmsg_free(skb);
		return -EINVAL;
	}

	if (buf != NULL && count) {
		result = memcpy_s(data, (size_t)count, buf, (size_t)count);
		if (result != EOK) {
			nlmsg_free(skb);
			pr_err("[%s]memset_s fail[%d]\n", __func__, result);
			return result;
		}
	}

	/* if aligned, just set real count */
	nlh = (struct nlmsghdr *)((unsigned char *)msg_header - GENL_HDRLEN - NLMSG_HDRLEN);
	nlh->nlmsg_len = count + GENL_HDRLEN + NLMSG_HDRLEN;

	pr_info("%s %d:%d\n", __func__,  cmd_type, nlh->nlmsg_len);
	/* send unicast genetlink message */
	result = genlmsg_unicast(&init_net, skb, flp_port->portid);
	if (result != 0)
		pr_err("flp:Failed to send netlink event:%d", result);

	return result;
}
/*lint -e845*/
/*lint +e826 +e834 +e776 +e845*/
/*lint +e838*/

/*lint -e834 -e776*/

/*lint -e845 -e826*/

int send_cmd_from_kernel_qmi_wrapper(unsigned char cmd_tag, unsigned char cmd_type,
    unsigned int subtype, const char *buf, size_t count)
{
	flp_buf_to_hal_t *flp_buf_hal = NULL;
	int ret;

	hwlog_info("[%s] send cmd to slpi start", __func__);
	flp_buf_hal = (flp_buf_to_hal_t *)kzalloc(sizeof(flp_buf_to_hal_t) + count, GFP_KERNEL);
	if (count > 0) {
		ret = memcpy_s(flp_buf_hal->data, count, buf, count);
		if (ret != EOK) {
			pr_err("[%s], memcpy failed!\n", __func__);
			return ret;
		}
	}

	flp_buf_hal->sensor_type = cmd_tag;
	flp_buf_hal->cmd = cmd_type;
	flp_buf_hal->subcmd = subtype;

	if (count > FLP_IOCTL_MAX) {
		count = FLP_IOCTL_MAX;
		hwlog_err("[%s] flp_buf_hal length too big\n", __func__);
	}

	hwlog_info("[%s] send flp_buf_hal size %d", count + FLP_IOCTL_PKG_HEADER);
	ap_sensor_route_write((char *)flp_buf_hal, (count + FLP_IOCTL_PKG_HEADER));
	if (flp_buf_hal != NULL)
		kfree((flp_buf_to_hal_t *)flp_buf_hal);
	return 0;
}

int send_cmd_from_kernel_to_slpi(unsigned char app_tag, unsigned char cmd,
    const char *buf, int buf_size, size_t count)
{
	struct flp_data_req *flp_data = NULL;
	unsigned int flp_len;
	int ret;

	if (buf_size < 0)
		return -1;
	flp_len = sizeof(struct flp_data_req) + buf_size;
	flp_data = kzalloc((size_t)flp_len, GFP_KERNEL); //lint !e838
	if (IS_ERR_OR_NULL(flp_data)) {
		pr_err("%s kmalloc fail\n", __func__);
		return -ENOMEM;
	}

	ret = memset_s((void *)flp_data, sizeof(struct pkt_header), 0, sizeof(struct pkt_header));
	if (ret != EOK) {
		pr_err("[%s]memset_s fail[%d]\n", __func__, ret);
		return ret;
	}
	flp_data->pkt.tag = app_tag;
	flp_data->pkt.cmd = cmd;
	flp_data->pkt.resp = FLP_CMD_REQUIRE_RESP;
	if (buf_size > 0) {
		ret = memcpy_s(flp_data->data, buf_size, buf, buf_size);
		if (ret != EOK) {
			pr_err("[%s], memcpy failed!\n", __func__);
			ret = -EINVAL;
			goto OUT;
		}
	}
	ret = send_cmd_from_kernel_qmi_wrapper(SENSOR_TYPE_FLP, CMD_CMN_CONFIG_REQ, 0, (const char *)flp_data, flp_len);
	if (ret != 0) {
		pr_err("[%s], send_cmd_from_kernel_qmi_wrapper failed!\n", __func__);
		ret = -EINVAL;
		goto OUT;
	}
OUT:
	if (!IS_ERR_OR_NULL(flp_data))
		kfree(flp_data);

	return ret;
}

/*
 * Function    : inputhub_wrapper_pack_and_send_cmd
 * Description : This interface support all scenarios and phone flp.
 *             : For all scenarios, no matter rd_info is nullptr or not, success is considered
 *             : when ipc interface return success and resp errno is 0.
 *             : But if rd_info is non-null pointer, it's considered that caller need resp value.
 *             : So variable rd will give rd_info.
 *             : For phone, returned error code and resp errno are not always required.
 *             : If caller need resp, will transport non-null pointer rd_info.
 *             : This function will return error code, how to use depend on caller.
 * Input       : [app_tag] app tag.
 *             : [cmd] cmd, CMD_CMN_CONFIG_REQ, CMD_CMN_OPEN_REQ and etc.
 *             : [buf] For CMD_CMN_CONFIG_REQ, this is customized buffer.
 *             :       For CMD_CMN_OPEN_REQ, CMD_CMN_CLOSE_REQ and etc, this is null.
 *             : [count] write buffer size.If buf is null, this will 0.
 *             : [rd] response info buffer
 * Output      : none
 * Return      : 0:successs, others: fail
 */
int inputhub_wrapper_pack_and_send_cmd(int app_tag, int cmd, int sub_cmd, const char *buf, size_t count, struct read_info *rd_info)
{
	char buffer[FLP_IOCTL_MAX];
	int ret;
	void *send_buf = NULL;
	int send_len = 0;

	pr_info("[%s], %d, %d, %d, %zu\n", __func__, app_tag, cmd, sub_cmd, count);
	if (count > INPUTHUB_WRAPPER_MAX_LEN) {
		pr_err("[%s], count %zu overflow limit %u\n", __func__, count, (unsigned int)INPUTHUB_WRAPPER_MAX_LEN);
		return -EINVAL;
	}

	/*
	 * Config packet need pack subcmd + buf
	 * For inputhub_as, success execute cmd need two point.
	 * One is ipc return success, the other is resp errno is 0.
	 */
	if (cmd == CMD_CMN_CONFIG_REQ) {
		((u32 *)buffer)[0] = sub_cmd;
		if (buf) {
			ret = memcpy_s(buffer + sizeof(u32), FLP_IOCTL_MAX - sizeof(u32), buf, count);
			if (ret != EOK) {
				pr_err("[%s], memcpy failed!\n", __func__);
				return -EINVAL;
			}
		}
		send_buf = (void *)buffer;
		send_len = sizeof(u32) + count;
	}

	// this interface has different impl for inputhub as and no inputhub_as
	reinit_completion(&g_flp_dev.shmem_completion);
	if (rd_info) {
		g_has_rd = true;
	}
	g_flp_dev.shmem_resp_errno = -1;
	ret = send_cmd_from_kernel_to_slpi(app_tag, cmd, send_buf, send_len, count);
	if (ret != 0) {
		pr_err("[%s], send cmd %d, sub cmd %d failed.Ret is %d\n",
			__func__, cmd, sub_cmd, ret);
		return -EINVAL;
	}

	// wait for sending result
	if (!wait_for_completion_timeout(&g_flp_dev.shmem_completion, msecs_to_jiffies(3000))) {
		pr_err("[%s]:[%d] shmem send timeout\n", __func__, __LINE__);
		return FLP_ERR_TIMEOUT;
	}
	ret = g_flp_dev.shmem_resp_errno;

	if (rd_info) {
		if (rd_info->errno != 0) {
			pr_err("[%s], send cmd %d, sub cmd %d failed.Resp errno is %d\n",
				__func__, cmd, sub_cmd, rd_info->errno);
			return -EINVAL;
		}
	}
	return ret;
}

/*
 * Send cellbatching data reported by contexthub to HAL
 */
/*lint +e845 */
static void flp_resort_cellbatching_data(char *data, unsigned int len)
{
	struct cell_batching_data_t *temp1 = (struct cell_batching_data_t *)data;
	struct cell_batching_data_t *temp2 = (struct cell_batching_data_t *)data;

	int index = 0;
	int ret;
	unsigned long time1 = 0;
	unsigned long time2 = 0;
	bool is_resort = false;
	unsigned int temp_len = len;

	if (data == NULL || len == 0 || (len % sizeof(struct cell_batching_data_t)) != 0) {
		pr_err("flp:%s invalid data len[%d]\n", __func__, len);
		return;
	}
	while ((temp_len - sizeof(struct cell_batching_data_t)) != 0) {
		index++;
		time1 = temp1[index-1].timestamplow;
		time1 |= (unsigned long) (temp1[index-1].timestamphigh) << 32;
		time2 = temp2[index].timestamplow;
		time2 |= (unsigned long) (temp2[index].timestamphigh) << 32;
		if (time1 > time2) {
			is_resort = true;
			pr_info("flp: %s resort cellbatching index %d\n", __func__, index);
			break;
		}
		temp_len -= sizeof(struct cell_batching_data_t);
	}
	if (is_resort) {
		char *begin_index = data + index * sizeof(struct cell_batching_data_t);
		char *buff = kzalloc(len, GFP_KERNEL);

		if (buff == NULL) {
			pr_info("flp: %s kmalloc fail\n", __func__);
			return;
		}
		ret = memcpy_s(buff, len, begin_index, len - index * sizeof(struct cell_batching_data_t));
		if (ret != EOK) {
			pr_err("%s memcpy_s error\n", __func__);
			kfree(buff);
			return;
		}
		ret = memcpy_s(buff + len - index * sizeof(struct cell_batching_data_t), index * sizeof(struct cell_batching_data_t), data, index * sizeof(struct cell_batching_data_t));
		if (ret != EOK) {
			pr_err("%s memcpy_s error\n", __func__);
			kfree(buff);
			return;
		}
		ret = memcpy_s(data, len, buff, len);
		if (ret != EOK) {
			pr_err("%s memcpy_s error\n", __func__);
			kfree(buff);
			return;
		}
		kfree(buff);
		pr_info("flp: %s done\n", __func__);
	}
}

static void flp_send_data_to_uplayer(struct flp_port_t *flp_port, const char *msg, unsigned int len, unsigned int check_type, unsigned int cmd)
{
	if (flp_port->channel_type & check_type)
		flp_generate_netlink_packet(flp_port, msg, len, cmd);
}

/*
 * Send the data reported by contexthub to HAL
 */
static int get_common_data_from_mcu(const struct pkt_header *head)
{
	unsigned int len;
	char *data = NULL;
	struct flp_port_t *flp_port = NULL;
	struct list_head *pos = NULL;

	if (!head) {
		pr_err("%s, head is null\n", __func__);
		return -EINVAL;
	}

	if (head->length < sizeof(unsigned int)) {
		pr_err("%s, length is too short\n", __func__);
		return -EINVAL;
	}
	len = head->length - sizeof(unsigned int);
	data = (char *)((pkt_subcmd_req_t *)head + 1);//lint !e838

	pr_info("flp:%s cmd:%d: len:%d\n", __func__, head->cmd, len);
	mutex_lock(&g_flp_dev.lock);
	list_for_each(pos, &g_flp_dev.list) {//lint !e838
		flp_port = container_of(pos, struct flp_port_t, list);
		pr_info("flp:%s, type[%d], subcmd[%d]\n", __func__, flp_port->channel_type, ((pkt_subcmd_req_t *)head)->subcmd);
		switch (((pkt_subcmd_req_t *)head)->subcmd) {
		case SUB_CMD_FLP_LOCATION_UPDATE_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_BATCHING, FLP_GENL_CMD_GNSS_LOCATION);
			break;
		case SUB_CMD_FLP_GEOF_TRANSITION_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_GEOFENCE, FLP_GENL_CMD_GEOFENCE_TRANSITION);
			break;
		case SUB_CMD_FLP_GEOF_MONITOR_STATUS_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_GEOFENCE, FLP_GENL_CMD_GEOFENCE_MONITOR);
			break;
		case SUB_CMD_FLP_RESET_RESP:
			if (IOMCU_APP_FLP & flp_port->channel_type) {
				flp_port->channel_type &= ~IOMCU_APP_FLP;
				g_flp_dev.service_type &= ~IOMCU_APP_FLP;
				flp_generate_netlink_packet(flp_port, data, (unsigned int)sizeof(unsigned int), FLP_GENL_CMD_IOMCU_RESET);
			}
			break;

		case SUB_CMD_FLP_CELLFENCE_TRANSITION_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_CELLFENCE, FLP_GENL_CMD_CELLTRANSITION);
			break;
		case SUB_CMD_FLP_CELLTRAJECTORY_REPORT_REQ:
			flp_resort_cellbatching_data(data, len);
			flp_send_data_to_uplayer(flp_port, data, len, FLP_CELLTRAJECTORY, FLP_GENL_CMD_TRAJECTORY_REPORT);
			break;
		case SUB_CMD_FLP_CELLDB_LOCATION_REPORT_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_OFFLINEDB, FLP_GENL_CMD_REQUEST_CELLDB);
			break;
		case SUB_CMD_FLP_GEOF_GET_LOCATION_REPORT_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_GEOFENCE, FLP_GENL_CMD_GEOFENCE_LOCATION);
			break;
		case SUB_CMD_FLP_WIFENCE_TRANSITION_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_WIFIFENCE, FLP_GENL_CMD_WIFIFENCE_TRANSITION);
			break;
		case SUB_CMD_FLP_WIFENCE_STATUS_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_WIFIFENCE, FLP_GENL_CMD_WIFIFENCE_MONITOR);
			break;
		case SUB_CMD_FLP_DIAG_DATA_REPORT_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_DIAG, FLP_GENL_CMD_DIAG_DATA_REPORT);
			break;
		case SUB_CMD_FLP_CELL_CELLBATCHING_REPORT_REQ:
			flp_resort_cellbatching_data(data, len);
			flp_send_data_to_uplayer(flp_port, data, len, FLP_CELLBATCHING, FLP_GENL_CMD_CELLBATCHING_REPORT);
			break;
		case SUB_CMD_FLP_GEOF_GET_SIZE_REPORT_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_GEOFENCE, FLP_GENL_CMD_GEOFENCE_SIZE);
			break;
		case SUB_CMD_FLP_CELLFENCE_GET_SIZE_REPORT_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_CELLFENCE, FLP_GENL_CMD_CELLFENCE_SIZE);
			break;
		case SUB_CMD_FLP_WIFENCE_GET_SIZE_REPORT_REQ:
			flp_send_data_to_uplayer(flp_port, data, len, FLP_WIFIFENCE, FLP_GENL_CMD_WIFIFENCE_SIZE);
			break;
		default:
			pr_err("flp:%s cmd[0x%x] error\n", __func__, head->cmd);
			mutex_unlock(&g_flp_dev.lock);
			return -EFAULT;
		}
	}
	mutex_unlock(&g_flp_dev.lock);
	return (int)len;
}

/*
 * Send the data of known ID reported by contexthub to HAL
 */
static int flp_handle_report_data(const struct pkt_header *head)
{
	if (head == NULL) {
		pr_err("[%s] head is NULL\n", __func__);
		return -EINVAL;
	}
	pr_err("FLP[%s] subcmd 0x%x.\n", __func__, ((pkt_subcmd_req_t *)head)->subcmd);
	switch (((pkt_subcmd_req_t *)head)->subcmd) {
	case SUB_CMD_FLP_LOCATION_UPDATE_REQ:
	case SUB_CMD_FLP_GEOF_TRANSITION_REQ:
	case SUB_CMD_FLP_GEOF_MONITOR_STATUS_REQ:
	case SUB_CMD_FLP_CELLFENCE_TRANSITION_REQ:
	case SUB_CMD_FLP_CELLTRAJECTORY_REPORT_REQ:
	case SUB_CMD_FLP_CELLDB_LOCATION_REPORT_REQ:
	case SUB_CMD_FLP_GEOF_GET_LOCATION_REPORT_REQ:
	case SUB_CMD_FLP_WIFENCE_TRANSITION_REQ:
	case SUB_CMD_FLP_WIFENCE_STATUS_REQ:
	case SUB_CMD_FLP_CELL_CELLBATCHING_REPORT_REQ:
	case SUB_CMD_FLP_DIAG_DATA_REPORT_REQ:
	case SUB_CMD_FLP_GEOF_GET_SIZE_REPORT_REQ:
	case SUB_CMD_FLP_CELLFENCE_GET_SIZE_REPORT_REQ:
	case SUB_CMD_FLP_WIFENCE_GET_SIZE_REPORT_REQ:
		return get_common_data_from_mcu(head);
	default:
		pr_err("FLP[%s]uncorrect subcmd 0x%x.\n", __func__, ((pkt_subcmd_req_t *)head)->subcmd);
	}
	return -EFAULT;
}

/*
 * Report data to work handler
 */
/*lint -e838*/
static void flp_report_data_handle_work_handler(struct work_struct *work)
{
	int ret;
	struct flp_report_data_handle_work *w = NULL;

	if (work == NULL) {
		pr_err("[%s] w or data is NULL\n", __func__);
		return;
	}

	w = container_of(work, struct flp_report_data_handle_work, worker);
	if (w == NULL || w->data == NULL) {
		pr_err("[%s] w or data is NULL\n", __func__);
		return;
	}

	ret = flp_handle_report_data((const struct pkt_header *)w->data);
	if (ret < 0)
		pr_warn("FLP[%s]handle report data fail\n", __func__);

	kfree(w->data);
	kfree(w);
}

/*
 * Process data sent from the sensor hub in a new thread for prevent deadlock
 */
static int get_data_from_mcu(const struct pkt_header *head)
{
	size_t len;
	struct flp_report_data_handle_work *wk = NULL;

	if (!head) {
		pr_err("[%s] head is NULL\n", __func__);
		return -EINVAL;
	}

	len = head->length + sizeof(struct pkt_header);
	if (len >= FLP_MAX_GET_DATA) {
		pr_err("[%s] date len too big, len:%zu\n", __func__, len);
		return -EINVAL;
	}

	wk = kzalloc(sizeof(struct flp_report_data_handle_work), GFP_KERNEL);
	if (!wk) {
		pr_err("[%s] alloc work error is err.\n", __func__);
		return -ENOMEM;
	}

	wk->data = kzalloc(len, GFP_KERNEL);
	if (!wk->data) {
		kfree(wk);
		pr_err("[%s] kzalloc wk data error\n", __func__);
		return -ENOMEM;
	}

	if (memcpy_s(wk->data, len, head, len) != EOK) {
		kfree(wk->data);
		kfree(wk);
		pr_err("%s memcpy_s error\n", __func__);
		return -EFAULT;
	}

	INIT_WORK(&wk->worker, flp_report_data_handle_work_handler);
	queue_work(g_flp_dev.wq, &wk->worker);
	return 0;
}
/*lint +e838*/

/*
 * Business recovery after contexthub reset
 */
static void  flp_service_recovery(void)
{
	struct flp_port_t *flp_port = NULL;
	unsigned int response = FLP_IOMCU_RESET;
	struct list_head *pos = NULL; // coverity is conflict with pclint for valiable init, disable pclint 838 warning below
	struct list_head *tmp = NULL;

	mutex_lock(&g_flp_dev.recovery_lock);
	list_for_each_safe(pos, tmp, &g_flp_dev.list) {//lint !e838
		flp_port = container_of(pos, struct flp_port_t, list);
		if (flp_port->channel_type & IOMCU_APP_FLP) {
			flp_port->channel_type &= ~IOMCU_APP_FLP;
			g_flp_dev.service_type &= ~IOMCU_APP_FLP;
			flp_generate_netlink_packet(flp_port, (char *)&response, (unsigned int)sizeof(unsigned int), FLP_GENL_CMD_IOMCU_RESET);
		}
	}
	mutex_unlock(&g_flp_dev.recovery_lock);
}

/*lint +e715*/
/*static struct notifier_block sensor_reboot_notify = {
	.notifier_call = flp_notifier,
	.priority = -1,
};*/

static void flp_timerout_work(struct work_struct *wk)
{
	struct flp_port_t *flp_port = container_of(wk, struct flp_port_t, work);

	flp_generate_netlink_packet(flp_port, NULL, 0, (unsigned char)flp_port->work_para);
}
static void flp_sleep_timeout(unsigned long data)
{
	struct flp_port_t *flp_port = (struct flp_port_t *)((uintptr_t)data);

	pr_info("flp sleep timeout\n");
	if (flp_port != NULL) {
		flp_port->work_para = FLP_GENL_CMD_NOTIFY_TIMEROUT;
		queue_work(system_power_efficient_wq, &flp_port->work);
		if (flp_port->need_hold_wlock)
			__pm_wakeup_event(&flp_port->wlock, jiffies_to_msecs(2 * HZ));
	}
}

void flp_port_resume(void)
{
	struct list_head *pos = NULL;
	struct flp_port_t *flp_port = NULL;

	mutex_lock(&g_flp_dev.lock);
	list_for_each(pos, &g_flp_dev.list) {//lint !e838
		flp_port = container_of(pos, struct flp_port_t, list);
		if (flp_port->need_awake) {
			flp_port->work_para = FLP_GENL_CMD_AWAKE_RET;
			queue_work(system_power_efficient_wq, &flp_port->work);
		}
	}
	mutex_unlock(&g_flp_dev.lock);
}
/*
 * FLP command ID check
 */
static bool flp_check_cmd(struct flp_port_t *flp_port, unsigned int cmd, int type)
{
	switch (type) {
	case FLP_GEOFENCE:
	case FLP_BATCHING:
	case FLP_CELLFENCE:
	case FLP_DIAG:
	case FLP_WIFIFENCE:
		if ((!(flp_port->channel_type & IOMCU_APP_FLP)) && (g_flp_dev.service_type & IOMCU_APP_FLP)) {
			pr_err("FLP[%s] ERR: FLP APP not support multi process!\n", __func__);
			return false;
		}
		if ((cmd & FLP_IOCTL_TAG_MASK) == FLP_IOCTL_TAG_FLP)
			return true;
		break;
	default:
		break;
	}
	return false;
}

/*lint -e715 -e502*/
/*
 * Send start message to contexthub
 */
static int flp_common_ioctl_open_service(struct flp_port_t *flp_port)
{
	unsigned int response = FLP_IOMCU_RESET;
	struct read_info rd;

	(void)memset_s((void *)&rd, sizeof(rd), 0, sizeof(struct read_info));
	if (g_flp_dev.service_type & IOMCU_APP_FLP) {
		g_flp_dev.service_type &= ~IOMCU_APP_FLP;
		flp_generate_netlink_packet(flp_port, (char *)&response, (unsigned int)sizeof(unsigned int),
					    FLP_GENL_CMD_IOMCU_RESET);
	}
	return 0;
}
/*
 * Send a closed message to contexthub
 */
static int flp_common_ioctl_close_service(struct flp_port_t *flp_port)
{
	if (g_flp_dev.service_type & IOMCU_APP_FLP)
		inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CLOSE_REQ, 0, NULL, (size_t)0, NULL);
	return 0;
}

/*lint -e530*/
/*
 * Send a service stop message to contexthub
 */
static int flp_stop_service_type(struct flp_port_t *flp_port, unsigned long arg)
{
	int service_id;
	int ret;
	unsigned int status;
	//struct read_info rd = {0};

	if (get_user(service_id, (int __user *)((uintptr_t)arg)))
		return -EFAULT;

	status = (unsigned int)service_id;
	if ((flp_port->channel_type & status) == 0 || (g_flp_dev.service_type & status) == 0) {
		pr_err("[%s]service has closed, service_id[%d]\n", __func__, service_id);
		return -EACCES;
	}
	if ((flp_port->channel_type & IOMCU_APP_FLP) == 0) {
		pr_err("[%s]iomcu flp had closed, service_id[%d]\n", __func__, service_id);
		return -EACCES;
	}

	if ((g_flp_dev.service_type & IOMCU_APP_FLP) == 0) {
		pr_err("[%s]global service_type has cleanup\n", __func__);
		flp_port->channel_type &= ~IOMCU_APP_FLP;
		return -EACCES;
	}

	(void)memset_s((void *)&g_rd_info, sizeof(g_rd_info), 0, sizeof(g_rd_info));
	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_COMMON_STOP_SERVICE_REQ,
					    (char *)&service_id, sizeof(service_id), &g_rd_info);
	if (ret != 0) {
		pr_err("[%s]iomcu err[%d]\n", __func__, g_rd_info.errno);
		return ret;
	}

	flp_port->channel_type &= ~status;

	/* here global service_type is bad design,I just obey the original design */
	g_flp_dev.service_type &= ~status;
	pr_info("%s current open service[0x%x], close[0x%x]\n",
		__func__, g_flp_dev.service_type, service_id);
	if ((g_flp_dev.service_type & IOMCU_APP_FLP) == 0)
		(void)inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CLOSE_REQ, 0, NULL, (size_t)0, NULL);

	return ret;
}
/*lint +e530*/

static int flp_iomcu_template(struct flp_port_t *flp_port, unsigned long arg, unsigned int iomcu_sub_cmd)
{
	struct cellfence_ioctrl_hdr_type cfence_hdr;
	char usr[FLP_IOCTL_MAX];

	if ((flp_port->channel_type & IOMCU_APP_FLP) == 0) {
		pr_err("[%s] ERR: you must add cellfence first error\n", __func__);
		return -EINVAL;
	}

	if (copy_from_user(&cfence_hdr, (void *)((uintptr_t)arg), sizeof(struct cellfence_ioctrl_hdr_type))) {
		pr_err("[%s]copy_from_user error\n", __func__);
		return -EFAULT;
	}

	if (cfence_hdr.len == 0 || FLP_IOCTL_MAX < cfence_hdr.len) {
		pr_err("[%s] cfence_hdr.len  [%u]error\n", __func__, cfence_hdr.len);
		return -EPERM;
	}

	if (copy_from_user(usr, (void *)cfence_hdr.buf, (unsigned long)cfence_hdr.len)) {
		pr_err("[%s]copy_from_user usr error\n", __func__);
		return -EFAULT;
	}

	return inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, iomcu_sub_cmd,
				    usr, (unsigned long)cfence_hdr.len, NULL);
}
/*
 * cellfence command processing
 */
int cellfence_operate(struct flp_port_t *flp_port, unsigned long arg)
{
	unsigned int sub_cmd = SUB_CMD_FLP_CELLFENCE_OPT_REQ;

	return flp_iomcu_template(flp_port, arg, sub_cmd);
}
/*
 * cellfence data injection
 */
int cellfence_inject_result(struct flp_port_t *flp_port, unsigned long arg)
{
	unsigned int sub_cmd = SUB_CMD_FLP_CELLFENCE_INJECT_RESULT_REQ;

	return flp_iomcu_template(flp_port, arg, sub_cmd);
}

/*
 * Send wifi message to contexthub
 */
static int flp_wifi_cfg(struct flp_port_t *flp_port, unsigned long arg)
{
	unsigned int sub_cmd = SUB_CMD_FLP_COMMON_WIFI_CFG_REQ;

	return flp_iomcu_template(flp_port, arg, sub_cmd);
}
/*
 * flp service switch
 */
static inline void flp_service_ops(struct flp_port_t *flp_port, unsigned int data)
{
	g_flp_dev.denial_sevice = data;
	pr_info("%s 0x%x\n", __func__, g_flp_dev.denial_sevice);

	if (g_flp_dev.denial_sevice)
		flp_common_ioctl_close_service(flp_port);
	else
		flp_common_ioctl_open_service(flp_port);
}

/*
 * Location information injection
 */
static int flp_common_inject_location(unsigned long arg)
{
	struct iomcu_location inject_data;

	if (copy_from_user(&inject_data, (void *)((uintptr_t)arg), sizeof(struct iomcu_location))) {
		pr_err("%s copy_from_user error\n", __func__);
		return -EFAULT;
	}
	return inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_INJECT_LOCATION_REQ, (char *)&inject_data, sizeof(struct iomcu_location), NULL);
}

#if (defined CONFIG_SMART_FLP_GEOFENCE) || (defined CONFIG_SMART_FLP_WIFIFENCE) || (defined CONFIG_SMART_FLP_CELLFENCE)
/*
 * Public data injection
 */
static int flp_common_inject_data(struct flp_port_t *flp_port, unsigned long arg)
{
#ifdef CONFIG_DFX_DEBUG_FS
	struct hal_config_t config;
	int ret;

	if (copy_from_user(&config, (void *)((uintptr_t)arg), sizeof(struct hal_config_t))) {
		pr_err("[%s] copy hal config error\n", __func__);
		return -EFAULT;
	}
	ret = flp_fence_shmem(flp_port, &config, FLP_SHMEM_CMD_INJECT_DATA, 0);
	return ret;
#else
	return 0;
#endif
}
#endif
/*
 * Send push location status message to contexthub
 */
static int flp_common_push_location_status(struct flp_port_t *flp_port, unsigned long arg)
{
	struct loc_source_status_hal hal_status;
	int ret;
	unsigned int clean_bit;

	if (copy_from_user(&hal_status, (void *)((uintptr_t)arg), sizeof(struct loc_source_status_hal))) {
		pr_err("[flperr][%s] copy_from_user error\n", __func__);
		return -EFAULT;
	}

	if (hal_status.source >= LOCATION_SOURCE_SUPPORT || hal_status.status > LOC_STATUS_ON) {
		pr_err("[%s]:line[%d] ERR: Invalid source[0x%x] status[0x%x]\n", __func__, __LINE__, hal_status.source, hal_status.status);
		return -EINVAL;
	}

	flp_port->location_status.source |= (1 << hal_status.source);
	clean_bit = hal_status.status ^ 1;
	flp_port->location_status.status[hal_status.source] |= (1 << hal_status.status);
	flp_port->location_status.status[hal_status.source] &= (~(1 << clean_bit));

	if (!(flp_port->channel_type & IOMCU_APP_FLP)) {
		pr_info("[%s]:line[%d]you must have flp first source[0x%x] status[0x%x], ap store source[0x%x] status[0x%x]\n",
			__func__, __LINE__, hal_status.source, hal_status.status, flp_port->location_status.source,
			flp_port->location_status.status[hal_status.source]);
		return 0;
	}

	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_BATCH_PUSH_GNSS_REQ,
					(char *)&flp_port->location_status, sizeof(struct loc_source_status), NULL);

	pr_info("[%s]:line[%d] hal send source[0x%x] status[0x%x], ap will send source[0x%x] status[0x%x] result[%d]\n",
	       __func__, __LINE__, hal_status.source, hal_status.status, flp_port->location_status.source,
	       flp_port->location_status.status[hal_status.source], ret);
	return ret;
}

/*
 * Send debug config message to contexthub
 */
/*lint -e603*/
static int flp_common_set_debug_config(unsigned long arg)
{
	int cfg;
	int ret;

	if (get_user(cfg, (int __user *)((uintptr_t)arg))) {
		pr_err("[%s]:line[%d] open or send error\n", __func__, __LINE__);
		return -EFAULT;
	}

	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_COMMON_DEBUG_CONFIG_REQ,
				   (char *)&cfg, sizeof(cfg), NULL);

	pr_info("[%s]:line[%d] dbg cfg[%d] result[%d]\n", __func__, __LINE__, cfg, ret);
	return ret;
}
/*lint +e603*/

/*
 * hifence ioctl message processing
 */
static int flp_hifence_common_ioctl(struct flp_port_t *flp_port, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case FLP_IOCTL_COMMON_STOP_SERVICE_TYPE:
		return flp_stop_service_type(flp_port, arg);
	case FLP_IOCTL_COMMON_WIFI_CFG:
		return flp_wifi_cfg(flp_port, arg);
	case FLP_IOCTL_COMMON_INJECT:
		return flp_common_inject_location(arg);
	case FLP_IOCTL_COMMON_LOCATION_STATUS:
		return flp_common_push_location_status(flp_port, arg);
	case FLP_IOCTL_COMMON_DEBUG_CONFIG:
		return flp_common_set_debug_config(arg);
#if (defined CONFIG_SMART_FLP_GEOFENCE) || (defined CONFIG_SMART_FLP_WIFIFENCE) || (defined CONFIG_SMART_FLP_CELLFENCE)
	case FLP_IOCTL_COMMON_INJECT_DATA:
		return flp_common_inject_data(flp_port, arg);
#endif
	default:
		pr_err("[%s] cmd[0x%x] error\n", __func__, cmd);
		break;
	}
	return -EFAULT;
}

/*
 * flp common ioctl message processing
 */
static int flp_common_ioctl(struct flp_port_t *flp_port, unsigned int cmd, unsigned long arg)
{
	unsigned int data = 0;
	int ret;

	if (cmd != FLP_IOCTL_COMMON_RELEASE_WAKELOCK) {
		if (copy_from_user(&data, (void *)((uintptr_t)arg), sizeof(unsigned int))) {
			pr_err("%s copy_from_user error\n", __func__);
			return -EFAULT;
		}
	}

	switch (cmd) {
	case FLP_IOCTL_COMMON_SLEEP:
		pr_info("flp:start timer %d\n",  data);
		/* if timer is running just delete it ,then restart it */
		smart_softtimer_delete(&flp_port->sleep_timer);
		ret = smart_softtimer_modify(&flp_port->sleep_timer, data);
		if (ret == 0)
			smart_softtimer_add(&flp_port->sleep_timer);
		break;
	case FLP_IOCTL_COMMON_AWAKE_RET:
		flp_port->need_awake = data;
		break;
	case FLP_IOCTL_COMMON_SETPID:
		flp_port->portid = data;
		break;
	case FLP_IOCTL_COMMON_CLOSE_SERVICE:
		flp_service_ops(flp_port, data);
		break;
	case FLP_IOCTL_COMMON_HOLD_WAKELOCK:
		flp_port->need_hold_wlock = data;
		pr_info("%s 0x%x\n", __func__, flp_port->need_hold_wlock);
		break;
	case FLP_IOCTL_COMMON_RELEASE_WAKELOCK:
		if (flp_port->need_hold_wlock)
			__pm_relax(&flp_port->wlock);/*lint !e455*/
		break;
	default:
		ret = flp_hifence_common_ioctl(flp_port, cmd, arg);
		return ret;
	}
	return 0;
}

static int flp_rd_info_resp(const char *head, size_t len)
{
	struct pkt_common_resp *resp = (struct pkt_common_resp *)head;
	int ret;

	g_flp_dev.shmem_resp_errno = resp->hd.errno;
	pr_err("%s get read info resp errno:%d\n", __func__, resp->hd.errno);
	if (g_has_rd) {
		g_rd_info.errno = resp->hd.errno;
		g_rd_info.data_length = len - sizeof(struct pkt_common_resp);
		ret = memcpy_s(g_rd_info.data, MAX_PKT_LENGTH,
			head + sizeof(struct pkt_common_resp), g_rd_info.data_length);
		pr_err("%s get read info data_length:%d, %ld\n", __func__, g_rd_info.data_length, *(int *)g_rd_info.data);
		if (ret != EOK) {
			pr_err("%s memcpy_s error\n", __func__);
			return ret;
		}
	}
	g_has_rd = false;
	complete(&g_flp_dev.shmem_completion);
	return 0;
}

static int flp_shmem_resp(const char *head, size_t len)
{
	struct flp_shmem_resp_pkt *resp = (struct flp_shmem_resp_pkt *)head;
	int ret;

	/*if (resp->transid != g_flp_shmem_transid) {
		pr_warn("%s shmem response transid:%u, but expect:%u\n",
			__func__, resp->transid, g_flp_shmem_transid);
		return 0;
	}

	pr_info("%s get shmem resp transid:%u\n", __func__, resp->transid);*/
	g_flp_dev.shmem_resp_errno = resp->errno;
	pr_err("%s get shmem resp errno:%d\n", __func__, resp->errno);
	if (g_has_rd) {
		g_rd_info.errno = resp->errno;
		g_rd_info.data_length = len - sizeof(struct flp_shmem_resp_pkt);
		ret = memcpy_s(g_rd_info.data, MAX_PKT_LENGTH,
			head + sizeof(struct flp_shmem_resp_pkt), g_rd_info.data_length);
		pr_err("%s get shmem resp data_length:%d\n", __func__, g_rd_info.data_length);
		if (ret != EOK) {
			pr_err("%s memcpy_s error\n", __func__);
			return ret;
		}
	}
	complete(&g_flp_dev.shmem_completion);
	return 0;
}

int flp_shmem_send_async(unsigned short int cmd, const void __user *buf,
			unsigned int length)
{
	struct flp_shmem_hdr *iomcu_data = NULL;
	unsigned int iomcu_len;
	int ret;
	struct geofencing_useful_data_t *data = NULL;

	if (!buf || length == 0) {
		pr_err("%s user buf is null or length:%u fail\n",
			__func__, length);
		return -ENOMEM;
	}

	/*if (length > shmem_get_capacity()) {
		pr_err("%s length:%u too large, capacity:%u\n",
			__func__, length, shmem_get_capacity());
		return -ENOMEM;
	}*/

	iomcu_len = sizeof(struct flp_shmem_hdr) + length;
	iomcu_data = kzalloc((size_t)iomcu_len, GFP_KERNEL); //lint !e838
	if (IS_ERR_OR_NULL(iomcu_data)) {
		pr_err("%s kmalloc fail\n", __func__);
		return -ENOMEM;
	}

	ret = (int)copy_from_user(iomcu_data->data, (const void __user *)buf,
				(unsigned long)length);
	if (ret) {
		pr_err("%s copy_from_user length:%u ret:%d error\n", __func__,
			length, ret);
		ret = -EFAULT;
		goto OUT;
	}
	if (length >= sizeof(struct geofencing_useful_data_t)) {
	data = (struct geofencing_useful_data_t *)(iomcu_data->data);

	}

	iomcu_data->tag = FLP_IOMCU_SHMEM_TAG;
	iomcu_data->cmd = cmd;
	iomcu_data->transid = ++g_flp_shmem_transid;
	iomcu_data->data_len = length;

	pr_info("%s cmd:0x%x,transid:%u iomcu_len:%u\n", __func__, cmd, iomcu_data->transid, iomcu_len);

/*#ifdef CONFIG_INPUTHUB_30
	if (iomcu_len > INPUTHUB_WRAPPER_MAX_LEN)
		ret = shmem_send(TAG_FLP, iomcu_data, (unsigned int)iomcu_len);
	else*/
		ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ,
			SUB_CMD_FLP_SHMEM_REQ, (const char *)iomcu_data, iomcu_len, NULL);
/*#else
	ret = shmem_send(TAG_FLP, iomcu_data, (unsigned int)iomcu_len);
#endif*/
	if (ret) {
		pr_err("%s shmem_send error\n", __func__);
		ret = -EFAULT;
		goto OUT;
	}
OUT:
	if (!IS_ERR_OR_NULL(iomcu_data))
		kfree(iomcu_data);

	return ret;
}

int flp_shmem_send_sync(unsigned short int cmd, const void __user *buf,
			unsigned int length)
{
	int ret;

	ret = flp_shmem_send_async(cmd, buf, length);
	if (ret) {
		pr_err("[%s]:[%d] flp_shmem_send_async send fail\n", __func__, __LINE__);
		return g_flp_dev.shmem_resp_errno;
	}

	return ret;
}

/*lint +e715*/
/*lint -e438*/
/*static int flp_open_send_status_flp(struct flp_port_t *flp_port)
{
	int ret = 0;

	if (!(flp_port->channel_type & IOMCU_APP_FLP)) {
		ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_OPEN_REQ, 0, NULL, (size_t)0, NULL);
		if (ret != 0) {
			pr_err("[flperr][%s]:line[%d] iomcu open err\n", __func__, __LINE__);
			return ret;
		}
	}
	return ret;
}*/

#if (defined CONFIG_SMART_FLP_GEOFENCE) || (defined CONFIG_SMART_FLP_WIFIFENCE) || (defined CONFIG_SMART_FLP_CELLFENCE)
/*
 * Send fence message to contexthub through share memory
 */
static int flp_fence_shmem(struct flp_port_t *flp_port, struct hal_config_t *config,
				unsigned short int shmem_cmd, unsigned int type)
{
	int ret;

	/*ret = flp_open_send_status_flp(flp_port);
	if (ret != 0)
		return ret;*/

	ret = flp_shmem_send_sync(shmem_cmd, (const void __user *)config->buf,
		(unsigned long)config->length);
	if (ret != 0 && ret != FLP_ERR_TIMEOUT)
		return ret;

	flp_port->channel_type |= type;
	g_flp_dev.service_type |= type;

	return ret;
}
#endif

#if (defined CONFIG_SMART_FLP_GEOFENCE) || (defined CONFIG_SMART_FLP_WIFIFENCE)
/*
 * Read fence status
 */
/*lint -e603*/
static int flp_fence_status_cmd(struct flp_port_t *flp_port, unsigned long arg, unsigned int cmd)
{
	//struct read_info rd = {0};
	int ret;
	int id;

	if (flp_port == NULL)
		return -EFAULT;

	if (get_user(id, (int __user *)((uintptr_t)arg)))
		return -EFAULT;

	(void)memset_s((void *)&g_rd_info, sizeof(g_rd_info), 0, sizeof(g_rd_info));
	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ,
										cmd, (char *)&id, sizeof(id), &g_rd_info);
	if (ret != 0) {
		pr_err("[%s]iomcu err[%d]\n", __func__, g_rd_info.errno);
		ret = -ENOENT;
		goto STATUS_FIN;
	}

	if (sizeof(int) != g_rd_info.data_length) {
		pr_err("[%s]iomcu len err[%d]\n", __func__, g_rd_info.data_length);
		ret = -EPERM;
		goto STATUS_FIN;
	}

	ret = *(int *)g_rd_info.data;

STATUS_FIN:
	return ret;
}
/*lint +e603*/
#endif

#ifdef CONFIG_SMART_FLP_GEOFENCE
/*
 * Send a fence message to contexthub
 */
static int flp_ioctl_add_geofence(struct flp_port_t *flp_port, unsigned long arg)
{
	struct hal_config_t config;
	int ret;

	if (copy_from_user(&config, (void *)((uintptr_t)arg), sizeof(struct hal_config_t))) {
		pr_err("[flperr][%s] copy_from_user hal_config error\n", __func__);
		return -EFAULT;
	}

	if (config.length > FLP_GEOFENCE_MAX_NUM * sizeof(struct geofencing_useful_data_t) || config.length == 0) {
		pr_err("[flperr]geofence number overflow %u\n", config.length);
		return -EFAULT;
	}

	pr_err("[%s] flp_ioctl_add_geofence %d , %u, data %.*x\n", __func__,
		sizeof(struct geofencing_useful_data_t), config.length, config.length, config.buf);
	ret = flp_fence_shmem(flp_port, &config, FLP_SHMEM_ADD_GEOFENCE, (unsigned int)FLP_GEOFENCE);
	return ret;
}

/*
 * Read fence status
 */
static int flp_geofence_status_cmd(struct flp_port_t *flp_port, unsigned long arg)
{
	int ret;

	if ((flp_port->channel_type & FLP_GEOFENCE) == 0) {
		pr_err("[%s]you must add geo first\n", __func__);
		return -EIO;
	}

	ret = flp_fence_status_cmd(flp_port, arg, SUB_CMD_FLP_GEOF_GET_TRANSITION_REQ);
	return ret;
}

/*
 * Send delete geofence message to contexthub
 */
static int flp_ioctl_remove_geofence(struct flp_port_t *flp_port, unsigned long arg)
{
	struct hal_config_t config;
	int ret;

	if ((flp_port->channel_type & FLP_GEOFENCE) == 0) {
		pr_err("[%s] not start\n", __func__);
		return -EPERM;
	}

	if (copy_from_user(&config, (const void __user *)((uintptr_t)arg), sizeof(struct hal_config_t))) {
		pr_err("[%s] copy_from_user hal_config error\n", __func__);
		return -EFAULT;
	}

	if (config.length > FLP_GEOFENCE_MAX_NUM * sizeof(int) || config.length == 0) {
		pr_err("[%s] geofence number %u overflow\n", __func__, config.length);
		return -EFAULT;
	}
	pr_err("[%s] flp geofence number %u, data %.*x\n", __func__, config.length, config.length, config.buf);

	ret = flp_fence_shmem(flp_port, &config, FLP_SHMEM_REMOVE_GEOFENCE, (unsigned int)FLP_GEOFENCE);
	return ret;
}

/*
 * Send modified geofence message to contexthub
 */
static int flp_ioctl_modify_geofence(struct flp_port_t *flp_port, unsigned long arg)
{
	struct geofencing_option_info_t modify_config;
	int ret;

	if ((flp_port->channel_type & FLP_GEOFENCE) == 0) {
		pr_err("%s not start\n", __func__);
		return -EPERM;
	}
	if (copy_from_user(&modify_config, (const void __user *)((uintptr_t)arg), sizeof(struct geofencing_option_info_t))) {
		pr_err("%s copy_from_user error\n", __func__);
		return -EFAULT;
	}

	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_MODIFY_GEOF_REQ,
			     (char *)&modify_config, sizeof(struct geofencing_option_info_t), NULL);
	return ret;
}

/*
 * Send a message to get geofence location to contexthub
 */
static int flp_geofence_get_location(struct flp_port_t *flp_port, unsigned long arg)
{
	if ((flp_port->channel_type & FLP_GEOFENCE) == 0) {
		pr_err("%s not start\n", __func__);
		return -EPERM;
	}

	inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_GEOF_GET_LOCATION_REQ, NULL, (size_t)0, NULL);
	return 0;
}

static int flp_geofence_get_size(struct flp_port_t *flp_port, unsigned long arg)
{
	int ret;

	if ((flp_port->channel_type & FLP_GEOFENCE) == 0) {
		pr_err("%s not start\n", __func__);
		return -EPERM;
	}
	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_GEOF_GET_SIZE_REQ,
		NULL, (size_t)0, NULL);
	return ret;
}

/*
 * geofence IOCTL message processing
 */
static int flp_geofence_ioctl(struct flp_port_t *flp_port, unsigned int cmd, unsigned long arg)
{
	int ret;

	switch (cmd) {
	case FLP_IOCTL_GEOFENCE_ADD:
		ret = flp_ioctl_add_geofence(flp_port, arg);
		break;
	case FLP_IOCTL_GEOFENCE_REMOVE:
		ret = flp_ioctl_remove_geofence(flp_port, arg);
		break;
	case FLP_IOCTL_GEOFENCE_MODIFY:
		ret = flp_ioctl_modify_geofence(flp_port, arg);
		break;
	case FLP_IOCTL_GEOFENCE_STATUS:
		ret = flp_geofence_status_cmd(flp_port, arg);
		break;
	case FD_IOCTL_GEOFENCE_GET_LOCATION:
		ret = flp_geofence_get_location(flp_port, arg);
		break;
	case FD_IOCTL_GEOFENCE_GET_SIZE:
		ret = flp_geofence_get_size(flp_port, arg);
		break;
	default:
		pr_err("%s input cmd[0x%x] error\n", __func__, cmd);
		ret = -EFAULT;
		break;
	}
	return ret;
}
#endif /* CONFIG_SMART_FLP_GEOFENCE end */

/*lint +e438*/
#ifdef CONFIG_SMART_FLP_BATCHING
/* max complexiy must less than 15 */
/*
 * Send location BATCHING command to contexthub
 */
static int __flp_location_ioctl(struct flp_port_t *flp_port, unsigned int cmd, unsigned long arg)
{
	int data;

	if (!(flp_port->channel_type & FLP_BATCHING)) {
		pr_err("%s not start\n", __func__);
		return -EPERM;
	}
	switch (cmd) {
	case FLP_IOCTL_BATCHING_STOP:
		if (copy_from_user(&data, (void *)((uintptr_t)arg), sizeof(int))) {
			pr_err("%s copy_from_user error\n", __func__);
			return -EFAULT;
		}
		inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_STOP_BATCHING_REQ, (char *)&data, sizeof(int), NULL);
		flp_port->channel_type &= ~FLP_BATCHING;
		g_flp_dev.service_type &= ~FLP_BATCHING;
		break;
	case FLP_IOCTL_BATCHING_UPDATE:
		if (copy_from_user(&flp_port->gps_batching_config, (void *)((uintptr_t)arg), sizeof(struct batching_config_t))) {
			pr_err("%s copy_from_user error\n", __func__);
			return -EFAULT;
		}
		inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_UPDATE_BATCHING_REQ,
		(char *)&flp_port->gps_batching_config, sizeof(struct batching_config_t), NULL);
		break;
	case FLP_IOCTL_BATCHING_LASTLOCATION:
		if (copy_from_user(&data, (void *)((uintptr_t)arg), sizeof(int))) {
			pr_err("%s copy_from_user error\n", __func__);
			return -EFAULT;
		}
		inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_GET_BATCHED_LOCATION_REQ, (char *)&data, sizeof(int), NULL);
		break;
	case FLP_IOCTL_BATCHING_FLUSH:
		inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_FLUSH_LOCATION_REQ, NULL, (size_t)0, NULL);
		break;

	case FLP_IOCTL_BATCHING_MULT_LASTLOCATION:
		if (copy_from_user(&data, (void *)((uintptr_t)arg), sizeof(int))) {
			pr_err("%s copy_from_user error\n", __func__);
			return -EFAULT;
		}
		inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_BATCHING_MULT_LASTLOCATION_REQ, (char *)&data, sizeof(int), NULL);
		break;
	case FLP_IOCTL_BATCHING_MULT_FLUSH:
		if (copy_from_user(&data, (void *)((uintptr_t)arg), sizeof(int))) {
			pr_err("%s copy_from_user error\n", __func__);
			return -EFAULT;
		}
		inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_BATCHING_MULT_FLUSH_REQ, (char *)&data, sizeof(int), NULL);
		break;

	default:
		return -EFAULT;
	}
	return 0;
}

/*
 * location batching ioctl message processing
 */
static int flp_location_ioctl(struct flp_port_t *flp_port, unsigned int cmd, unsigned long arg)
{
	//struct read_info rd = {0};
	int ret;

	switch (cmd) {
	case FLP_IOCTL_BATCHING_START:
		/*ret = flp_open_send_status_flp(flp_port);
		if (ret != 0)
			return ret;*/

		if (copy_from_user(&flp_port->gps_batching_config, (void *)((uintptr_t)arg), sizeof(struct batching_config_t))) {
			pr_err("%s copy_from_user error\n", __func__);
			return -EFAULT;
		}
		ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_START_BATCHING_REQ,
				     (char *)&flp_port->gps_batching_config, sizeof(struct batching_config_t), NULL);
		flp_port->channel_type |= FLP_BATCHING;
		g_flp_dev.service_type |= FLP_BATCHING;
		break;
	case FLP_IOCTL_BATCHING_STOP:
	case FLP_IOCTL_BATCHING_UPDATE:
	case FLP_IOCTL_BATCHING_LASTLOCATION:
	case FLP_IOCTL_BATCHING_FLUSH:
	case FLP_IOCTL_BATCHING_MULT_LASTLOCATION:
	case FLP_IOCTL_BATCHING_MULT_FLUSH:
		ret = __flp_location_ioctl(flp_port, cmd, arg);
		break;
	case FLP_IOCTL_BATCHING_GET_SIZE:
		(void)memset_s((void *)&g_rd_info, sizeof(g_rd_info), 0, sizeof(g_rd_info));
		ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_GET_BATCH_SIZE_REQ, NULL, (size_t)0, &g_rd_info);
		if (ret == 0) {
			if (copy_to_user((void *)((uintptr_t)arg), g_rd_info.data, sizeof(u16))) {
				pr_err("%s copy_to_user error\n", __func__);
				return -EFAULT;
			}
		}
		break;
	default:
		pr_err("%s input cmd[0x%x] error\n", __func__, cmd);
		return -EFAULT;
	}
	return ret;
}
#endif /* CONFIG_SMART_FLP_BATCHING end */

/*lint -e438*/
/*lint -e438*/
#ifdef CONFIG_SMART_FLP_CELLFENCE
/*
 * Send a message to add cellfence to contexthub
 */
static int cellfence_add(struct flp_port_t *flp_port, unsigned long arg, unsigned short int cmd)
{
	struct hal_config_t config;
	int ret;

	if (copy_from_user(&config, (void *)((uintptr_t)arg), sizeof(struct hal_config_t))) {
		pr_err("[%s] copy_from_user hal_config error\n", __func__);
		return -EFAULT;
	}

	if (config.length > CELLFENCE_ADD_INFO_BUF_SIZE || config.length == 0) {
		pr_err("[%s] length %u overflow\n", __func__, config.length);
		return -EFAULT;
	}
	ret = flp_fence_shmem(flp_port, &config, cmd, (unsigned int)FLP_CELLFENCE);
	return ret;
}

/*
 * Send the base station trace message to contexthub
 */
/*lint -e603*/
static int celltrajectory_cfg(struct flp_port_t *flp_port, unsigned long arg)
{
	int cfg = 0;
	int ret;

	if (get_user(cfg, (int __user *)((uintptr_t)arg)))
		return -EFAULT;

	/*ret = flp_open_send_status_flp(flp_port);
	if (ret != 0) {
		pr_err("[%s]open or send error\n", __func__);
		return ret;
	}*/
	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ,
									SUB_CMD_FLP_CELLTRAJECTORY_CFG_REQ, (char *)&cfg, sizeof(cfg), NULL);
	if (ret != 0) {
		pr_err("[%s]GEOFENCE_RM_REQ error\n", __func__);
		return ret;
	}
	pr_info("flp:[%s]cfg[%d]\n", __func__, cfg);
	if (cfg != 0) {
		flp_port->channel_type |= FLP_CELLTRAJECTORY;
		g_flp_dev.service_type |= FLP_CELLTRAJECTORY;
	} else {
		flp_port->channel_type &= ~FLP_CELLTRAJECTORY;
		g_flp_dev.service_type &= ~FLP_CELLTRAJECTORY;
	}

	return ret;
}
/*lint +e603*/

/*
 * Send the message of the base station trajectory request to contexthub
 */
static int celltrajectory_request(struct flp_port_t *flp_port)
{
	if ((flp_port->channel_type & FLP_CELLTRAJECTORY) == 0) {
		pr_err("[%s] ERR: you must celltrajectory_cfg first\n", __func__);
		return -EINVAL;
	}

	return inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ,
				    SUB_CMD_FLP_CELLTRAJECTORY_REQUEST_REQ, NULL, (size_t)0, NULL);
}

/*
 * Send cellbatching config to contexthub
 */
static int flp_cellbatching_cfg(struct flp_port_t *flp_port, unsigned long arg)
{
	int ret;
	struct cell_batching_start_config_t ct_cfg;

	if (arg == 0) {
		pr_err("[flperr][%s] invalid param\n", __func__);
		return -EFAULT;
	}

	if (copy_from_user(&ct_cfg, (void *)((uintptr_t)arg), sizeof(struct cell_batching_start_config_t))) {
		pr_err("[%s]copy_from_user error\n", __func__);
		return -EIO;
	}

	/*if ((flp_port->channel_type & IOMCU_APP_FLP) == 0) {
		ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_OPEN_REQ, 0, NULL, (size_t)0, NULL);
		if (ret != 0) {
			pr_err("[%s]CMD_CMN_OPEN_REQ error\n", __func__);
			return ret;
		}
	}*/

	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ,
	SUB_CMD_FLP_CELL_CELLBATCHING_CFG_REQ, (char *)&ct_cfg, sizeof(struct cell_batching_start_config_t), NULL);
	if (ret != 0) {
		if (!(g_flp_dev.service_type & IOMCU_APP_FLP))
			(void)inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CLOSE_REQ, 0, NULL, (size_t)0, NULL);
		pr_err("[%s]GEOFENCE_RM_REQ error\n", __func__);
		return ret;
	}

	pr_info("flp:[%s]cfg[%d]\n", __func__, ct_cfg.cmd);
	if (ct_cfg.cmd) {
		flp_port->channel_type |= FLP_CELLBATCHING;
		g_flp_dev.service_type |= FLP_CELLBATCHING;
	} else {
		flp_port->channel_type &= ~FLP_CELLBATCHING;
		g_flp_dev.service_type &= ~FLP_CELLBATCHING;
	}

	return ret;
}

/*
 * Send cellbatching request to contexthub
 */
static int flp_cellbatching_request(struct flp_port_t *flp_port)
{

	if ((flp_port->channel_type & FLP_CELLBATCHING) == 0) {
		pr_err("[%s] ERR: you must cellbatching_cfg first\n", __func__);
		return -EINVAL;
	}
	return inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ,
				    SUB_CMD_FLP_CELL_CELLBATCHING_REQ, NULL, (size_t)0, NULL);
}

static int flp_cellfence_get_size(struct flp_port_t *flp_port, unsigned long arg)
{
	int ret;

	if ((flp_port->channel_type & FLP_CELLFENCE) == 0) {
		pr_err("%s not start\n", __func__);
		return -EPERM;
	}
	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_CELLFENCE_GET_SIZE_REQ,
	NULL, (size_t)0, NULL);
	return ret;
}

/*
 * cellfence ioctl message processing
 */
static int cellfence_ioctl(struct flp_port_t *flp_port, unsigned int cmd, unsigned long arg)
{
	if (!flp_check_cmd(flp_port, (int)cmd, (int)FLP_CELLFENCE))
		return -EPERM;
	switch (cmd) {
	case FD_IOCTL_CELLFENCE_ADD:
		return cellfence_add(flp_port, arg, FLP_SHMEM_ADD_CELLFENCE);
	case FD_IOCTL_CELLFENCE_ADD_V2:
		return cellfence_add(flp_port, arg, FLP_SHMEM_ADD_CELLFENCE_V2);
	case FD_IOCTL_CELLFENCE_OPERATE:
		return cellfence_operate(flp_port, arg);
	case FD_IOCTL_TRAJECTORY_CONFIG:
		return celltrajectory_cfg(flp_port, arg);
	case FD_IOCTL_TRAJECTORY_REQUEST:
		return celltrajectory_request(flp_port);
	case FD_IOCTL_CELLFENCE_INJECT_RESULT:
		return cellfence_inject_result(flp_port, arg);
	case FD_IOCTL_CELLBATCHING_CONFIG:
		return flp_cellbatching_cfg(flp_port, arg);
	case FD_IOCTL_CELLBATCHING_REQUEST:
		return flp_cellbatching_request(flp_port);
	case FD_IOCTL_CELLFENCE_GET_SIZE:
		return flp_cellfence_get_size(flp_port, arg);
	default:
		pr_err("[%s]cmd err[%u]\n", __func__, cmd);
		return -EINVAL;
	}
}
#endif /* CONFIG_SMART_FLP_CELLFENCE */

#ifdef CONFIG_SMART_FLP_WIFIFENCE
/*
 * Send a message to add wififence to contexthub
 */
static int flp_add_wififence(struct flp_port_t *flp_port, unsigned long arg)
{
	struct hal_config_t config;
	int ret;

	if (copy_from_user(&config, (void *)((uintptr_t)arg), sizeof(struct hal_config_t))) {
		pr_err("[flperr][%s] copy_from_user hal_config error\n", __func__);
		return -EFAULT;
	}

	if (config.length > MAX_ADD_WIFI_NUM * sizeof(struct wififence_req_t) || config.length == 0) {
		pr_err("[flperr]wififence number error %u\n", config.length);
		return -EFAULT;
	}
	ret = flp_fence_shmem(flp_port, &config, FLP_SHMEM_ADD_WIFIFENCE, (unsigned int)FLP_WIFIFENCE);
	return ret;
}

/*
 * Send operation (including pause, resume, delete) WIFI fence message to contexthub
 */
static int flp_oper_wififence(struct flp_port_t *flp_port, unsigned long arg, unsigned int cmd)
{
	struct hal_config_t config;
	char *cmd_data = NULL; //lint !e838

	if ((flp_port->channel_type & FLP_WIFIFENCE) == 0) {
		pr_err("%s not start\n", __func__);
		return -EPERM;
	}
	if (copy_from_user(&config, (const void __user *)((uintptr_t)arg), sizeof(struct hal_config_t))) {
		pr_err("%s copy_from_user error\n", __func__);
		return -EFAULT;
	}
	if (config.length > FLP_MAX_WIFIFENCE_SIZE || config.length == 0) {
		pr_err("flp geofence number overflow %d\n", config.length);
		return -EFAULT;
	}
	cmd_data = kzalloc((size_t)config.length, GFP_KERNEL);//lint !e838
	if (cmd_data == NULL) {
		pr_err("%s kmalloc fail\n", __func__);
		return -ENOMEM;
	}
	if (copy_from_user(cmd_data, (const void __user *)config.buf, (unsigned long)config.length)) {
		pr_err("%s copy_from_user error\n", __func__);
		kfree(cmd_data);
		return -EFAULT;
	}

	inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, cmd,
			     (char *)cmd_data, (size_t)config.length, NULL);
	kfree(cmd_data);
	return 0;
}

/*
 * Send query WIFI fence status command to contexthub
 */
static int flp_wififence_status_cmd(struct flp_port_t *flp_port, unsigned long arg)
{
	int ret;

	if ((flp_port->channel_type & FLP_WIFIFENCE) == 0) {
		pr_err("[%s]you must add wifence first\n", __func__);
		return -EIO;
	}
	ret = flp_fence_status_cmd(flp_port, arg, SUB_CMD_FLP_GET_WIFENCE_STATUS_REQ);
	return ret;
}

static int flp_wififence_get_size(struct flp_port_t *flp_port, unsigned long arg)
{
	int ret;

	if ((flp_port->channel_type & FLP_WIFIFENCE) == 0) {
		pr_err("%s not start\n", __func__);
		return -EPERM;
	}
	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_WIFENCE_GET_SIZE_REQ,
		NULL, (size_t)0, NULL);
	return ret;
}

/*
 * wififence ioctl message processing
 */
static int flp_wififence_ioctl(struct flp_port_t *flp_port, unsigned int cmd, unsigned long arg)
{
	if (flp_check_cmd(flp_port, cmd, (int)FLP_WIFIFENCE) == 0)
		return -EPERM;

	switch (cmd) {
	case FD_IOCTL_WIFIFENCE_ADD:
		return flp_add_wififence(flp_port, arg);
	case FD_IOCTL_WIFIFENCE_REMOVE:
		return flp_oper_wififence(flp_port, arg, SUB_CMD_FLP_REMOVE_WIFENCE_REQ);
	case FD_IOCTL_WIFIFENCE_PAUSE:
		return flp_oper_wififence(flp_port, arg, SUB_CMD_FLP_PAUSE_WIFENCE_REQ);
	case FD_IOCTL_WIFIFENCE_RESUME:
		return flp_oper_wififence(flp_port, arg, SUB_CMD_FLP_RESUME_WIFENCE_REQ);
	case FD_IOCTL_WIFIFENCE_GET_STATUS:
		return flp_wififence_status_cmd(flp_port, arg);
	case FD_IOCTL_WIFIFENCE_GET_SIZE:
		return flp_wififence_get_size(flp_port, arg);
	default:
		pr_err("[%s]cmd err[%u]\n", __func__, cmd);
		return -EINVAL;
	}
}
#endif

#ifdef CONFIG_SMART_FLP_DIAG
/*
 * Send diag command to contexthub
 */
static int flp_diag_send_cmd(struct flp_port_t *flp_port, unsigned long arg)
{
	unsigned int data_len = 0;
	char *data_buf = NULL;
	int ret = 0;

	if (arg == 0) {
		pr_err("[flperr][%s] invalid param\n", __func__);
		return -EFAULT;
	}

	if (copy_from_user(&data_len, (void *)((uintptr_t)arg), sizeof(data_len))) {
		pr_err("[flperr][%s] copy_from_user error\n", __func__);
		return -EFAULT;
	}

	if (data_len > FLP_DIAG_MAX_CMD_LEN || !data_len) {
		pr_err("[flperr][%s] invalid data_len=%d\n", __func__, data_len);
		return -EFAULT;
	}
	data_buf = kzalloc(data_len + sizeof(unsigned int), GFP_KERNEL);//lint !e838
	if (data_buf == NULL) {
		pr_err("[flperr]%s kzalloc fail\n", __func__);
		return -ENOMEM;
	}
	if (copy_from_user(data_buf, (void *)((uintptr_t)arg), data_len + sizeof(unsigned int))) {
		pr_err("[flperr][%s] copy_from_user error\n", __func__);
		ret = -EFAULT;
		goto FLP_DIAG_EXIT;
	}

	/*if ((flp_port->channel_type & IOMCU_APP_FLP) == 0) {
		ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_OPEN_REQ, 0, NULL, (size_t)0, NULL);
		if (ret != 0) {
			pr_err("[%s]CMD_CMN_OPEN_REQ error\n", __func__);
			goto FLP_DIAG_EXIT;
		}
	}*/

	ret = inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CONFIG_REQ, SUB_CMD_FLP_DIAG_SEND_CMD_REQ,
				(char *)(data_buf + sizeof(unsigned int)), (size_t)data_len, NULL);
	if (ret != 0) {
		pr_err("[%s]GEOFENCE_RM_REQ error\n", __func__);
		goto FLP_DIAG_EXIT;
	}

	pr_info("flp:[%s]data_len[%d]\n", __func__, data_len);
	flp_port->channel_type |= FLP_DIAG;
	g_flp_dev.service_type |= FLP_DIAG;

FLP_DIAG_EXIT:
	if ((g_flp_dev.service_type & IOMCU_APP_FLP) == 0)
		(void)inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CLOSE_REQ, 0, NULL, (size_t)0, NULL);
	kfree(data_buf);
	return ret;
}

/*
 * diag ioctl message processing
 */
static int flp_diag_ioctl(struct flp_port_t *flp_port, unsigned int cmd, unsigned long arg)
{
	if (flp_check_cmd(flp_port, (int)cmd, (int)FLP_DIAG) == 0)
		return -EPERM;
	switch (cmd) {
	case FD_IOCTL_DIAG_SEND_CMD:
		return flp_diag_send_cmd(flp_port, arg);
	default:
		pr_err("[%s]cmd err[%u]\n", __func__, cmd);
		return -EINVAL;
	}
}
#endif /* CONFIG_SMART_FLP_DIAG end */
/*lint -e732*/
/*
 * flp ioctl message processing
 */
static long flp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	long ret;
	struct flp_port_t *flp_port  = (struct flp_port_t *)file->private_data;

	if (flp_port == NULL) {
		pr_err("flp ioctl parameter error\n");
		return -EINVAL;
	}
	pr_info("[%s]cmd[0x%x]\n", __func__, cmd & 0x0FFFF);
	mutex_lock(&g_flp_dev.lock);
	if ((g_flp_dev.denial_sevice != 0) && (cmd != FLP_IOCTL_COMMON_CLOSE_SERVICE)) {
		mutex_unlock(&g_flp_dev.lock);
		return 0;
	}

	switch (cmd & FLP_IOCTL_TYPE_MASK) {
#ifdef CONFIG_SMART_FLP_GEOFENCE
	case FLP_IOCTL_TYPE_GEOFENCE:
		if (!flp_check_cmd(flp_port, (int)cmd, (int)FLP_GEOFENCE)) {
			ret = -EPERM;
			break;
		}
		ret = (long)flp_geofence_ioctl(flp_port, cmd, arg);
		break;
#endif
#ifdef CONFIG_SMART_FLP_BATCHING
	case FLP_IOCTL_TYPE_BATCHING:
		if (!flp_check_cmd(flp_port, (int)cmd, (int)FLP_BATCHING)) {
			ret = -EPERM;
			break;
		}
		ret = (long)flp_location_ioctl(flp_port, cmd, arg);
		break;
#endif
#ifdef CONFIG_SMART_FLP_CELLFENCE
	case FLP_IOCTL_TYPE_CELLFENCE:
		ret = (long)cellfence_ioctl(flp_port, cmd, arg);
		break;
#endif
#ifdef CONFIG_SMART_FLP_WIFIFENCE
	case FLP_IOCTL_TYPE_WIFIFENCE:
		ret = (long)flp_wififence_ioctl(flp_port, cmd, arg);
		break;
#endif
#ifdef CONFIG_SMART_FLP_DIAG
	case FLP_IOCTL_TYPE_DIAG:
		ret = (long)flp_diag_ioctl(flp_port, cmd, arg);
		break;
#endif

	case FLP_IOCTL_TYPE_COMMON:
		ret = (long)flp_common_ioctl(flp_port, cmd, arg);
		break;
	default:
		pr_err("flp ioctl input cmd[0x%x] error\n", cmd);
		ret = -EFAULT;
		break;
	}
	mutex_unlock(&g_flp_dev.lock);
	pr_info("[%s]cmd[0x%x] has processed ret[%ld]\n", __func__, cmd&0x0FFFF, ret);
	return ret;
}
/*lint +e732*/
/*lint -e438*/
/*
 * Device node open function, internal resource initialization
 */
static int flp_open(struct inode *inode, struct file *filp)/*lint -e715*/
{
	int ret = 0;
	struct flp_port_t *flp_port = NULL;
	struct list_head *pos = NULL;
	int count = 0;

	mutex_lock(&g_flp_dev.lock);
		list_for_each(pos, &g_flp_dev.list) {//lint !e838
		count++;
	}

	if (count > 100) {
		pr_err("flp open clinet limit\n");
		ret = -EACCES;
		goto FLP_OPEN_ERR;
	}

	flp_port = kmalloc(sizeof(struct flp_port_t), GFP_KERNEL);
	if (flp_port == NULL) {
		pr_err("flp open no mem\n");
		ret = -ENOMEM;
		goto FLP_OPEN_ERR;
	}
	(void)memset_s(flp_port, sizeof(struct flp_port_t), 0, sizeof(struct flp_port_t));
	INIT_LIST_HEAD(&flp_port->list);
	smart_softtimer_create(&flp_port->sleep_timer,
			flp_sleep_timeout, (unsigned long)((uintptr_t)flp_port), 0);
	INIT_WORK(&flp_port->work, flp_timerout_work);
	mutex_lock(&g_flp_dev.recovery_lock);
	list_add_tail(&flp_port->list, &g_flp_dev.list);
	mutex_unlock(&g_flp_dev.recovery_lock);
	mutex_unlock(&g_flp_dev.lock);
	//wakeup_source_init(&flp_port->wlock, "smart_flp");
	filp->private_data = flp_port;
	pr_info("%s %d: v1.4 enter\n", __func__, __LINE__);
	return 0;
FLP_OPEN_ERR:
	mutex_unlock(&g_flp_dev.lock);
	return ret;
}

static void __flp_release(struct flp_port_t *flp_port)
{
	kfree(flp_port);
}

/*
 * Device node close function, internal resource release
 */
static int flp_release(struct inode *inode, struct file *file)/*lint -e715*/
{
	struct flp_port_t *flp_port  = (struct flp_port_t *)file->private_data;

	pr_info("[%s]\n", __func__);

	if (flp_port == NULL) {
		pr_err("flp_close parameter error\n");
		return -EINVAL;
	}

	smart_softtimer_delete(&flp_port->sleep_timer);
	cancel_work_sync(&flp_port->work);
	//wakeup_source_trash(&flp_port->wlock);

	mutex_lock(&g_flp_dev.lock);
	mutex_lock(&g_flp_dev.recovery_lock);
	list_del(&flp_port->list);
	mutex_unlock(&g_flp_dev.recovery_lock);

	/* if start batching or Geofence function ever */
	if (flp_port->channel_type & IOMCU_APP_FLP) {
		inputhub_wrapper_pack_and_send_cmd(TAG_FLP, CMD_CMN_CLOSE_REQ, 0, NULL, (size_t)0, NULL);
		flp_port->channel_type &= ~IOMCU_APP_FLP;
		g_flp_dev.service_type &= ~IOMCU_APP_FLP;
	}
	__flp_release(flp_port);
	file->private_data = NULL;
	mutex_unlock(&g_flp_dev.lock);
	return 0;
}

#if defined(DDR_SHMEMEXT_ADDR_AP) && defined(DDR_SHMEMEXT_SIZE)
/*
 * Device node mmap function
 */
static int flp_mmap(struct file *filp, struct vm_area_struct *vma)
{
	unsigned long size;
	int ret;
	struct flp_port_t *flp_port;
	unsigned long page = 0;
	unsigned long offset;

	flp_port = (struct flp_port_t *)filp->private_data;
	if (flp_port == NULL) {
		pr_err("[%s]:line[%d] flp parameter error\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (vma->vm_start == 0) {
		pr_err("[%s]:line[%d] Failed : vm_start.0x%lx\n", __func__, __LINE__, vma->vm_start);
		return -EINVAL;
	}

	size = vma->vm_end - vma->vm_start;
	offset = vma->vm_pgoff;
	vma->vm_pgoff = 0;
	pr_info("[%s]:line[%d] enter, vm_start.0x%lx, size.0x%lx, vm_end.0x%lx, offset.0x%lx\n",
	       __func__, __LINE__, vma->vm_start, size, vma->vm_end, offset);

	offset = offset >> MMAP_OFFSET_CHECK_BIT;
	if (offset == OFFLINE_TYPE_CELL) {
		if (size > OFFLINE_CELL_MMAP_SIZE) {
			pr_err("[%s]:line[%d] size.0x%lx.\n", __func__, __LINE__, size);
			return -EINVAL;
		}
		page = DDR_SHMEMEXT_ADDR_AP;
	} else if (offset == OFFLINE_TYPE_INDOOR) {
		if (size > OFFLINE_INDOOR_MMAP_SIZE) {
			pr_err("[%s]:line[%d] current indoor is not set, use cell size[0x%lx].\n", __func__, __LINE__, size);
			return -EINVAL;
		}
		page = OFFLINE_INDOOR_MMAP_ADDR;
	} else {
		pr_err("[%s]:line[%d] Failed : offset[0x%lx] is invalid.\n", __func__, __LINE__, offset);
		return -EINVAL;
	}

	vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
	ret = remap_pfn_range(vma, vma->vm_start, (page >> PAGE_SHIFT), size, vma->vm_page_prot);
	if (ret != 0) {
		pr_err("[%s]:line[%d] remap_pfn_range failed, ret.%d\n", __func__, __LINE__, ret);
		return -EINVAL;
	}
	flp_port->channel_type |= FLP_OFFLINEDB;
	return 0;
}
#else
/*
 * Device node mmap function
 */
static int flp_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct flp_port_t *flp_port;

	flp_port = (struct flp_port_t *)filp->private_data;
	if (flp_port == NULL) {
		pr_err("[%s]:line[%d] flp parameter error\n", __func__, __LINE__);
		return -EINVAL;
	}
	pr_info("[%s]:line[%d] enter, vm_start.0x%lx, vm_end0x%lx, mmap dose not support\n",
	       __func__, __LINE__, vma->vm_start, vma->vm_end);
	return -EINVAL;
}
#endif

static ssize_t flp_write(struct file *file, const char __user *data,
	size_t len, loff_t *ppos)
{
	int ret = 0;
	char *user_data = (char *)kzalloc(len, GFP_KERNEL);
	hwlog_err("%s enter, len = %d\n", __func__, len);

	if (user_data == NULL) {
		hwlog_err("%s user_data NULL\n", __func__);
		return len;
	}

	if (copy_from_user(user_data, data, len)) {
		hwlog_err("%s copy_from_user failed\n", __func__);
		if (user_data != NULL)
			kfree(user_data);
		return len;
	}

	if (((pkt_subcmd_req_t *)user_data)->hd.cmd == CMD_CMN_CONFIG_REQ) {
		if (((pkt_subcmd_req_t *)user_data)->hd.resp == 0) {
			hwlog_err("%s get_data_from_mcu enter \n", __func__);
			ret = get_data_from_mcu((struct pkt_header *)user_data);
		} else {
			hwlog_err("%s flp_rd_info_resp enter \n", __func__);
			ret = flp_rd_info_resp(user_data, len);
		}
	} else if (((pkt_subcmd_req_t *)user_data)->hd.cmd == CMD_CMN_CONFIG_RESP) {
		if (((pkt_subcmd_req_t *)user_data)->hd.resp != 0) {
			hwlog_err("%s flp_rd_info_resp enter \n", __func__);
			ret = flp_rd_info_resp(user_data, len);
		}
	} else if (((pkt_subcmd_req_t *)user_data)->hd.cmd == CMD_SHMEM_AP_RECV_REQ) {
		hwlog_err("%s flp_shmem_resp enter\n", __func__);
		ret = flp_shmem_resp(user_data, len);
	} else if (((pkt_subcmd_req_t *)user_data)->hd.cmd == CMD_CMN_RECOVERY_RESP) {
		flp_service_recovery();
	} else {
		hwlog_err("%s unkonwn flp msg, cmd = %d\n", __func__, ((pkt_subcmd_req_t *)user_data)->hd.cmd);
	}

	if (user_data != NULL)
		kfree(user_data);

	if (ret != 0)
		hwlog_err("[%s] work error\n", __func__);

	return len;
}

/*lint +e438*/
/*lint +e826*/
/*lint -e785 -e64*/
static const struct file_operations flp_fops = {
	.owner          = THIS_MODULE,
	.llseek         = no_llseek,
	.unlocked_ioctl = flp_ioctl,
	.open           = flp_open,
	.release        = flp_release,
	.mmap           = flp_mmap,
#ifdef CONFIG_COMPAT
	.compat_ioctl = flp_ioctl,
#endif
	.write			= flp_write,
};

/* Description:   miscdevice to motion */
static struct miscdevice flp_miscdev = {
	.minor =    MISC_DYNAMIC_MINOR,
	.name =     "flp",
	.fops =     &flp_fops,
};
/*lint +e785 +e64*/

/**********************************************
Function:       flp_register
Data Accessed:  no
Data Updated:   no
Input:          void
Output:        void
Return:        result of function, 0 successed, else false
**********************************************/
/*lint -e655*/
int flp_register(void)
{
	int ret;

	pr_err("[%s] flp_register start!\n", __func__);
	if (get_contexthub_dts_status() != 0) {
		pr_err("[%s] contexthub disabled!\n", __func__);
		return -EINVAL;
	}

	(void)memset_s((void *)&g_flp_dev, sizeof(g_flp_dev), 0, sizeof(g_flp_dev));
	ret = genl_register_family(&flp_genl_family);
	if (ret != 0)
		return ret;
	INIT_LIST_HEAD(&g_flp_dev.list);
	/*ret = inputhub_wrapper_register_event_notifier(TAG_FLP,
			CMD_CMN_CONFIG_REQ, get_data_from_mcu);
	if (ret != 0) {
		pr_err("[%s] register data report process failed!\n", __func__);
		goto register_failed;
	}
	ret = inputhub_wrapper_register_event_notifier(TAG_FLP,
			CMD_SHMEM_AP_RECV_REQ, flp_shmem_resp);
	if (ret != 0) {
		pr_err("[%s] register flp_shmem_resp failed!\n", __func__);
		goto unregister_config_resp;
	}

	register_iom3_recovery_notifier(&sensor_reboot_notify);*/
	mutex_init(&g_flp_dev.lock);
	mutex_init(&g_flp_dev.recovery_lock);
	init_completion(&g_flp_dev.shmem_completion);

	g_flp_dev.wq = create_freezable_workqueue("flp_wq");
	if (!g_flp_dev.wq) {
		pr_err("[%s] create workqueue fail!\n", __func__);
		ret = -EFAULT;
		return ret;
	}

	ret = misc_register(&flp_miscdev);
	if (ret != 0)    {
		pr_err("cannot register smart flp err=%d\n", ret);
		goto destroy_wq;
	}
	pr_info("flp register success\n");
	return 0;

destroy_wq:
	destroy_workqueue(g_flp_dev.wq);
/*unregister_shmem_resp:
	(void)inputhub_wrapper_unregister_event_notifier(TAG_FLP,
		CMD_SHMEM_AP_RECV_REQ, flp_shmem_resp);
unregister_config_resp:
	(void)inputhub_wrapper_unregister_event_notifier(TAG_FLP,
		CMD_CMN_CONFIG_REQ, get_data_from_mcu);
register_failed:
	genl_unregister_family(&flp_genl_family);*/
	return ret;
}
EXPORT_SYMBOL_GPL(flp_register);
/*lint +e655*/

/*************************************************************
Function:       flp_unregister
Description:    unregister for Fused Location Provider function
Data Accessed:  no
Data Updated:   no
Input:          void
Output:        void
Return:        void
**************************************************************/
int flp_unregister(void)
{
	destroy_workqueue(g_flp_dev.wq);
	/*(void)inputhub_wrapper_unregister_event_notifier(TAG_FLP,
		CMD_SHMEM_AP_RECV_REQ, flp_shmem_resp);
	(void)inputhub_wrapper_unregister_event_notifier(TAG_FLP, CMD_CMN_CONFIG_REQ, get_data_from_mcu);*/
	genl_unregister_family(&flp_genl_family);
	misc_deregister(&flp_miscdev);
	return 0;
}
EXPORT_SYMBOL_GPL(flp_unregister);
