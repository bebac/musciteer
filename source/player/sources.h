// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__sources_h__
#define __player__sources_h__

// ----------------------------------------------------------------------------
#include "audio_output.h"
#include "player_session.h"
#include "source_local_task.h"
#include "source_spotify_task.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  class sources
  {
  public:
    void play(std::shared_ptr<player_session> session)
    {
      auto track = session->track();
      assert(track);

      auto sources = track->sources();

      if ( sources.size() > 0 )
      {
        auto& source = sources[0];

        if ( source.name() == "local" ) {
          local_session_ch_.send(std::move(session));
        }
        else if ( source.name() == "spotify" ) {
          //spotify_session_ch_.send(std::move(session));
        }
        else {
          // TODO: Error!
        }
      }
      else
      {
        // TODO: Error!
      }
    }
  public:
    static void start(dripcore::loop* loop)
    {
      loop->spawn<source_local_task>(local_session_ch_);
      //loop->spawn<source_spotify_task>(spotify_session_ch_);
    }
  private:
    static session_channel local_session_ch_;
    //static session_channel spotify_session_ch_;
  };
}

// ----------------------------------------------------------------------------
#endif
