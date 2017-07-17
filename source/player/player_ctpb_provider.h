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

        if ( !track_ids.empty() )
        {
          std::discrete_distribution<> dist(weights.begin(), weights.end());
          std::mt19937 gen;

          std::seed_seq seq{time(0), idx_};
          gen.seed(seq);

          idx_ = dist(gen);

          return track_ids[idx_];
        }
        else
        {
          return std::string();
        }
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
        count_to_weight_tab_{ {
          240, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11,
          10, 9, 8, 7, 6, 5, 4, 3, 2, 1 }
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

        if ( !track_ids.empty() )
        {
          std::discrete_distribution<> dist(weights.begin(), weights.end());
          std::mt19937 gen;

          std::seed_seq seq{time(0), idx_};
          gen.seed(seq);

          idx_ = dist(gen);

          return track_ids[idx_];
        }
        else
        {
          return std::string();
        }
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

            if ( count > 99 ) {
              count = 99;
            }

            weights.push_back(count+1);
          }

          yield(true);

          return true;
        });

        if ( !track_ids.empty() )
        {
          std::discrete_distribution<> dist(weights.begin(), weights.end());
          std::mt19937 gen;

          std::seed_seq seq{time(0), idx_};
          gen.seed(seq);

          idx_ = dist(gen);

          return track_ids[idx_];
        }
        else
        {
          return std::string();
        }
      }
    private:
      long int idx_;
    private:
      player_ctpb_ochannel channel_;
    };


    //
    // Top Played.
    //

    class top_played : public dripcore::task
    {
    public:
      top_played(const player_ctpb_ochannel& channel) : channel_(channel, this)
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
           << "player_ctpb::top_played - took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms "
           << "track id '" << track.id() << "', play_count " << track.play_count() << ", skip_count " << track.skip_count()
           << std::endl;

          channel_.send(std::move(track));
        }
      }
    private:
      std::string generate_track_id()
      {
        auto tracks = musciteer::dm::tracks();

        struct track_entry
        {
          std::string id;
          int         count;
        };

        std::vector<track_entry> track_entries;
        std::vector<int> weights;

        //
        // Build the track list and find the highest and lowest
        // play count.
        //

        int count_hi_max = 0;
        int count_hi_min = 0;
        int count_lo_max = 0;
        int count_lo_min = std::numeric_limits<int>::max();

        tracks.each([&](musciteer::dm::track& track)
        {
          int count = track.play_count() - track.skip_count();

          // If track has a negative play count >= 2 skip it.
          if ( count < 0 && count > -2 ) {
            count = 0;
          }

          if ( count < 0 ) {
            return true;
          }

          if ( count > count_hi_max ) {
            count_hi_max = count;
          }

          if ( count < count_lo_min ) {
            count_lo_min = count;
          }

          track_entries.push_back(track_entry{track.id(), count});

          yield(true);

          return true;
        });

        //
        // Divide the tracks into 3 bins.
        //

        int bin_size = count_hi_max / 3;

        count_hi_min = count_hi_max - bin_size;
        count_lo_max = count_lo_min + bin_size;

        int count_80 = 0;
        int count_15 = 0;
        int count_05 = 0;

        auto bin_count = [&]() {
          for ( const auto& entry : track_entries )
          {
            if ( entry.count >= count_hi_min) {
              count_80++;
            }
            else if ( entry.count >= count_lo_max ) {
              count_15++;
            }
            else {
              count_05++;
            }
          }
        };


        do
        {
#if 0
          std::cout
           << "player_ctpb::more_played\n"
           << "  entries      = " << track_entries.size() << "\n"
           << "  count_hi_max = " << count_hi_max << "\n"
           << "  count_hi_min = " << count_hi_min << "\n"
           << "  count_lo_max = " << count_lo_max << "\n"
           << "  count_lo_min = " << count_lo_min << "\n"
           << std::endl;
#endif

          bin_count();

#if 0
          std::cout
           << "player_ctpb::more_played\n"
           << "  count_80     = " << count_80 << "\n"
           << "  count_15     = " << count_15 << "\n"
           << "  count_05     = " << count_05 << "\n"
           << std::endl;
#endif

          //
          // If there is only a few tracks in the 80% bin include
          // a lower play count and recount.
          //

          if ( count_80 < (track_entries.size()*0.1) && count_hi_min > 1 )
          {
            count_80 = 0;
            count_15 = 0;
            count_05 = 0;

            count_hi_min--;

            if ( count_lo_max > 1 ){
              count_lo_max--;
            }
          }
          else {
            break;
          }
        }
        while ( true );

        //
        // Build the weight list and pick a track.
        //

        int weight_max = track_entries.size()*100;
        int weight_80  = weight_max*0.80/count_80;
        int weight_15  = weight_max*0.15/count_15;
        int weight_05  = weight_max*0.05/count_05;

        for ( const auto& entry : track_entries )
        {
          if ( entry.count >= count_hi_min) {
            weights.push_back(weight_80);
          }
          else if ( entry.count >= count_lo_max ) {
            weights.push_back(weight_15);
          }
          else {
            weights.push_back(weight_05);
          }
        }

        if ( !track_entries.empty() )
        {
          std::discrete_distribution<> dist(weights.begin(), weights.end());
          std::mt19937 gen;

          std::seed_seq seq{time(0), idx_};
          gen.seed(seq);

          idx_ = dist(gen);

          return track_entries[idx_].id;
        }
        else
        {
          return std::string();
        }
      }
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
      else if ( type == "top-played" ) {
        ctpb_task_ = task_->spawn<player_ctpb::top_played>(channel_).lock();
      }
      else {
        throw std::runtime_error("player_ctpb_provider - unknown type");
      }
      ctpb_type_ = type;
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
  public:
    std::string info()
    {
      return ctpb_type_;
    }
  private:
    player_ctpb_channel channel_;
    std::shared_ptr<dripcore::task> ctpb_task_;
    std::string ctpb_type_;
  private:
    dripcore::task* task_;
  };
}

// ----------------------------------------------------------------------------
#endif