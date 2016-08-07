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

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"
#include "../dm/albums.h"

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>

// ----------------------------------------------------------------------------
#include <regex>

// ----------------------------------------------------------------------------
class tracks_handler
{
public:
  tracks_handler(http::request& request, http::response& response, dripcore::task* task)
    : request(request), response(response), task_(task), route_re_("^/?([^/]*)?/?([^/]*)?")
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
    auto tracks = musicbox::dm::tracks();

    json j;

    tracks.each([&](musicbox::dm::track& track) {
      j.push_back(musicbox::to_json(track));
      task_->yield(true);
      return true;
    });

    auto payload = j.dump();

    response << "HTTP/1.1 200 OK" << crlf
      << "Content-Length: " << payload.length() << crlf
      << crlf
      << payload;
  }
private:
  void get_track(const std::string& id)
  {
    auto tracks = musicbox::dm::tracks();
    auto track = tracks.find_by_id(id);

    if ( !track.id_is_null() )
    {
      json j = musicbox::to_json(track);

      auto payload = j.dump();

      response << "HTTP/1.1 200 OK" << crlf
        << "Content-Length: " << payload.length() << crlf
        << crlf
        << payload;
    }
    else
    {
      not_found();
    }
  }
private:
  void delete_track(const std::string& id)
  {
    auto tracks = musicbox::dm::tracks();
    auto albums = musicbox::dm::albums();

    auto track = tracks.find_by_id(id);

    if ( !track.id_is_null() )
    {
      auto album = track.album();

      album.tracks_del(track);
      tracks.remove(track);
      albums.save(album);

      response << "HTTP/1.1 200 OK" << crlf
        << "Content-Length: " << 0 << crlf
        << crlf;
    }
    else
    {
      not_found();
    }
  }
protected:
  void method_not_allowed()
  {
    response << "HTTP/1.1 405 Method Not Allowed" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  void not_found()
  {
    response << "HTTP/1.1 404 Not Found" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  http::request& request;
  http::response& response;
  dripcore::task* task_;
private:
  std::regex route_re_;
private:
  static constexpr const char* crlf = "\r\n";
};

// ----------------------------------------------------------------------------
#endif