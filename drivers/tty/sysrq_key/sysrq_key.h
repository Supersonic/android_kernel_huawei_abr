#ifdef CONFIG_HANDSET_SYSRQ_RESET
#ifndef SYSRQ_KEY_H_
#define SYSRQ_KEY_H_
struct sysrq_state {
	struct input_handle handle;
	struct work_struct reinject_work;
	unsigned long key_down[BITS_TO_LONGS(KEY_CNT)];
	unsigned int alt;
	unsigned int alt_use;
	bool active;
	bool need_reinject;
	bool reinjecting;

	/* reset sequence handling */
	bool reset_canceled;
	bool reset_requested;
	unsigned long reset_keybit[BITS_TO_LONGS(KEY_CNT)];
	int reset_seq_len;
	int reset_seq_cnt;
	int reset_seq_version;
	struct timer_list keyreset_timer;
};
bool sysrq_handle_keypress(struct sysrq_state *sysrq, unsigned int code, int value);
void sysrq_key_init(void);
#endif
#endif
