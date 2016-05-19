// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __player__player_impl_h__
#define __player__player_impl_h__

// ----------------------------------------------------------------------------
#include "audio_output.h"

// ----------------------------------------------------------------------------
#include <string>
#include <cassert>

// ----------------------------------------------------------------------------
namespace musicbox
{
  class player_task : public dripcore::task
  {
    using subscribe = audio_output_subscribe;
    using unsubscribe = audio_output_unsubscribe;
    using device_list_request = audio_output_device_list_request;
  private:
    enum state
    {
      stopped,
      playing,
      paused
    };
  public:
    player_task(message_channel message_ch);
  public:
    void main() override;
  private:
    void load_settings();
  private:
    void dispatch(message& m);
  private:
    void handle(subscribe& m);
    void handle(unsubscribe& m);
    void handle(device_list_request& m, unsigned ref);
    void handle(audio_output_device& m);
    void handle(play_request& m);
  private:
    void open_audio_output();
  private:
    state state_;
  private:
    message_channel message_ch_;
  private:
    std::string audio_output_device_;
    audio_output_alsa audio_output_;
  private:
    static constexpr const char* audio_output_device_key = "__output_device__";
  };
}

// ----------------------------------------------------------------------------
#endif
