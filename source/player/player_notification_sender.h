// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__player_notification_sender_h__
#define __player__player_notification_sender_h__

// ----------------------------------------------------------------------------
#include "message.h"
#include "audio_output_alsa.h"

// ----------------------------------------------------------------------------
#include <chrono>

// ----------------------------------------------------------------------------
namespace musciteer
{
  class player_notification_sender
  {
    using time_point = std::chrono::steady_clock::time_point;
    using milliseconds = std::chrono::milliseconds;
  public:
    player_notification_sender(unsigned id, message_channel ch)
      :
      id_(id),
      channel_(ch)
    {
    }
  public:
    void set_stream_length(milliseconds length)
    {
      stream_length_ = length;
    }
  public:
    void stream_begin(const audio_output_alsa& output)
    {
      stream_begin_now();
      stream_begin_notify(output);
    }
  public:
    void stream_progress()
    {
      using std::chrono::steady_clock;
      using std::chrono::seconds;

      auto now = steady_clock::now();

      if ( now > stream_notify_next_)
      {
        stream_progress_notify(now - stream_begin_time_, stream_length_);
        stream_notify_next(now + seconds(1) - (now - stream_notify_next_));
      }
    }
  public:
    void stream_end(bool audio_output_error = false)
    {
      using std::chrono::steady_clock;

      stream_progress_notify(steady_clock::now() - stream_begin_time_, stream_length_);
      stream_end_notify(audio_output_error);
    }
  private:
    void stream_begin_now()
    {
      stream_begin_time_ = std::chrono::steady_clock::now();
      stream_notify_next_ = stream_begin_time_ + std::chrono::seconds(1);
    }
  private:
    void stream_notify_next(time_point next)
    {
      stream_notify_next_ = next;
    }
  private:
    void stream_begin_notify(const audio_output_alsa& output)
    {
      message m(message::stream_begin_notify_id);
      auto& n = m.stream_begin_notify;

      n.stream_id = id_;
      n.replaygain_enabled = output.get_replaygain_enabled();
      n.replaygain = output.get_replaygain();
      n.scale = output.get_replaygain_scale();

      channel_.send(std::move(m));
    }
  private:
    void stream_progress_notify(std::chrono::steady_clock::duration duration, milliseconds length)
    {
      using std::chrono::duration_cast;

      message m(message::stream_progress_notify_id);
      auto& n = m.stream_progress_notify;

      n.stream_id = id_;
      n.duration = duration_cast<milliseconds>(duration).count();
      n.length = duration_cast<milliseconds>(length).count();

      channel_.send(std::move(m));
    }
  private:
    void stream_end_notify(bool audio_output_error)
    {
      message m(message::stream_end_notify_id);
      auto& n = m.stream_end_notify;

      n.stream_id = id_;
      n.audio_output_error = audio_output_error;

      channel_.send(std::move(m));
    }
  private:
    unsigned id_;
    message_channel channel_;
    time_point stream_begin_time_;
    time_point stream_notify_next_;
    milliseconds stream_length_;
  };
}

// ----------------------------------------------------------------------------
#endif
