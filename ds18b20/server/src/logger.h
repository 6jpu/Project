/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  logger.h
 *    Description:  This log head file 
 *
 *        Version:  1.0.0(2023年04月20日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月20日 10时34分53秒"
 *                 
 ********************************************************************************/
#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifdef __cplusplus
extern "C"
{
#endif

// 日志路径
#define LOG_PATH              "./Log/"
#define LOG_ERROR             "log.error"
#define LOG_WARN              "log.warn"
#define LOG_INFO              "log.info"
#define LOG_DEBUG             "log.debug"
#define LOG_OVERFLOW_SUFFIX             "00"    // 日志溢出后的文件后缀，如 log.error00

#define LOG_FILE_SIZE  (5*1024*1024)            // 单个日志文件的大小，5M

// 日志级别
typedef enum tagLogLevel
{
    LOG_LEVEL_ERROR    = 1,                             /* error级别 */
    LOG_LEVEL_WARN     = 2,                             /* warn级别  */
    LOG_LEVEL_INFO     = 3,                             /* info级别  */
    LOG_LEVEL_DEBUG    = 4,                             /* debug级别 */
} LOG_LEVEL_E;

typedef struct tagLogFile
{
    char szCurLog[64];
    char szPreLog[64];
} LOG_FILE_S;

#define PARSE_LOG_ERROR(format, args...)  \
    WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, format, ##args)

#define PARSE_LOG_WARN(format, args...)  \
    WriteLog(LOG_LEVEL_WARN, __FILE__, __FUNCTION__, __LINE__, format, ##args)

#define PARSE_LOG_INFO(format, args...)  \
    WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, format, ##args)

#define PARSE_LOG_DEBUG(format, args...)  \
    WriteLog(LOG_LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__, format, ##args)

extern void WriteLog
(
    LOG_LEVEL_E enLogLevel,
    const char *pcFileName,
    const char *pcFuncName,
    int         iFileLine,
    const char *format, 
    ...
);

#ifdef __cplusplus
}
#endif

#endif

