# CM4Stack_QtDemo
Qt demo for CM4STACK

![](https://github.com/m5stack/M5Stack_Linux_Libs/blob/master/assets/image/2024-07-05-09-44-30.png?raw=true)

### Dependency

```bash
sudo apt update
sudo apt install qtbase5-dev qt5-qmake qtbase5-dev-tools qml qtdeclarative5-dev pkg-config
sudo apt install build-essential cmake 
```

### Get code

```bash
git clone https://github.com/Forairaaaaa/CM4Stack_QtDemo.git
cd CM4Stack_QtDemo
```

### Build

```bash
mkdir build && cd build
cmake .. && make
```

### Run

```bash
./cm4QtDemo
```

### Display on specified framebuffer

```bash
# export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb1
export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb$(cat /proc/fb | grep fb_st7789v | awk '{print $1}')
./cm4QtDemo
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