#ifndef PORT_SCAN_H
#define PORT_SCAN_H
#include <string>
#include <vector>

#include "../tcpclient.h"
#include "../eventloop.h"
#include "../connection.h"
#include "../utils.h"

//网络库测试程序:端口扫描

using namespace std;
using namespace cppevent;

class PortScan
{
public:
    PortScan(EventLoop* loop);
    void scan(const string& ip);
    const vector<int>& ports() const;
private:
    TcpClient client_;
    vector<int> ports_;
};

#endif // PORT_SCAN_H


//int main(){
//    EventLoop loop;
//    PortScan scaner(&loop);
//    scaner.scan("127.0.0.1");
//    vector<int> ret = scaner.ports();
//    std::copy(ret.begin(), ret.end(), std::ostream_iterator<int>(std::cout, " "));
//    std::cout<<std::endl;

//    loop.loop();
//    return 0;
//}
