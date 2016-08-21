// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__player_handler_h__
#define __http_server__player_handler_h__

// ----------------------------------------------------------------------------
#include "api.h"

// ----------------------------------------------------------------------------
#include "../dm/player.h"
#include "../player/player.h"

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>

// ----------------------------------------------------------------------------
#include <regex>

// ----------------------------------------------------------------------------
class player_handler
{
public:
  player_handler(http::request& request, http::response& response, dripcore::task* task)
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
          get_player();
        }
        else {
          method_not_allowed();
        }
      }
      else if ( match[0].length() > 0 && match[1].length() > 0 )
      {
        if ( match[2].length() == 0 )
        {
          if ( match[1] == "output" )
          {
            if ( request.method() == http::method::get ) {
              get_player_output();
            }
            else if ( request.method() == http::method::post ) {
              post_player_output();
            }
            else {
              method_not_allowed();
            }
          }
          else if ( match[1] == "ctpb" )
          {
            if ( request.method() == http::method::get ) {
              get_player_ctpb();
            }
            else if ( request.method() == http::method::post ) {
              post_player_ctpb();
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
private:
  void get_player()
  {
    json j;

    j["output"] = get_output_json();
    j["ctpb"] = get_ctpb_json();

    auto payload = j.dump();

    response << "HTTP/1.1 200 OK" << crlf
      << "Content-Length: " << payload.length() << crlf
      << crlf
      << payload;
  }
private:
  void get_player_output()
  {
    json j = get_output_json();

    auto payload = j.dump();

    response << "HTTP/1.1 200 OK" << crlf
      << "Content-Length: " << payload.length() << crlf
      << crlf
      << payload;
  }
private:
  void post_player_output()
  {
    std::string content_type;
    std::string content_length_s;
    std::string content;

    if ( !request.get_header("content-type", content_type) ) {
      throw std::runtime_error("no content-type header");
    }

    if ( !request.get_header("content-length", content_length_s) ) {
      throw std::runtime_error("no content-length header");
    }

    auto pos = std::size_t{0};
    auto len = std::stoul(content_length_s, &pos);
    auto buf = request.rdbuf();

    for ( size_t i=0; i<len; ++i) {
      content += buf->sbumpc();
    }

    json j = json::parse(content);

    if ( !j.is_object() ) {
      throw std::runtime_error("player output must be an object");
    }

    musciteer::dm::player player_settings;

    if ( j.count("current") )
    {
      player_settings.audio_device(j["current"].get<std::string>());
      player_settings.save();

      auto player = musciteer::player();
      player.settings_changed();

      json j = get_output_json();

      auto payload = j.dump();

      response << "HTTP/1.1 200 OK" << crlf
        << "Content-Length: " << payload.length() << crlf
        << crlf
        << payload;
    }
    else
    {
      internal_error();
    }
  }
private:
  void get_player_ctpb()
  {
    json j = get_ctpb_json();

    auto payload = j.dump();

    response << "HTTP/1.1 200 OK" << crlf
      << "Content-Length: " << payload.length() << crlf
      << crlf
      << payload;
  }
private:
  void post_player_ctpb()
  {
    std::string content_type;
    std::string content_length_s;
    std::string content;

    if ( !request.get_header("content-type", content_type) ) {
      throw std::runtime_error("no content-type header");
    }

    if ( !request.get_header("content-length", content_length_s) ) {
      throw std::runtime_error("no content-length header");
    }

    auto pos = std::size_t{0};
    auto len = std::stoul(content_length_s, &pos);
    auto buf = request.rdbuf();

    for ( size_t i=0; i<len; ++i) {
      content += buf->sbumpc();
    }

    json j = json::parse(content);

    if ( !j.is_object() ) {
      throw std::runtime_error("player ctpb must be an object");
    }

    musciteer::dm::player player_settings;

    if ( j.count("enabled") ) {
      player_settings.ctpb_enabled(j["enabled"].get<bool>());
    }

    if ( j.count("type") ) {
      player_settings.ctpb_type(j["type"].get<std::string>());
    }

    player_settings.save();

    auto player = musciteer::player();
    player.settings_changed();

    json r = get_output_json();

    auto payload = r.dump();

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
  void internal_error()
  {
    response << "HTTP/1.1 500 Internal Error" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
private:
  json get_output_json()
  {
    auto player = musciteer::player();

    message_channel reply_ch;
    player.audio_device_list(reply_ch);
    auto m = reply_ch.recv(task_);

    json output = {
      { "current", m.device_list_res.current },
      { "devices", m.device_list_res.device_names }
    };

    return output;
  }
private:
  json get_ctpb_json()
  {
    musciteer::dm::player player_settings;

    json ctpb = {
      { "enabled", player_settings.ctpb_enabled() },
      { "type", player_settings.ctpb_type() }
    };

    return ctpb;
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