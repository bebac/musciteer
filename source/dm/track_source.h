// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musicbox__dm__track_source_h__
#define __musicbox__dm__track_source_h__

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
  class track_source
  {
  public:
    track_source();
    track_source(const std::string& name, const std::string& uri);
  public:
    bool operator==(const track_source& rhs)
    {
      return name_ == rhs.name() ? true : false;
    }
  public:
    const std::string& name() const;
    const std::string& uri() const;
  public:
    void name(const std::string&);
    void uri(const std::string&);
  public:
    void read(msgpack::istream& is);
    void write(msgpack::ostream& os) const;
  private:
    std::string name_;
    std::string uri_;
  };

  inline msgpack::istream& operator>>(msgpack::istream& is, track_source& value)
  {
    value.read(is);
    return is;
  }

  inline msgpack::ostream& operator<<(msgpack::ostream& os, const track_source& value)
  {
    value.write(os);
    return os;
  }
}

// ----------------------------------------------------------------------------
#endif