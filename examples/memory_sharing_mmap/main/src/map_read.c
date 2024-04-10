#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "comm.h"

struct Conn_stat
{
    int count;
    char ip[64];
};

int map_read_main(const char *file_path)
{
    // if (argc != 2)
    // {
    //     printf("Usage: %s  file.\n", argv[0]);
    //     exit(1);
    // }
    // 打开文件-只读，注意与写中的参数不同
    int fd = open(file_path, O_RDONLY, 0644);
    if (fd < 0)
    {
        perror("open");
        exit(2);
    }

    struct Conn_stat stat;
    // 注意与写中的参数不同
    struct Conn_stat *p = (struct Conn_stat *)mmap(NULL, sizeof(struct Conn_stat), PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED)
    {
        perror("mmap");
        exit(3);
    }
    close(fd);
    int i = 0;
    while ((i++) < 10)
    {
        printf("ip = %s ,count: %d\t\t\n", p->ip, p->count);
        sleep(1);
    }
    // 接触映射
    int ret = munmap(p, sizeof(stat));
    if (ret < 0)
    {
        perror("mmumap");
        exit(4);
    }
    return 0;
}