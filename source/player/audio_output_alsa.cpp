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
class audio_output_alsa::audio_output_alsa_impl
{
public:
  audio_output_alsa_impl() : replaygain_enabled_(false), replaygain_(0.0), replaygain_scale_(1.0)
  {
  }
public:
  void open(const std::string& device_name)
  {
    auto err = snd_pcm_open(&handle_, device_name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if ( err != 0 ) {
      throw alsa_error(err);
    }
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
  unsigned int hw_period_samples() const { return 1024; }
public:
  void set_params(int channels, unsigned sample_rate)
  {
    int err;

    snd_pcm_hw_params_t *hw_params;

    snd_pcm_hw_params_malloc(&hw_params);

    if ( (err = snd_pcm_hw_params_any(handle_, hw_params)) < 0 ) {
      throw alsa_error(err);
    }

    if ( (err = snd_pcm_hw_params_set_format(handle_, hw_params, SND_PCM_FORMAT_S32_LE)) < 0 ) {
      throw alsa_error(err);
    }

    if ( (err = snd_pcm_hw_params_set_rate(handle_, hw_params, sample_rate, 0)) < 0 ) {
      throw alsa_error(err);
    }

    if ( (err = snd_pcm_hw_params_set_channels(handle_, hw_params, channels)) < 0 ) {
      throw alsa_error(err);
    }

    if ( (err = snd_pcm_hw_params_set_access(handle_, hw_params, SND_PCM_ACCESS_MMAP_INTERLEAVED)) < 0 ) {
      throw alsa_error(err);
    }

    unsigned int buffer_us = 1000000;
    int buffer_dir;

    if ( (err = snd_pcm_hw_params_set_buffer_time_max(handle_, hw_params, &buffer_us, &buffer_dir)) < 0 ) {
      throw alsa_error(err);
    }

    if ( (err = snd_pcm_hw_params_set_buffer_time_near(handle_, hw_params, &buffer_us, &buffer_dir)) < 0 ) {
      throw alsa_error(err);
    }

    unsigned int period_us = hw_period_samples()*1000000/sample_rate;
    int period_dir;

    if ( (err = snd_pcm_hw_params_set_period_time_near(handle_, hw_params, &period_us, &period_dir)) < 0 ) {
      throw alsa_error(err);
    }

    if ( (err = snd_pcm_hw_params(handle_, hw_params)) < 0 ) {
      throw alsa_error(err);
    }

    snd_pcm_hw_params_free(hw_params);
  }
public:
  void prepare()
  {
    auto err = snd_pcm_prepare(handle_);
    if ( err != 0 ) {
      throw alsa_error(err);
    }
  }
public:
  void start()
  {
    auto err = snd_pcm_start(handle_);
    if ( err != 0 ) {
      throw alsa_error(err);
    }
  }
public:
  void drain()
  {
    auto err = snd_pcm_drain(handle_);
    if ( err != 0 ) {
      throw alsa_error(err);
    }
  }
public:
  snd_pcm_sframes_t avail_update()
  {
    return snd_pcm_avail_update(handle_);
  }
public:
  void mmap_begin(const snd_pcm_channel_area_t** areas,  snd_pcm_uframes_t* offset, snd_pcm_uframes_t* frames)
  {
    auto err = snd_pcm_mmap_begin(handle_, areas, offset, frames);
    if ( err != 0 ) {
      throw alsa_error(err);
    }
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
  bool is_open()     { return snd_pcm_state(handle_) == SND_PCM_STATE_OPEN; }
  bool is_prepared() { return snd_pcm_state(handle_) == SND_PCM_STATE_PREPARED; }
  bool is_running()  { return snd_pcm_state(handle_) == SND_PCM_STATE_RUNNING; }
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
  std::atomic<bool>  replaygain_enabled_;
  std::atomic<float> replaygain_;
  std::atomic<float> replaygain_scale_;
private:
  snd_pcm_t* handle_;
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

void audio_output_alsa::start()
{
  pimpl_->start();
}

void audio_output_alsa::drain()
{
  pimpl_->drain();
}

snd_pcm_sframes_t audio_output_alsa::avail_update()
{
  return pimpl_->avail_update();
}

void audio_output_alsa::mmap_begin(const snd_pcm_channel_area_t** areas,  snd_pcm_uframes_t* offset, snd_pcm_uframes_t* frames)
{
  pimpl_->mmap_begin(areas, offset, frames);
}

snd_pcm_uframes_t audio_output_alsa::mmap_commit(snd_pcm_uframes_t offset, snd_pcm_uframes_t frames)
{
  return pimpl_->mmap_commit(offset, frames);
}

int audio_output_alsa::recover(int error, int silent)
{
  return pimpl_->recover(error, silent);
}

bool audio_output_alsa::is_open()
{
  return pimpl_->is_open();
}

bool audio_output_alsa::is_prepared()
{
  return pimpl_->is_prepared();
}

bool audio_output_alsa::is_running()
{
  return pimpl_->is_running();
}

void audio_output_alsa::each(std::function<void(std::string&& device_name)> value_cb)
{
  audio_output_alsa_impl::each(value_cb);
}
