/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * File System Helper Functions
 *
 */
#include "imlib_config.h"



#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
#include <stdio.h>
#include <unistd.h>
#include <string.h>
// #include "py/runtime.h"
// #include "extmod/vfs.h"
// #include "extmod/vfs_fat.h"

#include "common.h"
#include "fb_alloc.h"
#include "ff_wrapper.h"
#define FF_MIN(x,y) (((x)<(y))?(x):(y))

#if 0

static void ff_fail(FIL *fp, uint32_t res)
{
    if (fp) fclose(fp);
    // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT(ffs_strerror(res)));
}

static void ff_read_fail(FIL *fp)
{
    if (fp) fclose(fp);
    // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to read requested bytes!"));
}

static void ff_write_fail(FIL *fp)
{
    if (fp) fclose(fp);
    // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to write requested bytes!"));
}

 static void ff_expect_fail(FIL *fp)
{
    if (fp) fclose(fp);
    // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Unexpected value read!"));
}

 void ff_unsupported_format(FIL *fp)
{
    if (fp) fclose(fp);
    // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Unsupported format!"));
}

 void ff_file_corrupted(FIL *fp)
{
    if (fp) fclose(fp);
    // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("File corrupted!"));
}

 void ff_not_equal(FIL *fp)
{
    if (fp) fclose(fp);
    // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Images not equal!"));
}

 void ff_no_intersection(FIL *fp)
{
    if (fp) fclose(fp);
    // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("No intersection!"));
}

void file_read_open(FIL *fp, const char *path)
{
    *fp = fopen(path, "rb");
    if(NULL == *fp) ff_fail(fp, 0);
    // FRESULT res = f_open_helper(fp, path, FA_READ|FA_OPEN_EXISTING);
    // if (res != FR_OK) ff_fail(fp, res);
}

void file_write_open(FIL *fp, const char *path)
{
    *fp = fopen(path, "wb");
    if(NULL == *fp) ff_fail(fp, 0);
    // FRESULT res = f_open_helper(fp, path, FA_WRITE|FA_CREATE_ALWAYS);
    // if (res != FR_OK) ff_fail(fp, res);
}

void file_close(FIL *fp)
{
    if(NULL != *fp)
    {
        fclose(*fp);
    }
    // FRESULT res = f_close(fp);
    // if (res != FR_OK) ff_fail(fp, res);
}

void file_seek(FIL *fp, size_t offset)
{
    fseek(*fp, offset, SEEK_SET);
    // FRESULT res = f_lseek(fp, offset);
    // if (res != FR_OK) ff_fail(fp, res);
}

void file_truncate(FIL *fp)
{
    // FRESULT res = f_truncate(fp);
    // if (res != FR_OK) ff_fail(fp, res);
}

void file_sync(FIL *fp)
{
    int fd = fileno(*fp); 
    fsync(fd);
    // FRESULT res = f_sync(fp);
    // if (res != FR_OK) ff_fail(fp, res);
}

// These wrapper functions are used for backward compatibility with
// OpenMV code using vanilla FatFS. Note: Extracted from cc3200 ftp.c

// FATFS *lookup_path(const TCHAR **path) {
    // mp_vfs_mount_t *fs = mp_vfs_lookup_path(*path, path);
    // if (fs == MP_VFS_NONE || fs == MP_VFS_ROOT) {
    //     return NULL;
    // }
    // // here we assume that the mounted device is FATFS
    // return &((fs_user_mount_t*)MP_OBJ_TO_PTR(fs->obj))->fatfs;
}

// FRESULT f_open_helper(FIL *fp, const TCHAR *path, BYTE mode) {
//     FATFS *fs = lookup_path(&path);
//     if (fs == NULL) {
//         return FR_NO_PATH;
//     }
//     return f_open(fs, fp, path, mode);
// }

// FRESULT f_opendir_helper(FF_DIR *dp, const TCHAR *path) {
//     FATFS *fs = lookup_path(&path);
//     if (fs == NULL) {
//         return FR_NO_PATH;
//     }
//     return f_opendir(fs, dp, path);
// }

// FRESULT f_stat_helper(const TCHAR *path, FILINFO *fno) {
//     FATFS *fs = lookup_path(&path);
//     if (fs == NULL) {
//         return FR_NO_PATH;
//     }
//     return f_stat(fs, path, fno);
// }

// FRESULT f_mkdir_helper(const TCHAR *path) {
//     FATFS *fs = lookup_path(&path);
//     if (fs == NULL) {
//         return FR_NO_PATH;
//     }
//     return f_mkdir(fs, path);
// }

// FRESULT f_unlink_helper(const TCHAR *path) {
//     FATFS *fs = lookup_path(&path);
//     if (fs == NULL) {
//         return FR_NO_PATH;
//     }
//     return f_unlink(fs, path);
// }

// FRESULT f_rename_helper(const TCHAR *path_old, const TCHAR *path_new) {
//     FATFS *fs_old = lookup_path(&path_old);
//     if (fs_old == NULL) {
//         return FR_NO_PATH;
//     }
//     FATFS *fs_new = lookup_path(&path_new);
//     if (fs_new == NULL) {
//         return FR_NO_PATH;
//     }
//     if (fs_old != fs_new) {
//         return FR_NO_PATH;
//     }
//     return f_rename(fs_new, path_old, path_new);
// }
// When a sector boundary is encountered while writing a file and there are
// more than 512 bytes left to write FatFs will detect that it can bypass
// its internal write buffer and pass the data buffer passed to it directly
// to the disk write function. However, the disk write function needs the
// buffer to be aligned to a 4-byte boundary. FatFs doesn't know this and
// will pass an unaligned buffer if we don't fix the issue. To fix this problem
// we use a temporary buffer to fix the alignment and to speed everything up.

// We use this temporary buffer for both reads and writes. The buffer allows us
// to do multi-block reads and writes which signifcantly speed things up.

static uint32_t file_buffer_offset = 0;
static uint8_t *file_buffer_pointer = 0;
static uint32_t file_buffer_size = 0;
static uint32_t file_buffer_index = 0;

void file_buffer_init0()
{
    file_buffer_offset = 0;
    file_buffer_pointer = 0;
    file_buffer_size = 0;
    file_buffer_index = 0;
}

OMV_ATTR_ALWAYS_INLINE static void file_fill(FIL *fp)
{
    if (file_buffer_index == file_buffer_size) {
        file_buffer_pointer -= file_buffer_offset;
        file_buffer_size += file_buffer_offset;
        file_buffer_offset = 0;
        file_buffer_index = 0;
        uint32_t file_remaining = fsize(*fp) - ftell(*fp);
        uint32_t can_do = FF_MIN(file_buffer_size, file_remaining);
        uint8_t bytes;
        uint32_t res = fread(*fp, file_buffer_pointer, can_do, &bytes);
        // FRESULT res = fread(*fp, file_buffer_pointer, can_do, &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        // if (bytes != can_do) ff_read_fail(fp);
    }
}

OMV_ATTR_ALWAYS_INLINE static void file_flush(FIL *fp)
{
    if (file_buffer_index == file_buffer_size) {
        uint8_t bytes;
        uint32_t res = fwrite(*fp, file_buffer_pointer, file_buffer_index, &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        if (bytes != file_buffer_index) ff_write_fail(*fp);
        file_buffer_pointer -= file_buffer_offset;
        file_buffer_size += file_buffer_offset;
        file_buffer_offset = 0;
        file_buffer_index = 0;
    }
}

uint32_t file_tell_w_buf(FIL *fp)
{
    if (fp->flag & FA_READ) {
        return ftell(*fp) - file_buffer_size + file_buffer_index;
    } else {
        return ftell(*fp) + file_buffer_index;
    }
}

uint32_t file_size_w_buf(FIL *fp)
{
    if (fp->flag & FA_READ) {
        return fsize(*fp);
    } else {
        return fsize(*fp) + file_buffer_index;
    }
}

void file_buffer_on(FIL *fp)
{
    file_buffer_offset = ftell(*fp) % 4;
    file_buffer_pointer = fb_alloc_all(&file_buffer_size, FB_ALLOC_PREFER_SIZE) + file_buffer_offset;
    if (!file_buffer_size) {
        // mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("No memory!"));
    }
    file_buffer_size -= file_buffer_offset;
    file_buffer_index = 0;
    if (fp->flag & FA_READ) {
        uint32_t file_remaining = fsize(*fp) - ftell(*fp);
        uint32_t can_do = FF_MIN(file_buffer_size, file_remaining);
        uint8_t bytes;
        uint32_t res = fread(*fp, file_buffer_pointer, can_do, &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        // if (bytes != can_do) ff_read_fail(fp);
    }
}

void file_buffer_off(FIL *fp)
{
    if ((fp->flag & FA_WRITE) && file_buffer_index) {
        uint8_t bytes;
        uint32_t res = fwrite(*fp, file_buffer_pointer, file_buffer_index, &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        // if (bytes != file_buffer_index) ff_write_fail(fp);
    }
    file_buffer_pointer = 0;
    fb_free(NULL);
}

void read_byte(FIL *fp, uint8_t *value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(*value); i++) {
            file_fill(fp);
            ((uint8_t *) value)[i] = file_buffer_pointer[file_buffer_index++];
        }
    } else {
        uint8_t bytes;
        uint32_t res = fread(*fp, value, sizeof(*value), &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        // if (bytes != sizeof(*value)) ff_read_fail(fp);
    }
}

void read_byte_expect(FIL *fp, uint8_t value)
{
    uint8_t compare;
    read_byte(fp, &compare);
    if (value != compare) ff_expect_fail(fp);
}

void read_byte_ignore(FIL *fp)
{
    uint8_t trash;
    read_byte(fp, &trash);
}

void read_word(FIL *fp, uint16_t *value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(*value); i++) {
            file_fill(fp);
            ((uint8_t *) value)[i] = file_buffer_pointer[file_buffer_index++];
        }
    } else {
        uint8_t bytes;
        uint32_t res = fread(*fp, value, sizeof(*value), &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        // if (bytes != sizeof(*value)) ff_read_fail(fp);
    }
}

void read_word_expect(FIL *fp, uint16_t value)
{
    uint16_t compare;
    read_word(fp, &compare);
    if (value != compare) ff_expect_fail(fp);
}

void read_word_ignore(FIL *fp)
{
    uint16_t trash;
    read_word(fp, &trash);
}

void read_long(FIL *fp, uint32_t *value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(*value); i++) {
            file_fill(fp);
            ((uint8_t *) value)[i] = file_buffer_pointer[file_buffer_index++];
        }
    } else {
        uint8_t bytes;
        uint32_t res = fread(*fp, value, sizeof(*value), &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        // if (bytes != sizeof(*value)) ff_read_fail(fp);
    }
}

void read_long_expect(FIL *fp, uint32_t value)
{
    uint32_t compare;
    read_long(fp, &compare);
    if (value != compare) ff_expect_fail(fp);
}

void read_long_ignore(FIL *fp)
{
    uint32_t trash;
    read_long(fp, &trash);
}

void read_data(FIL *fp, void *data, size_t size)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        while (size) {
            file_fill(fp);
            uint32_t file_buffer_space_left = file_buffer_size - file_buffer_index;
            uint32_t can_do = FF_MIN(size, file_buffer_space_left);
            memcpy(data, file_buffer_pointer+file_buffer_index, can_do);
            file_buffer_index += can_do;
            data += can_do;
            size -= can_do;
        }
    } else {
        uint8_t bytes;
        uint32_t res = fread(*fp, data, size, &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        if (bytes != size) ff_read_fail(fp);
    }
}

void write_byte(FIL *fp, uint8_t value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(value); i++) {
            file_buffer_pointer[file_buffer_index++] = ((uint8_t *) &value)[i];
            file_flush(fp);
        }
    } else {
        uint8_t bytes;
        uint32_t res = fwrite(*fp, &value, sizeof(value), &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        if (bytes != sizeof(value)) ff_write_fail(fp);
    }
}

void write_word(FIL *fp, uint16_t value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(value); i++) {
            file_buffer_pointer[file_buffer_index++] = ((uint8_t *) &value)[i];
            file_flush(fp);
        }
    } else {
        uint8_t bytes;
        uint32_t res = fwrite(*fp, &value, sizeof(value), &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        if (bytes != sizeof(value)) ff_write_fail(fp);
    }
}

void write_long(FIL *fp, uint32_t value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(value); i++) {
            file_buffer_pointer[file_buffer_index++] = ((uint8_t *) &value)[i];
            file_flush(fp);
        }
    } else {
        uint8_t bytes;
        uint32_t res = fwrite(*fp, &value, sizeof(value), &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        if (bytes != sizeof(value)) ff_write_fail(fp);
    }
}

void write_data(FIL *fp, const void *data, size_t size)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        while (size) {
            uint32_t file_buffer_space_left = file_buffer_size - file_buffer_index;
            uint32_t can_do = FF_MIN(size, file_buffer_space_left);
            memcpy(file_buffer_pointer+file_buffer_index, data, can_do);
            file_buffer_index += can_do;
            data += can_do;
            size -= can_do;
            file_flush(fp);
        }
    } else {
        uint8_t bytes;
        uint32_t res = fwrite(*fp, data, size, &bytes);
        // if (res != FR_OK) ff_fail(fp, res);
        if (bytes != size) ff_write_fail(fp);
    }
}


#else
#include "imlib_io.h"

int ff_unsupported_format(FIL *fp)
{
    if (*fp) fclose(*fp);
    *fp = NULL;
    ERR_PRINT("ff_unsupported_format");
    return 0;
}

int ff_file_corrupted(FIL *fp)
{
    if (*fp) fclose(*fp);
    *fp = NULL;
    ERR_PRINT("ff_file_corrupted!\n");
    return 0;
}
int ff_not_equal(FIL *fp)
{
    if (*fp) fclose(*fp);
    *fp = NULL;
    ERR_PRINT("ff_not_equal!\n");
    return 0;
}

int ff_no_intersection(FIL *fp)
{
    if (*fp) fclose(*fp);
    *fp = NULL;
    ERR_PRINT("ff_no_intersection!\n");
    return 0;
}
int file_read_open(FIL *fp, const char *path)
{
    *fp = fopen(path, "rb");
    if(NULL == *fp)
    {
        LOG_PRINT("%s,not exit!", path);
    }
    return 0;
}
int file_write_open(FIL *fp, const char *path)
{
    *fp = fopen(path, "wb");
    return 0;
}

int file_close(FIL *fp)
{
    fclose(*fp);
    return 0;
}
int file_seek(FIL *fp, size_t offset)
{
    fseek(*fp, offset, SEEK_SET);
    return 0;
}
int file_truncate(FIL *fp)
{
    return -1;
}
int file_sync(FIL *fp)
{
    int fd = fileno(*fp); 
    fsync(fd);
    return 0;
}

long file_fsize(FIL *fp)
{
    long n;
    fpos_t fpos;        //当前位置
    fgetpos(*fp, &fpos); //获取当前位置
    fseek(*fp, 0, SEEK_END);
    n = ftell(*fp);
    fsetpos(*fp, &fpos); //恢复之前的位置
    return n;
}

static uint32_t file_buffer_offset = 0;
static uint8_t *file_buffer_pointer = 0;
static uint32_t file_buffer_size = 0;
static uint32_t file_buffer_index = 0;

int file_buffer_init0()
{
    file_buffer_offset = 0;
    file_buffer_pointer = 0;
    file_buffer_size = 0;
    file_buffer_index = 0;
    return 0;
}






int file_buffer_on(FIL *fp) // does fb_alloc_all
{
    file_buffer_offset = ftell(*fp) % 4;
    file_buffer_pointer = fb_alloc_all(&file_buffer_size, FB_ALLOC_PREFER_SIZE) + file_buffer_offset;
    if (!file_buffer_size) {
        // mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("No memory!"));
        ERR_PRINT("MemoryError: No memory!");
        // printf("%s:%d,No memory!\n", __FUNCTION__, __LINE__);
    }
    file_buffer_size -= file_buffer_offset;
    file_buffer_index = 0;
    // if (fp->flag & FA_READ) {
    //     uint32_t file_remaining = fsize(*fp) - ftell(*fp);
    //     uint32_t can_do = FF_MIN(file_buffer_size, file_remaining);
    //     uint8_t bytes;
    //     uint32_t res = fread(*fp, file_buffer_pointer, can_do, &bytes);
    //     // if (res != FR_OK) ff_fail(fp, res);
    //     // if (bytes != can_do) ff_read_fail(fp);
    // }
    return 0;
}
uint32_t file_tell_w_buf(FIL *fp) // valid between on and off
{
    return -1;
}
uint32_t file_size_w_buf(FIL *fp) // valid between on and off
{
    return -1;
}
int file_buffer_off(FIL *fp) // does fb_free
{
    return -1;
}
int read_byte(FIL *fp, uint8_t *value)
{
    int num;
    num = fread(value, 1, 1, *fp);
    if(num != 1)
    {
        ERR_PRINT("file error!\n");
    }
    return num;
}
int read_byte_expect(FIL *fp, uint8_t value)
{
    int num;
    uint8_t str_s;
    num = fread(&str_s, 1, 1, *fp);
    if(str_s != value)
    {
        ERR_PRINT("file error!\n");
        return -1;
    }
    return num;
}
int read_byte_ignore(FIL *fp)
{
    int num;
    uint32_t str_s;
    num = fread(&str_s, 1, 1, *fp);
    if(num != 1)
    {
        ERR_PRINT("file error!\n");
    }
    return num;
}
int read_word(FIL *fp, uint16_t *value)
{
    int num;
    num = fread(value, 2, 1, *fp);
    if(num != 1)
    {
        ERR_PRINT("file error!\n");
    }
    return num;
}
int read_word_expect(FIL *fp, uint16_t value)
{
    int num;
    uint16_t str_s;
    num = fread(&str_s, 2, 1, *fp);
    if(str_s != value)
    {
        ERR_PRINT("file error!\n");
        return -1;
    }
    return num;
}
int read_word_ignore(FIL *fp)
{
    int num;
    uint16_t str_s;
    num = fread(&str_s, 2, 1, *fp);
    if(num != 1)
    {
        ERR_PRINT("file error!\n");
        return -1;
    }
    return num;
}
int read_long(FIL *fp, uint32_t *value)
{
    int num;
    num = fread(value, 4, 1, *fp);
    if(num != 1)
    {
        ERR_PRINT("file error!\n");
        return -1;
    }
    return num;
}
int read_long_expect(FIL *fp, uint32_t value)
{
    int num;
    uint32_t str_s;
    num = fread(&str_s, 4, 1, *fp);
    if(str_s != value)
    {
        ERR_PRINT("file error!\n");
        return -1;
    }
    return num;
}
int read_long_ignore(FIL *fp)
{
    uint32_t str_s;
    return fread(&str_s, 4, 1, *fp);
}
int read_data(FIL *fp, void *data, size_t size)
{
    return fread(data, 1, size, *fp);

}


int write_byte(FIL *fp, uint8_t value)
{
    return fwrite(&value, 1, 1, *fp);
}
int write_word(FIL *fp, uint16_t value)
{
    return fwrite(&value, 2, 1, *fp);
}
int write_long(FIL *fp, uint32_t value)
{
    return  fwrite(&value, 4, 1, *fp);
}
int write_data(FIL *fp, const void *data, size_t size)
{
    return fwrite(data, 1, size, *fp);
}



#endif

#endif //IMLIB_ENABLE_IMAGE_FILE_IO