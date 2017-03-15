#ifndef COROUTINE_H
#define COROUTINE_H
#include <vector>
#include <ucontext.h>
#include <functional>
#include <set>
//参考云风的coroutine
//https://github.com/cloudwu/coroutine/
namespace coroutine {
class Schedule;
class Coroutine;

struct Coroutine {
    typedef std::function<void(Coroutine* s)> Function;
public:
    enum Status{
        eDEAD,
        eREADY,
        eSUSPEND,
        eRUNNING,
        eSIZE
    };
    typedef Coroutine* Pointer;
    friend void mainfunc(void* arg);
    friend class Schedule;

public:
    Coroutine() : stack_(), func_(), status_(eDEAD), id_(-1), schedule_(nullptr){}
    int id() const { return this->id_; }
    void yield();
private:
    char* stack_top() { return &*stack_.begin(); }

private:
    std::vector<char> stack_;
    Function func_;
    ucontext_t ctx_;
    int status_;
    int id_;
    Schedule* schedule_;
};

const int  STACK_SIZE  = 1024*1024;

class Schedule {
    friend struct Coroutine;
public:
    typedef Coroutine::Function Function;
public:
    Schedule();
    ~Schedule();

    Coroutine::Pointer running_coroutine() const { return running_coroutine_;  }
    bool available(Coroutine::Pointer) const;
    bool empty() const { return coroutines_.size() == 0; }

    Coroutine::Pointer new_coroutine(Function func);
    int resume(Coroutine::Pointer co);
    void resume_all();
    void remove(Coroutine::Pointer co);

private:
    std::size_t stack_size() const { return STACK_SIZE; }
    char* stack_top() { return stack_; }
    char* stack_bottom() { return stack_top() + stack_size() ; }

private:
    std::set<Coroutine::Pointer> coroutines_;
    char stack_[STACK_SIZE];
    ucontext_t main_ctx_;
    Coroutine::Pointer running_coroutine_;
    int max_id_;
};


} //namespace

#endif // COROUTINE_H
