//
// Created by FDC on 2021/12/10.
//

#ifndef MTHREAD_THREAD_HEADER_H
#define MTHREAD_THREAD_HEADER_H

#include "thread_help.h"

namespace M
{
    struct defer_lock_t { explicit defer_lock_t() = default;};
    struct try_to_lock_t { explicit try_to_lock_t() = default; };
    struct adopt_lock_t { explicit adopt_lock_t() = default; };

    constexpr defer_lock_t defer_lock {};
    constexpr try_to_lock_t try_to_lock {};
    constexpr adopt_lock_t adopt_lock {};

    template<typename Mutex>
    class lock_guard
    {
    public:
        typedef Mutex mutex_type;
        explicit lock_guard(mutex_type &m);
        lock_guard(mutex_type& m, adopt_lock_t);

        ~lock_guard();

        lock_guard(const lock_guard&) = delete;
        lock_guard& operator=(const lock_guard&) = delete;
    private:
        mutex_type& m_;
    };

    class mutex {
    public:
        constexpr mutex() = default;

        ~mutex();

        mutex(const mutex &) = delete;
        mutex &operator=(const mutex &) = delete;

        void lock();
        bool try_lock();
        void unlock();

        typedef mutex_t* native_handle_type;
        native_handle_type native_handle();
    private:
        mutex_t m_ = MUTEX_INITIALIZER;
    };

    class recursive_mutex {
    public:
        recursive_mutex();
        ~recursive_mutex();

        recursive_mutex(const recursive_mutex &) = delete;
        recursive_mutex &operator=(const recursive_mutex &) = delete;

        void lock();
        bool try_lock();
        void unlock();

        typedef recursive_mutex_t* native_handle_type;
        native_handle_type native_handle();
    private:
        recursive_mutex_t m_;
    };

    class timed_mutex {
    public:
        timed_mutex();

        ~timed_mutex();

        timed_mutex(const timed_mutex &) = delete;

        timed_mutex &operator=(const timed_mutex &) = delete;

        void lock();

        bool try_lock();

        template<typename Rep, typename Period>
        bool try_lock_for(const std::chrono::duration<Rep, Period> &rel_time);

        template<typename Clock, typename Duration>
        bool try_lock_until(const std::chrono::time_point<Clock, Duration> &abs_time);

        void unlock();
    };

    class recursive_timed_mutex {
    public:
        recursive_timed_mutex();

        ~recursive_timed_mutex();

        recursive_timed_mutex(const recursive_timed_mutex &) = delete;

        recursive_timed_mutex &operator=(const recursive_timed_mutex &) = delete;

        void lock();

        bool try_lock();

        template<typename Rep, typename Period>
        bool try_lock_for(const std::chrono::duration<Rep, Period> &rel_time);

        template<typename Clock, typename Duration>
        bool try_lock_until(const std::chrono::time_point<Clock, Duration> &abs_time);

        void unlock();
    };

    template<typename Mutex>
    class unique_lock {
    public:
        typedef Mutex mutex_type;

        unique_lock() noexcept :  m_(nullptr), owns_(false) {}
        explicit unique_lock(mutex_type &m): m_(std::addressof(m)), owns_(true)
        {
            m_->lock();
        }
        unique_lock(mutex_type &m, defer_lock_t) noexcept
                : m_(std::addressof(m)), owns_(false) {}

        unique_lock(mutex_type &m, try_to_lock_t)
                : m_(std::addressof(m)), owns_(m.try_lock()) {}

        unique_lock(mutex_type &m, adopt_lock_t)
                : m_(std::addressof(m)), owns_(true) {}

        template<typename Clock, typename Duration>
        unique_lock(mutex_type &m, const std::chrono::time_point<Clock, Duration> &abs_time)
                : m_(std::addressof(m)), owns_(m.try_lock_until(abs_time)) {}

        template<typename Rep, typename Period>
        unique_lock(mutex_type &m, const std::chrono::duration<Rep, Period> &rel_time)
                : m_(std::addressof(m)), owns_(m.try_lock_for(rel_time)) {}

        ~unique_lock();

        unique_lock(unique_lock &) = delete;
        unique_lock &operator=(unique_lock &) = delete;

        unique_lock(unique_lock&& other) noexcept;
        unique_lock &operator=(unique_lock&& other) noexcept;

        void lock();
        bool try_lock();

        template<typename Rep, typename Period>
        bool try_lock_for(const std::chrono::duration<Rep, Period> &rel_time);

        template<typename Clock, typename Duration>
        bool try_lock_until(const std::chrono::time_point<Clock, Duration> &abs_time);

        void unlock();

        void swap(unique_lock &other) noexcept;
        mutex_type *release() noexcept;

        bool owns_lock() const noexcept { return owns_; }

        explicit  operator bool() const noexcept { return owns_; }

        mutex_type *mutex() const noexcept { return m_; }
    private:
        mutex_type* m_;
        bool owns_;
    };

};

#endif //MTHREAD_THREAD_HEADER_H
