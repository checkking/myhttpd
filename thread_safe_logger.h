// thread_safe_logger.h
// Copyright (c) 2016
// Thread safe logger class
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-19
#ifndef CHECKKING_MYHTTPD_SERVER_H
#define CHECKKING_MYHTTPD_SERVER_H

#include <pthread.h>

namespace checkking {
namespace myhttpd {

bool logger_init(const char* log_dir, const char* module, const LEVEL& log_level);

class ThreadSafeLogger : public Logger {
public:
    ThreadSafeLogger() {}
    ~ThreadSafeLogger() {}

    bool init(const char* file_path, const Level& log_level);
    void finalize();
    void log(LEVEL level, const char* logformat, ...);

private:
    pthread_mutex_t _mutext;
    static __thread char _buff[LOG_BUFFER_SIZE];
};

extern ThreadSafeLogger WARN_LOGER;
extern ThreadSafeLogger INFO_LOGER;
} // namespace myhttpd
} // namespace checkking
#endif
