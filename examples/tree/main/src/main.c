#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DIR_NAME_BUF_SIZE  128 //目录文件名buf长
#define PATH_BUF_SIZE 256  //完整路径名长度（包含file name)
#define NEST_COUNT 16 //目录递归层数

//关键buf，决定当前路径下的文件或目录是打印空白还是打印分支
static unsigned char nest_str[NEST_COUNT]; 

static void PrintTree(int nest, char *name) //树打印函数
{
    int i;
    for (i = 1; i <= nest; i++)
    {
if (i == nest)
{
    printf("|-- %s\n", name);
}
else
{
            if (nest_str[i] == 'y')
            {
                printf("|   ");
            }
            else
            {
                printf("    ");
            }
}
    }
}

static int isDir(char *filename)
{
    struct stat buf;
    if (stat(filename, &buf) < 0) //读文件失败
    {
        return 0;
    }
    
    if (S_ISDIR(buf.st_mode) ) //当前文件时目录文件
    {
        return 1;
    }
    else
    {
        return 0; //非目录文件
    }
}

static void Search(int nest, char *path)
{
    if (nest > NEST_COUNT) //目录递归深度达到规定的层数就返回
    {
        return;
    }
    
    //open direction
    DIR* dirp = opendir(path);
    
    struct dirent *dir = NULL;
    if (dirp == NULL)
    {
        return;
    }
    else
    {
         //read direction
        dir = readdir(dirp);
    }
    
    while (dir != NULL)
    {
        if (dir->d_name[0] == '.') //.和..文件忽略
        {
            dir = readdir(dirp);
        }
        else
        {
            char cur_dir_name[DIR_NAME_BUF_SIZE];
            memset(cur_dir_name, '\0', DIR_NAME_BUF_SIZE);
            if (strlen(dir->d_name) > (DIR_NAME_BUF_SIZE - 1))
            {
                break;
            }
            strcpy(cur_dir_name, dir->d_name);
            
            char cur_path[PATH_BUF_SIZE + 1];
            strcpy(cur_path, path);
            strcat(cur_path,"/");
            if (strlen(cur_path) + strlen(dir->d_name) > (PATH_BUF_SIZE - 1))
            {
                break;
            }
            strcat(cur_path, dir->d_name);
            int bDir = isDir(cur_path);
            
            PrintTree(nest, cur_dir_name);
            
            //The current folder is the last folder.
            if ((dir = readdir(dirp)) == NULL)
            {
                if (bDir == 1)
                {
                    nest_str[nest] = 'n';
                }
                else
                {
                    nest_str[nest] = 'y';
                }
            }
            else
            {
                nest_str[nest] = 'y';
            }
            
            if (bDir == 1)
            {
                Search(nest + 1, cur_path);
            }
        }
    }
    closedir(dirp);
}

int main(int argc, const char * argv[])
{
    memset(nest_str, 'y', NEST_COUNT);
    
    //get current direction
    char cur_path[PATH_BUF_SIZE];
    getcwd(cur_path, PATH_BUF_SIZE);
    
    printf(".\n");
    Search(1, cur_path);
    
    return 0;
}
