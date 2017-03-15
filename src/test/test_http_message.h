#ifndef TEST_HTTP_MESSAGE
#define TEST_HTTP_MESSAGE
#include "test.h"
#include "../event_loop.h"
#include "../http/http_request.h"
#include "../http/http_response.h"
namespace test {
static void test_http_request() {
    net::http::HttpRequest msg;
    msg.set_request_line("GET", "/", "HTTP/1.1");
    msg["Host"] = "cn.bing.com";          //msg.addHeaders("Host","cn.bing.com");
    msg["Connection"] = "keep-alive";  //msg.addHeaders("Connection","keep-alive");
    msg.set_cookie("coco","kiki");
    msg.set_cookie("coco2","kiki2");
    DEBUG_INFO<< msg.to_string();
    DEBUG_INFO<< msg["Host"];
    for(auto w : msg){
        DEBUG_INFO<<w.first<<","<<w.second;
    }

//    for(auto iter = msg.begin(); iter != msg.end();  iter++){
//        cout<<iter->first<<endl;
//        cout<<iter->second<<endl;
//    }
}

static void test_http_response() {
    net::http::HttpResponse msg;
    msg.set_status_line("HTTP/1.1", 200, "OK");
    msg["Host"] = "cn.bing.com";          //msg.addHeaders("Host","cn.bing.com");
    msg["Connection"] = "keep-alive";  //msg.addHeaders("Connection","keep-alive");
    msg.set_cookie("keykey","valuevalue");
    DEBUG_INFO<< msg.to_string();
}


} //namespace
#endif // TEST_HTTP_MESSAGE

