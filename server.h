// base_server.h
// Copyright (c) 2016
// Base http server class
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-18
#ifndef CHECKKING_MYHTTPD_SERVER_H
#define CHECKKING_MYHTTPD_SERVER_H

namespace checkking {
namespace myhttpd {
class BaseServer : boost::noncopyable {
public:
    explicit BaseServer(int port, const std::string& name = "HttpServer")
            : _port(port), _name(name), _running(false) {}
    virtual ~BaseServer() {}

    virtual void start() {
        _running = true;
    }

    virtual void stop() {
        _running = false;
    }

protected:
    virtual void listen() {}
    virtual void handle_request(void *) {} 
    virtual void do_get(void *) {}
    virtual void do_post(void *) {}

protected:
    int _port;
    std::string _name;
    bool _running;
};
} // namespace myhttpd
} // namespace checkking
#endif
