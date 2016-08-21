// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__dm__album_h__
#define __musciteer__dm__album_h__

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
    class track;
    class artist;

    class album
    {
      using track_id_list = std::set<std::string>;
      using alt_id_list = std::set<std::string>;
    public:
      album();
    public:
      bool id_is_null();
    public:
      const std::string& id() const;
      const std::string& title() const;
      const track_id_list& track_ids() const;
    public:
      dm::artist artist() const;
    public:
      bool alt_ids_find(const std::string& alt_id) const;
    public:
      void id(const std::string&);
      void title(const std::string&);
    public:
      void tracks_add(const dm::track&);
      void tracks_del(const dm::track&);
      void tracks_each(std::function<void(const dm::track& track)>) const;
    public:
      void artist(const dm::artist&);
    public:
      void alt_ids_add(const std::string& alt_id);
    public:
      void read(msgpack::istream&);
      void write(msgpack::ostream&) const;
    private:
      std::string id_;
      std::string title_;
      track_id_list track_ids_;
      std::string artist_id_;
      alt_id_list alt_ids_;
    };

    inline msgpack::istream& operator>>(msgpack::istream& is, album& value)
    {
      value.read(is);
      return is;
    }

    inline msgpack::ostream& operator<<(msgpack::ostream& os, const album& value)
    {
      value.write(os);
      return os;
    }
  }
}

// ----------------------------------------------------------------------------
#endif