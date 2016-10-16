// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__api_handler_base_h__
#define __http_server__api_handler_base_h__

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>

// ----------------------------------------------------------------------------
#include <regex>

// ----------------------------------------------------------------------------
class api_handler_base
{
public:
  api_handler_base(http::request& request, http::response& response)
    :
    request(request),
    response(response),
    route_re_("^/?([^/]*)?/?([^/]*)?")
  {
  }
public:
  virtual ~api_handler_base()
  {
  }
protected:
  void method_not_allowed()
  {
    response << "HTTP/1.1 405 Method Not Allowed" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  void unprocessable_entity()
  {
    response << "HTTP/1.1 422 Unprocessable Entity" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  void not_found()
  {
    response << "HTTP/1.1 404 Not Found" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  void internal_error()
  {
    response << "HTTP/1.1 500 Internal Error" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  http::request& request;
  http::response& response;
protected:
  std::regex route_re_;
protected:
  static constexpr const char* crlf = "\r\n";
};

// ----------------------------------------------------------------------------
#endif