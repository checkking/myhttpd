// thread.h
// Copyright (c) 2016
// A wraper of pthread
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-18
#ifndef CHECKKING_MYHTTPD_THREAD_H
#define CHECKKING_MYHTTPD_THREAD_H

#include <string>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <pthread.h>

namespace checkking {
namespace myhttpd {

class Thread : boost::noncopyable {
public:
    typedef boost::function<void ()> ThreadFunc;
    explicit Thread(const ThreadFunc& func, const std::string& name = std::string())
            : _started(false), _joined(false), _name(name), _func(func) {}
    ~Thread() {
        if (_started && !_joined) {
            pthread_detach(_thread_id);
        }
    }
    
    void start();
    void join();

private:
    bool _started;
    bool _joined;
    std::string _name;
    ThreadFunc _func;
    pthread_t _thread_id;
};

namespace CurrentThread {
pid_t tid();
const char* name();
bool is_main_thread();
} // namespace  CurrentThread
} // namespace myhttpd
} // namespace checkking

#endif
