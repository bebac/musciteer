// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__dm__spotify_web_api_h__
#define __musciteer__dm__spotify_web_api_h__

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"

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
    using clock = std::chrono::system_clock;

    class spotify_web_api_data
    {
      using string = std::string;
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
                case 1: is >> access_token; break;
                case 2: is >> refresh_token; break;
                case 3: is >> expires_at; break;
                default:
                  is >> msgpack::skip;
                  break;
              }
            }
            else
            {
              // ERROR!
              std::cout << "failed to read spotify web api data key" << std::endl;
            }
          }
        }
        else
        {
          // ERROR!
          std::cout << "failed to read spotify web api data map" << std::endl;
        }
      }
    public:
      void write(msgpack::ostream& os) const
      {
        msgpack::map map{3};

        os << map
          << 1 << access_token
          << 2 << refresh_token
          << 3 << expires_at;
      }
    public:
      string access_token;
      string refresh_token;
      clock::time_point expires_at;
    };

    class spotify_web_api
    {
    public:
      using clock = std::chrono::system_clock;
    public:
      spotify_web_api() : kvstore_(musciteer::kvstore())
      {
        kvstore_.get(spotify_web_api_key, data_);
      }
    public:
      const std::string& access_token()
      {
        return data_.access_token;
      }
    public:
      void access_token(const std::string& access_token)
      {
        data_.access_token = access_token;
      }
      void access_token(std::string&& access_token)
      {
        data_.access_token = std::move(access_token);
      }
    public:
      const std::string& refresh_token()
      {
        return data_.refresh_token;
      }
    public:
      void refresh_token(const std::string& refresh_token)
      {
        data_.refresh_token = refresh_token;
      }
      void refresh_token(std::string&& refresh_token)
      {
        data_.refresh_token = std::move(refresh_token);
      }
    public:
      clock::time_point expires_at()
      {
        return data_.expires_at;
      }
    public:
      void expires_at(unsigned expire_in)
      {
        data_.expires_at = clock::now() + std::chrono::seconds(expire_in);
      }
    public:
      void save()
      {
        kvstore_.set(spotify_web_api_key, data_);
      }
    public:
      static void remove()
      {
        auto kvstore = musciteer::kvstore();
        kvstore.remove(spotify_web_api_key);
      }
    private:
      kvstore kvstore_;
    private:
      spotify_web_api_data data_;
    private:
      static constexpr const char* spotify_web_api_key = "__spotify_web_api__";
    };

    inline msgpack::istream& operator>>(msgpack::istream& is, spotify_web_api_data& value)
    {
      value.read(is);
      return is;
    }

    inline msgpack::ostream& operator<<(msgpack::ostream& os, const spotify_web_api_data& value)
    {
      value.write(os);
      return os;
    }
  }
}

// ----------------------------------------------------------------------------
#endif

