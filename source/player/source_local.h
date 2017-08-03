// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__source_local_h__
#define __player__source_local_h__

// ----------------------------------------------------------------------------
#include "source.h"
#include "source_local_task.h"

// ----------------------------------------------------------------------------
#include <dripcore/loop.h>

// ----------------------------------------------------------------------------
namespace musciteer
{
  class source_local : public source
  {
    using source::source;
  public:
    void start() override
    {
      spawn<source_local_task>(session_ch_);
    }
  public:
    void restart() override
    {
    }
  };
}

// ----------------------------------------------------------------------------
#endif
