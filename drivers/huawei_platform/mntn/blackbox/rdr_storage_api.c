/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_storage_api.c
 * Description: rdr abstration of storage r/w operations
 * Author: 2021-03-27 zhangxun dfx re-design
 */
#include <linux/syscalls.h>
#include <linux/printk.h>
#include <linux/fs.h>

#include "rdr_inner.h"
#include "rdr_storage_api.h"

#include "securec.h"

#define DIR_LIMIT           (S_IRWXU | S_IRWXG)
#define FILE_LIMIT (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define RDR_BLOCK_SIZE (SZ_4K / SZ_4) // set to 1024 in case of stack-overflow

/*
 * Description : General API of partition write
 */
int rdr_partition_write(const char *part_name, long offset, char *tmp_buf, size_t len)
{
	int ret;
	int fd_dfx;

	fd_dfx = SYS_OPEN(part_name, O_WRONLY, FILE_LIMIT);
	if (fd_dfx < 0) {
		PRINT_ERROR("%s():%d:open fail[%d]\n", __func__, __LINE__, fd_dfx);
		return -1;
	}
	SYS_LSEEK(fd_dfx, offset, SEEK_SET);

	ret = SYS_WRITE(fd_dfx, tmp_buf, len);
	if (ret <= 0) {
		PRINT_ERROR("%s():%d:write fail\n", __func__, __LINE__);
		goto close;
	}

	ret = (int)SYS_FSYNC(fd_dfx);
	if (ret < 0)
		PRINT_ERROR("[%s]sys_fsync failed, ret is %d\n", __func__, ret);
	SYS_CLOSE(fd_dfx);
	return 0;
close:
	ret = (int)SYS_FSYNC(fd_dfx);
	if (ret < 0)
		PRINT_ERROR("[%s]sys_fsync failed, ret is %d\n", __func__, ret);
	SYS_CLOSE(fd_dfx);
	return -1;
}

/*
 * Description : General API of partition read
 */
int rdr_partition_read(const char *part_name, long offset, char *tmp_buf, size_t len)
{
	int ret;
	int fd_dfx;
	fd_dfx = SYS_OPEN(part_name, O_RDONLY, FILE_LIMIT);
	if (fd_dfx < 0) {
		PRINT_ERROR("%s():%d:open fail[%d]\n", __func__, __LINE__, fd_dfx);
		return -1;
	}
	SYS_LSEEK(fd_dfx, offset, SEEK_SET);

	ret = SYS_READ(fd_dfx, tmp_buf, len);
	if (ret <= 0) {
		PRINT_ERROR("%s():%d:read fail\n", __func__, __LINE__);
		goto close;
	}
	SYS_CLOSE(fd_dfx);
	return 0;
close:
	SYS_CLOSE(fd_dfx);
	return -1;
}


/*
 * Description : Get the size of a file/partition (using lseek)
 */
long rdr_partition_size(const char *src)
{
	int fdsrc;
	long offset;
	fdsrc = SYS_OPEN(src, O_RDONLY, FILE_LIMIT);
	if (fdsrc < 0)
		return 0;
	offset = SYS_LSEEK(fdsrc, 0, SEEK_END);
	PRINT_INFO("%s():%d:seek value[%lx]\n", __func__, __LINE__, offset);
	SYS_CLOSE(fdsrc);
	return offset;
}

#define DUMP_MAX_SIZE                     0x80000000
/*
 * Description : Copy subset of partition (given: offset and size) to an actual file in file-system
 */
int rdr_partition_offset_to_file(const char *file, long offset, long size, const char *partition)
{
	int fdsrc, fddst;
	char buf[RDR_BLOCK_SIZE];
	long cnt, seek_return;
	int ret = 0;

	if (!file || !partition) {
		PRINT_ERROR("rdr:%s():dst or src is NULL\n", __func__);
		return -1;
	}
	PRINT_INFO("rdr:%s():dst=%s or src=%s\n", __func__, file, partition);

	fdsrc = SYS_OPEN(partition, O_RDONLY, FILE_LIMIT);
	if (fdsrc < 0) {
		PRINT_ERROR("rdr:%s():open %s failed, return [%d]\n", __func__, partition, fdsrc);
		ret = -1;
		return ret;
	}

	fddst = SYS_OPEN(file, O_CREAT | O_WRONLY, FILE_LIMIT);
	if (fddst < 0) {
		PRINT_ERROR("rdr:%s():open %s failed, return [%d]\n", __func__, file, fddst);
		SYS_CLOSE(fdsrc);
		ret = -1;
		return ret;
	}

	/* Offset address read, seek to resize addr */
	PRINT_INFO("%s():%d:seek value[%lx]\n", __func__, __LINE__, offset);
	seek_return = (long)SYS_LSEEK(fdsrc, offset, SEEK_SET);
	if (seek_return < 0) {
		PRINT_ERROR("%s():%d:lseek fail[%ld]\n", __func__, __LINE__, seek_return);
		ret = -1;
		goto close;
	}

	/* start to dump flash to data partiton */
	while (size > 0 && size <= DUMP_MAX_SIZE) {
		cnt = SYS_READ(fdsrc, buf, RDR_BLOCK_SIZE);
		if (cnt == 0)
			break;
		if (cnt < 0) {
			PRINT_ERROR("rdr:%s():read %s failed, return [%ld]\n", __func__, partition, cnt);
			ret = -1;
			goto close;
		}

		cnt = SYS_WRITE(fddst, buf, RDR_BLOCK_SIZE);
		if (cnt <= 0) {
			PRINT_ERROR("rdr:%s():write %s failed, return [%ld]\n", __func__, file, cnt);
			ret = -1;
			goto close;
		}

		size = size - RDR_BLOCK_SIZE;
	}

close:
	SYS_CLOSE(fdsrc);
	SYS_CLOSE(fddst);
	return ret;
}

int rdr_create_done_file(const char *name)
{
	int fd;

	if (strncmp(name, "/data", strlen("/data")) != 0) {
		PRINT_ERROR("invalid input path name\n");
		return -1;
	}

	fd = SYS_OPEN(name, O_CREAT | O_RDONLY, FILE_LIMIT);
	if (fd < 0) {
		PRINT_ERROR("create file[%s] fail,ret:%d\n", name, fd);
		return -1;
	}
	SYS_CLOSE(fd);
	return 0;
}

/*
 * Description : General API of file write
 */
int rdr_file_create(const char *file, char *tmp_buf, size_t len)
{
	int ret;
	int fd_dfx;

	fd_dfx = SYS_OPEN(file, O_CREAT | O_WRONLY | O_TRUNC, FILE_LIMIT);
	if (fd_dfx < 0) {
		PRINT_ERROR("%s():%d:open fail[%d]\n", __func__, __LINE__, fd_dfx);
		return -1;
	}

	ret = SYS_WRITE(fd_dfx, tmp_buf, len);
	if (ret <= 0) {
		PRINT_ERROR("%s():%d:write fail\n", __func__, __LINE__);
		goto close;
	}

	ret = (int)SYS_FSYNC(fd_dfx);
	if (ret < 0)
		PRINT_ERROR("[%s]sys_fsync failed, ret is %d\n", __func__, ret);
	SYS_CLOSE(fd_dfx);
	return 0;
close:
	ret = (int)SYS_FSYNC(fd_dfx);
	if (ret < 0)
		PRINT_ERROR("[%s]sys_fsync failed, ret is %d\n", __func__, ret);
	SYS_CLOSE(fd_dfx);
	return -1;
}

static int __rdr_create_dir(const char *path)
{
	int fd;

	if (path == NULL) {
		PRINT_ERROR("invalid parameter path\n");
		PRINT_END();
		return -1;
	}

	fd = SYS_ACCESS(path, 0);
	if (fd != 0) {
		PRINT_INFO("rdr: need create dir %s\n", path);
		fd = SYS_MKDIR(path, DIR_LIMIT);
		if (fd < 0) {
			PRINT_ERROR("rdr: create dir %s failed! ret = %d\n", path, fd);
			PRINT_END();
			return fd;
		}
		PRINT_INFO("rdr: create dir %s successed [%d]!!!\n", path, fd);
	}

	return 0;
}

int rdr_create_dir(const char *path)
{
	char cur_path[RDR_MAX_PATH_SIZE];
	int index = 0;

	PRINT_START();
	if (path == NULL) {
		PRINT_ERROR("invalid  parameter path\n");
		PRINT_END();
		return -1;
	}
	(void)memset_s(cur_path, RDR_MAX_PATH_SIZE, 0, RDR_MAX_PATH_SIZE);

	if (*path != '/')
		return -1;
	cur_path[index++] = *path++;

	while (*path != '\0') {
		if (*path == '/') {
			if (__rdr_create_dir(cur_path) < 0)
				PRINT_ERROR("rdr: create dir failed\n");
		}
		cur_path[index] = *path;
		path++;
		index++;
	}

	PRINT_END();
	return 0;
}


static int rdr_wait_partition_timeout(const char *path, int timeouts)
{
	struct kstat m_stat;
	int timeo;

	if (path == NULL) {
		PRINT_ERROR("invalid  parameter path\n");
		return -1;
	}

	timeo = timeouts;

	while (vfs_stat(path, &m_stat) != 0) {
		current->state = TASK_INTERRUPTIBLE;
		(void)schedule_timeout(HZ);    /* wait for 1 second */
		if (timeouts-- < 0) {
			PRINT_ERROR("%d:rdr:wait partiton[%s] fail. use [%d]'s . skip!\n", __LINE__,
				path, timeo);
			return -1;
		}
	}

	return 0;
}

#define RDR_TIMEOUT 30
#define RDR_WAIT_TRY_TIMES 1
int rdr_wait_partition(const char *path)
{
	int count = 0;
	PRINT_START();
	while (rdr_wait_partition_timeout(path, RDR_TIMEOUT) != 0) {
		if (count >= RDR_WAIT_TRY_TIMES) {
			PRINT_ERROR("%s(): wait failed, exit", __func__);
			return -1;
		}
		count++;
	}
	PRINT_END();
	return 0;
}

int rdr_safe_copy(char *dst, u32 dst_len, char *to_copy, u32 copy_len, char *region, u32 reg_len)
{
	u64 start, end, cp_start, cp_end;
	start = (u64)region;
	end = (u64)region + reg_len;
	cp_start = (u64)to_copy;
	cp_end = (u64)to_copy + copy_len;
	if (cp_start < start || cp_end > end) {
		PRINT_ERROR("%s(): src overflow! ACCESS: [%p - %p], VALID: [%p - %p]",
			__func__, to_copy, to_copy + copy_len, region, region + reg_len);
		return -1;
	}
	if (memcpy_s(dst, dst_len, to_copy, copy_len) != 0) {
		PRINT_ERROR("%s(): memcpy_s failed, dst: %p, src: %p, reg: %p, dst_len: %u, src_len:%u, region: %u",
			__func__, dst, to_copy, region, dst_len, copy_len, reg_len);
		return -1;
	}
	return 0;
}
