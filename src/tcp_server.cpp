#include "tcp_server.h"
#include "utils.h"
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "event_loop.h"
namespace net {

TcpServer::TcpServer(EventLoop *loop, std::size_t port):
    loop_(loop),
    port_(port),
    listener_(loop, port),
    connections_(),
    buf_(1024*2),
    create_time_(),
    heartbeat_time_(),
    error_nums_(0)
{
    create_time_.now();
    listener_.on_connection([this](Connection::Pointer conn) {
        connections_.insert(conn);
        init_heartsbeats(conn);
        conn->set_name("ServerConn");
        conn->on_read( [this](net::Connection::Pointer c) {
            //如果有设定心跳时间，就把他加入队列尾巴。队列头的连接会被shutdown
            process_heartbeats(c);
            if (message_callback_) {
                message_callback_(c);
            }
        });

        conn->on_error([this](net::Connection::Pointer c,
                       const ErrorCode& code) {
            ++error_nums_;
            if (error_callback_) {
                error_callback_(c, code);
            }
        });

        conn->on_close([this] (net::Connection::Pointer c) {
            if (close_callback_) {
                close_callback_(c);
            }
            hanlde_close(c);
        });

        //lock the connection, then user cannot edit its callbacks easily
        conn->set_lock(true);
        if (connection_callback_) {
            connection_callback_(conn);
        }
    });
}

TcpServer::~TcpServer() {

}

void TcpServer::start() {
    listener_.listen();
}

void TcpServer::on_message(const MessageCallback &cb) {
    message_callback_ = cb;
}

void TcpServer::on_error(const ErrorCallback &cb) {
    error_callback_ = cb;
}

void TcpServer::on_close(const TcpServer::CloseCallback &cb) {
    close_callback_ = cb;
}

void TcpServer::shutdown(Connection::Pointer conn) {
    auto iter = connections_.find(conn);
    if (iter != connections_.end()) {
        conn->shutdown();
        //connections_.erase(conn);
    }
}

time_t TcpServer::worked_time() const {
    Time now;
    now.now();
    return now.sec() - create_time_.sec();
}

std::string TcpServer::info() const {
    std::string info("TcpServer infomation:");
    info = info + "[worked_time]: " + utils::to_string(worked_time()) + ". ";
    info = info + "[port]: " + utils::to_string(port()) + ". ";
    info = info + "[connections]: " + utils::to_string(size()) + ". ";
    return info;
}

void TcpServer::set_heartbeat_time(time_t sec) {
    heartbeat_pool_ = RingList<std::set<
            ConnectionHolder::Pointer>>((std::size_t)sec);
    heartbeat_time_.set_sec(sec);
    enable_clean_useless_conn();
}



void TcpServer::enable_clean_useless_conn() {
    loop_->run_every(Time(1), [this] (TimeEvent::Pointer) {
        heartbeat_pool_.front().clear();
        heartbeat_pool_.next();
    });
}

void TcpServer::hanlde_close(Connection::Pointer c) {
    auto iter = connections_.find(c);
    assert(iter != connections_.end());
    connections_.erase(iter);
    loop_->add_task([this, c]() {});
}

//如果有设定心跳时间，就把他加入队列尾巴。队列头的连接会被shutdown
void TcpServer::process_heartbeats(Connection::Pointer c) {
    if (heartbeat_time_.sec() != 0) {
        Any any = c->get_context();
        assert(any.type() == typeid(ConnectionHolder::WeakPointer));
        auto weak = any_cast<ConnectionHolder::WeakPointer>(any);
        ConnectionHolder::Pointer holder = weak.lock();
        if (holder) {
            heartbeat_pool_.back().insert(holder);
        }
    }
}

void TcpServer::init_heartsbeats(Connection::Pointer conn) {
    if (heartbeat_time_.sec() != 0) {
        ConnectionHolder::Pointer conn_holder = ConnectionHolder::create(conn);
        heartbeat_pool_.back().insert(conn_holder);
        ConnectionHolder::WeakPointer weak(conn_holder);
        conn->set_context(weak);
    }
}

void TcpServer::on_connection(const ConnectionCallback &cb) {
    connection_callback_ = cb;
}


}//namespace
