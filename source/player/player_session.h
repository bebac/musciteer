// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__player_session_h__
#define __player__player_session_h__

// ----------------------------------------------------------------------------
#include "../dm/track.h"

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"

// ----------------------------------------------------------------------------
#include <memory>

// ----------------------------------------------------------------------------
class audio_output_alsa;

// ----------------------------------------------------------------------------
namespace musicbox
{
  class player_session : public std::enable_shared_from_this<player_session>
  {
  public:
    player_session()
    {
      auto kvstore = musicbox::kvstore();
      id_ = kvstore.increment(stream_id_key, 1, 1);
    }
  public:
    unsigned id() const
    {
      return id_;
    }
  public:
    std::shared_ptr<musicbox::track> track() const
    {
      return track_;
    }
  public:
    std::shared_ptr<audio_output_alsa> audio_output() const
    {
      return audio_output_;
    }
  public:
    void track(const musicbox::track& track)
    {
      track_.reset(new musicbox::track(track));
    }
  public:
    void audio_output(std::shared_ptr<audio_output_alsa> audio_output)
    {
      audio_output_ = audio_output;
    }
  public:
    void play(std::shared_ptr<audio_output_alsa> audio_output);
  private:
    unsigned id_;
    std::shared_ptr<musicbox::track> track_;
    std::shared_ptr<audio_output_alsa> audio_output_;
  private:
    static constexpr const char* stream_id_key = "__session_id__";
  };
}

// ----------------------------------------------------------------------------
#endif
