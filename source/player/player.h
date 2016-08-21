// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__player_h__
#define __player__player_h__

// ----------------------------------------------------------------------------
#include "player_task.h"

// ----------------------------------------------------------------------------
#include <string>
#include <cassert>

// ----------------------------------------------------------------------------
namespace musciteer
{
  class player
  {
  public:
    player()
    {
    }
  public:
    void subscribe(message_channel ch)
    {
      message m(message::subscribe_id, 0);

      m.subscribe.channel = ch;

      message_ch_.send(std::move(m));
    }
  public:
    void unsubscribe(message_channel ch)
    {
      message m(message::unsubscribe_id, 0);

      m.unsubscribe.channel = ch;

      message_ch_.send(std::move(m));
    }
  public:
    void audio_device_list(message_channel reply_ch)
    {
      message m(message::device_list_req_id, 0);

      m.device_list_req.reply = reply_ch;

      message_ch_.send(std::move(m));
    }
  public:
    void settings_changed()
    {
      message m(message::settings_changed_id, 0);

      message_ch_.send(std::move(m));
    }
  public:
    void stream_data(unsigned stream_id, message_channel reply_ch)
    {
      message m(message::stream_data_req_id, 0);

      m.stream_data_req.stream_id = stream_id;
      m.stream_data_req.reply = reply_ch;

      message_ch_.send(std::move(m));
    }
  public:
    void play(const std::string& id = std::string{})
    {
      message m(message::play_req_id, 0);

      m.play_req.id = id;

      message_ch_.send(std::move(m));
    }
  public:
    void stop()
    {
      message m(message::stop_req_id, 0);

      message_ch_.send(std::move(m));
    }
  public:
    void skip()
    {
      message m(message::skip_req_id, 0);

      message_ch_.send(std::move(m));
    }
  public:
    void queue(const std::string& id)
    {
      message m(message::queue_req_id, 0);

      m.queue_req.id = id;

      message_ch_.send(std::move(m));
    }
  public:
    void source_notification(source_notification::id type, const std::string& source_name, const std::string& source_message)
    {
      message m(message::source_notify_id, 0);

      m.source_notify.type = type;
      m.source_notify.source_name = source_name;
      m.source_notify.message = source_message;

      message_ch_.send(std::move(m));
    }
  public:
    static void start(dripcore::loop* loop)
    {
      loop->spawn<player_task>(message_ch_);
    }
  private:
    static message_channel message_ch_;
  };
}

// ----------------------------------------------------------------------------
#endif
