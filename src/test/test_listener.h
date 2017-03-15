#ifndef TEST_LISTENER
#define TEST_LISTENER
#include "test.h"
#include "../event_loop.h"
#include "../listener.h"
namespace test {
    static void test_listener() {
        net::EventLoop loop;
        net::Listener listener(&loop, 23333);
        DEBUG_INFO<<"start listen!";
        listener.listen();
        listener.set_new_connection_callback([](net::Connection::Pointer) {
            DEBUG_INFO<<"new connection ok!";
        });
        loop.loop();
    }
} //namespace
#endif // TEST_LISTENER

