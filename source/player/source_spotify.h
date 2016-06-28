// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__source_spotify_h__
#define __player__source_spotify_h__

// ----------------------------------------------------------------------------
#include "source.h"
#include "source_spotify_task.h"

// ----------------------------------------------------------------------------
#include <dripcore/loop.h>

// ----------------------------------------------------------------------------
namespace musicbox
{
  class source_spotify : public source
  {
    using source::source;
  public:
    void start() override
    {
      spawn<source_spotify_task>(session_ch_);
    }
  };
}

// ----------------------------------------------------------------------------
#endif