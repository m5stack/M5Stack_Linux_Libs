# CAN
CAN（Controller Area Network），这是一种常见的实时通信协议，用于在汽车、工业自动化、机器人和其他领域中进行高速、可靠的数据通信。

关于CAN的重要概念：

1. **多主多从架构：** CAN通信是多主多从的通信架构，多个设备可以同时传输和接收数据，从而实现高效的分布式通信。

2. **帧格式：** CAN通信使用帧格式来传输数据。基本帧由标识符、数据、控制位和CRC（循环冗余校验）组成。

3. **标识符：** 每个CAN帧都有一个唯一的标识符，用于标识消息的类型和优先级。标识符由11位或29位组成，决定了消息的优先级。

4. **数据率：** CAN支持多种数据传输率，例如125 Kbps、500 Kbps、1 Mbps等，您可以根据需要进行选择。

5. **冲突检测：** CAN使用CSMA/CR（Carrier Sense Multiple Access / Collision Resolution）机制来检测和解决数据冲突。

6. **错误检测和纠正：** CAN使用CRC来检测错误，并且具有一些错误纠正的能力，确保可靠的数据传输。

7. **广播和单播：** CAN支持广播消息，使得所有设备都能收到消息。同时，也支持通过标识符进行单播，只有特定设备会接收消息。

8. **应用领域：** CAN通信广泛应用于汽车内部网络、工业自动化、航空航天、机器人和医疗设备等领域。

CAN 在 linux 被抽象成了一个网络设备,所以对 CAN 总线的编程被称为[SocketCAN](https://en.wikipedia.org/wiki/SocketCAN).

在linux 中使用can 首先要设置can的通信速率,然后开启can总线
```bash
# 查看 CAN 接口
sudo ip link list

# 设置 CAN 接口速率为 500000
sudo ip link set can0 type can bitrate 500000

# 开启 CAN 接口
sudo ip link set up can0

# 关闭 CAN 接口
sudo ip link set down can0
```


为了展示 CAN 编程,创建一个虚拟的 CAN 总线.
```bash
sudo modprobe can
sudo modprobe can_raw
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
sudo ip link show vcan0

```

然后在 C 程序中读写 CAN 总线.
``` C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int main(void)
{
    int s;
    int nbytes;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;

    const char *ifname = "vcan0";

    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1)
    {
        perror("Error while opening socket");
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("Error in socket bind");
        return -2;
    }

    frame.can_id = 0x123;
    frame.can_dlc = 2;
    frame.data[0] = 0x11;
    frame.data[1] = 0x22;

    nbytes = write(s, &frame, sizeof(struct can_frame));

    printf("Wrote %d bytes\n", nbytes);

    return 0;
}

```
可以使用 candump 实用程序在 vcan0 接口上分析数据包，该实用程序是 SocketCAN can-utils 包的一部分。
如果没有安装 can-utils,可以使用下面的命令安装.
``` bsah
sudo apt install can-utils
```
首先在一个终端中运行命令:
``` bash
candump vcan0
```
然后另起一个终端编译运行上述程序,然后就能在 运行 candump 命令的终端看到:
``` bash
  vcan0  123   [2]  11 22
```
看完信息可以按 `ctrl + c` 结束 candump 程序的运行.

在编译上述程序时可以在 [M5Stack_Linux_Libs]() 的 examples/linux_can 目录下编译并运行.
``` bash
# 克隆仓库
git clone https://github.com/M5STACK/M5Stack_Linux_Libs.git

# 进入目录
cd M5Stack_Linux_Libs/examples/linux_can

# 编译
make 

# 运行
make run

推荐：
    https://zhuanlan.zhihu.com/p/470681140?utm_id=0
```
