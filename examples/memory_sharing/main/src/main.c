// client.c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include "comm.h"

int creat_shmread()
{
  pid_t pid = fork();
  if (pid == -1)
  {
    perror("shmread fork error");
    exit(1);
  }
  else if (pid == 0)
  {
    printf("shmread The returned value is %d\nIn child process!!\nMy PID is %d\n", pid, getpid());
    shmread_main();
    printf("shmread over !\n");
    exit(0);
  }
  else
  {
    printf("shmread The returned value is %d\nIn father process!!\nMy PID is %d\n", pid, getpid());
    return pid;
  }
}

int creat_shmwrite()
{
  pid_t pid = fork();
  if (pid == -1)
  {
    perror("shmwrite fork error");
    exit(2);
  }
  else if (pid == 0)
  {
    printf("shmwrite The returned value is %d\nIn child process!!\nMy PID is %d\n", pid, getpid());
    shmwrite_main();
    printf("shmwrite over !\n");
    exit(0);
  }
  else
  {
    printf("shmwrite The returned value is %d\nIn father process!!\nMy PID is %d\n", pid, getpid());
    return pid;
  }
}

int main(int argc, char *argv[])
{
  int shmwrite = creat_shmwrite();
  int shmread = creat_shmread();
  waitpid(shmwrite, NULL, 0);
  waitpid(shmread, NULL, 0);
  return 0;
}