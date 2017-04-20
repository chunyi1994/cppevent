#ifndef SLICE_H
#define SLICE_H
#include <string>
namespace net {

class Slice {
public:
    typedef const char* ConstIterator;

    static const std::size_t npos = std::string::npos;
public:
    Slice();

    Slice(const char* buf, std::size_t size);

    void clear();

    bool empty() const;

    char operator[](std::size_t pos) const;

    char at(std::size_t pos) const;

    std::size_t size() const { return size_; }

    std::size_t length() const { return size(); }

    ConstIterator begin() const { return start_; }

    ConstIterator end() const { return start_ + size_; }

    ConstIterator cbegin() const { return start_; }

    ConstIterator cend() const { return start_ + size_; }

    std::size_t find(const std::string& target, std::size_t pos = 0) const;

    std::size_t find(const char* target, std::size_t len, std::size_t pos = 0) const;

    Slice subslice(std::size_t pos, std::size_t len) const;

    std::string to_string() const;

    const char* data() const { return start_; }

private:
    const char* start_;

    std::size_t size_;
};

} //namespace
#endif // SLICE_H
