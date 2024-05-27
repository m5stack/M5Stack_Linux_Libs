# CAN
CAN (Controller Area Network) is a common real-time communication protocol used for high-speed, reliable data communication in automotive, industrial automation, robotics, and other fields.

Key concepts regarding CAN:

1. **Multi-master, Multi-slave Architecture:** CAN communication follows a multi-master, multi-slave architecture where multiple devices can simultaneously transmit and receive data, enabling efficient distributed communication.

2. **Frame Format:** CAN communication uses a frame format to transmit data. A basic frame consists of an identifier, data, control bits, and CRC (Cyclic Redundancy Check).

3. **Identifier:** Each CAN frame has a unique identifier used to specify the type and priority of the message. The identifier is either 11 bits or 29 bits in length, which determines the message priority.

4. **Data Rate:** CAN supports various data transmission rates, such as 125 Kbps, 500 Kbps, and 1 Mbps. You can choose the rate according to your needs.

5. **Collision Detection:** CAN uses the CSMA/CR (Carrier Sense Multiple Access / Collision Resolution) mechanism to detect and resolve data collisions.

6. **Error Detection and Correction:** CAN employs CRC to detect errors and has some error correction capabilities to ensure reliable data transmission.

7. **Broadcast and Unicast:** CAN supports broadcast messages, allowing all devices to receive the message. It also supports unicast through identifiers, where only specific devices will receive the message.

8. **Application Areas:** CAN communication is widely used in automotive internal networks, industrial automation, aerospace, robotics, and medical devices, among other fields.

CAN has been abstracted as a network device in Linux, so programming for the CAN bus is referred to as [SocketCAN](https://en.wikipedia.org/wiki/SocketCAN).

In Linux, to use CAN, you first need to set the communication baud rate for CAN, and then enable the CAN bus.
```bash
# View CAN interface.
sudo ip link list

# Set the CAN interface speed to 500000.
sudo ip link set can0 type can bitrate 500000

# Activate the CAN interface.
sudo ip link set up can0

# Disable the CAN interface.
sudo ip link set down can0
```


To demonstrate CAN programming, create a virtual CAN bus.
```bash
sudo modprobe can
sudo modprobe can_raw
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
sudo ip link show vcan0

```

Then read and write the CAN bus in a C program.
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
You can analyze packets on the vcan0 interface using the candump utility, which is part of the SocketCAN can-utils package.
If can-utils is not installed, you can use the following command to install it.
``` bsah
sudo apt install can-utils
```
First, run the command in a terminal:
``` bash
candump vcan0
```
Then, start another terminal to compile and run the above program, and you will be able to see in the terminal where the candump command is running:
``` bash
  vcan0  123   [2]  11 22
```
After viewing the information, you can press `ctrl + c` to terminate the candump program.

When compiling the above program, you can compile and run it in the examples/linux_can directory of [M5Stack_Linux_Libs]().
``` bash
# Clone repository
git clone https://github.com/M5STACK/M5Stack_Linux_Libs.git

# Enter directory
cd M5Stack_Linux_Libs/examples/linux_can

# Compile
scons 


#Quote：
#    https://zhuanlan.zhihu.com/p/470681140?utm_id=0
```
