// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __player__source_local_h__
#define __player__source_local_h__

// ----------------------------------------------------------------------------
#include "player_session.h"
#include "player_task_flac.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  using session_channel = dripcore::channel<std::shared_ptr<player_session>>;

  class source_local_task : public dripcore::task
  {
  public:
    source_local_task(session_channel channel) : ch_(channel)
    {
    }
  private:
    void main() final
    {
      while ( true )
      {
        auto msg = ch_.recv(this);

        if ( !msg ) {
          break;
        }

        spawn<player_task_flac>(msg);
      }
    }
  private:
    session_channel ch_;
  };
}

// ----------------------------------------------------------------------------
#endif
