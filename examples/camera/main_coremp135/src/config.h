/* config.h */
// Copyright (c) 2024 M5Stack Technology CO LTD
#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_CAPTURE_WIDTH   320   // Display width
#define CONFIG_CAPTURE_HEIGHT  240   // Display height
#define CONFIG_CAPTURE_FPS     10    //
#define CONFIG_CAPTURE_BUF_CNT 10    //
#define CONFIG_ENCODE_BITRATE  2500  //

#define CONFIG_DEVNAME_LEN    32
#define CONFIG_IOCTL_RETRY    5
#define CONFIG_CAPTURE_DEVICE "/dev/video0"
#define CONFIG_DISPLAY_DEV    "/dev/fb1"  // lcd

#define CONFIG_QUEUE_SIZE  20
#define CONFIG_QUEUE_LIMIT 2

#endif