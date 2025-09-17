
/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SAMPLE_LOGGER_H__
#define __SAMPLE_LOGGER_H__

#include <stdio.h>

#if 1
#define COLOR_BLACK     "\033[1;30;30m"
#define COLOR_RED       "\033[1;30;31m"
#define COLOR_GREEN     "\033[1;30;32m"
#define COLOR_YELLOW    "\033[1;30;33m"
#define COLOR_BLUE      "\033[1;30;34m"
#define COLOR_PURPLE    "\033[1;30;35m"
#define COLOR_WHITE     "\033[1;30;37m"
#define COLOR_DEFAULT   "\033[0m"
#else
#define COLOR_BLACK
#define COLOR_RED
#define COLOR_GREEN
#define COLOR_YELLOW
#define COLOR_BLUE
#define COLOR_PURPLE
#define COLOR_WHITE
#define COLOR_DEFAULT
#endif

#define SAMPLE_LOG_E(fmt, ...) printf(COLOR_RED    "[FAIL ][%32s][%4d]: " fmt COLOR_DEFAULT "\n", __func__, __LINE__, ##__VA_ARGS__)
#define SAMPLE_LOG_W(fmt, ...) printf(COLOR_YELLOW "[WARN ][%32s][%4d]: " fmt COLOR_DEFAULT "\n", __func__, __LINE__, ##__VA_ARGS__)
#define SAMPLE_LOG_I(fmt, ...) printf(COLOR_GREEN  "[INFO ][%32s][%4d]: " fmt COLOR_DEFAULT "\n", __func__, __LINE__, ##__VA_ARGS__)
#define SAMPLE_LOG_D(fmt, ...) printf(COLOR_WHITE  "[DEBUG][%32s][%4d]: " fmt COLOR_DEFAULT "\n", __func__, __LINE__, ##__VA_ARGS__)

#endif /* __SAMPLE_LOGGER_H__ */