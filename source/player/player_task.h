// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __player__player_task_h__
#define __player__player_task_h__

// ----------------------------------------------------------------------------
#include "audio_output.h"

// ----------------------------------------------------------------------------
#include "../dm/track.h"

// ----------------------------------------------------------------------------
#include <string>
#include <cassert>
#include <queue>

// ----------------------------------------------------------------------------
namespace musicbox
{
  class player_task : public dripcore::task
  {
    using subscribe = audio_output_subscribe_message;
    using unsubscribe = audio_output_unsubscribe_message;
    using device_list_request = audio_output_device_list_request;
    using stream_begin_notify =  audio_output_stream_begin_notification;
    using stream_end_notify =  audio_output_stream_end_notification;
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
    ~player_task();
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
    void handle(stream_data_request& m);
    void handle(play_request& m);
    void handle(queue_request& m);
    void handle(stream_begin_notify& m);
    void handle(stream_end_notify& m);
  private:
    void play(const std::string& uri);
    void audio_output_subscribe(message_channel&);
    void audio_output_unsubscribe(message_channel&);
    void audio_output_open();
  private:
    state state_;
  private:
    message_channel message_ch_;
  private:
    std::string audio_output_device_;
    audio_output_alsa audio_output_;
  private:
    std::set<message_channel> observers_;
  private:
    std::queue<musicbox::track> play_q_;
  private:
    unsigned playing_id_;
    musicbox::track playing_track_;
  private:
    static constexpr const char* audio_output_device_key = "__output_device__";
    static constexpr const char* stream_id_key = "__stream_id__";
  };
}

// ----------------------------------------------------------------------------
#endif
