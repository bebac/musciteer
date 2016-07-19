// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include "http_connection.h"
#include "tracks_handler.h"
#include "albums_handler.h"
#include "player_handler.h"
#include "sources_handler.h"
#include "static_file_handler.h"
#include "api.h"

// ----------------------------------------------------------------------------
#include <dripcore/socket.h>
#include <dripcore/streambuf.h>

// ----------------------------------------------------------------------------
#include <regex>

// ----------------------------------------------------------------------------
void websocket_send_task::main()
{
  std::cout << "websocket_send_task started" << std::endl;
  while ( true )
  {
    auto msg = ch_.recv(this);

    if ( msg.type == 0 ) {
      break;
    }

    switch ( msg.type )
    {
      case message::device_list_res_id:
      {
        json event = {
          { "event", "audio_device_list"},
          { "data",  { msg.device_list_res.current, msg.device_list_res.device_names } }
        };

        handler_.send_message(event.dump());
        break;
      }
      case message::stream_begin_notify_id:
      {
        json event = {
          { "event", "stream_begin"},
          { "data",  {
            { "stream_id", msg.stream_begin_notify.stream_id } }
          }
        };

        handler_.send_message(event.dump());
        break;
      }
      case message::stream_end_notify_id:
      {
        json event = {
          { "event", "stream_end"},
          { "data",  {
            { "stream_id", msg.stream_begin_notify.stream_id } }
          }
        };

        handler_.send_message(event.dump());
        break;
      }
      case message::stream_progress_notify_id:
      {
        json event = {
          { "event", "stream_progress"},
          { "data", {
            { "stream_id", msg.stream_progress_notify.stream_id },
            { "duration", msg.stream_progress_notify.duration },
            { "length", msg.stream_progress_notify.length } }
          }
        };

        handler_.send_message(event.dump());
        break;
      }
      case message::queue_update_id:
      {
        json event = {
          { "event", "queue_update"},
          { "data",  {
            { "queue_size", msg.queue_update.queue_size },
            { "track", musicbox::to_json(*msg.queue_update.track) } }
          }
        };
        handler_.send_message(event.dump());
        break;
      }
      case message::player_state_id:
      {
        json event = {
          { "event", "player_state"},
          { "data",  {
            { "state", msg.player_state.state } }
          }
        };
        handler_.send_message(event.dump());
        break;
      }
      case message::source_notify_id:
      {
        json event = {
          { "event", "source_notification"},
          { "data",  {
            { "type", unsigned(msg.source_notify.type) },
            { "source_name", msg.source_notify.source_name },
            { "message", msg.source_notify.message } }
          }
        };
        handler_.send_message(event.dump());
        break;
      }
      case message::stream_data_res_id:
      {
        json data;

        if ( msg.stream_data_res.stream_id != -1 )
        {
          data = {
            { "stream_id", msg.stream_data_res.stream_id },
            { "track", musicbox::to_json(*msg.stream_data_res.track) }
          };
        }

        json event = {
          { "event", "stream_data" }, { "data", data }
        };

        handler_.send_message(event.dump());
        break;
      }
    }
  }
  std::cout << "websocket_send_task stopped" << std::endl;
}

// ----------------------------------------------------------------------------
http_connection::http_connection(dripcore::socket socket)
  : socket_(std::move(socket))
{
  std::cout << "connection " << size_t(this) << std::endl;
}

// ----------------------------------------------------------------------------
http_connection::~http_connection()
{
  std::cout << "~connection " << size_t(this) << std::endl;
}

// ----------------------------------------------------------------------------
void http_connection::main()
{
  std::unique_ptr<dripcore::streambuf> sbuf(new dripcore::streambuf(socket_, *this));

  try
  {
    loop(sbuf.get());
  }
  catch(const dripcore::end_of_stream& e)
  {
    std::cout << "client disconnected" << std::endl;
  }
  catch(const std::ios_base::failure &e)
  {
    //std::cout << "connection " << size_t(this) << " streaming error code=" << e.code() << ", message=" << e.what() << std::endl;
  }
  catch(const std::exception& e)
  {
    std::cout << "connection error " << e.what() << std::endl;
  }
}

// ----------------------------------------------------------------------------
void http_connection::loop(std::streambuf* sbuf)
{
  while ( true )
  {
    http::request request(sbuf);
    http::response response(sbuf);

    if ( request >> request )
    {
      std::string upgrade;

      if ( request.get_header("Upgrade", upgrade) )
      {
        if ( upgrade == "websocket" )
        {
          //std::cout << "connection " << size_t(this) << " upgrade to websocket" << std::endl;
          websocket_handler handler(request, response);
          handler.set_task(this);
          handler.call(request.uri());
          return;
        }
        else
        {
          // Error!
        }
      }
      else
      {
        dispatch(request, response);
      }
    }
    else
    {
      std::cout << "failed to read request!" << std::endl;
      break;
    }
  }
}

// ----------------------------------------------------------------------------
void http_connection::dispatch(http::request& request, http::response& response)
{
  auto uri = request.uri();

  std::regex track_re("^/api/tracks(/.*)?");
  std::regex albums_re("^/api/albums(/.*)?");
  std::regex player_re("^/api/player(/.*)?");
  std::regex sources_re("^/api/sources(/.*)?");
  std::regex assets_re("^/(assets/.+)");

  std::smatch match;

  //std::cout << "dispatch uri " << uri << std::endl;

  if ( uri == "/" || uri == "/albums" || uri == "/tracks" )
  {
    static_file_handler handler(request, response);
    handler.call("index.html");
  }
  else if ( uri == "/android")
  {
    static_file_handler handler(request, response);
    handler.call("android.html");
  }
  else if ( std::regex_match(uri, match, track_re) )
  {
    track_handler handler(request, response);

    if ( match.size() == 2 )
    {
      handler.call(match[1]);
    }
    else
    {
      // ERROR!
    }
  }
  else if ( std::regex_match(uri, match, albums_re) )
  {
    albums_handler handler(request, response);

    if ( match.size() == 2 )
    {
      handler.call(match[1]);
    }
    else
    {
      // ERROR!
    }
  }
  else if ( std::regex_match(uri, match, player_re) )
  {
    player_handler handler(request, response, this);

    if ( match.size() == 2 )
    {
      handler.call(match[1]);
    }
    else
    {
      // ERROR!
    }
  }
  else if ( std::regex_match(uri, match, sources_re) )
  {
    sources_handler handler(request, response, this);

    if ( match.size() == 2 )
    {
      handler.call(match[1]);
    }
    else
    {
      // ERROR!
    }
  }
  else if ( std::regex_match(uri, match, assets_re) )
  {
    if ( match.size() == 2 )
    {
      static_file_handler handler(request, response);
      handler.call(match[1]);
    }
    else
    {
      // ERROR!
    }
  }
  else
  {
    not_found(response);
  }
}

// ----------------------------------------------------------------------------
void http_connection::not_found(http::response& response)
{
  response << "HTTP/1.1 404 Not Found" << crlf
    << "Content-Length: 0" << crlf
    << crlf;
}
