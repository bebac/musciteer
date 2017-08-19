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
#include "player_list_provider_base.h"
#include "player_ctpb_provider.h"

// ----------------------------------------------------------------------------
#include "../dm/track.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>

// ----------------------------------------------------------------------------
#include <string>
#include <cassert>
#include <deque>
#include <unordered_map>
#include <regex>

// ----------------------------------------------------------------------------
class audio_output;

// ----------------------------------------------------------------------------
namespace musciteer
{
  class player_session;

  class player_task : public dripcore::task
  {
    using subscribe = audio_output_subscribe_message;
    using unsubscribe = audio_output_unsubscribe_message;
    using device_list_request = audio_output_device_list_request;
    using stream_begin_notify =  audio_output_stream_begin_notification;
    using stream_progress_notify =  audio_output_stream_progress_notification;
    using stream_end_notify =  audio_output_stream_end_notification;
  private:
    enum state
    {
      stopped,
      playing,
      stopping,
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
    void handle(device_list_request& m);
    void handle(settings_changed_message& m);
    void handle(stream_data_request& m);
    void handle(play_request& m);
    void handle(pause_request& m);
    void handle(stop_request& m);
    void handle(skip_request& m);
    void handle(queue_request& m);
    void handle(stream_begin_notify& m);
    void handle(stream_progress_notify& m);
    void handle(stream_end_notify& m);
    void handle(source_notification& m);
  private:
    void become_playing(const musciteer::dm::track& track);
    void become_stopped();
    void end_session();
    void player_state_notify();
    void queue_update_notify(const musciteer::dm::track& track);
    void audio_output_subscribe(message_channel&);
    void audio_output_unsubscribe(message_channel&);
    bool audio_output_open();
    bool audio_output_close();
  private:
    state state_;
  private:
    message_channel message_ch_;
  private:
    std::shared_ptr<audio_output> audio_output_;
    std::string audio_output_device_;
  private:
    std::set<message_channel> observers_;
  private:
    std::deque<musciteer::dm::track> play_q_;
    std::unique_ptr<list_provider_base> list_provider_;
    std::regex tag_provider_re_;
    bool continuous_playback_;
    player_ctpb_provider ctpb_provider_;
    bool replaygain_enabled_;
  private:
    std::shared_ptr<player_session> session_;
  private:
    std::unordered_map<std::string, std::string> source_status_;
  private:
    static constexpr const char* audio_output_device_key = "__output_device__";
  };
}

// ----------------------------------------------------------------------------
#endif
