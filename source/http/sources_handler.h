// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__sources_handler_h__
#define __http_server__sources_handler_h__

// ----------------------------------------------------------------------------
#include "api.h"
#include "api_handler_base.h"

// ----------------------------------------------------------------------------
#include "../dm/source_local.h"
#include "../dm/source_spotify.h"

// ----------------------------------------------------------------------------
#include "../file_system.h"
#include "../flac_file_importer.h"

// ----------------------------------------------------------------------------
#include "../player/sources.h"

// ----------------------------------------------------------------------------
class sources_handler : public api_handler_base
{
public:
  sources_handler(http::request& request, http::response& response, dripcore::task* task)
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
      if ( match[0].length() == 0 ) {
        // TODO
        method_not_allowed();
      }
      else if ( match[0] == "/" && match[1].length() == 0 ) {
        // TODO
        method_not_allowed();
      }
      else if ( match[0].length() > 0 && match[1].length() > 0 )
      {
        if ( match[2].length() == 0 ) {
          // TODO
          method_not_allowed();
        }
        else if ( match[1] == "local" && match[2] == "directories" )
        {
          if ( request.method() == http::method::get )
          {
            get_sources_local_directories();
          }
          else if ( request.method() == http::method::post )
          {
            post_sources_local_directories();
          }
          else
          {
            method_not_allowed();
          }
        }
        else if ( match[1] == "local" && match[2] == "scan" )
        {
          if ( request.method() == http::method::post )
          {
            post_sources_local_scan();
          }
          else
          {
            method_not_allowed();
          }
        }
        else if ( match[1] == "spotify" && match[2] == "settings" )
        {
          if ( request.method() == http::method::get )
          {
            get_sources_spotify_settings();
          }
          else if ( request.method() == http::method::post )
          {
            post_sources_spotify_settings();
          }
          else
          {
            method_not_allowed();
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
    else {
      not_found();
    }
  }
protected:
  void get_sources_local_directories()
  {
    musciteer::dm::source_local source_local{};

    json j;

    j = source_local.directories();

    ok(std::move(j));
  }
protected:
  void post_sources_local_directories()
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

    if ( !j.is_array() ) {
      throw std::runtime_error("source local directories must be an array");
    }

    musciteer::dm::source_local source_local{};

    source_local.directories(j.get<std::vector<std::string>>());

    json res = source_local.directories();

    ok(std::move(res));
  }
protected:
  void post_sources_local_scan()
  {
    musciteer::dm::source_local source_local{};

    for ( auto& dirname : source_local.directories() )
    {
      auto dir = file_system::directory{dirname};

      dir.each_file([&](const std::string& filename)
      {
        if ( file_system::extension(filename) == "flac" )
        {
          std::cout << filename << std::endl;
          musciteer::flac_file_importer importer(filename);

          importer.run();

          task_->yield(true);
        }
      });
    }

    ok();
  }
protected:
  void get_sources_spotify_settings()
  {
    musciteer::dm::source_spotify settings{};

    json j;

    j["username"] = settings.username();
    // Maybe we should'nt send the password.
    //j["password"] = settings.password();

    ok(std::move(j));
  }
protected:
  void post_sources_spotify_settings()
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
      throw std::runtime_error("source spotify settings must be an object");
    }

    musciteer::dm::source_spotify settings{};

    if ( j["username"].is_string() ) {
      settings.username(j["username"]);
    }

    if ( j["password"].is_string() ) {
      settings.password(j["password"]);
    }

    settings.save();

    auto sources = musciteer::sources();
    sources.settings_changed("spotify");

    ok();
  }
protected:
  dripcore::task* task_;
};

// ----------------------------------------------------------------------------
#endif