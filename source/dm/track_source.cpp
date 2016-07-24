// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "track_source.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  namespace dm
  {
    track_source::track_source() : name_(), uri_()
    {
    }

    track_source::track_source(const std::string& name, const std::string& uri)
      : name_(name), uri_(uri)
    {
    }

    const std::string& track_source::name() const
    {
      return name_;
    }

    const std::string& track_source::uri() const
    {
      return uri_;
    }

    void track_source::name(const std::string& name)
    {
      name_ = name;
    }

    void track_source::uri(const std::string& uri)
    {
      uri_ = uri;
    }

    void track_source::read(msgpack::istream& is)
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
              case 1: is >> name_; break;
              case 2: is >> uri_; break;
              default:
                is >> msgpack::skip;
                break;
            }
          }
          else
          {
            // ERROR!
            std::cout << "failed to read track_source key" << std::endl;
          }
        }
      }
      else
      {
        // ERROR!
        std::cout << "failed to read track_source map" << std::endl;
      }
    }

    void track_source::write(msgpack::ostream& os) const
    {
      msgpack::map map{2};

      os << map
        << 1 << name_
        << 2 << uri_;
    }
  }
}