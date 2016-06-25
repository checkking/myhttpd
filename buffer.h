// buffer.h
// Copyright (c) 2016
// A fixed size buffer class
// 
// Author: checkking (checkking at foxmail dot com)
// Date: 2016-06-22
#ifndef CHECKKING_MYHTTPD_BUFFER_H
#define CHECKKING_MYHTTPD_BUFFER_H

#include <boost/noncopyable.hpp>

namespace checkking {
namespace myhttpd {

#define BUFFER_SIZE 1024

// [start, end)
typedef struct OneLinePiar {
    const char* start;
    const char* end;
} OneLinePiar;

class Buffer : boost::noncopyable {
public:
    Buffer(): _ridx(0), _widx(0) {
        bzero(_buff);
    }
    ~Buffer() {}

    int read_socket(int fd) {
        char* buff_ptr = &_buff[_widx];
        int total_read = 0;
        int n = 0;
        while (writable_size() > 0 && (n = ::recv(fd, buff_ptr, writable_size(), 0)) > 0) {
            total_read += n;
            buff_ptr += n;
            _widx += n;
        }
        return total_read;
    }

    int get_line(OneLinePiar& result) {
        if (!readable()) {
            return 0;
        }
        result.start = &_buff[_ridx];
        while (_ridx != _widx && _buff[_ridx++] != '\n');
        result.end = &_buff[_ridx];
        return result.end - result.start;
    }

    int get_ch(char& ch) {
        if (!readable()) {
            return 0;
        }
        ch = _buff[_ridx++];
        return 1;
    }

    int size() const {
        return BUFFER_SIZE;
    }

    void clear() {
        bzero(&_buff);
        _ridx = 0;
        _widx = 0;
    }

    bool readable() const {
        return _widx > _ridx;
    }

    bool writable() const {
        return _widx < BUFFER_SIZE;
    }

    int writable_size() const {
        return BUFFER_SIZE - _widx;
    }

    int readable_size() const {
        return _widx - _ridx;
    }

private:
    char _buff[BUFFER_SIZE];
    int _ridx;
    int _widx;
};
} // namespace myhttpd
} // namespace checkking
#endif
