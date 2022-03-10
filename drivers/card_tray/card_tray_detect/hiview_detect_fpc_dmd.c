/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: This Hiview detect probe dmd
 * Author: wangjingjin@huawei.com
 * Create: 2021-08-30
 */

#include <linux/workqueue.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <log/hiview_hievent.h>
#include <log/hw_log.h>
#include "hiview_detect_fpc_dmd.h"

#define FPC_FALL_IMONITOR_ID 926007507
#define HWLOG_TAG hiview_detect_fpc_dmd
HWLOG_REGIST();
#define FPC_DMD_DELAY_TIME  15000

static int g_60pin_btb_gpio = -1;
struct delayed_work dwork;

static void HiviewDetectFpcDmd(struct work_struct *work)
{
    struct hiview_hievent *hi_event = NULL;

    hi_event = hiview_hievent_create(FPC_FALL_IMONITOR_ID);
    if (!hi_event) {
        hwlog_err("%s create hievent fail\n", __func__);
        return;
    }
    hiview_hievent_put_string(hi_event, "FPC", "fracture");
    hiview_hievent_put_string(hi_event, "Detect", "60pin_fpc");
    hiview_hievent_report(hi_event);
    hiview_hievent_destroy(hi_event);
    return;
}

int BtbFpcDetect(struct card_tray_info *di,
    struct device_node *card_tray_node)
{
    int ret;
    int btb_check;
    struct pinctrl *btb_pctrl = NULL;
    struct pinctrl_state *btb_pctrl_state = NULL;
    int btb_gpio_value;

    btb_check = of_property_read_u32(card_tray_node, "btb_60pin_gpio",
        (u32 *)(&g_60pin_btb_gpio));
    if (btb_check) {
        hwlog_err("%s : btb_60pin_gpio read failed!\n", __func__);
        return -1;
    }
    btb_pctrl = devm_pinctrl_get(di->dev);
    if (IS_ERR(btb_pctrl)) {
        hwlog_err("failed to devm pinctrl get\n");
        ret = -EINVAL;
        return ret;
    }
    btb_pctrl_state = pinctrl_lookup_state(btb_pctrl, "btb_default");
    if (IS_ERR(btb_pctrl_state)) {
        hwlog_err("failed to pinctrl lookup state default\n");
        ret = -EINVAL;
        return ret;
    }
    ret = pinctrl_select_state(btb_pctrl, btb_pctrl_state);
    if (ret < 0) {
        hwlog_err("set iomux normal error, %d\n", ret);
        return ret;
    }

    msleep(1);
    if (g_60pin_btb_gpio < 0) {
        hwlog_err("%s : The 60pin_bin_gpio is error!\n", __func__);
        return -1;
    }
    btb_gpio_value = gpio_get_value(g_60pin_btb_gpio);
    if (btb_gpio_value) {
        hwlog_err("%s : Failed to get gpio180 value!\n", __func__);
        INIT_DELAYED_WORK(&dwork, HiviewDetectFpcDmd);
        schedule_delayed_work(&dwork, FPC_DMD_DELAY_TIME);
    }
    return 0;
}
