// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__spotify_handler_h__
#define __http_server__spotify_handler_h__

// ----------------------------------------------------------------------------
#include "api_handler_base.h"

// ----------------------------------------------------------------------------
#include "../spotify_web/http_client.h"
#include "../spotify_web/track_importer.h"

// ----------------------------------------------------------------------------
class spotify_handler : public api_handler_base
{
public:
  spotify_handler(http::request& request, http::response& response, dripcore::task* task)
    :
    api_handler_base(request, response),
    http_client_(task),
    code_re_("code=(.*)"),
    task_(task)
  {
  }
public:
  void call(const std::string& path, const std::string& query)
  {
    std::smatch match;

    if ( std::regex_match(path, match, route_re_) )
    {
      if ( match[0].length() == 0 || (match[0] == "/" && match[1].length() == 0 ))
      {
        method_not_allowed();
      }
      else if ( match[1] == "import" )
      {
        if ( request.method() == http::method::post )
        {
          post_spotify_import();
        }
        else
        {
          method_not_allowed();
        }
      }
      else if ( match[1] == "authorize" )
      {
        if ( request.method() == http::method::get )
        {
          get_spotify_auhtorize(query);
          redirect("/");
        }
        else
        {
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
protected:
  void post_spotify_import()
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
      throw std::runtime_error("spotify import must be an object");
    }

    if ( !j["url"].is_string() ) {
      throw std::runtime_error("spotify import url must be a string");
    }

    std::string url = j["url"];

    auto spotify_json = http_client_.get_json("https://api.spotify.com"+url);

    if ( spotify_json["type"] == "track" )
    {
      spotify_web::track_importer importer(http_client_);

      importer.import_track(spotify_json);
    }
    else if ( spotify_json["type"] == "album" )
    {
      spotify_web::track_importer importer(http_client_);

      for ( auto item : spotify_json["tracks"]["items"] )
      {
        std::string track_url = item["href"];
        importer.import_track(track_url);
      }
    }
    else
    {
      throw std::runtime_error("don't know how to import that");
    }

    ok();
  }
protected:
  void get_spotify_auhtorize(const std::string& query)
  {
    std::smatch match;

    if ( std::regex_search(query, match, code_re_) )
    {
      // TODO: Need some kind of spotify web api worker.
      std::cerr << "spotify_handler - authorize code=" << match[1] << std::endl;
    }
    else
    {
      std::cerr << "spotify_handler - failed to read authorization code" << std::endl;
    }
  }
protected:
  spotify_web::http_client http_client_;
protected:
  std::regex code_re_;
protected:
  dripcore::task* task_;
};

// ----------------------------------------------------------------------------
#endif