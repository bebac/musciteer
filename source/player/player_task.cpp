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
    audio_output_subscribe(message_ch_);
  }

  player_task::~player_task()
  {
    audio_output_unsubscribe(message_ch_);
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
      case message::stream_data_req_id:
        handle(m.stream_data_req);
        break;
      case message::play_req_id:
        handle(m.play_req);
        break;
      case message::queue_req_id:
        handle(m.queue_req);
        break;
      case message::stream_begin_notify_id:
        handle(m.stream_begin_notify);
        break;
      case message::stream_end_notify_id:
        handle(m.stream_end_notify);
        break;
    }
  }

  void player_task::handle(subscribe& m)
  {
    observers_.insert(m.channel);
    audio_output_subscribe(m.channel);
  }

  void player_task::handle(unsubscribe& m)
  {
    observers_.erase(m.channel);
    audio_output_unsubscribe(m.channel);
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

  void player_task::handle(stream_data_request& m)
  {
    message r(message::stream_data_res_id);

    if ( playing_id_ == m.stream_id )
    {
      r.stream_data_res.stream_id = playing_id_;
      r.stream_data_res.track = std::make_shared<musicbox::track>(playing_track_);
    }
    else
    {
      r.stream_data_res.stream_id = -1;
    }

    m.reply.send(std::move(r));
  }

  void player_task::handle(play_request& m)
  {
    switch ( state_ )
    {
      case stopped:
      {
        auto tracks = musicbox::tracks();

        playing_track_ = tracks.find_by_id(m.id);

        auto sources = playing_track_.sources();

        if ( sources.size() > 0 )
        {
          audio_output_open();
          play(sources[0].uri());
          state_ = playing;
        }
        break;
      }
      case playing:
        break;
      case paused:
        break;
    }
  }

  void player_task::handle(queue_request& m)
  {
    auto tracks = musicbox::tracks();

    auto track = tracks.find_by_id(m.id);

    if ( !track.id().empty() )
    {
      play_q_.push(track);
    }
    else
    {
      // ERROR!
      return;
    }

    switch ( state_ )
    {
      case stopped:
      {
        playing_track_ = play_q_.front();

        auto sources = playing_track_.sources();

        if ( sources.size() > 0 )
        {
          audio_output_open();
          play(sources[0].uri());
          state_ = playing;
        }

        play_q_.pop();
        break;
      }
      case playing:
      {
        for ( auto observer : observers_ )
        {
          message n(message::queue_update_id);

          n.queue_update.queue_size = play_q_.size();
          n.queue_update.track = std::make_shared<musicbox::track>(track);

          observer.send(std::move(n));
        }
        break;
      }
      case paused:
        break;
    }
  }

  void player_task::handle(stream_begin_notify& m)
  {
  }

  void player_task::handle(stream_end_notify& m)
  {
    switch ( state_ )
    {
      case stopped:
        std::cout << "got stream end while player_task state==stopped!" << std::endl;
        break;
      case playing:
      {
        if ( !play_q_.empty() )
        {
          playing_track_ = play_q_.front();

          auto sources = playing_track_.sources();

          if ( sources.size() > 0 )
          {
            play(sources[0].uri());
          }

          play_q_.pop();
        }
        else
        {
          playing_id_ = -1;
          state_ = stopped;
        }
        break;
      }
      case paused:
        break;
    }
  }

  void player_task::play(const std::string& uri)
  {
    auto kvstore = musicbox::kvstore();

    playing_id_ = kvstore.increment(stream_id_key, 1, 1);

    spawn<player_task_flac>(uri, playing_id_, audio_output_);
  }

  void player_task::audio_output_subscribe(message_channel& ch)
  {
    message m(message::subscribe_id, 0);

    m.subscribe.channel = ch;

    audio_output_.send(std::move(m));
  }

  void player_task::audio_output_unsubscribe(message_channel& ch)
  {
    message m(message::unsubscribe_id, 0);

    m.subscribe.channel = ch;

    audio_output_.send(std::move(m));
  }

  void player_task::audio_output_open()
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
