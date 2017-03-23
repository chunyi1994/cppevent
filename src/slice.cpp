#include "slice.h"
#include <algorithm>
namespace net {
Slice::Slice() :
    start_(nullptr),
    size_(0)
{

}

Slice::Slice(const char *buf, std::size_t size) :
    start_(buf),
    size_(size)
{

}

bool Slice::empty() const {
    return size_ == 0 || !start_;
}

char Slice::operator[](std::size_t pos) const {
    return at(pos);
}

char Slice::at(std::size_t pos) const {
    if (pos >= size()) {
        return '\0';
    }
    return start_[pos];
}

void Slice::clear() {
    size_ = 0;
    start_ = nullptr;
}

std::size_t Slice::find(const std::string &target, std::size_t pos) const {
    return find(target.data(), target.length(), pos);
}

std::size_t Slice::find(const char *target, std::size_t len, std::size_t pos) const {
    if (empty() || pos >= size()) {
        return npos;
    }
    ConstIterator iter = cbegin() + pos;
    while (iter != cend()) {
        if ((std::size_t)std::distance(iter, cend()) < len) {
            return npos;
        }
        if (std::equal(target, target + len, iter)) {
            return std::distance(begin(), iter);
        }
        ++iter;
    }
    return npos;
}

Slice Slice::subslice(std::size_t pos, std::size_t len) const {
    if (empty() || pos >= size()) {
        return Slice();
    }
    ConstIterator begin_iter = pos + begin();
    len = std::min(len, static_cast<std::size_t>(std::distance(begin_iter, cend())));
    return Slice(begin_iter, len);
}

std::string Slice::to_string() const {
    if (empty()) {
        return std::string("");
    }
    return std::string(begin(), size());
}

} //namespace
