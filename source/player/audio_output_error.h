// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__audio_output_error_h__
#define __player__audio_output_error_h__

// ----------------------------------------------------------------------------
#include <stdexcept>

// ----------------------------------------------------------------------------
class audio_output_error : public std::exception
{
public:
  audio_output_error(int error_code) : error_code_(error_code)
  {
  }
public:
  ~audio_output_error() = default;
public:
  int error_code() const
  {
    return error_code_;
  }
protected:
  int error_code_;
};

// ----------------------------------------------------------------------------
#endif
