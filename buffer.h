#ifndef BUFFER_H
#define BUFFER_H
#include <vector>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <string.h>
#include <utility>
#include <iostream>
namespace cppevent{
class Buffer{
public:
    Buffer();
    size_t size() const;
    void append(const std::string& msg);
    void append(const char* msg, size_t len);
    void clear();
    std::string read(size_t len);
    bool readLine(std::string& line);
    bool  readLine(std::string &str, char br);

private:
    const char* readPtr() const;
    char* writePtr();
    size_t writeableSize() const;

    std::vector<char> data_;
    size_t writeIndex_;
    size_t readIndex_;
};

}

#endif // BUFFER_H
