#ifndef TEST_COROUTINE_H
#define TEST_COROUTINE_H
#include "test.h"
#include "../event_loop.h"
#include "../tcp_server.h"
#include "../utils.h"
namespace test{

//测试协程
void test_coroutine_read_at_last () {
    net::EventLoop base;
    net::Listener listener(&base, 23333);
    std::vector<char> buf(1024);
    listener.listen();
    listener.on_connection([&base,&buf](net::Connection::Pointer conn) {
        DEBUG_INFO<<"new connection!";
        base.start_coroutine([&base,&buf, conn](coroutine::Coroutine* co) {
            EXCEPT_TRUE(co);
            //至少读20个字节再返回
            //std::size_t n = utils::read_at_least(conn, net::Buffer(buf), 20, co);
            //std::string recv(buf.data(), n);
            //DEBUG_INFO<<"recv:"<<recv;
            //EXCEPT_TRUE(recv.size() >= 20);
        });
    });
    base.loop();
}

void test_coroutine () {
    net::EventLoop base;
    net::Listener listener(&base, 23333);
    std::vector<char> buf(1024);
    listener.listen();
    std::string msg;
    listener.on_connection([&base,&buf, &msg](net::Connection::Pointer conn) {
        DEBUG_INFO<<"new connection!";
        base.start_coroutine([&base,&buf, conn, &msg](coroutine::Coroutine*) {


            DEBUG_INFO<<"msg:"<<msg;
        });
    });

    base.loop();
}

}//namespace
#endif // TEST_COROUTINE_H

