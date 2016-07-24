// ----------------------------------------------------------------------------
//
//     Filename   : msgpack/array.h
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __msgpack__array_h__
#define __msgpack__array_h__

// ----------------------------------------------------------------------------
#include <msgpack/istream.h>
#include <msgpack/ostream.h>

// ----------------------------------------------------------------------------
#include <string>
#include <vector>
#include <set>

// ----------------------------------------------------------------------------
namespace msgpack
{
  class array
  {
  public:
    array() : size_(0)
    {
    }
    array(size_t size) : size_(size)
    {
    }
  public:
    void size(size_t size)
    {
      size_ = size;
    }
  public:
    size_t size() const
    {
      return size_;
    }
  public:
    void write(ostream& os) const
    {
      if ( size_ < 0x10 )
      {
        os.put(0x90 | size_);
      }
      else if ( size_ < 0x10000 )
      {
        os.put(0xdc, static_cast<unsigned short>(size_));
      }
      else
      {
        os.put(0xdd, static_cast<unsigned int>(size_));
      }
    }
  public:
    void read(istream& is)
    {
      auto c = is.get();

      if ( c >= 0x90 && c <= 0x9f )
      {
        size(c & 0xf);
      }
      else if ( c == 0xdc )
      {
        size(is.read_u16());
      }
      else if ( c == 0xdd )
      {
        size(is.read_u32());
      }
      else
      {
        is.setstate(std::ios_base::failbit);
      }
    }
  private:
    size_t size_;
  };

  inline msgpack::istream& operator>>(msgpack::istream& is, array& value)
  {
    value.read(is);
    return is;
  }

  inline msgpack::ostream& operator<<(msgpack::ostream& os, const array& value)
  {
    value.write(os);
    return os;
  }

  template<typename T>
  inline msgpack::ostream& operator<<(msgpack::ostream& os, const std::vector<T>& value)
  {
    msgpack::array array{value.size()};

    os << array;
    for ( const auto& v : value ) {
      os << v;
    }

    return os;
  }

  template<typename T>
  inline msgpack::istream& operator>>(msgpack::istream& is, std::vector<T>& value)
  {
    msgpack::array array;

    if ( is >> array )
    {
      value.reserve(array.size());

      for ( size_t i=0; i<array.size(); i++ )
      {
        T v;
        if ( is >> v ) {
          value.push_back(v);
        }
        else {
          break;
        }
      }
    }

    return is;
  }

  template<typename T>
  inline msgpack::ostream& operator<<(msgpack::ostream& os, const std::set<T>& value)
  {
    msgpack::array array{value.size()};

    os << array;
    for ( const auto& v : value ) {
      os << v;
    }

    return os;
  }

  template<typename T>
  inline msgpack::istream& operator>>(msgpack::istream& is, std::set<T>& value)
  {
    msgpack::array array;

    if ( is >> array )
    {
      for ( size_t i=0; i<array.size(); i++ )
      {
        T v;
        if ( is >> v ) {
          value.emplace(v);
        }
        else {
          break;
        }
      }
    }

    return is;
  }

}

// ----------------------------------------------------------------------------
#endif