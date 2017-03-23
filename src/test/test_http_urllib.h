#ifndef TEST_HTTP_URLLIB_H
#define TEST_HTTP_URLLIB_H
#include "test.h"
#include "../event_loop.h"
#include "../http/http_urllib.h"
#include "../connection.h"
#include <thread>
namespace test {
static void start_server333() {
    net::EventLoop loop;
    net::Listener listener(&loop, 23333);
    DEBUG_INFO<<"start listen!";
    listener.listen();
    net::Connection::Pointer conn;
    listener.on_connection([&conn](net::Connection::Pointer co) {
        conn = co;
        conn->on_read([](const net::Connection::Pointer& c) {
            std::string recv = c->buf().read_all();
            DEBUG_INFO<<recv;
        });
    });
    loop.loop();
}

static void test_urllib() {
//    std::thread t(start_server333);
//    t.detach();
//    sleep(3);
//    std::map<std::string, std::string> data {{"nihao","wobuhao"},
//                                            {"key","value"} };
//    net::http::post("http://127.0.0.1:23333",data);

//    return;
    int err = 0;
    std::string name;
    std::string path;
    std::size_t port;
    err = net::http::parse_url("http://www.cctv.com:8080/fadfafaegae/3e3a/////", name, path,port);
    EXCEPT_TRUE(err >= 0);
    EXCEPT_EQ(name, "www.cctv.com");
    EXCEPT_EQ(path, "/fadfafaegae/3e3a/////");
    EXCEPT_EQ(port, 8080);

    err = net::http::parse_url("http://www.cctv.com:jiu8080/fadfafaegae/3e3a/////", name, path,port);
    EXCEPT_TRUE(err < 0);
    err = net::http::parse_url("http://www.cctv.com:/fadfafaegae/3e3a/////", name, path,port);
    EXCEPT_TRUE(err < 0);

    err = net::http::parse_url("http://www.cctv.com/fadfafaegae/3e3a/////", name, path,port);
    EXCEPT_TRUE(err >= 0);
    EXCEPT_EQ(name, "www.cctv.com");
    EXCEPT_EQ(path, "/fadfafaegae/3e3a/////");
    EXCEPT_EQ(port, 80);

    err = net::http::parse_url("http://www.cctv.com", name, path,port);
    EXCEPT_TRUE(err >= 0);
    EXCEPT_EQ(name, "www.cctv.com");
    EXCEPT_EQ(path, "/");
    EXCEPT_EQ(port, 80);

    err = net::http::parse_url("www.cctv.com", name, path,port);
    EXCEPT_TRUE(err >= 0);
    EXCEPT_EQ(name, "www.cctv.com");
    EXCEPT_EQ(path, "/");
    EXCEPT_EQ(port, 80);

    err = net::http::parse_url("www.cctv.com/", name, path,port);
    EXCEPT_TRUE(err >= 0);
    EXCEPT_EQ(name, "www.cctv.com");
    EXCEPT_EQ(path, "/");
    EXCEPT_EQ(port, 80);


    std::map<std::string, std::string> data{ {"username", "qiuhonghong"}, {"password","qiuhonghong"} };
    auto r = net::http::post("http://www.cecilqiu.online/cecilqiu/dl", data, 4);
   // if (r.first == net::http::eOK) {
        net::http::HttpResponse::Pointer response = r.second;
        DEBUG_INFO<<response->to_string();
    //} else {
      //  DEBUG_INFO<<"bu ok";
    //}
    //net::http::get("http://10.100.10.104");

}
} //namespace
#endif // TEST_HTTP_URLLIB_H

