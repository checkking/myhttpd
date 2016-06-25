// logger_macro.h
// Copyright (c) 2016
// Base http server class
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-19
#ifndef CHECKKING_MYHTTPD_LOGGER_MACRO_H
#define CHECKKING_MYHTTPD_LOGGER_MACRO_H

#define LOG_FATAL(log_fmt, log_arg...) \
    do { \
        WARN_LOGER.log(FATAL, "[%s:%d][%s]" log_fmt "\n", \
                __FILE__, __LINE__, __FUNCTION__, ##log_arg); \
        exit(1); \
    } while(0) 

#define LOG_WARN(log_fmt, log_arg...) \
    do { \
        WARN_LOGER.log(WARNNING, "[%s:%d][%s]" log_fmt "\n", \
                __FILE__, __LINE__, __FUNCTION__, ##log_arg); \
    } while(0) 

#define LOG_NOTICE(log_fmt, log_arg...) \
    do { \
        INFO_LOGER.log(NOTICE, "[%s:%d][%s]" log_fmt "\n", \
                __FILE__, __LINE__, __FUNCTION__, ##log_arg); \
    } while(0) 

#define LOG_TRACE(log_fmt, log_arg...) \
    do { \
        INFO_LOGER.log(TRACE, "[%s:%d][%s]" log_fmt "\n", \
                __FILE__, __LINE__, __FUNCTION__, ##log_arg); \
    } while(0) 

#define LOG_DEBUG(log_fmt, log_arg...) \
    do { \
        INFO_LOGER.log(DEBUG, "[%s:%d][%s]" log_fmt "\n", \
                __FILE__, __LINE__, __FUNCTION__, ##log_arg); \
    } while(0) 

#endif
