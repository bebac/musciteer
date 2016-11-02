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
#include "../file_system.h"

// ----------------------------------------------------------------------------
#include <http/request.h>

// ----------------------------------------------------------------------------
#include <fstream>
#include <sstream>

// ----------------------------------------------------------------------------
class static_file_handler
{
public:
  static_file_handler(http::request_environment& env)
    :
    env(env)
  {
    const char* snap_path = getenv("SNAP");

    if ( snap_path ) {
      root_ = std::string(snap_path) + "/" + public_dir;
    }
    else {
      root_ = public_dir;
    }
    //std::cout << "static_file_handler root_=" << root_ << std::endl;
  }
public:
  void call(const std::string& path)
  {
    if ( env.method() == http::method::get )
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

    auto filename = root_ + path;

    file_system::status status(filename);

    if ( status )
    {
      auto ims_s = std::string();
      auto ims_t = std::chrono::system_clock::time_point();

      if ( env.get_header("If-Modified-Since", ims_s) )
      {
        auto is = std::istringstream{ims_s};
        auto tm = std::tm{};

        if ( is >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S GMT") )
        {
          ims_t = std::chrono::system_clock::from_time_t(::timegm(&tm));
        }
        else
        {
          std::cerr << "static_file_handler - failed to parse If-Modified-Since header value" << std::endl;
        }
      }

      auto newer = std::chrono::duration_cast<std::chrono::seconds>(status.mtime() - ims_t);

      if ( newer.count() > 0 )
      {
        ifstream f(filename, ifstream::ate | ifstream::binary);

        if ( f )
        {
          auto size = f.tellg();
          auto lm_t = std::chrono::system_clock::to_time_t(status.mtime());

          env.os << "HTTP/1.1 200 OK" << crlf
            << "Content-Type: " << mime_type(path) << crlf
            << "Content-Length: " << size << crlf
            << "Last-Modified:" << std::put_time(std::gmtime(&lm_t), "%a, %d %b %Y %H:%M:%S %Z") << crlf
            << crlf;

          f.seekg(0);

          env.os << f.rdbuf();
        }
        else
        {
          not_found();
        }
      }
      else
      {
        not_modified();
      }
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
      else if ( ext == ".svg" ) {
        res = "image/svg+xml svg";
      }
    }

    return res;
  }
protected:
  void not_modified()
  {
    env.os << "HTTP/1.1 304 Not Modified" << crlf
      << "Content-Length: " << 0 << crlf
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
  void not_found()
  {
    env.os << "HTTP/1.1 404 Not Found" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  http::request_environment& env;
protected:
  std::string root_;
private:
  static constexpr const char* crlf = "\r\n";
  static constexpr const char* public_dir = "public/";
};

// ----------------------------------------------------------------------------
#endif