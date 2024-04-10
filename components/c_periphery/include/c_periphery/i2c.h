/*
 * c-periphery
 * https://github.com/vsergeev/c-periphery
 * License: MIT
 */

#ifndef _PERIPHERY_I2C_H
#define _PERIPHERY_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

enum i2c_error_code {
    I2C_ERROR_ARG               = -1, /* Invalid arguments */
    I2C_ERROR_OPEN              = -2, /* Opening I2C device */
    I2C_ERROR_QUERY             = -3, /* Querying I2C device attributes */
    I2C_ERROR_NOT_SUPPORTED     = -4, /* I2C not supported on this device */
    I2C_ERROR_TRANSFER          = -5, /* I2C transfer */
    I2C_ERROR_CLOSE             = -6, /* Closing I2C device */
};

typedef struct i2c_handle i2c_t;

/* Primary Functions */
i2c_t *i2c_new(void);
int i2c_open(i2c_t *i2c, const char *path);
int i2c_transfer(i2c_t *i2c, struct i2c_msg *msgs, size_t count);
int i2c_close(i2c_t *i2c);
void i2c_free(i2c_t *i2c);

/* Miscellaneous */
int i2c_fd(i2c_t *i2c);
int i2c_tostring(i2c_t *i2c, char *str, size_t len);

/* Error Handling */
int i2c_errno(i2c_t *i2c);
const char *i2c_errmsg(i2c_t *i2c);


/* I2C internal address max length */
#define INT_ADDR_MAX_BYTES 4

/* I2C page max bytes */
#define PAGE_MAX_BYTES 4096

#define GET_I2C_DELAY(delay) (delay)
#define GET_I2C_FLAGS(tenbit, flags) ((tenbit) ? ((flags) | I2C_M_TEN) : (flags))
#define GET_WRITE_SIZE(addr, remain, page_bytes) ((addr) + (remain) > (page_bytes) ? (page_bytes) - (addr) : remain)

void i2c_slave_address(i2c_t *i2c, unsigned short addr);

/* I2c ioctl read, write can set i2c flags */
int i2c_ioctl_read(i2c_t *i2c, unsigned int iaddr, void *buf, size_t len);
int i2c_ioctl_write(i2c_t *i2c, unsigned int iaddr, const void *buf, size_t len);

/* I2C file I/O read, write */
int i2c_read(i2c_t *i2c, unsigned int iaddr, void *buf, size_t len);
int i2c_write(i2c_t *i2c, unsigned int iaddr, const void *buf, size_t len);

/* I2c primitive read, write can set i2c flags */
int i2c_primitive_read(i2c_t *i2c, void *offset, size_t offset_len, void *buf, size_t buf_len);
int i2c_primitive_write(i2c_t *i2c, void *offset, size_t offset_len, void *buf, size_t buf_len);

/* struct i2c_msg from <linux/i2c.h>:

    struct i2c_msg {
    	__u16 addr;
    	__u16 flags;
    #define I2C_M_TEN		0x0010
    #define I2C_M_RD		0x0001
    #define I2C_M_STOP		0x8000
    #define I2C_M_NOSTART		0x4000
    #define I2C_M_REV_DIR_ADDR	0x2000
    #define I2C_M_IGNORE_NAK	0x1000
    #define I2C_M_NO_RD_ACK		0x0800
    #define I2C_M_RECV_LEN		0x0400
    	__u16 len;
    	__u8 *buf;
    };
*/

#ifdef __cplusplus
}
#endif

#endif

