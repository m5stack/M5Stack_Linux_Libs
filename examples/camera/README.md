# camera
This is a demo program that reads images from a camera and displays them on an LCD screen.  
/dev/video0 -> /dev/fb1


In Debian, you can use ffmpeg to display the content from a camera directly onto an LCD screen.  
```bash
apt install ffmpeg
ffmpeg -f video4linux2 -i /dev/video0 -pix_fmt rgb565le -vf "scale=320:240" -f fbdev /dev/fb1
```
- `ffmpeg`: This is the command-line tool used for handling multimedia data.
- `-f video4linux2`: This option specifies the input format as `video4linux2`, which is a standard interface for video capture devices in Linux.
- `-i /dev/video0`: This option sets the input device, where `/dev/video0` is the default device file for the first video capture device (e.g., a webcam) on a Linux system.
- `-pix_fmt rgb565le`: This option sets the pixel format to `rgb565le`, which stands for RGB with 5 bits for red, 6 bits for green, and 5 bits for blue, in little-endian format.
- `-vf scale=320:240`: This is a video filter command that scales the video to a resolution of 320x240 pixels.
- `-f fbdev`: This option sets the output format to `fbdev`, which is the framebuffer device.
- `/dev/fb1`: This is the output device, where `/dev/fb1` is the device file for the second framebuffer on a Linux system. The video will be displayed on this framebuffer.