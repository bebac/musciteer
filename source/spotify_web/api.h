// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __spotify_web__api_h__
#define __spotify_web__api_h__

// ----------------------------------------------------------------------------
#include "socket.h"
#include "id.h"

// ----------------------------------------------------------------------------
#include "../dm/spotify_web_api.h"
#include "../dm/album_cover.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/streambuf.h>

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>
#include <http/base64.h>

// ----------------------------------------------------------------------------
#include <json.h>

// ----------------------------------------------------------------------------
namespace spotify_web
{
  class api
  {
  public:
    api(dripcore::task* task)
      :
      task_(task),
      api_data_(),
      socket_(task)
    {
    }
  public:
    bool is_authorized() const
    {
      auto access_token = api_data_.access_token();
      return !access_token.empty();
    }
  public:
    void authorize(const std::string& code, const std::string& state);
  public:
    void get(const std::string& path, json& body);
    void get(const std::string& path, musciteer::dm::album_cover& cover);
  public:
    json audio_features(const std::string& track_id);
  private:
    void refresh_access_token();
    void update_api_data(json data);
  private:
    dripcore::task* task_;
    musciteer::dm::spotify_web_api api_data_;
    socket socket_;
  };
}

// ----------------------------------------------------------------------------
#endif
