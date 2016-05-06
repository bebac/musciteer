// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __http_server__static_file_handler_h__
#define __http_server__static_file_handler_h__

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>

// ----------------------------------------------------------------------------
#include <streambuf>
#include <fstream>

// ----------------------------------------------------------------------------
class static_file_handler
{
public:
  static_file_handler(http::request& request, http::response& response)
    :
    request(request),
    response(response)
  {
  }
public:
  void call(const std::string& path)
  {
    if ( request.method() == http::method::get )
    {
      get(path);
    }
    else
    {
      method_not_allowed();
    }
  }
protected:
  void get(const std::string& path)
  {
    using std::ifstream;

    ifstream f(public_dir+path, ifstream::ate | ifstream::binary);

    if ( f )
    {
      auto size = f.tellg();

      response << "HTTP/1.1 200 OK" << crlf
         << "Content-Type: " << mime_type(path) << crlf
         << "Content-Length: " << size << crlf
         << crlf;

      f.seekg(0);

      response << f.rdbuf();
    }
    else
    {
      not_found();
    }
  }
protected:
  std::string mime_type(const std::string& path)
  {
    auto res = std::string("text/plain");
    auto pos = path.find_last_of('.');

    if ( pos != std::string::npos )
    {
      auto ext = path.substr(pos);

      if ( ext == ".html" ) {
        res = "text/html";
      }
      else if ( ext == ".js" ) {
        res = "application/javascript";
      }
      else if ( ext == ".css" ) {
        res = "text/css";
      }
    }

    return res;
  }
protected:
  void method_not_allowed()
  {
    response << "HTTP/1.1 405 Method Not Allowed" << crlf
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
  http::request& request;
  http::response& response;
private:
  static constexpr const char* crlf = "\r\n";
  static constexpr const char* public_dir = "public/";
};

// ----------------------------------------------------------------------------
#endif