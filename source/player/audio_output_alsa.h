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
#include <functional>
#include <memory>
#include <atomic>

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
struct s16_le_i_frame
{
  int16_t l;
  int16_t r;
};

// Interleaved signed 32 bit frame
struct s32_le_i_frame
{
  int32_t l;
  int32_t r;
};

// FLAC delivers samples non interleaved in 32 bit. Only handle 2 channels.
struct s32_le_n_frame
{
  int32_t* l;
  int32_t* r;
};

// ----------------------------------------------------------------------------
class audio_output_alsa
{
  using error_handler = std::function<void(int error_code)>;
public:
  audio_output_alsa();
public:
  void open(const std::string& device_name);
  void close();
public:
  operator bool() const;
public:
  void set_error_handler(error_handler);
  void clr_error_handler();
public:
  const char* strerror(int error_code) { return snd_strerror(error_code); }
public:
  float get_replaygain_scale() const;
public:
  bool get_replaygain_enabled() const;
  void set_replaygain_enabled(bool value);
  float get_replaygain() const;
  void set_replaygain(float rg, float peak);
  void set_params(int channels, unsigned sample_rate);
public:
  unsigned int hw_period_samples() const;
public:
  void prepare();
  bool start();
  bool drain();
  void drop();
public:
  snd_pcm_sframes_t avail_update();
  bool mmap_begin(const snd_pcm_channel_area_t** areas,  snd_pcm_uframes_t* offset, snd_pcm_uframes_t* frames);
  snd_pcm_uframes_t mmap_commit(snd_pcm_uframes_t offset, snd_pcm_uframes_t frames);
public:
  int recover(int error, int silent);
public:
  bool is_open() const;
  bool is_setup() const;
  bool is_prepared() const;
  bool is_running() const;
  bool is_xrun() const;
  bool is_draining() const;
public:
  const char* strstate() const;
public:
  static void each(std::function<void(std::string&& device_name)> value_cb);
private:
  class audio_output_alsa_impl;
  std::shared_ptr<audio_output_alsa_impl> pimpl_;
};

#endif
