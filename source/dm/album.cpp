// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "album.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  album::album() : id_(), title_(), track_ids_(), alt_ids_()
  {
  }

  const std::string& album::id() const
  {
    return id_;
  }

  const std::string& album::title() const
  {
    return title_;
  }

  void album::id(const std::string& id)
  {
    id_ = id;
  }

  void album::title(const std::string& title)
  {
    title_ = title;
  }

  void album::read(msgpack::istream& is)
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
            case 3: is >> track_ids_; break;
            case 4: is >> alt_ids_; break;
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

  void album::write(msgpack::ostream& os) const
  {
    msgpack::map map{2};

    os << map
      << 1 << id_
      << 2 << title_
      << 3 << track_ids_
      << 4 << alt_ids_;
  }
}