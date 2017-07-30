// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__album_provider_h__
#define __player__album_provider_h__

// ----------------------------------------------------------------------------
#include "player_list_provider_base.h"

// ----------------------------------------------------------------------------
#include "../dm/album.h"

// ----------------------------------------------------------------------------
#include <vector>

// ----------------------------------------------------------------------------
namespace musciteer
{
  class album_provider : public list_provider_base
  {
  public:
    album_provider(const dm::album& album) :  index_(0)
    {
      album.tracks_each([&](const dm::track& track)
      {
        if ( !track.id_is_null() ) {
          tracks_.push_back(track);
        }
      });

      std::sort(tracks_.begin(), tracks_.end(), [](const dm::track& a, const dm::track& b)
      {
        unsigned ia = (a.disc_number() << 16) | a.track_number();
        unsigned ib = (b.disc_number() << 16) | b.track_number();
        return ia < ib;
      });
    }
  public:
    bool done() override
    {
      return tracks_.size() == 0 || index_ >= tracks_.size();
    }
  public:
    dm::track next() override
    {
      auto t = dm::track();

      if ( index_ < tracks_.size() )
      {
        t = tracks_[index_];
        index_++;
      }

      return t;
    }
  public:
    std::string info() override
    {
      return "album";
    }
  private:
    std::vector<dm::track> tracks_;
    size_t index_;
  };
}

// ----------------------------------------------------------------------------
#endif
