#ifndef ANY
#define ANY
#include <typeinfo>
#include <assert.h>
#include "logging/logger.h"
namespace net {
class __AnyPlaceholder {
public:
    virtual ~__AnyPlaceholder() {}
    virtual __AnyPlaceholder*clone() const { return nullptr; }
    virtual const std::type_info& type() const  { return typeid(void); }
};

template<class T>
class __AnyHolder : public __AnyPlaceholder{
public:
    __AnyHolder(const T& value) :  value_(value) {}
    virtual __AnyPlaceholder* clone() const { return new __AnyHolder<T>(value_); }
    virtual const std::type_info& type() const { return typeid(T); }
    virtual ~__AnyHolder() {}
    T value_;
};

class Any {
public:
    Any() : holder_(nullptr) {}

    Any(const Any& other) :
        holder_( other.holder_ ? other.holder_->clone() : nullptr) {  }

    Any& operator = (const Any& other) {
        holder_=  other.holder_ ? other.holder_->clone() : nullptr;
        return *this;
    }

    template<class T>
    Any(const T& value) :
        holder_(new __AnyHolder<T>(value)) { }

    Any(Any&& other) {
        holder_=  other.holder_;
        other.holder_ = nullptr;
    }

    ~Any() {
        if (holder_) {
            delete holder_;
        }
    }
    const std::type_info& type() const { return holder_? holder_->type() : typeid(void); }

    template<class V> friend V any_cast(const Any& any);
private:
    __AnyPlaceholder* holder_;
};

template<class T>
static T any_cast(const Any& any) {
    assert(typeid(T) == any.type());
    __AnyHolder<T>* holder = dynamic_cast<__AnyHolder<T>*>(any.holder_);
    return holder->value_;
}

}//namespace
#endif // ANY

