/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: user_log.h
 * ******************************************************************************
 * @brief 	: This is a simple log function，use JLINK RTT tool
 * @version	: 3.0
 * @date 	: 2024-08-01
 * @note 	: add file,func,line infomation
 * *
 * *
 * ******* 1. Improvement *********
 * * 1. add file,func,line infomation.
 * *
 * * 2. Change to will not show the full path,Only the file name is displayed,the log more concise.
 * *
 * *
 * ******* 2. Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******* 3. HOW TO USE *********
 * * 0. Include this file into your current project.
 * * 1. include the header file where needed.
 * * 2. Modify the macro definitions and enums in 'user_log.h'.
 * *
 * ******************************************************************************
 */
#ifndef __USER_LOG_H__
#define __USER_LOG_H__

#include "SEGGER_RTT.h"
#include <string.h>

/* ---------------------------- macro definition ---------------------------- */

#define LOG_LEVEL_CLOSE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_ERROR 3

/**
 * @brief the macro is used to debug log functions
 */
#define DEBUG_LOG_ENABLE LOG_LEVEL_DEBUG

/* ---------------------------- macro judge ---------------------------- */

#if defined(DEBUG_LOG_ENABLE) && (DEBUG_LOG_ENABLE >= LOG_LEVEL_DEBUG)

#define __LOG_FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)
#define LOG_SEGGER(type, color, format, ...)                           \
    SEGGER_RTT_printf(0, "%s%s[%4d][%20s][%20s][%4d]" format "\r\n%s", \
                      color,                                           \
                      type,                                            \
                      (int)HAL_GetTick() / 1000,                       \
                      __LOG_FILENAME__,                                \
                      __FUNCTION__,                                    \
                      __LINE__,                                        \
                      ##__VA_ARGS__,                                   \
                      RTT_CTRL_RESET)
#endif

#if (DEBUG_LOG_ENABLE == LOG_LEVEL_CLOSE)
#define LOG_DEBUG(format, ...)
#define LOG_INFO(format, ...)
#define LOG_ERROR(format, ...)

#elif (DEBUG_LOG_ENABLE == LOG_LEVEL_DEBUG)
#define LOG_DEBUG(format, ...) LOG_SEGGER("[D] ", RTT_CTRL_TEXT_WHITE, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG_SEGGER("[I] ", RTT_CTRL_TEXT_BRIGHT_GREEN, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_SEGGER("[E] ", RTT_CTRL_TEXT_BRIGHT_RED, format, ##__VA_ARGS__)

#elif (DEBUG_LOG_ENABLE == LOG_LEVEL_INFO)
#define LOG_DEBUG(format, ...)
#define LOG_INFO(format, ...) LOG_SEGGER("[INF] ", RTT_CTRL_TEXT_BRIGHT_GREEN, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_SEGGER("[ERR] ", RTT_CTRL_TEXT_BRIGHT_RED, format, ##__VA_ARGS__)

#elif (DEBUG_LOG_ENABLE == LOG_LEVEL_ERROR)
#define LOG_DEBUG(format, ...)
#define LOG_INFO(format, ...)
#define LOG_ERROR(format, ...) LOG_SEGGER("[ERR] ", RTT_CTRL_TEXT_BRIGHT_RED, format, ##__VA_ARGS__)

#else
#error "DEBUG_LOG_ENABLE:The macro is not defined correctly"

#endif

#endif
