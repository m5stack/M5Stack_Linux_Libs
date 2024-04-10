#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include "comm.h"

struct Conn_stat
{
    int count;
    char ip[64];
    int count_max;
};

int shmwrite_main(void)
{

    void *shm = NULL;
    int shmid = 0; // 存储共享内存的标识
    struct Conn_stat stat = {0, "127.0.0.1"};

    // 创建共享内存
    shmid = shmget((key_t)1234, sizeof(struct Conn_stat), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(1);
    }
    // 将共享内存挂接到当前进程的地址空间
    shm = shmat(shmid, (void *)0, 0);
    if (shm == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(2);
    }
    printf("Memory attached at %p\n", shm);
    // 设置共享内存
    struct Conn_stat *p = (struct Conn_stat *)shm;
    memcpy(p, &stat, sizeof(struct Conn_stat));
    // 修改共享内存中的数据
    int i = 0;
    p->count_max = 50 - 3;
    while ((i++) < 50)
    {
        p->count++;
        sleep(1);
    }
    // 把共享内存从当前进程中分离
    if (shmdt(shm) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(3);
    }
    return 0;
}