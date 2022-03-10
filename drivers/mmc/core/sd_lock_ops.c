#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/scatterlist.h>
#include "core.h"
#include "lock.h"
#include "mmc_ops.h"
#include "sd_lock_ops.h"

void mmc_sd_go_highspeed(struct mmc_card *card)
{
	//mmc_card_set_highspeed(card);//temp remove this
	mmc_set_timing(card->host, MMC_TIMING_SD_HS);
}

int sd_send_status(struct mmc_card *card)
{
	int err;
	u32 status;
	do {
		err = mmc_send_status(card, &status);
		if (err)
		{
			break;
		}
		if (card->host->caps & MMC_CAP_WAIT_WHILE_BUSY)
			break;
		if (mmc_host_is_spi(card->host))
			break;
	} while (R1_CURRENT_STATE(status) == 7);

	return err;
}

int sd_send_blocklen(struct mmc_card *card,unsigned int blocklen)
{
	int err;
	struct mmc_command cmd = {0};

	cmd.opcode = MMC_SET_BLOCKLEN;
	cmd.arg = blocklen;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
	err = mmc_wait_for_cmd(card->host, &cmd, MMC_CMD_RETRIES);
	if (err)
	{
		printk("%s failed blocklen=%d \n",__func__,blocklen);
	}
	return err;
}

int sd_send_lock_unlock_cmd(struct mmc_card *card,u8* data_buf,int data_size,int max_buf_size)
{
	int err;
	struct mmc_request mrq = {0};
	struct mmc_command cmd = {0};
	struct mmc_data data = {0};
	struct scatterlist sg;

	cmd.opcode = MMC_LOCK_UNLOCK;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;

	data.blksz = data_size;
	data.blocks = 1;
	data.flags = MMC_DATA_WRITE;
	data.sg = &sg;
	data.sg_len = 1;
	mmc_set_data_timeout(&data, card);
	data.timeout_ns = (4*1000*1000*1000u);

	mrq.cmd = &cmd;
	mrq.data = &data;

	sg_init_one(&sg, data_buf, max_buf_size);
	mmc_wait_for_req(card->host, &mrq);
	err = cmd.error;
	if (err) {
		printk("%s: lock unlock cmd error %d\n", __func__, cmd.error);
		return err;
	}

	err = data.error;
	if (err) {
		dev_err(mmc_dev(card->host), "%s: data error %d\n",__func__, data.error);
	}
	return err;
}

int sd_wait_lock_unlock_cmd(struct mmc_card *card,int mode)
{
	int err;
	struct mmc_command cmd = {0};
	unsigned long erase_timeout;
	unsigned long normal_timeout;


	cmd.opcode = MMC_SEND_STATUS;
	cmd.arg = card->rca << 16;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

	/* set timeout for forced erase operation to 3 min. (see MMC spec) */
	erase_timeout = jiffies + 180 * HZ;
	normal_timeout = jiffies + 10 * HZ;
	if(mode & MMC_LOCK_MODE_ERASE) {
		do {
			/* we cannot use "retries" here because the
			 * R1_LOCK_UNLOCK_FAILED bit is cleared by subsequent reads to
			 * the status register, hiding the error condition */
			err = mmc_wait_for_cmd(card->host, &cmd, 0);
			if (err) {
				printk("[SDLOCK] %s mmc_wait_for_cmd err=%d resp[0] =%x \n",__func__,err,cmd.resp[0]);
				break;
			}

			/* the other modes don't need timeout checking */
			if (!(mode & MMC_LOCK_MODE_ERASE))
				continue;
			if (time_after(jiffies, erase_timeout)) {
				dev_dbg(&card->dev, "forced erase timed out\n");
				err = -ETIMEDOUT;
				break;
			}
		} while (!(cmd.resp[0] & R1_READY_FOR_DATA) || (cmd.resp[0] & R1_CARD_IS_LOCKED));
	} else {
		do {
			/* we cannot use "retries" here because the
			 * R1_LOCK_UNLOCK_FAILED bit is cleared by subsequent reads to
			 * the status register, hiding the error condition */
			err = mmc_wait_for_cmd(card->host, &cmd, 0);
			if (err) {
				printk("[SDLOCK] %s mmc_wait_for_cmd err=%d resp[0] =%x \n",__func__,err,cmd.resp[0]);
				break;
			}

			if (time_after(jiffies, normal_timeout)) {
				dev_dbg(&card->dev, "normal timed out\n");
				err = -ETIMEDOUT;
				printk("[SDLOCK] %s normal timed out err=%d resp[0] =%x \n",__func__,err,cmd.resp[0]);
				break;
			}
		} while (!(cmd.resp[0] & R1_READY_FOR_DATA));
	}

	printk("[SDLOCK] %s MMC_SEND_STATUS and cmd.resp[0] = 0x%x. \r\n",__func__, cmd.resp[0]);

	if (cmd.resp[0] & R1_LOCK_UNLOCK_FAILED) {
		printk("%s: LOCK_UNLOCK operation failed\n", __func__);
		err = -EIO;
		return err;
	}

	if (cmd.resp[0] & R1_CARD_IS_LOCKED)
	{
		printk("%s: R1_CARD_IS_LOCKED\n", __func__);
		mmc_card_set_locked(card);
	}
	else
	{
		printk("%s: R1_CARD_IS_UNLOCKED\n", __func__);
		card->state &= ~MMC_STATE_LOCKED;
	}

	return err;
}

static int sd_init_get_lock_status(unsigned int ocr, struct mmc_card *card)
{
	u32 status = 0;
	int ret = 0;
	int err = 0;
	
	/* whether voltage switch just for sd card */
	if (ocr & SD_ROCR_S18A)
	{
		card->swith_voltage = true;
	}
	else
	{
		card->swith_voltage = false;
	}
	printk("%s, sd card voltage swith(3.3v--> 1.8v) is %d\n", __func__, card->swith_voltage);

	/*
	 * Check if card is locked.
	 */
	err = mmc_send_status(card, &status);
	if (err)
	{
	   ret = -1;
	   return ret;
	}
	
	if (status & R1_CARD_IS_LOCKED) {
		mmc_card_set_encrypted(card);
		mmc_card_set_locked(card);
	}
	
	if (status & R1_CARD_IS_LOCKED) 
	{
		if (card->auto_unlock ) 
		{
			if (card->unlock_pwd[0] > 0 ) 
			{
				//unlock sd card
				err = mmc_lock_unlock_by_buf(card,  card->unlock_pwd+1, (int)card->unlock_pwd[0], MMC_LOCK_MODE_UNLOCK);
				if(err) 
				{
					printk("[SDLOCK] %s unlock failed \n", __func__);
				} 
				else 
				{
					printk("[SDLOCK] %s unlock success \n", __func__);
					if(!mmc_card_locked(card)) 
					{
						card->auto_unlock = false;
						printk("[SDLOCK] %s unlock success and sdcard status is unlocked.\n", __func__);
					} 
					else 
					{
						printk("[SDLOCK] %s unlock success but sdcard status is locked, abnormal status.\n", __func__);
					}
				}
				//Check if card is locked
				err = mmc_send_status(card, &status);
				if (err) 
				{
					printk("[SDLOCK] %s resume sd card exception /n", __func__);
					ret = -1;
					return ret;

				}
			} 
			else 
			{
				printk("[SDLOCK] %s unlock password is null\n", __func__);
			}
		}

		if (status & R1_CARD_IS_LOCKED) 
		{
			printk(KERN_WARNING "[SDLOCK] sdcard is locked\n");
			ret = 1;
			return ret;

		} 
		else 
		{
			printk(KERN_WARNING "[SDLOCK] sdcard resume to unlocked\n");
		}
	} 
	else 
	{
		printk(KERN_WARNING "[SDLOCK] sdcard is unlocked\n");
	}
	return ret;
}

void sd_suspend_set_lock_flag(struct mmc_card * card) {
 	if (!mmc_card_locked(card)) {
		printk(KERN_WARNING "[SDLOCK]: sdcard is unlocked on suspend and set auto_unlock = true. \n");
		card->auto_unlock = true;
	}
	
	return;
}

/*
 * Adds sysfs entries as relevant.
 */
int mmc_sd_sysfs_add(struct mmc_host *host, struct mmc_card *card)
{
	int ret;

	ret = mmc_lock_add_sysfs(card);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

/*
 * Removes the sysfs entries added by mmc_sysfs_add().
 */
void mmc_sd_sysfs_remove(struct mmc_host *host, struct mmc_card *card)
{
	mmc_lock_remove_sysfs(card);
}

static struct mmc_sdlock_ops sd_lock_ops = {
	.init_get_lock_status = sd_init_get_lock_status,
	.suspend_set_lock_flag = sd_suspend_set_lock_flag,
};

struct mmc_sdlock_ops * sd_lock_func_attach(void) {
	struct mmc_sdlock_ops *ops= NULL;
	
	ops = &sd_lock_ops;

  	if(NULL != ops) {
		return ops;
	}
	return ops;
}

