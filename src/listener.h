#ifndef LISTENER_H
#define LISTENER_H
#include <functional>
#include <netinet/in.h>
#include <memory>
#include "connection.h"
namespace net {
class EventLoop;
class Event;
class Listener
{
public:
    typedef std::function<void(Connection::Pointer)> NewConnectionCallback;
public:
    Listener(EventLoop *loop, std::size_t port);

    ~Listener();

    void on_connection(const NewConnectionCallback &cb);

    void listen();

private:
    //nocopyable
    Listener& operator=(const Listener&) = delete;
    Listener(const Listener&) = delete;
    Listener(Listener&&) = delete;

private:
    EventLoop* loop_;

    std::size_t port_;

    int listen_socket_;

    Event* event_;

    NewConnectionCallback connection_callback_;
};
} //namespace
#endif // LISTENER_H
