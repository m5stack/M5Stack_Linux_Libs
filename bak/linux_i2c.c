/*
 * Copyright (c) 2015-2017 Animal Creek Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file libi2c/i2c.c
 * @brief I2C Helper Library
 * @author Mark Greer <mgreer@animalcreek.com>
 *
 * Set of routines to provide easy-to-use ways to perform common Linux(tm)
 * I2C operations.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "linux_i2c.h"

/**
 * @cond
 */

#define I2C_FILENAME_PREFIX	"/dev/i2c-"
#define I2C_INDEX_ID_MAX	3 /* 8 bits can produce 3 decimal digits */
#define I2C_FILENAME_MAX	(sizeof(I2C_FILENAME_PREFIX) + \
				 I2C_INDEX_ID_MAX)

struct i2c_info {
	int		fd;
	uint16_t	addr;
};

static int i2c_rw(void *i2c_handle, void *offset, size_t offset_len, void *buf,
		  size_t buf_len, int write_flag)
{
	struct i2c_info *iip = (struct i2c_info *)i2c_handle; struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data ioctl_data;
	unsigned int idx, count;
	int ret;

	if (!iip)
		return -EINVAL;

	idx = 0;
	count = 0;

	/* Send the device's register/memory address to be read/written */
	if (offset_len) {
		msgs[0].addr = iip->addr;
		msgs[0].flags = 0; /* write */
		msgs[0].len = offset_len;
		msgs[0].buf = (unsigned char*)offset;
		idx++;
		count++;
	}

	/* Read/write from/to the device's register/memory */
	if (buf_len) {
		msgs[idx].addr = iip->addr;
		msgs[idx].flags = write_flag ? 0 : I2C_M_RD;
		msgs[idx].len = buf_len;
		msgs[idx].buf = (unsigned char*)buf;
		count++;
	}

	if (count) {
		ioctl_data.msgs = msgs;
		ioctl_data.nmsgs = count;

		ret = ioctl(iip->fd, I2C_RDWR, &ioctl_data);
		if (ret < 0)
			return -errno;
	}

	return 0;
}

/**
 * @endcond
 */

/**
 * @brief Read specified bytes number of bytes from I2C device
 * @param [in] i2c_handle Handle returned by i2c_init()
 * @param [in] offset Offset within I2C device to start reading
 * @param [in] offset_len Length in bytes of 'offset' parameter
 * @param [out] buf Address of buffer where data will be placed
 * @param [in] buf_len Length in bytes of the buffer pointed to by 'buf'
 * @return 0 on success, negative errno on failure
 */
int linux_i2c_read(void *i2c_handle, void *offset, size_t offset_len, void *buf,
	     size_t buf_len)
{
	return i2c_rw(i2c_handle, offset, offset_len, buf, buf_len, 0);
}

/**
 * @brief Write specified number of bytes to I2C device
 * @param [in] i2c_handle Handle returned by i2c_init()
 * @param [in] offset Offset within I2C device to start writing
 * @param [in] offset_len Length in bytes of 'offset' parameter
 * @param [in] buf Address of buffer containing data to write
 * @param [in] buf_len Length in bytes of the data pointed to by 'buf'
 * @return 0 on success, negative errno on failure
 */
int linux_i2c_write(void *i2c_handle, void *offset, size_t offset_len, void *buf,
	      size_t buf_len)
{
	return i2c_rw(i2c_handle, offset, offset_len, buf, buf_len, 1);
}

/**
 * @brief Initialize a handle that provides access to specified I2C device
 * @param [in] i2c_id Identifier for I2C bus (e.g., '0' for /dev/i2c-0)
 * @param [in] i2c_addr I2C bus address of I2C device
 * @param [out] i2c_handlep Pointer to handle to use for I2C operations
 * @return 0 on success, negative errno on failure
 */
int linux_i2c_init(uint8_t i2c_id, uint16_t i2c_addr, void **i2c_handlep)
{
	struct i2c_info *iip;
	char filename[I2C_FILENAME_MAX];
	unsigned long funcs;
	int ret;

	if (!i2c_handlep)
		return -EINVAL;

	iip = (struct i2c_info *)malloc(sizeof(*iip));
	if (!iip)
		return -ENOMEM;

	ret = snprintf(filename, I2C_FILENAME_MAX, I2C_FILENAME_PREFIX "%hhu",
		       i2c_id);
	if (ret < 0) {
		ret = -EIO;
		goto err_free_iip;
	} else if (ret >= (int)I2C_FILENAME_MAX) {
		ret = -EINVAL;
		goto err_free_iip;
	}

	ret = open(filename, O_RDWR);
	if (ret < 0) {
		ret = -errno;
		goto err_free_iip;
	}

	iip->fd = ret;

	ret = ioctl(iip->fd, I2C_FUNCS, &funcs);
	if (ret < 0) {
		ret = -errno;
		goto err_close;
	}

	/* I2C driver must support I2C_RDWR ioctl */
	if (!(funcs & I2C_FUNC_I2C)) {
		ret = -ENOSYS;
		goto err_close;
	}

	iip->addr = i2c_addr;

	*i2c_handlep = iip;

	return 0;

err_close:
	close(iip->fd);
err_free_iip:
	free(iip);

	return ret;
}

/**
 * @brief Destroy resource allocated by i2c_init()
 * @param [in] i2c_handle Handle returned by i2c_init()
 * @return 0 on success, negative errno on failure
 */
int linux_i2c_destroy(void *i2c_handle)
{
	struct i2c_info *iip = (struct i2c_info *)i2c_handle;

	if (!iip)
		return -EINVAL;

	close(iip->fd);
	free(iip);

	return 0;
}