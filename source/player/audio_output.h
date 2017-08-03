// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__audio_output_h__
#define __player__audio_output_h__

// ----------------------------------------------------------------------------
#include "message.h"
#include "audio_output_alsa.h"

// ----------------------------------------------------------------------------
#include <chrono>

// ----------------------------------------------------------------------------
class audio_output
{
public:
  using device_list_request = audio_output_device_list_request;
  using open_request = audio_output_open_request;
  using close_request = audio_output_close_request;
  using stream_begin = audio_output_stream_begin;
  using stream_end = audio_output_stream_end;
  using stream_buffer = audio_output_stream_buffer;
  using stream_begin_notify =  audio_output_stream_begin_notification;
  using stream_end_notify =  audio_output_stream_end_notification;
  using stream_progress_notify =  audio_output_stream_progress_notification;
  using subscribe = audio_output_subscribe_message;
  using unsubscribe = audio_output_unsubscribe_message;
  using replaygain_request = audio_output_replaygain_request;
public:
  using time_point = std::chrono::steady_clock::time_point;
  using milliseconds = std::chrono::milliseconds;
private:
  enum state
  {
    state_closed,
    state_open,
    state_playing
  };
public:
  audio_output();
public:
  ~audio_output();
public:
  void send(message&& m);
  void send(audio_buffer&& buffer);
private:
  void loop();
  void dispatch(message& m);
private:
  void handle(subscribe& m);
  void handle(unsubscribe& m);
private:
  void handle(device_list_request& m);
  void handle(open_request& m);
  void handle(close_request& m);
  void handle(stream_begin& m);
  void handle(stream_end& m);
  void handle(stream_buffer& m);
  void handle(replaygain_request& m);
private:
  void calculate_scale();
  void update_stream_time(bool last=false);
private:
  state state_;
private:
  unsigned stream_id_;
  time_point stream_begin_time_;
  time_point stream_notify_next_;
  milliseconds stream_length_;
  bool rg_enabled_;
  double rg_;
  double rg_peak_;
  double scale_;
private:
  audio_output_alsa output_;
private:
  message_channel msg_ch_;
private:
  dripcore::channel<audio_buffer> completed_buffer_ch_;
private:
  std::set<message_channel> observers_;
private:
  std::thread thr_;
};

// ----------------------------------------------------------------------------
#endif

