// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__http_connection_h__
#define __musciteer__http_connection_h__

// ----------------------------------------------------------------------------
#include "../player/player.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/socket.h>

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>
#include <http/handlers/websocket_handler_base.h>

// ----------------------------------------------------------------------------
#include <json.h>

// ----------------------------------------------------------------------------
class websocket_handler;

// ----------------------------------------------------------------------------
class websocket_send_task : public dripcore::task
{
public:
  websocket_send_task(websocket_handler& handler, message_channel ch)
    : handler_(handler), ch_(ch)
  {
  }
private:
  void main() override;
private:
  websocket_handler& handler_;
  message_channel ch_;
};

// ----------------------------------------------------------------------------
class websocket_handler : public http::websocket_handler_base
{
  using websocket_handler_base::websocket_handler_base;

  void on_connect() override
  {
    std::cout << "websocket connected" << std::endl;

    // Start send task.
    task_->spawn<websocket_send_task>(*this, message_ch_);

    musicbox::player().subscribe(message_ch_);
  }

  void on_close() override
  {
    std::cout << "websocket closed" << std::endl;

    musicbox::player().unsubscribe(message_ch_);

    message_ch_.send(message{}, task_);
  }

  void on_message(const std::string& message) override
  {
    auto j = json::parse(message);

    if ( j.count("event") )
    {
      if ( j["event"] == "audio_device_list_sync" )
      {
        musicbox::player().audio_device_list(message_ch_);
      }
      else if ( j["event"] == "audio_device" )
      {
        musicbox::player().audio_device(j["data"].get<std::string>());
      }
      else if ( j["event"] == "play" )
      {
        musicbox::player().play(j["data"].get<std::string>());
      }
    }

    std::cout << "websocket message=\"" << j << "\"" << std::endl;
  }
public:
  void set_task(dripcore::task* task)
  {
    task_ = task;
  }
private:
  message_channel message_ch_;
private:
  dripcore::task* task_;
};

// ----------------------------------------------------------------------------
class http_connection : public dripcore::task
{
public:
  http_connection(dripcore::socket socket);
  ~http_connection();
public:
  void main() override;
protected:
  void loop(std::streambuf* sbuf);
protected:
  void dispatch(http::request& request, http::response& response);
protected:
  void not_found(http::response& response);
private:
  dripcore::socket socket_;
private:
  static constexpr const char* crlf = "\r\n";
};

// ----------------------------------------------------------------------------
#endif