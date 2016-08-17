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
#include <msgpack/istream.h>
#include <msgpack/array.h>
#include <msgpack/map.h>

// ----------------------------------------------------------------------------
#include <limits>

// ----------------------------------------------------------------------------
namespace msgpack
{
  istream::istream(std::streambuf* buf) : std::istream(buf)
  {
  }

  int istream::read_i32()
  {
    int value = 0;

    auto c = get();

    if ( c >= 0xe0  && c <= 0xff )
    {
      value = char(c);
    }
    else if ( c >= 0 && c <= 0x7f )
    {
      value = c;
    }
    else if ( c == 0xcc )
    {
      value = read<unsigned char>();
    }
    else if ( c == 0xd1 )
    {
      value = read<short>();
    }
    else if ( c == 0xcd )
    {
      value = read<unsigned short>();
    }
    else if ( c == 0xd2 )
    {
      value = read<int>();
    }
    else if ( c == 0xce )
    {
      auto v = read<unsigned int>();
      if ( v < std::numeric_limits<int>::max() )
      {
        value = v;
      }
      else
      {
        std::cerr << "read_i32 failed c=" << c << std::endl;
        setstate(std::ios_base::failbit);
      }
    }
    else
    {
      std::cerr << "read_i32 failed c=" << c << std::endl;
      setstate(std::ios_base::failbit);
    }

    return value;
  }

  unsigned int istream::read_u32()
  {
    unsigned int value = 0;

    auto c = get();

    if ( c >= 0xe0  && c <= 0xff )
    {
      std::cerr << "read_u32 failed c=" << c << std::endl;
      setstate(std::ios_base::failbit);
    }
    else if ( c >= 0 && c <= 0x7f )
    {
      value = c;
    }
    else if ( c == 0xcc )
    {
      value = read<unsigned char>();
    }
    else if ( c == 0xd1 )
    {
      auto v = read<short>();
      if ( v > 0 )
      {
        value = v;
      }
      else
      {
        std::cerr << "read_u32 failed c=" << c << std::endl;
        setstate(std::ios_base::failbit);
      }
    }
    else if ( c == 0xcd )
    {
      value = read<unsigned short>();
    }
    else if ( c == 0xd2 )
    {
      auto v = read<int>();
      if ( v > 0 )
      {
        value = v;
      }
      else
      {
        std::cerr << "read_u32 failed c=" << c << std::endl;
        setstate(std::ios_base::failbit);
      }
    }
    else if ( c == 0xce )
    {
      value = read<unsigned int>();
    }
    else
    {
      std::cerr << "read_u32 failed c=" << c << std::endl;
      setstate(std::ios_base::failbit);
    }

    return value;
  }

  long long istream::read_i64()
  {
    long long value = 0;

    auto c = get();

    if ( c >= 0xe0  && c <= 0xff )
    {
      value = char(c);
    }
    else if ( c >= 0 && c <= 0x7f )
    {
      value = c;
    }
    else if ( c == 0xcc )
    {
      value = read<unsigned char>();
    }
    else if ( c == 0xd1 )
    {
      value = read<short>();
    }
    else if ( c == 0xcd )
    {
      value = read<unsigned short>();
    }
    else if ( c == 0xd2 )
    {
      value = read<int>();
    }
    else if ( c == 0xd3 )
    {
      value = read<long long>();
    }
    else if ( c == 0xce )
    {
      value = read<unsigned int>();
    }
    else if ( c == 0xcf )
    {
      auto v = read<unsigned long long>();
      if ( v < std::numeric_limits<long long>::max() )
      {
        value = v;
      }
      else
      {
        std::cerr << "read_i64 failed c=" << c << std::endl;
        setstate(std::ios_base::failbit);
      }
    }
    else
    {
      std::cerr << "read_i32 failed c=" << c << std::endl;
      setstate(std::ios_base::failbit);
    }

    return value;
  }

  unsigned long long istream::read_u64()
  {
    unsigned long long value = 0;

    auto c = get();

    if ( c >= 0xe0  && c <= 0xff )
    {
      std::cerr << "read_u64 failed c=" << c << std::endl;
      setstate(std::ios_base::failbit);
    }
    else if ( c >= 0 && c <= 0x7f )
    {
      value = c;
    }
    else if ( c == 0xcc )
    {
      value = read<unsigned char>();
    }
    else if ( c == 0xd1 )
    {
      auto v = read<short>();
      if ( v > 0 )
      {
        value = v;
      }
      else
      {
        std::cerr << "read_u64 failed c=" << c << std::endl;
        setstate(std::ios_base::failbit);
      }
    }
    else if ( c == 0xcd )
    {
      value = read<unsigned short>();
    }
    else if ( c == 0xd2 )
    {
      auto v = read<int>();
      if ( v > 0 )
      {
        value = v;
      }
      else
      {
        std::cerr << "read_u64 failed c=" << c << std::endl;
        setstate(std::ios_base::failbit);
      }
    }
    else if ( c == 0xd3 )
    {
      auto v = read<long long>();
      if ( v > 0 )
      {
        value = v;
      }
      else
      {
        std::cerr << "read_u64 failed c=" << c << std::endl;
        setstate(std::ios_base::failbit);
      }
    }
    else if ( c == 0xce )
    {
      value = read<unsigned int>();
    }
    else if ( c == 0xcf )
    {
      value = read<unsigned long long>();
    }
    else
    {
      std::cerr << "read_u64 failed c=" << c << std::endl;
      setstate(std::ios_base::failbit);
    }

    return value;
  }

  bool istream::read_bool()
  {
    bool value = false;

    auto c = get();

    if ( c == 0xc2 )
    {
      value = false;
    }
    else if ( c == 0xc3 )
    {
      value = true;
    }
    else
    {
      setstate(std::ios_base::failbit);
    }

    return value;
  }

  void istream::read_string(std::string& value)
  {
    auto c = get();

    if ( c >= 0xa0 && c <= 0xbf )
    {
      read_string(value, c & 0x1f);
    }
    else if ( c == 0xd9 )
    {
      read_string(value, read<unsigned char>());
    }
    else if ( c == 0xda )
    {
      read_string(value, read<unsigned short>());
    }
    else if ( c == 0xdb )
    {
      read_string(value, read<unsigned int>());
    }
    else
    {
      setstate(std::ios_base::failbit);
    }
  }

  void istream::read_string(std::string& value, std::size_t len)
  {
    value.resize(len);
    std::istream::read(&value[0], len);
  }

  void istream::skip()
  {
    auto c = get();

    if ( c >= 0x00  && c <= 0x7f ) {
      // ignore positive fixint.
    }
    else if ( c >= 0xc0 && c <= 0xc3 ) {
      // ignore nil, false, true.
    }
    else if ( c == 0xcc || c == 0xd0 ) {
      ignore(sizeof(unsigned char));
    }
    else if ( c == 0xcd || c == 0xd1 ) {
      ignore(sizeof(unsigned short));
    }
    else if ( c == 0xce || c == 0xd2 ) {
      ignore(sizeof(unsigned int));
    }
    else if ( c == 0xcf || c == 0xd3 ) {
      ignore(sizeof(unsigned long long));
    }
    else if ( c >= 0xa0 && c <= 0xbf ) {
      ignore(c & 0x1f);
    }
    else if ( c == 0xd9 ) {
      ignore(read<unsigned char>());
    }
    else if ( c == 0xda ) {
      ignore(read<unsigned short>());
    }
    else if ( c == 0xdb ) {
      ignore(read<unsigned int>());
    }
    else if ( (c >= 0x90 && c <= 0x9f) || c == 0xdc || c == 0xdd )
    {
      msgpack::array arr;

      // Put back array identifier.
      unget();

      if ( *this >> arr )
      {
        for ( size_t i=0; i<arr.size(); i++ ) {
          *this >> msgpack::skip;
        }
      }
      else
      {
        setstate(std::ios_base::failbit);
      }
    }
    else if ( (c >= 0x80 && c <= 0x8f) || c == 0xde || c == 0xdf )
    {
      msgpack::map map;

      // Put back object identifier.
      unget();

      if ( *this >> map )
      {
        for ( size_t i=0; i<map.size(); i++ ) {
          *this >> msgpack::skip >> msgpack::skip;
        }
      }
      else
      {
        setstate(std::ios_base::failbit);
      }
    }
    else if ( c == 0xd5 ) {
      ignore(1 + sizeof(short));
    }
    else if ( c == 0xd6 ) {
      ignore(1 + sizeof(int));
    }
    else if ( c == 0xd7 ) {
      ignore(1 + sizeof(long long));
    }
  }
}
