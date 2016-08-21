// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__dm__album_cover_h__
#define __musciteer__dm__album_cover_h__

// ----------------------------------------------------------------------------
#include <msgpack/istream.h>
#include <msgpack/ostream.h>
#include <msgpack/map.h>
#include <msgpack/array.h>

// ----------------------------------------------------------------------------
#include <string>

// ----------------------------------------------------------------------------
namespace musciteer
{
  namespace dm
  {
    class album_cover
    {
    public:
      album_cover();
    public:
      const std::string& mime_type() const;
      const std::string& data() const;
    public:
      void mime_type(const std::string&);
      void data(const unsigned char*, size_t);
    public:
      void read(msgpack::istream&);
      void write(msgpack::ostream&) const;
    private:
      std::string mime_type_;
      std::string data_;
    };

    inline msgpack::istream& operator>>(msgpack::istream& is, album_cover& value)
    {
      value.read(is);
      return is;
    }

    inline msgpack::ostream& operator<<(msgpack::ostream& os, const album_cover& value)
    {
      value.write(os);
      return os;
    }
  }
}

// ----------------------------------------------------------------------------
#endif
