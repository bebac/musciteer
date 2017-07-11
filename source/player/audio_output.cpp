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
audio_output_alsa::audio_output_alsa()
  :
  state_(state_closed),
  stream_begin_time_(),
  stream_notify_next_(),
  stream_length_(),
  rg_enabled_(false),
  rg_(0),
  scale_(1.0),
  handle_(nullptr),
  msg_ch_(),
  thr_(&audio_output_alsa::loop, this)
{
}

// ----------------------------------------------------------------------------
audio_output_alsa::~audio_output_alsa()
{
  msg_ch_.send(message());
  thr_.join();
}

// ----------------------------------------------------------------------------
void audio_output_alsa::send(message&& m)
{
  msg_ch_.send(std::move(m));
}

// ----------------------------------------------------------------------------
void audio_output_alsa::send(audio_buffer&& buffer)
{
  message m(message::stream_buffer_id);

  m.stream_buffer.buffer = std::move(buffer);
  msg_ch_.send(std::move(m));
}

// ----------------------------------------------------------------------------
void audio_output_alsa::loop()
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
void audio_output_alsa::dispatch(message& m)
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
    default:
      std::cerr << "audio output unhandled message type " << m.type << std::endl;
      break;
  }
}

// ----------------------------------------------------------------------------
void audio_output_alsa::handle(subscribe& m)
{
  observers_.insert(m.channel);
}

// ----------------------------------------------------------------------------
void audio_output_alsa::handle(unsubscribe& m)
{
  observers_.erase(m.channel);
}

// ----------------------------------------------------------------------------
void audio_output_alsa::handle(device_list_request& m)
{
  message r(message::device_list_res_id);

  void **hints, **n;
  char *name, *descr, *io;

  if (snd_device_name_hint(-1, "pcm", &hints) < 0)
    return;

  n = hints;

  while (*n != NULL)
  {
    name = snd_device_name_get_hint(*n, "NAME");
    descr = snd_device_name_get_hint(*n, "DESC");
    io = snd_device_name_get_hint(*n, "IOID");

    if ( (io == NULL) || (io && strcmp(io, "Output") == 0) ) {
      r.device_list_res.device_names.push_back(name);
    }

    if (name != NULL) {
      free(name);
    }
    if (descr != NULL) {
      free(descr);
    }
    if (io != NULL) {
      free(io);
    }

    n++;
  }
  snd_device_name_free_hint(hints);

  // Just loopback current.
  r.device_list_res.current = m.current;

  m.reply.send(std::move(r));
}

// ----------------------------------------------------------------------------
void audio_output_alsa::handle(open_request& m)
{
  message r(message::open_res_id);

  switch ( state_ )
  {
    case state_closed:
    {
      assert(handle_ == nullptr);

      rg_enabled_ = m.replaygain_enabled;

      int err = snd_pcm_open(&handle_, m.device_name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);

      if ( err == 0 )
      {
        state_ = state_open;
        r.open_res.error_code = 0;
      }
      else
      {
        r.open_res.error_code = err;
        r.open_res.error_message = snd_strerror(err);
      }
      break;
    }
    case state_open:
    {
      assert(handle_);

      rg_enabled_ = m.replaygain_enabled;

      r.open_res.error_code = 0;
      break;
    }
    case state_playing:
    {
      assert(handle_);

      rg_enabled_ = m.replaygain_enabled;

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
void audio_output_alsa::handle(close_request& m)
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
      assert(handle_);

      int err = snd_pcm_close(handle_);

      if ( err == 0 )
      {
        handle_ = nullptr;
        state_ = state_closed;
        r.close_res.error_code = 0;
      }
      else
      {
        r.open_res.error_code = err;
        r.open_res.error_message = snd_strerror(err);
      }
      break;
    }
  }

  m.reply.send(std::move(r));
}

// ----------------------------------------------------------------------------
void audio_output_alsa::handle(stream_begin& m)
{
  switch ( state_ )
  {
    case state_closed:
      break;
    case state_open:
    {
      assert(handle_);

      int err;

      err = snd_pcm_set_params(handle_, SND_PCM_FORMAT_S32_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, m.sample_rate, 0, 100000);

      if ( err < 0 ) {
        //_log_(error) << "snd_pcm_set_params failed! " << snd_strerror(err);
        std::cerr << "snd_pcm_set_params failed! " << snd_strerror(err);
        break;
      }

      err = snd_pcm_prepare(handle_);

      if ( err < 0 ) {
        std::cerr << "prepare pcm error " << snd_strerror(err) << std::endl;
      }
      else
      {
        completed_buffer_ch_ = m.completed_buffer_ch;
        stream_id_ = m.stream_id;
        stream_begin_time_ = std::chrono::steady_clock::now();
        stream_notify_next_ = stream_begin_time_ + std::chrono::seconds(1);
        stream_length_ = m.length;
        rg_ = m.replaygain;

        if ( rg_enabled_ ) {
          scale_ = std::pow(10, rg_ / 20);
        }
        else {
          scale_ = 1.0;
        }

        if ( scale_ > 1.0 )
        {
          auto peak = m.replaygain_peak;

          if ( scale_*peak > 1.0 )
          {
            std::cerr << "audio_output - calculated scale " << scale_ << " will cause clipping, adjusting..." << std::endl;
            scale_ = 0.99/peak;
          }
        }

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
      break;
    }
    case state_playing:
      // TODO: ERROR!
      break;
  }
}

// ----------------------------------------------------------------------------
void audio_output_alsa::handle(stream_end& m)
{
  switch ( state_ )
  {
    case state_closed:
      break;
    case state_open:
      break;
    case state_playing:
    {
      assert(handle_);

      auto res = snd_pcm_drain(handle_);

      //update_stream_time(true);

      if ( res < 0 ) {
        std::cerr << "drain pcm error " << snd_strerror(res) << std::endl;
      }
      else
      {
        m.reply.send(true);
        state_ = state_open;

        for ( auto observer : observers_ )
        {
          message m(message::stream_end_notify_id);
          auto& n = m.stream_end_notify;

          n.stream_id = stream_id_;

          observer.send(std::move(m));
        }
      }
      break;
    }
  }
}

// ----------------------------------------------------------------------------
void audio_output_alsa::handle(stream_buffer& m)
{
  switch ( state_ )
  {
    case state_closed:
      break;
    case state_open:
      break;
    case state_playing:
    {
      assert(handle_);

      auto buffer = std::move(m.buffer);

      update_stream_time();

      if ( rg_enabled_ ) {
        buffer.scale(scale_);
      }

      snd_pcm_sframes_t frames = snd_pcm_writei(handle_, buffer.data(), buffer.size());

      if ( frames < 0 ) {
        std::cerr << "audio output underrun" << std::endl;
        frames = snd_pcm_recover(handle_, frames, 0);
      }

      if ( frames < 0 ) {
        std::cerr << "audio output error! " << snd_strerror(frames) << std::endl;
      }
      else {
      }

      update_stream_time();

      // Return completed buffer for recycling.
      completed_buffer_ch_.send(std::move(buffer));
      break;
    }
  }
}

// ----------------------------------------------------------------------------
void audio_output_alsa::update_stream_time(bool last)
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