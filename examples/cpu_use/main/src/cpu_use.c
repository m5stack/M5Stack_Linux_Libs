#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
typedef struct cpu_occupy_          
{
    char name[20];                  
    unsigned int user;             
    unsigned int nice;              
    unsigned int system;            
    unsigned int idle;              
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
}cpu_occupy_t;
 
double cal_cpuoccupy (cpu_occupy_t *o, cpu_occupy_t *n)
{
    double od, nd;
    double id, sd;
    double cpu_use ;
 
    od = (double) (o->user + o->nice + o->system +o->idle+o->softirq+o->iowait+o->irq);
    nd = (double) (n->user + n->nice + n->system +n->idle+n->softirq+n->iowait+n->irq);
 
    id = (double) (n->idle);    
    sd = (double) (o->idle) ;    
    if((nd-od) != 0)
        cpu_use =100.0 - ((id-sd))/(nd-od)*100.00; 
    else 
        cpu_use = 0;
    return cpu_use;
}
 
void get_cpuoccupy (cpu_occupy_t *cpust)
{
    FILE *fd;
    int n;
    char buff[256];
    cpu_occupy_t *cpu_occupy;
    cpu_occupy=cpust;
 
    fd = fopen ("/proc/stat", "r");
    if(fd == NULL)
    {
            perror("fopen:");
            exit (0);
    }
    fgets (buff, sizeof(buff), fd);
 
    sscanf (buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle ,&cpu_occupy->iowait,&cpu_occupy->irq,&cpu_occupy->softirq);
 
    fclose(fd);
}
 
double get_sysCpuUsage()
{
    cpu_occupy_t cpu_stat1;
    cpu_occupy_t cpu_stat2;
    double cpu;
    get_cpuoccupy((cpu_occupy_t *)&cpu_stat1);
    sleep(1);
    // Second time obtaining CPU usage.
    get_cpuoccupy((cpu_occupy_t *)&cpu_stat2);
 
    // Calculate CPU usage rate.
    cpu = cal_cpuoccupy ((cpu_occupy_t *)&cpu_stat1, (cpu_occupy_t *)&cpu_stat2);
 
    return cpu;
}
 
int main(int argc,char **argv)
{
    while(1)
    {
        printf("CPU useing :%f\n",get_sysCpuUsage());
    }
    return 0;
}

