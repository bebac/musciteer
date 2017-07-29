// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "track.h"

// ----------------------------------------------------------------------------
#include "albums.h"
#include "artists.h"

// ----------------------------------------------------------------------------
namespace musciteer
{
  namespace dm
  {
    track::track()
      : id_(), title_(), tn_(0), dn_(0), duration_(0), album_id_(),
      artist_ids_(), sources_(), play_count_(0), skip_count_(0)
    {
    }

    bool track::id_is_null() const
    {
      return id_.empty();
    }

    const std::string& track::id() const
    {
      return id_;
    }

    const std::string& track::title() const
    {
      return title_;
    }

    unsigned track::track_number() const
    {
      return tn_;
    }

    unsigned track::disc_number() const
    {
      return dn_;
    }

    unsigned track::duration() const
    {
      return duration_;
    }

    dm::album track::album() const
    {
      auto albums = dm::albums();
      return albums.find_by_id(album_id_);
    }

    std::vector<dm::artist> track::artists() const
    {
      auto res = std::vector<dm::artist>();
      auto artists = dm::artists();

      for ( auto id : artist_ids_ )
      {
        res.push_back(artists.find_by_id(id));
      }

      return res;
    }

    const track_source& track::sources_get(const std::string& name) const
    {
      for ( auto& s : sources_ )
      {
        if ( s.name() == name ) {
          return s;
        }
      }
      throw std::runtime_error("track source not found");
    }

    track::source_list track::sources() const
    {
      return sources_;
    }

    void track::sources_each(std::function<void(const dm::track_source& source)> cb) const
    {
      for ( auto& source : sources_ )
      {
        cb(source);
      }
    }

    unsigned track::play_count() const
    {
      return play_count_;
    }

    unsigned track::skip_count() const
    {
      return skip_count_;
    }

    const track::tag_list& track::tags() const
    {
      return tags_;
    }

    void track::id(const std::string& id)
    {
      id_ = id;
    }

    void track::title(const std::string& title)
    {
      title_ = title;
    }

    void track::track_number(unsigned tn)
    {
      tn_ = tn;
    }

    void track::disc_number(unsigned dn)
    {
      dn_ = dn;
    }

    void track::duration(unsigned duration)
    {
      duration_ = duration;
    }

    void track::album(const dm::album& album)
    {
      album_id_ = album.id();
    }

    void track::artists_add(const dm::artist& artist)
    {
      auto id = artist.id();
      auto it = std::find(artist_ids_.begin(), artist_ids_.end(), id);

      if ( it == artist_ids_.end() )
      {
        artist_ids_.push_back(id);
      }
    }

    void track::sources_add(const dm::track_source& source)
    {
      auto it = std::find(sources_.begin(), sources_.end(), source);

      if ( it == sources_.end() )
      {
        sources_.push_back(source);
      }
      else
      {
        *it = source;
      }
    }

    void track::sources_remove(const std::string& source_name)
    {
      sources_.erase(
        std::remove_if(sources_.begin(), sources_.end(), [&](const dm::track_source& s) {
          return s.name() == source_name;
        }),
        sources_.end()
      );
    }

    void track::increment_play_count()
    {
      play_count_++;
    }

    void track::increment_skip_count()
    {
      skip_count_++;
    }

    void track::tags(const tag_list& value)
    {
      tags_ = value;
    }

    void track::read(msgpack::istream& is)
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
              case  1: is >> id_; break;
              case  2: is >> title_; break;
              case  3: is >> tn_; break;
              case  4: is >> dn_; break;
              case  5: is >> duration_; break;
              case  6: is >> album_id_; break;
              case  7: is >> artist_ids_; break;
              case  8: is >> sources_; break;
              case  9: is >> play_count_; break;
              case 10: is >> skip_count_; break;
              case 11: is >> tags_; break;
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

    void track::write(msgpack::ostream& os) const
    {
      msgpack::map map{11};

      os << map
        <<  1 << id_
        <<  2 << title_
        <<  3 << tn_
        <<  4 << dn_
        <<  5 << duration_
        <<  6 << album_id_
        <<  7 << artist_ids_
        <<  8 << sources_
        <<  9 << play_count_
        << 10 << skip_count_
        << 11 << tags_;
    }
  }
}
