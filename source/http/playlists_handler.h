// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__playlists_handler_h__
#define __http_server__playlists_handler_h__

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"

// ----------------------------------------------------------------------------
class playlists_handler : public api_handler_base
{
public:
  playlists_handler(http::request_environment& env, dripcore::task* task)
    :
    api_handler_base(env),
    task_(task)
  {
  }
public:
  void call(const std::string& path)
  {
    auto method = env.method();

    std::smatch match;

    if ( std::regex_match(path, match, route_re_) )
    {
      if ( match[0].length() == 0 || (match[0] == "/" && match[1].length() == 0 ))
      {
        if ( method == http::method::get ) {
          get_playlists(path);
        }
        else {
          method_not_allowed();
        }
      }
      else
      {
        not_found();
      }
    }
    else
    {
      not_found();
    }
  }
private:
  void get_playlists(const std::string& path)
  {
    auto tracks = musciteer::dm::tracks();
    auto result = std::set<std::string>();

    tracks.each([&](musciteer::dm::track& track)
    {
      const auto& tags = track.tags();

      result.insert(tags.begin(), tags.end());

      task_->yield(true);
      return true;
    });

    json j = result;

    ok(j);
  }
protected:
  dripcore::task* task_;
};

// ----------------------------------------------------------------------------
#endif