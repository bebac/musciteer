// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include "player.h"
#include "player_task_flac.h"

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  /////
  // Instantiate the one and only player.

  player::instance_ptr player::instance_;

  /////
  // Player implementation.

  player_impl::player_impl(dripcore::loop* loop)
    : loop_(loop), output_device_()
  {
    auto kvstore = musicbox::kvstore();

    kvstore.get(output_device_key, output_device_);
  }

  void player_impl::subscribe(message_channel ch)
  {
    message req(message::subscribe_id, 0);

    req.subscribe.channel = ch;

    audio_output_.send(std::move(req));
  }

  void player_impl::unsubscribe(message_channel ch)
  {
    message req(message::unsubscribe_id, 0);

    req.subscribe.channel = ch;

    audio_output_.send(std::move(req));
  }

  void player_impl::audio_device_list(message_channel reply_ch)
  {
    message req(message::device_list_req_id, 0);

    req.device_list_req.current = output_device_;
    req.device_list_req.reply = reply_ch;

    audio_output_.send(std::move(req));
  }

  void player_impl::audio_device(const std::string& device_name)
  {
    auto kvstore = musicbox::kvstore();

    output_device_ = device_name;

    kvstore.set(output_device_key, output_device_);
  }

  void player_impl::play(const std::string& id)
  {
    {
      message req(message::open_req_id, 0);

      req.open_req.device_name = output_device_;
      //m.open_req.reply = output_channel_;
      audio_output_.send(std::move(req));
    }

    {
      message req(message::play_req_id, 0);

      auto tracks = musicbox::tracks();

      auto track = tracks.find_by_id(id);
      auto sources = track.sources();

      if ( sources.size() > 0 )
      {
        auto filename = sources[0].uri();

        req.play_req.stream_id = 1;
        req.play_req.uri = filename;

        loop_->spawn<player_task_flac>(req.play_req, audio_output_);
      }
      else
      {
        std::cout << "no source!" << std::endl;
      }
    }
  }
}
