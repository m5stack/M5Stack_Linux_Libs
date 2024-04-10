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

int can_test_main(int argc,char *argv[])
{
	int s, nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
	debugging = 0;
    int opt;
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

    rfilter[0].can_id = 0x11;
    rfilter[0].can_mask = CAN_SFF_MASK;

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
	while (1)
	{
		nbytes = read(s, &frame, sizeof(frame));
        if(nbytes > 0)
        {
			if(debugging)
			{
				printf("ID=0x%X DLC=%d ", frame.can_id, frame.can_dlc);
#if 0
				for(int i = 0; i < frame.len; ++ i ) // 新版本接口
#else
				for(int i = 0; i < frame.can_dlc; ++ i ) // 已经弃用的接口，在底版本中要用到。遇到报错了，请手动调整。
#endif
				{
					printf("data[0]=0x%X ", frame.data[i]);
				}
				printf("\n");
			}
			frame.can_id = 0x12;
			nbytes = write(s, &frame, sizeof(frame));
			if (nbytes != sizeof(frame))
			{
				printf("Send Error frame\n!");
				break;
			}
        }
	}
	close(s);
	return 0;
}
