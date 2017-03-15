#ifndef BUFFER_H
#define BUFFER_H
#include <vector>
#include <string>
namespace net {


class Buffer {
public:
    Buffer() : buf_(nullptr), size_(0) {}
    Buffer(char* buff, std::size_t n) : buf_(buff), size_(n) {}
    Buffer(std::vector<char>& buff) : buf_(buff.data()), size_(buff.size()) {}
    Buffer(std::string& buff) : buf_(&buff[0]), size_(buff.size()) {}
    char* data() { return buf_; }
    std::size_t size() const { return size_; }

    void set_buf(char* buf) { buf_ = buf; }
    void set_size(std::size_t size) { size_ = size; }
private:
    char* buf_;
    std::size_t size_;
};

}
#endif // BUFFER_H

