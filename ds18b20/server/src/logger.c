/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  logger.c
 *    Description:  This log file 
 *                 
 *        Version:  1.0.0(2023年04月20日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月20日 10时31分40秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>     // va_stat 头文件
#include <errno.h>      // errno 头文件
#include <time.h>       // 时间结构体头文件
#include <sys/time.h>   // 时间函数头文件
#include <sys/stat.h>   // stat 头文件
#include "logger.h"

static LOG_FILE_S gstLogFile[5] = 
{
    {"", ""},
    {
        /* error级别 */
        LOG_PATH LOG_ERROR,                     // ./Log/log.error
        LOG_PATH LOG_ERROR LOG_OVERFLOW_SUFFIX  // ./Log/log.error00
    },
    {
        /* warn级别 */
        LOG_PATH LOG_WARN,                      // ./Log/log.warn
        LOG_PATH LOG_WARN LOG_OVERFLOW_SUFFIX   // ./Log/log.warn00
    }, 
    {
        /* info级别 */
        LOG_PATH LOG_INFO,                      // ./Log/log.info
        LOG_PATH LOG_INFO LOG_OVERFLOW_SUFFIX   // ./Log/log/info00
    }, 
    {
        /* debug级别 */
        LOG_PATH LOG_DEBUG,                     // ./Log/log.debug
        LOG_PATH LOG_DEBUG LOG_OVERFLOW_SUFFIX  // ./Log/log.debug00
    }, 
};

static void __Run_Log
(
    LOG_LEVEL_E enLogLevel,
    const char *pcFileName,
    const char *pcFuncName,
    int         iFileLine,
    const char *format,
    va_list     vargs
)
{
    FILE *logfile = NULL;
    logfile = fopen(gstLogFile[enLogLevel].szCurLog, "a");
    if (logfile == NULL)
    {
        printf("open %s error[%d](%s).\n", gstLogFile[enLogLevel].szCurLog, errno, strerror(errno));
        return;
    }

    /* 获取时间信息 */
    struct timeval stTimeVal = {0};
    struct tm stTime = {0};
    gettimeofday(&stTimeVal, NULL);
    localtime_r(&stTimeVal.tv_sec, &stTime);

    char buf[768];
    snprintf(buf, 768, "%.2d-%.2d %.2d:%.2d:%.2d.%.3lu [%s][%s:%d] ",
                                            stTime.tm_mon + 1,
                                            stTime.tm_mday,
                                            stTime.tm_hour,
                                            stTime.tm_min,
                                            stTime.tm_sec,
                                            (unsigned long)(stTimeVal.tv_usec / 1000),
                                            pcFileName,
                                            pcFuncName,
                                            iFileLine);

    fprintf(logfile, "%s", buf);
    vfprintf(logfile, format, vargs);
    fprintf(logfile, "%s", "\r\n");
    fflush(logfile);

    fclose(logfile);

    return;
}
static void __LogCoverStrategy(char *pcPreLog) // 日志满后的覆盖策略
{
    int iLen = strlen(pcPreLog);
    int iNum = (pcPreLog[iLen - 2] - '0') * 10 + (pcPreLog[iLen - 1] - '0');
    iNum = (iNum + 1) % 10;

    pcPreLog[iLen - 2] = iNum / 10 + '0';
    pcPreLog[iLen - 1] = iNum % 10 + '0';
}

void WriteLog
(
    LOG_LEVEL_E enLogLevel,
    const char *pcFileName,
    const char *pcFuncName,
    int         iFileLine,
    const char *format, 
    ...
)
{
    char szCommand[64]; // system函数中的指令
    struct stat statbuff;
    if (stat(gstLogFile[enLogLevel].szCurLog, &statbuff) >= 0) // 如果存在
    {
        if (statbuff.st_size > LOG_FILE_SIZE) // 如果日志文件超出限制
        {
            printf("LOGFILE(%s) > 5M, del it.\n", gstLogFile[enLogLevel].szCurLog);
            snprintf(szCommand, 134, "cp -f %s %s", gstLogFile[enLogLevel].szCurLog, gstLogFile[enLogLevel].szPreLog); 
            puts(szCommand);
            system(szCommand);      // 将当前超出限制的日志保存到 log.error00 中

            snprintf(szCommand, 128, "rm -f %s", gstLogFile[enLogLevel].szCurLog);
            system(szCommand);      // 删掉 log.error
            printf("%s\n\n", szCommand);
            
            // 如果 log.error 超出 5M 后，将依次保存在 log.error00、log.error01、... 中
            __LogCoverStrategy(gstLogFile[enLogLevel].szPreLog); 
        }
    }
    else // 如果不存在，则创建
    {
        printf("LOGFILE(%s) is not found, create it.\n\n", gstLogFile[enLogLevel].szCurLog);
        snprintf(szCommand, 128, "touch %s", gstLogFile[enLogLevel].szCurLog);
        system(szCommand);
    }

    va_list argument_list;
    va_start(argument_list, format);

    if (format)
    {
        __Run_Log(enLogLevel, pcFileName, pcFuncName, iFileLine, format, argument_list);
    }

    va_end(argument_list);

    return;
}

