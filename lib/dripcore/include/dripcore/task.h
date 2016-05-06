// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __dripcore__task_h__
#define __dripcore__task_h__

// ----------------------------------------------------------------------------
#include <dripcore/loop.h>
#include <dripcore/event.h>

// ----------------------------------------------------------------------------
#include <ucontext.h>
#include <unistd.h>

#include <iostream>

// ----------------------------------------------------------------------------
namespace dripcore
{
  enum class task_state
  {
    init,
    running,
    stopping,
    done
  };

  class task
  {
  private:
    using stack_ptr = std::unique_ptr<unsigned char[]>;
  private:
    union function_arguments
    {
      void* ptr;
      int   u32[2];
    };
  public:
    task()
      :
      stack_size_(32*1024),
      stack_ptr_{new unsigned char[stack_size_]},
      rdy_(),
      state_(task_state::init),
      loop_(nullptr)
    {
    }
  public:
    virtual ~task()
    {
      detach_eventable(rdy_);
    }
  private:
    task(const task&) = delete;
    task& operator=(const task&) = delete;
  public:
    template<typename T, typename... Args>
    std::weak_ptr<task> spawn(Args&&... args)
    {
      auto ptr = loop_->spawn<T>(std::forward<Args>(args)...);
      yield(true);
      return ptr;
    }
  public:
    void stop()
    {
      state_ = task_state::stopping;
    }
  public:
    template <typename T>
    void attach_eventable(const T& eventable)
    {
      loop_->add(eventable);
    }
  public:
    template <typename T>
    void detach_eventable(const T& eventable)
    {
      loop_->del(eventable);
    }
  protected:
    virtual void main() = 0;
  public:
    void init(loop* loop);
  public:
    void resume()
    {
      rdy_.notify();
    }
  private:
    void resume_();
  public:
    bool stopping()
    {
      return state_ == task_state::stopping;
    }
  public:
    bool done()
    {
      return state_ == task_state::done;
    }
#if 0
  public:
    operator bool() const
    {
      return state_ == task_state::done;
    }
#endif
  public:
    void yield(bool resume=false);
#if 1
  public:
    template <typename T>
    void wait_readable(const T& eventable)
    {
      wait_readable(eventable.native_handle());
    }
  public:
    void wait_readable(int fd);
  public:
    template <typename T>
    void wait_writable(const T& eventable)
    {
      wait_writable(eventable.native_handle());
    }
  public:
    void wait_writable(int fd);
#endif
  private:
    static void call(int arg0, int arg1);
  private:
    ucontext_t       caller_;
    ucontext_t       callee_;
    size_t           stack_size_;
    stack_ptr        stack_ptr_;
    event            rdy_;
    event_descriptor ed_;
    task_state       state_;
    loop*            loop_;
  };
}
// ----------------------------------------------------------------------------
#endif