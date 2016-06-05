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
  class io
  {
  public:
    virtual ssize_t recv(void *buf, size_t len, int flags=0) = 0;
    virtual ssize_t send(const void *buf, size_t len, int flags=0) = 0;
  public:
    virtual int native_handle() const = 0;
  };
}

// ----------------------------------------------------------------------------
#endif