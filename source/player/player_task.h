// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__player_task_h__
#define __player__player_task_h__

// ----------------------------------------------------------------------------
#include "message.h"

// ----------------------------------------------------------------------------
#include "../dm/track.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>

// ----------------------------------------------------------------------------
#include <string>
#include <cassert>
#include <queue>

// ----------------------------------------------------------------------------
class audio_output_alsa;

// ----------------------------------------------------------------------------
namespace musicbox
{
  class player_session;

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
    void handle(stop_request& m);
    void handle(queue_request& m);
    void handle(stream_begin_notify& m);
    void handle(stream_end_notify& m);
  private:
    void become_playing(const musicbox::track& track);
    void become_stopped();
    void player_state_notify(unsigned state);
    void queue_update_notify(const musicbox::track& track);
    void audio_output_subscribe(message_channel&);
    void audio_output_unsubscribe(message_channel&);
    void audio_output_open();
  private:
    state state_;
  private:
    message_channel message_ch_;
  private:
    std::shared_ptr<audio_output_alsa> audio_output_;
    std::string audio_output_device_;
  private:
    std::set<message_channel> observers_;
  private:
    std::queue<musicbox::track> play_q_;
  private:
    std::shared_ptr<player_session> session_;
  private:
    static constexpr const char* audio_output_device_key = "__output_device__";
  };
}

// ----------------------------------------------------------------------------
#endif
