// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musicbox__dm__album_h__
#define __musicbox__dm__album_h__

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
  class album
  {
    using track_id_list = std::set<std::string>;
    using alt_id_list = std::set<std::string>;
  public:
    album();
  public:
    const std::string& id() const;
    const std::string& title() const;
  public:
    void id(const std::string&);
    void title(const std::string&);
  public:
    void read(msgpack::istream&);
    void write(msgpack::ostream&) const;
  private:
    std::string id_;
    std::string title_;
    track_id_list track_ids_;
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

// ----------------------------------------------------------------------------
#endif