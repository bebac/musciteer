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
#include <http/transfer_encoding/chunked.h>

// ----------------------------------------------------------------------------
#include <regex>

// ----------------------------------------------------------------------------
class api_handler_base
{
public:
  api_handler_base(http::request_environment& env)
    :
    env(env),
    route_re_("^/?([^/]*)?/?([^/]*)/?([^/]*)")
  {
  }
public:
  virtual ~api_handler_base()
  {
  }
protected:
  void ok()
  {
    env.os << "HTTP/1.1 200 OK" << crlf
      << "Content-Length: " << 0 << crlf
      << crlf;
  }
protected:
  void ok(json body)
  {
    env.os << "HTTP/1.1 200 OK" << crlf
      << "Content-Type: " << "application/json" << crlf
      << "Transfer-Encoding: chunked" << crlf
      << crlf
      << http::chunk << body
      << crlf;
  }
protected:
  void redirect(const std::string& location)
  {
    env.os << "HTTP/1.1 301 Moved Permanently" << crlf
      << "location: " << location << crlf
      << crlf;
  }
protected:
  void method_not_allowed()
  {
    env.os << "HTTP/1.1 405 Method Not Allowed" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  void unprocessable_entity()
  {
    env.os << "HTTP/1.1 422 Unprocessable Entity" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  void not_found()
  {
    env.os << "HTTP/1.1 404 Not Found" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  void internal_error()
  {
    env.os << "HTTP/1.1 500 Internal Error" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  http::request_environment& env;
protected:
  std::regex route_re_;
protected:
  static constexpr const char* crlf = "\r\n";
};

// ----------------------------------------------------------------------------
#endif