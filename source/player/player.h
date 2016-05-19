// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
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
namespace musicbox
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

      m.subscribe.channel = ch;

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
    void audio_device(const std::string& device_name)
    {
      message m(message::device_id, 0);

      m.device.device_name = device_name;

      message_ch_.send(std::move(m));
    }
  public:
    void play(const std::string& id)
    {
      message m(message::play_req_id, 0);

      m.play_req.id = id;

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
