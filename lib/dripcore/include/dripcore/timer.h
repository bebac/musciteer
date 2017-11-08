// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2017
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __dripcore__timer_h__
#define __dripcore__timer_h__

// ----------------------------------------------------------------------------
#include <functional>
#include <chrono>

// ----------------------------------------------------------------------------
#include <signal.h>
#include <time.h>

// ----------------------------------------------------------------------------
namespace dripcore
{
  class timer
  {
  public:
    timer()
    {
      sigevent sigevent;

      sigevent.sigev_notify = SIGEV_THREAD;
      sigevent.sigev_value.sival_ptr = this;
      sigevent.sigev_notify_attributes = nullptr;
      sigevent.sigev_notify_function = [](sigval value) {
        static_cast<timer*>(value.sival_ptr)->invoke();
      };

      if ( timer_create(CLOCK_MONOTONIC, &sigevent, &timer_) == -1 ) {
        throw std::system_error(errno, std::system_category());
      }
    }
    ~timer()
    {
      timer_delete(timer_);
    }
  public:
    template<class Rep, class Period>
    void set(const std::chrono::duration<Rep, Period>& duration, std::function<void()> callback)
    {
      using std::chrono::duration_cast;
      using std::chrono::nanoseconds;
      set(duration_cast<nanoseconds>(duration).count());
      callback_ = callback;
    }
  private:
    void set(long nsecs)
    {
      struct itimerspec its;

      its.it_value.tv_sec = nsecs / 1000000000;
      its.it_value.tv_nsec = nsecs % 1000000000;
      its.it_interval.tv_sec = 0;
      its.it_interval.tv_nsec = 0;

      if ( timer_settime(timer_, 0, &its, NULL) == -1 ) {
        throw std::system_error(errno, std::system_category());
      }
    }
  private:
    void invoke()
    {
      callback_();
    }
  private:
    std::function<void()> callback_;
    timer_t timer_;
  };
}

// ----------------------------------------------------------------------------
#endif
