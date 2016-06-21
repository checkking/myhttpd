// logger.h
// Copyright (c) 2016
// Base logger class
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-18
#ifndef CHECKKING_MYHTTPD_SERVER_H
#define CHECKKING_MYHTTPD_SERVER_H

#include <pthread.h>

namespace checkking {
namespace myhttpd {

#define FILE_PATH_NAME_LEN 250
#define MOUDULE_NAME_LEN 50
#define LOG_BUFFER_SIZE 1024 * 1024 * 4

typedef enum Level {
    DEBUG = 1,
    TRACE,
    NOTICE,
    WARNNING,
    FATAL,
} Level;

class Logger : boost::noncopyable {
public:
    Logger(): _level(NOTICE), _log_fp(NULL) {
        _logfile_name[0] = '\0';
    }
    virtual ~Logger() {
    
    }
    virtual bool init(const char* file_path, const Level& log_level);
    virtual void finalize();
    virtual void log(LEVEL level, const char* logformat, ...);

procted:
    Level _level;
    FILE* _log_fp;
    char _logfile_name[FILE_PATH_NAME_LEN];

};
} // namespace myhttpd
} // namespace checkking
#endif
