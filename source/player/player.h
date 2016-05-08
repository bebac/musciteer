// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __player__player_h__
#define __player__player_h__

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
    player_impl(dripcore::loop* loop)
      : loop_(loop), audio_output_device_()
    {
    }
  public:
    void subscribe(message_channel ch);
    void unsubscribe(message_channel ch);
  public:
    void audio_device_list(message_channel reply_ch);
    void audio_device(const std::string& device_name);
  public:
    void play();
  private:
    dripcore::loop* loop_;
    std::string audio_output_device_;
    audio_output_alsa audio_output_;
  };

  class player
  {
  public:
    using instance_ptr = std::shared_ptr<player_impl>;
  public:
    player()
    {
      assert(instance_);
    }
  public:
    void subscribe(message_channel ch)
    {
      assert(instance_);
      instance_->subscribe(ch);
    }
  public:
    void unsubscribe(message_channel ch)
    {
      assert(instance_);
      instance_->unsubscribe(ch);
    }
  public:
    void audio_device_list(message_channel reply_ch)
    {
      assert(instance_);
      instance_->audio_device_list(reply_ch);
    }
  public:
    void audio_device(const std::string& device_name)
    {
      assert(instance_);
      instance_->audio_device(device_name);
    }
  public:
    void play()
    {
      assert(instance_);
      instance_->play();
    }
  public:
    static void start(dripcore::loop* loop)
    {
      assert(!instance_);
      instance_.reset(new player_impl(loop));
    }
  private:
    static instance_ptr instance_;
  };
}

// ----------------------------------------------------------------------------
#endif
