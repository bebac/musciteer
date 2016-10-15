// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__dm__albums_h__
#define __musciteer__dm__albums_h__

// ----------------------------------------------------------------------------
#include "album.h"

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"
#include "../storage/base62.h"

// ----------------------------------------------------------------------------
namespace musciteer
{
  namespace dm
  {
    class albums
    {
    public:
      albums() : kvstore_(musciteer::kvstore())
      {
      }
    public:
      album find_by_id(const std::string& id)
      {
        auto album = dm::album();

        kvstore_.get(id, album);

        return album;
      }
    public:
      void save(const dm::album& album)
      {
        kvstore_.set(album.id(), album);
      }
    public:
      void remove(const dm::album& album)
      {
        kvstore_.remove(album.id());
      }
    public:
      void each(std::function<bool(dm::album& album)> value_cb)
      {
        kvstore_.each(
          [](const std::string& key) -> bool
          {
            if ( key.length() == 6 && key[0] == 'a' && key[1] == 'l' ) {
              return true;
            }
            else {
              return false;
            }
          },
          [&](msgpack::istream& is) -> bool
          {
            dm::album album;

            if ( is >> album )
            {
              value_cb(album);
            }
            else
            {
              // ERROR!
              std::cerr << "failed to read album!" << std::endl;
            }
            return true;
          }
        );
      }
    public:
      album create_album()
      {
        dm::album album;

        auto id = kvstore_.increment("__album_next__", 1, key_min);

        if ( id > key_max )
        {
          throw std::runtime_error("album key space exhausted!");
        }

        album.id(base62_encode(id));
        return album;
      }
    private:
      kvstore kvstore_;
    private:
      static constexpr const int64_t key_min = 33675269744;
      static constexpr const int64_t key_max = 33690046079;
    };
  }
}

// ----------------------------------------------------------------------------
#endif
