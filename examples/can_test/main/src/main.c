/*
sudo ip link list

# Set the CAN interface speed to 250,000.
ip link set can0 type can bitrate 250000
ip link set can1 type can bitrate 250000
# Enable the CAN interface.
ip link set up can0
ip link set up can1
# Disable the CAN interface.
sudo ip link set down can0
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    int s;
    int nbytes;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;
    struct can_filter rfilter[1];

    // Create socket.
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Socket");
        return 1;
    }

    // Set the socket hardware port to can0.
    strcpy(ifr.ifr_name, "can0" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // Bind the socket to the CAN interface.
    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    // Enable filtering of received CAN frames.
    // rfilter[0].can_id = 1;
    // rfilter[0].can_mask = CAN_SFF_MASK;
    // setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    // Send CAN frame.
    frame.can_id = 0x123;
    frame.can_dlc = 2;
    frame.data[0] = 0x11;
    frame.data[1] = 0x22;

    printf("Sending CAN frame...\n");
    nbytes = write(s, &frame, sizeof(struct can_frame));
    if (nbytes != sizeof(struct can_frame)) {
        perror("Write");
        return 1;
    }

    // Receive CAN frame.
    printf("Receiving CAN frame...\n");
    nbytes = read(s, &frame, sizeof(struct can_frame));
    if (nbytes != sizeof(struct can_frame)) {
        perror("Read");
        return 1;
    }

    printf("Received frame with ID: 0x%X\n", frame.can_id);
    printf("Data: 0x%X 0x%X\n", frame.data[0], frame.data[1]);

    // Close the socket.
    close(s);

    return 0;
}
