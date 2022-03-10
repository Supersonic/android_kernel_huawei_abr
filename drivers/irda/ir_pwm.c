/*
 * ir_pwm.c
 *
 * infrared use pwm driver
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pwm.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/spinlock.h>
#include <linux/timex.h>
#include <linux/kthread.h>
#include <linux/sched/rt.h>
#include <uapi/linux/sched/types.h>
#include <linux/completion.h>
#include <linux/pinctrl/pinctrl-state.h>
#include <linux/pinctrl/consumer.h>
#include <linux/workqueue.h>
#include <huawei_platform/log/hw_log.h>
#include "ir_core.h"

#ifdef HWLOG_TAG
#undef HWLOG_TAG
#endif
#define HWLOG_TAG ir_core
HWLOG_REGIST();

enum {
	USE_PWM_SUBSYS = 0,
	USE_CLK_IMITATE,
};

struct ir_pwm {
	struct ir_device *ir;
	struct pwm_device *pwm;
	struct clk *clk;
	struct pinctrl *pctl;
	const char *clk_name;
	struct task_struct *task;
	wait_queue_head_t  tx_wait;
	struct completion tx_done;
	raw_spinlock_t tx_spin_lock;
	bool tx_start;
	bool tx_lock;
	unsigned long lock_flags;
	unsigned int pwm_mode;
	unsigned int delay_mode;
	const unsigned int *tx_buf;
	unsigned int tx_size;
	unsigned int tx_cnt;
	bool pwm_enable;
#ifdef IR_CORE_DEBUG
	ktime_t trace[IR_CIR_SIZE];
	unsigned long abr_sleep[IR_CIR_SIZE];
#endif /* IR_CORE_DEBUG */
};

#define ns2cycles(X) (((((X) / 1000 * 0x10C7UL + \
	(X) % 1000 * 0x5UL) * loops_per_jiffy * HZ) + \
	(1UL << 31)) >> 32)

#define IR_PWM_SLEEP_THR       1500
#define IR_PWM_SLEEP_MIN       50
#define IR_PWM_SLEEP_MAX       55
#define IR_PWM_CLK_ENABLE_US   60
#define IR_PWM_CLK_DISABLE_US  70

static void ir_pwm_config_pinctrl(struct ir_pwm *ir_pwm, const char *name)
{
	int ret;
	struct pinctrl_state *state = NULL;

	if (!ir_pwm->pctl)
		return;

	state = pinctrl_lookup_state(ir_pwm->pctl, name);
	if (IS_ERR(state)) {
		hwlog_err("%s: set pinctrl state to %s fail ret=%d\n", __func__,
			name, PTR_ERR(state));
		return;
	}

	ret = pinctrl_select_state(ir_pwm->pctl, state);
	if (ret)
		hwlog_err("%s: set pinctrl state to %s fail ret=%d\n", __func__,
			name, ret);

}

static void ir_pwm_config_subsys(struct ir_pwm *ir_pwm)
{
	int period;
	int duty;
	int ret;
	struct ir_device *ir = ir_pwm->ir;

	period = DIV_ROUND_CLOSEST(NSEC_PER_SEC, ir->carrier_freq);
	/* 100: percentage */
	duty = DIV_ROUND_CLOSEST(ir->duty_cycle * period, 100);
	ret = pwm_config(ir_pwm->pwm, duty, period);
	if (ret)
		hwlog_err("%s: set pwm subsys config to %u %u fail\n", __func__,
			ir->carrier_freq, ir->duty_cycle);
}

static void ir_pwm_config_clk(struct ir_pwm *ir_pwm)
{
	int ret;
	struct ir_device *ir = ir_pwm->ir;

	ret = clk_set_rate(ir_pwm->clk, ir->carrier_freq);
	if (ret)
		hwlog_err("%s: set clk rate to %u fail\n", __func__,
			ir->carrier_freq);

	/* 100: percentage */
	ret = clk_set_duty_cycle(ir_pwm->clk, ir->duty_cycle, 100);
	if (ret)
		hwlog_err("%s: set duty cycle to %u fail\n", __func__,
			ir->duty_cycle);
}

static void ir_pwm_config(struct ir_pwm *ir_pwm)
{
	switch (ir_pwm->pwm_mode) {
	case USE_PWM_SUBSYS:
		ir_pwm_config_subsys(ir_pwm);
		break;
	case USE_CLK_IMITATE:
		ir_pwm_config_clk(ir_pwm);
		clk_prepare(ir_pwm->clk);
		break;
	default:
		break;
	}
}

static void ir_pwm_enable(struct ir_pwm *ir_pwm)
{
	int ret;

	if (ir_pwm->pwm_enable)
		return;

	switch (ir_pwm->pwm_mode) {
	case USE_PWM_SUBSYS:
		pwm_enable(ir_pwm->pwm);
		break;
	case USE_CLK_IMITATE:
		ret = clk_enable(ir_pwm->clk);
		if (ret)
			hwlog_err("%s: clk_prepare_enable fail ret=%d\n",
				__func__, ret);
		break;
	default:
		return;
	}

	ir_pwm->pwm_enable = true;
}

static void ir_pwm_disable(struct ir_pwm *ir_pwm)
{
	if (!ir_pwm->pwm_enable)
		return;

	switch (ir_pwm->pwm_mode) {
	case USE_PWM_SUBSYS:
		pwm_disable(ir_pwm->pwm);
		break;
	case USE_CLK_IMITATE:
		clk_disable(ir_pwm->clk);
		break;
	default:
		return;
	}

	ir_pwm->pwm_enable = false;
}

static void ir_pwm_tx_lock(struct ir_pwm *ir_pwm)
{
	if (ir_pwm->tx_lock)
		return;

	ir_pwm->tx_lock = true;
	raw_spin_lock_irqsave(&ir_pwm->tx_spin_lock, ir_pwm->lock_flags);
}

static void ir_pwm_tx_unlock(struct ir_pwm *ir_pwm)
{
	if (!ir_pwm->tx_lock)
		return;

	raw_spin_unlock_irqrestore(&ir_pwm->tx_spin_lock, ir_pwm->lock_flags);
	ir_pwm->tx_lock = false;
}

static void ir_pwm_delay_uninterruptible(unsigned long us)
{
	cycles_t start;
	unsigned long cycles = ns2cycles(us * NSEC_PER_USEC);

	cycles = (cycles > 1) ? (cycles - 1) : cycles;
	start = get_cycles();
	while ((get_cycles() - start) < cycles)
		;
}

static void ir_pwm_delay_interruptible(struct ir_pwm *ir_pwm,
	unsigned long us)
{
	ktime_t kstart;
	long delta;
	unsigned long tus = us;

	if (tus <= IR_PWM_SLEEP_THR) {
		ir_pwm_delay_uninterruptible(tus);
		return;
	}

	kstart = ktime_get();
	ir_pwm_tx_unlock(ir_pwm);
	while (tus > IR_PWM_SLEEP_THR) {
		usleep_range(IR_PWM_SLEEP_MIN, IR_PWM_SLEEP_MAX);
		delta = ktime_us_delta(ktime_get(), kstart);
		if (us <= delta) {
#ifdef IR_CORE_DEBUG
			ir_pwm->abr_sleep[ir_pwm->tx_cnt - 1] = delta;
#endif /* IR_CORE_DEBUG */
			ir_pwm_tx_lock(ir_pwm);
			return;
		}
		tus = us - delta;
	}

	ir_pwm_tx_lock(ir_pwm);
	delta = ktime_us_delta(ktime_get(), kstart);
	if (us > delta)
		ir_pwm_delay_uninterruptible(us - delta);
}

static void ir_pwm_delay(struct ir_pwm *ir_pwm, unsigned long us)
{
	if (ir_pwm->delay_mode == 0)
		ir_pwm_delay_interruptible(ir_pwm, us);
	else if (ir_pwm->delay_mode)
		ir_pwm_delay_uninterruptible(us);
}

static inline void ir_pwm_tx_next(struct ir_pwm *ir_pwm)
{
#ifdef IR_CORE_DEBUG
	if (ir_pwm->ir->trace_mode)
		ir_pwm->trace[ir_pwm->tx_cnt] = ktime_get();
#endif /* IR_CORE_DEBUG */

	ir_pwm->tx_cnt++;
}

static int ir_pwm_tx_thread(void *param)
{
	unsigned int delta;
	struct ir_pwm *ir_pwm = param;
	struct sched_param sch_param = { .sched_priority = MAX_RT_PRIO - 1 };

	sched_setscheduler(current, SCHED_FIFO, &sch_param);

	while (true) {
		wait_event_interruptible(ir_pwm->tx_wait, ir_pwm->tx_start);
		if (kthread_should_stop())
			break;

		ir_pwm_tx_lock(ir_pwm);
		while (ir_pwm->tx_cnt < ir_pwm->tx_size) {
			if (ir_pwm->tx_cnt % 2) { /* space */
				ir_pwm_disable(ir_pwm);
				delta = IR_PWM_CLK_ENABLE_US;
			} else {
				ir_pwm_enable(ir_pwm);
				delta = IR_PWM_CLK_DISABLE_US;
			}
			if ((ir_pwm->tx_cnt + 1) == ir_pwm->tx_size)
				delta = 0;

			ir_pwm_tx_next(ir_pwm);
			if (ir_pwm->tx_buf[ir_pwm->tx_cnt - 1] > delta)
				ir_pwm_delay(ir_pwm, ir_pwm->tx_buf[ir_pwm->tx_cnt - 1] - delta);
			if (!ir_pwm->tx_start)
				break;
		}
		ir_pwm_tx_next(ir_pwm);
		ir_pwm_tx_unlock(ir_pwm);
		ir_pwm->tx_start = false;
		complete(&ir_pwm->tx_done);
	}

	return 0;
}

#ifdef IR_CORE_DEBUG
static void ir_pwm_trace_print(struct ir_pwm *ir_pwm)
{
	int i;
	char str[IR_TRACE_SIZE] = { 0 };

	if (!ir_pwm->ir->trace_mode)
		return;

	memset(str, 0, IR_TRACE_SIZE);
	for (i = 1; i < ir_pwm->tx_cnt; i++)
		snprintf(str, IR_TRACE_SIZE - strlen(str), "%s %ld ", str,
			ktime_us_delta(ir_pwm->trace[i], ir_pwm->trace[i - 1]));

	pr_err("%s_use: %s\n", __func__, str);

	memset(str, 0, IR_TRACE_SIZE);
	for (i = 0; i < ir_pwm->tx_size; i++)
		snprintf(str, IR_TRACE_SIZE - strlen(str), "%s %ld ", str,
			ir_pwm->abr_sleep[i]);

	pr_err("%s_abr: %s\n", __func__, str);
}
#else
static void ir_pwm_trace_print(struct ir_pwm *ir_pwm)
{
}
#endif /* IR_CORE_DEBUG */

static inline void ir_pwm_reset_buf(struct ir_pwm *ir_pwm)
{
	ir_pwm->tx_buf = NULL;
	ir_pwm->tx_cnt = 0;
	ir_pwm->tx_size = 0;
#ifdef IR_CORE_DEBUG
	memset(ir_pwm->abr_sleep, 0, sizeof(ir_pwm->abr_sleep));
#endif /* IR_CORE_DEBUG */
}

static int ir_pwm_transmit(struct device *dev, const unsigned int *buf, u32 n)
{
	ktime_t start;
	long delta;
	struct ir_pwm *ir_pwm = dev_get_drvdata(dev);

	if (!ir_pwm)
		return -ENODEV;

	ir_pwm->tx_buf = buf;
	ir_pwm->tx_size = n;
	ir_pwm->tx_cnt = 0;
	ir_pwm_config(ir_pwm);
	ir_pwm_config_pinctrl(ir_pwm, "pwm_enable");

	start = ktime_get();
	ir_pwm->tx_start = true;
	wake_up_interruptible(&ir_pwm->tx_wait);
	if (!wait_for_completion_timeout(&ir_pwm->tx_done, HZ * 10)) /* 10s */
		ir_pwm->tx_start = false;
	delta = ktime_us_delta(ktime_get(), start);

	pr_err("%s: use %ldus %u-%u\n", __func__, delta, ir_pwm->tx_cnt,
		ir_pwm->tx_size);
	ir_pwm_trace_print(ir_pwm);

	ir_pwm_disable(ir_pwm);
	if (ir_pwm->pwm_mode == USE_CLK_IMITATE)
		clk_unprepare(ir_pwm->clk);
	ir_pwm_config_pinctrl(ir_pwm, "default");
	ir_pwm_reset_buf(ir_pwm);
	return 0;
}

static void ir_pwm_get_mode(struct device *dev, struct ir_pwm *ir_pwm)
{
	struct device_node *np = dev->of_node;

	if (!np || of_property_read_u32(np, "pwm_mode", &ir_pwm->pwm_mode)) {
		hwlog_err("%s: pwm_mode tag not found\n", __func__);
		ir_pwm->pwm_mode = USE_CLK_IMITATE;
	}

	hwlog_info("%s: pwm mode is %u\n", __func__, ir_pwm->pwm_mode);
}

static void ir_pwm_get_clk_name(struct device *dev, struct ir_pwm *ir_pwm)
{
	struct device_node *np = dev->of_node;

	if (!np || of_property_read_string(np, "clock-names", &ir_pwm->clk_name))
		hwlog_err("%s: clock-names tag not found\n", __func__);

	if (!ir_pwm->clk_name)
		ir_pwm->clk_name = "default";

	hwlog_info("%s: clock name is %s\n", __func__, ir_pwm->clk_name);
}

static int ir_pwm_int_source(struct device *dev, struct ir_pwm *ir_pwm)
{
	switch (ir_pwm->pwm_mode) {
	case USE_PWM_SUBSYS:
		ir_pwm->pwm = devm_pwm_get(dev, NULL);
		if (IS_ERR(ir_pwm->pwm))
			return PTR_ERR(ir_pwm->pwm);
		break;
	case USE_CLK_IMITATE:
		ir_pwm_get_clk_name(dev, ir_pwm);
		ir_pwm->clk = devm_clk_get(dev, ir_pwm->clk_name);
		if (IS_ERR(ir_pwm->clk))
			return PTR_ERR(ir_pwm->clk);
		break;
	default:
		return -EFAULT;
	}

	ir_pwm->pctl = devm_pinctrl_get(dev);
	if (IS_ERR(ir_pwm->pctl))
		return PTR_ERR(ir_pwm->pctl);

	return 0;
}

static const struct ir_ops ir_pwm_ops = {
	.name = "irctrl",
	.tx = ir_pwm_transmit,
};

#ifdef IR_CORE_DEBUG
static int ir_pwm_dbg_dmode_set(void *data, u64 val)
{
	struct ir_pwm *ir_pwm = data;

	hwlog_info("%s: set delay mode to %llu\n", __func__, val);
	ir_pwm->delay_mode = val;
	return 0;
}

static int ir_pwm_dbg_dmode_get(void *data, u64 *val)
{
	struct ir_pwm *ir_pwm = data;

	*val = ir_pwm->delay_mode;
	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(ir_pwm_dmode_fops,
	ir_pwm_dbg_dmode_get, ir_pwm_dbg_dmode_set, "%llu\n");

static int ir_pwm_dbg_wave(void *data, u64 val)
{
	struct ir_pwm *ir_pwm = data;
	ktime_t start;
	long delta;

	switch (ir_pwm->pwm_mode) {
	case USE_PWM_SUBSYS:
		if (!ir_pwm->pwm)
			return -EFAULT;
		break;
	case USE_CLK_IMITATE:
		if (!ir_pwm->clk)
			return -EFAULT;
		break;
	default:
		return -EFAULT;
	}

	start = ktime_get();
	if (val == 0) { /* config + enable clk command */
		ir_pwm_config(ir_pwm);
		ir_pwm_config_pinctrl(ir_pwm, "pwm_enable");
		ir_pwm_enable(ir_pwm);
	} else if (val == 1) { /* enable clk command */
		ir_pwm_enable(ir_pwm);
	} else if (val == 2) { /* disable clk command */
		ir_pwm_disable(ir_pwm);
	} else if (val == 3) { /* unprepare clk command */
		if (ir_pwm->pwm_mode == USE_CLK_IMITATE)
			clk_unprepare(ir_pwm->clk);
		ir_pwm_config_pinctrl(ir_pwm, "default");
	}
	delta = ktime_us_delta(ktime_get(), start);

	hwlog_info("%s: pwm wave mode=%u,cmd=%lu,use %ldus\n", __func__, val,
		ir_pwm->pwm_mode, delta);
	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(ir_pwm_wave_fops,
	NULL, ir_pwm_dbg_wave, "%llu\n");

static int ir_pwm_dbg_pinctrl(void *data, u64 val)
{
	struct ir_pwm *ir_pwm = data;

	switch (val) {
	case 0: /* default cmd */
		ir_pwm_config_pinctrl(ir_pwm, "default");
		break;
	case 1: /* pwm enable cmd */
		ir_pwm_config_pinctrl(ir_pwm, "pwm_enable");
		break;
	default:
		return -EFAULT;
	}

	hwlog_info("%s: pwm pinctrl to %llu\n", __func__, val);
	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(ir_pwm_pinctrl_fops,
	NULL, ir_pwm_dbg_pinctrl, "%llu\n");

static void ir_pwm_debugfs_create(struct ir_pwm *ir_pwm)
{
	if (!ir_pwm->ir->sub)
		return;

	debugfs_create_file("delay_mode", 0600, ir_pwm->ir->sub, ir_pwm,
		&ir_pwm_dmode_fops);
	debugfs_create_file("pwm_wave", 0200, ir_pwm->ir->sub, ir_pwm,
		&ir_pwm_wave_fops);
	debugfs_create_file("pwm_pinctrl", 0200, ir_pwm->ir->sub, ir_pwm,
		&ir_pwm_pinctrl_fops);
}
#else
static void ir_pwm_debugfs_create(struct ir_pwm *ir_pwm)
{
}
#endif /* IR_CORE_DEBUG */

static int ir_pwm_probe(struct platform_device *pdev)
{
	int ret;
	struct ir_pwm *ir_pwm = NULL;

	ir_pwm = devm_kzalloc(&pdev->dev, sizeof(*ir_pwm), GFP_KERNEL);
	if (!ir_pwm)
		return -ENOMEM;

	ir_pwm_get_mode(&pdev->dev, ir_pwm);
	ret = ir_pwm_int_source(&pdev->dev, ir_pwm);
	if (ret) {
		hwlog_err("%s: init pwm source fail\n", __func__);
		return ret;
	}

	ir_pwm->ir = devm_ir_register_device(&pdev->dev, &ir_pwm_ops);
	if (!ir_pwm->ir) {
		hwlog_err("%s: ir pwm register fail\n", __func__);
		return -ENOMEM;
	}

	init_completion(&ir_pwm->tx_done);
	raw_spin_lock_init(&ir_pwm->tx_spin_lock);
	ir_pwm->task = kthread_create(ir_pwm_tx_thread, ir_pwm, "ir_tx_pwm.%p", ir_pwm);
	init_waitqueue_head(&ir_pwm->tx_wait);
	wake_up_process(ir_pwm->task);
	ir_pwm_debugfs_create(ir_pwm);

	dev_set_drvdata(&pdev->dev, ir_pwm);
	hwlog_info("%s-\n", __func__);
	return 0;
}

static const struct of_device_id ir_pwm_match_table[] = {
	{
		.compatible = "huawei,ir-pwm",
		.data = NULL,
	},
	{},
};

static struct platform_driver ir_pwm_driver = {
	.probe = ir_pwm_probe,
	.driver = {
		.name = "ir-pwm",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(ir_pwm_match_table),
	},
};

static int __init ir_pwm_init(void)
{
	return platform_driver_register(&ir_pwm_driver);
}

static void __exit ir_pwm_exit(void)
{
	platform_driver_unregister(&ir_pwm_driver);
}

fs_initcall(ir_pwm_init);
module_exit(ir_pwm_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("huawei ir pwm driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
