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
#include "api_handler_base.h"

// ----------------------------------------------------------------------------
#include "../dm/player.h"
#include "../player/player.h"

// ----------------------------------------------------------------------------
class player_handler : public api_handler_base
{
public:
  player_handler(http::request_environment& env, dripcore::task* task)
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
            if ( method == http::method::get ) {
              get_player_output();
            }
            else if ( method == http::method::post ) {
              post_player_output();
            }
            else {
              method_not_allowed();
            }
          }
          else if ( match[1] == "ctpb" )
          {
            if ( method == http::method::get ) {
              get_player_ctpb();
            }
            else if ( method == http::method::post ) {
              post_player_ctpb();
            }
            else {
              method_not_allowed();
            }
          }
          else if ( match[1] == "replaygain" )
          {
            if ( method == http::method::get ) {
              get_player_replaygain();
            }
            else if ( method == http::method::post ) {
              post_player_replaygain();
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

    ok(j);
  }
private:
  void get_player_output()
  {
    json j = get_output_json();

    ok(j);
  }
private:
  void post_player_output()
  {
    auto j = read_json_content(env);

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

      ok(j);
    }
    else
    {
      internal_error();
    }
  }
private:
  void get_player_ctpb()
  {
    ok(get_ctpb_json());
  }
private:
  void post_player_ctpb()
  {
    auto j = read_json_content(env);

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

    ok(get_ctpb_json());
  }
private:
  void get_player_replaygain()
  {
    ok(get_replaygain_json());
  }
private:
  void post_player_replaygain()
  {
    auto j = read_json_content(env);

    if ( !j.is_object() ) {
      throw std::runtime_error("player replaygain must be an object");
    }

    musciteer::dm::player player_settings;

    if ( j.count("enabled") ) {
      player_settings.replaygain_enabled(j["enabled"].get<bool>());
    }

    player_settings.save();

    auto player = musciteer::player();
    player.settings_changed();

    ok(get_replaygain_json());
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
private:
  json get_replaygain_json()
  {
    musciteer::dm::player player_settings;

    json replaygain = {
      { "enabled", player_settings.replaygain_enabled() }
    };

    return replaygain;
  }
private:
  json read_json_content(http::request_environment& env)
  {
    auto content_type = std::string{};
    auto content_length = size_t{0};
    auto content = std::string{};

    if ( !env.get_header("Content-Type", content_type) ) {
      throw std::runtime_error("no content-type header");
    }

    if ( !env.get_content_length(content_length) ) {
      throw std::runtime_error("no content-length header");
    }

    for ( size_t i=0; i<content_length; ++i ) {
      content += env.is.get();
    }

    return json::parse(content);
  }
protected:
  dripcore::task* task_;
};

// ----------------------------------------------------------------------------
#endif