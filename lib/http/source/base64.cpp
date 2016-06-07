// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include <http/base64.h>

// ----------------------------------------------------------------------------
#include <iostream>
#include <stdexcept>

// ----------------------------------------------------------------------------
namespace base64
{
  inline unsigned to6bit1(unsigned v) { return (v & 0x00fc0000) >> 18; }
  inline unsigned to6bit2(unsigned v) { return (v & 0x0003f000) >> 12; }
  inline unsigned to6bit3(unsigned v) { return (v & 0x00000fc0) >>  6; }
  inline unsigned to6bit4(unsigned v) { return (v & 0x0000003f);       }

  typedef unsigned char byte;

  char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz"
                    "0123456789+/";
  char pad        = '=';
  char newline    = '\n';
  auto base       = sizeof(alphabet)-1;

  std::string encode(const void* data, size_t len)
  {
    std::string result;

    const byte* buf = static_cast<const byte*>(data);

    result.reserve((len / 3 + (len % 3 > 0)) * 4);

    for ( size_t i = 0; i < len / 3; i++ )
    {
      unsigned tmp = 0;

      tmp += (*buf++) << 16;
      tmp += (*buf++) << 8;
      tmp += (*buf++);

      result.push_back(alphabet[to6bit1(tmp)]);
      result.push_back(alphabet[to6bit2(tmp)]);
      result.push_back(alphabet[to6bit3(tmp)]);
      result.push_back(alphabet[to6bit4(tmp)]);
    }

    switch ( len % 3 )
    {
      case 1:
      {
        unsigned tmp = 0;

        tmp += (*buf++) << 16;

        result.push_back(alphabet[to6bit1(tmp)]);
        result.push_back(alphabet[to6bit2(tmp)]);
        result.push_back(pad);
        result.push_back(pad);
        break;
      }
      case 2:
      {
        unsigned tmp = 0;

        tmp += (*buf++) << 16;
        tmp += (*buf++) << 8;

        result.push_back(alphabet[to6bit1(tmp)]);
        result.push_back(alphabet[to6bit2(tmp)]);
        result.push_back(alphabet[to6bit3(tmp)]);
        result.push_back(pad);
        break;
      }
    }

    return result;
  }

  std::string decode(const void* data, size_t len)
  {
#if 0
    if ( len % 4 ) {
      throw std::runtime_error("invlid base64 encoding");
    }
#endif

    std::string result;
    size_t      padding = 0;

    const byte* buf = static_cast<const byte*>(data);

    if ( len > 0 && buf[len-1] == newline )
      len--;

    if ( len > 0 && buf[len-1] == pad )
      padding++;

    if ( len > 1 && buf[len-2] == pad )
      padding++;

    result.reserve(((len / 4) * 3) - padding);

    for ( size_t i = 0; i < len; )
    {
      unsigned tmp = 0;

      for ( size_t q = 0; q < 4 && i < len; q++, i++ )
      {
        if ( buf[i] != newline )
        {
          tmp <<= 6;

          if ( buf[i] >= 0x41 && buf[i] <= 0x5a ) {
            tmp |= buf[i] - 0x41;
          }
          else if ( buf[i] >= 0x61 && buf[i] <= 0x7a ) {
            tmp |= buf[i] - 0x47;
          }
          else if ( buf[i] >= 0x30 && buf[i] <= 0x39 ) {
            tmp |= buf[i] + 0x04;
          }
          else if ( buf[i] == 0x2b ) {
            tmp |= 0x3E;
          }
          else if ( buf[i] == 0x2f ) {
            tmp |= 0x3F;
          }
          else if ( buf[i] == pad )
          {
            switch( padding )
            {
            case 1:
              result.push_back((tmp >> 16) & 0xff);
              result.push_back((tmp >> 8 ) & 0xff);
              return result;
            case 2:
              result.push_back((tmp >> 10) & 0xff);
              return result;
            default:
              throw std::runtime_error("invalid padding in base 64!");
            }
          }
          else
          {
            throw std::runtime_error("invalid character in base 64!");
          }
        }
        else
        {
          q--;
        }
      }

      result.push_back((tmp >> 16) & 0xff);
      result.push_back((tmp >> 8 ) & 0xff);
      result.push_back((tmp      ) & 0xff);
    }

    return result;
  }
}
