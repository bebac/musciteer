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
  public:
    album() : id_(), title_()
    {
    }
  public:
    const std::string& id() const
    {
      return id_;
    }
  public:
    void id(const std::string& id)
    {
      id_ = id;
    }
  public:
    const std::string& title() const
    {
      return title_;
    }
  public:
    void title(const std::string& title)
    {
      title_ = title;
    }
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
              case 1: is >> id_; break;
              case 2: is >> title_; break;
              case 3: is >> track_ids; break;
              default:
                //is >> msgpack::skip;
                break;
            }
          }
          else
          {
            // ERROR!
            std::cout << "failed to read album key" << std::endl;
          }
        }
      }
      else
      {
        // ERROR!
        std::cout << "failed to read album map" << std::endl;
      }
    }
  public:
    void write(msgpack::ostream& os) const
    {
      msgpack::map map{2};

      os << map
        << 1 << id_
        << 2 << title_
        << 3 << track_ids;
    }
  private:
    std::string id_;
    std::string title_;
    track_id_list track_ids;
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