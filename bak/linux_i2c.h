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

#ifndef __LIBI2C_H_
#define __LIBI2C_H_

/**
 * @file libi2c/i2c.h
 * @brief I2C Helper Library API
 * @author Mark Greer <mgreer@animalcreek.com>
 *
 * Set of routines to provide easy-to-use ways to perform common Linux(tm)
 * I2C operations.
 * 
 * No delay !
 */

#include <unistd.h>
#include <stdint.h>
#if __cplusplus
extern "C"
{
#endif
int linux_i2c_read(void *i2c_handle, void *offset, size_t offset_len, void *buf,
	     size_t buf_len);
int linux_i2c_write(void *i2c_handle, void *offset, size_t offset_len, void *buf,
	      size_t buf_len);
int linux_i2c_init(uint8_t i2c_id, uint16_t i2c_addr, void **i2c_handlep);
int linux_i2c_destroy(void *i2c_handle);
#if __cplusplus
}
#endif
#endif