// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__albums_handler_h__
#define __http_server__albums_handler_h__

// ----------------------------------------------------------------------------
#include "api.h"

// ----------------------------------------------------------------------------
#include "../dm/albums.h"
#include "../dm/album_cover.h"

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>

// ----------------------------------------------------------------------------
#include <regex>

// ----------------------------------------------------------------------------
class albums_handler
{
public:
  albums_handler(http::request& request, http::response& response)
    : request(request), response(response), route_re_("^/?([^/]*)?/?([^/]*)?")
  {
  }
public:
  void call(const std::string& path)
  {
    if ( request.method() == http::method::get )
    {
      std::smatch match;

      if ( std::regex_match(path, match, route_re_) )
      {
        if ( match[0].length() == 0 ) {
          get_albums(path);
        }
        else if ( match[0] == "/" && match[1].length() == 0 ) {
          get_albums(path);
        }
        else if ( match[0].length() > 0 && match[1].length() > 0 )
        {
          if ( match[2].length() == 0 ) {
            get_album(match[1]);
          }
          else if ( match[2] == "tracks" ) {
            get_album_tracks(match[1]);
          }
          else if ( match[2] == "cover" ) {
            get_album_cover(match[1]);
          }
          else {
            not_found();
          }
        }
        else {
          not_found();
        }
      }
      else {
        not_found();
      }
    }
    else
    {
      method_not_allowed();
    }
  }
protected:
  void get_albums(const std::string& path)
  {
    auto albums = musicbox::dm::albums();

    json j;

    albums.each([&](musicbox::dm::album& album) {
      j.push_back(musicbox::to_json(album));
      return true;
    });

    auto payload = j.dump();

    response << "HTTP/1.1 200 OK" << crlf
      << "Content-Length: " << payload.length() << crlf
      << crlf
      << payload;
  }
private:
  void get_album(const std::string& id)
  {
    auto albums = musicbox::dm::albums();

    auto album = albums.find_by_id(id);

    if ( !album.id().empty() )
    {
      json j = musicbox::to_json(album);

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
  void get_album_tracks(const std::string& id)
  {
    auto albums = musicbox::dm::albums();

    auto album = albums.find_by_id(id);

    if ( !album.id().empty() )
    {
      json j;

      album.tracks_each([&](const musicbox::dm::track& track)
      {
        if ( !track.id_is_null() ) {
          j.push_back(musicbox::to_json(track));
        }
      });

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
  void get_album_cover(const std::string& id)
  {
    auto kvstore = musicbox::kvstore();

    musicbox::dm::album_cover cover;

    if ( kvstore.get(id+"/cover", cover) )
    {
      auto& data = cover.data();

      response << "HTTP/1.1 200 OK" << crlf
        << "Content-Type: " << cover.mime_type() << crlf
        << "Content-Length: " << data.length() << crlf
        << crlf
        << data;
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
private:
  std::regex route_re_;
private:
  static constexpr const char* crlf = "\r\n";
};

// ----------------------------------------------------------------------------
#endif