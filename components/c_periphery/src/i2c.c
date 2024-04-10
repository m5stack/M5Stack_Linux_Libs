/*
 * c-periphery
 * https://github.com/vsergeev/c-periphery
 * License: MIT
 */

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>

#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c.h"

struct i2c_handle {
    int fd;
    unsigned short addr;		/* I2C i2c(slave) address */
    unsigned char tenbit;		/* I2C is 10 bit device address */
    unsigned char delay;		/* I2C internal operation delay, unit millisecond */
    unsigned short flags;		/* I2C i2c_ioctl_read/write flags */
    unsigned int page_bytes;    /* I2C max number of bytes per page, 1K/2K 8, 4K/8K/16K 16, 32K/64K 32 etc */
    unsigned int iaddr_bytes;   /* I2C device internal(word) address bytes, such as: 24C04 1 byte, 24C64 2 bytes */
    struct {
        int c_errno;
        char errmsg[96];
    } error;
};
void i2c_slave_address(i2c_t *i2c, unsigned short addr)
{
    i2c->addr = addr;
}
static int _i2c_error(i2c_t *i2c, int code, int c_errno, const char *fmt, ...) {
    va_list ap;

    i2c->error.c_errno = c_errno;

    va_start(ap, fmt);
    vsnprintf(i2c->error.errmsg, sizeof(i2c->error.errmsg), fmt, ap);
    va_end(ap);

    /* Tack on strerror() and errno */
    if (c_errno) {
        char buf[64];
        strerror_r(c_errno, buf, sizeof(buf));
        snprintf(i2c->error.errmsg+strlen(i2c->error.errmsg), sizeof(i2c->error.errmsg)-strlen(i2c->error.errmsg), ": %s [errno %d]", buf, c_errno);
    }

    return code;
}

i2c_t *i2c_new(void) {
    i2c_t *i2c = calloc(1, sizeof(i2c_t));
    if (i2c == NULL)
        return NULL;

    i2c->fd = -1;

    return i2c;
}
void _i2c_init_args(i2c_t *i2c) {
    /* 7 bit device address */
    i2c->tenbit = 0;

    /* no delay */
    i2c->delay = 0;

    /* 8 bytes per page */
    i2c->page_bytes = 8;

    /* 1 byte internal(word) address */
    i2c->iaddr_bytes = 1;
}
void i2c_free(i2c_t *i2c) {
    if(i2c->fd != -1)
    {
        i2c_close(i2c);
    }
    free(i2c);
}

int i2c_open(i2c_t *i2c, const char *path) {
    unsigned long supported_funcs;

    memset(i2c, 0, sizeof(i2c_t));
    _i2c_init_args(i2c);
    /* Open device */
    if ((i2c->fd = open(path, O_RDWR)) < 0)
        return _i2c_error(i2c, I2C_ERROR_OPEN, errno, "Opening I2C device \"%s\"", path);

    /* Query supported functions */
    if (ioctl(i2c->fd, I2C_FUNCS, &supported_funcs) < 0) {
        int errsv = errno;
        close(i2c->fd);
        i2c->fd = -1;
        return _i2c_error(i2c, I2C_ERROR_QUERY, errsv, "Querying I2C functions");
    }

    if (!(supported_funcs & I2C_FUNC_I2C)) {
        close(i2c->fd);
        i2c->fd = -1;
        return _i2c_error(i2c, I2C_ERROR_NOT_SUPPORTED, 0, "I2C not supported on %s", path);
    }

    return 0;
}

int i2c_transfer(i2c_t *i2c, struct i2c_msg *msgs, size_t count) {
    struct i2c_rdwr_ioctl_data i2c_rdwr_data;

    /* Prepare I2C transfer structure */
    memset(&i2c_rdwr_data, 0, sizeof(struct i2c_rdwr_ioctl_data));
    i2c_rdwr_data.msgs = msgs;
    i2c_rdwr_data.nmsgs = count;

    /* Transfer */
    if (ioctl(i2c->fd, I2C_RDWR, &i2c_rdwr_data) < 0)
        return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");

    return 0;
}

int i2c_close(i2c_t *i2c) {
    if (i2c->fd < 0)
        return 0;

    /* Close fd */
    if (close(i2c->fd) < 0)
        return _i2c_error(i2c, I2C_ERROR_CLOSE, errno, "Closing I2C device");

    i2c->fd = -1;

    return 0;
}

int i2c_tostring(i2c_t *i2c, char *str, size_t len) {
    return snprintf(str, len, "I2C (fd=%d)", i2c->fd);
}

const char *i2c_errmsg(i2c_t *i2c) {
    return i2c->error.errmsg;
}

int i2c_errno(i2c_t *i2c) {
    return i2c->error.c_errno;
}

int i2c_fd(i2c_t *i2c) {
    return i2c->fd;
}

static void _i2c_iaddr_convert(unsigned int iaddr, unsigned int len, unsigned char *addr)
{
    union {
        unsigned int iaddr;
        unsigned char caddr[INT_ADDR_MAX_BYTES];
    } convert;

    /* I2C internal address order is big-endian, same with network order */
    convert.iaddr = htonl(iaddr);

    /* Copy address to addr buffer */
    int i = len - 1;
    int j = INT_ADDR_MAX_BYTES - 1;

    while (i >= 0 && j >= 0) {

        addr[i--] = convert.caddr[j--];
    }
}

static int _i2c_select(int bus, unsigned long dev_addr, unsigned long tenbit)
{
    /* Set i2c device address bit */
    if (ioctl(bus, I2C_TENBIT, tenbit)) {

        perror("Set I2C_TENBIT failed");
        return -1;
    }

    /* Set i2c device as slave ans set it address */
    if (ioctl(bus, I2C_SLAVE, dev_addr)) {

        perror("Set i2c device address failed");
        return -1;
    }

    return 0;
}

static int _i2c_rw(i2c_t *i2c, void *offset, size_t offset_len, void *buf,
		  size_t buf_len, int write_flag)
{
    struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data ioctl_data;
	unsigned int idx, count;
	int ret;

	if (!i2c)
		return -EINVAL;

	idx = 0;
	count = 0;

	/* Send the device's register/memory address to be read/written */
	if (offset_len) {
		msgs[0].addr = i2c->addr;
		msgs[0].flags = 0; /* write */
		msgs[0].len = offset_len;
		msgs[0].buf = (unsigned char*)offset;
		idx++;
		count++;
	}

	/* Read/write from/to the device's register/memory */
	if (buf_len) {
		msgs[idx].addr = i2c->addr;
		msgs[idx].flags = write_flag ? 0 : I2C_M_RD;
		msgs[idx].len = buf_len;
		msgs[idx].buf = (unsigned char*)buf;
		count++;
	}

	if (count) {
		ioctl_data.msgs = msgs;
		ioctl_data.nmsgs = count;

		ret = ioctl(i2c->fd, I2C_RDWR, &ioctl_data);
		if (ret < 0)
			return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");
	}

	return 0;
}


int i2c_ioctl_read(i2c_t *i2c, unsigned int iaddr, void *buf, size_t len)
{
    struct i2c_msg ioctl_msg[2];
    struct i2c_rdwr_ioctl_data ioctl_data;
    unsigned char addr[INT_ADDR_MAX_BYTES];
    unsigned short flags = GET_I2C_FLAGS(i2c->tenbit, i2c->flags);

    memset(addr, 0, sizeof(addr));
    memset(ioctl_msg, 0, sizeof(ioctl_msg));
    memset(&ioctl_data, 0, sizeof(ioctl_data));

    /* Target have internal address */
    if (i2c->iaddr_bytes) {

        _i2c_iaddr_convert(iaddr, i2c->iaddr_bytes, addr);

        /* First message is write internal address */
        ioctl_msg[0].len	=	i2c->iaddr_bytes;
        ioctl_msg[0].addr	= 	i2c->addr;
        ioctl_msg[0].buf	= 	addr;
        ioctl_msg[0].flags	= 	flags;

        /* Second message is read data */
        ioctl_msg[1].len	= 	len;
        ioctl_msg[1].addr	= 	i2c->addr;
        ioctl_msg[1].buf	=	buf;
        ioctl_msg[1].flags	=	flags | I2C_M_RD;

        /* Package to i2c message to operation i2c device */
        ioctl_data.nmsgs	=	2;
        ioctl_data.msgs		=	ioctl_msg;
    }
    /* Target did not have internal address */
    else {

        /* Direct send read data message */
        ioctl_msg[0].len	= 	len;
        ioctl_msg[0].addr	= 	i2c->addr;
        ioctl_msg[0].buf	=	buf;
        ioctl_msg[0].flags	=	flags | I2C_M_RD;

        /* Package to i2c message to operation i2c device */
        ioctl_data.nmsgs	=	1;
        ioctl_data.msgs		=	ioctl_msg;
    }

    /* Using ioctl interface operation i2c device */
    if (ioctl(i2c->fd, I2C_RDWR, (unsigned long)&ioctl_data) == -1) {

        return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");
    }

    return len;
}

int i2c_ioctl_write(i2c_t *i2c, unsigned int iaddr, const void *buf, size_t len)
{
    ssize_t remain = len;
    size_t size = 0, cnt = 0;
    const unsigned char *buffer = buf;
    // unsigned char delay = GET_I2C_DELAY(i2c->delay);
    unsigned short flags = GET_I2C_FLAGS(i2c->tenbit, i2c->flags);

    struct i2c_msg ioctl_msg;
    struct i2c_rdwr_ioctl_data ioctl_data;
    unsigned char tmp_buf[PAGE_MAX_BYTES + INT_ADDR_MAX_BYTES];

    while (remain > 0) {

        size = GET_WRITE_SIZE(iaddr % i2c->page_bytes, remain, i2c->page_bytes);

        /* Convert i2c internal address */
        memset(tmp_buf, 0, sizeof(tmp_buf));
        _i2c_iaddr_convert(iaddr, i2c->iaddr_bytes, tmp_buf);

        /* Connect write data after device internal address */
        memcpy(tmp_buf + i2c->iaddr_bytes, buffer, size);

        /* Fill kernel ioctl i2c_msg */
        memset(&ioctl_msg, 0, sizeof(ioctl_msg));
        memset(&ioctl_data, 0, sizeof(ioctl_data));

        ioctl_msg.len	=	i2c->iaddr_bytes + size;
        ioctl_msg.addr	=	i2c->addr;
        ioctl_msg.buf	=	tmp_buf;
        ioctl_msg.flags	=	flags;

        ioctl_data.nmsgs =	1;
        ioctl_data.msgs	=	&ioctl_msg;

        if (ioctl(i2c->fd, I2C_RDWR, (unsigned long)&ioctl_data) == -1) {

            return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");
        }

        cnt += size;
        iaddr += size;
        buffer += size;
        remain -= size;
    }

    return cnt;
}


int i2c_read(i2c_t *i2c, unsigned int iaddr, void *buf, size_t len)
{
    ssize_t cnt;
    unsigned char addr[INT_ADDR_MAX_BYTES];
    // unsigned char delay = GET_I2C_DELAY(i2c->delay);

    /* Set i2c slave address */
    if (_i2c_select(i2c->fd, i2c->addr, i2c->tenbit) == -1) {

        return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");
    }

    /* Convert i2c internal address */
    memset(addr, 0, sizeof(addr));
    _i2c_iaddr_convert(iaddr, i2c->iaddr_bytes, addr);

    /* Write internal address to devide  */
    if (write(i2c->fd, addr, i2c->iaddr_bytes) != i2c->iaddr_bytes) {

        return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");
    }

    /* Read count bytes data from int_addr specify address */
    if ((cnt = read(i2c->fd, buf, len)) == -1) {

        return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");
    }

    return cnt;
}

int i2c_write(i2c_t *i2c, unsigned int iaddr, const void *buf, size_t len)
{
    ssize_t remain = len;
    ssize_t ret;
    size_t cnt = 0, size = 0;
    const unsigned char *buffer = buf;
    // unsigned char delay = GET_I2C_DELAY(i2c->delay);
    unsigned char tmp_buf[PAGE_MAX_BYTES + INT_ADDR_MAX_BYTES];

    /* Set i2c slave address */
    if (_i2c_select(i2c->fd, i2c->addr, i2c->tenbit) == -1) {

        return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");
    }

    /* Once only can write less than 4 byte */
    while (remain > 0) {

        size = GET_WRITE_SIZE(iaddr % i2c->page_bytes, remain, i2c->page_bytes);

        /* Convert i2c internal address */
        memset(tmp_buf, 0, sizeof(tmp_buf));
        _i2c_iaddr_convert(iaddr, i2c->iaddr_bytes, tmp_buf);

        /* Copy data to tmp_buf */
        memcpy(tmp_buf + i2c->iaddr_bytes, buffer, size);

        /* Write to buf content to i2c device length  is address length and
                write buffer length */
        ret = write(i2c->fd, tmp_buf, i2c->iaddr_bytes + size);
        if (ret == -1 || (size_t)ret != i2c->iaddr_bytes + size)
        {
            return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");
        }

        /* Move to next #size bytes */
        cnt += size;
        iaddr += size;
        buffer += size;
        remain -= size;
    }

    return cnt;
}


int i2c_primitive_read(i2c_t *i2c, void *offset, size_t offset_len, void *buf, size_t buf_len)
{
    return _i2c_rw(i2c, offset, offset_len, buf, buf_len, 0);
}

int i2c_primitive_write(i2c_t *i2c, void *offset, size_t offset_len, void *buf, size_t buf_len)
{
    return _i2c_rw(i2c, offset, offset_len, buf, buf_len, 1);
}
