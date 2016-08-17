// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include "player_task.h"
#include "player_session.h"
#include "audio_output.h"

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"
#include "../dm/player.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  player_task::player_task(message_channel message_ch)
    :
    state_(stopped),
    message_ch_(message_ch),
    audio_output_(new audio_output_alsa()),
    audio_output_device_(),
    observers_(),
    play_q_(),
    continuous_playback_(true),
    ctpb_provider_()
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
    musicbox::dm::player settings;

    audio_output_device_ = settings.audio_device();
    continuous_playback_ = settings.ctpb_enabled();

    // For now the only continuous playback provider is random.
    if ( settings.ctpb_type() == "random" )
    {
      ctpb_provider_.reset(new player_ctpb_provider());
    }
    else
    {
      std::cerr << "player_task - unknown ctpb provider" << std::endl;
    }

    std::cout << "player_task - loaded settings audio_device=" << audio_output_device_ << ", ctpb_enabled=" << continuous_playback_ << std::endl;
  }

  void player_task::dispatch(message& m)
  {
    switch ( m.type )
    {
      case message::subscribe_id:
        handle(m.subscribe);
        break;
      case message::unsubscribe_id:
        handle(m.unsubscribe);
        break;
      case message::device_list_req_id:
        handle(m.device_list_req, m.ref);
        break;
      case message::settings_changed_id:
        handle(m.settings_changed);
        break;
      case message::stream_data_req_id:
        handle(m.stream_data_req);
        break;
      case message::play_req_id:
        handle(m.play_req);
        break;
      case message::stop_req_id:
        handle(m.stop_req);
        break;
      case message::skip_req_id:
        handle(m.skip_req);
        break;
      case message::queue_req_id:
        handle(m.queue_req);
        break;
      case message::stream_begin_notify_id:
        handle(m.stream_begin_notify);
        break;
      case message::stream_progress_notify_id:
        handle(m.stream_progress_notify);
        break;
      case message::stream_end_notify_id:
        handle(m.stream_end_notify);
        break;
      case message::source_notify_id:
        handle(m.source_notify);
        break;
    }
  }

  void player_task::handle(subscribe& m)
  {
    observers_.insert(m.channel);
    audio_output_subscribe(m.channel);

    // Send source status to new observer.
    for ( const auto& s : source_status_ )
    {
      message n(message::source_notify_id);

      n.source_notify.type = source_notification::id::status;
      n.source_notify.source_name = s.first;
      n.source_notify.message = s.second;

      m.channel.send(std::move(n));
    }

    player_state_notify(state_);
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

    audio_output_->send(std::move(am));
  }

  void player_task::handle(settings_changed_message& m)
  {
    load_settings();
  }

  void player_task::handle(stream_data_request& m)
  {
    message r(message::stream_data_res_id);

    if ( session_ && session_->id() == m.stream_id )
    {
      r.stream_data_res.stream_id = session_->id();
      r.stream_data_res.track = session_->track();
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
        if ( m.id.empty() )
        {
          if ( !play_q_.empty() )
          {
            assert(!session_);
            become_playing(play_q_.front());
            play_q_.pop_front();
          }
          else if ( ctpb_provider_ )
          {
            auto track = ctpb_provider_->get_track();

            if ( !track.id_is_null() )
            {
              assert(!session_);
              become_playing(track);
            }
            else
            {
              std::cerr << "player_task - ctpb provider returned empty track!" << std::endl;
            }
          }
        }
        else
        {
          auto tracks = musicbox::dm::tracks();

          assert(!session_);
          become_playing(tracks.find_by_id(m.id));
        }
        break;
      }
      case playing:
        break;
      case stopping:
        break;
      case paused:
        break;
    }
  }

  void player_task::handle(stop_request& m)
  {
    switch ( state_ )
    {
      case stopped:
        break;
      case playing:
        assert(session_);
        session_->stop();
        state_ = stopping;
        break;
      case stopping:
        break;
      case paused:
        break;
    }
  }

  void player_task::handle(skip_request& m)
  {
    switch ( state_ )
    {
      case stopped:
        break;
      case playing:
      {
        assert(session_);
        auto track = session_->track();
        assert(track);
        track->increment_skip_count();
        session_->stop();
        break;
      }
      case stopping:
        break;
      case paused:
        break;
    }
  }

  void player_task::handle(queue_request& m)
  {
    auto tracks = musicbox::dm::tracks();

    auto track = tracks.find_by_id(m.id);

    if ( track.id_is_null() )
    {
      // ERROR!
      return;
    }

    switch ( state_ )
    {
      case stopped:
      {
        assert(!session_);
        play_q_.push_front(track);
        become_playing(play_q_.front());
        play_q_.pop_front();
        break;
      }
      case playing:
      {
        play_q_.push_back(track);
        queue_update_notify(track);
        break;
      }
      case stopping:
        break;
      case paused:
        break;
    }
  }

  void player_task::handle(stream_begin_notify& m)
  {
  }

  void player_task::handle(stream_progress_notify& m)
  {
    if ( session_ )
    {
      if ( m.stream_id == session_->id() )
      {
        if ( m.duration > 0 )
        {
          session_->fraction_played(float(m.duration) / float(m.length));
        }
      }
      else
      {
        std::cerr << "player_task - session id mismatch on stream progress notification" << std::endl;
      }
    }
    else
    {
      std::cerr << "player_task - received stream progress notify with session_ == nullptr" << std::endl;
    }
  }

  void player_task::handle(stream_end_notify& m)
  {
    if ( session_ )
    {
      auto tracks = musicbox::dm::tracks();
      auto track = session_->track();

      assert(track);

      if ( session_->fraction_played() > 0.80 ) {
        track->increment_play_count();
      }

      tracks.update(*track);
      end_session();
    }
    else
    {
      std::cerr << "player_task - received stream end notify with session_ == nullptr" << std::endl;
    }
  }

  void player_task::handle(source_notification& m)
  {
    std::cout
      << "got source notification"
      << " type=" << size_t(m.type)
      << ", source_name=" << m.source_name
      << ", message=" << m.message << std::endl;

    // Relay source notification to observers.
    for ( auto observer : observers_ )
    {
      message n(message::source_notify_id);

      n.source_notify.type = m.type;
      n.source_notify.source_name = m.source_name;
      n.source_notify.message = m.message;

      observer.send(std::move(n));
    }

    if ( m.type == source_notification::id::status )
    {
      // Save source status.
      source_status_[m.source_name] = std::move(m.message);
    }
    else if ( m.type == source_notification::id::track_unavailable )
    {
      end_session();
    }
  }

  void player_task::become_playing(const musicbox::dm::track& track)
  {
    if ( state_ == stopped ) {
      audio_output_open();
    }

    session_ = std::make_shared<player_session>();
    session_->track(track);
    session_->play(audio_output_);

    state_ = playing;

    player_state_notify(state_);
  }

  void player_task::become_stopped()
  {
    session_->done();
    session_.reset();

    state_ = stopped;

    player_state_notify(state_);
  }

  void player_task::end_session()
  {
    switch ( state_ )
    {
      case stopped:
        break;
      case playing:
      {
        session_->done();

        if ( !play_q_.empty() )
        {
          become_playing(play_q_.front());
          play_q_.pop_front();
        }
        else if ( continuous_playback_ )
        {
          assert(ctpb_provider_);

          auto track = ctpb_provider_->get_track();

          if ( !track.id_is_null() )
          {
            become_playing(track);
          }
          else
          {
            std::cerr << "player_task - ctpb provider returned empty track!" << std::endl;
            become_stopped();
          }
        }
        else
        {
          become_stopped();
        }
        break;
      }
      case stopping:
        become_stopped();
        break;
      case paused:
        break;
    }
  }

  void player_task::player_state_notify(unsigned state)
  {
    for ( auto observer : observers_ )
    {
      message n(message::player_state_id);

      n.player_state.state = state;

      observer.send(std::move(n));
    }
  }

  void player_task::queue_update_notify(const musicbox::dm::track& track)
  {
    for ( auto observer : observers_ )
    {
      message n(message::queue_update_id);

      n.queue_update.queue_size = play_q_.size();
      n.queue_update.track = std::make_shared<musicbox::dm::track>(track);

      observer.send(std::move(n));
    }
  }

  void player_task::audio_output_subscribe(message_channel& ch)
  {
    message m(message::subscribe_id, 0);

    m.subscribe.channel = ch;

    audio_output_->send(std::move(m));
  }

  void player_task::audio_output_unsubscribe(message_channel& ch)
  {
    message m(message::unsubscribe_id, 0);

    m.subscribe.channel = ch;

    audio_output_->send(std::move(m));
  }

  void player_task::audio_output_open()
  {
    message_channel ch;
    message m(message::open_req_id, 0);

    m.open_req.device_name = audio_output_device_;
    m.open_req.reply = ch;

    audio_output_->send(std::move(m));

    auto r = ch.recv(this);

    std::cout << "audio output open response error_code=" << r.open_res.error_code << ", error_message=" << r.open_res.error_message << std::endl;
  }
}
