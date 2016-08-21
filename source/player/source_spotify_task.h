// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__source_spotify_task_h__
#define __player__source_spotify_task_h__

// ----------------------------------------------------------------------------
#include "player_session.h"
#include "source.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/channel.h>

// ----------------------------------------------------------------------------
namespace musciteer
{
  using session_channel = dripcore::channel<std::shared_ptr<player_session>>;

  class source_spotify_task : public dripcore::task
  {
  public:
    source_spotify_task(session_channel channel, done_channel done_ch);
  private:
    void main() final;
  private:
    session_channel ch_;
    done_channel done_ch_;
  };
}

// ----------------------------------------------------------------------------
#endif
