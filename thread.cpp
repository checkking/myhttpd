// thread.cpp
// Copyright (c) 2016
// A wraper of pthread
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-18

#include "thread.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace checkking {
namespace myhttpd {


namespace CurrentThread {
__thread const char* thread_name = "main";

__thread pid_t t_cached_tid = 0;
pid_t tid() {
    if (t_cached_tid) {
        return t_cached_tid;
    }
    t_cached_tid = static_cast<pid_t>(::syscall(SYS_gettid));
    return t_cached_tid;
}
const char* name() {
    return thread_name;
}
bool is_main_trhead() {
    return tid() == ::getpid();
}
} // namespace CurrentThread

namespace {
struct ThreadData {
    typedef checkking::myhttpd::Thread::ThreadFunc ThreadFunc;
    ThreadFunc _func;
    std::string _name;

    ThreadData(const ThreadFunc& func, const std::string& name)
            : _func(func), _name(name)
    {
    }

    void runInThread() {
        pid_t tid = checkking::myhttpd::CurrentThread::tid();
        (void)tid;
        checkking::myhttpd::CurrentThread::thread_name = _name.c_str();
        _func();
        checkking::myhttpd::CurrentThread::thread_name = "finished";
    }
};

void* startThread(void* obj) {
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    
    return NULL;
}
}

void Thread::start() {
    assert(!_started);
    _started = true;
    ThreadData* data = new ThreadData(_func, _name);
    if (pthread_create(&_thread_id, NULL, &startThread, data)) {
        _started = false;
        delete data;
        abort();
    }
}

void Thread::join() {
    assert(_started);
    assert(!_joined);
    _joined = true;
    pthread_join(_thread_id, NULL);
}

} // namespace myhttpd
} // namespace checkking
