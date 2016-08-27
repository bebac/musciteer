// ----------------------------------------------------------------------------
//
//     Filename   : msgpack/ostream.h
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __msgpack__ostream_h__
#define __msgpack__ostream_h__

// ----------------------------------------------------------------------------
#include <iostream>
#include <cstring>
#include <vector>

// ----------------------------------------------------------------------------
#include <endian.h>

// ----------------------------------------------------------------------------
namespace msgpack
{
  class ostream : public std::ostream
  {
  public:
    ostream(std::streambuf* buf);
  public:
    ostream& operator<<(char);
    ostream& operator<<(unsigned char);
    ostream& operator<<(short);
    ostream& operator<<(unsigned short);
    ostream& operator<<(int);
    ostream& operator<<(unsigned int);
    ostream& operator<<(long);
    ostream& operator<<(unsigned long);
    ostream& operator<<(long long);
    ostream& operator<<(unsigned long long);
    ostream& operator<<(float);
    ostream& operator<<(double);
    ostream& operator<<(long double);
    ostream& operator<<(bool);
  public:
    ostream& operator<<(const char*);
  protected:
    void write_i16(short);
    void write_u16(unsigned short);
    void write_i32(int);
    void write_u32(unsigned int);
    void write_i64(long long);
    void write_u64(unsigned long long);
    void write_bool(bool);
    void write_float(float);
  public:
    void write_string(const char*, size_t);
  public:
    template<typename T> void put(unsigned char type, T v);
    void put(unsigned char type);
  };

  inline ostream& ostream::operator<<(char value)
  {
    std::ostream::put(value);
    return *this;
  }

  inline ostream& ostream::operator<<(unsigned char value)
  {
    write_u16(value);
    return *this;
  }

  inline ostream& ostream::operator<<(short value)
  {
    write_i16(value);
    return *this;
  }

  inline ostream& ostream::operator<<(unsigned short value)
  {
    write_u16(value);
    return *this;
  }

  inline ostream& ostream::operator<<(int value)
  {
    write_i32(value);
    return *this;
  }

  inline ostream& ostream::operator<<(unsigned int value)
  {
    write_u32(value);
    return *this;
  }

  inline ostream& ostream::operator<<(long value)
  {
    write_i64(value);
    return *this;
  }

  inline ostream& ostream::operator<<(long long value)
  {
    write_i64(value);
    return *this;
  }

  inline ostream& ostream::operator<<(unsigned long long value)
  {
    write_u64(value);
    return *this;
  }

  inline ostream& ostream::operator<<(bool value)
  {
    write_bool(value);
    return *this;
  }

  inline ostream& ostream::operator<<(float value)
  {
    write_float(value);
    return *this;
  }

  inline ostream& ostream::operator<<(const char* value)
  {
    write_string(value, std::strlen(value));
    return *this;
  }

  template<typename T>
  void ostream::put(unsigned char type, T v)
  {
    std::ostream::put(type);
    if ( sizeof(T) == 1 )
    {
      std::ostream::put(v);
    }
    else if ( sizeof(T) == 2 )
    {
      std::ostream::put(v >> 8);
      std::ostream::put(v);
    }
    else if ( sizeof(T) == 4 )
    {
      std::ostream::put(v >> 24);
      std::ostream::put(v >> 16);
      std::ostream::put(v >> 8);
      std::ostream::put(v);
    }
    else if ( sizeof(T) == 8 )
    {
      union
      {
        T v;
        unsigned char b[8];
      } tmp{T(htobe64(v))};

      for ( auto i = 0; i < 8; ++i ) {
        std::ostream::put(tmp.b[i]);
      }
    }
  }

  inline void ostream::put(unsigned char type)
  {
    std::ostream::put(type);
  }

  inline ostream& operator<<(ostream& os, const std::string& value)
  {
    os.write_string(value.data(), value.length());
    return os;
  }
}

// ----------------------------------------------------------------------------
#endif