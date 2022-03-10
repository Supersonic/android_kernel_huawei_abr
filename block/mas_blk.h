/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: mas block header
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef MAS_BLK_INTERNAL_H
#define MAS_BLK_INTERNAL_H

#define IO_FROM_SUBMIT_BIO_MAGIC 0x4C
#define IO_FROM_BLK_EXEC 0x4D

#define BLK_LLD_INIT_MAGIC 0x7A
#define IO_BUF_LEN 10

#define TEN_MS 10
#define ONE_HUNDRED 100

#define REQ_FG_META (REQ_FG | REQ_META)

/* Used to device sqr sqw bw */
#define BLK_MID_SQW_BW  300
#define BLK_MAX_SQW_BW  500
#define BLK_MAX_SQR_BW  0
#define DEVICE_CAPACITY_128_G  0x10000000  /* 128G/512 */

#ifdef CONFIG_MAS_UNISTORE_PRESERVE
#define SECTOR_BYTE 9
#define SECTION_SECTOR 3
#define BLKSIZE 4096 /* bytes */
#define RESERVED_RESET_TAG_NUM 4

struct unistore_section_info {
	struct list_head section_list;
	sector_t section_start_lba;
	ktime_t section_insert_time;
	sector_t section_id;
	sector_t next_section_start_lba;
	sector_t next_section_id;
	bool slc_mode;
	bool rcv_io_complete_flag;
};
#endif

enum blk_lld_feature_bits {
	__BLK_LLD_DUMP_SUPPORT = 0,
	__BLK_LLD_LATENCY_SUPPORT,
	__BLK_LLD_FLUSH_REDUCE_SUPPORT,
	__BLK_LLD_BUSYIDLE_SUPPORT,
	__BLK_LLD_INLINE_CRYPTO_SUPPORT,
	__BLK_LLD_IOSCHED_UFS_MQ,
	__BLK_LLD_IOSCHED_UFS_HW_IDLE,
	__BLK_LLD_UFS_CP_EN,
	__BLK_LLD_UFS_ORDER_EN,
	__BLK_LLD_UFS_UNISTORE_EN,
};

#define BLK_LLD_DUMP_SUPPORT (1ULL << __BLK_LLD_DUMP_SUPPORT)
#define BLK_LLD_LATENCY_SUPPORT (1ULL << __BLK_LLD_LATENCY_SUPPORT)
#define BLK_LLD_FLUSH_REDUCE_SUPPORT (1ULL << __BLK_LLD_FLUSH_REDUCE_SUPPORT)
#define BLK_LLD_BUSYIDLE_SUPPORT (1ULL << __BLK_LLD_BUSYIDLE_SUPPORT)
#define BLK_LLD_INLINE_CRYPTO_SUPPORT (1ULL << __BLK_LLD_INLINE_CRYPTO_SUPPORT)
#define BLK_LLD_IOSCHED_UFS_MQ (1ULL << __BLK_LLD_IOSCHED_UFS_MQ)
#define BLK_LLD_IOSCHED_UFS_HW_IDLE (1ULL << __BLK_LLD_IOSCHED_UFS_HW_IDLE)
#define BLK_LLD_UFS_CP_EN (1ULL << __BLK_LLD_UFS_CP_EN)
#define BLK_LLD_UFS_ORDER_EN (1ULL << __BLK_LLD_UFS_ORDER_EN)
#define BLK_LLD_UFS_UNISTORE_EN (1ULL << __BLK_LLD_UFS_UNISTORE_EN)

#define BLK_QUEUE_DURATION_UNIT_NS 10000

enum blk_busyidle_nb_flag {
	BLK_BUSYIDLE_NB_NOT_JOIN_POLL = 0,
};

#define BLK_BUSYIDLE_NB_FLAG_NOT_JOIN_POLL                                    \
	(1 << BLK_BUSYIDLE_NB_NOT_JOIN_POLL)

enum iosched_strategy {
	IOSCHED_NONE = 0,
	IOSCHED_MAS_UFS_MQ,
};

struct blk_mq_ctx;
struct blk_mq_alloc_data;

/*
 * IO Scheduler Operation Function Pointer
 */
struct blk_queue_ops {
	enum iosched_strategy io_scheduler_strategy;
	/* MQ Scheduler Init */
	int (*mq_iosched_init_fn)(struct request_queue *);
	/* MQ Scheduler Deinit */
	void (*mq_iosched_exit_fn)(struct request_queue *);
	/* Prepare for MQ reqeust allocation */
	void (*mq_req_alloc_prep_fn)(struct blk_mq_alloc_data *data,
		unsigned long ioflag, bool fs_submit);
	/* Request init in MQ */
	void (*mq_req_init_fn)(
		struct request_queue *, struct blk_mq_ctx *, struct request *);
	/* Request complete in MQ */
	void (*mq_req_complete_fn)(
		struct request *, struct request_queue *, bool);
	/* Request deinit in MQ */
	void (*mq_req_deinit_fn)(struct request *);
	/* Request insert to MQ */
	void (*mq_req_insert_fn)(struct request *req, struct request_queue *);
	/* Request requeue in MQ */
	void (*mq_req_requeue_fn)(struct request *, struct request_queue *);
	/* Request timeout process in MQ */
	void (*mq_req_timeout_fn)(struct request *);
	/* Release the CTX in MQ */
	void (*mq_ctx_put_fn)(void);
	/* Get hctx object by request */
	void (*mq_hctx_get_by_req_fn)(
		struct request *, struct blk_mq_hw_ctx **);
	/* Get tag in MQ */
	unsigned int (*mq_tag_get_fn)(struct blk_mq_alloc_data *);
	/* Release tag in MQ */
	int (*mq_tag_put_fn)(
		struct blk_mq_hw_ctx *, unsigned int, struct request *);
	/* wakeup all threads waiting tag */
	int (*mq_tag_wakeup_all_fn)(struct blk_mq_tags *);
	/* Execute queue function directly in MQ */
	void (*mq_exec_queue_fn)(struct request_queue *);
	/* Run hw queue in MQ */
	void (*mq_run_hw_queue_fn)(struct request_queue *);
	/* Run requeue in MQ */
	void (*mq_run_requeue_fn)(struct request_queue *);
	/* Enable poll */
	void (*blk_poll_enable_fn)(bool *);
	void (*blk_write_throttle_fn)(struct request_queue *, bool);
	/* Dump ioscheduler status */
	void (*blk_status_dump_fn)(struct request_queue *,
		enum blk_dump_scene);
	/* Dump req status in ioscheduler */
	void (*blk_req_dump_fn)(struct request_queue *, enum blk_dump_scene);
	void *scheduler_priv;
	char iosched_name[ELV_NAME_MAX];
};

/*
 * MQ tagset Operation Function Pointer
 */
struct blk_tagset_ops {
	/* MQ tagset init */
	struct blk_mq_tags *(*tagset_init_tags_fn)(unsigned int total_tags,
		unsigned int reserved_tags, unsigned int high_prio_tags,
		int node, int alloc_policy);
	/* MQ tagset free */
	void (*tagset_free_tags_fn)(struct blk_mq_tags *);
	/* iterate all the busy tag in whole tagset */
	void (*tagset_all_tag_busy_iter_fn)(
		struct blk_mq_tags *, busy_tag_iter_fn *, const void *);
	void (*tagset_tag_busy_iter_fn)(
		struct blk_mq_hw_ctx *, busy_iter_fn *, void *);
	void (*tagset_power_off_proc_fn)(struct blk_dev_lld *);
	struct blk_queue_ops *queue_ops;
};

struct bio_delay_stage_config {
	char *stage_name;
	void (*function)(struct bio *bio);
};

struct req_delay_stage_config {
	char *stage_name;
	void (*function)(struct request *req);
};

#ifdef CONFIG_MAS_BLK
void mas_blk_bio_clone_fast(
	struct bio *bio, struct bio *bio_src);
extern void mas_blk_bio_queue_split(
	struct request_queue *q, struct bio **bio, struct bio *split);
extern void mas_blk_bio_split_pre(
	const struct bio *bio, const struct bio *split);
extern void mas_blk_bio_split_post(const struct bio *bio);
extern bool mas_blk_bio_merge_allow(const struct request *rq,
	const struct bio *bio);
extern void mas_blk_bio_merge_done(
	const struct request_queue *q, const struct request *req,
	const struct request *next);
extern int mas_blk_account_io_completion(
	const struct request *req, unsigned int bytes);
struct request_queue *mas_blk_get_queue_by_lld(
	struct blk_dev_lld *lld);
int mas_blk_cust_ioctl(struct block_device *bdev,
	struct blkdev_cmd __user *arg);
extern int mas_blk_generic_make_request_check(struct bio *bio);
extern void mas_blk_generic_make_request(const struct bio *bio);
extern void mas_blk_start_plug(struct blk_plug *plug);
extern void mas_blk_flush_plug_list(
	const struct blk_plug *plug, bool from_schedule);
extern void mas_blk_bio_endio(const struct bio *bio);
extern void mas_blk_bio_free(const struct bio *bio);

extern void mas_blk_request_init_from_bio(
	struct request *req, struct bio *bio);
extern void mas_blk_insert_cloned_request(
	const struct request_queue *q, const struct request *rq);
extern void mas_blk_request_execute_nowait(
	const struct request_queue *q, const struct request *rq,
	rq_end_io_fn *done);
extern void mas_blk_mq_rq_ctx_init(
	struct request_queue *q, struct blk_mq_ctx *ctx, struct request *rq);
extern void mas_blk_mq_request_start(const struct request *rq);
extern void mas_blk_request_start(const struct request *req);
extern void mas_blk_requeue_request(const struct request *rq);
extern void mas_blk_request_update(
	const struct request *req, blk_status_t error, unsigned int nr_bytes);
extern void mas_blk_mq_request_free(struct request *rq);
extern void mas_blk_request_put(struct request *req);

extern void mas_blk_check_partition_done(
	struct gendisk *disk, bool has_part_tbl);
extern void mas_blk_queue_register(
	struct request_queue *q, const struct gendisk *disk);
extern void mas_blk_mq_init_allocated_queue(struct request_queue *q);
extern void mas_blk_sq_init_allocated_queue(const struct request_queue *q);
extern void mas_blk_allocated_queue_init(struct request_queue *q);
extern void mas_blk_mq_free_queue(const struct request_queue *q);
extern void mas_blk_cleanup_queue(struct request_queue *q);

extern void mas_blk_mq_allocated_tagset_init(struct blk_mq_tag_set *set);
extern void blk_add_queue_tags(
	struct blk_queue_tag *tags, struct request_queue *q);
extern void mas_blk_allocated_tags_init(struct blk_queue_tag *tags);

extern int mas_blk_dev_init(void);

extern struct blk_dev_lld *mas_blk_get_lld(struct request_queue *q);

extern void mas_blk_busyidle_check_bio(
	const struct request_queue *q, struct bio *bio);
extern bool mas_blk_busyidle_check_request_bio(
	const struct request_queue *q, const struct request *rq);
extern void mas_blk_busyidle_check_execute_request(
	const struct request_queue *q, struct request *rq,
	rq_end_io_fn *done);
extern void mas_blk_busyidle_check_bio_endio(struct bio *bio);
extern int mas_blk_busyidle_state_init(struct blk_idle_state *blk_idle);

extern void mas_blk_latency_init(void);
extern void mas_blk_queue_latency_init(struct request_queue *q);
extern void mas_blk_latency_bio_check(
	struct bio *bio, enum bio_process_stage_enum bio_stage);
extern void mas_blk_latency_req_check(
	struct request *req, enum req_process_stage_enum req_stage);
extern void mas_blk_latency_for_merge(
	const struct request *req, const struct request *next);
extern void mas_blk_queue_async_flush_init(struct request_queue *q);
extern void mas_blk_flush_update(const struct request *req, int error);
extern bool mas_blk_flush_async_dispatch(
	struct request_queue *q, const struct bio *bio);
extern void mas_blk_dump_register_queue(struct request_queue *q);
extern void mas_blk_dump_unregister_queue(struct request_queue *q);
extern void mas_blk_dump_bio(const struct bio *bio, enum blk_dump_scene s);
extern void mas_blk_dump_request(struct request *rq, enum blk_dump_scene s);
extern void mas_blk_dump_queue_status(
	const struct request_queue *q,  enum blk_dump_scene s);
extern int mas_blk_dump_queue_status2(
	struct request_queue *q, char *buf, int len);
extern int mas_blk_dump_lld_status(
	struct blk_dev_lld *lld, char *buf, int len);
extern void mas_blk_dump_init(void);

extern void mas_blk_queue_usr_ctrl_set(struct request_queue *q);
#ifdef CONFIG_MAS_MQ_USING_CP
static inline bool mas_blk_bio_is_cp(const struct bio *bio)
{
	return (bio->bi_opf & REQ_FG_META) ? true : false;
}

#endif

extern void blk_req_set_make_req_nr(struct request *req);

#define NO_EXTRA_MSG NULL

#ifdef CONFIG_MAS_UNISTORE_PRESERVE
void mas_blk_partition_remap(
	struct bio *bio, struct hd_struct *p);
bool mas_blk_match_expected_lba(
	struct request_queue *q, struct bio *bio);
void mas_blk_set_lba_flag(
	struct request *rq, struct blk_dev_lld *lld);
void mas_blk_set_data_flag(struct request_queue *q,
	struct blk_mq_alloc_data *data, struct bio *bio);
bool mas_blk_bio_need_dispatch(struct bio *bio,
	struct request_queue *q, struct stor_dev_pwron_info* stor_info);
void mas_blk_update_recovery_bio_page(struct bio *bio);
void mas_blk_dev_lld_init_unistore(
	struct blk_dev_lld *blk_lld);
void mas_blk_bio_set_opf_unistore(struct bio *bio);
void mas_blk_request_init_from_bio_unistore(
	struct request *req, struct bio *bio);
void mas_blk_order_info_reset(struct blk_dev_lld *lld);
void mas_blk_req_update_unistore(
	struct request *req, blk_status_t error, unsigned int nr_bytes);
struct bio* mas_blk_bio_segment_bytes_split(
	struct bio *bio, struct bio_set *bs, unsigned int bytes);
unsigned int mas_blk_bio_get_residual_byte(
	struct request_queue *q, struct bvec_iter iter);
bool mas_blk_bio_check_over_section(
	struct request_queue *q, struct bio *bio);
void mas_blk_request_init_unistore(struct request *req);
bool mas_blk_expected_lba_pu(struct blk_dev_lld *lld,
	unsigned char stream, sector_t req_lba);
bool mas_blk_enable_disorder_stream(unsigned char stream_type);
void mas_blk_recovery_update_section_list(struct blk_dev_lld *lld,
	sector_t expected_lba, unsigned char stream_type);
bool mas_blk_is_section_ready(struct request_queue *q, struct bio *bio);
void mas_blk_update_expected_info(struct blk_dev_lld *lld,
	sector_t update_lba, unsigned char stream_type);
bool mas_blk_is_reserved_empty(
	struct blk_mq_alloc_data *data);
void mas_blk_check_fg_io(struct bio *bio);

static inline bool mas_blk_is_order_stream(unsigned char stream_type)
{
	return stream_type && (stream_type <= BLK_STREAM_HOT_DATA);
}

#if defined(CONFIG_MAS_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
ssize_t mas_queue_device_pwron_info_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_device_reset_ftl_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_device_read_section_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_device_read_pu_size_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_device_config_mapping_partition_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_device_config_mapping_partition_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_stream_oob_info_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_device_fs_sync_done_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_device_rescue_block_inject_data_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_device_data_move_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_device_data_move_num_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_device_slc_mode_configuration_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_device_sync_read_verify_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_device_sync_read_verify_cp_verify_l4k_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_device_sync_read_verify_cp_open_l4k_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_device_bad_block_notify_regist_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_device_bad_block_total_num_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_device_bad_block_bad_num_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_unistore_debug_en_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_unistore_debug_en_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_unistore_en_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_recovery_debug_on_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_recovery_debug_on_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_recovery_page_cnt_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_reset_cnt_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_enable_disorder_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_enable_disorder_store(
	struct request_queue *q, const char *page, size_t count);
unsigned int mas_blk_get_max_recovery_num(void);
ssize_t mas_queue_max_recovery_num_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_max_recovery_num_store(
	struct request_queue *q, const char *page, size_t count);
ssize_t mas_queue_recovery_del_num_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_recovery_del_num_store(
	struct request_queue *q, const char *page, size_t count);
unsigned int mas_blk_get_max_del_num(void);
ssize_t mas_queue_reset_recovery_off_show(
	struct request_queue *q, char *page);
ssize_t mas_queue_reset_recovery_off_store(
	struct request_queue *q, const char *page, size_t count);
bool mas_blk_enable_disorder(void);
#else
static inline bool mas_blk_enable_disorder(void)
{
	return true;
}
#endif
#endif

#if defined(CONFIG_MAS_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
extern ssize_t mas_queue_io_latency_warning_threshold_store(
	const struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_timeout_tst_enable_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_io_latency_tst_enable_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_busyidle_tst_enable_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_busyidle_multi_nb_tst_enable_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_busyidle_tst_proc_result_simulate_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_busyidle_tst_proc_latency_simulate_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_apd_tst_enable_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_suspend_tst_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_io_prio_sim_show(
	const struct request_queue *q, char *page);
extern ssize_t mas_queue_io_prio_sim_store(
	struct request_queue *q, const char *page, size_t count);
#ifdef CONFIG_MAS_MQ_USING_CP
extern ssize_t mas_queue_cp_enabled_show(struct request_queue *q, char *page);
extern ssize_t mas_queue_cp_enabled_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_cp_debug_en_show(struct request_queue *q, char *page);
extern ssize_t mas_queue_cp_debug_en_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_cp_limit_show(struct request_queue *q, char *page);
extern ssize_t mas_queue_cp_limit_store(
	struct request_queue *q, const char *page, size_t count);
#endif /* CONFIG_MAS_MQ_USING_CP */
static inline void mas_blk_rdr_panic(char *msg)
{
	if (msg)
		pr_err("%s\n", msg);
	BUG_ON(1);
}
#ifdef CONFIG_MAS_ORDER_PRESERVE
extern ssize_t mas_queue_order_enabled_show(struct request_queue *q, char *page);
extern ssize_t mas_queue_order_enabled_store(
	struct request_queue *q, const char *page, size_t count);
extern ssize_t mas_queue_order_debug_en_show(struct request_queue *q, char *page);
extern ssize_t mas_queue_order_debug_en_store(
	struct request_queue *q, const char *page, size_t count);
extern int mas_blk_order_debug_en(void);
#endif /* CONFIG_MAS_ORDER_PRESERVE */
#else
static inline void mas_blk_rdr_panic(char *msg)
{
}
#endif /* CONFIG_MAS_DEBUG_FS */

extern ssize_t queue_var_store(
	unsigned long *var, const char *page, size_t count);

static inline void mas_blk_dump_fs(const struct bio *bio)
{
	if (bio->dump_fs)
		bio->dump_fs();
}
#endif /* CONFIG_MAS_BLK */

#endif
