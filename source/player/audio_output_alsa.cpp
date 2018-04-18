// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include "audio_output_alsa.h"

// ----------------------------------------------------------------------------
#include <iostream>
#include <cmath>

// ----------------------------------------------------------------------------
namespace
{
  using error_handler = std::function<void(int error_code)>;

  void default_audio_output_error_handler(int error_code)
  {
    throw alsa_error(error_code);
  }
}

// ----------------------------------------------------------------------------
class audio_output_alsa::audio_output_alsa_impl
{
public:
  audio_output_alsa_impl()
    :
    replaygain_enabled_(false),
    replaygain_(0.0),
    replaygain_scale_(1.0),
    handle_(nullptr),
    error_handler_(default_audio_output_error_handler)
  {
  }
public:
  void open(const std::string& device_name)
  {
    check_snd_pcm_error(
      snd_pcm_open(&handle_, device_name.c_str(), SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)
    );
  }
public:
  void close()
  {
    auto err = snd_pcm_close(handle_);
    if ( err == 0 ) {
      handle_ = nullptr;
    }
    else {
      throw alsa_error(err);
    }
  }
public:
  bool handle_is_null() const
  {
    return handle_ == nullptr;
  }
public:
  void set_error_handler(error_handler func)
  {
    error_handler_ = func;
  }
  void clr_error_handler()
  {
    error_handler_ = default_audio_output_error_handler;
  }
public:
  float get_replaygain_scale() const
  {
    return replaygain_enabled_ ? replaygain_scale_.load() : 1.0f;
  }
public:
  bool get_replaygain_enabled() const { return replaygain_enabled_; }
  void set_replaygain_enabled(bool value) { replaygain_enabled_ = value; }
public:
  float get_replaygain() const
  {
    return replaygain_;
  }
  void set_replaygain(float rg, float peak)
  {
    auto scale = float{1.0};

    if ( replaygain_enabled_ ) {
      scale = std::pow(10, rg / 20);
    }

    if ( scale > 1.0 )
    {
      if ( scale * peak > 1.0 )
      {
        std::cerr
          << "audio_output - calculated scale " << scale
          << " will cause clipping, adjusting..."
          << std::endl;

        scale = 0.99 / peak;
      }
    }
    replaygain_ = rg;
    replaygain_scale_ = scale;
  }
public:
  constexpr unsigned int hw_period_samples() const { return 1024; }
public:
  void set_params(int channels, unsigned sample_rate)
  {
    snd_pcm_hw_params_t *hw_params;

    snd_pcm_hw_params_malloc(&hw_params);

    check_snd_pcm_error(
      snd_pcm_hw_params_any(handle_, hw_params)
    );

    check_snd_pcm_error(
      snd_pcm_hw_params_set_format(handle_, hw_params, SND_PCM_FORMAT_S32_LE)
    );

    check_snd_pcm_error(
      snd_pcm_hw_params_set_rate(handle_, hw_params, sample_rate, 0)
    );

    check_snd_pcm_error(
      snd_pcm_hw_params_set_channels(handle_, hw_params, channels)
    );

    check_snd_pcm_error(
      snd_pcm_hw_params_set_access(handle_, hw_params, SND_PCM_ACCESS_MMAP_INTERLEAVED)
    );

    unsigned int buffer_us = 1000000;
    int buffer_dir;

    check_snd_pcm_error(
      snd_pcm_hw_params_set_buffer_time_max(handle_, hw_params, &buffer_us, &buffer_dir)
    );

    check_snd_pcm_error(
      snd_pcm_hw_params_set_buffer_time_near(handle_, hw_params, &buffer_us, &buffer_dir)
    );

    unsigned int period_us = hw_period_samples()*1000000/sample_rate;
    int period_dir;

    check_snd_pcm_error(
      snd_pcm_hw_params_set_period_time_near(handle_, hw_params, &period_us, &period_dir)
    );

    check_snd_pcm_error(
      snd_pcm_hw_params(handle_, hw_params)
    );

    snd_pcm_hw_params_free(hw_params);
  }
public:
  void prepare()
  {
    check_snd_pcm_error(
      snd_pcm_prepare(handle_)
    );
  }
public:
  bool start()
  {
    return check_snd_pcm_error(
      snd_pcm_start(handle_)
    );
  }
public:
  bool drain()
  {
    return check_snd_pcm_error(
      snd_pcm_drain(handle_)
    );
  }
public:
  void drop()
  {
    check_snd_pcm_error(
      snd_pcm_drop(handle_)
    );
  }
public:
  snd_pcm_sframes_t avail_update()
  {
    return snd_pcm_avail_update(handle_);
  }
public:
  bool mmap_begin(const snd_pcm_channel_area_t** areas,  snd_pcm_uframes_t* offset, snd_pcm_uframes_t* frames)
  {
    return check_snd_pcm_error(
      snd_pcm_mmap_begin(handle_, areas, offset, frames)
    );
  }
public:
  snd_pcm_uframes_t mmap_commit(snd_pcm_uframes_t offset, snd_pcm_uframes_t frames)
  {
    return snd_pcm_mmap_commit(handle_, offset, frames);
  }
public:
  int recover(int error, int silent)
  {
    return snd_pcm_recover(handle_, error, silent);
  }
public:
  bool is_open() const     { return snd_pcm_state(handle_) == SND_PCM_STATE_OPEN; }
  bool is_setup() const    { return snd_pcm_state(handle_) == SND_PCM_STATE_SETUP; }
  bool is_prepared() const { return snd_pcm_state(handle_) == SND_PCM_STATE_PREPARED; }
  bool is_running() const  { return snd_pcm_state(handle_) == SND_PCM_STATE_RUNNING; }
  bool is_xrun() const     { return snd_pcm_state(handle_) == SND_PCM_STATE_XRUN; }
  bool is_draining() const { return snd_pcm_state(handle_) == SND_PCM_STATE_DRAINING; }
public:
  const char* strstate() const
  {
    switch ( snd_pcm_state(handle_) )
    {
      case SND_PCM_STATE_OPEN:
        return "open";
      case SND_PCM_STATE_SETUP:
        return "setup";
      case SND_PCM_STATE_PREPARED:
        return "prepared";
      case SND_PCM_STATE_RUNNING:
        return "running";
      case SND_PCM_STATE_XRUN:
        return "xrun";
      case SND_PCM_STATE_DRAINING:
        return "draining";
      case SND_PCM_STATE_PAUSED:
        return "paused";
      case SND_PCM_STATE_SUSPENDED:
        return "suspended";
      case SND_PCM_STATE_DISCONNECTED:
        return "disconnected";
      default:
        return "unknown";
    }
  }
public:
  static void each(std::function<void(std::string&& device_name)> value_cb)
  {
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
        value_cb(name);
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
  }
private:
  bool check_snd_pcm_error(int error_code)
  {
    if ( error_code < 0 )
    {
      error_handler_(error_code);
      return false;
    }
    else
    {
      return true;
    }
  }
private:
  std::atomic<bool>  replaygain_enabled_;
  std::atomic<float> replaygain_;
  std::atomic<float> replaygain_scale_;
private:
  snd_pcm_t* handle_;
private:
  error_handler error_handler_;
};

// ----------------------------------------------------------------------------
audio_output_alsa::audio_output_alsa()
  : pimpl_(new audio_output_alsa_impl)
{
}

void audio_output_alsa::open(const std::string& device_name)
{
  pimpl_->open(device_name);
}

void audio_output_alsa::close()
{
  pimpl_->close();
}

audio_output_alsa::operator bool() const
{
  return !pimpl_->handle_is_null();
}

void audio_output_alsa::set_error_handler(error_handler func)
{
  pimpl_->set_error_handler(func);
}

void audio_output_alsa::clr_error_handler()
{
  pimpl_->clr_error_handler();
}

float audio_output_alsa::get_replaygain_scale() const
{
  return pimpl_->get_replaygain_scale();
}

bool audio_output_alsa::get_replaygain_enabled() const
{
  return pimpl_->get_replaygain_enabled();
}

void audio_output_alsa::set_replaygain_enabled(bool value)
{
  return pimpl_->set_replaygain_enabled(value);
}

float audio_output_alsa::get_replaygain() const
{
  return pimpl_->get_replaygain();
}

void audio_output_alsa::set_replaygain(float rg, float peak)
{
  return pimpl_->set_replaygain(rg, peak);
}

unsigned int audio_output_alsa::hw_period_samples() const
{
  return pimpl_->hw_period_samples();
}

void audio_output_alsa::set_params(int channels, unsigned sample_rate)
{
  pimpl_->set_params(channels, sample_rate);
}

void audio_output_alsa::prepare()
{
  pimpl_->prepare();
}

bool audio_output_alsa::start()
{
  return pimpl_->start();
}

bool audio_output_alsa::drain()
{
  return pimpl_->drain();
}

void audio_output_alsa::drop()
{
  pimpl_->drop();
}

snd_pcm_sframes_t audio_output_alsa::avail_update()
{
  return pimpl_->avail_update();
}

bool audio_output_alsa::mmap_begin(const snd_pcm_channel_area_t** areas,  snd_pcm_uframes_t* offset, snd_pcm_uframes_t* frames)
{
  return pimpl_->mmap_begin(areas, offset, frames);
}

snd_pcm_uframes_t audio_output_alsa::mmap_commit(snd_pcm_uframes_t offset, snd_pcm_uframes_t frames)
{
  return pimpl_->mmap_commit(offset, frames);
}

int audio_output_alsa::recover(int error, int silent)
{
  return pimpl_->recover(error, silent);
}

bool audio_output_alsa::is_open() const
{
  return pimpl_->is_open();
}

bool audio_output_alsa::is_setup() const
{
  return pimpl_->is_setup();
}

bool audio_output_alsa::is_prepared() const
{
  return pimpl_->is_prepared();
}

bool audio_output_alsa::is_running() const
{
  return pimpl_->is_running();
}

bool audio_output_alsa::is_xrun() const
{
  return pimpl_->is_xrun();
}

bool audio_output_alsa::is_draining() const
{
  return pimpl_->is_draining();
}

const char* audio_output_alsa::strstate() const
{
  return pimpl_->strstate();
}

void audio_output_alsa::each(std::function<void(std::string&& device_name)> value_cb)
{
  audio_output_alsa_impl::each(value_cb);
}
