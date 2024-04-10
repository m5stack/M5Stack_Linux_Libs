/**********************************************************************************
 *
 * Copyright (c) 2019-2022 Beijing AXera Technology Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Beijing AXera Technology Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Beijing AXera Technology Co., Ltd.
 *
 **********************************************************************************/
#ifndef _RUN_TIME_H_
#define _RUN_TIME_H_
#include <time.h>
#include <stdio.h>
extern struct timespec _run_time_start;
extern struct timespec _run_time_end;

#define run_time_start() {clock_gettime(CLOCK_MONOTONIC, &_run_time_start);}

#define float_run_time_ms() ({clock_gettime(CLOCK_MONOTONIC, &_run_time_end);  (float)(((_run_time_end.tv_sec * 1000000 + _run_time_end.tv_nsec / 1000) - (_run_time_start.tv_sec * 1000000 + _run_time_start.tv_nsec / 1000)) / 1000.f);})

#define print_run_time_ms() ({clock_gettime(CLOCK_MONOTONIC, &_run_time_end);  printf("Process elapsed time :%f ms\n",(float)(((_run_time_end.tv_sec * 1000000 + _run_time_end.tv_nsec / 1000) - (_run_time_start.tv_sec * 1000000 + _run_time_start.tv_nsec / 1000)) / 1000.f));})


#endif /* _RUN_TIME_H_ */
