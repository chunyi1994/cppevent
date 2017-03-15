#ifndef SINGLETON_H
#define SINGLETON_H
namespace logging {

template<class T>
class Singleton {
public:
    static T& get_instance() {
        static T instance_;
        return instance_;
    }

    ~Singleton() { }

private:
    Singleton() {}
    Singleton(const Singleton &other);
    Singleton & operator=(const Singleton &other);
};
}
#endif // SINGLETON_H

