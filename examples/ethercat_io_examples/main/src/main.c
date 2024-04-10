// #include <stdio.h>
// #include <sys/time.h>
// #include <unistd.h>

// #include "ethercattype.h"
// #include "nicdrv.h"
// #include "ethercatbase.h"
// #include "ethercatmain.h"
// #include "ethercatdc.h"
// #include "ethercatcoe.h"
// #include "ethercatfoe.h"
// #include "ethercatconfig.h"
// #include "ethercatprint.h"

// char IOmap[4096];
// // 使从站进入op状态
// void slavetop(int i)
// {
//         ec_slave[i].state = EC_STATE_OPERATIONAL;
//         ec_send_processdata();
//         ec_receive_processdata(EC_TIMEOUTRET);
//         ec_writestate(0);

// }
// void simpletest(char *ifname)
// {
//         int i,k = 1000,j=0;
//         if(ec_init(ifname))
//         {   
//                 printf("start ethernet at %s\n",ifname);
//                 if ( ec_config_init(FALSE) > 0 ) 
//                 {   
//                         printf("found %d slave on the bus\n",ec_slavecount);
//                         ec_config_map(&IOmap);
//                         for(i=0;i<ec_slavecount;i++)
//                         {   
//                                 printf("slave%d to op\n", i); 
//                                 slavetop(i);
//                         }   
//                         if(ec_slave[j].state == EC_STATE_OPERATIONAL)
//                         {   
//                                 while(k--)//控制运行周期数
//                                 {   
//                                     if(k < 500)
//                                         ec_slave[0].outputs[0x0000] = 0x0f;
//                                     else
//                                         ec_slave[0].outputs[0x0000] = 0x00;
//                                         ec_send_processdata();
//                                         ec_receive_processdata(EC_TIMEOUTRET);
//                                         usleep(5000);//发送周期
//                                 }   
//                         }   
//                         else
//                         {
//                                 slavetop(j);
//                                 printf("slave again to op\n");
//                         }
//                 }
//                 else
//                 {
//                         printf("no slave on the bus\n");
//                 }
//         }
//         else
//         {
//                 printf("no ethernet card\n");
//         }
// }
// int main(int argc, char *argv[])
// {
//         printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");

//         if (argc > 1)
//         {
//                 simpletest(argv[1]);
//                 // simpletest(argv[1]);
//                 // simpletest(argv[1]);
//         }
//         else
//         {
//                 printf("Usage: simple_test ifname1\nifname = eth0 for example\n");
//         }

//         printf("End program\n");
//         return (0);
// }




/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : simple_test [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * This is a minimal test.
 *
 * (c)Arthur Ketels 2010 - 2011   
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "ethercat.h"
#define EC_TIMEOUTMON 500

char IOmap[4096];  //  个人理解 这里就是给提供了一块内存空间
OSAL_THREAD_HANDLE thread1;      // demo 里面的一个 检测线程
OSAL_THREAD_HANDLE thread_freash;// 笔者自己加的  每5ms 就去刷新一下pdo 数据
int expectedWKC;
volatile int wkc;
boolean inOP;
uint8 currentgroup = 0;
// 状态监测线程，主要是监测ethcat 的异常状态？ 
// 这个线程笔者并没去深究，大概的用途就是去 定时的检测当前从站的在线状态，及时发现从站掉线
OSAL_THREAD_FUNC ecatcheck( void *ptr )
{
    int slave;
    (void)ptr;                  /* Not used */
    while(1)
    {
        if( inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
        {           
            /* one ore more slaves are not responding */
            ec_group[currentgroup].docheckstate = FALSE;
            ec_readstate();// 读取从站状态
            for (slave = 1; slave <= ec_slavecount; slave++) // 遍历
            {
               if ((ec_slave[slave].group == currentgroup) && (ec_slave[slave].state != EC_STATE_OPERATIONAL)) 
               {
                  ec_group[currentgroup].docheckstate = TRUE;
                  if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
                  {
                     printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
                     ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
                     ec_writestate(slave);  // 将指定的状态写到指定的从站之中
                  }
                  else if(ec_slave[slave].state == EC_STATE_SAFE_OP)
                  {
                     printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
                     ec_slave[slave].state = EC_STATE_OPERATIONAL;
                     ec_writestate(slave);
                  }
                  else if(ec_slave[slave].state > EC_STATE_NONE)
                  {
                     if (ec_reconfig_slave(slave, EC_TIMEOUTMON))
                     {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d reconfigured\n",slave);
                     }
                  }
                  else if(!ec_slave[slave].islost)
                  {
                     /* re-check state */
                     ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                     if (ec_slave[slave].state == EC_STATE_NONE)
                     {
                        ec_slave[slave].islost = TRUE;
                        printf("ERROR : slave %d lost\n",slave);
                     }
                  }
               }
               if (ec_slave[slave].islost) // 检查从站是否 离线？
               {
                  if(ec_slave[slave].state == EC_STATE_NONE)
                  {
                     if (ec_recover_slave(slave, EC_TIMEOUTMON)) // 恢复和从站之间的连接
                     {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d recovered\n",slave);
                     }
                  }
                  else
                  {
                     ec_slave[slave].islost = FALSE;
                     printf("MESSAGE : slave %d found\n",slave);
                  }
               }
            }
            if(!ec_group[currentgroup].docheckstate)
               printf("OK : all slaves resumed OPERATIONAL.\n");
        }
        osal_usleep(10000);
    }
}

// PDO 刷新线程，定时的去 收  发 PDO 数据。
OSAL_THREAD_FUNC ecatfreash( void *ptr )
{
   (void)ptr;         
   int i, oloop, iloop, chk;              /* Not used */
   inOP = FALSE;
   char *ifname = "enp3s0";
   if (ec_init(ifname))  // 初始化网卡1  基于野火的 i.mx6ull  这个是右面的那个网卡
   {
      printf("ec_init on %s succeeded.\n",ifname);
      /* find and auto-config slaves */

      if ( ec_config_init(FALSE) > 0 )  //  检查是否初始化完成了
      {
         printf("%d slaves found and configured.\n",ec_slavecount);

         ec_config_map(&IOmap); //根据 FMMU 的地址 完成数据的映射。

         ec_configdc();  //  配置时钟信息

         printf("Slaves mapped, state to SAFE_OP.\n");
         /* wait for all slaves to reach SAFE_OP state */
         ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);  // 检查当前的状态

         oloop = ec_slave[0].Obytes;
         if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
         if (oloop > 8) oloop = 8;
         iloop = ec_slave[0].Ibytes;
         if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
         if (iloop > 8) iloop = 8;

         printf("segments : %d : %d %d %d %d\n",ec_group[0].nsegments ,ec_group[0].IOsegment[0],ec_group[0].IOsegment[1],ec_group[0].IOsegment[2],ec_group[0].IOsegment[3]);

         printf("Request operational state for all slaves\n");
         expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
         printf("Calculated workcounter %d\n", expectedWKC);
         ec_slave[0].state = EC_STATE_OPERATIONAL;
         /* send one valid process data to make outputs in slaves happy*/
         ec_send_processdata();              
         ec_receive_processdata(EC_TIMEOUTRET);
         /* request OP state for all slaves */
         ec_writestate(0);
         chk = 200;
         /* wait for all slaves to reach OP state */
         do
         {
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
         }
         while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL)); //等待第一个从站进入到可以操作的状态 或者200个周期没进入 判定为超时
         if (ec_slave[0].state == EC_STATE_OPERATIONAL )
         {
            printf("Operational state reached for all slaves.\n");
            inOP = TRUE;
                /* cyclic loop */
            //for(i = 1; i <= 10000; i++)
            while(1)  // 笔者的改动之处 线程在这里进行无限的循环，每隔5ms 刷新一下pdo
            {
               ec_send_processdata();
               wkc = ec_receive_processdata(EC_TIMEOUTRET);  //
               osal_usleep(5000);                 
            }
               inOP = FALSE;
         }
         else
         {
               printf("Not all slaves reached operational state.\n");
               ec_readstate();
               for(i = 1; i<=ec_slavecount ; i++)
               {
                  if(ec_slave[i].state != EC_STATE_OPERATIONAL)
                  {
                     printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                           i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                  }
               }
         }
         printf("\nRequest init state for all slaves\n");
         ec_slave[0].state = EC_STATE_INIT;
         /* request INIT state for all slaves */
         ec_writestate(0);
      }
      else
      {
         printf("No slaves found!\n");
      }
   }
}

int main(int argc, char *argv[])
{  
   printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");
   if (argc > 1)
   {      
      /* create thread to handle slave error handling in OP */
      // pthread_create( &thread1, NULL, (void *) &ecatcheck, (void*) &ctime);
      osal_thread_create(&thread1, 128000, &ecatcheck, NULL);
      /* start cyclic part */
      // simpletest(argv[1]);      
      
      osal_thread_create(&thread_freash, 128000, &ecatfreash, NULL);
      // 这里需要等待一段时间，让刷新现成初始化好
      while( ec_slave[0].outputs == 0);  //  这个等待是必须的， 因为创建的子线程  ecatfreash  需要一段时间才能完成初始化工作
      while(1)
      {
        //  ec_slave[0].outputs[0] = ec_slave[0].inputs[0]; // 将第一个从站的 输入 等于输出
        //  // 显示的效果上面就是  我按下按键之后 从站上的灯就会亮起
        //  // 测试使用的从站模块是只有8个LED 和 8个 按键的模块。
        //  osal_usleep(5000);
        ec_slave[0].outputs[0x0000] = 0x0f;
        osal_usleep(1000 * 1000);
        ec_slave[0].outputs[0x0000] = 0xf0;
        osal_usleep(1000 * 1000);

      }              
   }else
   {  
      printf("Usage: simple_test ifname1\nifname = eth0 for example\n");
   }      
   ec_close();       
   printf("End program %s\n",argv[0]);
   return (0);
}
