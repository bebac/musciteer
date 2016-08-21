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
#include "source.h"
#include "source_local.h"
#include "source_spotify.h"

// ----------------------------------------------------------------------------
namespace musciteer
{
  class sources
  {
    using source_ptr = std::shared_ptr<source>;
  public:
    void play(std::shared_ptr<player_session> session);
  public:
    void settings_changed(const std::string source_name);
  public:
    static void start(dripcore::loop* loop);
  private:
    static source_ptr source_local_;
    static source_ptr source_spotify_;
  };
}

// ----------------------------------------------------------------------------
#endif
