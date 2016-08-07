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
#include <sys/mman.h>

// ----------------------------------------------------------------------------
#include <iostream>
#include <cstring>

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

  class task_stack
  {
  public:
    task_stack(size_t size) : size_(size)
    {
      ptr_ = mmap(nullptr, size_, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);

      if ( ptr_ == MAP_FAILED ) {
        throw std::system_error(errno, std::system_category());
      }
    }
  public:
    ~task_stack()
    {
      if ( munmap(ptr_, size_) < 0 ) {
        std::cerr << "munmap failed " << strerror(errno) << std::endl;
      }
    }
  public:
    void * ptr()
    {
      return ptr_;
    }
  public:
    size_t size()
    {
      return size_;
    }
  private:
    size_t size_;
    void * ptr_;
  };

  class task
  {
  private:
    union function_arguments
    {
      void* ptr;
      int   u32[2];
    };
  public:
    task()
      :
      stack_(0x10000),
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
    task_state state() const
    {
      return state_;
    }
  public:
    void stop()
    {
      state_ = task_state::stopping;
      shutdown();
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
    virtual void init() {};
    virtual void main() = 0;
    virtual void shutdown() {};
  public:
    void start(loop* loop);
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
  private:
    static void call(int arg0, int arg1);
  private:
    ucontext_t       caller_;
    ucontext_t       callee_;
    task_stack       stack_;
    event            rdy_;
    event_descriptor ed_;
    task_state       state_;
    loop*            loop_;
  };
}
// ----------------------------------------------------------------------------
#endif