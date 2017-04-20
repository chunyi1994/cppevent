#ifndef RING_LIST
#define RING_LIST
#include <vector>
#include <list>
namespace net{

template<class T>
class RingList;

template<class T>
class __RingListIteartor {
    typedef                   T                 ValueType;
    typedef                   T&              Reference;
    typedef                   T*               Pointer;
    typedef    __RingListIteartor    Self;

    friend class RingList<T>;
public:
    __RingListIteartor() : begin_(nullptr), end_(nullptr), iter_(nullptr)  { }
    Reference operator*() const{ return *iter_; }
    Pointer operator->() const { return iter_; }
    Self& operator++() {
        if (iter_ == end_) {
            iter_ = begin_;
        } else {
            ++iter_;
        }
        return *this;
    }

    Self operator++(int) {
        Self temp = *this;
        if (iter_ == end_) {
            iter_ = begin_;
        } else {
            ++iter_;
        }
        return temp;
    }

    Self& operator--() {
        if (iter_ == begin_) {
            iter_ = end_;
        } else {
            --iter_;
        }
        return *this;
    }

    Self operator--(int) {
        Self temp = *this;
        if (iter_ == begin_) {
            iter_ = end_;
        } else {
            --iter_;
        }
        return temp;
    }
    bool operator==(const Self& __x) const {
        return __x.begin_ == begin_ && __x.end_ == end_ && __x.iter_ == iter_;
    }
    bool operator!=(const Self& __x) const { return !operator==(__x); }
private:
    typename std::vector<T>::iterator begin_;
    typename std::vector<T>::iterator end_;
    typename std::vector<T>::iterator iter_;
};



template <class T>
class RingList {
public:
    typedef                __RingListIteartor<T>              Iterator;
    typedef                const Iterator                          ConstIterator;
    typedef                T&                                             Reference;
public:
    explicit RingList(std::size_t size = 2) : vec_(size) , front_(begin()) , back_(begin()){
        back_.iter_ += size - 1;
    }

    Iterator begin() {
        Iterator iter;
        iter.begin_ = vec_.begin();
        iter.end_ = vec_.begin() + vec_.size() - 1;
        iter.iter_ = vec_.begin();
        return iter;
    }

    Reference front() { return *front_; }
    Reference back() { return *back_; }
    void next() { ++front_;  ++back_; }

    //    T& at(std::size_t pos) { return vec_.at(pos); }
    //    T& operator[](std::size_t pos) { return vec_[pos]; }
    //    const T& at(std::size_t pos) const { return vec_.at(pos); }
    //    const T& operator[](std::size_t pos) const { return vec_[pos]; }

private:
    std::vector<T> vec_;
    Iterator front_;
    Iterator back_;
};

}//namespace
#endif // RING_LIST

