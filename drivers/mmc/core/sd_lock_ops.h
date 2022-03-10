/*
 *  linux/drivers/mmc/core/lock.h
 *
 *  Copyright 2006 Instituto Nokia de Tecnologia (INdT), All Rights Reserved.
 *  Copyright 2007 Pierre Ossman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _MMC_LOCK_OPS_H
#define _MMC_LOCK_OPS_H

#ifdef CONFIG_MMC_PASSWORDS

#define MMC_STATE_LOCKED	(1<<14)		/* card is currently locked */
#define MMC_STATE_ENCRYPT	(1<<15)		/* card is currently encrypt */

#define mmc_card_locked(c)	((c)->state & MMC_STATE_LOCKED)
#define mmc_card_encrypt(c)	((c)->state & MMC_STATE_ENCRYPT)
#define mmc_card_set_locked(c)	((c)->state |= MMC_STATE_LOCKED)
#define mmc_card_set_encrypted(c)	((c)->state |= MMC_STATE_ENCRYPT)

/*
 * MMC_LOCK_UNLOCK modes
 */
#define MMC_LOCK_MODE_ERASE		(1<<3)
#define MMC_LOCK_MODE_LOCK		(1<<2)
#define MMC_LOCK_MODE_CLR_PWD	        (1<<1)
#define MMC_LOCK_MODE_SET_PWD	        (1<<0)
#define MMC_LOCK_MODE_UNLOCK  		(1<<4)

struct mmc_card;
struct mmc_host;

struct mmc_sdlock_ops {
	int (*init_get_lock_status)(unsigned int, struct mmc_card *);
	void(*suspend_set_lock_flag)(struct mmc_card *);
};


struct mmc_sdlock_ops * sd_lock_func_attach(void);
int sd_send_status(struct mmc_card *card);
int sd_send_blocklen(struct mmc_card *card,unsigned int blocklen);
int sd_send_lock_unlock_cmd(struct mmc_card *card,u8* data_buf,int data_size,int max_buf_size);
int sd_wait_lock_unlock_cmd(struct mmc_card *card,int mode);
void mmc_sd_go_highspeed(struct mmc_card *card);

int mmc_sd_sysfs_add(struct mmc_host *host, struct mmc_card *card);
void mmc_sd_sysfs_remove(struct mmc_host *host, struct mmc_card *card);

#endif
#endif
