#include "coroutine.h"
#include <iostream>
#include <assert.h>
#include <string.h>
namespace coroutine {

Schedule::Schedule() :
    coroutines_(),
    running_coroutine_(nullptr),
    max_id_(0)
{
}

Schedule::~Schedule() {
    for (auto p : coroutines_) {
        if (p) {
            delete p;
        }
    }
}

bool Schedule::available(Coroutine::Pointer co) const {
    if (!co) {
        return false;
    }
    auto iter = coroutines_.find(co);
    return iter != coroutines_.end();
}

Coroutine::Pointer Schedule::new_coroutine(Function func) {
    Coroutine::Pointer co = new Coroutine;
    assert(co);
    co->func_ = func;
    co->id_ = max_id_++;
    coroutines_.insert(co);
    co->status_ = Coroutine::eREADY;
    return co;
}

void mainfunc(void* arg) {
    Schedule * s = (Schedule*)arg;
    Coroutine::Pointer co = s->running_coroutine();
    assert(co);
    co->func_(co);
    s->remove(co);
}

int Schedule::resume(Coroutine::Pointer co) {
    if (!co) {
        return -1;
    }
    int status = co->status_;
    switch (status) {
    case Coroutine::eREADY :
        getcontext(&co->ctx_);
        co->ctx_.uc_stack.ss_sp = stack_top();
        co->ctx_.uc_stack.ss_size = stack_size();
        co->ctx_.uc_link = &main_ctx_;
        co->status_ = Coroutine::eRUNNING;
        co->schedule_ = this;
        running_coroutine_ = co;
        //makecontext的第一个参数必须要是getcontext得到的。
        makecontext(&co->ctx_, (void (*)(void)) mainfunc, 1, this);
        swapcontext(&main_ctx_, &co->ctx_);
        break;
    case Coroutine::eSUSPEND :
        memcpy(stack_bottom() - co->stack_.size(), co->stack_top(), co->stack_.size());
        running_coroutine_ = co;
        co->status_ = Coroutine::eRUNNING;
        swapcontext(&main_ctx_, &co->ctx_);
        break;
    default:
        assert(0);
        break;
    }
    return 0;
}

void Schedule::resume_all() {
    for (auto p : coroutines_) {
        resume(p);
    }
}

void Schedule::remove(Coroutine::Pointer co) {
    auto iter = coroutines_.find(co);
    assert(iter != coroutines_.end());
    delete *iter;
    coroutines_.erase(iter);
}

void Coroutine::yield() {
    char addr;
    assert(schedule_->stack_bottom() > &addr);
    uint64_t distance = schedule_->stack_bottom() - &addr;
    assert(distance < schedule_->stack_size());
    stack_.resize(distance);
    memcpy(stack_top(), &addr, stack_.size());
    status_ =  Coroutine::eSUSPEND;
    schedule_->running_coroutine_ = nullptr;
    swapcontext(&ctx_ , &schedule_->main_ctx_);
}


} //namespace
