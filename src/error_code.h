#ifndef ERROR_CODE
#define ERROR_CODE
#include <string>
namespace net {
enum ErrorType {
    eEOF,
    eUNKNOWN,
    eOK,
    eINET_ERR,
    eSOCKET_ERR,
    eCONNECT_ERR,
    eOTHER_ERR
};

class ErrorCode {
public:
    ErrorCode(): code_(0), msg_() {}
    ErrorCode(int code, const std::string& msg): code_(code), msg_(msg) {}
    void set_code(int code) { code_ = code; }
    void set_msg(const std::string& msg) { msg_ = msg; }
    std::string msg() const { return msg_; }
    int code() const { return code_; }
private:
    int code_;
    std::string msg_;
};


}//namespace
#endif // ERROR_CODE

