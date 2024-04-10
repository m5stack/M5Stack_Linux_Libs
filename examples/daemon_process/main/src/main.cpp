#if 1
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
 
using namespace std;
 
void daemond()
{
    // The parent process creates a child process, then exits, effectively detaching itself from the controlling terminal.
    pid_t pid = fork();
    if(pid > 0)
    {
        exit(0);
    }
 
    // The child process creates a new session.
    setsid();
    // Change the current directory to the root directory.
    chdir("/");
    // Set the umask mask to prevent certain permissions from being inherited from the parent process.
    umask(0);
    // Close file descriptors to conserve system resources.
    // `STDIN_FILENO` is the file descriptor for the standard input device, which is typically the keyboard.
    // Anything written to `/dev/null` will be discarded by the system.
    close(STDIN_FILENO);
    // 再一次的保护
    open("/dev/null",O_RDWR);
    dup2(0,1);
    dup2(0,2);
    // Run task
    while(1);
    // exit
    
}
 
int main(void)
{
 
    daemond();
 
    return 0;
}

#else
/*
创建守护进程时可以直接调用daemon接口进行创建
#include <unistd.h>
int daemon(int nochdir, int noclose);
如果参数nochdir为0，则将守护进程的工作目录该为根目录，否则不做处理。
如果参数noclose为0，则将守护进程的标准输入、标准输出以及标准错误重定向到/dev/null，否则不做处理。
*/
#include <unistd.h>
 
int main()
{
	daemon(0, 0);
	while (1);
	return 0;
}
#endif