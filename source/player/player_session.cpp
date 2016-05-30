// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "player_session.h"
#include "sources.h"
#include "audio_output.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  void player_session::play(std::shared_ptr<audio_output_alsa> audio_output)
  {
    auto sources = musicbox::sources();
    audio_output_ = audio_output;
    sources.play(shared_from_this());
  }
}