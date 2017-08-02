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
class audio_output;

// ----------------------------------------------------------------------------
namespace musciteer
{
  class player_session : public std::enable_shared_from_this<player_session>
  {
    using track_ptr = std::shared_ptr<musciteer::dm::track>;
    using audio_output_ptr = std::shared_ptr<audio_output>;
  public:
    enum class control
    {
      stop,
      done,
      undefined
    };
    using control_channel = dripcore::channel<control>;
  public:
    player_session();
    ~player_session() = default;
  public:
    unsigned id() const;
  public:
    track_ptr track() const;
    void track(const musciteer::dm::track& track);
  public:
    float fraction_played() const;
    void fraction_played(float value);
  public:
    std::shared_ptr<audio_output> get_audio_output() const;
    void set_audio_output(std::shared_ptr<audio_output>);
  public:
    void play(std::shared_ptr<audio_output> audio_output);
    void stop();
    void done();
  public:
    control recv(dripcore::task* task);
  private:
    unsigned id_;
    track_ptr track_;
    float fraction_played_;
    audio_output_ptr audio_output_;
    control_channel control_ch_;
  private:
    static constexpr const char* stream_id_key = "__session_id__";
  };

  using session_channel = dripcore::channel<std::shared_ptr<player_session>>;
}

// ----------------------------------------------------------------------------
#endif
