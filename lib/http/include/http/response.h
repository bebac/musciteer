// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __http__response_h__
#define __http__response_h__

// ----------------------------------------------------------------------------
#include <iostream>

// ----------------------------------------------------------------------------
namespace http
{
  class response
  {
  public:
    response(std::basic_streambuf<char>* sbuf)
      :
      os_(sbuf)
    {
    }
  public:
    virtual ~response()
    {
      os_.flush();
    }
  public:
    template<typename T>
    std::ostream& operator<<(T value)
    {
      os_ << std::forward<T>(value);
      return os_;
    }
  private:
    std::ostream os_;
  };
}

// ----------------------------------------------------------------------------
#endif