// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __dripcore__io_h__
#define __dripcore__io_h__

// ----------------------------------------------------------------------------
namespace dripcore
{
  class io_task_events
  {
  public:
    io_task_events() : fd_(-1), task_(nullptr)
    {
    }
    io_task_events(int fd, task* task) : fd_(fd), task_(task)
    {
      task_->attach_eventable(fd_);
    }
  public:
    ~io_task_events()
    {
      if ( task_ )
      {
        task_->detach_eventable(fd_);
      }
    }
  public:
    void wait_writable()
    {
      if ( task_ ) {
        task_->wait_writable(fd_);
      }
    }
  public:
    void wait_readable()
    {
      if ( task_ ) {
        task_->wait_readable(fd_);
      }
    }
  private:
    int fd_;
    task* task_;
  };

  class io
  {
  public:
    io() : events_(nullptr)
    {
    }
  private:
    // No copying.
    io(const io& other) = delete;
    io& operator= (const io& rhs) = delete;
  public:
    virtual ~io()
    {
    }
  public:
    virtual ssize_t recv(void *buf, size_t len, int flags=0) = 0;
    virtual ssize_t send(const void *buf, size_t len, int flags=0) = 0;
  public:
    virtual void close()
    {
      events_.reset();
    }
  public:
    virtual int native_handle() const = 0;
  public:
    void wait_writable()
    {
      if ( events_ ) {
        events_->wait_writable();
      }
      else {
        throw std::runtime_error("io not attached to a task!");
      }
    }
  public:
    void wait_readable()
    {
      if ( events_ ) {
        events_->wait_readable();
      }
      else {
        throw std::runtime_error("io not attached to a task!");
      }
    }
  public:
    void task_attach(task* task)
    {
      events_.reset(new io_task_events(native_handle(), task));
    }
  public:
    void task_detach(task* task)
    {
      events_.reset();
    }
  private:
    std::unique_ptr<io_task_events> events_;
  };
}

// ----------------------------------------------------------------------------
#endif