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
#include <iostream>
#include <string>

// ----------------------------------------------------------------------------
namespace msgpack
{
  class istream : public std::istream
  {
  public:
    istream(std::streambuf* buf);
  public:
    //istream& operator>>(char&);
    //istream& operator>>(short&);
    //istream& operator>>(unsigned short&);
    istream& operator>>(int&);
    istream& operator>>(unsigned int&);
    istream& operator>>(bool&);
  public:
    istream& operator>>(std::string&);
  public:
    char           read_i8();
    unsigned char  read_u8();
    short          read_i16();
    unsigned short read_u16();
    int            read_i32();
    unsigned       read_u32();
    bool           read_bool();
  public:
    void read_string(std::string&);
    void read_string(std::string&, std::size_t);
  private:
    template<typename T> T read();
  };

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

  inline istream& istream::operator>>(bool& value)
  {
    value = read_bool();
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
  }
}

// ----------------------------------------------------------------------------
#endif