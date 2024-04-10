// client.c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include "comm.h"

char *file_n = "map.txt";

int creat_map_read()
{
  pid_t pid = fork();
  if (pid == -1)
  {
    perror("map_read fork error");
    exit(1);
  }
  else if (pid == 0)
  {
    printf("map_read The returned value is %d\nIn child process!!\nMy PID is %d\n", pid, getpid());
    map_read_main(file_n);
    printf("map_read over !\n");
    exit(0);
  }
  else
  {
    printf("map_read The returned value is %d\nIn father process!!\nMy PID is %d\n", pid, getpid());
    return pid;
  }
}

int creat_map_write()
{
  pid_t pid = fork();
  if (pid == -1)
  {
    perror("map_write fork error");
    exit(2);
  }
  else if (pid == 0)
  {
    printf("map_write The returned value is %d\nIn child process!!\nMy PID is %d\n", pid, getpid());
    map_write_main(file_n);
    printf("map_write over !\n");
    exit(0);
  }
  else
  {
    printf("map_write The returned value is %d\nIn father process!!\nMy PID is %d\n", pid, getpid());
    return pid;
  }
}

int main(int argc, char *argv[])
{
  int shmwrite = creat_map_write();
  int shmread = creat_map_read();
  waitpid(shmwrite, NULL, 0);
  waitpid(shmread, NULL, 0);
  return 0;
}