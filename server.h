// base_server.h
// Copyright (c) 2016
// Base http server class
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-18
#ifndef CHECKKING_MYHTTPD_SERVER_H
#define CHECKKING_MYHTTPD_SERVER_H

#include <boost/noncopyable.hpp>

namespace checkking {
namespace myhttpd {
class BaseServer : boost::noncopyable {
public:
    explicit BaseServer(int port, const std::string& name = "HttpServer")
            : _port(port), 
             _server_sock(-1),
             _name(name), 
             _running(false) {}
    virtual ~BaseServer() {}

    virtual void start() = 0;

    virtual void stop() = 0;
    
    virtual void close() = 0;

protected:
    virtual void startup() = 0; 
    virtual void listen(int fd) = 0;
    virtual void handle_request(void *) = 0;
    virtual bool do_get(void *) = 0;
    virtual bool do_post(void *) = 0;

protected:
    int _port;
    int _server_sock;
    std::string _name;
    bool _running;
};
} // namespace myhttpd
} // namespace checkking
#endif
