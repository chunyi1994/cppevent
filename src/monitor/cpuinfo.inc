#ifndef CPUINFO_INC
#define CPUINFO_INC
#include "../http/http_server2.h"
#include <fstream>
#include <string>
namespace net{
namespace monitor{

static void handle_cpuinfo(http::HttpRequest&, http::ResponseWriter& writer) {
    writer.write("<!DOCTYPE html><html lang=\"zh-cn\">");
    writer.write("<head><meta charset=\"utf-8\"><title>infomation</title></head><body>");
    std::ifstream ifs("/proc/cpuinfo");
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

#endif // CPUINFO_INC

