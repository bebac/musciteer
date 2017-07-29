// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__dm__track_h__
#define __musciteer__dm__track_h__

// ----------------------------------------------------------------------------
#include "album.h"
#include "artist.h"
#include "track_source.h"

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
    class track
    {
      using artist_id_list = std::vector<std::string>;
      using source_list = std::vector<track_source>;
      using tag_list = std::set<std::string>;
    public:
      track();
    public:
      bool id_is_null() const;
    public:
      const std::string& id() const;
      const std::string& title() const;
      unsigned track_number() const;
      unsigned disc_number() const;
      unsigned duration() const;
      dm::album album() const;
      std::vector<dm::artist> artists() const;
      const track_source& sources_get(const std::string& name) const;
      source_list sources() const;
      void sources_each(std::function<void(const dm::track_source& source)>) const;
      unsigned play_count() const;
      unsigned skip_count() const;
      const tag_list& tags() const;
    public:
      void id(const std::string&);
      void title(const std::string&);
      void track_number(unsigned);
      void disc_number(unsigned);
      void duration(unsigned);
      void album(const dm::album&);
      void artists_add(const dm::artist&);
    public:
      void sources_add(const dm::track_source&);
      void sources_remove(const std::string&);
    public:
      void increment_play_count();
      void increment_skip_count();
    public:
      void tags(const tag_list&);
    public:
      void read(msgpack::istream& is);
      void write(msgpack::ostream& os) const;
    private:
      std::string id_;
      std::string title_;
      unsigned tn_;
      unsigned dn_;
      unsigned duration_;
      std::string album_id_;
      artist_id_list artist_ids_;
      source_list sources_;
      unsigned play_count_;
      unsigned skip_count_;
      tag_list tags_;
    };

    inline msgpack::istream& operator>>(msgpack::istream& is, track& value)
    {
      value.read(is);
      return is;
    }

    inline msgpack::ostream& operator<<(msgpack::ostream& os, const track& value)
    {
      value.write(os);
      return os;
    }
  }
}

// ----------------------------------------------------------------------------
#endif
