#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include "comm.h"

struct Conn_stat
{
    int count;
    char ip[64];
    int count_max;
};

int shmread_main(void)
{

    void *shm = NULL; // 分配的共享内存的原始首地址
    struct Conn_stat *stat = NULL;
    int shmid; // 共享内存标识符
    // 创建共享内存
    shmid = shmget((key_t)1234, sizeof(struct Conn_stat), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(0);
    }
    // 将共享内存挂接到当前当前进程的地址空间
    shm = shmat(shmid, 0, 0);
    if (shm == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(1);
    }
    printf("\nMemory attached at %p\n", shm);
    // 设置共享内存
    stat = (struct Conn_stat *)shm;
    while (1)
    {
        if(stat->count > stat->count_max)
        {
            break;
        }
        printf("ip = %s ,count: %d\t\t\n", stat->ip, stat->count);
        sleep(1);
    }
    // 把共享内存从当前进程中分离
    if (shmdt(shm) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(2);
    }
    // 删除共享内存
    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "shmctl(IPC_RMID) failed, reason: %s\n", strerror(errno));
        exit(3);
    }
    return 0;
}
