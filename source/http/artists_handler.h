// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__artists_handler_h__
#define __http_server__artists_handler_h__

// ----------------------------------------------------------------------------
#include "../dm/artists.h"

// ----------------------------------------------------------------------------
class artists_handler : public api_handler_base
{
public:
  artists_handler(http::request_environment& env, dripcore::task* task)
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
          get_artists(path);
        }
        else {
          method_not_allowed();
        }
      }
      else if ( match[0].length() > 0 && match[1].length() > 0 )
      {
        if ( match[2].length() == 0 )
        {
          if ( method == http::method::get ) {
            get_artist(match[1]);
          }
          else {
            method_not_allowed();
          }
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
  void get_artists(const std::string& path)
  {
    auto artists = musciteer::dm::artists();

    json j;

    artists.each([&](const musciteer::dm::artist& artist) {
      j.push_back(musciteer::to_json(artist));
      task_->yield(true);
      return true;
    });

    ok(std::move(j));
  }
private:
  void get_artist(const std::string& id)
  {
    auto artists = musciteer::dm::artists();
    auto artist = artists.find_by_id(id);

    if ( !artist.id_is_null() )
    {
      ok(musciteer::to_json(artist));
    }
    else
    {
      not_found();
    }
  }

protected:
  dripcore::task* task_;
};

// ----------------------------------------------------------------------------
#endif