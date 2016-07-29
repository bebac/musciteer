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
#include <dripcore/channel.h>

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
    enum class control
    {
      stop,
      done,
      undefined
    };
  public:
    player_session()
      : track_(), fraction_played_(0.0), audio_output_(), control_ch_()
    {
      auto kvstore = musicbox::kvstore();
      id_ = kvstore.increment(stream_id_key, 1, 1);
    }
  public:
    ~player_session()
    {
    }
  public:
    unsigned id() const
    {
      return id_;
    }
  public:
    std::shared_ptr<musicbox::dm::track> track() const
    {
      return track_;
    }
  public:
    float fraction_played() const
    {
      return fraction_played_;
    }
  public:
    std::shared_ptr<audio_output_alsa> audio_output() const
    {
      return audio_output_;
    }
  public:
    void track(const musicbox::dm::track& track)
    {
      track_.reset(new musicbox::dm::track(track));
    }
  public:
    void fraction_played(float value)
    {
      fraction_played_ = value;
    }
  public:
    void audio_output(std::shared_ptr<audio_output_alsa> audio_output)
    {
      audio_output_ = audio_output;
    }
  public:
    void stop()
    {
      control_ch_.send(control::stop);
    }
  public:
    void done()
    {
      std::cerr << "player_session done id=" << id_ << ", fraction_played=" << fraction_played_ << std::endl;
      control_ch_.send(control::done);
    }
  public:
    control recv(dripcore::task* task)
    {
      return control_ch_.recv(task);
    }
  public:
    void play(std::shared_ptr<audio_output_alsa> audio_output);
  private:
    unsigned id_;
    std::shared_ptr<musicbox::dm::track> track_;
    float fraction_played_;
    std::shared_ptr<audio_output_alsa> audio_output_;
    dripcore::channel<control> control_ch_;
  private:
    static constexpr const char* stream_id_key = "__session_id__";
  };

  using session_channel = dripcore::channel<std::shared_ptr<player_session>>;
}



// ----------------------------------------------------------------------------
#endif
