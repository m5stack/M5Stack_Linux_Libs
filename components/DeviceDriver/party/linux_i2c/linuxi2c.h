#ifndef _LIB_I2C_H_
#define _LIB_I2C_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

// will have 4 ms delay!

/* I2c device */
typedef struct i2c_device {
    int bus;			        /* I2C Bus fd, return from i2c_open */
    unsigned short addr;		/* I2C device(slave) address */
    unsigned char tenbit;		/* I2C is 10 bit device address */
    unsigned char delay;		/* I2C internal operation delay, unit millisecond */
    unsigned short flags;		/* I2C i2c_ioctl_read/write flags */
    unsigned int page_bytes;    /* I2C max number of bytes per page, 1K/2K 8, 4K/8K/16K 16, 32K/64K 32 etc */
    unsigned int iaddr_bytes;   /* I2C device internal(word) address bytes, such as: 24C04 1 byte, 24C64 2 bytes */
} LINUXI2CDevice;

/* Close i2c bus */
void linuxi2c_close(int bus);

/* Open i2c bus, return i2c bus fd */
int linuxi2c_open(const char *bus_name);

/* Initialize I2CDevice with default value */
void linuxi2c_init_device(LINUXI2CDevice *device);

/* Get i2c device description */
char *linuxi2c_get_device_desc(const LINUXI2CDevice *device, char *buf, size_t size);

/* Select i2c device on i2c bus */
int linuxi2c_select(int bus, unsigned long dev_addr, unsigned long tenbit);

/* I2C internal(word) address convert */
void linuxi2c_iaddr_convert(unsigned int int_addr, unsigned int iaddr_bytes, unsigned char *addr);

/* I2C file I/O read, write */
ssize_t linuxi2c_read(const LINUXI2CDevice *device, unsigned int iaddr, void *buf, size_t len);
ssize_t linuxi2c_write(const LINUXI2CDevice *device, unsigned int iaddr, const void *buf, size_t len);

/* I2c ioctl read, write can set i2c flags */
ssize_t linuxi2c_ioctl_read(const LINUXI2CDevice *device, unsigned int iaddr, void *buf, size_t len);
ssize_t linuxi2c_ioctl_write(const LINUXI2CDevice *device, unsigned int iaddr, const void *buf, size_t len);

/* I2c primitive read, write can set i2c flags */
ssize_t linuxi2c_primitive_read(const LINUXI2CDevice *device, void *offset, size_t offset_len, void *buf, size_t buf_len);
ssize_t linuxi2c_primitive_write(const LINUXI2CDevice *device, void *offset, size_t offset_len, void *buf, size_t buf_len);

/* I2C read / write handle function */
typedef ssize_t (*LINUXI2C_READ_HANDLE)(const LINUXI2CDevice *dev, unsigned int iaddr, void *buf, size_t len);
typedef ssize_t (*LINUXI2C_WRITE_HANDLE)(const LINUXI2CDevice *dev, unsigned int iaddr, const void *buf, size_t len);

#ifdef  __cplusplus
}
#endif

#endif
