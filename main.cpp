#include <iostream>
#include <fstream>

#include "eventloop.h"
#include "tcpserver.h"
#include "httpclient.h"
#include "demo/tiny_proxy.h"
#include "demo/qq_crawler.h"
using namespace std;
using namespace cppevent;


int main(){
    EventLoop loop;
    QQCrawler crawler(&loop);
    crawler.start();
    loop.loop();
    cout<<"hello world"<<endl;
    return 0;
}


//int main(){
//    EventLoop loop;
//    ProxyServer server(&loop, 8080);
//    server.start();
//    loop.loop();
//    cout<<"hello world"<<endl;
//    return 0;
//}

//int main(){
//    EventLoop loop;
//    HttpClient client(&loop);
//    client.setMessageCallback([](const HttpMessage& response, const std::string& content){
//        cout<<"code : "<<response.statusCode()<<endl;
//        for(auto &w : response){
//            cout<<w.first <<" : " << w.second <<endl;
//        }
//        cout<<content<<endl;
//    });
//    client.get("http://www.baidu.com", 80);
//    loop.loop();
//    cout<<"hello world"<<endl;
//    return 0;
//}

//-----------------------------------------------------------------------------------------------------------------

//tcp server demo

int serverDemo(){
    EventLoop loop;
    TcpServer server(&loop, 8080);

    server.setMessageCallback([](const ConnectionPtr& conn){
                string msg = conn->readAll();
                cout<<msg<<endl;
                conn->send(msg);
    });

    server.setConnectionCallback([](const ConnectionPtr& conn){
        if(conn->connecting()){
            log("New Connection:" + conn->address().ip_ + ":" + int2string(conn->address().port_));
        }else{
            log("Connection Close");

        }
    });

    server.start();
    loop.loop();
    cout<<"hello world"<<endl;
    return 0;
}




