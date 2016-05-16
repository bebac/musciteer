// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__track_handler_h__
#define __http_server__track_handler_h__

// ----------------------------------------------------------------------------
#include "api.h"

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>

// ----------------------------------------------------------------------------
#include <streambuf>
#include <fstream>

// ----------------------------------------------------------------------------
class track_handler
{
public:
  track_handler(http::request& request, http::response& response)
    : request(request), response(response)
  {
  }
public:
  void call(const std::string& path)
  {
    if ( request.method() == http::method::get )
    {
      if ( path.length() == 0 || path == "/" )
      {
        get_all_tracks(path);
      }
    }
    else
    {
      method_not_allowed();
    }
  }
protected:
  void get_all_tracks(const std::string& path)
  {
    auto tracks = musicbox::tracks();

    json j;

    tracks.each([&](musicbox::track& track) {
      j.push_back(to_json(track));
      return true;
    });

    auto payload = j.dump();

    response << "HTTP/1.1 200 OK" << crlf
      << "Content-Length: " << payload.length() << crlf
      << crlf
      << payload;
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
  static constexpr const char* crlf = "\r\n";
};

// ----------------------------------------------------------------------------
#endif