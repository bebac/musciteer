// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__tag_provider_h__
#define __player__tag_provider_h__

// ----------------------------------------------------------------------------
#include "player_list_provider_base.h"

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"

// ----------------------------------------------------------------------------
namespace musciteer
{
  class tag_provider : public list_provider_base
  {
  public:
    tag_provider(const std::string& tag) :  tag_(tag), index_(0)
    {
      auto tracks = musciteer::dm::tracks();

      std::cerr << "find tag " << tag << std::endl;

      tracks.each([&](musciteer::dm::track& track)
      {
        const auto& tags = track.tags();

        for ( auto& tag : tags ) {
          std::cerr << tag << ", ";
        }
        std::cerr << std::endl;

        if ( tags.find(tag) != tags.end() ) {
          std::cerr << "tag found" << std::endl;
          tracks_.push_back(track);
        }

        return true;
      });

      shuffle();
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
      return tag_;
    }
  private:
    void shuffle()
    {
      std::random_device rd;
      std::mt19937 g(rd());

      std::shuffle(tracks_.begin(), tracks_.end(), g);
    }
  private:
    std::string tag_;
    std::vector<dm::track> tracks_;
    size_t index_;
  };
}

// ----------------------------------------------------------------------------
#endif
