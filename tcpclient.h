#ifndef TcpCONNECTOR_H
#define TcpCONNECTOR_H
#include <string>
#include <memory>
#include "connection.h"
using std::string;
namespace cppevent{
class EventLoop;
class TcpClient{
public:
    TcpClient(EventLoop *loop);
    void connect(const string& ip, int port);
    void setConnectionCallback(const ConnectionCallback& cb);
    void setMessageCallback(const MessageCallback& cb);

private:
    EventLoop* loop_;
    std::shared_ptr<Connection> connPtr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
};
}


//---------tcp client demo-----------------
//int main(){
//    EventLoop loop;
//    TcpClient client(&loop);

//    client.setConnectionCallback([](const ConnectionPtr& conn){
//        HttpMessage msg;
//        msg.setRequestLine("GET", "/", "HTTP/1.1");
//        msg["Host"] = "cn.bing.com";          //msg.addHeaders("Host","cn.bing.com");
//        msg["Connection"] = "keep-alive";  //msg.addHeaders("Connection","keep-alive");
//        conn->send(msg.toString());
//    });

//    client.setMessageCallback([](const ConnectionPtr& conn){
//        string msg = conn->readAll();
//        cout<<msg<<endl;
//    });

//    string ip;
//    int ret = getHostByName("cn.bing.com", ip);
//    client.connect(ip, 80);
//    loop.loop();
//    return 0;
//}
#endif // CONNECTOR_H
