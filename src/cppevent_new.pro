TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Werror
QMAKE_CXXFLAGS +=  -Wno-unused-function
QMAKE_CXXFLAGS +=  -Wno-unused-variable

LIBS += -lpthread

SOURCES += main.cpp \
    event_loop.cpp \
    poller.cpp \
    event.cpp \
    listener.cpp \
    connection.cpp \
    tcp_server.cpp \
    logging/log_recorder.cpp \
    logging/logger.cpp \
    linux_signal.cpp \
    coroutine/coroutine.cpp \
    timer.cpp \
    time_event.cpp \
    connector.cpp \
    tcp_client.cpp \
    http/http_message.cpp \
    http/http_request.cpp \
    http/http_response.cpp \
    http/http_server.cpp \
    http/http_handler.cpp

HEADERS += \
    event_loop.h \
    poller.h \
    event.h \
    test/dotest.h \
    test/test.h \
    listener.h \
    utils.h \
    test/test_listener.h \
    connection.h \
    tcp_address.h \
    test/test_connection.h \
    tcp_server.h \
    test/test_tcp_server.h \
    socket.h \
    logging/blocking_queue.h \
    logging/log_recorder.h \
    logging/logger.h \
    logging/singleton.h \
    linux_signal.h \
    coroutine/coroutine.h \
    timer.h \
    time_event.h \
    test/test_time_event.h \
    connector.h \
    tcp_client.h \
    test/test_connector_tcpclient.h \
    buffer.h \
    test/test_coroutine.h \
    http/cookie.h \
    http/http_message.h \
    http/http_request.h \
    test/test_http_message.h \
    http/http_response.h \
    http/http_connection.h \
    http/http_server.h \
    http/http_handler.h \
    test/test_utils.h \
    test/test_http_server.h

