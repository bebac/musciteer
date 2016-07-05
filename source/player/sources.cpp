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
  sources::source_ptr sources::source_local_;
  sources::source_ptr sources::source_spotify_;

  void sources::play(std::shared_ptr<player_session> session)
  {
    auto track = session->track();
    assert(track);

    auto sources = track->sources();

    if ( sources.size() > 0 )
    {
      auto& source = sources[0];

      if ( source.name() == "local" ) {
        source_local_->send(std::move(session));
      }
      else if ( source.name() == "spotify" ) {
        source_spotify_->send(std::move(session));
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

  void sources::settings_changed(const std::string source_name)
  {
    if ( source_name == "spotify" )
    {
      source_spotify_->restart();
    }
  }

  void sources::start(dripcore::loop* loop)
  {
    std::cerr << "starting local source" << std::endl;
    source_local_.reset(new source_local(loop));
    source_local_->start();

    std::cout << "starting spotify source" << std::endl;
    source_spotify_.reset(new source_spotify(loop));
    source_spotify_->start();
  }
}