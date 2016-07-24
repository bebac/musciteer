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
    auto player = musicbox::player();

    start_send_task();

    player.subscribe(message_ch_);
  }

  void on_close() override
  {
    auto player = musicbox::player();

    player.unsubscribe(message_ch_);

    stop_send_task();
  }

  void on_message(const std::string& message) override
  {
    auto j = json::parse(message);

    if ( j.count("event") )
    {
      auto player = musicbox::player();
      auto event = j["event"];

      if ( event == "stream_data_sync" )
      {
        player.stream_data(j["data"].get<unsigned>(), message_ch_);
      }
      else if ( event == "play" )
      {
        auto data = j["data"];

        if ( data.is_null() ) {
          player.play();
        }
        else {
          player.play(data.get<std::string>());
        }
      }
      else if ( event == "stop" )
      {
        player.stop();
      }
      else if ( event == "skip" )
      {
        player.skip();
      }
      else if ( event == "queue" )
      {
        player.queue(j["data"].get<std::string>());
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
  void start_send_task()
  {
    task_->spawn<websocket_send_task>(*this, message_ch_);
  }
private:
  void stop_send_task()
  {
    message_ch_.send(message{}, task_);
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