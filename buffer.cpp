#include "buffer.h"
namespace cppevent{
Buffer::Buffer():data_(1024), writeIndex_(0), readIndex_(0){
    data_[0] = '\0';
}

size_t Buffer::size() const{
    return writeIndex_ - readIndex_;
}

void Buffer::append(const char* msg, size_t len){
    if(writeableSize() < len){
        data_.resize(2 * std::max(len,data_.size()));
    }
    ::memcpy(writePtr(), msg, len);
    writeIndex_ += len;
    data_[writeIndex_] = '\0';
}

void Buffer::append(const std::string& msg){
    append(msg.c_str(), msg.length());
}

void Buffer::clear(){
    writeIndex_ = 0;
    readIndex_ = 0;
    data_[0] = '\0';
}

std::string Buffer::read(size_t len){
    len = std::min(size(), len);
    std::string ret(readPtr(), len);
    readIndex_ += len;
    if(size() == 0){
        clear();
    }
    return ret;
}

bool Buffer::readLine(std::string& line){
    return readLine(line, '\n');
}

bool Buffer:: readLine(std::string &str, char br){
    if(size() == 0){
        return false;
    }
    size_t i = readIndex_;
    while(i < writeIndex_){
        if(data_[i] == br){
            break;
        }
        i++;
    }
    str = read(i + 1 - readIndex_);
    return true;
}

const char* Buffer::readPtr() const{
    auto iter = readIndex_ + data_.begin();
    return static_cast<const char *>(&*iter);
}

char* Buffer::writePtr(){
    auto iter = writeIndex_ + data_.begin();
    return static_cast<char *>(&*iter);
}

size_t Buffer::writeableSize() const{
    return data_.size() - writeIndex_;
}

}
