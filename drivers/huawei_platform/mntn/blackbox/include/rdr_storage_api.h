/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_storage_api.h
 * Description: rdr storage interfaces
 * Author: 2021-03-27 zhangxun dfx re-design
 */
#ifndef RDR_STORAGE_API_H
#define RDR_STORAGE_API_H

#define RDR_MAX_PATH_SIZE 128

int rdr_partition_write(const char *part_name, long offset, char *tmp_buf, size_t len);
int rdr_partition_read(const char *part_name, long offset, char *tmp_buf, size_t len);
long rdr_partition_size(const char *src);

int rdr_partition_offset_to_file(const char *file, long offset, long size, const char *partition);
int rdr_create_done_file(const char *name);
int rdr_file_create(const char *file, char *tmp_buf, size_t len);
int rdr_create_dir(const char *path);

int rdr_wait_partition(const char *path);

int rdr_safe_copy(char *dst, u32 dst_len, char *to_copy, u32 copy_len, char *region, u32 reg_len);

#endif