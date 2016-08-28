// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__dm__track_source_h__
#define __musciteer__dm__track_source_h__

// ----------------------------------------------------------------------------
#include "replay_gain_value.h"

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
      const replay_gain_value& rg_ref_loudness() const;
      const replay_gain_value& rg_track_gain() const;
      const replay_gain_value& rg_track_peak() const;
    public:
      void name(const std::string&);
      void uri(const std::string&);
      void rg_ref_loudness(replay_gain_value);
      void rg_track_gain(replay_gain_value);
      void rg_track_peak(replay_gain_value);
    public:
      void read(msgpack::istream& is);
      void write(msgpack::ostream& os) const;
    private:
      std::string name_;
      std::string uri_;
      replay_gain_value rg_ref_loudness_;
      replay_gain_value rg_track_gain_;
      replay_gain_value rg_track_peak_;
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
}

// ----------------------------------------------------------------------------
#endif