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
bool logger_init(const char* log_dir, const char* module, const LEVEL& log_level) {
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
