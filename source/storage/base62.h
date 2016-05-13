// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musicbox__base62_h__
#define __musicbox__base62_h__

// ----------------------------------------------------------------------------
#include <string>

// ----------------------------------------------------------------------------
namespace musicbox
{
  /////
  // Encode 64 bit integer as base63 encoded string.

  std::string base62_encode(unsigned long long value);

  /////
  // Decode base62 encoded string to 64 bit integer.

  unsigned long long base62_decode(const std::string& value);
}

// ----------------------------------------------------------------------------
#endif
