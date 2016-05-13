// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musicbox__dm__track_h__
#define __musicbox__dm__track_h__

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
namespace musicbox
{
  class track
  {
    using artist_id_list = std::vector<std::string>;
    using source_list = std::vector<track_source>;
  public:
    track();
  public:
    const std::string& id() const;
    const std::string& title() const;
    unsigned track_number() const;
    unsigned disc_number() const;
    unsigned duration() const;
    musicbox::album album() const;
    std::vector<musicbox::artist> artists() const;
    source_list sources() const;
  public:
    void id(const std::string&);
    void title(const std::string&);
    void track_number(unsigned);
    void disc_number(unsigned);
    void duration(unsigned);
    void album(const musicbox::album&);
    void artists_add(const musicbox::artist&);
  public:
    void sources_add(const musicbox::track_source&);
    void sources_del(const musicbox::track_source&);
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

// ----------------------------------------------------------------------------
#endif