// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __dripcore__loop_h__
#define __dripcore__loop_h__

// ----------------------------------------------------------------------------
#include <memory>
#include <set>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>

// ----------------------------------------------------------------------------
#include <sys/epoll.h>
#include <unistd.h>

// ----------------------------------------------------------------------------
namespace dripcore
{
  using os_handle_t = int;

  struct event_descriptor
  {
    os_handle_t           fd;
    std::function<void()> rd;
    std::function<void()> wr;
  };

  class task;
  class loop
  {
    using lock_guard = std::lock_guard<std::recursive_mutex>;
  public:
    loop();
  public:
    template<typename T, typename... Args>
    std::weak_ptr<task> spawn(Args&&... args)
    {
      ///// TODO
      //
      // It should probably be safe to spawn tasks from outside
      // loop context.
      //
      /////
      auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
      tasks_.push_back(ptr);
      ptr->init(this);
      return ptr;
    }
  public:
    template<typename T>
    void add(const T& eventable)
    {
      add(eventable.native_handle());
    }
  public:
    template<typename T>
    void del(const T& eventable)
    {
      del(eventable.native_handle());
    }
  public:
    void add(int fd);
    void mod(event_descriptor* ev_desc);
    void del(int fd);
    void del(dripcore::task* task);
  public:
    std::thread::id thread_id()
    {
      return thread_id_;
    }
  public:
    void run();
  public:
    void shutdown();
  private:
    std::thread::id thread_id_;
    os_handle_t epoll_fd_;
    std::atomic<bool> running_;
    //mutable std::recursive_mutex mutex_;
    std::vector<std::shared_ptr<task>> tasks_;
  };
} // dripcore

// ----------------------------------------------------------------------------
#endif // __dripcore__loop_h__
