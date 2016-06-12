// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__source_local_h__
#define __player__source_local_h__

// ----------------------------------------------------------------------------
#include "player_session.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/channel.h>

// ----------------------------------------------------------------------------
namespace musicbox
{
  using session_channel = dripcore::channel<std::shared_ptr<player_session>>;

  class source_local_task : public dripcore::task
  {
  public:
    source_local_task(session_channel channel);
  private:
    void main() final;
  private:
    session_channel ch_;
  };
}

// ----------------------------------------------------------------------------
#endif
