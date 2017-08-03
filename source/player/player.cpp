// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include "player.h"

// ----------------------------------------------------------------------------
namespace musciteer
{
  /////
  // Instantiate player message channel.

  message_channel player::message_ch_ = message_channel();
}
