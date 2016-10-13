#include "port_scan.h"

PortScan::PortScan(EventLoop *loop) :
    client_(loop),
    ports_()
{

}

void PortScan::scan(const string &ip)
{
    int ret = 0;
    for(size_t i = 0; i <= 65535; i++)
    {
        ret = client_.connect(ip, i);
        if(ret > 0)
        {
            ports_.push_back(i);
        }
    }

}

const vector<int> &PortScan::ports() const
{
    return ports_;
}
