#ifndef ERROR_CODE
#define ERROR_CODE
#include <string>
#include <string.h>
#include <stdio.h>
namespace net {


class ErrorCode {

public:
    enum ErrorType {
        eOK,
        eERROR,
        eEOF,
        eUNKNOWN,
        eINET_ERR,
        eSOCKET_ERR,
        eCONNECT_ERR,
        eCONNECT_IN_PROGRESS,
        eOTHER_ERR
    };
public:
    ErrorCode(): code_(eUNKNOWN), errno_(-1),msg_("unknown") {}

    ErrorCode(int code, const std::string& msg): code_(code), errno_(-1), msg_(msg) {}

    ErrorCode(int linux_errno);

    //数字码
    void set_code(int code) { code_ = code; }
    int code() const { return code_; }

    //文字错误提示
    void set_msg(const std::string& msg) { msg_ = msg; }
    std::string msg() const { return msg_; }

    void set_errno(int linux_errno);
    int linux_errno() const { return errno_; }

private:
    void strerror();

private:
    int code_;
    int errno_;
    std::string msg_;
};

inline ErrorCode::ErrorCode(int linux_errno) :
    code_(eUNKNOWN),
    errno_(linux_errno),
    msg_("unknown") {
    strerror();
}

inline void ErrorCode::set_errno(int linux_errno) {
    errno_ = linux_errno;
    strerror();
}

inline void ErrorCode::strerror() {
    if (errno_ < 0) {
        return;
    }
    msg_ = ::strerror(errno_);
}


}//namespace
#endif // ERROR_CODE

