// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include "api.h"

// ----------------------------------------------------------------------------
#include <regex>

// ----------------------------------------------------------------------------
namespace spotify_web
{
  namespace
  {
    const std::regex url_re{"^(http|https)://([^:/]*)(:?([^/]+)?(.*))"};

    constexpr const char* host_accounts_spotify = "accounts.spotify.com";
    constexpr const char* host_api_spotify = "api.spotify.com";
  }

  void api::authorize(const std::string& code, const std::string& state)
  {
    socket_.connect(host_accounts_spotify);

    dripcore::streambuf sbuf(socket_, *task_);

    http::request_environment env(&sbuf);
    http::response res;

    std::string client_code = spotify_web::client_id + ":" + spotify_web::client_secret;

    std::stringstream pos;

    pos
      << "grant_type=authorization_code"
      << "&code=" << code
      << "&redirect_uri=" << state;

    auto params = pos.str();

    env.method(http::method::post);
    env.uri("/api/token");
    env.set_header("Host", host_accounts_spotify);
    env.set_header("Content-Type", "application/x-www-form-urlencoded");
    env.set_header("Content-Length", std::to_string(params.length()));
    env.set_header("Authorization", "Basic " + http::base64::encode(client_code.data(), client_code.length()));

    env.os << env << params << std::flush;
    env.is >> res;

    if ( res.status_code() != 200 )
    {
      std::cerr << "spotify_web_api - failed to get access token " << res.status_message() << std::endl;
      return;
    }

    auto content_length = size_t{0};
    auto content = std::string{};

    if ( res.get_content_length(content_length) )
    {
      for ( size_t i=0; i<content_length; ++i) {
        content += env.is.get();
      }

      json j = json::parse(content);

      update_api_data(j);
    }
    else
    {
      std::cerr << "spotify_web_api - access token response" << std::endl;
    }
  }

  void api::get(const std::string& path, json& body)
  {
    socket_.connect(host_api_spotify);

    dripcore::streambuf sbuf(socket_, *task_);

    http::request_environment env(&sbuf);
    http::response res;

    env.method(http::method::get);
    env.uri(path);

    //
    // Set auth header if we have an access token, otherwise assume path
    // doesn't require authorization.
    //

    if ( is_authorized() )
    {
      if ( api_data_.expires_at() <= musciteer::dm::clock::now() ) {
        refresh_access_token();
      }
      env.set_header("Authorization", "Bearer " + api_data_.access_token());
    }

    env.set_header("Host", host_api_spotify);

    env.os << env << std::flush;
    env.is >> res;

    if ( res.status_code() == 200 )
    {
      auto content_type = std::string{};
      auto content_length = size_t{0};
      auto content = std::string{};
      auto accept = std::string{"application/json"};

      if ( !res.get_header("Content-Type", content_type) ) {
        throw std::runtime_error("no content-type header");
      }

      if ( content_type.compare(0, accept.size(), accept) != 0 ) {
        throw std::runtime_error("wrong content type");
      }

      if ( !res.get_content_length(content_length) ) {
        throw std::runtime_error("spotify_web - get json has no content-length header");
      }

      for ( size_t i=0; i<content_length; ++i ) {
        content += env.is.get();
      }

      body = json::parse(content);
    }
    else
    {
      std::cerr << "spotify_web get error " << res.status_code() << " " << res.status_message() << std::endl;
    }
  }

  void api::get(const std::string& url, musciteer::dm::album_cover& cover)
  {
    auto tls    = false;
    auto scheme = std::string{};
    auto host   = std::string{};
    auto port   = std::string{};
    auto path   = std::string{};

    std::smatch match;

    if ( std::regex_match(url, match, url_re) )
    {
      scheme = match[1];
      host   = match[2];
      port   = match[4];
      path   = match[5];

      if ( scheme == "https" ) {
        tls = true;
      }

      if ( port.empty() && !tls ) {
        throw std::runtime_error("only https suppooted for now");
      }
      else if ( port.empty() && tls ) {
        port = "443";
      }
    }
    else
    {
      throw std::runtime_error("invalid url");
    }

    socket_.connect(host);

    dripcore::streambuf sbuf(socket_, *task_);

    http::request_environment env(&sbuf);
    http::response res;

    env.method(http::method::get);
    env.uri(path);

    //
    // Set auth header if we have an access token, otherwise assume path
    // doesn't require authorization.
    //

    if ( is_authorized() )
    {
      if ( api_data_.expires_at() <= musciteer::dm::clock::now() ) {
        refresh_access_token();
      }
      env.set_header("Authorization", "Bearer " + api_data_.access_token());
    }

    env.set_header("Host", host);

    env.os << env << std::flush;
    env.is >> res;

    if ( res.status_code() == 200 )
    {
      auto content_type = std::string{};
      auto content_length = size_t{0};
      auto content = std::string{};

      if ( !res.get_header("Content-Type", content_type) ) {
        throw std::runtime_error("no content-type header");
      }

      if ( !res.get_content_length(content_length) ) {
        throw std::runtime_error("spotify_web - get album cover response has no content-length header");
      }

      for ( size_t i=0; i<content_length; ++i ) {
        content += env.is.get();
      }

      cover.mime_type(content_type);
      cover.data(reinterpret_cast<const unsigned char*>(content.data()), content.length());
    }
    else
    {
      std::cerr << "spotify_web - failed to get album cover " << res.status_code() << " " << res.status_message() << std::endl;
    }
  }

  json api::audio_features(const std::string& track_id)
  {
    json result;

    if ( !is_authorized() ) {
      throw std::runtime_error("spotify_web - audio-features require authorization");
    }

    get("/v1/audio-features/" + track_id, result);

    return result;
  }

  void api::refresh_access_token()
  {
    socket_.connect(host_accounts_spotify);

    dripcore::streambuf sbuf(socket_, *task_);

    http::request_environment env(&sbuf);
    http::response res;

    auto client_code = spotify_web::client_id + ":" + spotify_web::client_secret;

    std::stringstream pos;

    pos
      << "grant_type=refresh_token"
      << "&refresh_token=" << api_data_.refresh_token();

    auto params = pos.str();

    env.method(http::method::post);
    env.uri("/api/token");
    env.set_header("Host", host_accounts_spotify);
    env.set_header("Content-Type", "application/x-www-form-urlencoded");
    env.set_header("Content-Length", std::to_string(params.length()));
    env.set_header("Authorization", "Basic " + http::base64::encode(client_code.data(), client_code.length()));

    env.os << env << params << std::flush;
    env.is >> res;

    if ( res.status_code() != 200 )
    {
      std::cerr << "spotify_web_api - failed to refresh access token " << res.status_message() << std::endl;
      return;
    }

    auto content_length = size_t{0};
    auto content = std::string{};

    if ( res.get_content_length(content_length) )
    {
      for ( size_t i=0; i<content_length; ++i) {
        content += env.is.get();
      }

      json j = json::parse(content);

      update_api_data(j);
    }
    else
    {
      std::cerr << "spotify_web_api - invalid refresh token response" << std::endl;
    }
  }

  void api::update_api_data(json data)
  {
    if ( !data.is_object() ) {
      throw std::runtime_error("spotify_web_api - data must be an object");
    }

    if ( data["access_token"].is_string() ) {
      api_data_.access_token(data["access_token"]);
    }

    if ( data["refresh_token"].is_string() ) {
      api_data_.refresh_token(data["refresh_token"]);
    }

    if ( data["expires_in"].is_number() ) {
      api_data_.expires_at(data["expires_in"]);
    }

    api_data_.save();
  }
}
