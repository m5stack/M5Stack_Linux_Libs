/* config.h */
#ifndef CONFIG_H
#define CONFIG_H
 
#define CONFIG_CAPTURE_WIDTH        640						//显示宽度
#define CONFIG_CAPTURE_HEIGHT       480						//显示高度
#define CONFIG_CAPTURE_FPS          30						//帧数
#define CONFIG_CAPTURE_BUF_CNT      10						//
#define CONFIG_ENCODE_BITRATE       2500					//
 
#define CONFIG_DEVNAME_LEN          32
#define CONFIG_IOCTL_RETRY          5
#define CONFIG_CAPTURE_DEVICE       "/dev/video0"
#define CONFIG_DISPLAY_DEV          "/dev/fb0"
 
#define CONFIG_QUEUE_SIZE           50
#define CONFIG_QUEUE_LIMIT          2
 
#endif