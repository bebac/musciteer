// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__sources_handler_h__
#define __http_server__sources_handler_h__

// ----------------------------------------------------------------------------
#include "api.h"

// ----------------------------------------------------------------------------
#include "../dm/source_local.h"
#include "../file_system.h"
#include "../flac_file_importer.h"

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>

// ----------------------------------------------------------------------------
#include <streambuf>
#include <fstream>

// ----------------------------------------------------------------------------
class sources_handler
{
public:
  sources_handler(http::request& request, http::response& response, dripcore::task* task)
    : request(request), response(response), task_(task), route_re_("^/?([^/]*)?/?([^/]*)?")
  {
  }
public:
  void call(const std::string& path)
  {
    std::smatch match;

    if ( std::regex_match(path, match, route_re_) )
    {
      if ( match[0].length() == 0 ) {
        // TODO
        method_not_allowed();
      }
      else if ( match[0] == "/" && match[1].length() == 0 ) {
        // TODO
        method_not_allowed();
      }
      else if ( match[0].length() > 0 && match[1].length() > 0 )
      {
        if ( match[2].length() == 0 ) {
          // TODO
          method_not_allowed();
        }
        else if ( match[1] == "local" && match[2] == "directories" )
        {
          if ( request.method() == http::method::get )
          {
            get_sources_local_directories();
          }
          else if ( request.method() == http::method::post )
          {
            post_sources_local_directories();
          }
          else
          {
            method_not_allowed();
          }
        }
        else if ( match[1] == "local" && match[2] == "scan" )
        {
          if ( request.method() == http::method::post )
          {
            post_sources_local_scan();
          }
          else
          {
            method_not_allowed();
          }
        }
        else {
          not_found();
        }
      }
      else {
        not_found();
      }
    }
    else {
      not_found();
    }
  }
protected:
  void get_sources_local_directories()
  {
    musicbox::source_local source_local{};

    json j;

    j = source_local.directories();

    auto payload = j.dump();

    response << "HTTP/1.1 200 OK" << crlf
      << "Content-Length: " << payload.length() << crlf
      << crlf
      << payload;
  }
protected:
  void post_sources_local_directories()
  {
    std::string content_type;
    std::string content_length_s;
    std::string content;

    if ( !request.get_header("content-type", content_type) ) {
      throw std::runtime_error("no content-type header");
    }

    if ( !request.get_header("content-length", content_length_s) ) {
      throw std::runtime_error("no content-length header");
    }

    auto pos = std::size_t{0};
    auto len = std::stoul(content_length_s, &pos);
    auto buf = request.rdbuf();

    for ( size_t i=0; i<len; ++i) {
      content += buf->sbumpc();
    }

    json j = json::parse(content);

    if ( !j.is_array() ) {
      throw std::runtime_error("source local directories must be an array");
    }

    musicbox::source_local source_local{};

    source_local.directories(j.get<std::vector<std::string>>());

    response << "HTTP/1.1 200 OK" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
  }
protected:
  void post_sources_local_scan()
  {
    musicbox::source_local source_local{};

    for ( auto& dirname : source_local.directories() )
    {
      auto dir = file_system::directory{dirname};

      dir.each_file([&](const std::string& filename)
      {
        if ( file_system::extension(filename) == "flac" )
        {
          std::cout << filename << std::endl;
          musicbox::flac_file_importer importer(filename);

          importer.run();

          task_->yield(true);
        }
      });
    }

    response << "HTTP/1.1 200 OK" << crlf
      << "Content-Length: 0" << crlf
      << crlf;
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
  dripcore::task* task_;
private:
  std::regex route_re_;
private:
  static constexpr const char* crlf = "\r\n";
};

// ----------------------------------------------------------------------------
#endif