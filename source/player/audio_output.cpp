// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include "audio_output.h"

// ----------------------------------------------------------------------------
#include <cmath>

// ----------------------------------------------------------------------------
audio_output::audio_output()
  :
  state_(state_closed),
  stream_begin_time_(),
  stream_notify_next_(),
  stream_length_(),
  rg_enabled_(false),
  rg_(0),
  scale_(1.0),
  output_(),
  msg_ch_(),
  thr_(&audio_output::loop, this)
{
}

// ----------------------------------------------------------------------------
audio_output::~audio_output()
{
  msg_ch_.send(message());
  thr_.join();
}

// ----------------------------------------------------------------------------
void audio_output::send(message&& m)
{
  msg_ch_.send(std::move(m));
}

// ----------------------------------------------------------------------------
void audio_output::send(audio_buffer&& buffer)
{
  message m(message::stream_buffer_id);

  m.stream_buffer.buffer = std::move(buffer);
  msg_ch_.send(std::move(m));
}

// ----------------------------------------------------------------------------
void audio_output::loop()
{
  while ( true )
  {
    auto req = msg_ch_.recv();

    if ( req.type == 0 ) {
      break;
    }

    dispatch(req);
  }
}

// ----------------------------------------------------------------------------
void audio_output::dispatch(message& m)
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
      handle(m.device_list_req);
      break;
    case message::open_req_id:
      handle(m.open_req);
      break;
    case message::close_req_id:
      handle(m.open_req);
      break;
    case message::stream_begin_id:
      handle(m.stream_begin);
      break;
    case message::stream_end_id:
      handle(m.stream_end);
      break;
    case message::stream_buffer_id:
      handle(m.stream_buffer);
      break;
    case message::replaygain_req_id:
      handle(m.replaygain_req);
      break;
    default:
      std::cerr << "audio output unhandled message type " << m.type << std::endl;
      break;
  }
}

// ----------------------------------------------------------------------------
void audio_output::handle(subscribe& m)
{
  observers_.insert(m.channel);
}

// ----------------------------------------------------------------------------
void audio_output::handle(unsubscribe& m)
{
  observers_.erase(m.channel);
}

// ----------------------------------------------------------------------------
void audio_output::handle(device_list_request& m)
{
  message r(message::device_list_res_id);

  audio_output_alsa::each([&](std::string&& device_name) {
    r.device_list_res.device_names.push_back(device_name);
  });

  // Just loopback current.
  r.device_list_res.current = m.current;

  m.reply.send(std::move(r));
}

// ----------------------------------------------------------------------------
void audio_output::handle(open_request& m)
{
  message r(message::open_res_id);

  rg_enabled_ = m.replaygain_enabled;

  switch ( state_ )
  {
    case state_closed:
    {
      try
      {
        output_.open(m.device_name);
        state_ = state_open;
        r.open_res.error_code = 0;
      }
      catch(const audio_output_error& err)
      {
        r.open_res.error_code = err.error_code();
        r.open_res.error_message = err.what();
      }
      break;
    }
    case state_open:
    {
      r.open_res.error_code = 0;
      break;
    }
    case state_playing:
    {
      r.open_res.error_code = 0;
      break;
    }
  }

  std::cout
    << "audio_output - open [ device_name=" << m.device_name
    << ", replaygain_enabled=" << m.replaygain_enabled << " ]"
    << std::endl;

  m.reply.send(std::move(r));
}

// ----------------------------------------------------------------------------
void audio_output::handle(close_request& m)
{
  message r(message::close_res_id);

  switch ( state_ )
  {
    case state_closed:
      r.close_res.error_code = 0;
      break;
    case state_open:
    case state_playing:
    {
      try
      {
        output_.close();
        state_ = state_closed;
        r.close_res.error_code = 0;
      }
      catch(const audio_output_error& err)
      {
        r.open_res.error_code = err.error_code();
        r.open_res.error_message = err.what();
      }
      break;
    }
  }

  m.reply.send(std::move(r));
}

// ----------------------------------------------------------------------------
void audio_output::handle(stream_begin& m)
{
  switch ( state_ )
  {
    case state_closed:
      break;
    case state_open:
    {
      try
      {
        completed_buffer_ch_ = m.completed_buffer_ch;
        stream_id_ = m.stream_id;
        stream_begin_time_ = std::chrono::steady_clock::now();
        stream_notify_next_ = stream_begin_time_ + std::chrono::seconds(1);
        stream_length_ = m.length;
        rg_ = m.replaygain;
        rg_peak_ = m.replaygain_peak;

        output_.set_params(2, m.sample_rate);
        output_.prepare();

        calculate_scale();

        state_ = state_playing;

        for ( auto observer : observers_ )
        {
          message m(message::stream_begin_notify_id);
          auto& n = m.stream_begin_notify;

          n.stream_id = stream_id_;
          n.replaygain_enabled = rg_enabled_;
          n.replaygain = rg_;
          n.scale = scale_;

          observer.send(std::move(m));
        }
      }
      catch(const audio_output_error& err)
      {
        std::cerr << "alsa output error " << err.what() << std::endl;
      }
      break;
    }
    case state_playing:
      // TODO: ERROR!
      break;
  }
}

// ----------------------------------------------------------------------------
void audio_output::handle(stream_end& m)
{
  switch ( state_ )
  {
    case state_closed:
      break;
    case state_open:
      break;
    case state_playing:
    {
      output_.drain();

      m.reply.send(true);
      state_ = state_open;

      for ( auto observer : observers_ )
      {
        message m(message::stream_end_notify_id);
        auto& n = m.stream_end_notify;

        n.stream_id = stream_id_;

        observer.send(std::move(m));
      }
      break;
    }
  }
}

// ----------------------------------------------------------------------------
void audio_output::handle(stream_buffer& m)
{
  switch ( state_ )
  {
    case state_closed:
      break;
    case state_open:
      break;
    case state_playing:
    {
      auto buffer = std::move(m.buffer);

      update_stream_time();

      if ( rg_enabled_ ) {
        buffer.scale(scale_);
      }

      output_.writei(buffer.data(), buffer.size());

      update_stream_time();

      // Return completed buffer for recycling.
      completed_buffer_ch_.send(std::move(buffer));
      break;
    }
  }
}

// ----------------------------------------------------------------------------
void audio_output::handle(replaygain_request& m)
{
  rg_enabled_ = m.replaygain_enabled;

  switch ( state_ )
  {
    case state_closed:
      break;
    case state_open:
      break;
    case state_playing:
      calculate_scale();
      break;
  }
}

// ----------------------------------------------------------------------------
void audio_output::calculate_scale()
{
  if ( rg_enabled_ ) {
    scale_ = std::pow(10, rg_ / 20);
  }
  else {
    scale_ = 1.0;
  }

  if ( scale_ > 1.0 )
  {
    auto peak = rg_peak_;

    if ( scale_ * peak > 1.0 )
    {
      std::cerr
        << "audio_output - calculated scale " << scale_
        << " will cause clipping, adjusting..."
        << std::endl;

      scale_ = 0.99 / peak;
    }
  }
}

// ----------------------------------------------------------------------------
void audio_output::update_stream_time(bool last)
{
  using std::chrono::steady_clock;
  using std::chrono::milliseconds;
  using std::chrono::duration_cast;

  auto now = steady_clock::now();

  if ( now > stream_notify_next_ || last )
  {
    steady_clock::duration stream_time = now - stream_begin_time_;
    steady_clock::duration offset = now - stream_notify_next_;

    for ( auto observer : observers_ )
    {
      message m(message::stream_progress_notify_id);
      auto& n = m.stream_progress_notify;

      n.stream_id = stream_id_;
      n.duration = duration_cast<milliseconds>(stream_time).count();
      n.length = duration_cast<milliseconds>(stream_length_).count();

      observer.send(std::move(m));
    }

    stream_notify_next_ = now + milliseconds(1000) - offset;
  }
}