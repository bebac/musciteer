// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musicbox__dm__source_spotify_h__
#define __musicbox__dm__source_spotify_h__

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"

// ----------------------------------------------------------------------------
#include <msgpack/istream.h>
#include <msgpack/ostream.h>
#include <msgpack/map.h>
#include <msgpack/array.h>

// ----------------------------------------------------------------------------
namespace musicbox
{
  namespace dm
  {
    struct source_spotify_data
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
                case 1: is >> username; break;
                case 2: is >> password; break;
                default:
                  //is >> msgpack::skip;
                  break;
              }
            }
            else
            {
              // ERROR!
              std::cout << "failed to read spotify source key" << std::endl;
            }
          }
        }
        else
        {
          // ERROR!
          std::cout << "failed to read spotify source map" << std::endl;
        }
      }
    public:
      void write(msgpack::ostream& os) const
      {
        msgpack::map map{2};

        os << map
          << 1 << username
          << 2 << password;
      }
    public:
      std::string username;
      std::string password;
    };

    class source_spotify
    {
    public:
      source_spotify() : kvstore_(musicbox::kvstore())
      {
        kvstore_.get(source_spotify_settings_key, data_);
      }
    public:
      const std::string& username()
      {
        return data_.username;
      }
    public:
      void username(const std::string& username)
      {
        data_.username = username;
      }
    public:
      const std::string& password()
      {
        return data_.password;
      }
    public:
      void password(const std::string& password)
      {
        data_.password = password;
      }
    public:
      void save()
      {
        kvstore_.set(source_spotify_settings_key, data_);
      }
    private:
      kvstore kvstore_;
    private:
      source_spotify_data data_;
    private:
      static constexpr const char* source_spotify_settings_key = "__source_spotify_settings__";
    };

    inline msgpack::istream& operator>>(msgpack::istream& is, source_spotify_data& value)
    {
      value.read(is);
      return is;
    }

    inline msgpack::ostream& operator<<(msgpack::ostream& os, const source_spotify_data& value)
    {
      value.write(os);
      return os;
    }
  }
}

// ----------------------------------------------------------------------------
#endif
