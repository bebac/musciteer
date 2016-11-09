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
#include "../spotify_web/id.h"

// ----------------------------------------------------------------------------
#include "../dm/spotify_web_api.h"

// ----------------------------------------------------------------------------
#include <http/base64.h>

// ----------------------------------------------------------------------------
class spotify_handler : public api_handler_base
{
public:
  spotify_handler(http::request_environment& env, dripcore::task* task)
    :
    api_handler_base(env),
    http_client_(task),
    host_re_("host=([^&]*)"),
    code_re_("code=([^&]*)"),
    state_re_("state=([^&]*)"),
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
        method_not_allowed();
      }
      else if ( match[1] == "import" )
      {
        if ( method == http::method::post )
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
        if ( method == http::method::get )
        {
          get_spotify_auhtorize(query);
        }
        else
        {
          method_not_allowed();
        }
      }
      else if ( match[1] == "callback" )
      {
        if ( method == http::method::get )
        {
          get_spotify_callback(query);
          redirect("/");
        }
        else
        {
          method_not_allowed();
        }
      }
      else if ( match[1] == "settings" )
      {
        if ( method == http::method::get )
        {
          get_spotify_settings(query);
        }
        else
        {
          method_not_allowed();
        }
      }
      else if ( match[1] == "revoke" )
      {
        if ( method == http::method::post )
        {
          post_spotify_revoke(query);
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

    if ( !env.get_header("content-type", content_type) ) {
      throw std::runtime_error("no content-type header");
    }

    if ( !env.get_header("content-length", content_length_s) ) {
      throw std::runtime_error("no content-length header");
    }

    auto pos = std::size_t{0};
    auto len = std::stoul(content_length_s, &pos);

    for ( size_t i=0; i<len; ++i) {
      content += env.is.get();
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

    if ( std::regex_search(query, match, host_re_) )
    {
      std::string host = match[1];

      std::stringstream url;
      std::stringstream redirect_uri;

      std::cerr << "spotify_handler - authorize host=" << host << std::endl;

      redirect_uri << "http%3A%2F%2F" << host << "%2Fapi%2Fspotify%2fcallback";

      url
        << "https://accounts.spotify.com/authorize"
        << "?client_id=" << spotify_web::client_id
        << "&response_type=code"
        << "&redirect_uri=" << redirect_uri.str()
        << "&scope=user-read-private%20user-read-email"
        << "&state=" << redirect_uri.str();

      redirect(url.str());
    }
    else
    {
      std::cerr << "spotify_handler - failed to read host" << std::endl;
      internal_error();
    }
  }
protected:
  void get_spotify_callback(const std::string& query)
  {
    std::smatch match;

    std::string code;
    std::string state;

    if ( std::regex_search(query, match, code_re_) ) {
      code = match[1];
    }

    if ( std::regex_search(query, match, state_re_)) {
      state = match[1];
    }

    if ( !code.empty() && !state.empty() )
    {
      std::string client_code = spotify_web::client_id + ":" + spotify_web::client_secret;

      std::stringstream pos;

      pos
        << "grant_type=authorization_code"
        << "&code=" << code
        << "&redirect_uri=" << state;

      auto params = pos.str();

      http::request req;

      req.method(http::method::post);
      req.uri("/api/token");
      req.port(443);
      req.set_header("Host", "accounts.spotify.com");
      req.set_header("Content-Type", "application/x-www-form-urlencoded");
      req.set_header("Content-Length", std::to_string(params.length()));
      req.set_header("Authorization", "Basic " + http::base64::encode(client_code.data(), client_code.length()));

      http_client_.get(
        req,
        [&](std::ostream& os)
        {
          os << params;
        },
        [&](http::response_environment& response)
        {
          std::string content_length_s;
          std::string content;

          if ( response.status_code() != 200 )
          {
            std::cerr << "spotify_handler - failed to authorize " << response.status_message() << std::endl;
            return;
          }

          if ( response.get_header("content-length", content_length_s) )
          {
            auto pos = std::size_t{0};
            auto len = std::stoul(content_length_s, &pos);

            for ( size_t i=0; i<len; ++i) {
              content += response.is.get();
            }

            json j = json::parse(content);

            if ( !j.is_object() ) {
              throw std::runtime_error("spotify handler - authorization must be an object");
            }

            musciteer::dm::spotify_web_api api{};

            if ( j["access_token"].is_string() ) {
              api.access_token(j["access_token"]);
            }

            if ( j["refresh_token"].is_string() ) {
              api.refresh_token(j["refresh_token"]);
            }

            if ( j["expires_in"].is_number() ) {
              api.expires_at(j["expires_in"]);
            }

            api.save();
#if 0
            std::time_t t = std::chrono::system_clock::to_time_t(api_data.expires_at());

            std::cerr
              << "spotify_handler - data" << std::endl
              << "  access_token  : " << api_data.access_token() << std::endl
              << "  refresh_token : " << api_data.refresh_token() << std::endl
              << "  expire_at     : " << std::put_time(std::localtime(&t), "%F %T") << std::endl;
#endif
          }
        },
        true
      );
    }
    else
    {
      std::cerr << "spotify_handler - failed to read authorization code" << std::endl;
    }
  }
protected:
  void get_spotify_settings(const std::string& query)
  {
    ok(spotify_web_api_data_as_json());
  }
protected:
  void post_spotify_revoke(const std::string& query)
  {
    musciteer::dm::spotify_web_api::remove();
    ok(spotify_web_api_data_as_json());
  }
protected:
  json spotify_web_api_data_as_json()
  {
    musciteer::dm::spotify_web_api api{};

    auto access_token = api.access_token();
    auto j = json::object();

    j["authorized"] = !access_token.empty();

    return j;
  }
protected:
  spotify_web::http_client http_client_;
protected:
  std::regex host_re_;
  std::regex code_re_;
  std::regex state_re_;
protected:
  dripcore::task* task_;
};

// ----------------------------------------------------------------------------
#endif