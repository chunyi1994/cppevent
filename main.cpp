#include <iostream>
#include "eventloop.h"
#include "event.h"
#include "socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "listener.h"
#include "tcpserver.h"
#include "tcpclient.h"
#include "chatroom.h"
#include "proxy_test.h"
#include "http_message.h"
#include "utils.h"
#include "httpclient.h"
#include "timer.h"
#include <fstream>
using namespace std;
using namespace cppevent;


//int main(){
//    EventLoop loop;
//    HttpClient client(&loop);
//    client.get("http://www.qq.com", 80);
//    loop.loop();
//    return 0;
//}

//-----------------------------------------------------------------------------------------------------------------

//tcp server demo

int main(){
    EventLoop loop;
    TcpServer server(&loop, 24444);
    Time time(3);
    loop.runAfter(time, [](){
        log("run After 3");
    });

    loop.runEvery(time, [](){
        log("run every 3");
    });

    server.setMessageCallback([](const ConnectionPtr& conn){
                string msg = conn->readAll();
                cout<<msg<<endl;
    });

    server.setConnectionCallback([](const ConnectionPtr& conn){
        if(conn->connecting()){
            log("New Connection");

        }else{
            log("Connection Close");
        }
    });

    server.start();
    loop.loop();
    cout<<"hello world"<<endl;
    return 0;
}




