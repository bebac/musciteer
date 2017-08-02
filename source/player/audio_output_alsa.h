// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__audio_output_alsa_h__
#define __player__audio_output_alsa_h__

// ----------------------------------------------------------------------------
#include "audio_output_error.h"

// ----------------------------------------------------------------------------
#include <alsa/asoundlib.h>

// ----------------------------------------------------------------------------
class alsa_error : public audio_output_error
{
public:
  using audio_output_error::audio_output_error;
public:
  const char* what() const noexcept override
  {
    return snd_strerror(error_code_);
  }
};

// ----------------------------------------------------------------------------
class audio_output_alsa
{
public:
  audio_output_alsa()
  {
  }
public:
  ~audio_output_alsa()
  {
    if ( handle_ )
    {
      try {
        close();
      }
      catch(const alsa_error& err) {
        std::cerr << err.what() << std::endl;
      }
    }
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
  void set_params(int channels, unsigned sample_rate)
  {
    auto err = snd_pcm_set_params(handle_, SND_PCM_FORMAT_S32_LE, SND_PCM_ACCESS_RW_INTERLEAVED, channels, sample_rate, 0, 100000);
    if ( err != 0 ) {
      throw alsa_error(err);
    }
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
  void writei(const void *buffer, snd_pcm_uframes_t size)
  {
    snd_pcm_sframes_t frames = snd_pcm_writei(handle_, buffer, size);

    if ( frames < 0 )
    {
      std::cerr << "alsa output " << snd_strerror(frames) << std::endl;
      frames = snd_pcm_recover(handle_, frames, 0);
    }

    if ( frames < 0 ) {
      throw alsa_error(frames);
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
  snd_pcm_t* handle_;
};

// ----------------------------------------------------------------------------
#endif
