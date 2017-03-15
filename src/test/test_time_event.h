#ifndef TEST_TIME_EVENT_H
#define TEST_TIME_EVENT_H
#include "test.h"
#include "../event_loop.h"
namespace test{


//6秒后执行一个任务
void test_time_event1() {
    net::EventLoop base;
    net::TimeEvent::Pointer event = net::TimeEvent::create(net::TimeEvent::eONCE);
    net::Time time1;
    time1.now();
    time1.add_sec(6);
    event->set_time(time1);
    event->set_time_callback([](net::TimeEvent::Pointer) {
        DEBUG_INFO<<"time_callback!";
    });
    base.add_time_event(event);
    base.loop();
}

//测试run_after run_at run_every
void test_time_event_event_loop () {
    net::EventLoop base;
    int n = 0;
     base.run_every(net::Time(1), [&n](net::TimeEvent::Pointer event) {
         if (n >= 9) {
             event->cancel();
             DEBUG_INFO<<"run event over!";
             return;
         }
         DEBUG_INFO<<"run_event:"<<n;
         ++n;
     });

     net::Time t;
     t.now();
     t.add_sec(5);
     base.run_at(t, [](net::TimeEvent::Pointer) {
         DEBUG_INFO<<"run at";
     });

     base.run_after(net::Time(10), [](net::TimeEvent::Pointer) {
         DEBUG_INFO<<"run after";
     });
    base.loop();
}

//几个任务排队执行
void test_time_event2() {
    net::EventLoop base;
    net::Time time1;
    net::TimeEvent::Pointer event = net::TimeEvent::create(net::TimeEvent::eONCE);
    time1.now();
    time1.add_sec(1);
    event->set_time(time1);
    event->set_time_callback([](net::TimeEvent::Pointer) {
        DEBUG_INFO<<"1111111111111!";
    });
    base.add_time_event(event);

    event = net::TimeEvent::create(net::TimeEvent::eONCE);
    time1.now();
    time1.add_sec(2);
    event->set_time(time1);
    event->set_time_callback([](net::TimeEvent::Pointer) {
        DEBUG_INFO<<"22222222222222222!";
    });
    base.add_time_event(event);


    event = net::TimeEvent::create(net::TimeEvent::eONCE);
    time1.now();
    time1.add_sec(6);
    event->set_time(time1);
    event->set_time_callback([](net::TimeEvent::Pointer) {
        DEBUG_INFO<<"3333333333333333333!";
    });
    base.add_time_event(event);


    event = net::TimeEvent::create(net::TimeEvent::eONCE);
    time1.now();
    time1.add_sec(9);
    event->set_time(time1);
    event->set_time_callback([](net::TimeEvent::Pointer) {
        DEBUG_INFO<<"4444444444444444444!";
        DEBUG_INFO<<"over!!";
    });
    base.add_time_event(event);
    base.loop();
}

}//namespace

#endif // TEST_TIME_EVENT_H

