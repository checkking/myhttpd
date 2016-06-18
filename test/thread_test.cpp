// thread_test.cpp
// unit test of Thread class
//
// Author: checkking (checkking@foxmail.com)
// Date: 2016-06-18

#include "../thread.h"
#include <stdio.h>
#include <string>
#include <boost/bind.hpp>

namespace checkking {
namespace myhttpd {
void func1() {
    printf("[func1] tid = [%d], name = [%s]\n", CurrentThread::tid(), CurrentThread::name());
}

void func2(int x) {
    printf("[func2] tid = [%d], name = [%s], x = [%d]\n", CurrentThread::tid(), CurrentThread::name(), x);
}

class Foo {
public:
    explicit Foo(double x) : _x(x) {}

    void func1() {
        printf("[Foo::func2] tid = [%d], name = [%s], x= [%.4f]\n", 
                CurrentThread::tid(), CurrentThread::name(), _x);
    }

    void func2(const std::string& text) {
        printf("[Foo::func2] tid = [%d], name = [%s], x = [%.4f], text = [%s]\n", 
                CurrentThread::tid(), CurrentThread::name(), _x, text.c_str());
    }

private:
    double _x;
};

} // namespace myhttpd
} // namespace checkking

int main() {
    using namespace checkking::myhttpd;
    printf("[main] pid = %d, tid = %d\n", ::getpid(), CurrentThread::tid());
    Thread t1(func1, "thread of func1");
    t1.start();
    t1.join();

    Thread t2(boost::bind(func2, 2), "thread of func2");
    t2.start();
    t2.join();

    Foo foo1(0.5);
    Thread t3(boost::bind(&Foo::func1, &foo1), 
            "thread of Foo::func1");
    t3.start();
    t3.join();

    Foo foo2(1.0);
    Thread t4(boost::bind(&Foo::func2, boost::ref(foo2), std::string("Check King")),
                "Thread of Foo::func2");
    t4.start();
    t4.join();

    printf("[main] thread name=%s\n",  CurrentThread::name());

    return 0;
}
