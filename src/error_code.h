#ifndef ERROR_CODE
#define ERROR_CODE
#include <string>
namespace net {
enum ErrorType {
    eOK,
    eEOF,
    eUNKNOWN,
    eINET_ERR,
    eSOCKET_ERR,
    eCONNECT_ERR,
    eOTHER_ERR
};

class ErrorCode {
public:
    ErrorCode(): code_(0), msg_() {}
    ErrorCode(int code, const std::string& msg): code_(code), msg_(msg) {}
    //数字码
    void set_code(int code) { code_ = code; }
    int code() const { return code_; }

    //文字错误提示
    void set_msg(const std::string& msg) { msg_ = msg; }
    std::string msg() const { return msg_; }

private:
    int code_;
    std::string msg_;
};


}//namespace
#endif // ERROR_CODE

