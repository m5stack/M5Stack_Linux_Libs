# DeviceDriver
Device driver for Linux platform, including various small sensors and actuators, along with application-level programs.
```bash
.
├── SConstruct
├── include
├── Kconfig
├── party
│   ├── framebuffer
│   │   ├── fbtools.c
│   │   └── fbtools.h
│   ├── linux_i2c
│   │   ├── linuxi2c.c
│   │   └── linuxi2c.h
│   ├── linux_spi
│   │   ├── linux_spi.c
│   │   └── linux_spi.h
│   ├── linux_uart
│   │   ├── linux_uart.c
│   │   └── linux_uart.h
│   └── ptmx
│       ├── ptmx_creat.c
│       └── ptmx_creat.h
```


## linux_uart
A non-standard C library for Linux, used to read and write to serial devices under the /dev directory. It facilitates serial communication on Linux.

The library is located in [components/DeviceDriver/party/linux_uart](../../components/DeviceDriver/party/linux_uart).

Activate the component.

Use the command `make menuconfig` to enable 'Enable DeviceDriver -> compile uart lib' and compile the UART library.

Permanently enable.

Add the following to the config_defaults.mk file in the project:
```
CONFIG_DEVICE_DRIVER_ENABLED=y
CONFIG_DEVICE_UART_ENABLED=y
```

Usage example: [examples/linux_uart](../../examples/linux_uart)

API
```c
typedef struct{
	int baud;              // UART baud rate.
	int data_bits;         // UART data bits.
	int stop_bits;         // UART stop bits.
	char parity;           // parity.
}uart_t;

/**
 * @brief init uart
 * @note 
 * @param [in] dev    device name
 * @param [in] param  struct uart_t pointer
 * @retval [return] [>0] fd index
 * @retval [return] [<=0] fail code
 */
int linux_uart_init(char* dev, void* param);

/**
 * @brief close uart
 * @note 
 * @param [in] fd    fd index
 * @retval 
 */
void linux_uart_deinit(int fd);

/**
 * @brief Read data from UART.
 * @note 
 * @param [in] fd    fd index
 * @param [in] cnt    buff size
 * @param [in] buf    buff pointer
 * @retval [return] [>0] The size of the read data.
 * @retval [return] [<0] Read failure code.
 */
int linux_uart_read(int fd, int cnt, void* buf);

/**
 * @brief Send data from UART.
 * @note 
 * @param [in] fd    fd index
 * @param [in] cnt    buff size
 * @param [in] buf    buff pointer
 * @retval [return] [>0] The size of the sent data.
 * @retval [return] [<0] Send failure code.
 */
int linux_uart_write(int fd, int cnt, void* buf);

```



## linux_i2c

A non-standard C library for Linux, used to read and write to i2c devices under the /dev directory. It facilitates i2c communication on Linux.

The library is located in [components/DeviceDriver/party/linux_i2c](../../components/DeviceDriver/party/linux_i2c).

Activate the component.

Use the command `make menuconfig` to enable 'Enable DeviceDriver -> compile i2c lib' and compile the i2c library.

Permanently enable.

Add the following to the config_defaults.mk file in the project:
```
CONFIG_DEVICE_DRIVER_ENABLED=y
CONFIG_DEVICE_I2C_ENABLED=y
```

Usage example: [examples/linux_i2c](../../examples/linux_i2c)

API：
```c
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

/*
**	@brief		:	Open i2c bus
**	#bus_name	:	i2c bus name such as: /dev/i2c-1
**	@return		:	failed return -1, success return i2c bus fd
*/
int linuxi2c_open(const char *bus_name);

/*
**	@brief		:	Initialize LINUXI2CDevice with defualt value
**	#device	    :	LINUXI2CDevice struct
*/
void linuxi2c_init_device(LINUXI2CDevice *device);

/*
**	@brief		:	Get LINUXI2CDevice struct desc
**	#device	    :	LINUXI2CDevice struct
**  #buf        :   Description message buffer
**  #size       :   #buf size
**	@return		:	return i2c device desc
*/
char *linuxi2c_get_device_desc(const LINUXI2CDevice *device, char *buf, size_t size);

/* Select i2c device on i2c bus */
int linuxi2c_select(int bus, unsigned long dev_addr, unsigned long tenbit);

/* I2C internal(word) address convert */
void linuxi2c_iaddr_convert(unsigned int int_addr, unsigned int iaddr_bytes, unsigned char *addr);

/*  I2C file I/O read, write
**	@brief	:	read #len bytes data from #device #iaddr to #buf
**	#device	:	LINUXI2CDevice struct, must call i2c_device_init first
**	#iaddr	:	i2c_device internal address will read data from this address, no address set zero
**	#buf	:	i2c data will read to here
**	#len	:	how many data to read, lenght must less than or equal to buf size
**	@return : 	success return read data length, failed -1
*/
ssize_t linuxi2c_read(const LINUXI2CDevice *device, unsigned int iaddr, void *buf, size_t len);
ssize_t linuxi2c_write(const LINUXI2CDevice *device, unsigned int iaddr, const void *buf, size_t len);

/* I2c ioctl read, write can set i2c flags
**	i2c_ioctl_read/write
**	I2C bus top layer interface to operation different i2c devide
**	This function will call XXX:ioctl system call and will be related
**	i2c-dev.c i2cdev_ioctl to operation i2c device.
**	1. it can choice ignore or not ignore i2c bus ack signal (flags set I2C_M_IGNORE_NAK)
**	2. it can choice ignore or not ignore i2c internal address
**
*/
ssize_t linuxi2c_ioctl_read(const LINUXI2CDevice *device, unsigned int iaddr, void *buf, size_t len);
ssize_t linuxi2c_ioctl_write(const LINUXI2CDevice *device, unsigned int iaddr, const void *buf, size_t len);

/*
**	i2c_primitive_read/write
**	I2C bus top layer interface to operation different i2c devide
**	This function will call XXX:primitive system call and will be related
**	i2c-dev.c i2cdev_ioctl to operation i2c device.
**	1. it can choice ignore or not ignore i2c bus ack signal (flags set I2C_M_IGNORE_NAK)
**	2. it can choice ignore or not ignore i2c internal address
**
*/
ssize_t linuxi2c_primitive_read(const LINUXI2CDevice *device, void *offset, size_t offset_len, void *buf, size_t buf_len);
ssize_t linuxi2c_primitive_write(const LINUXI2CDevice *device, void *offset, size_t offset_len, void *buf, size_t buf_len);
```


## linux_spi

A non-standard C library for Linux, used to read and write to spi devices under the /dev directory. It facilitates spi communication on Linux.

The library is located in [components/DeviceDriver/party/linux_spi](../../components/DeviceDriver/party/linux_spi).

Activate the component.

Use the command `make menuconfig` to enable 'Enable DeviceDriver -> compile spi lib' and compile the i2c library.

Permanently enable.

Add the following to the config_defaults.mk file in the project:
```
CONFIG_DEVICE_DRIVER_ENABLED=y
CONFIG_DEVICE_SPI_ENABLED=y
```

Usage example: [examples/linux_spi](../../examples/linux_spi)

API:
```c
// open and init SPIdev
// * spi_mode may have next mask: SPI_LOOP | SPI_CPHA | SPI_CPOL |
//   SPI_LSB_FIRST | SPI_CS_HIGH SPI_3WIRE | SPI_NO_CS | SPI_READY
//   or 0 by zefault
int spi_init(spi_t *self,
             const char *device, // filename like "/dev/spidev0.0"
             int mode,           // SPI_* (look "linux/spi/spidev.h")
             int bits,           // bits per word (usually 8)
             int speed);         // max speed [Hz]
//----------------------------------------------------------------------------
// sets mode on existing spi
int spi_set_mode(spi_t* self, int mode);
//----------------------------------------------------------------------------
// sets speed on existing spi
int spi_set_speed(spi_t* self, int speed);
//----------------------------------------------------------------------------
// close SPIdev file and free memory
void spi_free(spi_t *self);
//----------------------------------------------------------------------------
// read data from SPIdev
int spi_read(spi_t *self, void* rx_buf, int len);
//----------------------------------------------------------------------------
// write data to SPIdev
int spi_write(spi_t *self, const void* tx_buf, int len);
//----------------------------------------------------------------------------
// read and write `len` bytes from/to SPIdev
int spi_exchange(spi_t *self, void* rx_buf, const void* tx_buf, int len);
//----------------------------------------------------------------------------
// read data from SPIdev from specific register address
int spi_read_reg8(spi_t *self, uint8_t reg_addr, void* rx_buf, int len);
//----------------------------------------------------------------------------
// write data to SPIdev to specific register address
int spi_write_reg8(spi_t *self, uint8_t reg_addr, const void* tx_buf, int len);
```

## framebuffer

A non-standard C library for Linux, used to read and write to framebuffer devices under the /dev directory. It facilitates framebuffer communication on Linux.

The library is located in [components/DeviceDriver/party/framebuffer](../../components/DeviceDriver/party/framebuffer).

Activate the component.

Use the command `make menuconfig` to enable 'Enable DeviceDriver -> compile framebuffer lib' and compile the i2c library.

Permanently enable.

Add the following to the config_defaults.mk file in the project:
```
CONFIG_DEVICE_DRIVER_ENABLED=y
CONFIG_DEVICE_FRAMEBUFFER_ENABLED=y
```

Usage example: [examples/linux_framebuffer](../../examples/linux_framebuffer)

API:
```c
/* a framebuffer device structure */
typedef struct fbdev{
int fb;
unsigned long fb_mem_offset;
unsigned long fb_mem;
struct fb_fix_screeninfo fb_fix;
struct fb_var_screeninfo fb_var;
char dev[20];
} FBDEV, *PFBDEV;

/* open & init a frame buffer */
/* to use this function,
you must set FBDEV.dev="/dev/fb0"
or "/dev/fbX" */
/* it's your frame buffer. */
int fb_open(PFBDEV pFbdev);

/*close a frame buffer*/
int fb_close(PFBDEV pFbdev);

/*get display depth*/
int get_display_depth(PFBDEV pFbdev);

/*draw rgb888 or rgb565 to framebuffer.color=24/16*/
void fb_draw_img(PFBDEV pFbdev, int w, int h, int color, void *buf);
void fb_put_pixel(PFBDEV pFbdev, int x, int y, unsigned int color);

/*full screen clear */
void fb_memset(void *addr, int c, size_t len);
```

## ptmx

A non-standard C library for Linux, used to read and write to pts devices under the /dev directory. Standard input/output for linux virtual terminals to operate other processes. Useful in daemons.

The library is located in [components/DeviceDriver/party/ptmx](../../components/DeviceDriver/party/ptmx).

Activate the component.

Use the command `make menuconfig` to enable 'Enable DeviceDriver -> compile ptmx lib' and compile the i2c library.

Permanently enable.

Add the following to the config_defaults.mk file in the project:
```
CONFIG_DEVICE_DRIVER_ENABLED=y
CONFIG_DEVICE_PTMX_ENABLED=y
```

Usage example: [examples/ptxm_tcp_tran](../../examples/ptxm_tcp_tran)

API：
```c
typedef enum {
	PTMX_STOP = 0,
	PTMX_RUN = 1
} ptmx_status_t;

/*msg call back */
typedef void (*msg_call_back_t)(const char*, int);
/* a ptmx device structure */
struct ptmx_creat_t 
{
	int master_ptmx;
	char slave_ptmx_name[50];
	int ptmx_status;			
	pthread_t ptmx_loop_handler_d;
	msg_call_back_t call;
	int buff_size;
};
/*creat a ptmx struct ptmx_creat_t */
struct ptmx_creat_t* ptmx_creat();
/*destroy ptmx struct ptmx_creat_t */
int ptmx_destroy(struct ptmx_creat_t **p);
/*open one ptmx*/
int ptmx_open(struct ptmx_creat_t *self);

/*set msg callback fun*/
void ptmx_set_msg_callback(struct ptmx_creat_t *self, msg_call_back_t fun);
void *ptmx_loop_handler(void *par);
/*exit one ptmx*/
void ptmx_exit(struct ptmx_creat_t *self);
#ifdef __cplusplus
class ptmx
{
private:
	struct ptmx_creat_t ptm;
public:
	ptmx();
    /*open ptmx*/
	int open();
    /*set ptmx read buff*/
	void set_buff_size(int size);
    /*set msg callback fun*/
	void set_msg_call_back(msg_call_back_t msg);
	std::string get_slave_ptmx_name();
	void exit();
	~ptmx();
};
#endif

```


