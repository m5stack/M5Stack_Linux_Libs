#ifndef __M5_SH1107_DEV_H_
#define __M5_SH1107_DEV_H_

#define I2C_CONTROL_BYTE_CMD_SINGLE    0x80
#define I2C_CONTROL_BYTE_CMD_STREAM    0x00
#define I2C_CONTROL_BYTE_DATA_SINGLE   0xC0
#define I2C_CONTROL_BYTE_DATA_STREAM   0x40
#ifdef  __cplusplus
extern "C" {
#endif
#include "linux_i2c/linuxi2c.h"
extern LINUXI2CDevice i2cdev;

int m5_sh1107_dev_init(int num);

int m5_sh1107_dev_set_img(int page, int seg, void * images, int width);

int m5_sh1107_dev_deinit();

#ifdef  __cplusplus
}
#endif
#endif /* __M5_SH1107_DEV_H_ */