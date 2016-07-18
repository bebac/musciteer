// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__ctpb_provider_h__
#define __player__ctpb_provider_h__

// ----------------------------------------------------------------------------
#include <random>

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  class player_ctpb_provider
  {
  public:
    player_ctpb_provider()
      : rd_(), rg_(rd_()), di_()
    {
      auto tracks = musicbox::dm::tracks();

      tracks.each([&](musicbox::dm::track& track)
      {
        track_ids_.push_back(track.id());
        weights_.push_back(1);
        return true;
      });

      di_.reset(new std::discrete_distribution<>(weights_.begin(), weights_.end()));
    }
  public:
    const std::string& get_track_id()
    {
      return track_ids_[(*di_)(rg_)];
    }
  private:
    std::random_device rd_;
    std::mt19937 rg_;
    std::unique_ptr<std::discrete_distribution<>> di_;
  private:
    std::vector<std::string> track_ids_;
    std::vector<int> weights_;
  };
}

// ----------------------------------------------------------------------------
#endif