#ifndef _ZXCVBSAMPLE_LOG_H_
#define _ZXCVBSAMPLE_LOG_H_

#include <stdio.h>

typedef enum {
    SAMPLE_LOG_MIN_       = -1,
    SAMPLE_LOG_EMERGENCY_ = 0,
    SAMPLE_LOG_ALERT_     = 1,
    SAMPLE_LOG_CRITICAL_  = 2,
    SAMPLE_LOG_ERROR_     = 3,
    SAMPLE_LOG_WARN_      = 4,
    SAMPLE_LOG_NOTICE_    = 5,
    SAMPLE_LOG_INFO_      = 6,
    SAMPLE_LOG_DEBUG_     = 7,
    SAMPLE_LOG_MAX_
} SAMPLE_LOG_LEVEL_E_;

#if defined(CONFIG_SAMPLE_LOG_LEVEL_)
static SAMPLE_LOG_LEVEL_E_ log_level_ = CONFIG_SAMPLE_LOG_LEVEL_;
#elif defined(CONFIG_SAMPLE_LOG_LEVEL_EXPORT_)
extern SAMPLE_LOG_LEVEL_E_ log_level_;
#else
static SAMPLE_LOG_LEVEL_E_ log_level_ = SAMPLE_LOG_INFO_;
#endif

#if 1
#define MACRO_BLACK  "\033[1;30;30m"
#define MACRO_RED    "\033[1;30;31m"
#define MACRO_GREEN  "\033[1;30;32m"
#define MACRO_YELLOW "\033[1;30;33m"
#define MACRO_BLUE   "\033[1;30;34m"
#define MACRO_PURPLE "\033[1;30;35m"
#define MACRO_WHITE  "\033[1;30;37m"
#define MACRO_END    "\033[0m"
#else
#define MACRO_BLACK
#define MACRO_RED
#define MACRO_GREEN
#define MACRO_YELLOW
#define MACRO_BLUE
#define MACRO_PURPLE
#define MACRO_WHITE
#define MACRO_END
#endif

#define SLOGE(fmt, ...) printf(MACRO_RED "[E][%32s][%4d]: " fmt MACRO_END "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define SLOGW(fmt, ...)               \
    if (log_level_ >= SAMPLE_LOG_WARN_) \
    printf(MACRO_YELLOW "[W][%s][%4d]: " fmt MACRO_END "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define SLOGI(fmt, ...)               \
    if (log_level_ >= SAMPLE_LOG_INFO_) \
    printf(MACRO_GREEN "[I][%s][%4d]: " fmt MACRO_END "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define SLOGD(fmt, ...)                \
    if (log_level_ >= SAMPLE_LOG_DEBUG_) \
    printf(MACRO_WHITE "[D][%s][%4d]: " fmt MACRO_END "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define SLOGN(fmt, ...)                 \
    if (log_level_ >= SAMPLE_LOG_NOTICE_) \
    printf(MACRO_PURPLE "[N][%s][%4d]: " fmt MACRO_END "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif /* _SAMPLE_LOG_H_ */
