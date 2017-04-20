#ifndef UTILS
#define UTILS
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <iostream>
namespace utils {

//大小端转换
static uint32_t host_to_network_32(uint32_t hostlong) {
    return ::htonl(hostlong);
}

static uint16_t host_to_network_16(uint16_t hostshort) {
    return ::htons(hostshort);
}

static uint32_t network_to_host_32(uint32_t netlong) {
    return ::ntohl(netlong);
}

static uint16_t network_to_host_16(uint16_t netshort) {
    return ::ntohs(netshort);
}

static bool has_prefix(const std::string& src, const std::string& dest) {
    if (dest.length() > src.length()) {
        return false;
    }
    if (src.substr(0, dest.length()) == dest) {
        return true;
    }
    return false;
}

static bool is_digit_16(const std::string& str) {
    if (str.empty()) return false;
    for (auto c : str) {
        if ((c >= '0' && c <= '9')
                || (c >= 'a' && c <= 'f')
                || (c >= 'A' && c <= 'F')) {

            continue;
        } else {
            return false;
        }
    }
    return true;
}

static bool is_digit(const std::string& str) {
    for (auto c : str) {
        if (c > '9' || c < '0') {
            return false;
        }
    }
    return true;
}

template<class T>
static std::string to_string(T value) {
    std::stringstream ss;
    ss<<value;
    return ss.str();
}

template <class T>
static int to_int(const T& value) {
    int result = 0;
    std::stringstream ss;
    ss << value;
    ss >> result;
    return result;
}

static bool trim(std::string& str) {
    if (str.empty()) {
        return false;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase( str.find_last_not_of(" ") + 1);
    return true;
}

static bool trim_quo(std::string& str) {
    if (str.empty()) {
        return false;
    }
    str.erase(0, str.find_first_not_of("\""));
    str.erase(str.find_last_not_of("\"") + 1);
    return true;
}

static std::vector<std::string> split(const std::string& str, char c) {
    std::string line;
    std::stringstream ss(str);
    std::vector<std::string> result;
    while (std::getline(ss, line, c)) {
        result.push_back(line);
    }
    return result;
}
//todo 可能有问题
static std::vector<std::string> split(const std::string& str, const std::string& s) { //s是分隔符
    std::vector<std::string> result;
    std::size_t left = 0;
    std::size_t right = 0;
    while ( (right = str.find(s, left))  != std::string::npos)  {
        result.push_back(str.substr(left, right));
        left = right + s.length();
    }
    if (left < str.length()) {
        result.push_back(str.substr(left, str.size() - left));
    }
    return result;
}

static std::vector<std::string> split2(const std::string& str, const std::string& s) { //s是分隔符
    std::vector<std::string> result;
    std::size_t left = 0;
    std::size_t right = 0;
    while ( (right = str.find(s, left))  != std::string::npos)  {
        std::string m = str.substr(left, right);
        if (!m.empty()) {
            result.push_back(m);
        }
        left = right + s.length();
    }
    if (left < str.length()) {
        result.push_back(str.substr(left, str.size() - left));
    }
    return result;
}


static ssize_t file_size(const char *path) {
    ssize_t fileSize = -1;
    struct stat statBuff;
    if(stat(path, &statBuff) >= 0) {
        fileSize = statBuff.st_size;
    }
    return fileSize;
}


static int hex2int(const std::string& hexStr) {
    char *offset;
    if(hexStr.length() > 2) {
       if(hexStr[0] == '0' && hexStr[1] == 'x')  {
            return ::strtol(hexStr.c_str(), &offset, 0);
       }
    }
    return ::strtol(hexStr.c_str(), &offset, 16);
}


} // namespace
#endif // UTILS

