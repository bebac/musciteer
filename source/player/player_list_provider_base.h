// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__player_list_provider_base_h__
#define __player__player_list_provider_base_h__

// ----------------------------------------------------------------------------
#include "../dm/track.h"

// ----------------------------------------------------------------------------
#include <string>

// ----------------------------------------------------------------------------
namespace musciteer
{
  class list_provider_base
  {
  public:
    virtual bool done() = 0;
  public:
    virtual dm::track next() = 0;
  public:
    virtual std::string info() = 0;
  };
}

// ----------------------------------------------------------------------------
#endif
