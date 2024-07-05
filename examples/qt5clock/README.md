# qt5clock_QtDemo
Qt demo for CoreMP135

![](https://github.com/m5stack/M5Stack_Linux_Libs/blob/dev/assets/image/2024-07-05-09-44-30.png?raw=true)

### Dependency

```bash
sudo apt update
sudo apt install qtbase5-dev qt5-qmake qtbase5-dev-tools qml qtdeclarative5-dev pkg-config
sudo apt install python3 python3-pip libffi-dev
pip3 install parse scons requests 
```

### Get code

```bash
git clone https://github.com/m5stack/M5Stack_Linux_Libs.git
cd M5Stack_Linux_Libs/examples/qt5clock
```

### Build

```bash
scons
```

### Run

```bash
./dist/qt5clock
```

### Display on specified framebuffer

```bash
# export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb1
export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb$(cat /proc/fb | grep ili9342c | awk '{print $1}')
./dist/qt5clock
```

Demo will display on `/dev/fb1`, learn more:  https://doc.qt.io/qt-6/embedded-linux.html

You can unset this config:

```bash
unset QT_QPA_PLATFORM
```

## Compilation Engineering
Because the QT project has a very large number of dependencies, it is very difficult to do cross-compilation alone, so this example can no longer be developed using cross-compilation.  
When verifying on the host, you can directly compile using scons and then run it. If you want to deploy it to CoreMP135, there are two ways to compile.  
The first method: compile directly on CoreMP135, the advantage is immediate use after compilation, but the disadvantage is that the compilation speed is very slow.  
The second method: mount the image of CoreMP135 to a Linux system, use chroot to leverage the host's computing power for compilation. The advantage is fast compilation speed, but the disadvantage is that compilation testing is more complex.  
**`CoreMP135 needs to use the Debian system.`**
### mount rootfs cross-compilation 
```bash
# Dependency
sudo apt install qemu-user-static
# Insert the SD card into the card reader and then plug it into the Linux computer. Be sure to check if there is enough disk space for compiling operations. Please perform the disk expansion operation on the CoreMP135 device.

# Assuming the mounting directory of the disk is /media/nihao/rootfs.
# Assuming the mounting directory of the disk is /media/nihao/rootfs.
# Assuming the mounting directory of the disk is /media/nihao/rootfs.

sudo mount /tmp /media/nihao/rootfs/tmp -o bind
sudo mount /dev /media/nihao/rootfs/dev -o bind
sudo chroot /media/nihao/rootfs /usr/bin/apt update
sudo chroot /media/nihao/rootfs /usr/bin/apt install qtbase5-dev qt5-qmake qtbase5-dev-tools qml qtdeclarative5-dev pkg-config gcc g++ python3 python3-pip libffi-dev git -y
sudo chroot /media/nihao/rootfs /usr/bin/pip3 install parse scons requests --break-system-packages


sudo chroot /media/nihao/rootfs /usr/bin/bash -c "cd /root && git clone https://github.com/m5stack/M5Stack_Linux_Libs.git && cd M5Stack_Linux_Libs/examples/qt5clock && scons "
sync
sleep 1
sudo umount /media/nihao/rootfs/tmp
sudo umount /media/nihao/rootfs/dev
sync
```
### Please insert the SD card into the CoreMP135, boot up the system, and run the compiled program after logging in
```bash
cd /root/M5Stack_Linux_Libs/examples/qt5clock
# export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb1
export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb$(cat /proc/fb | grep ili9342c | awk '{print $1}')
./dist/qt5clock
```


![](https://github.com/m5stack/M5Stack_Linux_Libs/blob/dev/assets/image/20240705171644.jpg?raw=true)










