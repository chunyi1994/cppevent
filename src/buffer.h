#ifndef BUFFER_H
#define BUFFER_H
#include <vector>
#include <string>
#include <algorithm>
#include <string.h>
#include "slice.h"
namespace net {


class Buffer {
public:
    Buffer() :
        writeable_buf_(nullptr), readonly_buf_(nullptr),size_(0) {}

    Buffer(char* buff, std::size_t n) :
        writeable_buf_(buff), readonly_buf_(buff), size_(n) {}

    explicit Buffer(std::vector<char>& buff) :
        writeable_buf_(buff.data()), readonly_buf_(buff.data()), size_(buff.size()) {}

    explicit Buffer(std::string& buff) :
        writeable_buf_(&buff[0]),readonly_buf_(&buff[0]), size_(buff.size()) {}

    explicit Buffer(const std::string& buff) :
        writeable_buf_(nullptr),readonly_buf_(&buff[0]), size_(buff.size()) {}

    const char* data() { return readonly_buf_; }

    char* writeable_data() { return writeable_buf_; }

    std::size_t size() const { return size_; }

    void set_write_buf(char* buf) { writeable_buf_ = buf; }

    void set_read_buf(const char* buf) { readonly_buf_ = buf; }

    void set_size(std::size_t size) { size_ = size; }
private:
    char* writeable_buf_;
    const char* readonly_buf_;
    std::size_t size_;
};

//RingBuffer 是一个环形缓存，可以从后面append（添加）数据，
// 也可以从头部read（读取）数据
class RingBuffer {
public:
    typedef  std::vector<char>::iterator Iterator;
    typedef  std::vector<char>::const_iterator ConstIterator;
public:
    explicit RingBuffer(std::size_t size = 1024) :
        data_(size), write_index_(0), read_index_(0) {}

    std::size_t capacity() const { return data_.size(); }

    //void reserve(std::size_t size) {}

    std::size_t size() const { return write_index_ - read_index_; }

    bool empty() const { return size() == 0; }

    void append(const std::string& msg) { append(msg.data(), msg.length()); }

    void append(const char* msg, std::size_t len);

    void clear();

    ConstIterator begin() const { return data_.cbegin() + read_index_; }
    ConstIterator end() const { return data_.cbegin() + read_index_ + size(); }

    std::string read(std::size_t len);

    std::string read_all()  { return read(size()); }

    Slice slice(std::size_t len) const;

    std::size_t erase(std::size_t len);

    std::size_t find(const std::string& target) const;

private:
    const char* read_ptr() const { return (&*data_.begin()) + read_index_; }

    char* write_ptr() { return (&*data_.begin()) + write_index_; }

    std::size_t writeable_size() const { return data_.size() - write_index_; }

private:
    std::vector<char> data_;
    std::size_t write_index_;
    mutable std::size_t read_index_;
};

inline void RingBuffer::append(const char *msg, std::size_t len) {
    if (len == 0) {
        return;
    }
    if (size() == 0) {
        clear();
    }
    if (writeable_size() < len){
        data_.resize(2 * std::max(len, data_.size()));
    }
    ::memcpy(write_ptr(), msg, len);
    write_index_ += len;
    data_[write_index_] = '\0';
}

inline void RingBuffer::clear() {
    write_index_ = 0;
    read_index_ = 0;
    data_[0] = '\0';
}

inline std::string RingBuffer::read(std::size_t len) {
    len = std::min(size(), len);
    std::string ret(read_ptr(), len);
    read_index_ += len;
    return ret;
}

inline Slice RingBuffer::slice(std::size_t len) const {
    if (empty() || len == 0) {
        return Slice();
    }
    return Slice(read_ptr(), std::min(len, size()));
}

inline std::size_t RingBuffer::erase(std::size_t len) {
    len = std::min(size(), len);
    read_index_ += len;
    return len;
}

inline std::size_t RingBuffer::find(const std::string &target) const {
    if (target.empty() || empty()) {
        return std::string::npos;
    }
    ConstIterator start = begin();
    while (start != end()) {
        if ((std::size_t)std::distance(start, end()) < target.length()) {
            return std::string::npos;
        }
        if (std::equal(target.cbegin(), target.cend(), start)) {
            return std::distance(begin(), start);
        }
        ++start;
    }
    return std::string::npos;
}

}
#endif // BUFFER_H

