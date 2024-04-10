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

typedef struct PACKED
{
   uint16_t Controlword;
   uint32_t TargetPos;
   int32_t TargetVel;
   uint8_t ModeOp;
   uint8_t NONE;
   uint32_t acceleration_up;
   uint32_t acceleration_down;
} Drive_Outputs;

typedef struct PACKED
{
   uint16_t Statusword;
   uint32_t ActualPos;
   uint32_t ActualVel;
   uint8_t ModeOp;
   uint8_t NONE;
} Drive_Inputs;

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


void show_date(Drive_Inputs *iptr)
{
   printf("----------show status----------\n");
   printf("Statusword :%x\n", iptr->Statusword);
   printf("ActualPos  :%d\n", iptr->ActualPos);
   printf("ActualVel  :%d\n", iptr->ActualVel);
   printf("ModeOp     :%x\n", iptr->ModeOp);
}

int main(int argc, char *argv[])
{
   Drive_Inputs *iptr;
   Drive_Outputs *optr;

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

      // 电机初始化
      {
         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->ModeOp = 3;
         osal_usleep(1000 * 1000);
         optr->Controlword = 0x06;
         osal_usleep(1000 * 1000);
         optr->Controlword = 0x07;
         osal_usleep(1000 * 1000);
         optr->Controlword = 0x0f;
         osal_usleep(1000 * 1000);

         optr->acceleration_up = 0xffffff;
         optr->acceleration_down = 0xffffff;
         optr->TargetVel = 0;

         osal_usleep(6 * 1000);
         printf("-------------------------------------\n");
         show_date((Drive_Inputs *)ec_slave[0].inputs);
      }

      while (1)
      {
         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = 1;
         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = 0xffff;
         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = 0x1ffff;
         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = 0x5ffff;

         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = 1;

         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = 0;

         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);



         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = -1;
         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = -65535;
         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = -131071;
         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = -393215;
                           
         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = -1;
                           
         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

         optr = (Drive_Outputs *)ec_slave[0].outputs;
         iptr = (Drive_Inputs *)ec_slave[0].inputs;
         optr->TargetVel = 0;
                           
         osal_usleep(3000 * 1000);
         show_date((Drive_Inputs *)ec_slave[0].inputs);

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
