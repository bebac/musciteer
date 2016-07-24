// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "album.h"
#include "track.h"
#include "tracks.h"
#include "artists.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  namespace dm
  {
    album::album() : id_(), title_(), track_ids_(), alt_ids_()
    {
    }

    bool album::id_is_null()
    {
      return id_.empty();
    }

    const std::string& album::id() const
    {
      return id_;
    }

    const std::string& album::title() const
    {
      return title_;
    }

    const album::track_id_list& album::track_ids() const
    {
      return track_ids_;
    }

    dm::artist album::artist() const
    {
      auto artists = dm::artists();
      return artists.find_by_id(artist_id_);
    }

    bool album::alt_ids_find(const std::string& alt_id) const
    {
      return alt_ids_.count(alt_id) > 0;
    }

    void album::id(const std::string& id)
    {
      id_ = id;
    }

    void album::title(const std::string& title)
    {
      title_ = title;
    }

    void album::tracks_add(const dm::track& track)
    {
      track_ids_.insert(track.id());
    }

    void album::tracks_del(const dm::track& track)
    {
      track_ids_.erase(track.id());
    }

    void album::tracks_each(std::function<void(const dm::track& track)> cb) const
    {
      auto tracks = dm::tracks();

      for ( auto& id : track_ids_ )
      {
        cb(tracks.find_by_id(id));
      }
    }

    void album::artist(const dm::artist& artist)
    {
      artist_id_ = artist.id();
    }

    void album::alt_ids_add(const std::string& alt_id)
    {
      alt_ids_.insert(alt_id);
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
              case 4: is >> artist_id_; break;
              case 5: is >> alt_ids_; break;
              default:
                is >> msgpack::skip;
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
      msgpack::map map{5};

      os << map
        << 1 << id_
        << 2 << title_
        << 3 << track_ids_
        << 4 << artist_id_
        << 5 << alt_ids_;
    }
  }
}