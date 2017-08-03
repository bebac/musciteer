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
#include "../dm/source_spotify.h"

// ----------------------------------------------------------------------------
#include <dripcore/loop.h>

// ----------------------------------------------------------------------------
namespace musciteer
{
  enum class source_control
  {
    start,
    shutdown,
    restart
  };

  using control_channel = dripcore::channel<source_control>;

  class source_spotify_ctrl_task : public dripcore::task
  {
  public:
    source_spotify_ctrl_task(control_channel ctrl_ch, session_channel session_ch, done_channel done_ch)
      : ctrl_ch_(ctrl_ch), session_ch_(session_ch), done_ch_(done_ch)
    {
    }
  private:
    void main() final
    {
      auto task = std::weak_ptr<dripcore::task>();

      while ( true )
      {
        auto m = ctrl_ch_.recv(this);

        switch ( m )
        {
          case source_control::start:
          {
            musciteer::dm::source_spotify spotify_settings{};

            auto& username = spotify_settings.username();
            auto& password = spotify_settings.password();

            if ( !username.empty() && !password.empty() )
            {
              task = spawn<source_spotify_task>(session_ch_, done_ch_);
            }
            else
            {
              std::cout << "spotify source not configured" << std::endl;
            }
            break;
          }
          case source_control::shutdown:
            std::cout << "source_spotify_ctrl_task shutdown" << std::endl;
            break;
          case source_control::restart:
          {
            if ( !task.expired() )
            {
              std::cout << "spotify source stop" << std::endl;
              session_ch_.send(nullptr, this);
              done_ch_.recv(this);
              std::cout << "spotify source stopped" << std::endl;
            }
            ctrl_ch_.send(source_control::start);
            break;
          }
        }
      }
    }

  private:
    control_channel ctrl_ch_;
    session_channel session_ch_;
    done_channel done_ch_;
  };

  class source_spotify : public source
  {
    using source::source;
  public:
    void start() override
    {
      source::spawn<source_spotify_ctrl_task>(ctrl_ch_, session_ch_, done_ch_);
      ctrl_ch_.send(source_control::start);
    }
  public:
    void restart() override
    {
      ctrl_ch_.send(source_control::restart);
    }
  private:
    control_channel ctrl_ch_;
  };
}

// ----------------------------------------------------------------------------
#endif
