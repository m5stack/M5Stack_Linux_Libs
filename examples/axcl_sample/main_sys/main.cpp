/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/


 #include <chrono>
 #include <iostream>
 #include <thread>
 #include "axcl.h"
  
 
 extern "C" axclError axclrtControlExecuteShellCmd(const char* cmd, const char* const args[], size_t argc, const char** output);
  
 static AXCL_DEVICE_STATUS_E device_status = AXCL_DEVICE_STATUS_OFFLINE;
  
 void device_status_callback(int32_t device_id, AXCL_DEVICE_STATUS_E status, void *userdata) {
      device_status = status;
      if (AXCL_DEVICE_STATUS_OFFLINE == status) {
          printf("device 0x%x is offline, rebooting ...\n", device_id);
         //  axclrtRebootDevice(device_id);
      } else if (AXCL_DEVICE_STATUS_ONLINE == status) {
          printf("device 0x%x is online, reboot success\n", device_id);
      }
  }
 
 void print_device_info(int device_id)
 {
     axclrtRegisterDeviceStatusCallback(device_status_callback, nullptr);
     axclrtSetDevice(device_id);
     const char *cmd = "free -m | grep 'Mem' ; top -b -n 1 | grep -e '^CPU' ; cat /proc/ax_proc/mem_cmm_info | grep 'total size' ; echo 'npu:'`cat /proc/ax_proc/npu/top | grep utilization`; echo 'temp:'`cat /sys/class/thermal/thermal_zone0/temp`; echo 'fun:'`cat /sys/class/thermal/cooling_device0/fan_cur_speed`";
     // top -b -n 1 | grep -e '^Mem' -e '^CPU'
     // cat /proc/ax_proc/mem_cmm_info | grep 'total size'
     // cat /proc/ax_proc/npu/top | grep utilization
     // cat /sys/class/thermal/thermal_zone0/temp
     // cat /sys/class/thermal/cooling_device0/fan_cur_speed
 
     const char *nihao = (char*)malloc(1024 * 1024);
     axclrtControlExecuteShellCmd(cmd, nullptr, 0, &nihao);
     printf("%s", nihao);
     printf("DEVICE END\n");
     axclrtResetDevice(device_id);
 }
 
 
 
 void m5stack_get_device_info()
 {
     axclrtDeviceList lst;
     axclrtGetDeviceList(&lst);
     for (uint32_t i = 0; i < lst.num; i++)
     {
         printf("AX8850:%d\n", i);
         print_device_info(lst.devices[i]);
     }
 }
 
  int main(int argc, char **argv) {
      if (axclError ret = axclInit(""); AXCL_SUCC != ret) {
          printf("axcl init fail, ret = 0x%x\n", ret);
          return 1;
      }
      m5stack_get_device_info(); 
      axclFinalize();
      return 0;
  }
  