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

int can_master_main(int argc,char *argv[])
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

    rfilter[0].can_id = 0x12;
    rfilter[0].can_mask = CAN_SFF_MASK;

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));


	char test_data[3] = {0x01, 0x02, 0x03};

	frame.can_id = 0x11;
	frame.can_dlc = 0x03;
	frame.data[0] = 0x01;
	frame.data[1] = 0x02;
	frame.data[2] = 0x03;

	nbytes = write(s, &frame, sizeof(frame));
	if (nbytes != sizeof(frame))
	{
		printf("Send Error frame!\n");
		close(s);
		return 1;
	}
	sleep(1);

	FD_SET(s, &readfd);
	select(s + 1, &readfd, NULL, NULL, &timeout);
	if (FD_ISSET(s, &readfd)) /* 测试fd_key是否在描述符集合中 */
	{
		nbytes = read(s, &frame, sizeof(frame));
		if(nbytes > 0)
		{
			if(frame.data[0] != 0x01)
			{
				close(s);
				return 3;
			}
			if(frame.data[1] != 0x02)
			{
				close(s);
				return 4;
			}
			if(frame.data[2] != 0x03)
			{
				close(s);
				return 5;
			}
		}
		else
		{
			printf("reace Error frame\n!");
			close(s);
			return 2;
		}
	}
	else
	{
		printf("reace Error frame\n!");
		close(s);
		return 6;
	}
        



	printf("can test success!\n");


	// while (1)
	// {
	// 	nbytes = read(s, &frame, sizeof(frame));
    //     if(nbytes > 0)
    //     {
	// 		if(debugging)
	// 		{
	// 			printf("ID=0x%X DLC=%d ", frame.can_id, frame.can_dlc);
	// 			for(int i = 0; i < frame.len; ++ i )
	// 			{
	// 				printf("data[0]=0x%X ", frame.data[i]);
	// 			}
	// 			printf("\n");
	// 		}
	// 		frame.can_id = 0x12;
	// 		nbytes = write(s, &frame, sizeof(frame));
	// 		if (nbytes != sizeof(frame))
	// 		{
	// 			printf("Send Error frame\n!");
	// 			break;
	// 		}
    //     }
	// }
	close(s);
	return 0;
}
