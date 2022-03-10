/*
 * hwril_sim_sd.c
 *
 * generic netlink for hwril-sim-sd
 *
 * Copyright (c) 2020-2021 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include "hwril_sim_sd.h"
#include <linux/if.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/version.h>
#include <net/genetlink.h>

static const struct nla_policy hwril_sim_sd_policy[HWRIL_SIM_SD_ATTR_MAX + 1] = {
	[HWRIL_SIM_SD_ATTR_SIM_DET_RESULT] = { .type = NLA_U32 },
	[HWRIL_SIM_SD_ATTR_NMC_DET_CARD_TYPE] = { .type = NLA_U32 },
};

static sim_nmc_handler g_sim_sd_cb = NULL;

static int hwril_sim_sd_get_nmc_detect_result(struct sk_buff *skb, struct genl_info *info);
static int hwril_sim_sd_sim_detect_result_notify(struct sk_buff *skb, struct genl_info *info);

static struct genl_ops hwril_sim_sd_genl_ops[] = {
	{
		.cmd = HWRIL_SIM_SD_CMD_GET_CARD_TYPE,
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,19,0)
		.policy = hwril_sim_sd_policy,
#else
		.validate = GENL_DONT_VALIDATE_STRICT | GENL_DONT_VALIDATE_DUMP,
#endif
		.doit = hwril_sim_sd_get_nmc_detect_result
	},
	{
		.cmd = HWRIL_SIM_SD_CMD_SIM_DET_RET_NOTIFY,
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,19,0)
		.policy = hwril_sim_sd_policy,
#else
		.validate = GENL_DONT_VALIDATE_STRICT | GENL_DONT_VALIDATE_DUMP,
#endif
		.doit = hwril_sim_sd_sim_detect_result_notify
	},
};

struct genl_multicast_group hwril_sim_sd_mcgrps[] = {
	{ .name = "events" },
};

static struct genl_family hwril_sim_sd_genl_family = {
	.hdrsize = 0,
	.name = HWRIL_SIM_SD_GENL_FAMILY,
	.version = 1,
	.maxattr = HWRIL_SIM_SD_ATTR_MAX,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)
	.policy = hwril_sim_sd_policy,
#endif
	.ops = hwril_sim_sd_genl_ops,
	.n_ops = ARRAY_SIZE(hwril_sim_sd_genl_ops),
	.mcgrps = hwril_sim_sd_mcgrps,
	.n_mcgrps = ARRAY_SIZE(hwril_sim_sd_mcgrps),
};

int register_sim_sd_handler(sim_nmc_handler cb)
{
	pr_info("%s, enter, update cb", __func__);
	g_sim_sd_cb = cb;
	return 0;
}

int hwril_sim_sd_nmc_detect_result_notify(u32 card_type)
{
	struct sk_buff *skb = NULL;
	void *hdr = NULL;
	pr_info("%s: enter, card_type is %u", __func__, card_type);

	skb = genlmsg_new(GENLMSG_DEFAULT_SIZE, GFP_ATOMIC);
	if (!skb) {
		pr_info("%s: no mem", __func__);
		return -ENOMEM;
	}
	hdr = genlmsg_put(skb, 0, 0, &hwril_sim_sd_genl_family, 0,
		HWRIL_SIM_SD_CMD_NMC_DET_CARD_TYPE_NOTIFY);
	if (!hdr) {
		pr_info("%s: msg size error", __func__);
		nlmsg_free(skb);
		return -EMSGSIZE;
	}
	if (nla_put_u32(skb, HWRIL_SIM_SD_ATTR_NMC_DET_CARD_TYPE, card_type)) {
		pr_info("%s: msg size error", __func__);
		genlmsg_cancel(skb, hdr);
		nlmsg_free(skb);
		return -EMSGSIZE;
	}
	genlmsg_end(skb, hdr);
	return genlmsg_multicast(&hwril_sim_sd_genl_family, skb, 0, 0, GFP_ATOMIC);
}

static int hwril_sim_sd_get_nmc_detect_result(struct sk_buff *skb, struct genl_info *info)
{
	pr_info("%s: enter", __func__);
	if (g_sim_sd_cb != NULL) {
		pr_info("%s: send msg to nmc to get card type!!!", __func__);
		g_sim_sd_cb(GET_NMC_DETECT_RESULT, 0);
	} else {
		pr_info("%s: g_sim_sd_cb is NULL, send unknown msg to rild!!!", __func__);
		hwril_sim_sd_nmc_detect_result_notify(0);
	}
	return 0;
}

static int hwril_sim_sd_sim_detect_result_notify(struct sk_buff *skb, struct genl_info *info)
{
	u32 result = 0;

	pr_info("%s: enter", __func__);
	if (!info || !info->attrs) {
		pr_info("%s: null attr", __func__);
		return 0;
	}
	if (info->attrs[HWRIL_SIM_SD_ATTR_SIM_DET_RESULT])
		result = nla_get_u32(info->attrs[HWRIL_SIM_SD_ATTR_SIM_DET_RESULT]);
	pr_info("%s: sim_detect_result is %u", __func__, result);

	if (g_sim_sd_cb != NULL) {
		pr_info("%s: send sim_detect_result to nmc", __func__);
		g_sim_sd_cb(SIM_DETECT_RESULT_NOTIFY, result);
	} else {
		pr_info("%s: g_sim_sd_cb is NULL, do nothing", __func__);
	}
	return 0;
}

static int __init hwril_sim_sd_init(void)
{
	int err;

	pr_info("hwril_sim_sd: init");
	err = genl_register_family(&hwril_sim_sd_genl_family);
	if (err < 0) {
		pr_err("hwril_sim_sd: genl_register_family failed %d", __func__, err);
		return err;
	}

	return 0;
}

static void __exit hwril_sim_sd_exit(void)
{
	int err;

	pr_info("hwril_sim_sd: exit");
	err = genl_unregister_family(&hwril_sim_sd_genl_family);
	if (err < 0)
		pr_err("hwril_sim_sd: genl_unregister_family failed %d", err);
}

module_init(hwril_sim_sd_init)
module_exit(hwril_sim_sd_exit)
