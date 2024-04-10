#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include "comm.h"

struct Conn_stat
{
    int count;
    char ip[64];
};

// 这个进程创建映射区进行写
int map_write_main(const char *file_path)
{
    // if (argc != 2)
    // {
    //     printf("Usage: %s  file.\n", argv[0]);
    //     exit(1);
    // }
    struct Conn_stat stat = {0, "127.0.0.1"};
    // 打开文件
    int fd = open(file_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("open");
        exit(2);
    }
    // 将文件截断为第二个参数这么大
    ftruncate(fd, sizeof(struct Conn_stat));
    // 创建一个结构体大小的共享映射区。共享映射区我们可以当做数组区看待。
    struct Conn_stat *p = (struct Conn_stat *)mmap(NULL, sizeof(struct Conn_stat), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED)
    {
        perror("mmap");
        exit(3);
    }
    close(fd); // 关闭不用的文件描述符
    memcpy(p, &stat, sizeof(struct Conn_stat));
    while (p->count < 30)
    {
        p->count++;
        sleep(1);
    }
    // 解除映射
    int ret = munmap(p, sizeof(struct Conn_stat));
    if (ret < 0)
    {
        perror("mmumap");
        exit(4);
    }

    return 0;
}
