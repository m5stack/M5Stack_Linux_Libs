/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * File System Helper Functions
 *
 */
#ifndef __FF_WRAPPER_H__
#define __FF_WRAPPER_H__
#include <stdint.h>
// #include <ff.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C"
{
#endif
typedef FILE* FIL;
// extern const char *ffs_strerror(FRESULT res);

//OOFATFS wrappers
// FRESULT f_open_helper(FIL *fp, const TCHAR *path, BYTE mode);
// FRESULT f_opendir_helper(FF_DIR *dp, const TCHAR *path);
// FRESULT f_stat_helper(const TCHAR *path, FILEINFO *fno);
// FRESULT f_mkdir_helper(const TCHAR *path);
// FRESULT f_unlink_helper(const TCHAR *path);
// FRESULT f_rename_helper(const TCHAR *path_old, const TCHAR *path_new);

int ff_unsupported_format(FIL *fp);
int ff_file_corrupted(FIL *fp);
int ff_not_equal(FIL *fp);
int ff_no_intersection(FIL *fp);
int file_read_open(FIL *fp, const char *path);
int file_write_open(FIL *fp, const char *path);
int file_close(FIL *fp);
int file_seek(FIL *fp, size_t offset);
int file_truncate(FIL *fp);
int file_sync(FIL *fp);
long file_fsize(FIL *fp);
// File buffer functions.
int file_buffer_init0();
int file_buffer_on(FIL *fp); // does fb_alloc_all
uint32_t file_tell_w_buf(FIL *fp); // valid between on and off
uint32_t file_size_w_buf(FIL *fp); // valid between on and off
int file_buffer_off(FIL *fp); // does fb_free
int read_byte(FIL *fp, uint8_t *value);
int read_byte_expect(FIL *fp, uint8_t value);
int read_byte_ignore(FIL *fp);
int read_word(FIL *fp, uint16_t *value);
int read_word_expect(FIL *fp, uint16_t value);
int read_word_ignore(FIL *fp);
int read_long(FIL *fp, uint32_t *value);
int read_long_expect(FIL *fp, uint32_t value);
int read_long_ignore(FIL *fp);
int read_data(FIL *fp, void *data, size_t size);
int write_byte(FIL *fp, uint8_t value);
int write_word(FIL *fp, uint16_t value);
int write_long(FIL *fp, uint32_t value);
int write_data(FIL *fp, const void *data, size_t size);
#ifdef __cplusplus
}
#endif
#endif /* __FF_WRAPPER_H__ */

