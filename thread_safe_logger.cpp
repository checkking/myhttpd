// thread_safe_logger.h
// Copyright (c) 2016
// Thread safe logger class
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-19
#ifndef CHECKKING_MYHTTPD_SERVER_H
#define CHECKKING_MYHTTPD_SERVER_H

#include <unistd.h>
#include <stdio.h>

namespace checkking {
namespace myhttpd {
__thread char ThreadSafeLogger::_buff[LOG_BUFFER_SIZE];

bool ThreadSafeLogger::init(const char* file_path, const Level& log_level) {
    assert(access(file_path, W_OK) == 0);
    assert(_log_fp == NULL);
    _level = log_level;
    snprintf(_logfile_name, file_path, sizeof(_logfile_name));
    _logfile_name[sizeof(_logfile_name) - 1] = '\0';
    if (_logfile_name[0] == '\0') {
        _log_fp = stdout;
        fprintf(stderr, "no log file configured, log to stdout!\n");
    } else {
        _log_fp = fopen(_logfile_name, "a");
        if (_log_fp == NULL) {
            fprintf(stderr, "cannot open log file [%s]\n", _logfile_name);
            return false;
        }
    }
    pthread_mutex_init(&m_mutex, NULL);
    return true;
}

void ThreadSafeLogger::log(const Level& level, const char* logformat, ...) {
    if (!check_level(level) || logformat == NULL) {
        fprintf(stderr, "check level or logformat failed!\n");
        return;
    }
    char* p_buff = _buff;
    int prefix_len = make_log_prefix(p_buff, level);
    if (prefix_len > 0) {
        p_buff += prefix_len;
    }

    va_list args;
    va_start(args, logformat);
    int size = vsnprintf(p_buff, LOG_BUFFER_SIZE - prefix_len, logformat, args);
    va_end(args);
    write_log(_buff, prefix_len + size);
}

bool ThreadSafeLogger::write_log(const char* log_str, int log_size) {
    if (log_str == NULL || log_size == 0) {
        return false;
    }
    if (0 != access(_logfile_name, W_OK)) {
        pthread_mutex_lock(&_mutex);
        if (0 != access(_logfile_name, W_OK)) {
            log_close();
            if (!init(_logfile_name, _level)) {
                fprintf(stderr, "init logger failed!");
                pthread_mutex_unlock(&_mutex);
                return false;
            }
        }
        pthread_mutex_unlock(&_mutex);
    }

    pthread_mutex_lock(&_mutex);
    if (1 == fwrite(log_str, log_size, 1, _log_fp)) {
        fflush(_log_fp);
        *log_str = '\0';
    } else {
        int x = errno;
        fprintf(stderr, "Failed to write to logfile. errno:%s    message:%s", strerror(x), log_str);
        pthread_mutex_unlock(&_mutex);
        return false;
    }
    pthread_mutex_unlock(&_mutex);

    return true;
}

bool logger_init(const char* log_dir, const char* module, const Level& log_level) {
    if (access(log_dir, F_OK) == -1) {
        // log dir not exist, mkdir it
        if (mkdir (p_logdir, S_IREAD | S_IWRITE ) < 0) {
            fprintf(stderr, "create folder [%s] failed!\n", p_logdir);
            return false;
        }
    } else if (access(log_dir, W_OK) == -1) {
        // has no write premission
        fprintf(stderr, "have no write access to [%s]!\n", p_logdir);
        return false;
    } 
    char log_file[FILE_PATH_NAME_LEN];
    snprintf(log_file, FILE_PATH_NAME_LEN, "%s/%s.access", log_dir, module);
    INFO_LOGER.init(log_file, log_level);
    snprintf(log_file, FILE_PATH_NAME_LEN, "%s/%s.error", log_dir, module);
    if (log_level <= WARNNING) {
        log_level = WARNNING;
    }
    if (!WARN_LOGER.init(log_file, log_level)) {
        return false;
    }

    return true;
}

} // namespace myhttpd
} // namespace checkking
#endif
