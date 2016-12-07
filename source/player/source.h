// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__source_h__
#define __player__source_h__

// ----------------------------------------------------------------------------
#include "player_session.h"

// ----------------------------------------------------------------------------
#include <dripcore/loop.h>
#include <dripcore/task.h>
#include <dripcore/task_channel.h>

// ----------------------------------------------------------------------------
namespace musciteer
{
  using done_channel  = dripcore::task_channel<bool, 1>;
  using done_ichannel = dripcore::task_ichannel<bool>;
  using done_ochannel = dripcore::task_ochannel<bool>;

  class source
  {
  public:
    source(dripcore::loop* loop) : session_ch_(), done_ch_(), loop_(loop)
    {
    }
  public:
    virtual void start() = 0;
    virtual void restart() = 0;
  public:
    void send(std::shared_ptr<player_session> session)
    {
      session_ch_.send(std::move(session));
    }
  protected:
    template<typename T, typename... Args>
    std::weak_ptr<dripcore::task> spawn(Args&&... args)
    {
      return loop_->spawn<T>(std::forward<Args>(args)...);
    }
  protected:
    session_channel session_ch_;
    done_channel done_ch_;
  private:
    dripcore::loop* loop_;
  };
}

// ----------------------------------------------------------------------------
#endif