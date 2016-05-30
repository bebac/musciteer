// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __player__sources_h__
#define __player__sources_h__

// ----------------------------------------------------------------------------
#include "audio_output.h"
#include "player_session.h"
#include "source_local_task.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  class sources
  {
  public:
    void play(std::shared_ptr<player_session> session)
    {
      local_session_ch_.send(std::move(session));
    }
  public:
    static void start(dripcore::loop* loop)
    {
      loop->spawn<source_local_task>(local_session_ch_);
    }
  private:
    static session_channel local_session_ch_;
  };
}

// ----------------------------------------------------------------------------
#endif
