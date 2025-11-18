#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include "linuxi2c.h"

/* I2C default delay */
#define I2C_DEFAULT_DELAY 1

/* I2C internal address max length */
#define INT_ADDR_MAX_BYTES 4

/* I2C page max bytes */
#define PAGE_MAX_BYTES 4096

#define GET_I2C_DELAY(delay) (delay)
#define GET_I2C_FLAGS(tenbit, flags) ((tenbit) ? ((flags) | I2C_M_TEN) : (flags))
#define GET_WRITE_SIZE(addr, remain, page_bytes) ((addr) + (remain) > (page_bytes) ? (page_bytes) - (addr) : remain)

static void linuxi2c_delay(unsigned char delay);

/*
**	@brief		:	Open i2c bus
**	#bus_name	:	i2c bus name such as: /dev/i2c-1
**	@return		:	failed return -1, success return i2c bus fd
*/
int linuxi2c_open(const char *bus_name)
{
    int fd;
    unsigned long funcs;
    int ret;
    /* Open i2c-bus devcice */
    if ((fd = open(bus_name, O_RDWR)) == -1) {

        return -errno;
    }

	ret = ioctl(fd, I2C_FUNCS, &funcs);
	if (ret < 0) {
		ret = -errno;
        close(fd);
		return ret;
	}

	/* I2C driver must support I2C_RDWR ioctl */
	if (!(funcs & I2C_FUNC_I2C)) {
		ret = -ENOSYS;
		close(fd);
        return ret;
	}
    return fd;
}


void linuxi2c_close(int bus)
{
    close(bus);
}


/*
**	@brief		:	Initialize LINUXI2CDevice with defualt value
**	#device	    :	LINUXI2CDevice struct
*/
void linuxi2c_init_device(LINUXI2CDevice *device)
{
    /* 7 bit device address */
    device->tenbit = 0;

    /* no delay */
    device->delay = 0;

    /* 8 bytes per page */
    device->page_bytes = 32;

    /* 1 byte internal(word) address */
    device->iaddr_bytes = 1;
}


/*
**	@brief		:	Get LINUXI2CDevice struct desc
**	#device	    :	LINUXI2CDevice struct
**  #buf        :   Description message buffer
**  #size       :   #buf size
**	@return		:	return i2c device desc
*/
char *linuxi2c_get_device_desc(const LINUXI2CDevice *device, char *buf, size_t size)
{
    memset(buf, 0, size);
    snprintf(buf, size, "Device address: 0x%x, tenbit: %s, internal(word) address: %d bytes, page max %d bytes, delay: %dms",
             device->addr, device->tenbit ? "True" : "False", device->iaddr_bytes, device->page_bytes, device->delay);

    return buf;
}


/*
**	i2c_ioctl_read/write
**	I2C bus top layer interface to operation different i2c devide
**	This function will call XXX:ioctl system call and will be related
**	i2c-dev.c i2cdev_ioctl to operation i2c device.
**	1. it can choice ignore or not ignore i2c bus ack signal (flags set I2C_M_IGNORE_NAK)
**	2. it can choice ignore or not ignore i2c internal address
**
*/
ssize_t linuxi2c_ioctl_read(const LINUXI2CDevice *device, unsigned int iaddr, void *buf, size_t len)
{
    struct i2c_msg ioctl_msg[2];
    struct i2c_rdwr_ioctl_data ioctl_data;
    unsigned char addr[INT_ADDR_MAX_BYTES];
    unsigned short flags = GET_I2C_FLAGS(device->tenbit, device->flags);

    memset(addr, 0, sizeof(addr));
    memset(ioctl_msg, 0, sizeof(ioctl_msg));
    memset(&ioctl_data, 0, sizeof(ioctl_data));

    /* Target have internal address */
    if (device->iaddr_bytes) {

        linuxi2c_iaddr_convert(iaddr, device->iaddr_bytes, addr);

        /* First message is write internal address */
        ioctl_msg[0].len	=	device->iaddr_bytes;
        ioctl_msg[0].addr	= 	device->addr;
        ioctl_msg[0].buf	= 	addr;
        ioctl_msg[0].flags	= 	flags;

        /* Second message is read data */
        ioctl_msg[1].len	= 	len;
        ioctl_msg[1].addr	= 	device->addr;
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
        ioctl_msg[0].addr	= 	device->addr;
        ioctl_msg[0].buf	=	buf;
        ioctl_msg[0].flags	=	flags | I2C_M_RD;

        /* Package to i2c message to operation i2c device */
        ioctl_data.nmsgs	=	1;
        ioctl_data.msgs		=	ioctl_msg;
    }

    /* Using ioctl interface operation i2c device */
    if (ioctl(device->bus, I2C_RDWR, (unsigned long)&ioctl_data) == -1) {

        perror("Ioctl read i2c error:");
        return -1;
    }

    return len;
}


ssize_t linuxi2c_ioctl_write(const LINUXI2CDevice *device, unsigned int iaddr, const void *buf, size_t len)
{
    ssize_t remain = len;
    size_t size = 0, cnt = 0;
    const unsigned char *buffer = buf;
    unsigned char delay = GET_I2C_DELAY(device->delay);
    unsigned short flags = GET_I2C_FLAGS(device->tenbit, device->flags);

    struct i2c_msg ioctl_msg;
    struct i2c_rdwr_ioctl_data ioctl_data;
    unsigned char tmp_buf[PAGE_MAX_BYTES + INT_ADDR_MAX_BYTES];

    while (remain > 0) {

        size = GET_WRITE_SIZE(iaddr % device->page_bytes, remain, device->page_bytes);

        /* Convert i2c internal address */
        memset(tmp_buf, 0, sizeof(tmp_buf));
        linuxi2c_iaddr_convert(iaddr, device->iaddr_bytes, tmp_buf);

        /* Connect write data after device internal address */
        memcpy(tmp_buf + device->iaddr_bytes, buffer, size);

        /* Fill kernel ioctl i2c_msg */
        memset(&ioctl_msg, 0, sizeof(ioctl_msg));
        memset(&ioctl_data, 0, sizeof(ioctl_data));

        ioctl_msg.len	=	device->iaddr_bytes + size;
        ioctl_msg.addr	=	device->addr;
        ioctl_msg.buf	=	tmp_buf;
        ioctl_msg.flags	=	flags;

        ioctl_data.nmsgs =	1;
        ioctl_data.msgs	=	&ioctl_msg;

        if (ioctl(device->bus, I2C_RDWR, (unsigned long)&ioctl_data) == -1) {

            perror("Ioctl write i2c error:");
            return -1;
        }

        /* XXX: Must have a little time delay */
        linuxi2c_delay(delay);

        cnt += size;
        iaddr += size;
        buffer += size;
        remain -= size;
    }

    return cnt;
}


/*
**	@brief	:	read #len bytes data from #device #iaddr to #buf
**	#device	:	LINUXI2CDevice struct, must call i2c_device_init first
**	#iaddr	:	i2c_device internal address will read data from this address, no address set zero
**	#buf	:	i2c data will read to here
**	#len	:	how many data to read, lenght must less than or equal to buf size
**	@return : 	success return read data length, failed -1
*/
ssize_t linuxi2c_read(const LINUXI2CDevice *device, unsigned int iaddr, void *buf, size_t len)
{
    ssize_t cnt;
    unsigned char addr[INT_ADDR_MAX_BYTES];
    unsigned char delay = GET_I2C_DELAY(device->delay);

    /* Set i2c slave address */
    if (linuxi2c_select(device->bus, device->addr, device->tenbit) == -1) {

        return -1;
    }

    /* Convert i2c internal address */
    memset(addr, 0, sizeof(addr));
    linuxi2c_iaddr_convert(iaddr, device->iaddr_bytes, addr);

    /* Write internal address to devide  */
    if (write(device->bus, addr, device->iaddr_bytes) != device->iaddr_bytes) {

        perror("Write i2c internal address error");
        return -1;
    }

    /* Wait a while */
    linuxi2c_delay(delay);

    /* Read count bytes data from int_addr specify address */
    if ((cnt = read(device->bus, buf, len)) == -1) {

        perror("Read i2c data error");
        return -1;
    }

    return cnt;
}


/*
**	@brief	:	write #buf data to i2c #device #iaddr address
**	#device	:	LINUXI2CDevice struct, must call i2c_device_init first
**	#iaddr	: 	i2c_device internal address, no address set zero
**	#buf	:	data will write to i2c device
**	#len	:	buf data length without '/0'
**	@return	: 	success return write data length, failed -1
*/
ssize_t linuxi2c_write(const LINUXI2CDevice *device, unsigned int iaddr, const void *buf, size_t len)
{
    ssize_t remain = len;
    ssize_t ret;
    size_t cnt = 0, size = 0;
    const unsigned char *buffer = buf;
    unsigned char delay = GET_I2C_DELAY(device->delay);
    unsigned char tmp_buf[PAGE_MAX_BYTES + INT_ADDR_MAX_BYTES];

    /* Set i2c slave address */
    if (linuxi2c_select(device->bus, device->addr, device->tenbit) == -1) {

        return -1;
    }

    /* Once only can write less than 4 byte */
    while (remain > 0) {

        size = GET_WRITE_SIZE(iaddr % device->page_bytes, remain, device->page_bytes);

        /* Convert i2c internal address */
        memset(tmp_buf, 0, sizeof(tmp_buf));
        linuxi2c_iaddr_convert(iaddr, device->iaddr_bytes, tmp_buf);

        /* Copy data to tmp_buf */
        memcpy(tmp_buf + device->iaddr_bytes, buffer, size);

        /* Write to buf content to i2c device length  is address length and
                write buffer length */
        ret = write(device->bus, tmp_buf, device->iaddr_bytes + size);
        if (ret == -1 || (size_t)ret != device->iaddr_bytes + size)
        {
            perror("I2C write error:");
            return -1;
        }

        /* XXX: Must have a little time delay */
        linuxi2c_delay(delay);

        /* Move to next #size bytes */
        cnt += size;
        iaddr += size;
        buffer += size;
        remain -= size;
    }

    return cnt;
}


/*
**	@brief	:	i2c internal address convert
**	#iaddr	:	i2c device internal address
**	#len	:	i2c device internal address length
**	#addr	:	save convert address
*/
void linuxi2c_iaddr_convert(unsigned int iaddr, unsigned int len, unsigned char *addr)
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


/*
**	@brief		:	Select i2c address @i2c bus
**	#bus		:	i2c bus fd
**	#dev_addr	:	i2c device address
**	#tenbit		:	i2c device address is tenbit
**	#return		:	success return 0, failed return -1
*/
int linuxi2c_select(int bus, unsigned long dev_addr, unsigned long tenbit)
{
    /* Set i2c device address bit */
    if (ioctl(bus, I2C_TENBIT, tenbit)) {

        perror("Set I2C_TENBIT failed");
        return -1;
    }

    /* Set i2c device as slave ans set it address */
    if (ioctl(bus, I2C_SLAVE_FORCE, dev_addr)) {

        perror("Set i2c device address failed");
        return -1;
    }

    return 0;
}

static int i2c_rw(const LINUXI2CDevice *device, void *offset, size_t offset_len, void *buf,
		  size_t buf_len, int write_flag)
{
    struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data ioctl_data;
	unsigned int idx, count;
	int ret;

	if (!device)
		return -EINVAL;

	idx = 0;
	count = 0;

	/* Send the device's register/memory address to be read/written */
	if (offset_len) {
		msgs[0].addr = device->addr;
		msgs[0].flags = 0; /* write */
		msgs[0].len = offset_len;
		msgs[0].buf = (unsigned char*)offset;
		idx++;
		count++;
	}

	/* Read/write from/to the device's register/memory */
	if (buf_len) {
		msgs[idx].addr = device->addr;
		msgs[idx].flags = write_flag ? 0 : I2C_M_RD;
		msgs[idx].len = buf_len;
		msgs[idx].buf = (unsigned char*)buf;
		count++;
	}

	if (count) {
		ioctl_data.msgs = msgs;
		ioctl_data.nmsgs = count;

		ret = ioctl(device->bus, I2C_RDWR, &ioctl_data);
		if (ret < 0)
			return -errno;
	}

	return 0;
}

/*
**	@brief	:	Read specified bytes number of bytes from I2C device
**	#device	:	LINUXI2CDevice struct, must call i2c_device_init first
**	#offset	:	Offset within I2C device to start reading
**	#offset_len	:	Length in bytes of 'offset' parameter
**	#buf	:	Address of buffer where data will be placed
**	#buf_len	:	Length in bytes of the buffer pointed to by 'buf'
**	@return : 	0 on success, negative errno on failure
*/
ssize_t linuxi2c_primitive_read(const LINUXI2CDevice *device, void *offset, size_t offset_len, void *buf, size_t buf_len)
{
	return i2c_rw(device, offset, offset_len, buf, buf_len, 0);
}

/*
**	@brief	:	write #buf data to i2c #device #iaddr address
**	#device	:	LINUXI2CDevice struct, must call i2c_device_init first
**	#offset	: 	Offset within I2C device to start writing
**	#offset_len	: 	Length in bytes of 'offset' parameter
**	#buf	:	Address of buffer containing data to write
**	#buf_len	:	Length in bytes of the data pointed to by 'buf'
**	@return	: 	0 on success, negative errno on failure
*/
ssize_t linuxi2c_primitive_write(const LINUXI2CDevice *device, void *offset, size_t offset_len, void *buf, size_t buf_len)
{
	return i2c_rw(device, offset, offset_len, buf, buf_len, 1);
}

/*
**	@brief	:	i2c delay
**	#msec	:	milliscond to be delay
*/
static void linuxi2c_delay(unsigned char msec)
{
    if(msec)
        usleep(msec * 1000);
}
