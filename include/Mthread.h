#ifndef __MTHREAD__
#define __MTHREAD__

#include <tuple>
#include <memory>
#include "Types.h"

#if defined (__APPLE__) || defined (__linux__)
#include <pthread.h>
#elif defined (_WIN32)
#include <process.h>
#endif

template<typename T >
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template<size_t...> struct tuple_indices {};

template<size_t Sp, class IntTuple, size_t Ep> struct make_indices_imp;

template<size_t Sp, size_t... Indices, size_t Ep>
struct make_indices_imp<Sp, tuple_indices<Indices...>, Ep>
{
    typedef typename make_indices_imp<Sp + 1, tuple_indices<Indices..., Sp>, Ep>::type type;
};

//recursion end Sp=Ep
template<size_t Ep, size_t... Indices>
struct make_indices_imp<Ep, tuple_indices<Indices...>, Ep>
{
    typedef tuple_indices<Indices...> type;
};

//indices
template<size_t Ep, size_t Sp = 0>
struct make_tuple_indices {
    typedef typename make_indices_imp<Sp, tuple_indices<>, Ep>::type type;
};

class MThread
{
public:
    //delete copy constructor/operator
    MThread(const MThread&) = delete;
    MThread& operator=(const MThread&) = delete;
    
    //typedef
  
    typedef thread_t native_handle_type;

    //constructor
    MThread();
    //#ifndef _LIBCPP_CXX03_LANG
    template<typename F, typename... Args,
                    std::enable_if_t<!std::is_same_v<remove_cvref_t<F>, MThread>>>
    MThread(F&& f, Args&&... args)
    {
        typedef std::tuple<std::decay_t<F>, std::decay_t<Args>...> Tp;
        std::unique_ptr<Tp> tp(std::forward<F>(f), std::forward<Args>(args)...);

        int ec = -1;
        #if defined (__APPLE__) || defined (__linux__)
            ec = pthread_create(&t_, &Invoke, tp.get());
        #elif defined (_WIN32)
            ec = _beginthreadex(nullptr, 0, &Invoke, tp.get(), 0, &t_);
        #endif
        if(ec == 0)
        {
            tp.release();
        }
        else
        {
            printf("thread constructor failed");
            std::abort();
        }
    }
    //#endif

    ~MThread()
    {
        assert(!joinable());
    }
    
    void operator=(MThread&& t)
    {
        t_ = t.t_;
        t.t_ = 0;
    }

    //observers
    bool joinable() const noexcept
    {
        return 0 == t_;
    }
    
    ThreadId get_id() const noexcept
    {
        return t_;
    }
    
    native_handle_type native_handle()
    {
        return t_;
    }
    
    static unsigned int hardware_concurrency() noexcept;

    //operations
    void join()
    {
        int ec = -1;
        if (0 != t_)
        {
            #if defined (__APPLE__) || defined (__linux__)
                ec = pthread_join(t_, 0);
            #elif defined (_WIN32)
                //ec = _beginthreadex(nullptr, 0, &Invoke, tp.get(), 0, &t_);
            #endif
            if (ec == 0)
                t_ = 0;
        }
    }
    void detach()
    {
        int ec = -1;
        if (0 != t_)
        {
            #if defined (__APPLE__) || defined (__linux__)
                ec = pthread_detach(t_);
            #elif defined (_WIN32)
                //ec = _beginthreadex(nullptr, 0, &Invoke, tp.get(), 0, &t_);
            #endif
            if (ec == 0)
                t_ = 0;
        }
    }
    void swap(MThread& t) noexcept
    {
        std::swap(t.t_, t_);
    }

private:
    ThreadId threadId_;
    native_handle_type type_;
    thread_t t_;
    template<typename Tuple>
    inline int Invoke(void* rawVals) noexcept
    {
        std::unique_ptr<Tuple> tp(static_cast<Tuple>(rawVals));
        typedef typename make_tuple_indices<std::tuple_size_v<Tuple>, 2>::type IndexType;
        thread_execute(tp, IndexType());
        return 0;
    }

    template<typename Tuple, size_t... Indices>
    inline void thread_execute(Tuple tp, tuple_indices<Indices...>)
    {
        invoke(std::move(std::get<0>(tp)), std::move(std::get<Indices>(tp))...);
    }
};

#endif /* MThread_h */