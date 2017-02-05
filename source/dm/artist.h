// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__dm__artist_h__
#define __musciteer__dm__artist_h__

// ----------------------------------------------------------------------------
#include <msgpack/istream.h>
#include <msgpack/ostream.h>
#include <msgpack/map.h>
#include <msgpack/array.h>

// ----------------------------------------------------------------------------
#include <string>
#include <set>
#include <vector>
#include <functional>

// ----------------------------------------------------------------------------
namespace musciteer
{
  namespace dm
  {
    class album;

    class artist
    {
      using album_id_list = std::set<std::string>;
    public:
      artist();
    public:
      bool id_is_null();
    public:
      const std::string& id() const;
      const std::string& name() const;
    public:
      void id(const std::string& id);
      void name(const std::string& name);
    public:
      void albums_add(const dm::album& album);
      void albums_del(const dm::album& album);
      void albums_each(std::function<void(const dm::album& album)>) const;
    public:
      void read(msgpack::istream& is);
      void write(msgpack::ostream& os) const;
    private:
      std::string id_;
      std::string name_;
      album_id_list album_ids_;
    };

    inline msgpack::istream& operator>>(msgpack::istream& is, artist& value)
    {
      value.read(is);
      return is;
    }

    inline msgpack::ostream& operator<<(msgpack::ostream& os, const artist& value)
    {
      value.write(os);
      return os;
    }
  }
}

// ----------------------------------------------------------------------------
#endif