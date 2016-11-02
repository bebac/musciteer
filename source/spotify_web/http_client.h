// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __spotify_web__http_client_h__
#define __spotify_web__http_client_h__

// ----------------------------------------------------------------------------
#include "ssl_socket.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/socket.h>
#include <dripcore/streambuf.h>

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>

// ----------------------------------------------------------------------------
#include <json.h>

// ----------------------------------------------------------------------------
#include <regex>

// ----------------------------------------------------------------------------
#include <netdb.h>

// ----------------------------------------------------------------------------
namespace spotify_web
{
  class http_client
  {
  public:
    http_client(dripcore::task* task)
      : task_(task), url_re_("^(http|https)://([^:/]*)(:?([^/]+)?(.*))")
    {
    }
  public:
    void get(const std::string url, std::function<void(http::response_environment&)> cb)
    {
      auto scheme = std::string{};
      auto hostname = std::string{};
      auto port_s = std::string{};
      auto port = 0;
      auto path = std::string{};

      std::smatch match;

      if ( std::regex_match(url, match, url_re_) )
      {
        scheme = match[1];
        hostname = match[2];
        port_s = match[4];
        path = match[5];

        if ( scheme == "http" && port_s.empty() )
        {
          port = 80;
        }
        else if ( scheme == "https" && port_s.empty() )
        {
          port = 443;
        }
        else
        {
          port = atoi(port_s.c_str());
        }
      }
      else
      {
        throw std::runtime_error("invalid url");
      }

      auto socket = dripcore::ipv4::tcp::socket();

      init_sock_addr(hostname, port);
      connect(socket);

      if ( scheme == "https" )
      {
        socket.task_detach(task_);

        spotify_web::ssl_socket ssl_socket(std::move(socket));

        ssl_socket.task_attach(task_);

        dripcore::streambuf sbuf(ssl_socket, *task_);

        // Perform ssl handshake.
        ssl_socket.connect(task_);

        http::response_environment env(&sbuf);

        //http::request request(&sbuf);

        //request
        env.os
          << "GET " << path << " HTTP/1.1\r\n"
          << "Host: " << hostname << "\r\n"
          << "\r\n"
          << std::flush;

        env.is >> env;

        cb(env);
      }
      else
      {
        dripcore::streambuf sbuf(socket, *task_);

        http::response_environment env(&sbuf);

        env.os
          << "GET " << path << " HTTP/1.1\r\n"
          << "Host: " << hostname << "\r\n"
          << "\r\n"
          << std::flush;

        env.is >> env;

        cb(env);
      }
    }
  public:
#if 0
    void get(http::request& req, std::function<void(http::response_environment&)> res_cb, bool tls = false)
    {
      auto port = req.port();
      auto path = req.uri();
      auto host = std::string{};

      if ( !req.get_header("Host", host) ) {
        throw std::runtime_error("http request requires a host header");
      }

      auto socket = dripcore::ipv4::tcp::socket();

      init_sock_addr(host, port);
      connect(socket);

      if ( tls )
      {
        socket.task_detach(task_);

        spotify_web::ssl_socket ssl_socket(std::move(socket));

        ssl_socket.task_attach(task_);

        dripcore::streambuf sbuf(ssl_socket, *task_);

        // Perform ssl handshake.
        ssl_socket.connect(task_);

        http::response_environment env(&sbuf);

        env.os << req;
        env.is >> env;

        res_cb(env);
      }
      else
      {
        dripcore::streambuf sbuf(socket, *task_);

        http::response_environment env(&sbuf);

        env.os << req;
        env.is >> env;

        res_cb(env);
      }
    }
#endif
  public:
    json get_json(const std::string& url)
    {
      json j;

      get(url, [&](http::response_environment& response)
      {
        std::string content_type;
        std::string content_length_s;
        std::string content;
        std::string accept = "application/json";

        if ( !response.get_header("content-type", content_type) ) {
          throw std::runtime_error("no content-type header");
        }

        if ( content_type.compare(0, accept.size(), accept) != 0 ) {
          throw std::runtime_error("wrong content type");
        }

        if ( !response.get_header("content-length", content_length_s) ) {
          throw std::runtime_error("no content-length header");
        }

        auto pos = std::size_t{0};
        auto len = std::stoul(content_length_s, &pos);

        for ( size_t i=0; i<len; ++i) {
          content += response.is.get();
        }

        j = json::parse(content);
      });

      return j;
    }
  private:
    void connect(dripcore::socket& socket)
    {
      socket.task_attach(task_);
      socket.nonblocking(true);
      socket.connect((struct sockaddr *)&sock_addr_, sizeof(sock_addr_));
      socket.wait_writable();
    }
  private:
    void init_sock_addr(const std::string& hostname, unsigned port)
    {
      gaicb aicb[1];
      gaicb* pai = &aicb[0];

      pai->ar_name = hostname.c_str();
      pai->ar_service = 0;
      pai->ar_request = 0;
      pai->ar_result = 0;

      sigevent ev;

      ev.sigev_notify = SIGEV_THREAD;
      ev.sigev_value.sival_ptr = this;
      ev.sigev_notify_attributes = nullptr;
      ev.sigev_notify_function = [](sigval value) {
        http_client* self = reinterpret_cast<http_client*>(value.sival_ptr);
        self->task_->resume();
      };

      int res;

      res = getaddrinfo_a(GAI_NOWAIT, &pai, 1, &ev);

      if ( res ) {
        throw std::runtime_error(gai_strerror(res));
      }

      task_->yield();

      res = gai_error(pai);

      if ( res ) {
        throw std::runtime_error(gai_strerror(res));
      }

      if ( sizeof(sock_addr_) != pai->ar_result->ai_addrlen ) {
        throw std::runtime_error("wrong sock_addr size");
      }

      std::memcpy(&sock_addr_, pai->ar_result->ai_addr, pai->ar_result->ai_addrlen);

      sock_addr_.sin_family = AF_INET;
      sock_addr_.sin_port = htons(port);

      freeaddrinfo(pai->ar_result);
    }
  private:
    dripcore::task* task_;
  private:
    struct sockaddr_in sock_addr_;
    std::regex url_re_;
  };
}

// ----------------------------------------------------------------------------
#endif
