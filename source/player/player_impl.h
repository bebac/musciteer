// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __player__player_impl_h__
#define __player__player_impl_h__

// ----------------------------------------------------------------------------
#include "audio_output.h"

// ----------------------------------------------------------------------------
#include <memory>
#include <string>
#include <vector>
#include <cassert>

// ----------------------------------------------------------------------------
namespace musicbox
{
  class player_impl
  {
  public:
    player_impl(dripcore::loop* loop);
  public:
    void subscribe(message_channel ch);
    void unsubscribe(message_channel ch);
  public:
    void audio_device_list(message_channel reply_ch);
    void audio_device(const std::string& device_name);
  public:
    void play(const std::string& id);
  private:
    dripcore::loop* loop_;
    std::string output_device_;
    audio_output_alsa audio_output_;
  private:
    static constexpr const char* output_device_key = "__output_device__";
  };
}

// ----------------------------------------------------------------------------
#endif
