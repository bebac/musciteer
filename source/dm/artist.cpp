// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "artist.h"
#include "album.h"
#include "albums.h"

// ----------------------------------------------------------------------------
namespace musciteer
{
  namespace dm
  {
    artist::artist() : id_(), name_(), album_ids_()
    {
    }

    bool artist::id_is_null()
    {
      return id_.empty();
    }

    const std::string& artist::id() const
    {
      return id_;
    }

    const std::string& artist::name() const
    {
      return name_;
    }

    void artist::id(const std::string& id)
    {
      id_ = id;
    }

    void artist::name(const std::string& name)
    {
      name_ = name;
    }

    void artist::albums_add(const dm::album& album)
    {
      album_ids_.insert(album.id());
    }

    void artist::albums_each(std::function<void(const dm::album& album)> cb) const
    {
      auto albums = dm::albums();

      for ( auto& id : album_ids_ )
      {
        cb(albums.find_by_id(id));
      }
    }

    void artist::read(msgpack::istream& is)
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
              case 2: is >> name_; break;
              case 3: is >> album_ids_; break;
              default:
                is >> msgpack::skip;
                break;
            }
          }
          else
          {
            // ERROR!
            std::cout << "failed to read artist key" << std::endl;
          }
        }
      }
      else
      {
        // ERROR!
        std::cout << "failed to read artist map" << std::endl;
      }
    }

    void artist::write(msgpack::ostream& os) const
    {
      msgpack::map map{3};

      os << map
        << 1 << id_
        << 2 << name_
        << 3 << album_ids_;
    }
  }
}
