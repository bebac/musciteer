// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "track_source.h"

// ----------------------------------------------------------------------------
namespace musciteer
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

    const replay_gain_value& track_source::rg_ref_loudness() const
    {
      return rg_ref_loudness_;
    }

    const replay_gain_value& track_source::rg_track_gain() const
    {
      return rg_track_gain_;
    }

    const replay_gain_value& track_source::rg_track_peak() const
    {
      return rg_track_peak_;
    }

    void track_source::name(const std::string& name)
    {
      name_ = name;
    }

    void track_source::uri(const std::string& uri)
    {
      uri_ = uri;
    }

    void track_source::rg_ref_loudness(replay_gain_value value)
    {
      rg_ref_loudness_ = value;
    }

    void track_source::rg_track_gain(replay_gain_value value)
    {
      rg_track_gain_ = value;
    }

    void track_source::rg_track_peak(replay_gain_value value)
    {
      rg_track_peak_ = value;
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
              case 3: is >> rg_ref_loudness_; break;
              case 4: is >> rg_track_gain_; break;
              case 5: is >> rg_track_peak_; break;
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
      auto size = size_t{2};

      if ( rg_ref_loudness_ ) {
        size += 1;
      }
      if ( rg_track_gain_ ) {
        size += 1;
      }
      if ( rg_track_peak_ ) {
        size += 1;
      }

      msgpack::map map{size};

      os << map
        << 1 << name_
        << 2 << uri_;

      if ( rg_ref_loudness_ ) {
        os << 3 << rg_ref_loudness_;
      }
      if ( rg_track_gain_ ) {
        os << 4 << rg_track_gain_;
      }
      if ( rg_track_peak_ ) {
        os << 5 << rg_track_peak_;
      }
    }
  }
}