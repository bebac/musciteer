// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include "sources.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  session_channel sources::local_session_ch_ = session_channel();
  session_channel sources::spotify_session_ch_ = session_channel();
}