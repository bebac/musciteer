// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musicbox__dm__albums_h__
#define __musicbox__dm__albums_h__

// ----------------------------------------------------------------------------
#include "album.h"

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"
#include "../storage/base62.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  class albums
  {
  public:
    albums() : kvstore_(musicbox::kvstore())
    {
    }
  public:
    album find_by_id(const std::string& id)
    {
      return kvstore_.get<musicbox::album>(id);
    }
  public:
    void save(const musicbox::album& album)
    {
      kvstore_.set(album.id(), album);
    }
  public:
    void each(std::function<bool(musicbox::album& album)> value_cb)
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
          musicbox::album album;

          if ( is >> album )
          {
            value_cb(album);
          }
          else
          {
            // ERROR!
          }
          return true;
        }
      );
    }
  public:
    album create_album()
    {
      musicbox::album album;

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
    static constexpr const uint64_t key_min = 33675269744;
    static constexpr const uint64_t key_max = 33690046079;
  };
}

// ----------------------------------------------------------------------------
#endif
