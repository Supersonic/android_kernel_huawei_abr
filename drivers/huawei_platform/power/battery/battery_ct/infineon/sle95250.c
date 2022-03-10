/*
 * sle95250.c
 *
 * sle95250 driver
 *
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
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

#include "sle95250.h"
#include <linux/cpumask.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/memory.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/preempt.h>
#include <linux/slab.h>
#include <linux/smp.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/sched.h>
#include <uapi/linux/sched/types.h>
#include <chipset_common/hwpower/battery/battery_type_identify.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <huawei_platform/log/hw_log.h>
#include <huawei_platform/power/power_mesg_srv.h>
#include "../batt_early_param.h"
#include "include/optiga_nvm.h"
#include "include/optiga_reg.h"
#include "include/optiga_swi.h"

#ifdef BATTBD_FORCE_MATCH
#include "../batt_cpu_manager.h"
#endif /* BATTBD_FORCE_MATCH */

#ifdef HWLOG_TAG
#undef HWLOG_TAG
#endif
#define HWLOG_TAG battct_sle95250
HWLOG_REGIST();

#ifdef BATTBD_FORCE_MATCH
static void sle95250_dev_on(struct sle95250_dev *di)
{
	struct sched_param param = { .sched_priority = MAX_RT_PRIO - 1 };

	pm_qos_add_request(&di->pm_qos, PM_QOS_CPU_DMA_LATENCY, 0);
	kick_all_cpus_sync();

	if (sched_setscheduler_nocheck(current, SCHED_FIFO, &param))
		hwlog_err("set current fifo fail\n");
	else
		hwlog_info("set current fifo sucess\n");

	if (di->sysfs_mode)
		bat_type_apply_mode(BAT_ID_SN);
	optiga_swi_slk_irqsave(di);
	hwlog_info("sle95250_dev_on, ic_index:%u\n", di->ic_index);
	optiga_swi_pwr_off(di);
	optiga_swi_pwr_on(di);
	opiga_select_chip_addr(di, OPTIGA_SINGLE_CHIP_ADDR);
}

static void sle95250_dev_off(struct sle95250_dev *di)
{
	struct sched_param param = { .sched_priority = 0 };

	hwlog_info("sle95250_dev_off, ic_index:%u\n", di->ic_index);
	optiga_swi_pwr_off_by_cmd(di);
	optiga_swi_slk_irqstore(di);
	if (di->sysfs_mode)
		bat_type_release_mode(true);

	if (sched_setscheduler_nocheck(current, SCHED_NORMAL, &param))
		hwlog_err("set current normal fail\n");
	pm_qos_remove_request(&di->pm_qos);
}

static int sle95250_cyclk_set(struct sle95250_dev *di)
{
	int ret;

	if (!di)
		return -ENODEV;

	sle95250_dev_on(di);
	ret = optiga_reg_life_span_lock_set(di);
	sle95250_dev_off(di);
	if (ret) {
		hwlog_err("%s: lock fail, ic_index:%u\n", __func__, di->ic_index);
		return -EAGAIN;
	}

	return 0;
}

static int sle95250_pglk_iqr(struct sle95250_dev *di, uint8_t *pg_lked)
{
	int ret;
	int rty;
	uint8_t pg_lked_loc;

	sle95250_dev_on(di);
	for (rty = 0; rty < SLE95250_PGLK_RTY; rty++) {
		ret = optiga_reg_pglk_read(di, &pg_lked_loc);
		if (ret)
			goto pglk_iqr_fail;

		*pg_lked = pg_lked_loc;

		ret = optiga_reg_pglk_read(di, &pg_lked_loc);
		if (ret)
			goto pglk_iqr_fail;

		ret = (pg_lked_loc != *pg_lked);
		if (ret)
			continue;

		break;
	}

	if (ret)
		goto pglk_iqr_fail;

	sle95250_dev_off(di);
	return 0;
pglk_iqr_fail:
	sle95250_dev_off(di);
	hwlog_err("%s: fail, ic_index:%u\n", __func__, di->ic_index);
	return ret;
}
#endif /* BATTBD_FORCE_MATCH */

static enum batt_ic_type sle95250_get_ic_type(void)
{
	return INFINEON_SLE95250_TYPE;
}

static int sle95250_get_uid(struct platform_device *pdev,
	const unsigned char **uuid, unsigned int *uuid_len)
{
	struct sle95250_dev *di = NULL;

	di = platform_get_drvdata(pdev);
	if (!di)
		return -ENODEV;

	if (!uuid || !uuid_len || !pdev) {
		hwlog_err("%s: pointer NULL\n", __func__);
		return -EINVAL;
	}

	if (!di->mem.uid_ready) {
		hwlog_err("%s: no uid available, ic_index:%u\n", __func__, di->ic_index);
		return -EINVAL;
	}

	*uuid = di->mem.uid;
	*uuid_len = SLE95250_UID_LEN;
	return 0;
}

#ifdef BATTBD_FORCE_MATCH
static void sle95250_crc16_cal(uint8_t *msg, int len, uint16_t *crc)
{
	int i, j;
	uint16_t crc_mul = 0xA001; /* G(x) = x ^ 16 + x ^ 15 + x ^ 2 + 1 */

	*crc = CRC16_INIT_VAL;
	for (i = 0; i < len; i++) {
		*crc ^= *(msg++);
		for (j = 0; j < BIT_P_BYT; j++) {
			if (*crc & ODD_MASK)
				*crc = (*crc >> 1) ^ crc_mul;
			else
				*crc >>= 1;
		}
	}
}
#endif

static int sle95250_get_batt_type(struct platform_device *pdev,
	const unsigned char **type, unsigned int *type_len)
{
	struct sle95250_dev *di = NULL;

	di = platform_get_drvdata(pdev);
	if (!di)
		return -ENODEV;

	if (!pdev || !type || !type_len) {
		hwlog_err("%s: pointer NULL\n", __func__);
		return -EINVAL;
	}

	if (!di->mem.batttp_ready) {
		hwlog_err("%s: no batt_type info available, ic_index:%u\n", __func__, di->ic_index);
		return -EINVAL;
	}

	*type = di->mem.batt_type;
	*type_len = SLE95250_BATTTYP_LEN;
	hwlog_info("[%s]Btp0:0x%x; Btp1:0x%x, ic_index:%u\n", __func__, di->mem.batt_type[0],
		di->mem.batt_type[1], di->ic_index);

	return 0;
}

#ifdef BATTBD_FORCE_MATCH
static void sle95250_bin2prt(uint8_t *sn_print,
	uint8_t *sn_bin, int sn_print_size)
{
	uint8_t hex;
	uint8_t byt_cur;
	uint8_t byt_cur_bin = 0;
	uint8_t bit_cur = 0;

	for (byt_cur = 0; byt_cur < SLE95250_SN_ASC_LEN; byt_cur++) {
		if (!(SLE95250_SN_HEXASC_MASK & BIT(byt_cur))) {
			if (!(bit_cur % BIT_P_BYT))
				sn_print[byt_cur] = sn_bin[byt_cur_bin];
			else
				sn_print[byt_cur] =
					((sn_bin[byt_cur_bin] &
					LOW4BITS_MASK) << 4) | /* high 4 bits */
					((sn_bin[byt_cur_bin + 1] &
					HIGH4BITS_MASK) >> 4); /* low 4 bits */
			bit_cur += BIT_P_BYT;
			byt_cur_bin += 1;
		} else {
			if (bit_cur % BIT_P_BYT == 0) {
				hex = (sn_bin[byt_cur_bin] &
					HIGH4BITS_MASK) >> 4; /* low 4 bits */
			} else {
				hex = sn_bin[byt_cur_bin] & LOW4BITS_MASK;
				byt_cur_bin += 1;
			}
			snprintf(sn_print + byt_cur,
				sn_print_size - byt_cur, "%X", hex);
			/* bit index in current byte */
			bit_cur += BIT_P_BYT / 2;
		}
	}
}
#endif

static int sle95250_get_batt_sn(struct platform_device *pdev,
	struct power_genl_attr *res, const unsigned char **sn, unsigned int *sn_size)
{
#ifdef BATTBD_FORCE_MATCH
	int ret;
	uint8_t rty;
	uint8_t sn_bin[SLE95250_SN_BIN_LEN] = { 0 };
	/* extra 1 bytes for \0 */
	char sn_print[SLE95250_SN_ASC_LEN + 1] = { 0 };
#endif
	struct sle95250_dev *di = NULL;

	if (!pdev || !sn || !sn_size) {
		hwlog_err("[%s] pointer NULL\n", __func__);
		return -EINVAL;
	}
	(void)res;
	di = platform_get_drvdata(pdev);
	if (!di)
		return -EINVAL;
	if (di->mem.sn_ready) {
		hwlog_info("[%s] sn in mem already, ic_index:%u\n", __func__, di->ic_index);
		*sn = di->mem.sn;
		*sn_size = SLE95250_SN_ASC_LEN;
		return 0;
	}

#ifndef BATTBD_FORCE_MATCH
	hwlog_info("[%s] operation banned in user mode, ic_index:%u\n", __func__, di->ic_index);
	return -EINVAL;
#else
	ret = batt_cpu_single_core_set();
	if (ret) {
		hwlog_err("%s: single core set fail, ic_index:%u\n", __func__, di->ic_index);
		goto get_sn_fail;
	}

	sle95250_dev_on(di);
	for (rty = 0; rty < SLE95250_SN_RTY; rty++) {
		ret = optiga_nvm_read_with_check(di, SLE95250_RO_AREA,
			SLE95250_SN_PG_OFFSET, SLE95250_SN_BYT_OFFSET,
			SLE95250_SN_BIN_LEN - BYT_P_PG_NVM, sn_bin);
		if (ret)
			continue;

		ret = optiga_nvm_read_with_check(di, SLE95250_RO_AREA,
			SLE95250_SN_PG_OFFSET + 1, BYT_OFFSET0, BYT_P_PG_NVM,
			&sn_bin[SLE95250_SN_BIN_LEN - BYT_P_PG_NVM]);
		if (ret)
			continue;

		break;
	}
	sle95250_dev_off(di);

	if (ret) {
		hwlog_err("%s: fail, ic_index:%u\n", __func__, di->ic_index);
		goto get_sn_fail;
	}

	sle95250_bin2prt(sn_print, sn_bin, SLE95250_SN_ASC_LEN + 1);
	memcpy(di->mem.sn, sn_print, SLE95250_SN_ASC_LEN);
	di->mem.sn_ready = true;
	*sn = di->mem.sn;
	*sn_size = SLE95250_SN_ASC_LEN;

	batt_turn_on_all_cpu_cores();
	return 0;
get_sn_fail:
	batt_turn_on_all_cpu_cores();
	return -EINVAL;
#endif /* BATTBD_FORCE_MATCH */
}

static int sle95250_certification(struct platform_device *pdev,
	struct power_genl_attr *res, enum key_cr *result)
{
	struct sle95250_dev *di = NULL;

	di = platform_get_drvdata(pdev);
	if (!di)
		return -ENODEV;

	if (!pdev || !res || !result) {
		hwlog_err("%s: pointer NULL\n", __func__);
		return -EINVAL;
	}

	if (!di->mem.ecce_pass) {
		hwlog_err("%s: ecce_pass %d, ic_index:%u\n", __func__, di->mem.ecce_pass, di->ic_index);
		*result = KEY_FAIL_UNMATCH;
		return -EINVAL;
	}

	*result = KEY_PASS;
	return 0;
}

static int sle95250_ct_read(struct sle95250_dev *di)
{
	if (di->mem.uid_ready)
		return 0;
	else
		return -EINVAL;
}

static int sle95250_act_read(struct sle95250_dev *di)
{
#ifdef BATTBD_FORCE_MATCH
	int ret;
	int rty;
	int page_ind;
	int page_max;
	int read_len;
	uint16_t crc_act_read;
	uint16_t crc_act_cal;
	uint8_t page_data0[BYT_P_PG_NVM];
#endif

	if (di->mem.act_sig_ready) {
		hwlog_info("[%s] act_sig in memory already, ic_index:%u\n", __func__, di->ic_index);
		return 0;
	}

#ifndef BATTBD_FORCE_MATCH
	hwlog_info("[%s] operation banned in user mode, ic_index:%u\n", __func__, di->ic_index);
	return -EINVAL;
#else
	ret = batt_cpu_single_core_set();
	if (ret) {
		hwlog_err("%s: single core set fail, ic_index:%u\n", __func__, di->ic_index);
		goto act_read_fail;
	}

	rty = 0;
	page_max = (SLE95250_ACT_LEN + BYT_P_PG_NVM - 1) / BYT_P_PG_NVM;
	sle95250_dev_on(di);
	for (page_ind = 0; page_ind < page_max; page_ind++) {
		read_len = (SLE95250_ACT_LEN < (page_ind + 1) * BYT_P_PG_NVM) ?
			(SLE95250_ACT_LEN % BYT_P_PG_NVM) : BYT_P_PG_NVM;
		ret = optiga_nvm_read(di, SLE95250_USER_AREA, page_ind, BYT_OFFSET0,
			read_len, page_data0);
		if (ret) {
			page_ind--;
			rty++;
			if (rty >= SLE95250_ACT_RTY)
				break;
			continue;
		}

		memcpy(di->mem.act_sign + page_ind * BYT_P_PG_NVM, page_data0,
			read_len);

		if (page_ind == (page_max - 1)) {
			memcpy((u8 *)&crc_act_read,
				&page_data0[SLE95250_ACT_CRC_BYT0],
				SLE95250_ACT_CRC_LEN);
			sle95250_crc16_cal(di->mem.act_sign,
				(int)(di->mem.act_sign[0] + 1), &crc_act_cal);
			ret = (crc_act_cal != crc_act_read);
			if (ret) {
				page_ind = -1;
				rty++;
				if (rty >= SLE95250_ACT_RTY)
					break;
				continue;
			}
		}
	}
	sle95250_dev_off(di);

	if (rty)
		hwlog_info("[%s] rty : %d, ic_index:%u\n", __func__, rty, di->ic_index);

	if (ret) {
		hwlog_err("%s act read fail, ic_index:%u\n", __func__, di->ic_index);
		goto act_read_fail;
	}

	di->mem.act_sig_ready = true;
	batt_turn_on_all_cpu_cores();
	return 0;
act_read_fail:
	batt_turn_on_all_cpu_cores();
	return ret;
#endif /* BATTBD_FORCE_MATCH */
}

#ifdef BATTBD_FORCE_MATCH
static int sle95250_act_write(struct sle95250_dev *di, uint8_t *act)
{
	int ret;
	int retry;
	int page_ind;
	int page_max;
	int write_len;
	uint8_t page_data_w[BYT_P_PG_NVM];
	uint8_t page_data_r[BYT_P_PG_NVM];

	page_max = (SLE95250_ACT_LEN + BYT_P_PG_NVM - 1) / BYT_P_PG_NVM;
	retry = 0;
	sle95250_dev_on(di);
	for (page_ind = 0; page_ind < page_max; page_ind++) {
		write_len = (SLE95250_ACT_LEN < (page_ind + 1) * BYT_P_PG_NVM) ?
			(SLE95250_ACT_LEN % BYT_P_PG_NVM) : BYT_P_PG_NVM;
		memcpy(page_data_w, act + page_ind * BYT_P_PG_NVM, write_len);
		ret = optiga_nvm_write(di, SLE95250_USER_AREA, page_ind,
			BYT_OFFSET0, write_len, page_data_w);
		if (ret) {
			page_ind--;
			retry++;
			if (retry >= SLE95250_ACT_RTY)
				break;
			continue;
		}

		ret = optiga_nvm_read(di, SLE95250_USER_AREA, page_ind, BYT_OFFSET0,
			write_len, page_data_r);
		if (ret) {
			page_ind--;
			retry++;
			if (retry >= SLE95250_ACT_RTY)
				break;
			continue;
		}

		ret = memcmp(page_data_r, page_data_w, write_len);
		if (ret) {
			page_ind--;
			retry++;
			if (retry >= SLE95250_ACT_RTY)
				break;
			continue;
		}
	}
	sle95250_dev_off(di);

	if (ret) {
		hwlog_err("%s: fail, ic_index:%u\n", __func__, di->ic_index);
		return ret;
	}

	di->mem.act_sig_ready = false;
	return 0;
}
#endif

static int sle95250_set_act_signature(struct platform_device *pdev,
	enum res_type type, const struct power_genl_attr *res)
{
#ifdef BATTBD_FORCE_MATCH
	int ret;
	uint16_t crc_act;
	uint8_t pglked;
	uint8_t act[SLE95250_ACT_LEN] = { 0 };
	struct sle95250_dev *di = NULL;
#endif

#ifndef BATTBD_FORCE_MATCH
	hwlog_info("[%s] operation banned in user mode\n", __func__);
	return 0;
#else
	di = platform_get_drvdata(pdev);
	if (!di)
		return -ENODEV;

	if (!pdev || !res) {
		hwlog_err("%s: pointer NULL\n", __func__);
		return -EINVAL;
	}

	if (res->len > SLE95250_ACT_LEN) {
		hwlog_err("%s: input act_sig oversize, ic_index:%u\n", __func__, di->ic_index);
		return -EINVAL;
	}

	ret = batt_cpu_single_core_set();
	if (ret) {
		hwlog_err("%s: single core set fail, ic_index:%u\n", __func__, di->ic_index);
		goto act_sig_set_fail;
	}

	ret = sle95250_pglk_iqr(di, &pglked);
	if (ret) {
		hwlog_err("%s: pglk iqr fail, ic_index:%u\n", __func__, di->ic_index);
		goto act_sig_set_succ;
	}
	if (pglked != SLE95250_NOPGLK_MASK) {
		hwlog_err("%s: smpg locked, act set abandon, ic_index:%u\n", __func__, di->ic_index);
		goto act_sig_set_succ;
	}

	memcpy(act, res->data, res->len);

	/* attach crc16 suffix */
	sle95250_crc16_cal(act, res->len, &crc_act);
	memcpy(act + sizeof(act) - sizeof(crc_act), (uint8_t *)&crc_act,
		sizeof(crc_act));

	switch (type) {
	case RES_ACT:
		ret = sle95250_act_write(di, act);
		if (ret) {
			hwlog_err("%s: act write fail, ic_index:%u\n", __func__, di->ic_index);
			goto act_sig_set_fail;
		}
		di->mem.act_sig_ready = false;
		break;
	default:
		hwlog_err("%s: invalid option, ic_index:%u\n", __func__, di->ic_index);
		goto act_sig_set_fail;
	}

act_sig_set_succ:
	batt_turn_on_all_cpu_cores();
	return 0;
act_sig_set_fail:
	batt_turn_on_all_cpu_cores();
	return -EINVAL;
#endif /* BATTBD_FORCE_MATCH */
}

static int sle95250_prepare(struct platform_device *pdev, enum res_type type,
	struct power_genl_attr *res)
{
	int ret;
	struct sle95250_dev *di = NULL;

	if (!pdev || !res) {
		hwlog_err("%s: pointer NULL\n", __func__);
		return -EINVAL;
	}

	di = platform_get_drvdata(pdev);
	if (!di)
		return -ENODEV;

	switch (type) {
	case RES_CT:
		ret = sle95250_ct_read(di);
		if (ret) {
			hwlog_err("%s: res_ct read fail, ic_index:%u\n", __func__, di->ic_index);
			goto prepare_fail;
		}
		res->data = (const unsigned char *)di->mem.uid;
		res->len = SLE95250_UID_LEN;
		break;
	case RES_ACT:
		ret = sle95250_act_read(di);
		if (ret) {
			hwlog_err("%s: res_act read fail, ic_index:%u\n", __func__, di->ic_index);
			goto prepare_fail;
		}
		res->data = (const unsigned char *)di->mem.act_sign;
		res->len = SLE95250_ACT_LEN;
		break;
	default:
		hwlog_err("%s: invalid option, ic_index:%u\n", __func__, di->ic_index);
		res->data = NULL;
		res->len = 0;
	}

	return 0;
prepare_fail:
	return -EINVAL;
}

static int sle95250_set_cert_ready(struct sle95250_dev *di)
{
#ifdef BATTBD_FORCE_MATCH
	int ret;
#endif

#ifndef BATTBD_FORCE_MATCH
	hwlog_info("[%s] operation banned in user mode, ic_index:%u\n", __func__, di->ic_index);
	return 0;
#else
	ret = batt_cpu_single_core_set();
	if (ret) {
		hwlog_err("%s: single core set fail, ic_index:%u\n", __func__, di->ic_index);
		goto cert_ready_set_fail;
	}

	sle95250_dev_on(di);
	ret = optiga_reg_pglk_set(di, SLE95250_ALLPGLK_MASK);
	sle95250_dev_off(di);
	if (ret)
		hwlog_err("%s: set_cert_ready fail, ic_index:%u\n", __func__, di->ic_index);
	else
		di->mem.cet_rdy = CERT_READY;

	batt_turn_on_all_cpu_cores();
	return ret;
cert_ready_set_fail:
	batt_turn_on_all_cpu_cores();
	return -EINVAL;
#endif /* BATTBD_FORCE_MATCH */
}

#ifdef BATTBD_FORCE_MATCH
static int sle95250_set_batt_as_org(struct sle95250_dev *di)
{
	int ret;

	ret = batt_cpu_single_core_set();
	if (ret) {
		hwlog_err("%s: single core set fail, ic_index:%u\n", __func__, di->ic_index);
		goto org_set_fail;
	}

	ret = sle95250_cyclk_set(di);
	if (ret)
		hwlog_err("%s: set_batt_as_org fail, ic_index:%u\n", __func__, di->ic_index);
	else
		di->mem.source = BATTERY_ORIGINAL;

	batt_turn_on_all_cpu_cores();
	return ret;
org_set_fail:
	batt_turn_on_all_cpu_cores();
	return -EINVAL;
}
#else
static int sle95250_set_batt_as_org(struct sle95250_dev *di)
{
	hwlog_info("[%s] operation banned in user mode, ic_index:%u\n", __func__, di->ic_index);
	return 0;
}
#endif /* BATTBD_FORCE_MATCH */

static int sle95250_check_spar_batt(struct sle95250_dev *di,
	enum batt_source *cell_type)
{
	*cell_type = di->mem.source;
	return 0;
}

static int sle95250_set_batt_safe_info(struct platform_device *pdev,
	enum batt_safe_info_t type, void *value)
{
	int ret;
	enum batt_source cell_type;
	struct sle95250_dev *di = NULL;

	if (!pdev || !value) {
		hwlog_err("%s: pointer NULL\n", __func__);
		return -EINVAL;
	}
	di = platform_get_drvdata(pdev);
	if (!di)
		return -EINVAL;

	switch (type) {
	case BATT_CHARGE_CYCLES:
		break;
	case BATT_SPARE_PART:
		ret = sle95250_check_spar_batt(di, &cell_type);
		if (ret) {
			hwlog_err("%s: org_spar check fail, ic_index:%u\n", __func__, di->ic_index);
			goto battinfo_set_succ;
		}
		if (cell_type == BATTERY_ORIGINAL) {
			hwlog_info("[%s]: has been org, quit work, ic_index:%u\n", __func__, di->ic_index);
			goto battinfo_set_succ;
		}

		if (*((enum batt_source *)value) == BATTERY_ORIGINAL) {
			ret = sle95250_set_batt_as_org(di);
			if (ret) {
				hwlog_err("%s: set_as_org fail, ic_index:%u\n", __func__, di->ic_index);
				goto battinfo_set_fail;
			}
		}
		break;
	case BATT_CERT_READY:
		ret = sle95250_set_cert_ready(di);
		if (ret) {
			hwlog_err("%s: set_cert_ready fail, ic_index:%u\n", __func__, di->ic_index);
			goto battinfo_set_fail;
		}
		di->mem.sn_ready = false;
		break;
	default:
		hwlog_err("%s: invalid option, ic_index:%u\n", __func__, di->ic_index);
		goto battinfo_set_fail;
	}

battinfo_set_succ:
	return 0;
battinfo_set_fail:
	return -EINVAL;
}

static void sle95250_ck_cert_ready(struct sle95250_dev *di,
	enum batt_cert_state *cet_rdy)
{
	*cet_rdy = di->mem.cet_rdy;
}

static int sle95250_get_batt_safe_info(struct platform_device *pdev,
	enum batt_safe_info_t type, void *value)
{
	int ret;
	enum batt_source cell_type;
	struct sle95250_dev *di = NULL;

	if (!pdev || !value) {
		hwlog_err("%s: pointer NULL\n", __func__);
		return -EINVAL;
	}

	di = platform_get_drvdata(pdev);
	if (!di)
		return -ENODEV;

	switch (type) {
	case BATT_CHARGE_CYCLES:
		*(int *)value = BATT_INVALID_CYCLES;
		break;
	case BATT_SPARE_PART:
		ret = sle95250_check_spar_batt(di, &cell_type);
		if (ret) {
			hwlog_err("%s: org_spar check fail, ic_index:%u\n", __func__, di->ic_index);
			goto battinfo_get_fail;
		}
		*(enum batt_source *)value = cell_type;
		break;
	case BATT_CERT_READY:
		sle95250_ck_cert_ready(di, (enum batt_cert_state *)value);
		break;
	case BATT_MATCH_INFO:
		hwlog_err("%s: group_sn_ready:%d, ic_index:%u\n",
			__func__, di->mem.group_sn_ready, di->ic_index);
		if (!di->mem.group_sn_ready) {
			*(uint8_t **)value = NULL;
			goto battinfo_get_fail;
		}
		*(uint8_t **)value = di->mem.group_sn;
		break;
	default:
		hwlog_err("%s: invalid option, ic_index:%u\n", __func__, di->ic_index);
		goto battinfo_get_fail;
	}

	return 0;
battinfo_get_fail:
	return -EINVAL;
}

static void sle95250_cert_info_parse(struct sle95250_dev *di)
{
	uint8_t cert0[sizeof(struct batt_cert_para)] = { 0 };
	struct batt_cert_para *cert = NULL;

	if (!di)
		return;

	cert = batt_get_cert_para(di->ic_index);
	if (!cert) {
		hwlog_err("%s: cert info NULL\n", __func__);
		return;
	}

	if (!memcmp(cert0, (uint8_t *)cert, sizeof(struct batt_cert_para))) {
		hwlog_err("%s: cert signature all zero, ic_index:%u\n", __func__, di->ic_index);
		return;
	}

	if (cert->key_result == KEY_PASS)
		di->mem.ecce_pass = true;
	else
		return;

	memcpy(di->mem.act_sign, cert->signature, SLE95250_ACT_LEN);
	di->mem.act_sig_ready = true;
}

static void sle95250_batt_info_parse(struct sle95250_dev *di)
{
	uint8_t info0[sizeof(struct batt_info_para)] = { 0 };
	uint8_t batt_type[SLE95250_BATTTYP_LEN] = { 0 };
	struct batt_info_para *info = NULL;

	if (!di)
		return;

	info = batt_get_info_para(di->ic_index);
	if (!info) {
		hwlog_err("%s: batt info NULL\n", __func__);
		di->mem.source = BATTERY_UNKNOWN;
		return;
	}

	if (!memcmp(info0, (uint8_t *)info, sizeof(struct batt_info_para))) {
		hwlog_err("%s: batt info all zero, ic_index:%u\n", __func__, di->ic_index);
		di->mem.source = BATTERY_UNKNOWN;
		return;
	}

	di->mem.cet_rdy = CERT_READY;

	memcpy(batt_type, info->type, SLE95250_BATTTYP_LEN);
	di->mem.batt_type[0] = batt_type[1];
	di->mem.batt_type[1] = batt_type[0];
	di->mem.batttp_ready = true;

	di->mem.source = info->source;

	memcpy(di->mem.sn, info->sn, SLE95250_SN_ASC_LEN);
	if ((di->mem.sn[0] == ASCII_0) && (strlen(info->sn) == 1))  {
		hwlog_err("%s: no sn info, ic_index:%u\n", __func__, di->ic_index);
		di->mem.sn_ready = false;
	} else {
		di->mem.sn_ready = true;
	}
}

static void sle95250_group_sn_info_parse(struct sle95250_dev *di)
{
	uint8_t info0[sizeof(struct batt_group_sn_para)] = { 0 };
	struct batt_group_sn_para *info = NULL;
	int len;

	if (!di)
		return;

	info = batt_get_group_sn_para(di->ic_index);
	if (!info) {
		hwlog_err("%s: batt info NULL\n", __func__);
		return;
	}

	if (!memcmp(info0, (uint8_t *)info, sizeof(struct batt_group_sn_para))) {
		hwlog_err("%s: group sn info all zero, ic_index:%u\n", __func__, di->ic_index);
		return;
	}

	len = (SLE95250_SN_ASC_LEN < info->group_sn_len) ?
		SLE95250_SN_ASC_LEN : info->group_sn_len;
	memcpy(di->mem.group_sn, info->group_sn, len);
	di->mem.group_sn_ready = true;
}

static int sle95250_ic_ck(struct sle95250_dev *di)
{
	struct batt_ic_para *ic_para = NULL;

	if (!di)
		return -ENODEV;

	ic_para = batt_get_ic_para(di->ic_index);
	if (!ic_para) {
		hwlog_err("%s: ic_para NULL\n", __func__);
		return -EINVAL;
	}

	if (ic_para->ic_type != INFINEON_SLE95250_TYPE)
		return -EINVAL;
	di->mem.ic_type = (enum batt_ic_type)ic_para->ic_type;

	memcpy(di->mem.uid, ic_para->uid, SLE95250_UID_LEN);
	di->mem.uid_ready = true;

	return 0;
}

static int sle95250_ct_ops_register(struct platform_device *pdev,
	struct batt_ct_ops *bco)
{
	int ret;
	struct sle95250_dev *di = NULL;

	if (!bco || !pdev) {
		hwlog_err("%s: bco/pdev NULL\n", __func__);
		return -ENXIO;
	}
	di = platform_get_drvdata(pdev);
	if (!di)
		return -EINVAL;

	ret = sle95250_ic_ck(di);
	if (ret) {
		hwlog_err("%s: ic unmatch, ic_index:%u\n", __func__, di->ic_index);
		return -ENXIO;
	}

	hwlog_info("[%s]: ic matched, ic_index:%u\n", __func__, di->ic_index);

	sle95250_batt_info_parse(di);
	sle95250_cert_info_parse(di);
	sle95250_group_sn_info_parse(di);

	bco->get_ic_type = sle95250_get_ic_type;
	bco->get_ic_uuid = sle95250_get_uid;
	bco->get_batt_type = sle95250_get_batt_type;
	bco->get_batt_sn = sle95250_get_batt_sn;
	bco->certification = sle95250_certification;
	bco->set_act_signature = sle95250_set_act_signature;
	bco->prepare = sle95250_prepare;
	bco->set_batt_safe_info = sle95250_set_batt_safe_info;
	bco->get_batt_safe_info = sle95250_get_batt_safe_info;
	bco->power_down = NULL;
	return 0;
}

#ifdef BATTBD_FORCE_MATCH
static int sle95250_cyclk_iqr(struct sle95250_dev *di, uint8_t *lk_status)
{
	int ret;

	if (!di)
		return -ENODEV;

	sle95250_dev_on(di);
	ret = optiga_reg_life_span_lock_read(di, lk_status);
	sle95250_dev_off(di);
	if (ret) {
		hwlog_err("%s: lk_status iqr fail, ic_index:%u\n",
			__func__, di->ic_index);
		return -EAGAIN;
	}

	return 0;
}

static int sle95250_sysfs_ecce(struct platform_device *pdev)
{
	int ret;
	enum key_cr result;
	struct power_genl_attr res;
	uint8_t odc_read[SLE95250_ODC_LEN] = { 0 };

	res.data = odc_read;
	res.len = SLE95250_TE_PBK_LEN;

	ret = sle95250_certification(pdev, &res, &result);
	if (ret) {
		hwlog_err("%s: ecce fail; result : %d\n", __func__, result);
		return -1;
	}

	hwlog_info("[%s] ecce done\n", __func__);
	return 0;
}

static void sle95250_sysfs_ic_type(enum batt_ic_type *ic_type)
{
	*ic_type = sle95250_get_ic_type();
}

static int sle95250_sysfs_uid(struct platform_device *pdev, uint8_t *uid)
{
	int ret;
	uint32_t uuid_len;
	uint8_t uid_loc[SLE95250_UID_LEN];
	const uint8_t **uuid = (const uint8_t **)&uid_loc;

	ret = sle95250_get_uid(pdev, uuid, &uuid_len);
	if (ret) {
		hwlog_err("%s: uid iqr fail\n", __func__);
		return -EAGAIN;
	}
	memcpy(uid, *uuid, SLE95250_UID_LEN);

	return 0;
}

static int sle95250_sysfs_batt_type(struct platform_device *pdev,
	uint8_t *batt_type)
{
	int ret;
	uint32_t type_len;
	uint8_t *type = NULL;

	ret = sle95250_get_batt_type(pdev, (const unsigned char **)&type,
		&type_len);
	if (ret) {
		hwlog_err("%s: batt type iqr fail\n", __func__);
		return -EAGAIN;
	}
	memcpy(batt_type, type, SLE95250_BATTTYP_LEN);

	return 0;
}

static int sle95250_sysfs_get_sn(struct platform_device *pdev, uint8_t *batt_sn)
{
	int ret;
	struct power_genl_attr res;
	uint32_t sn_size;
	uint8_t sn_loc[SLE95250_SN_ASC_LEN];
	const uint8_t **sn = (const uint8_t **)&sn_loc;

	ret = sle95250_get_batt_sn(pdev, &res, sn, &sn_size);
	if (ret) {
		hwlog_err("%s: batt sn iqr fail\n", __func__);
		return -EAGAIN;
	}
	memcpy(batt_sn, *sn, SLE95250_SN_ASC_LEN);

	return 0;
}

#ifndef BATTBD_FORCE_MATCH
static int sle95250_group_sn_write(struct platform_device *pdev, uint8_t *group_sn)
{
	struct sle95250_dev *di = platform_get_drvdata(pdev);

	if (!di)
		return -ENODEV;
	(void)group_sn;
	hwlog_info("[%s] operation banned in user mode, ic_index:%u\n",
		__func__, di->ic_index);
	return 0;
}

static int sle95250_get_group_sn(struct platform_device *pdev, uint8_t *group_sn)
{
	struct sle95250_dev *di = platform_get_drvdata(pdev);

	if (!di)
		return -ENODEV;

	if (di->mem.group_sn_ready) {
		hwlog_info("[%s] group sn in mem already, ic_index:%u\n",
			__func__, di->ic_index);
		memcpy(group_sn, di->mem.group_sn, SLE95250_SN_ASC_LEN);
		return 0;
	}
	hwlog_info("[%s] operation banned in user mode, ic_index:%u\n",
		__func__, di->ic_index);
	return -EINVAL;
}
#else

static void sle95250_u64_to_byte_array(uint64_t data, uint8_t *arr)
{
	int i;

	for (i = 0; i < SLE95250_BYTE_COUNT_PER_U64; ++i)
		arr[i] = (data >> (i * SLE95250_BIT_COUNT_PER_BYTE));
}

/*
 * Note: arr length must be 8
 */
static void sle95250_byte_array_to_u64(uint64_t *data, uint8_t *arr)
{
	int i;

	*data = 0;
	for (i = 0; i < SLE95250_BYTE_COUNT_PER_U64; ++i)
		*data += (uint64_t)arr[i] << (i * SLE95250_BIT_COUNT_PER_BYTE);
}

/*
 * Note: arr length must not be smaller than 16
 */
static int sle95250_hex_array_to_u64(uint64_t *data, uint8_t *arr)
{
	uint64_t val;
	int i;

	*data = 0;
	for (i = 0; i < SLE95250_HEX_COUNT_PER_U64; ++i) {
		if ('0' <= arr[i] && arr[i] <= '9') { /* number */
			val = arr[i] - '0';
		} else if ('a' <= arr[i] && arr[i] <= 'f') { /* lowercase */
			val = arr[i] - 'a' + SLE95250_HEX_NUMBER_BASE;
		} else if ('A' <= arr[i] && arr[i] <= 'F') { /* uppercase */
			val = arr[i] - 'A' + SLE95250_HEX_NUMBER_BASE;
		} else {
			hwlog_err("[%s] failed int arr[%d]=%d\n",
				__func__, i, arr[i]);
			*data = 0;
			return -1;
		}
		*data += val << ((SLE95250_HEX_COUNT_PER_U64 - 1 - i) *
			SLE95250_BIT_COUNT_PER_HEX);
	}
	return 0;
}

static int sle95250_group_sn_write(struct platform_device *pdev,
	uint8_t *group_sn)
{
	int ret;
	int rty;
	uint8_t page_data_w[SLE95250_IC_GROUP_SN_LENGTH];
	uint8_t page_data_r[SLE95250_IC_GROUP_SN_LENGTH];
	uint64_t val = 0;
	struct sle95250_dev *di = platform_get_drvdata(pdev);
	int i;

	if (!di)
		return -ENODEV;

	ret = sle95250_hex_array_to_u64(&val, group_sn);
	if (ret) {
		hwlog_err("%s: hex to u64 fail\n", __func__);
		return -EINVAL;
	}

	hwlog_err("%s: val = %016llX\n", __func__, val);
	sle95250_u64_to_byte_array(val, page_data_w);
	for (i = 0; i < SLE95250_IC_GROUP_SN_LENGTH; ++i)
		hwlog_err("%s: page_data_w[%d] = %u\n", __func__, i, page_data_w[i]);

	ret = batt_cpu_single_core_set();
	if (ret) {
		hwlog_err("%s: single core set fail, ic_index:%u\n",
			__func__, di->ic_index);
		goto group_sn_write_fail;
	}

	sle95250_dev_on(di);
	for (rty = 0; rty < SLE95250_SN_RTY; rty++) {
		ret = optiga_nvm_write(di, SLE95250_RO_AREA,
			SLE95250_GROUP_SN_PG, SLE95250_GROUP_SN_OFFSET,
			SLE95250_IC_GROUP_SN_LENGTH, page_data_w);
		if (ret)
			continue;

		ret = optiga_nvm_read(di, SLE95250_RO_AREA, SLE95250_GROUP_SN_PG,
			SLE95250_GROUP_SN_OFFSET, SLE95250_IC_GROUP_SN_LENGTH,
			page_data_r);
		if (ret)
			continue;

		ret = memcmp(page_data_w, page_data_r,
			SLE95250_IC_GROUP_SN_LENGTH);
		if (ret)
			continue;
		break;
	}

	sle95250_dev_off(di);

	if (ret) {
		hwlog_err("%s: fail, ic_index:%u\n", __func__, di->ic_index);
		goto group_sn_write_fail;
	}

	hwlog_info("%s: succ, ic_index:%u\n", __func__, di->ic_index);
	batt_turn_on_all_cpu_cores();
	return 0;

group_sn_write_fail:
	batt_turn_on_all_cpu_cores();
	return -EINVAL;
}

static int sle95250_get_group_sn(struct platform_device *pdev, uint8_t *group_sn)
{
	int ret;
	int rty;
	struct sle95250_dev *di = platform_get_drvdata(pdev);
	uint8_t byte_group_sn[SLE95250_IC_GROUP_SN_LENGTH] = { 0 };
	uint64_t hash_group_sn = 0;

	if (!di)
		return -ENODEV;

	ret = batt_cpu_single_core_set();
	if (ret) {
		hwlog_err("%s: single core set fail, ic_index:%u\n",
			__func__, di->ic_index);
		goto get_group_sn_fail;
	}

	sle95250_dev_on(di);
	for (rty = 0; rty < SLE95250_SN_RTY; rty++) {
		ret = optiga_nvm_read(di, SLE95250_RO_AREA, SLE95250_GROUP_SN_PG,
			SLE95250_GROUP_SN_OFFSET, SLE95250_IC_GROUP_SN_LENGTH,
			byte_group_sn);
		if (ret)
			continue;
	}

	sle95250_dev_off(di);
	if (ret) {
		hwlog_err("%s: group sn read fail\n", __func__);
		goto get_group_sn_fail;
	}

	sle95250_byte_array_to_u64(&hash_group_sn, byte_group_sn);
	ret = snprintf(group_sn, SLE95250_SN_ASC_LEN + 1,
		"%016llX", hash_group_sn);
	if (ret < 0) {
		hwlog_err("%s: snprintf fail\n", __func__);
		goto get_group_sn_fail;
	}

	batt_turn_on_all_cpu_cores();
	return 0;
get_group_sn_fail:
	batt_turn_on_all_cpu_cores();
	return -EINVAL;
}
#endif /* BATTBD_FORCE_MATCH */

static int sle95250_sysfs_write_group_sn(struct platform_device *pdev,
	uint8_t *group_sn, int sn_len)
{
	int ret;

	if (!group_sn || (sn_len != SLE95250_IC_GROUP_SN_LENGTH * 2)) {
		hwlog_err("[%s] group_sn NULL or sn_len err\n", __func__);
		return -EINVAL;
	}

	ret = sle95250_group_sn_write(pdev, group_sn);
	if (ret) {
		hwlog_err("%s: group sn write fail\n", __func__);
		return -EAGAIN;
	}

	return 0;
}

static int sle95250_sysfs_get_group_sn(struct platform_device *pdev,
	uint8_t *group_sn, int sn_len)
{
	int ret;

	if (!group_sn || (sn_len != SLE95250_SN_ASC_LEN)) {
		hwlog_err("[%s] group_sn NULL or sn_len err\n", __func__);
		return -EINVAL;
	}

	ret = sle95250_get_group_sn(pdev, group_sn);
	if (ret) {
		hwlog_err("%s: group sn iqr fail\n", __func__);
		return -EAGAIN;
	}

	return 0;
}

static int sle95250_sysfs_set_act(struct platform_device *pdev,
	uint8_t *act_data)
{
	int ret;
	struct power_genl_attr res;
	enum res_type type = RES_ACT;

	if (!act_data) {
		hwlog_err("[%s] act_data NULL\n", __func__);
		return -EINVAL;
	}

	res.data = act_data;
	res.len = SLE95250_ACT_LEN;

	ret = sle95250_set_act_signature(pdev, type, &res);
	if (ret) {
		hwlog_err("%s: act set fail\n", __func__);
		return -EAGAIN;
	}

	return 0;
}

static int sle95250_sysfs_get_act(struct platform_device *pdev, uint8_t *act)
{
	int ret;
	uint8_t loop_ind;
	struct power_genl_attr res;
	enum res_type type = RES_ACT;

	ret = sle95250_prepare(pdev, type, &res);
	if (ret) {
		hwlog_err("%s: act get fail\n", __func__);
		return -EAGAIN;
	}

	for (loop_ind = 1; loop_ind < SLE95250_ACT_LEN; loop_ind++) {
		if (res.data[loop_ind] - res.data[loop_ind - 1] !=
			TEST_ACT_GAP) {
			hwlog_err("%s: act iqr wrong\n", __func__);
			return -EAGAIN;
		}
	}

	*act = res.data[0];
	return 0;
}

static int sle95250_sysfs_get_batt_cyc(struct platform_device *pdev,
	int *batt_cyc)
{
	int ret;
	enum batt_safe_info_t type = BATT_CHARGE_CYCLES;

	ret = sle95250_get_batt_safe_info(pdev, type, batt_cyc);
	if (ret) {
		hwlog_err("%s: batt_cyc get fail\n", __func__);
		return -EAGAIN;
	}

	return 0;
}

static int sle95250_sysfs_batt_cyc_drc(struct platform_device *pdev,
	int cyc_dcr)
{
	int ret;
	enum batt_safe_info_t type = BATT_CHARGE_CYCLES;

	ret = sle95250_set_batt_safe_info(pdev, type, &cyc_dcr);
	if (ret) {
		hwlog_err("%s: batt_cyc dcr %d fail\n", __func__, cyc_dcr);
		return -EAGAIN;
	}

	return 0;
}

static int sle95250_sysfs_spar_ck(struct platform_device *pdev,
	enum batt_source *batt_spar)
{
	int ret;
	enum batt_safe_info_t type = BATT_SPARE_PART;

	ret = sle95250_get_batt_safe_info(pdev, type, batt_spar);
	if (ret) {
		hwlog_err("%s: battn spar check %d fail\n", __func__, ret);
		return -EAGAIN;
	}
	return 0;
}

static int sle95250_sysfs_org_set(struct platform_device *pdev)
{
	int ret;
	int value = ORG_TYPE;
	enum batt_safe_info_t type = BATT_SPARE_PART;

	ret = sle95250_set_batt_safe_info(pdev, type, &value);
	if (ret) {
		hwlog_err("%s: org set %d fail\n", __func__, ret);
		return -EAGAIN;
	}

	return 0;
}

#define sle95250_sysfs_fld(_name, n, m, show, store) \
{ \
	.attr = __ATTR(_name, m, show, store), \
	.name = SLE95250_SYSFS_##n, \
}

#define sle95250_sysfs_fld_ro(_name, n) \
	sle95250_sysfs_fld(_name, n, 0444, sle95250_sysfs_show, NULL)

#define sle95250_sysfs_fld_rw(_name, n) \
	sle95250_sysfs_fld(_name, n, 0644, sle95250_sysfs_show, \
		sle95250_sysfs_store)

#define sle95250_sysfs_fld_wo(_name, n) \
	sle95250_sysfs_fld(_name, n, 0200, NULL, sle95250_sysfs_store)

struct sle95250_sysfs_fld_info {
	struct device_attribute attr;
	uint8_t name;
};

static ssize_t sle95250_sysfs_show(struct device *dev,
	struct device_attribute *attr, char *buf);

static ssize_t sle95250_sysfs_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count);

static struct sle95250_sysfs_fld_info sle95250_sysfs_fld_tbl[] = {
	sle95250_sysfs_fld_ro(ecce, ECCE),
	sle95250_sysfs_fld_ro(ic_type, IC_TYPE),
	sle95250_sysfs_fld_ro(uid, UID),
	sle95250_sysfs_fld_ro(batttype, BATT_TYPE),
	sle95250_sysfs_fld_ro(sn, BATT_SN),
	sle95250_sysfs_fld_rw(actsig, ACT_SIG),
	sle95250_sysfs_fld_rw(battcyc, BATT_CYC),
	sle95250_sysfs_fld_rw(org_spar, SPAR_CK),
	sle95250_sysfs_fld_rw(cyclk, CYCLK),
	sle95250_sysfs_fld_ro(pglk, PGLK),
	sle95250_sysfs_fld_rw(group_sn, GROUP_SN),
};

#define SLE95250_SYSFS_ATTRS_SIZE (ARRAY_SIZE(sle95250_sysfs_fld_tbl) + 1)

static struct attribute *sle95250_sysfs_attrs[SLE95250_SYSFS_ATTRS_SIZE];

static const struct attribute_group sle95250_sysfs_attr_group = {
	.attrs = sle95250_sysfs_attrs,
};

static struct sle95250_sysfs_fld_info *sle95250_sysfs_fld_lookup(
	const char *name)
{
	int s;
	int e = ARRAY_SIZE(sle95250_sysfs_fld_tbl);

	for (s = 0; s < e; s++) {
		if (!strncmp(name, sle95250_sysfs_fld_tbl[s].attr.attr.name,
			strlen(name)))
			break;
	}

	if (s >= e)
		return NULL;

	return &sle95250_sysfs_fld_tbl[s];
}

static ssize_t sle95250_sysfs_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int ret;
	int str_len = 0;
	int batt_cyc;
	uint8_t act;
	uint8_t pglk = 0;
	uint8_t cyc_lk;
	uint8_t uid[SLE95250_UID_LEN];
	uint8_t sn[SLE95250_SN_ASC_LEN];
	uint8_t batttype[SLE95250_BATTTYP_LEN];
	enum batt_ic_type ic_type;
	enum batt_source batt_spar;
	struct sle95250_dev *di = NULL;
	struct sle95250_sysfs_fld_info *info = NULL;
	struct platform_device *pdev = NULL;
	uint8_t group_sn[SLE95250_SN_ASC_LEN + 1] = { 0 };

	info = sle95250_sysfs_fld_lookup(attr->attr.name);
	dev_get_drv_data(di, dev);
	pdev = container_of(dev, struct platform_device, dev);
	if (!info || !di || !pdev)
		return -EINVAL;
	di->sysfs_mode = true;
	switch (info->name) {
	case SLE95250_SYSFS_ECCE:
		hwlog_info("[%s] SLE95250_SYSFS_ECCE, ic_index:%u\n",
			__func__, di->ic_index);
		ret = sle95250_sysfs_ecce(pdev);
		if (ret) {
			hwlog_err("%s: ecce fail, ic_index:%u\n",
				__func__, di->ic_index);
			str_len = scnprintf(buf, PAGE_SIZE, "ecce fail\n");
			break;
		}
		str_len = scnprintf(buf, PAGE_SIZE, "ecce succ\n");
		break;
	case SLE95250_SYSFS_IC_TYPE:
		hwlog_info("[%s] SLE95250_SYSFS_IC_TYPE, ic_index:%u\n",
			__func__, di->ic_index);
		sle95250_sysfs_ic_type(&ic_type);
		str_len = scnprintf(buf, PAGE_SIZE, "ic type : %d\n", ic_type);
		break;
	case SLE95250_SYSFS_UID:
		hwlog_info("[%s] SLE95250_SYSFS_UID, ic_index:%u\n",
			__func__, di->ic_index);
		ret = sle95250_sysfs_uid(pdev, uid);
		if (ret) {
			hwlog_err("%s: uid read fail, ic_index:%u\n",
				__func__, di->ic_index);
			str_len = scnprintf(buf, PAGE_SIZE, "uid rd fail\n");
			break;
		}

		str_len = scnprintf(buf, PAGE_SIZE,
			"uid [h2l]:0x%x %x %x %x-0x%x %x %x %x-0x%x %x %x %x\n",
			uid[11], uid[10], uid[9], uid[8], uid[7], uid[6],
			uid[5], uid[4], uid[3], uid[2], uid[1], uid[0]);
		break;
	case SLE95250_SYSFS_BATT_TYPE:
		hwlog_info("[%s] SLE95250_SYSFS_BATT_TYPE, ic_index:%u\n",
			__func__, di->ic_index);
		ret = sle95250_sysfs_batt_type(pdev, batttype);
		if (ret) {
			hwlog_err("%s: batt_type read fail, ic_index:%u\n",
				__func__, di->ic_index);
			str_len = scnprintf(buf, PAGE_SIZE, "batttp rd fail\n");
			break;
		}
		str_len = scnprintf(buf, PAGE_SIZE, "Btp0: %d; Btp1: %d\n",
			batttype[SLE95250_PKVED_IND],
			batttype[SLE95250_CELVED_IND]);
		break;
	case SLE95250_SYSFS_BATT_SN:
		hwlog_info("[%s] SLE95250_SYSFS_BATT_SN, ic_index:%u\n",
			__func__, di->ic_index);
		ret = sle95250_sysfs_get_sn(pdev, sn);
		if (ret) {
			hwlog_err("%s: batt_sn read fail, ic_index:%u\n",
				__func__, di->ic_index);
			str_len = scnprintf(buf, PAGE_SIZE, "battsn rd fail\n");
			break;
		}

		hwlog_info("SN[ltoh]:%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c, ic_index:%u\n",
			sn[0], sn[1], sn[2], sn[3], sn[4], sn[5], sn[6], sn[7], sn[8],
			sn[9], sn[10], sn[11], sn[12], sn[13], sn[14], sn[15], di->ic_index);
		str_len = scnprintf(buf, PAGE_SIZE,
			"SN [ltoh]: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", sn[0],
			sn[1], sn[2], sn[3], sn[4], sn[5], sn[6], sn[7], sn[8],
			sn[9], sn[10], sn[11], sn[12], sn[13], sn[14], sn[15]);
		break;
	case SLE95250_SYSFS_ACT_SIG:
		hwlog_info("[%s] SLE95250_SYSFS_ACT_SIG, ic_index:%u\n",
			__func__, di->ic_index);
		ret = sle95250_sysfs_get_act(pdev, &act);
		if (ret) {
			hwlog_err("%s: batt_act read fail, ic_index:%u\n",
				__func__, di->ic_index);
			str_len = scnprintf(buf, PAGE_SIZE, "batact rd fail\n");
			break;
		}
		str_len = scnprintf(buf, PAGE_SIZE, "Act = %u\n", act);
		break;
	case SLE95250_SYSFS_BATT_CYC:
		hwlog_info("[%s] SLE95250_SYSFS_BATT_CYC, ic_index:%u\n",
			__func__, di->ic_index);
		ret = sle95250_sysfs_get_batt_cyc(pdev, &batt_cyc);
		if (ret) {
			hwlog_err("%s: batt_cyc read fail, ic_index:%u\n",
				__func__, di->ic_index);
			str_len = scnprintf(buf, PAGE_SIZE, "batcyc rd fail\n");
			break;
		}
		str_len = scnprintf(buf, PAGE_SIZE, "battcyc = %d\n", batt_cyc);
		break;
	case SLE95250_SYSFS_SPAR_CK:
		hwlog_info("[%s] SLE95250_SYSFS_SPAR_CK, ic_index:%u\n",
			__func__, di->ic_index);
		ret = sle95250_sysfs_spar_ck(pdev, &batt_spar);
		if (ret) {
			hwlog_err("%s: batt spar check read fail, ic_index:%u\n",
				__func__, di->ic_index);
			str_len = scnprintf(buf, PAGE_SIZE, "spar ck fail\n");
			break;
		}
		str_len = scnprintf(buf, PAGE_SIZE, "batspar=%d\n", batt_spar);
		break;
	case SLE95250_SYSFS_CYCLK:
		hwlog_info("[%s] SLE95250_SYSFS_CYCLK, ic_index:%u\n",
			__func__, di->ic_index);
		ret = sle95250_cyclk_iqr(di, &cyc_lk);
		if (ret) {
			hwlog_err("%s: cyc_lk iqr fail, ic_index:%u\n",
				__func__, di->ic_index);
			str_len = scnprintf(buf, PAGE_SIZE, "cyclk iqr fail\n");
			break;
		}
		str_len = scnprintf(buf, PAGE_SIZE, "cyc_lk = 0x%x\n", cyc_lk);
		break;
	case SLE95250_SYSFS_PGLK:
		hwlog_info("[%s] SLE95250_SYSFS_PGLK, ic_index:%u\n",
			__func__, di->ic_index);
		ret = sle95250_pglk_iqr(di, &pglk);
		hwlog_info("[%s] pglk = 0x%x, ic_index:%u\n",
			__func__, pglk, di->ic_index);
		if (ret) {
			hwlog_err("%s: pglk iqr fail, ic_index:%u\n",
				__func__, di->ic_index);
			str_len = scnprintf(buf, PAGE_SIZE, "pglk iqr fail\n");
			break;
		}
		str_len = scnprintf(buf, PAGE_SIZE, "pglk: 0x%x\n", pglk);
		break;
	case SLE95250_SYSFS_GROUP_SN:
		hwlog_info("[%s] SLE95250_SYSFS_GROUP_SN, ic_index:%u\n",
			__func__, di->ic_index);
		ret = sle95250_sysfs_get_group_sn(pdev, &group_sn[0],
			SLE95250_SN_ASC_LEN);
		if (ret) {
			hwlog_err("%s: group sn read fail, ic_index:%u\n",
				__func__, di->ic_index);
			str_len = scnprintf(buf, PAGE_SIZE, "group sn rd fail\n");
			break;
		}
		str_len = scnprintf(buf, PAGE_SIZE, "Group SN: %s\n", group_sn);
		break;
	default:
		break;
	}

	di->sysfs_mode = false;
	return str_len;
}

static ssize_t sle95250_sysfs_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct sle95250_sysfs_fld_info *info = NULL;
	struct sle95250_dev *di = NULL;
	long val = 0;
	int ret;
	int lp_ind;
	uint8_t act_data[SLE95250_ACT_LEN];
	struct platform_device *pdev = NULL;
	/* 2 : group_sn is hex array, every char need two uint8_t spaces */
	uint8_t group_sn[SLE95250_IC_GROUP_SN_LENGTH * 2] = { 0 };
	int group_sn_len;

	info = sle95250_sysfs_fld_lookup(attr->attr.name);
	dev_get_drv_data(di, dev);
	pdev = container_of(dev, struct platform_device, dev);
	if (!info || !di || !pdev)
		return -EINVAL;

	di->sysfs_mode = true;
	switch (info->name) {
	case SLE95250_SYSFS_ACT_SIG:
		hwlog_info("[%s] SLE95250_SYSFS_ACT_SIG, ic_index:%u\n",
			__func__, di->ic_index);
		if (kstrtol(buf, 10, &val) < 0 ||
			val < 0 || val > ACT_MAX_LEN) {
			hwlog_err("%s: val is not valid!, ic_index:%u\n",
				__func__, di->ic_index);
			break;
		}
		for (lp_ind = 0; lp_ind < SLE95250_ACT_LEN; lp_ind++) {
			if (lp_ind == 0)
				act_data[lp_ind] = val;
			else
				act_data[lp_ind] = act_data[lp_ind - 1] + 1;
		}
		ret = sle95250_sysfs_set_act(pdev, act_data);
		if (ret) {
			hwlog_err("%s: act write fail, ic_index:%u\n",
				__func__, di->ic_index);
			break;
		}
		break;
	case SLE95250_SYSFS_BATT_CYC:
		hwlog_info("[%s] SLE95250_SYSFS_BATT_CYC, ic_index:%u\n",
			__func__, di->ic_index);
		if (kstrtol(buf, POWER_BASE_DEC, &val) < 0 ||
			val < 0 || val > CYC_MAX) {
			hwlog_err("%s: val is not valid!, ic_index:%u\n",
				__func__, di->ic_index);
			break;
		}
		ret = sle95250_sysfs_batt_cyc_drc(pdev, val);
		if (ret) {
			hwlog_err("%s: batt_cyc set fail, ic_index:%u\n",
				__func__, di->ic_index);
			break;
		}
		break;
	case SLE95250_SYSFS_SPAR_CK:
		hwlog_info("[%s] SLE95250_SYSFS_SPAR_CK, ic_index:%u\n",
			__func__, di->ic_index);
		if (kstrtol(buf, POWER_BASE_DEC, &val) < 0 || val != 1) {
			hwlog_err("%s: val is not valid!, ic_index:%u\n",
				__func__, di->ic_index);
			break;
		}
		ret = sle95250_sysfs_org_set(pdev);
		if (ret) {
			hwlog_err("%s: batt set as org fail, ic_index:%u\n",
				__func__, di->ic_index);
			break;
		}
		break;
	case SLE95250_SYSFS_CYCLK:
		hwlog_info("[%s] SLE95250_SYSFS_SPAR_CK, ic_index:%u\n",
			__func__, di->ic_index);
		if (kstrtol(buf, POWER_BASE_DEC, &val) < 0 || val != 1) {
			hwlog_err("%s: val is not valid!, ic_index:%u\n",
				__func__, di->ic_index);
			break;
		}
		ret = sle95250_cyclk_set(di);
		if (ret) {
			hwlog_err("%s: cyc lk fail, ic_index:%u\n",
				__func__, di->ic_index);
			break;
		}
		break;
	case SLE95250_SYSFS_GROUP_SN:
		hwlog_info("[%s] SLE95250_SYSFS_GROUP_SN, ic_index:%u\n",
			__func__, di->ic_index);
		group_sn_len = (count < SLE95250_IC_GROUP_SN_LENGTH * 2) ?
			count : SLE95250_IC_GROUP_SN_LENGTH * 2;
		memcpy(group_sn, buf, group_sn_len);
		ret = sle95250_sysfs_write_group_sn(pdev, &group_sn[0],
			SLE95250_IC_GROUP_SN_LENGTH * 2);
		if (ret) {
			hwlog_err("%s: group sn write fail, ic_index:%u\n",
				__func__, di->ic_index);
			break;
		}
		break;
	default:
		break;
	}

	di->sysfs_mode = false;
	return count;
}

static void sle95250_sysfs_init_attrs(void)
{
	int s;
	int e = ARRAY_SIZE(sle95250_sysfs_fld_tbl);

	for (s = 0; s < e; s++)
		sle95250_sysfs_attrs[s] = &sle95250_sysfs_fld_tbl[s].attr.attr;

	sle95250_sysfs_attrs[e] = NULL;
}

static int sle95250_sysfs_create(struct sle95250_dev *di)
{
	sle95250_sysfs_init_attrs();
	return sysfs_create_group(&di->dev->kobj, &sle95250_sysfs_attr_group);
}
#endif /* BATTBD_FORCE_MATCH */

static int sle95250_parse_dts(struct sle95250_dev *di, struct device_node *np)
{
	int ret;

	ret = of_property_read_u32(np, "ic_index", &di->ic_index);
	if (ret) {
		hwlog_err("%s: ic_index not given in dts\n", __func__);
		di->ic_index = SLE95250_DFT_IC_INDEX;
	}
	hwlog_info("[%s] ic_index = 0x%x\n", __func__, di->ic_index);

	ret = of_property_read_u8(np, "tau", &di->tau);
	if (ret) {
		hwlog_err("%s: tau not given in dts, ic_index:%u\n",
			__func__, di->ic_index);
		di->tau = SLE95250_DEFAULT_TAU;
	}
	hwlog_info("[%s] tau = %u, ic_index:%u\n", __func__,
		di->tau, di->ic_index);

	ret = of_property_read_u16(np, "cyc_full", &di->cyc_full);
	if (ret) {
		hwlog_err("%s: cyc_full not given in dts, ic_index:%u\n",
			__func__, di->ic_index);
		di->cyc_full = SLE95250_DFT_FULL_CYC;
	}
	hwlog_info("[%s] cyc_full = 0x%x, ic_index:%u\n", __func__,
		di->cyc_full, di->ic_index);

	return 0;
}

static int sle95250_gpio_init(struct platform_device *pdev,
	struct sle95250_dev *di)
{
	int ret;

	di->onewire_gpio = of_get_named_gpio(pdev->dev.of_node,
		"onewire-gpio", 0);

	if (!gpio_is_valid(di->onewire_gpio)) {
		hwlog_err("onewire_gpio is not valid, ic_index:%u\n", di->ic_index);
		return -EINVAL;
	}
	/* get the gpio */
	ret = devm_gpio_request(&pdev->dev, di->onewire_gpio, "onewire_sle");
	if (ret) {
		hwlog_err("gpio request failed %d, ic_index:%u\n",
			di->onewire_gpio, di->ic_index);
		return ret;
	}
	gpio_direction_input(di->onewire_gpio);

	return 0;
}

static void sle95250_mem_free(struct sle95250_dev *di)
{
	if (!di)
		return;

	kfree(di);
	optiga_swi_unloading();
}

static int sle95250_remove(struct platform_device *pdev)
{
	struct sle95250_dev *di = NULL;

	hwlog_info("[%s] remove begin\n", __func__);
	di = platform_get_drvdata(pdev);
	if (!di)
		return -ENODEV;

	sle95250_mem_free(di);
	hwlog_info("[%s] remove done\n", __func__);

	return 0;
}

static int sle95250_probe(struct platform_device *pdev)
{
	int ret;
	struct sle95250_dev *di = NULL;
	struct device_node *np = NULL;

	hwlog_info("[%s] probe begin\n", __func__);

	if (!pdev || !pdev->dev.of_node)
		return -ENODEV;

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di) {
		return -ENOMEM;
	}

	di->dev = &pdev->dev;
	np = pdev->dev.of_node;

	ret = sle95250_parse_dts(di, np);
	if (ret) {
		hwlog_err("%s: dts parse fail, ic_index:%u\n",
			__func__, di->ic_index);
		goto dts_parse_fail;
	}

	ret = sle95250_gpio_init(pdev, di);
	if (ret) {
		hwlog_err("%s: gpio init fail, ic_index:%u\n",
			__func__, di->ic_index);
		goto gpio_init_fail;
	}

	ret = optiga_swi_init(di);
	if (ret) {
		hwlog_err("%s: swi init fail, ic_index:%u\n",
			__func__, di->ic_index);
		goto swi_init_fail;
	}

	sle95250_reg_sec_ic_ops();

	di->reg_node.ic_memory_release = NULL;
	di->reg_node.ic_dev = pdev;
	di->reg_node.ct_ops_register = sle95250_ct_ops_register;
	add_to_aut_ic_list(&di->reg_node);

#ifdef BATTBD_FORCE_MATCH
	ret = sle95250_sysfs_create(di);
	if (ret)
		hwlog_err("%s: sysfs create fail, ic_index:%u\n",
			__func__, di->ic_index);
#endif /* BATTBD_FORCE_MATCH */

	platform_set_drvdata(pdev, di);

	return 0;
swi_init_fail:
gpio_init_fail:
dts_parse_fail:
	sle95250_mem_free(di);
	return ret;
}

static const struct of_device_id sle95250_match_table[] = {
	{
		.compatible = "infineon,sle95250",
		.data = NULL,
	},
	{
		.compatible = "infineon,sle95250_1",
		.data = NULL,
	},
	{},
};

static struct platform_driver sle95250_driver = {
	.probe = sle95250_probe,
	.remove = sle95250_remove,
	.driver = {
		.name = "infineon,sle95250",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(sle95250_match_table),
	},
};

static int __init sle95250_init(void)
{
	return platform_driver_register(&sle95250_driver);
}

static void __exit sle95250_exit(void)
{
	platform_driver_unregister(&sle95250_driver);
}

subsys_initcall_sync(sle95250_init);
module_exit(sle95250_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("sle95250 ic");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
