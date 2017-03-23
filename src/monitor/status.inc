#ifndef STATUS_INC
#define STATUS_INC
#include "../http/http_server2.h"
#include <fstream>
#include <string>
#include <unistd.h>
#include "../utils.h"
namespace net{
namespace monitor{

static void handle_status(http::HttpRequest&, http::ResponseWriter& writer) {
    writer.write("<!DOCTYPE html><html lang=\"zh-cn\">");
    writer.write("<head><meta charset=\"utf-8\"><title>infomation</title></head><body>");
    int pid = ::getpid();
    std::ifstream ifs("/proc/" + utils::to_string(pid) + "/status");
    if (ifs) {
        std::string line;
        while (std::getline(ifs, line)) {
            writer.write("<p>");
            writer.write(line);
            writer.write("</p>");
        }
    }
    writer.write("</body></html>");
}

}//namespace
}//namespace
#endif // STATUS_INC

