// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__ctpb_provider_h__
#define __player__ctpb_provider_h__

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/task_channel.h>

// ----------------------------------------------------------------------------
#include <random>

// ----------------------------------------------------------------------------
namespace musciteer
{
  using player_ctpb_channel  = dripcore::task_channel<dm::track, 1>;
  using player_ctpb_ichannel = dripcore::task_ichannel<dm::track>;
  using player_ctpb_ochannel = dripcore::task_ochannel<dm::track>;

  namespace player_ctpb
  {
    //
    // Random.
    //

    class random : public dripcore::task
    {
    public:
      random(const player_ctpb_ochannel& channel) : channel_(channel, this)
      {
      }
    private:
      void main() final
      {
        while ( !stopping() )
        {
          auto tracks   = musciteer::dm::tracks();
          auto start    = std::chrono::steady_clock::now();
          auto track_id = generate_track_id();
          auto track    = tracks.find_by_id(track_id);
          auto end      = std::chrono::steady_clock::now();

          std::cout
           << "player_ctpb::random - took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms "
           << "track id '" << track.id() << "', play_count " << track.play_count() << ", skip_count " << track.skip_count()
           << std::endl;

          channel_.send(std::move(track));
        }
      }
    private:
      std::string generate_track_id()
      {
        auto tracks = musciteer::dm::tracks();

        std::vector<std::string> track_ids;
        std::vector<int> weights;

        tracks.each([&](musciteer::dm::track& track)
        {
          track_ids.push_back(track.id());
          weights.push_back(1);

          yield(true);

          return true;
        });

        std::discrete_distribution<> dist(weights.begin(), weights.end());
        std::mt19937 gen;

        std::seed_seq seq{time(0), idx_};
        gen.seed(seq);

        idx_ = dist(gen);

        return track_ids[idx_];
      }
    private:
      long int idx_;
    private:
      player_ctpb_ochannel channel_;
    };

    //
    // Less Played.
    //

    class less_played : public dripcore::task
    {
    public:
      less_played(const player_ctpb_ochannel& channel)
        :
        count_to_weight_tab_{
          240, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11,
          10, 9, 8, 7, 6, 5, 4, 3, 2, 1
        },
        channel_(channel, this)
      {
      }
    private:
      void main() final
      {
        while ( !stopping() )
        {
          auto tracks   = musciteer::dm::tracks();
          auto start    = std::chrono::steady_clock::now();
          auto track_id = generate_track_id();
          auto track    = tracks.find_by_id(track_id);
          auto end      = std::chrono::steady_clock::now();

          std::cout
           << "player_ctpb::less_played - took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms "
           << "track id '" << track.id() << "', play_count " << track.play_count() << ", skip_count " << track.skip_count()
           << std::endl;

          channel_.send(std::move(track));
        }
      }
    private:
      std::string generate_track_id()
      {
        auto tracks = musciteer::dm::tracks();

        std::vector<std::string> track_ids;
        std::vector<int> weights;

        tracks.each([&](musciteer::dm::track& track)
        {
          int count = track.play_count() - track.skip_count();

          // If track has a negative play count >= 2 skip it.
          if ( count < 0 && count > -2 ) {
            count = 0;
          }

          if ( count >= 0 )
          {
            track_ids.push_back(track.id());

            if ( count > 24 ) {
              count = 24;
            }

            weights.push_back(count_to_weight_tab_[count]);
          }

          yield(true);

          return true;
        });

        std::discrete_distribution<> dist(weights.begin(), weights.end());
        std::mt19937 gen;

        std::seed_seq seq{time(0), idx_};
        gen.seed(seq);

        idx_ = dist(gen);

        return track_ids[idx_];
      }
    private:
      std::array<int, 25> count_to_weight_tab_;
    private:
      long int idx_;
    private:
      player_ctpb_ochannel channel_;
    };

    //
    // More Played.
    //

    class more_played : public dripcore::task
    {
    public:
      more_played(const player_ctpb_ochannel& channel)
        :
        count_to_weight_tab_{
          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
          16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
          29, 30, 31, 32
        },
        channel_(channel, this)
      {
      }
    private:
      void main() final
      {
        while ( !stopping() )
        {
          auto tracks   = musciteer::dm::tracks();
          auto start    = std::chrono::steady_clock::now();
          auto track_id = generate_track_id();
          auto track    = tracks.find_by_id(track_id);
          auto end      = std::chrono::steady_clock::now();

          std::cout
           << "player_ctpb::more_played - took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms "
           << "track id '" << track.id() << "', play_count " << track.play_count() << ", skip_count " << track.skip_count()
           << std::endl;

          channel_.send(std::move(track));
        }
      }
    private:
      std::string generate_track_id()
      {
        auto tracks = musciteer::dm::tracks();

        std::vector<std::string> track_ids;
        std::vector<int> weights;

        tracks.each([&](musciteer::dm::track& track)
        {
          int count = track.play_count() - track.skip_count();

          // If track has a negative play count >= 2 skip it.
          if ( count < 0 && count > -2 ) {
            count = 0;
          }

          if ( count >= 0 )
          {
            track_ids.push_back(track.id());

            if ( count > 31 ) {
              count = 31;
            }

            weights.push_back(count_to_weight_tab_[count]);
          }

          yield(true);

          return true;
        });

        std::discrete_distribution<> dist(weights.begin(), weights.end());
        std::mt19937 gen;

        std::seed_seq seq{time(0), idx_};
        gen.seed(seq);

        idx_ = dist(gen);

        return track_ids[idx_];
      }
    private:
      std::array<int, 32> count_to_weight_tab_;
    private:
      long int idx_;
    private:
      player_ctpb_ochannel channel_;
    };
  }

  //
  // Continuous Playback Provider.
  //

  class player_ctpb_provider
  {
  public:
    player_ctpb_provider(dripcore::task* task) : task_(task)
    {
    }
  public:
    bool is_loaded()
    {
      return !!ctpb_task_;
    }
  public:
    void load(const std::string& type)
    {
      if ( type == "random" ) {
        ctpb_task_ = task_->spawn<player_ctpb::random>(channel_).lock();
      }
      else if ( type == "less-played" ) {
        ctpb_task_ = task_->spawn<player_ctpb::less_played>(channel_).lock();
      }
      else if ( type == "more-played" ) {
        ctpb_task_ = task_->spawn<player_ctpb::more_played>(channel_).lock();
      }
      else {
        throw std::runtime_error("player_ctpb_provider - unknown type");
      }
    }
  public:
    void unload()
    {
      if ( ctpb_task_ )
      {
        ctpb_task_->stop();
      }
    }
  public:
    musciteer::dm::track get_track()
    {
      return channel_.recv(task_);
    }
  private:
    player_ctpb_channel channel_;
    std::shared_ptr<dripcore::task> ctpb_task_;
  private:
    dripcore::task* task_;
  };
}

// ----------------------------------------------------------------------------
#endif