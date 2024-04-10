/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : simple_test [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * This is a minimal test. used CL3-E57H
 *
 * (c)Arthur Ketels 2010 - 2011
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "ethercat.h"
#define EC_TIMEOUTMON 500


char IOmap[4096];                 //  个人理解 这里就是给提供了一块内存空间
OSAL_THREAD_HANDLE thread1;       // demo 里面的一个 检测线程
OSAL_THREAD_HANDLE thread_freash; // 笔者自己加的  每5ms 就去刷新一下pdo 数据
int expectedWKC;
volatile int wkc;
boolean inOP;
uint8 currentgroup = 0;

static int drive_write8(uint16 slave, uint16 index, uint8 subindex, uint8 value)
{
   int wkc;

   wkc = ec_SDOwrite(slave, index, subindex, FALSE, sizeof(value), &value, EC_TIMEOUTRXM);

   return wkc;
}

static int drive_write16(uint16 slave, uint16 index, uint8 subindex, uint16 value)
{
   int wkc;

   wkc = ec_SDOwrite(slave, index, subindex, FALSE, sizeof(value), &value, EC_TIMEOUTRXM);

   return wkc;
}

static int drive_write32(uint16 slave, uint16 index, uint8 subindex, int32 value)
{
   int wkc;

   wkc = ec_SDOwrite(slave, index, subindex, FALSE, sizeof(value), &value, EC_TIMEOUTRXM);

   return wkc;
}

// 该函数用于设置PDO映射表
int drive_setup(uint16 slave)
{
   int wkc = 0;
   printf("Drive setup:%d-----------------------------\n", slave);

   wkc += drive_write16(slave, 0x1C12, 0, 0);
   wkc += drive_write16(slave, 0x1C13, 0, 0);

   wkc += drive_write8(slave, 0x1600, 0, 0);
   wkc += drive_write32(slave, 0x1600, 1, 0x60400010); // Controlword
   wkc += drive_write32(slave, 0x1600, 2, 0x607A0020); // Target position
   wkc += drive_write32(slave, 0x1600, 3, 0x60FF0020); // Target velocity
   wkc += drive_write32(slave, 0x1600, 4, 0x60600008); // Modes of operation display
   wkc += drive_write32(slave, 0x1600, 5, 0x00000008);
   wkc += drive_write32(slave, 0x1600, 6, 0x60830020);
   wkc += drive_write32(slave, 0x1600, 7, 0x60830020);
   wkc += drive_write8(slave, 0x1600, 0, 7);


   wkc += drive_write16(slave, 0x1A00, 0, 0);
   wkc += drive_write32(slave, 0x1A00, 1, 0x60410010); // Statusword
   wkc += drive_write32(slave, 0x1A00, 2, 0x60640020); // Position actual value
   wkc += drive_write32(slave, 0x1A00, 3, 0x606C0020); // Velocity actual value
   wkc += drive_write32(slave, 0x1A00, 4, 0x60610008); // Modes of operation display
   wkc += drive_write32(slave, 0x1A00, 5, 0x00000008);
   wkc += drive_write8(slave, 0x1A00, 0, 5);

   wkc += drive_write16(slave, 0x1C12, 1, 0x1600);
   wkc += drive_write8(slave, 0x1C12, 0, 1);

   wkc += drive_write16(slave, 0x1C13, 1, 0x1A00);
   wkc += drive_write8(slave, 0x1C13, 0, 1);

   strncpy(ec_slave[slave].name, "Drive", EC_MAXNAME);

   if (wkc != 22)
   {
      printf("Drive %d setup failed\nwkc: %d\n", slave, wkc);
      return -1;
   }
   else
      printf("Drive %d setup succeed.\n", slave);

   return 0;
}

// 状态监测线程，主要是监测ethcat 的异常状态？
// 这个线程笔者并没去深究，大概的用途就是去 定时的检测当前从站的在线状态，及时发现从站掉线
OSAL_THREAD_FUNC ecatcheck(void *ptr)
{
   int slave;
   (void)ptr; /* Not used */
   while (1)
   {
      if (inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
      {
         /* one ore more slaves are not responding */
         ec_group[currentgroup].docheckstate = FALSE;
         ec_readstate();                                  // 读取从站状态
         for (slave = 1; slave <= ec_slavecount; slave++) // 遍历
         {
            if ((ec_slave[slave].group == currentgroup) && (ec_slave[slave].state != EC_STATE_OPERATIONAL))
            {
               ec_group[currentgroup].docheckstate = TRUE;
               if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
               {
                  printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
                  ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
                  ec_writestate(slave); // 将指定的状态写到指定的从站之中
               }
               else if (ec_slave[slave].state == EC_STATE_SAFE_OP)
               {
                  printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
                  ec_slave[slave].state = EC_STATE_OPERATIONAL;
                  ec_writestate(slave);
               }
               else if (ec_slave[slave].state > EC_STATE_NONE)
               {
                  if (ec_reconfig_slave(slave, EC_TIMEOUTMON))
                  {
                     ec_slave[slave].islost = FALSE;
                     printf("MESSAGE : slave %d reconfigured\n", slave);
                  }
               }
               else if (!ec_slave[slave].islost)
               {
                  /* re-check state */
                  ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                  if (ec_slave[slave].state == EC_STATE_NONE)
                  {
                     ec_slave[slave].islost = TRUE;
                     printf("ERROR : slave %d lost\n", slave);
                  }
               }
            }
            if (ec_slave[slave].islost) // 检查从站是否 离线？
            {
               if (ec_slave[slave].state == EC_STATE_NONE)
               {
                  if (ec_recover_slave(slave, EC_TIMEOUTMON)) // 恢复和从站之间的连接
                  {
                     ec_slave[slave].islost = FALSE;
                     printf("MESSAGE : slave %d recovered\n", slave);
                  }
               }
               else
               {
                  ec_slave[slave].islost = FALSE;
                  printf("MESSAGE : slave %d found\n", slave);
               }
            }
         }
         if (!ec_group[currentgroup].docheckstate)
            printf("OK : all slaves resumed OPERATIONAL.\n");
      }
      osal_usleep(10000);
   }
}

char ifname[50];
// PDO 刷新线程，定时的去 收  发 PDO 数据。
OSAL_THREAD_FUNC ecatfreash(void *ptr)
{
   (void)ptr;
   int i, oloop, iloop, chk; /* Not used */
   inOP = FALSE;
   // char *ifname = "enp3s0";
   // char *ifname = "eth0";
   if (ec_init(ifname)) // 初始化网卡1  基于野火的 i.mx6ull  这个是右面的那个网卡
   {
      printf("ec_init on %s succeeded.\n", ifname);
      /* find and auto-config slaves */

      if (ec_config_init(FALSE) > 0) //  检查是否初始化完成了
      {
         printf("%d slaves found and configured.\n", ec_slavecount);
         int slave_ix;
         for (slave_ix = 1; slave_ix <= ec_slavecount; slave_ix++)
         {
            ec_slavet *slave = &ec_slave[slave_ix];
            slave->PO2SOconfig = drive_setup;
         }

         ec_config_map(&IOmap); // 根据 FMMU 的地址 完成数据的映射。

         ec_configdc(); //  配置时钟信息

         printf("Slaves mapped, state to SAFE_OP.\n");
         /* wait for all slaves to reach SAFE_OP state */
         ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4); // 检查当前的状态

         oloop = ec_slave[0].Obytes;
         if ((oloop == 0) && (ec_slave[0].Obits > 0))
            oloop = 1;
         if (oloop > 8)
            oloop = 8;
         iloop = ec_slave[0].Ibytes;
         if ((iloop == 0) && (ec_slave[0].Ibits > 0))
            iloop = 1;
         if (iloop > 8)
            iloop = 8;

         printf("segments : %d : %d %d %d %d\n", ec_group[0].nsegments, ec_group[0].IOsegment[0], ec_group[0].IOsegment[1], ec_group[0].IOsegment[2], ec_group[0].IOsegment[3]);

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
         } while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL)); // 等待第一个从站进入到可以操作的状态 或者200个周期没进入 判定为超时
         if (ec_slave[0].state == EC_STATE_OPERATIONAL)
         {
            printf("Operational state reached for all slaves.\n");
            inOP = TRUE;
            /* cyclic loop */
            // for(i = 1; i <= 10000; i++)
            while (1) // 笔者的改动之处 线程在这里进行无限的循环，每隔5ms 刷新一下pdo
            {
               ec_send_processdata();
               wkc = ec_receive_processdata(EC_TIMEOUTRET); //
               osal_usleep(5000);
            }
            inOP = FALSE;
         }
         else
         {
            printf("Not all slaves reached operational state.\n");
            ec_readstate();
            for (i = 1; i <= ec_slavecount; i++)
            {
               if (ec_slave[i].state != EC_STATE_OPERATIONAL)
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
      strcpy(ifname, argv[1]);
      /* create thread to handle slave error handling in OP */
      // pthread_create( &thread1, NULL, (void *) &ecatcheck, (void*) &ctime);
      osal_thread_create(&thread1, 128000, &ecatcheck, NULL);
      /* start cyclic part */
      // simpletest(argv[1]);

      osal_thread_create(&thread_freash, 128000, &ecatfreash, NULL);
      // 这里需要等待一段时间，让刷新现成初始化好
      while (ec_slave[0].outputs == 0)
         ; //  这个等待是必须的， 因为创建的子线程  ecatfreash  需要一段时间才能完成初始化工作

      // int lens;
      // char bufkk[128];
      // ec_SDOread(1, 0x1600, 3, 0, &lens, bufkk, EC_TIMEOUTRXM);
      // printf("ec_SDOread:");
      // for (int i = 0; i < lens; i++)
      // {
      //    printf(" %x", bufkk[i]);
      // }
      // printf("\n");

      uint8_t ubuf8;
      uint16_t ubuf16;
      uint32_t ubuf32;
      int32_t buf32;

      ubuf8 = 3;
      // WRITE(0, 0x6060, 0, ubuf8, 3, "write work mode pv");
      ec_slave[0].outputs[0x000A] = 3;
      osal_usleep(1000 * 1000);

      // READ(0, 0x6060, 0, ubuf8, "read 0x6060");
      // ec_SDOwrite(0, 0x6060, 0, TRUE, sizeof(ubuf8), &ubuf8, EC_TIMEOUTRXM);
      for (int i = 0; i < 0x0017; i++)
      {
         printf("read %x data:%x\n", i, ec_slave[0].inputs[i]);
      }

      printf("-------------------------------------\n");

      ec_slave[0].outputs[0x0000] = 0x06;
      // WRITE(0, 0x6040, 0, buf1, 0x06, "start power with stop");
      osal_usleep(1000 * 1000);
      for (int i = 0; i < 0x0017; i++)
      {
         printf("read %x data:%x\n", i, ec_slave[0].inputs[i]);
      }

      printf("-------------------------------------\n");

      // WRITE(0, 0x6040, 0, buf1, 0x07, "wait");
      ec_slave[0].outputs[0x0000] = 0x07;
      osal_usleep(1000 * 1000);
      for (int i = 0; i < 0x0017; i++)
      {
         printf("read %x data:%x\n", i, ec_slave[0].inputs[i]);
      }

      printf("-------------------------------------\n");

      // WRITE(0, 0x6040, 0, buf1, 0x0f, "allow contral");
      ec_slave[0].outputs[0x0000] = 0x0f;
      osal_usleep(1000 * 1000);
      for (int i = 0; i < 0x0017; i++)
      {
         printf("read %x data:%x\n", i, ec_slave[0].inputs[i]);
      }

      printf("-------------------------------------\n");

      ec_slave[0].outputs[0x0006] = 0xff;
      ec_slave[0].outputs[0x0007] = 0xff;
      ec_slave[0].outputs[0x0008] = 0x05;
      // osal_usleep(1000 * 1000);
      for (int i = 0; i < 0x0017; i++)
      {
         printf("read %x data:%x\n", i, ec_slave[0].inputs[i]);
      }

      printf("-------------------------------------\n");

      ec_slave[0].outputs[0x000C] = 0xff;
      ec_slave[0].outputs[0x000E] = 0xff;
      ec_slave[0].outputs[0x000F] = 0xff;
      // ec_slave[0].outputs[0x0007] = 0xff;

      // ec_slave[0].outputs[0x0008] = 0x0f;
      // osal_usleep(1000 * 1000);

      for (int i = 0; i < 0x0017; i++)
      {
         printf("read %x data:%x\n", i, ec_slave[0].inputs[i]);
      }

      printf("-------------------------------------\n");

      ec_slave[0].outputs[0x0010] = 0xff;
      ec_slave[0].outputs[0x0011] = 0xff;
      ec_slave[0].outputs[0x0012] = 0xff;
      // ec_slave[0].outputs[0x0007] = 0xff;

      // ec_slave[0].outputs[0x0008] = 0x0f;
      osal_usleep(1000 * 1000);

      for (int i = 0; i < 0x0017; i++)
      {
         printf("read %x data:%x\n", i, ec_slave[0].inputs[i]);
      }

      printf("-------------------------------------\n");

      // // // WRITE(0, 0x60ff, 0, buf3, 100, "sipeed");
      // // // osal_usleep(1000 * 1000);

      // // // WRITE(0, 0x6083, 0, buf2, 100, "sipeed+");
      // // // osal_usleep(1000 * 1000);

      // // // WRITE(0, 0x6084, 0, buf2, 100, "sipeed-");
      // // // osal_usleep(1000 * 1000);

      // // CHECKERROR(0);

      while (1)
      {
         //  ec_slave[0].outputs[0] = ec_slave[0].inputs[0]; // 将第一个从站的 输入 等于输出
         //  // 显示的效果上面就是  我按下按键之后 从站上的灯就会亮起
         //  // 测试使用的从站模块是只有8个LED 和 8个 按键的模块。
         //  osal_usleep(5000);
         //   ec_slave[0].outputs[0x0000] = 0x0f;
         //   osal_usleep(1000 * 1000);
         //   ec_slave[0].outputs[0x0000] = 0xf0;
         //   osal_usleep(1000 * 1000);

         // uint8_t buf;
         // READ(0, 0x6060, 0, buf, "read 0x6060");
         // osal_usleep(1000 * 1000);
         // WRITE(0, 0x6060, 0, buf, 3, "write work mode pv");
         // osal_usleep(1000 * 1000);

         // uint16_t buf1;
         // WRITE(0, 0x6040, 0, buf1, 0x06, "start power with stop");

         // osal_usleep(1000 * 1000);

         // WRITE(0, 0x6040, 0, buf1, 0x07, "wait");
         // osal_usleep(1000 * 1000);

         // WRITE(0, 0x6040, 0, buf1, 0x0f, "allow contral");
         // osal_usleep(1000 * 1000);

         // READ(0, 0x6060, 0, buf, "read 0x6060");

         // printf("read 0x6041 data0:%x\n", ec_slave[0].inputs[0x000C]);
         // printf("read 0x6041 data1:%x\n", ec_slave[0].inputs[0x000D]);
         for (int i = 0; i < 0x0017; i++)
         {
            printf("read %x data:%x\n", i, ec_slave[0].inputs[i]);
         }

         printf("-------------------------------------\n");

         // WRITE(0, 0x6060, 0, buf, 3, "write work mode pv");

         // slave, ECT_SDO_SMCOMMTYPE, iSM + 1, FALSE, &rdl, &tSM, EC_TIMEOUTRXM

         // READ(0, 0x2000, 0, ubuf16, "read 0x2000");
         // CHECKERROR(0);
         osal_usleep(1000 * 1000);

         osal_usleep(4000 * 1000);

         ec_slave[0].outputs[0x0006] = 0xff;
         ec_slave[0].outputs[0x0007] = 0xff;
         ec_slave[0].outputs[0x0008] = 0x00;

         osal_usleep(5000 * 1000);

         ec_slave[0].outputs[0x0006] = 0xff;
         ec_slave[0].outputs[0x0007] = 0xff;
         ec_slave[0].outputs[0x0008] = 0x05;

         osal_usleep(5000 * 1000);
         ec_slave[0].outputs[0x0006] = 1;
         ec_slave[0].outputs[0x0007] = 0;
         ec_slave[0].outputs[0x0008] = 0;
      }
   }
   else
   {
      printf("Usage: simple_test ifname1\nifname = eth0 for example\n");
   }
   ec_close();
   printf("End program %s\n", argv[0]);
   return (0);
}











// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/time.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/time.h>
// #include <time.h>
// #define __USE_GNU
// #include <pthread.h>
// #include <math.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <termios.h> //PISOX中定义的标准接口

// #include "ethercat.h"

// #define NSEC_PER_SEC 1000000000
// #define EC_TIMEOUTMON 500

// #define EEP_MAN_SYNAPTICON (0x000022d2)
// #define EEP_ID_SYNAPTICON (0x00000201)

// struct sched_param schedp;
// char IOmap[4096];
// pthread_t thread1, thread2;
// uint64_t diff, maxt, avg, cycle;
// struct timeval t1, t2;
// int dorun = 0;
// int64 toff = 0;
// int expectedWKC;
// boolean needlf;
// volatile int wkc;
// boolean inOP;
// uint8 currentgroup = 0;

// enum {
//    STATE_RESET,
//    STATE_INIT,
//    STATE_PREREADY,
//    STATE_READY,
//    STATE_ENABLE,
//    STATE_DISABLE
// };

// int state = STATE_RESET;

// typedef struct PACKED
// {
//    uint16_t Controlword;
//    int16_t TargetTor;
//    int32_t TargetPos;
//    int32_t TargetVel;
//    uint8_t ModeOp;
//    int16_t TorOff;
// } Drive_Outputs;

// typedef struct PACKED
// {
//    uint16_t Statusword;
//    int32_t ActualPos;
//    int32_t ActualVel;
//    int16_t ActualTor;
//    uint8_t ModeOp;
//    int32_t SecPos;
// } Drive_Inputs;

// static int drive_write8(uint16 slave, uint16 index, uint8 subindex, uint8 value)
// {
//    int wkc;

//    wkc = ec_SDOwrite(slave, index, subindex, FALSE, sizeof(value), &value, EC_TIMEOUTRXM);

//    return wkc;
// }

// static int drive_write16(uint16 slave, uint16 index, uint8 subindex, uint16 value)
// {
//    int wkc;

//    wkc = ec_SDOwrite(slave, index, subindex, FALSE, sizeof(value), &value, EC_TIMEOUTRXM);

//    return wkc;
// }

// static int drive_write32(uint16 slave, uint16 index, uint8 subindex, int32 value)
// {
//    int wkc;

//    wkc = ec_SDOwrite(slave, index, subindex, FALSE, sizeof(value), &value, EC_TIMEOUTRXM);

//    return wkc;
// }

// // 该函数用于设置PDO映射表
// int drive_setup(uint16 slave)
// {
//    int wkc = 0;

//    printf("Drive setup:%d----------------------------------------------------------\n", slave);

//    wkc += drive_write16(slave, 0x1C12, 0, 0);
//    wkc += drive_write16(slave, 0x1C13, 0, 0);

//    wkc += drive_write16(slave, 0x1A00, 0, 0);
//    wkc += drive_write32(slave, 0x1A00, 1, 0x60410010); // Statusword
//    wkc += drive_write32(slave, 0x1A00, 2, 0x60640020); // Position actual value
//    wkc += drive_write32(slave, 0x1A00, 3, 0x606C0020); // Velocity actual value
//    wkc += drive_write32(slave, 0x1A00, 4, 0x60770010); // Torque actual value
//    wkc += drive_write32(slave, 0x1A00, 5, 0x60610008); // Modes of operation display
//    wkc += drive_write32(slave, 0x1A00, 6, 0x230A0020); // 2nd Pos
//    wkc += drive_write8(slave, 0x1A00, 0, 6);

//    wkc += drive_write8(slave, 0x1600, 0, 0);
//    wkc += drive_write32(slave, 0x1600, 1, 0x60400010); // Controlword
//    wkc += drive_write32(slave, 0x1600, 2, 0x60710010); // Target torque
//    wkc += drive_write32(slave, 0x1600, 3, 0x607A0020); // Target position
//    wkc += drive_write32(slave, 0x1600, 4, 0x60FF0020); // Target velocity
//    wkc += drive_write32(slave, 0x1600, 5, 0x60600008); // Modes of operation display
//    wkc += drive_write32(slave, 0x1600, 6, 0x60B20010); // Torque offset
//    wkc += drive_write8(slave, 0x1600, 0, 6);

//    wkc += drive_write16(slave, 0x1C12, 1, 0x1600);
//    wkc += drive_write8(slave, 0x1C12, 0, 1);

//    wkc += drive_write16(slave, 0x1C13, 1, 0x1A00);
//    wkc += drive_write8(slave, 0x1C13, 0, 1);

//    strncpy(ec_slave[slave].name, "Drive", EC_MAXNAME);

//    if (wkc != 22)
//    {
//       printf("Drive %d setup failed\nwkc: %d\n", slave, wkc);
//       return -1;
//    }
//    else
//       printf("Drive %d setup succeed.\n", slave);

//    return 0;
// }

// /* add ns to timespec */
// void add_timespec(struct timespec *ts, int64 addtime)
// {
//    int64 sec, nsec;

//    nsec = addtime % NSEC_PER_SEC;
//    sec = (addtime - nsec) / NSEC_PER_SEC;
//    ts->tv_sec += sec;
//    ts->tv_nsec += nsec;
//    if (ts->tv_nsec > NSEC_PER_SEC)
//    {
//       nsec = ts->tv_nsec % NSEC_PER_SEC;
//       ts->tv_sec += (ts->tv_nsec - nsec) / NSEC_PER_SEC;
//       ts->tv_nsec = nsec;
//    }
// }

// /* PI calculation to get linux time synced to DC time */
// void ec_sync(int64 reftime, int64 cycletime, int64 *offsettime)
// {
//    static int64 integral = 0;
//    int64 delta;
//    /* set linux sync point 50us later than DC sync, just as example */
//    delta = (reftime - 50000) % cycletime;
//    if (delta > (cycletime / 2))
//    {
//       delta = delta - cycletime;
//    }
//    if (delta > 0)
//    {
//       integral++;
//    }
//    if (delta < 0)
//    {
//       integral--;
//    }
//    *offsettime = -(delta / 100) - (integral / 20);
// }

// static inline int64_t calcdiff_ns(struct timespec t1, struct timespec t2)
// {
//    int64_t tdiff;
//    tdiff = NSEC_PER_SEC * (int64_t)((int)t1.tv_sec - (int)t2.tv_sec);
//    tdiff += ((int)t1.tv_nsec - (int)t2.tv_nsec);
//    return tdiff;
// }

// static int latency_target_fd = -1;
// static int32_t latency_target_value = 0;

// /* 消除系统时钟偏移函数，取自cyclic_test */
// static void set_latency_target(void)
// {
//    struct stat s;
//    int ret;

//    if (stat("/dev/cpu_dma_latency", &s) == 0)
//    {
//       latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
//       if (latency_target_fd == -1)
//          return;
//       ret = write(latency_target_fd, &latency_target_value, 4);
//       if (ret == 0)
//       {
//          printf("# error setting cpu_dma_latency to %d!: %s\n", latency_target_value, strerror(errno));
//          close(latency_target_fd);
//          return;
//       }
//       printf("# /dev/cpu_dma_latency set to %dus\n", latency_target_value);
//    }
// }

// void test_driver(char *ifname, int mode)
// {
//    int cnt, i, j ;
//    Drive_Inputs *iptr;
//    Drive_Outputs *optr;
//    struct sched_param schedp;
//    cpu_set_t mask;
//    pthread_t thread;
//    int ht;
//    int chk = 2000;
//    int64 cycletime;
//    struct timespec ts, tnow;

//    CPU_ZERO(&mask);
//    CPU_SET(2, &mask);
//    thread = pthread_self();
//    pthread_setaffinity_np(thread, sizeof(mask), &mask);

//    memset(&schedp, 0, sizeof(schedp));
//    schedp.sched_priority = 99; /* 设置优先级为99，即RT */
//    sched_setscheduler(0, SCHED_FIFO, &schedp);

//    printf("Starting Redundant test\n");

//    /* initialise SOEM, bind socket to ifname */
//    if (ec_init(ifname))
//    {
//       printf("ec_init on %s succeeded.\n", ifname);
//       /* find and auto-config slaves */
//       if (ec_config_init(FALSE) > 0)
//       {
//          printf("%d slaves found and configured.\n", ec_slavecount);
//          /* wait for all slaves to reach SAFE_OP state */

//          int slave_ix;
//          for (slave_ix = 1; slave_ix <= ec_slavecount; slave_ix++)
//          {
//             ec_slavet *slave = &ec_slave[slave_ix];
//             slave->PO2SOconfig = drive_setup;
//          }

//          /* configure DC options for every DC capable slave found in the list */
//          ec_config_map(&IOmap); // 此处调用drive_setup函数，进行PDO映射表设置

//          printf("------------------------------------------------\n");

//          ec_configdc(); // 设置同步时钟，该函数必须在设置pdo映射之后；

//          // setup dc for devices
//          for (slave_ix = 1; slave_ix <= ec_slavecount; slave_ix++)
//          {
//             ec_dcsync0(slave_ix, TRUE, 4000000U, 20000U);
//             // ec_dcsync01(slave_ix, TRUE, 4000000U, 8000000U, 20000U);
//          }

//          printf("Slaves mapped, state to SAFE_OP.\n");
//          ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE);

//          /* read indevidual slave state and store in ec_slave[] */
//          ec_readstate();
//          for (cnt = 1; cnt <= ec_slavecount; cnt++)
//          {
//             printf("Slave:%d Name:%s Output size:%3dbits Input size:%3dbits State:%2d delay:%d.%d\n",
//                    cnt, ec_slave[cnt].name, ec_slave[cnt].Obits, ec_slave[cnt].Ibits,
//                    ec_slave[cnt].state, (int)ec_slave[cnt].pdelay, ec_slave[cnt].hasdc);
//          }
//          expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
//          printf("Calculated workcounter %d\n", expectedWKC);

//          printf("Request operational state for all slaves\n");
//          /* activate cyclic process data */
//          /* wait for all slaves to reach OP state */
//          ec_slave[0].state = EC_STATE_OPERATIONAL;
//          /* request OP state for all slaves */
//          ec_writestate(0);

//          clock_gettime(CLOCK_MONOTONIC, &ts);
//          ht = (ts.tv_nsec / 1000000) + 1; /* round to nearest ms */
//          ts.tv_nsec = ht * 1000000;
//          cycletime = 4000 * 1000; /* cycletime in ns */

//          /* 对PDO进行初始化 */
//          for (i = 0; i < ec_slavecount; i++)
//          {
//             optr = (Drive_Outputs *)ec_slave[i + 1].outputs;
//             if(optr == NULL)
//             {
//                printf("optr is NULL.\n");
//             }
//             optr->Controlword = 0;
//             optr->TargetPos = 0;
//             optr->ModeOp = 0;
//             optr->TargetTor = 0;
//             optr->TargetVel = 0;
//             optr->TorOff = 0;
//          }

//          do
//          {
//             /* wait to cycle start */
//             clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
//             if (ec_slave[0].hasdc)
//             {
//                /* calulate toff to get linux time and DC synced */
//                ec_sync(ec_DCtime, cycletime, &toff);
//             }
//             wkc = ec_receive_processdata(EC_TIMEOUTRET);
//             ec_send_processdata();
//             add_timespec(&ts, cycletime + toff);
//          } while (chk-- && (wkc != expectedWKC));
//          /* 此处与SOEM官方例程不一样，因为ec_statecheck函数消耗的时间较多，有可能超过循环周期 */

//          if (wkc == expectedWKC)
//          {
//             printf("Operational state reached for all slaves.\n");
//             inOP = TRUE;
//             cnt = 0;
//             while (cnt<10)
//             {
//                /* 计算下一周期唤醒时间 */
//                add_timespec(&ts, cycletime + toff);
//                /* wait to cycle start */
//                clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
//                clock_gettime(CLOCK_MONOTONIC, &tnow);
//                if (ec_slave[0].hasdc)
//                {
//                   /* calulate toff to get linux time and DC synced */
//                   ec_sync(ec_DCtime, cycletime, &toff);
//                }
//                wkc = ec_receive_processdata(EC_TIMEOUTRET);
//                diff = calcdiff_ns(tnow, ts);

//                switch (state)
//                {
//                case STATE_RESET: /* 对驱动器清除故障 */
//                   for (i = 0; i < ec_slavecount; i++)
//                   {
//                      optr = (Drive_Outputs *)ec_slave[i + 1].outputs;
//                      optr->Controlword = 128;
//                   }
//                   state = 0;
//                   break;
//                case STATE_INIT /* 初始化驱动器 */:
//                   for (i = 0; i < ec_slavecount; i++)
//                   {
//                      optr = (Drive_Outputs *)ec_slave[i + 1].outputs;
//                      iptr = (Drive_Inputs *)ec_slave[i + 1].inputs;
//                      optr->Controlword = 0;
//                      optr->TargetPos = iptr->ActualPos;
//                   }
//                   state = STATE_PREREADY;
//                   break;
//                case STATE_PREREADY:
//                   for (i = 0; i < ec_slavecount; i++)
//                   {
//                      optr = (Drive_Outputs *)ec_slave[i + 1].outputs;
//                      optr->Controlword = 6;
//                   }
//                   state = STATE_READY;
//                   break;
//                case STATE_READY /* 系统转为准备使能状态 */:
//                   for (i = 0; i < ec_slavecount; i++)
//                   {
//                      optr = (Drive_Outputs *)ec_slave[i + 1].outputs;
//                      optr->Controlword = 7;
//                      optr->ModeOp = 8;
//                   }
//                   state = STATE_ENABLE;
//                   break;
//                case STATE_ENABLE /* 驱动器使能 */:
//                   for (i = 0; i < ec_slavecount; i++)
//                   {
//                      optr = (Drive_Outputs *)ec_slave[i + 1].outputs;
//                      optr->Controlword = 15;
//                   }
//                   break;
//                case STATE_DISABLE:
//                /* 使电机失能并在10个循环之后退出循环 */
//                   for (i = 0; i < ec_slavecount; i++)
//                   {
//                      optr = (Drive_Outputs *)ec_slave[i + 1].outputs;
//                      optr->ModeOp = 0;
//                      optr->TargetVel = 0;
//                      optr->Controlword = 6;
//                   }
//                   cnt ++;
//                   break;
//                default:
//                   break;
//                }
//                ec_send_processdata();
//                cycle++;

//                avg += diff;
//                if (diff > maxt)
//                   maxt = diff;

//                for (j = 0; j < 1; j++)
//                {
//                   iptr = (Drive_Inputs *)ec_slave[j + 1].inputs;
//                   optr = (Drive_Outputs *)ec_slave[j + 1].outputs;
//                   printf("  %d: CW: %d, status: %d, pos: %d", j + 1, optr->Controlword, iptr->Statusword, iptr->ActualPos);
//                }
//                printf(", MaxLatency: %lu, avg: %lu    \r", maxt, avg / cycle);
//                fflush(stdout);
//             }
//             dorun = 0;
//          }
//          else /* ECAT进入OP失败 */
//          {
//             printf("Not all slaves reached operational state.\n");
//             ec_readstate();
//             for (i = 1; i <= ec_slavecount; i++)
//             {
//                if (ec_slave[i].state != EC_STATE_OPERATIONAL)
//                {
//                   printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
//                          i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
//                }
//             }
//          }
//          /* 断开ECAT通讯 */
//          printf("\nRequest safe operational state for all slaves\n");
//          ec_slave[0].state = EC_STATE_SAFE_OP;
//          /* request SAFE_OP state for all slaves */
//          ec_writestate(0);
//          ec_slave[0].state = EC_STATE_PRE_OP;
//          ec_writestate(0);
//          ec_slave[0].state = EC_STATE_INIT;
//          ec_writestate(0);
//          ec_readstate();
//          if (ec_statecheck(0, EC_STATE_SAFE_OP, 1000) == EC_STATE_INIT)
//          {
//             printf("ECAT changed into state init\n");
//          }
//       }
//       else
//       {
//          printf("No slaves found!\n");
//       }
//       printf("End driver test, close socket\n");
//       /* stop SOEM, close socket */
//       ec_close();
//    }
//    else
//    {
//       printf("No socket connection on %s\nExcecute as root\n", ifname);
//    }
// }

// // 检测键盘输入，如检测到esc即关闭SOEM退出程序
// OSAL_THREAD_FUNC scanKeyboard()
// {
//    int in;
//    // int i;
//    // Drive_Outputs *optr;
//    struct sched_param schedp;
//    cpu_set_t mask;
//    pthread_t thread;
//    struct termios new_settings;
//    struct termios stored_settings;

//    CPU_ZERO(&mask);
//    CPU_SET(2, &mask);
//    thread = pthread_self();
//    pthread_setaffinity_np(thread, sizeof(mask), &mask);
//    memset(&schedp, 0, sizeof(schedp));
//    schedp.sched_priority = 20;
//    sched_setscheduler(0, SCHED_FIFO, &schedp);

//    tcgetattr(0, &stored_settings);
//    new_settings = stored_settings;
//    new_settings.c_lflag &= (~ICANON); //屏蔽整行缓存
//    new_settings.c_cc[VTIME] = 0;

//    /*这个函数调用把当前终端接口变量的值写入termios_p参数指向的结构。
//    如果这些值其后被修改了，你可以通过调用函数tcsetattr来重新配置
//    调用tcgetattr初始化一个终端对应的termios结构
//    int tcgetattr(int fd, struct termios *termios_p);*/
//    tcgetattr(0, &stored_settings);
//    new_settings.c_cc[VMIN] = 1;

//    /*int tcsetattr(int fd , int actions , const struct termios *termios_h)
//    参数actions控制修改方式，共有三种修改方式，如下所示。
//    1.TCSANOW：立刻对值进行修改
//    2.TCSADRAIN：等当前的输出完成后再对值进行修改。
//    3.TCSAFLUSH：等当前的输出完成之后，再对值进行修改，但丢弃还未从read调用返回的当前的可用的任何输入。*/
//    tcsetattr(0, TCSANOW, &new_settings);
//    in = getchar();
//    tcsetattr(0, TCSANOW, &stored_settings);
//    while (1)
//    {
//       if (in == 27)
//       {
//          state = STATE_DISABLE;
//          printf("the keyboard input is: \n");
//          putchar(in);
//          break;
//       }
//       osal_usleep(10000); //间隔10ms循环一次;
//    }
// }

// OSAL_THREAD_FUNC ecatcheck(void *ptr)
// {
//    int slave;
//    (void)ptr; /* Not used */
//    struct sched_param schedp;
//    cpu_set_t mask;
//    pthread_t thread;
//    time_t terr;

//    /* 设定线程优先级为20 */
//    CPU_ZERO(&mask);
//    CPU_SET(2, &mask);
//    thread = pthread_self();
//    pthread_setaffinity_np(thread, sizeof(mask), &mask);

//    memset(&schedp, 0, sizeof(schedp));
//    schedp.sched_priority = 21;
//    sched_setscheduler(0, SCHED_FIFO, &schedp);

//    while (1)
//    {
//       if (inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
//       {
//          time(&terr);
//          printf("wkc: %d, expwkc: %d, docheckstate: %d, error time: %s\n", wkc, expectedWKC, ec_group[0].docheckstate, ctime(&terr));
//          if (needlf)
//          {
//             needlf = FALSE;
//             printf("\n");
//          }
//          /* one ore more slaves are not responding */
//          ec_group[currentgroup].docheckstate = FALSE;
//          ec_readstate();
//          for (slave = 1; slave <= ec_slavecount; slave++)
//          {
//             if ((ec_slave[slave].group == currentgroup) && (ec_slave[slave].state != EC_STATE_OPERATIONAL))
//             {
//                ec_group[currentgroup].docheckstate = TRUE;
//                if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
//                {
//                   printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
//                   ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
//                   ec_writestate(slave);
//                }
//                else if (ec_slave[slave].state == EC_STATE_SAFE_OP)
//                {
//                   printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
//                   ec_slave[slave].state = EC_STATE_OPERATIONAL;
//                   ec_writestate(slave);
//                }
//                else if (ec_slave[slave].state > EC_STATE_NONE)
//                {
//                   if (ec_reconfig_slave(slave, EC_TIMEOUTMON))
//                   {
//                      ec_slave[slave].islost = FALSE;
//                      printf("MESSAGE : slave %d reconfigured\n", slave);
//                   }
//                }
//                else if (!ec_slave[slave].islost)
//                {
//                   /* re-check state */
//                   ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
//                   if (ec_slave[slave].state == EC_STATE_NONE)
//                   {
//                      ec_slave[slave].islost = TRUE;
//                      printf("ERROR : slave %d lost\n", slave);
//                   }
//                }
//             }
//             if (ec_slave[slave].islost)
//             {
//                if (ec_slave[slave].state == EC_STATE_NONE)
//                {
//                   if (ec_recover_slave(slave, EC_TIMEOUTMON))
//                   {
//                      ec_slave[slave].islost = FALSE;
//                      printf("MESSAGE : slave %d recovered\n", slave);
//                   }
//                }
//                else
//                {
//                   ec_slave[slave].islost = FALSE;
//                   printf("MESSAGE : slave %d found\n", slave);
//                }
//             }
//          }
//          if (!ec_group[currentgroup].docheckstate)
//             printf("OK : all slaves resumed OPERATIONAL.\n");
//       }
//       osal_usleep(10000);
//    }
// }

// #define stack64k (64 * 1024)

// int main(int argc, char *argv[])
// {
//    int mode;

//    printf("SOEM (Simple Open EtherCAT Master)\nRedundancy test\n");
//    if (argc > 1)
//    {
//       dorun = 0;

//       set_latency_target(); // 消除系统时钟偏移

//       /* create thread to handle slave error handling in OP */
//       osal_thread_create(&thread2, stack64k * 4, &ecatcheck, NULL);
//       osal_thread_create(&thread1, stack64k * 4, &scanKeyboard, NULL);

//       /* start acyclic part */
//       test_driver(argv[1], mode);
//    }
//    else
//    {
//       printf("Usage: red_test ifname1 Mode_of_operation\nifname = eth0 for example\n");
//    }

//    printf("End program\n");

//    return (0);
// }

InitGUI():

     /*initialize default window process*/

    __mg_def_proc[0] = PreDefMainWinProc;

    __mg_def_proc[1] = PreDefDialogProc;

    __mg_def_proc[2] = PreDefControlProc;

 mg_InitFixStr() 

mg_InitMisc ()

    mg_InitGAL ()//here to get width an height

    mg_InitSystemRes ()

    mg_InitGDI()    

    mg_InitScreenDC (__gal_screen)

    license_create();

    splash_draw_framework();

mg_InitCursor()

    mg_InitLWEvent()     

    mg_InitLFManager ()     

    mg_InitMenu ()   

mg_InitControlClass()     

    img_InitAccel()

    mg_InitDesktop ()

    mg_InitFreeQMSGList ()

    createThreadInfoKey ()

    SystemThreads()// initial eventloop ,run select to get input data

    SetKeyboardLayout ("default")

    SetCursor (GetSystemCursor (IDC_ARROW))

    mg_TerminateMgEtc ()

return 0;
