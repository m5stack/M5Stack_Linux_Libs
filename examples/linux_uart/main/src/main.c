#include <stdio.h> /*标准输入输出定义*/
#include <unistd.h>
#include <string.h>
#include "main.h"
int Debug_s = 1;



char uart_dev[20];

static void help_p()
{
    printf("\n--------Welcome to debugging help--------------\n");
    printf("-h:\t\tDisplay the Help page\n");
    printf("-d:\t\tEnable debugging log display\n");
    printf("-s:\t\tStart uart test server!\n");
    printf("-t:\t\tStart uart test\n");
    printf("\texample ./linux_uart -f /dev/ttySX -s/-t \n");
    printf("\n-----------------------------------------------\n");
}
int debugging;

int main(int argc,char *argv[])
{
    int opt;
    if (1 != argc)
    {
        while ((opt = getopt(argc, argv, "hdf:st")) != -1)
        {
            switch (opt)
            {
            case 'h':
                help_p();
                return 0;
                break;
            case 'd':
                debugging = 1;
                break;
            case 'f':
                sprintf(uart_dev,"%s", optarg);
                break;
            case 's':
                server_main();
                break;
            case 't':
                main_clint();
                break;
            case '?':
                printf("Unknown option: %s\n",optarg);  
                break;
            default:
                break;
            }
        }
    }
    else
    {
        help_p();
        return 1;
    }
    return 0;
}
