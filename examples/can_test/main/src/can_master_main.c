#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "test.h"
static void help_p()
{
    printf("\n--------Welcome to debugging help--------------\n");
    printf("-h:\t\tDisplay the Help page\n");
    printf("-d:\t\tEnable debugging log display\n");
    printf("\n-----------------------------------------------\n");
}
int debugging ;

int main(int argc,char *argv[])
{
    struct timeval timeout;
    fd_set readfd;    
	int s, nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
	debugging = 0;
    int opt;
	FD_ZERO(&readfd); /* 清空文件描述符集合 */
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    if (1 != argc)
    {
        while ((opt = getopt(argc, argv, "hd")) != -1)
        {
            switch (opt)
            {
            case 'h':
                help_p();
                exit(0);
                break;
            case 'd':
                debugging = 1;
                break;
            default:
                break;
            }
        }
    }
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	strcpy(ifr.ifr_name, "can0");
	ioctl(s, SIOCGIFINDEX, &ifr);
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(s, (struct sockaddr *)&addr, sizeof(addr)); 
	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    rfilter[0].can_id = 1;
    rfilter[0].can_mask = CAN_SFF_MASK;

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));


    frame.can_id = 604;
    frame.can_dlc = 8;
	frame.data[1] = 1;
	frame.data[2] = 2;
	frame.data[3] = 3;
	frame.data[4] = 4;
	frame.data[5] = 5;
	frame.data[6] = 6;
	frame.data[7] = 7;
	frame.data[8] = 8;

	while (1)
	{
		nbytes = write(s, &frame, sizeof(frame));
		if (nbytes != sizeof(frame))
		{
			printf("Send Error frame!\n");
			close(s);
			return 1;
		}
		sleep(1);
	}
	
	close(s);
	return 0;
}
