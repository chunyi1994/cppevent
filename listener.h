#ifndef LISTEN_ACCEPTOR_H
#define LISTEN_ACCEPTOR_H
#include <functional>
#include <netinet/in.h>

#include "socket.h"

namespace cppevent{
class EventLoop;
class Event;
class Connection;
typedef std::function<void(int, sockaddr_in*, size_t)> ListenerConnectionCallback;

class Listener
{
public:
    Listener(EventLoop *loop, size_t port);
    ~Listener();
    void setNewConnectionCallback(const ListenerConnectionCallback &cb);
    void listen();
private:
    void handleRead();

    Listener& operator=(const Listener&);
    Listener(const Listener&);

    size_t port_;
    Socket listenSocket_;
    Event* event_;
    ListenerConnectionCallback connectionCallback_;
};

}
#endif // LISTEN_ACCEPTOR_H

