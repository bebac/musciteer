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
  tracks_handler(http::request_environment& env, dripcore::task* task)
    :
    api_handler_base(env),
    brief_re_("brief=([^&]*)"),
    task_(task)
  {
  }
public:
  void call(const std::string& path, const std::string& query)
  {
    auto method = env.method();

    std::smatch match;

    if ( std::regex_match(path, match, route_re_) )
    {
      if ( match[0].length() == 0 || (match[0] == "/" && match[1].length() == 0 ))
      {
        if ( method == http::method::get ) {
          get_tracks(path, query);
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
            get_track(match[1]);
          }
          else if ( method == http::method::delete_ ) {
            delete_track(match[1]);
          }
          else {
            method_not_allowed();
          }
        }
        else if ( match[2] == "sources" )
        {
          if ( match[3].length() == 0 )
          {
            if ( method == http::method::get ) {
              get_track_sources(match[1]);
            }
            else {
              method_not_allowed();
            }
          }
          else
          {
            if ( method == http::method::get ) {
              get_track_source(match[1], match[3]);
            }
            else if ( method == http::method::delete_ ) {
              delete_track_source(match[1], match[3]);
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
    else
    {
      not_found();
    }
  }
protected:
  void get_tracks(const std::string& path, const std::string& query)
  {
    auto tracks = musciteer::dm::tracks();
    auto brief = false;

    std::smatch match;

    if ( std::regex_search(query, match, brief_re_) ) {
      brief = true;
    }

    json j;

    tracks.each([&](musciteer::dm::track& track) {
      j.push_back(musciteer::to_json(track, brief));
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
private:
  void get_track_sources(const std::string& id)
  {
    auto tracks = musciteer::dm::tracks();
    auto track = tracks.find_by_id(id);

    if ( !track.id_is_null() )
    {
      json sources;

      track.sources_each([&](const musciteer::dm::track_source& source) {
        sources.push_back(musciteer::to_json(source));
      });

      ok(sources);
    }
    else
    {
      not_found();
    }
  }
private:
  void get_track_source(const std::string& id, const std::string& source_name)
  {
    auto tracks = musciteer::dm::tracks();
    auto track = tracks.find_by_id(id);

    if ( !track.id_is_null() )
    {
      track.sources_each([&](const musciteer::dm::track_source& source) {
        if ( source.name() == source_name )
        {
          ok(musciteer::to_json(source));
          return;
        }
      });

      not_found();
    }
    else
    {
      not_found();
    }
  }
private:
  void delete_track_source(const std::string& id, const std::string& source_name)
  {
    auto tracks = musciteer::dm::tracks();
    auto track = tracks.find_by_id(id);

    if ( !track.id_is_null() )
    {
      track.sources_remove(source_name);
      tracks.save(track);
      ok();
    }
    else
    {
      not_found();
    }
  }
protected:
  std::regex brief_re_;
protected:
  dripcore::task* task_;
};

// ----------------------------------------------------------------------------
#endif