// one_loop_per_process_server.h
// Copyright (c) 2016
// A kind of http server class: One Loop per process.
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-18
#ifndef CHECKKING_MYHTTPD_SERVER_H
#define CHECKKING_MYHTTPD_SERVER_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>    // for socket
#include <netinet/in.h>    // for sockaddr_in
#include <stdlib.h>
#include "thread_safe_logger.h"
#include "logger_macro.h"
#include "buffer.h"

namespace checkking {
namespace myhttpd {

#define METHOD_MAX_LEN 255
#define URL_MAX_LEN 255
#define PATH_MAX_LEN 255
#define OUT_PUT_BUFF_SIZE 1024
#define ENV_VAR_MAX_LEN 255

typedef struct OneLinePair {
    const char* start_ptr;
    const char* end_ptr;
} OneLinePair;

class OneLoopPerProcessServer : public BaseServer {
public:
    explicit OneLoopPerProcessServer(int port, const std::string& name = "OneLoopPerProcessServer")
            : BaseServer(port, name) {}
    ~OneLoopPerProcessServer() {
        assert(!_running);
        if (_server_sock > 0) {
            ::close(_server_sock);
            _server_sock = -1;
        }
    }

    // handle signal
    void handle_signal(int sig) {
        stop();
    }

    void start() {
        _running = true;
        startup();
        listen();
        _running = false;
    }

    void stop() {
        _running = false;
        pid_t pid;
        while ((pid = waitpid(-1, NULL, 0)) > 0) {
            LOG_INFO("waited pid=[%d]", pid);
        }
    }

protected:

    void startup() {
        struct sockaddr_in hostname;
        _server_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_server_sock < 0) {
            exit(1);
        }
        if (::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
            LOG_FATAL("signal error");
        }
        if (::signal(SIGCHLD, handle_signal) == SIG_ERR) {
            LOG_FATAL("signal error");
        }
        bzero(&hostname, sizeof(hostname));
        hostname.sin_family = AF_INET;
        hostname.sin_addr.s_addr = htonl(INADDR_ANY);
        hostname.sin_port = htons(*_port);
        if (bind(_server_sock, (struct sockaddr *)&hostname, sizeof(hostname)) < 0) {
            LOG_FATAL("bind socket failed!");
        }
    }

    void listen(int fd);
    void handle_request(void *);
    bool do_get(void *);
    bool do_post(void *);

private:
    int read_request(int client_sock);
    bool parse_uri(const OneLinePair& line_pair);
    bool close_client(int clinet_sock);
    void unimplemented(int client, const char* method);
    bool dispatch_uri(int client);
    bool execute_cgi(int cliet, const char* path, const char* query_string);
    bool serve_file(int client, const char* path);
    bool discard_headers(int client);

private:
    char _method[METHOD_MAX_LEN + 1];
    char _url[URL_MAX_LEN + 1];
    char _path[PATH_MAX_LEN + 1];
    Buffer _buff;
    char _buff_out[OUT_PUT_BUFF_SIZE];
    bool _cgi;
    char _env_var[ENV_VAR_MAX_LEN + 1];
};
} // namespace myhttpd
} // namespace checkking
#endif
