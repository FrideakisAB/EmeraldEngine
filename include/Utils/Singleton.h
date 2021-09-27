#ifndef SINGLETON_H
#define SINGLETON_H

#include <mutex>
#include <boost/noncopyable.hpp>

template<typename T>
class Singleton : private boost::noncopyable {
private:
    inline static T *instance = nullptr;
    inline static std::once_flag flag;

    static void init()
    {
        instance = new T();
    }

public:
    static T &Get()
    {
        std::call_once(flag, init);
        return *instance;
    }

    static void Destroy()
    {
        delete instance;
    }
};

#endif
