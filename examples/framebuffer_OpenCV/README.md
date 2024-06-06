# framebuffer_OpenCV
This is a replica of the CoreMP135_framebuffer_OpenCV project by nnn112358, used to demonstrate the application of OpenCV on CoreMP135.

This project requires the use of a higher version of gcc.

```bash
sudo apt install arm-linux-gnueabihf-g++ arm-linux-gnueabihf-gcc

scons

scons push
```

### result in CoreMP135
```bash
root@M5Core135:~$ ./frame_buffer_opencv
/dev/fb0 information
--------------------------
Frame Buffer Device Information:
ID: stmdrmfb
Memory Length: 1843200 bytes
Type: 0
Type Aux: 0
Visual: 2
Line Length: 2560 bytes per line
--------------------------
Variable Framebuffer Information:
Resolution: 1280x720
Virtual Resolution: 1280x720
Bits Per Pixel: 16
Red: Offset 11, Length 5
Green: Offset 5, Length 6
Blue: Offset 0, Length 5
--------------------------
/dev/fb1 information
--------------------------
Frame Buffer Device Information:
ID: fb_ili9341
Memory Length: 153600 bytes
Type: 0
Type Aux: 0
Visual: 2
Line Length: 640 bytes per line
--------------------------
Variable Framebuffer Information:
Resolution: 320x240
Virtual Resolution: 320x240
Bits Per Pixel: 16
Red: Offset 11, Length 5
Green: Offset 5, Length 6
Blue: Offset 0, Length 5
--------------------------
```
### Apendix
<img width="640" alt="S__80977923" src="https://github.com/nnn112358/CoreMP135_framebuffer_OpenCV/assets/27625496/7253fda7-6f79-4ebc-9a60-d4bf5b55b4bf">


### Acknowledgments
[nnn112358](https://github.com/nnn112358)

### Reference link
- https://github.com/nnn112358/CoreMP135_framebuffer_OpenCV.git
- https://github.com/nihui/opencv-mobile.git
