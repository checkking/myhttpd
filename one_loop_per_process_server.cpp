// one_loop_per_process_server.cpp
// Copyright (c) 2016
// A kind of http server class: One Loop per process.
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-19

#include "thread_safe_logger.h"
#include "logger_macro.h"

namespace checkking {
namespace myhttpd {

void OneLoopPerProcessServer::listen() {
    if (_server_sock == -1) {
        LOG_WARN("server socket [%d] not bined!", _server_sock);
        return;
    }
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);
    while (_running) {
        client_sock = ::accept(_server_sock, &client_name, &client_name_len);
        if (client_sock == -1) {
            LOG_FATAL("accept failed!");
        }
        handle_request(client_sock); // child
    }
}

int OneLoopPerProcessServer::read_request(int client_sock) {
    assert(client_sock > 0 && max_len > 0 && buff != NULL);
    char* buff_ptr = buff;
    int total_read = 0;
    int n = 0;
    while ((total_read < max_len) && (n = ::recv(client_sock, buff_ptr, max_len - total_read, 0)) > 0) {
        total_read += n;
        buff_ptr += n;
    }
    if (n < 0) {
        return -1;
    }

    return total_read;
}

void OneLoopPerProcessServer::handle_request(void* argv) {
    int client_sock = *((int *)(argv)); 
    _buff.clear();
    int read_len = read_request(client_sock);
    if (read_len <= 0) {
        close_client(client_sock);
    }
    // read one line to get method and url, if method is "GET", discard the other chars.
    OneLinePair line_pair;
    if (_buff.get_line(line_pair) > 0) {
        if (!parse_uri(line_pair)) {
            LOG_WARN("parse uri failed!");
            // FIXME:tell client close connection.
            close_client(client_sock);
            return;
        }
    } else {
        LOG_WARN("no method url line read.");
        // FIXME:tell client close connection.
        close_client(client_sock);
        return;
    }
    if (!dispatch_uri(client_sock)) {
        LOG_WARN("dispatch uri failed!");
    }
}

bool OneLoopPerProcessServer::parse_uri(const OneLinePair& line_pair) {
    if (line_pair.start_ptr >= line_pair.end_ptr) {
        return false;
    }
    bzero(&_method, sizeof(_method));
    bzero(&_url, sizeof(_url));
    const char* p = line_pair.start_ptr;
    const char* q = line_pair.end_ptr;
    // parse method
    int i = 0;
    while (i < METHOD_MAX_LEN && !isspace(*p) && p != q) {
        _method[i] = *p;
        ++i;
        ++p;
    }
    if (i == METHOD_MAX_LEN || p == q) {
        return false;
    }
    // parse url
    while (isspace(*p) && p != q) { 
        ++p;
    }
    if (p == q) {
        return false;
    }
    i = 0;
    while (i < URL_MAX_LEN &&  p < q) {
        _url[i++] = *(p++);
    }
    if (i == URL_MAX_LEN) {
        return false;
    }

    return true;
}

bool OneLoopPerProcessServer::dispatch_uri(int client) {
    if (_method == NULL || _url == NULL) {
        return false;
    }
    if (strcasecmp(_method, "GET") && strcasecmp(_method, "POST")) {
        return unimplemented(client, _mehtod, _url);
    }
    if (strcasecmp(_method, "GET") == 0) {
        return do_get((void *)(&client));
    }
    if (strcasecmp(_method, "POST") == 0) {
        return do_post((void *)(&client));
    }
}

bool OneLoopPerProcessServer::do_get(void *argv) {
    int client = *(int *)argv;
    char* query_string = _url;
    while (*query_string != '\0' && *query_string != '?') {
        ++query_string;
    }
    if (*query_string == '?') {
        *query_string = '\0';
        ++query_string;
        _cgi = true;
    }
    if (snprintf(_path, PATH_MAX_LEN + 1, "htdocs%s", _url) < 0) {
        return false;
    }
    
    if (_cgi) {
        return execute_cgi(client, _path, query_string);
    } else {
        return serve_file(client, _method, _path);
    }
}

void OneLoopPerProcessServer::do_post(void *argv) {
}

bool OneLoopPerProcessServer::discard_headers(int client) {
    return false;
    // read and discard headers
    OneLinePair line_pair;
    int n = 0;
    bool header_found = false;
    while (!header_found) {
        while ((n = _buff.get_line(line_pair)) > 0) {
            if (n == 1 && *(line_pair.start) == '\n') {
                header_found = true;
            }
        }
        if (!header_found) {
            n = _buff.read_socket(client);
            if (n == 0) {
                // FIXME:imcompleted header
                return  false;
            }
        }
    }
}

bool OneLoopPerProcessServer::serve_file(int client, const char *path) {
    bool r = discard_headers(client);
    if (!r) {
        return false;
    }
    FILE *file = NULL;
    file = fopen(path, "r");
    if (file == NULL) {
        return notfound(client);
    } else {
        headers(client, path);
        while (!feof(file)) {
            fgets(client, _buff_out, sizeof(_buff_out), file);
            ::send(client, _buff_out, strlen(_buff_out), 0);
        }
    }

    return true;
}

bool OneLoopPerProcessServer::execute_cgi(int client, const char* path, const char* query_string) {
    int cgi_output[2];
    int cgi_input[2];
    pid_t pid;
    int content_len = -1;
    bool header_found = false;

    if (strcasecmp(_method, "GET")) {
        if (!discard_headers(client)) {
            return false;
        }
    } else if (strcasecmp(_method, "POST")) {
        OneLinePair line_pair;
        int n = -1;
        while ((n = _buff.get_line(line_pair)) > 0) {
            if (n == 1 && *(line_pair.start) == '\n') {
                break;
            }
            *(line_pair.start + 15) == '\0';
            if (strcasecmp(line_pair.start, "Content-Lenght:") == 0) {
                content_len = atoi(line_pair.start + 16);
            }
        }
        if (content_len == -1) {
            return false;
        }
    } else {
        return false;
    }

    snprintf(_buff_out, sizeof (_buff_out), "HTTP/1.0 200 OK\r\n");
    ::send(client, _buff_out, strlen(_buff_out), 0);
    if (::pipe(cgi_output) < 0 || ::pipe(cgi_input) < 0) {
        internal_error(client);
        return false;
    }
    if ((pid = fork()) < 0) {
        internal_error(client);
        return false;
    }
    if (pid == 0) {
        // child, cgi script
        ::dup2(cgi_output[1], 1);
        ::dup2(cgi_input[0], 0);
        ::close(cgi_output[0]);
        ::close(cgi_input[1]);
        bzero(_env_var, sizeof(_env_var));
        snprintf(_env_var, sizeof(_env_var), "REQUEST_METHOD=%s", _method);
        putenv(_env_var);
        bzero(_env_var, sizeof(_env_var));
        if (strcasecmp(_method == "GET")) {
            snprintf(_env_var, sizeof(_env_var), "QUERY_STRING=%s", query_string);
            putenv(_env_var);
        } else if (strcasecmp(_method == "POST")) {
            snprintf(_env_var, sizeof(_env_var), "CONTENT_LENGTH=%d", content_len);
            putenv(_env_var);
        }
        ::execl(path, path, NULL);
        exit(0);
    } else if (pid > 0) {
        // parent
        ::close(cgi_output[1]);
        ::close(cgi_input[0]);
        char c;
        if (strcasecmp(_method == "POST")) {
            int i = 0;
            for (; i < content_len; ++i) {
                ::recv(client, &c, 1, 0);
                ::write(cgi_input[1], &c, 1);
            }
            while (::read(cgi_output[0], &c, 1, 0)) {
                ::send(client, &c, 1, 0);
            }
        }
        ::close(cgi_output[0]);
        ::close(cgi_input[1]);
        int status;
        waitpid(pid, &status, 0);
    }

    return true;
}

bool OneLoopPerProcessServer::close_client(int client_sock) {
    if (client_sock <= 0) {
        return false;
    }
    ::close(client_sock);
}
} // namespace myhttpd
} // namespace checkking
