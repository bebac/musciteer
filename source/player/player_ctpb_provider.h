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
    player_ctpb_provider() : idx_(0)
    {
    }
  public:
    musicbox::dm::track get_track()
    {
      auto tracks = musicbox::dm::tracks();

      auto start = std::chrono::steady_clock::now();

      std::vector<std::string> track_ids;
      std::vector<int> weights;

      tracks.each([&](musicbox::dm::track& track)
      {
        track_ids.push_back(track.id());
        weights.push_back(1);
        return true;
      });

      std::discrete_distribution<> dist(weights.begin(), weights.end());
      std::mt19937 gen;

      std::seed_seq seq{time(0), idx_};
      gen.seed(seq);

      idx_ = dist(gen);

      auto track_id = track_ids[idx_];

      auto end = std::chrono::steady_clock::now();

      std::cout
        << "ctpb provider - produced track id '" << track_id << "' in "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << "ms"
        << std::endl;

      return tracks.find_by_id(track_ids[idx_]);
    }
  private:
    long int idx_;
  };
}

// ----------------------------------------------------------------------------
#endif