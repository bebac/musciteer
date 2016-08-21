// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__base62_h__
#define __musciteer__base62_h__

// ----------------------------------------------------------------------------
#include <string>

// ----------------------------------------------------------------------------
namespace musciteer
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
