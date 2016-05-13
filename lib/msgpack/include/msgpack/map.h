// ----------------------------------------------------------------------------
//
//     Filename   : msgpack/map.h
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __msgpack__map_h__
#define __msgpack__map_h__

// ----------------------------------------------------------------------------
#include <msgpack/istream.h>
#include <msgpack/ostream.h>

// ----------------------------------------------------------------------------
#include <map>

// ----------------------------------------------------------------------------
namespace msgpack
{
  class map
  {
  public:
    map() : size_(0)
    {
    }
    map(size_t size) : size_(size)
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
        os.put(0x80 | size_);
      }
      else if ( size_ < 0x10000 )
      {
        os.put(0xde, static_cast<unsigned short>(size_));
      }
      else
      {
        os.put(0xdf, static_cast<unsigned int>(size_));
      }
    }
  public:
    void read(istream& is)
    {
      auto c = is.get();

      if ( c >= 0x80 && c <= 0x8f )
      {
        size(c & 0xf);
      }
      else if ( c == 0xde )
      {
        size(is.read_u16());
      }
      else if ( c == 0xdf )
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

  inline msgpack::istream& operator>>(msgpack::istream& is, msgpack::map& value)
  {
    value.read(is);
    return is;
  }

  inline msgpack::ostream& operator<<(msgpack::ostream& os, const msgpack::map& value)
  {
    value.write(os);
    return os;
  }

  template<typename K, typename V>
  inline msgpack::ostream& operator<<(msgpack::ostream& os, const std::map<K,V>& value)
  {
    msgpack::map map{value.size()};

    os << map;
    for ( const auto& v : value ) {
      os << v.first << v.second;
    }

    return os;
  }

  template<typename K, typename V>
  inline msgpack::istream& operator>>(msgpack::istream& is, std::map<K,V>& value)
  {
    msgpack::map map;

    if ( is >> map )
    {
      value.reserve(map.size());

      for ( size_t i=0; i<map.size(); i++ )
      {
        std::pair<K,V> v;
        if ( is >> v.first >> v.second ) {
          value.emplace_back(v);
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