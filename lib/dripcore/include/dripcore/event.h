// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __dripcore__event_h__
#define __dripcore__event_h__

// ----------------------------------------------------------------------------
#include <sys/eventfd.h>

// ----------------------------------------------------------------------------
namespace dripcore
{
  class event
  {
  public:
    event()
    {
      if ( (fd_ = eventfd(0, EFD_NONBLOCK)) == -1 ) {
        throw std::system_error(errno, std::system_category());
      }
    }
  public:
    ~event()
    {
      close(fd_);
    }
  public:
    void notify()
    {
      uint64_t v = 1;
      if ( write(fd_, &v, sizeof(uint64_t)) < 0 ) {
        throw std::system_error(errno, std::system_category());
      }
    }
  public:
    size_t read()
    {
      uint64_t v;

      ssize_t res = ::read(fd_, &v, sizeof(uint64_t));

      if ( res == sizeof(uint64_t) ) {
        return res;
      }
      else if ( res < 0 && errno == EAGAIN ) {
        return 0;
      }
      else {
        throw std::system_error(errno, std::system_category());
      }
    }
  public:
    int native_handle() const
    {
      return fd_;
    }
  private:
    int fd_;
  };
}

// ----------------------------------------------------------------------------
#endif