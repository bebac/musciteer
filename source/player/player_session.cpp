// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include "player_session.h"
#include "sources.h"
#include "audio_output.h"

// ----------------------------------------------------------------------------
namespace musciteer
{
  player_session::player_session()
    :
    track_(),
    fraction_played_(0.0),
    audio_output_(),
    control_ch_()
  {
    auto kvstore = musciteer::kvstore();
    id_ = kvstore.increment(stream_id_key, 1, 1);
  }

  unsigned player_session::id() const
  {
    return id_;
  }

  std::shared_ptr<musciteer::dm::track> player_session::track() const
  {
    return track_;
  }

  void player_session::track(const musciteer::dm::track& track)
  {
    track_.reset(new musciteer::dm::track(track));
  }

  float player_session::fraction_played() const
  {
    return fraction_played_;
  }

  void player_session::fraction_played(float value)
  {
    fraction_played_ = value;
  }

  std::shared_ptr<audio_output_alsa> player_session::audio_output() const
  {
    return audio_output_;
  }

  void player_session::audio_output(std::shared_ptr<audio_output_alsa> audio_output)
  {
    audio_output_ = audio_output;
  }

  void player_session::play(std::shared_ptr<audio_output_alsa> audio_output)
  {
    auto sources = musciteer::sources();
    audio_output_ = audio_output;
    sources.play(shared_from_this());
  }

  void player_session::stop()
  {
    control_ch_.send(control::stop);
  }

  void player_session::done()
  {
    std::cerr << "player_session done id=" << id_ << ", fraction_played=" << fraction_played_ << std::endl;
    control_ch_.send(control::done);
  }

  player_session::control player_session::recv(dripcore::task* task)
  {
    return control_ch_.recv(task);
  }
}
