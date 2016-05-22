// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include "audio_output.h"

// ----------------------------------------------------------------------------
audio_output_alsa::audio_output_alsa()
  :
  state_(state_closed),
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
      handle(m.device_list_req, m.ref);
      break;
    case message::open_req_id:
      handle(m.open_req, m.ref);
      break;
    case message::close_req_id:
      handle(m.open_req, m.ref);
      break;
    case message::stream_begin_id:
      handle(m.stream_begin, m.ref);
      break;
    case message::stream_end_id:
      handle(m.stream_end, m.ref);
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
void audio_output_alsa::handle(device_list_request& m, unsigned ref)
{
  message r(message::device_list_res_id, ref);

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
void audio_output_alsa::handle(open_request& m, unsigned ref)
{
  message r(message::open_res_id, ref);

  switch ( state_ )
  {
    case state_closed:
    {
      assert(handle_ == nullptr);

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
      r.open_res.error_code = 0;
      break;
  }

  m.reply.send(std::move(r));
}

// ----------------------------------------------------------------------------
void audio_output_alsa::handle(close_request& m, unsigned ref)
{
  message r(message::close_res_id, ref);

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
void audio_output_alsa::handle(stream_begin& m, unsigned ref)
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
        state_ = state_playing;

        for ( auto observer : observers_ )
        {
          message n(message::stream_begin_notify_id);

          n.stream_begin_notify.stream_id = stream_id_;
          observer.send(std::move(n));
        }
      }
      break;
    }
  }
}

// ----------------------------------------------------------------------------
void audio_output_alsa::handle(stream_end& m, unsigned ref)
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
          message n(message::stream_end_notify_id);

          n.stream_end_notify.stream_id = stream_id_;
          observer.send(std::move(n));
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

      update_stream_time();

      snd_pcm_sframes_t frames = snd_pcm_writei(handle_, m.buffer.data(), m.buffer.size());

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
      completed_buffer_ch_.send(std::move(m.buffer));
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
      message n(message::stream_progress_notify_id);

      n.stream_progress_notify.stream_id = stream_id_;
      n.stream_progress_notify.duration = duration_cast<milliseconds>(stream_time).count();
      n.stream_progress_notify.length = duration_cast<milliseconds>(stream_length_).count();
      observer.send(std::move(n));
    }

    stream_notify_next_ = now + milliseconds(1000) - offset;
  }
}