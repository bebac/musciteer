// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include "player_task.h"
#include "player_task_flac.h"

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  player_task::player_task(message_channel message_ch)
    : state_(stopped), message_ch_(message_ch)
  {
  }

  void player_task::main()
  {
    load_settings();

    while ( true )
    {
      auto msg = message_ch_.recv(this);

      if ( msg.type == 0 ) {
        break;
      }

      dispatch(msg);
    }
  }

  void player_task::load_settings()
  {
    auto kvstore = musicbox::kvstore();

    kvstore.get(audio_output_device_key, audio_output_device_);
  }

  void player_task::dispatch(message& m)
  {
    switch ( m.type )
    {
      case message::subscribe_id:
        handle(m.subscribe);
        break;
      case message::unsubscribe_id:
        handle(m.subscribe);
        break;
      case message::device_list_req_id:
        handle(m.device_list_req, m.ref);
        break;
      case message::device_id:
        handle(m.device);
        break;
      case message::play_req_id:
        handle(m.play_req);
        break;
    }
  }

  void player_task::handle(subscribe& m)
  {
    message am(message::subscribe_id, 0);

    am.subscribe = std::move(m);

    audio_output_.send(std::move(am));
  }

  void player_task::handle(unsubscribe& m)
  {
    message am(message::unsubscribe_id, 0);

    am.unsubscribe = std::move(m);

    audio_output_.send(std::move(am));
  }

  void player_task::handle(device_list_request& m, unsigned ref)
  {
    message am(message::device_list_req_id, ref);

    am.device_list_req = std::move(m);
    am.device_list_req.current = audio_output_device_;

    audio_output_.send(std::move(am));
  }

  void player_task::handle(audio_output_device& m)
  {
    auto kvstore = musicbox::kvstore();

    audio_output_device_ = m.device_name;

    kvstore.set(audio_output_device_key, audio_output_device_);
  }

  void player_task::handle(play_request& m)
  {
    switch ( state_ )
    {
      case stopped:
      {
        open_audio_output();

        auto tracks = musicbox::tracks();

        auto track = tracks.find_by_id(m.id);
        auto sources = track.sources();

        if ( sources.size() > 0 )
        {
          auto filename = sources[0].uri();

          spawn<player_task_flac>(filename, 1, audio_output_);
        }
        break;
      }
      case playing:
        break;
      case paused:
        break;
    }
  }

  void player_task::open_audio_output()
  {
    message_channel ch;
    message m(message::open_req_id, 0);

    m.open_req.device_name = audio_output_device_;
    m.open_req.reply = ch;

    audio_output_.send(std::move(m));

    auto r = ch.recv(this);

    std::cout << "audio output open response error_code=" << r.open_res.error_code << ", error_message=" << r.open_res.error_message << std::endl;
  }
}
