#ifndef __TYPES__
#define __TYPES__
#include <ostream>

#if defined (__APPLE__) || defined (__linux__)
#include <pthread.h>
#include <sys/sysctl.h>
#elif defined (_WIN32)
#include <process.h>
#include <xthreads.h>
#include <windows.h>
#endif


#if defined (__APPLE__) || defined (__linux__)
typedef pthread_t _Thrd_id_t;
struct thread_t {
    _Thrd_id_t _Id;
    int _Hnd;
};
#elif defined (_WIN32)
typedef _Thrd_t thread_t;
#endif

#if defined (__clang__) || defined (__GNUC__)
#define CPP_STANDARD __cplusplus	
#elif defined (_MSC_VER)
#define CPP_STANDARD _MSVC_LANG
#endif

#if CPP_STANDARD < 201103L
#define _LIBCPP_CXX03_LANG
#elif CPP_STANDARD < 201402L
#define _LIBCPP_CXX11_LANG
#elif CPP_STANDARD < 201703L
#define _LIBCPP_CXX14_LANG
#elif CPP_STANDARD == 201703L
#define _LIBCPP_CXX17_LANG
#else
#define _LIBCPP_CXX20_LANG
#endif

#ifdef _LIBCPP_CXX20_LANG
#include <compare>
#endif

class ThreadId {
public:
    _Thrd_id_t id_;
    ThreadId() : id_(0) {}
    ThreadId(_Thrd_id_t id) : id_(id) {}

#ifdef _LIBCPP_CXX20_LANG
    friend std::strong_ordering operator<=>(ThreadId& x, ThreadId& y)
    {
        return x.id_ <=> y.id_;
    }
#else
    friend bool operator==(ThreadId& x, ThreadId& y)
    {
        return x.id_ == y.id_;
    }
    friend bool operator!=(ThreadId& x, ThreadId& y)
    {
        return x.id_ != y.id_;
    }
    friend bool operator<(ThreadId& x, ThreadId& y)
    {
        return x.id_ < y.id_;
    }
    friend bool operator<=(ThreadId& x, ThreadId& y)
    {
        return x.id_ <= y.id_;
    }
    friend bool operator>(ThreadId& x, ThreadId& y)
    {
        return x.id_ > y.id_;
    }
    friend bool operator>=(ThreadId& x, ThreadId& y)
    {
        return x.id_ >= y.id_;
    }
#endif

    template<typename Ch, typename Tr>
    friend std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, ThreadId id)
    {
        return os << id.id_;
    }

    friend class MThread;
};

// C++ version < C++14
#ifdef _LIBCPP_CXX11_LANG
template<typename T>
using decay_t = typename decay<T>::type;

template<typename T>
using is_same_t = typename is_same<T>::type;

template<typename T>
using is_same_v = typename is_same<T>::value;

template<typename T>
using enable_if_t = typename enable_if<T>::type;

template<typename T>
using remove_cv_t = typename remove_cv<T>::type;

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;
#endif

#if defined (__APPLE__) || defined (__linux__)
#define MUTEXT_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#elif defined (_WIN32)
#define MUTEXT_INITIALIZER 0
#endif

#endif /* Types_h */
