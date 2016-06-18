// one_loop_per_process_server.h
// Copyright (c) 2016
// A kind of http server class: One Loop per process.
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-18
#ifndef CHECKKING_MYHTTPD_SERVER_H
#define CHECKKING_MYHTTPD_SERVER_H

namespace checkking {
namespace myhttpd {


class OneLoopPerProcessServer : public BaseServer {
public:
    explicit OneLoopPerProcessServer(int port, const std::string& name = "OneLoopPerProcessServer"): BaseServer(port, name) {}
    ~OneLoopPerProcessServer() {
        assert(!_running);
    }

    // handle signal
    void handle_signal(int sig) {
        pid_t pid;
        while ((pid = waitpid(-1, NULL, 0)) > 0) {
             
        }
    }

private:

};

} // namespace myhttpd
} // namespace checkking
#endif
