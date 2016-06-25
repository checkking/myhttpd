// thread_safe_logger.h
// Copyright (c) 2016
// Thread safe logger class
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-19
#ifndef CHECKKING_MYHTTPD_SERVER_H
#define CHECKKING_MYHTTPD_SERVER_H

#include "logger.h"
#include <pthread.h>
#include <time.h>

namespace checkking {
namespace myhttpd {

bool logger_init(const char* log_dir, const char* module, const Level& log_level);

class ThreadSafeLogger : public Logger {
public:
    ThreadSafeLogger() {}
    ~ThreadSafeLogger() {
        log_close();
    }

    bool init(const char* file_path, const Level& log_level);
    void log(const Level& level, const char* logformat, ...);

private:
    void log_close() {
        if (_log_fp != NULL) {
            fflush(_log_fp);
            fclose(_log_fp);
            _log_fp = NULL;
        }
    }
    int make_log_prefix(char* str, const Level& level) {
        time_t now;
        now = time(&now);;
        struct tm vtm; 
        localtime_r(&now, &vtm);
        return snprintf(str, LOG_BUFFER_SIZE, "%s: %02d-%02d %02d:%02d:%02d ", level2string(level),
                vtm.tm_mon + 1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
    }

    bool write_log(const char* log_str, int log_size);

private:
    pthread_mutex_t _mutex;
    static __thread char _buff[LOG_BUFFER_SIZE];
};

extern ThreadSafeLogger WARN_LOGER;
extern ThreadSafeLogger INFO_LOGER;
} // namespace myhttpd
} // namespace checkking
#endif
