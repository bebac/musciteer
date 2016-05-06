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
#include "static_file_handler.h"

// ----------------------------------------------------------------------------
#include <dripcore/socket.h>
#include <dripcore/socket_streambuf.h>

// ----------------------------------------------------------------------------
#include <regex>

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
  std::unique_ptr<dripcore::socket_streambuf> sbuf(new dripcore::socket_streambuf(socket_, *this));

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

  std::regex assets_re("^/(assets/.+)");
  std::smatch assets_match;

  if ( uri == "/")
  {
    static_file_handler handler(request, response);
    handler.call("index.html");
  }
  else if ( std::regex_match(uri, assets_match, assets_re) )
  {
    if ( assets_match.size() == 2 )
    {
      static_file_handler handler(request, response);
      handler.call(assets_match[1]);
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
