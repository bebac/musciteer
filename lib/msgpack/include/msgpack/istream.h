// ----------------------------------------------------------------------------
//
//     Filename   : msgpack/istream.h
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __msgpack__istream_h__
#define __msgpack__istream_h__

// ----------------------------------------------------------------------------
#include "msgpack.h"

// ----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <chrono>

// ----------------------------------------------------------------------------
namespace msgpack
{
  namespace details
  {
    class skip {};
  }

  namespace
  {
    const details::skip skip{};
  }

  class istream : public std::istream
  {
  public:
    istream(std::streambuf* buf);
  public:
    //istream& operator>>(char&);
    //istream& operator>>(short&);
    //istream& operator>>(unsigned short&);
    istream& operator>>(const details::skip&);
    istream& operator>>(int&);
    istream& operator>>(unsigned int&);
    istream& operator>>(long long&);
    istream& operator>>(unsigned long long&);
    istream& operator>>(bool&);
    istream& operator>>(float&);
  public:
    istream& operator>>(std::string&);
  public:
    char read_i8();
    unsigned char read_u8();
    short read_i16();
    unsigned short read_u16();
    int read_i32();
    unsigned read_u32();
    long long read_i64();
    unsigned long long read_u64();
    bool read_bool();
    float read_float();
  public:
    void read_string(std::string&);
    void read_string(std::string&, std::size_t);
  private:
    void skip();
  public:
    template<typename T> T read();
  };

  inline istream& istream::operator>>(const details::skip& value)
  {
    skip();
    return *this;
  }

  inline istream& istream::operator>>(int& value)
  {
    value = read_i32();
    return *this;
  }

  inline istream& istream::operator>>(unsigned int& value)
  {
    value = read_u32();
    return *this;
  }

  inline istream& istream::operator>>(long long& value)
  {
    value = read_i64();
    return *this;
  }

  inline istream& istream::operator>>(unsigned long long& value)
  {
    value = read_u64();
    return *this;
  }

  inline istream& istream::operator>>(bool& value)
  {
    value = read_bool();
    return *this;
  }

  inline istream& istream::operator>>(float& value)
  {
    value = read_float();
    return *this;
  }

  inline istream& istream::operator>>(std::string& value)
  {
    read_string(value);
    return *this;
  }

  inline char istream::read_i8()
  {
    return read<char>();
  }

  inline unsigned char istream::read_u8()
  {
    return read<unsigned char>();
  }

  inline short istream::read_i16()
  {
    return read<short>();
  }

  inline unsigned short istream::read_u16()
  {
    return read<unsigned short>();
  }

  template<typename T>
  T istream::read()
  {
    if ( sizeof(T) == 1 )
    {
      return T(get());
    }
    else if ( sizeof(T) == 2 )
    {
      T v = 0;

      v |= get() << 8;
      v |= get();

      return v;
    }
    else if ( sizeof(T) == 4 )
    {
      T v = 0;

      v |= get() << 24;
      v |= get() << 16;
      v |= get() << 8;
      v |= get();

      return v;
    }
    else if ( sizeof(T) == 8 )
    {
      union
      {
        T v;
        unsigned char b[8];
      } tmp;

      for ( auto i = 0; i < 8; ++i ) {
        tmp.b[i] = get();
      }

      return be64toh(tmp.v);
    }
    else
    {
      throw std::runtime_error("msgpack istream read sizeof(T) > 8");
    }
  }

  inline istream& operator>>(msgpack::istream& is, std::chrono::system_clock::time_point& value)
  {
    auto c = is.get();

    if ( c == 0xd7 )
    {
      c = is.get();

      if ( c == time_point_tag )
      {
        value = std::chrono::system_clock::time_point(std::chrono::nanoseconds(is.read<long long>()));
      }
      else
      {
        is.setstate(std::ios_base::failbit);
      }
    }
    else
    {
      is.setstate(std::ios_base::failbit);
    }

    return is;
  }
}

// ----------------------------------------------------------------------------
#endif