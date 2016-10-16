// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__tracks_handler_h__
#define __http_server__tracks_handler_h__

// ----------------------------------------------------------------------------
#include "api.h"
#include "api_handler_base.h"

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"
#include "../dm/albums.h"

// ----------------------------------------------------------------------------
class tracks_handler :public api_handler_base
{
public:
  tracks_handler(http::request& request, http::response& response, dripcore::task* task)
    :
    api_handler_base(request, response),
    task_(task)
  {
  }
public:
  void call(const std::string& path)
  {
    std::smatch match;

    if ( std::regex_match(path, match, route_re_) )
    {
      if ( match[0].length() == 0 || (match[0] == "/" && match[1].length() == 0 ))
      {
        if ( request.method() == http::method::get ) {
          get_tracks(path);
        }
        else {
          method_not_allowed();
        }
      }
      else if ( match[0].length() > 0 && match[1].length() > 0 )
      {
        if ( match[2].length() == 0 )
        {
          if ( request.method() == http::method::get ) {
            get_track(match[1]);
          }
          else if ( request.method() == http::method::delete_ ) {
            delete_track(match[1]);
          }
          else {
            method_not_allowed();
          }
        }
        else {
          not_found();
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
protected:
  void get_tracks(const std::string& path)
  {
    auto tracks = musciteer::dm::tracks();

    json j;

    tracks.each([&](musciteer::dm::track& track) {
      j.push_back(musciteer::to_json(track));
      task_->yield(true);
      return true;
    });

    ok(std::move(j));
  }
private:
  void get_track(const std::string& id)
  {
    auto tracks = musciteer::dm::tracks();
    auto track = tracks.find_by_id(id);

    if ( !track.id_is_null() )
    {
      ok(musciteer::to_json(track));
    }
    else
    {
      not_found();
    }
  }
private:
  void delete_track(const std::string& id)
  {
    if ( id.length() != 6 )
    {
      unprocessable_entity();
      return;
    }

    if ( !(id[0] == 't') )
    {
      unprocessable_entity();
      return;
    }

    auto tracks = musciteer::dm::tracks();
    auto albums = musciteer::dm::albums();

    auto track = tracks.find_by_id(id);

    if ( !track.id_is_null() )
    {
      auto album = track.album();

      album.tracks_del(track);
      tracks.remove(track);
      albums.save(album);

      ok();
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