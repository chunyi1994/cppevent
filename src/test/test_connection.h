#ifndef TEST_CONNECTION
#define TEST_CONNECTION
#include "test.h"
#include "../event_loop.h"
#include "../listener.h"
#include "../connection.h"
#include "utils.h"
#include <sys/epoll.h>
#include <arpa/inet.h>
namespace test {
    static void test_connection() {
        net::EventLoop loop;
        net::Listener listener(&loop, 23333);
        DEBUG_INFO<<"start listen!";
        listener.listen();
        net::Connection::Pointer conn;
        std::vector<char> buf(1024);
        listener.on_connection([&conn,&buf](net::Connection::Pointer co) {
            conn = co;
            conn->on_read([](const net::Connection::Pointer& c) {
                std::string recv = c->buf().read_all();
                DEBUG_INFO<<recv;
            });
        });
        loop.loop();
    }
} //namespace
#endif // TEST_CONNECTION

