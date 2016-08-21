// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__dm__player_h__
#define __musciteer__dm__player_h__

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"

// ----------------------------------------------------------------------------
#include <msgpack/istream.h>
#include <msgpack/ostream.h>
#include <msgpack/map.h>
#include <msgpack/array.h>

// ----------------------------------------------------------------------------
namespace musciteer
{
  namespace dm
  {
    struct player_data
    {
    public:
      void read(msgpack::istream& is)
      {
        msgpack::map map;

        if ( is >> map )
        {
          for ( size_t i=0; i<map.size(); i++ )
          {
            unsigned key;

            if ( is >> key )
            {
              switch ( key )
              {
                case 1: is >> ctpb_enabled; break;
                case 2: is >> ctpb_type; break;
                case 3: is >> audio_device; break;
                default:
                  is >> msgpack::skip;
                  break;
              }
            }
            else
            {
              // ERROR!
              std::cout << "failed to read player key" << std::endl;
            }
          }
        }
        else
        {
          // ERROR!
          std::cout << "failed to read player map" << std::endl;
        }
      }
    public:
      void write(msgpack::ostream& os) const
      {
        msgpack::map map{3};

        os << map
          << 1 << ctpb_enabled
          << 2 << ctpb_type
          << 3 << audio_device;
      }
    public:
      bool ctpb_enabled;
      std::string ctpb_type;
      std::string audio_device;
    };

    class player
    {
    public:
      player() : kvstore_(musciteer::kvstore())
      {
        if ( !kvstore_.get(player_key, data_) )
        {
          data_.ctpb_enabled = true;
          data_.ctpb_type = "random";
          data_.audio_device = "default";
        }
      }
    public:
      bool ctpb_enabled() const
      {
        return data_.ctpb_enabled;
      }
    public:
      void ctpb_enabled(bool value)
      {
        data_.ctpb_enabled = value;
      }
    public:
      const std::string& ctpb_type() const
      {
        return data_.ctpb_type;
      }
    public:
      void ctpb_type(const std::string& value)
      {
        data_.ctpb_type = value;
      }
    public:
      const std::string& audio_device() const
      {
        return data_.audio_device;
      }
    public:
      void audio_device(const std::string& value)
      {
        data_.audio_device = value;
      }
    public:
      void save()
      {
        kvstore_.set(player_key, data_);
      }
    private:
      kvstore kvstore_;
    private:
      player_data data_;
    private:
      static constexpr const char* player_key = "__player__";
    };

    inline msgpack::istream& operator>>(msgpack::istream& is, player_data& value)
    {
      value.read(is);
      return is;
    }

    inline msgpack::ostream& operator<<(msgpack::ostream& os, const player_data& value)
    {
      value.write(os);
      return os;
    }
  }
}

// ----------------------------------------------------------------------------
#endif
