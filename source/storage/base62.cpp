// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "base62.h"

// ----------------------------------------------------------------------------
#include <algorithm>

// ----------------------------------------------------------------------------
namespace musciteer
{
  char alphabet[] = "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz";

  auto base       = sizeof(alphabet)-1;

  std::string base62_encode(unsigned long long value)
  {
      std::string result;

      do
      {
        result.push_back(alphabet[value % base]);
        value /= base;
      }
      while(value > 0);

      std::reverse(result.begin(), result.end());

      return std::move(result);
  }

  unsigned long long base62_decode(const std::string& value)
  {
    unsigned long long result = 0;

    auto len = value.length();

    for ( unsigned i=len; i>0; i-- )
    {
      int c = value[i-1];
      int v;

      if ( c >= '0' && c <= '9') {
        v = c - '0';
      }
      else if ( c >= 'A' && c <= 'Z' ) {
        v = c - 'A' + 10;
      }
      else if ( c >= 'a' && c <= 'z' ) {
        v = c - 'a' + 36;
      }
      else {
        v = 0;
      }

      result += v * std::pow(62, len-i);
    }

    return result;
  }
}
