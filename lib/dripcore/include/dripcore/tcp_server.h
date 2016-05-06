// ----------------------------------------------------------------------------
//
//     Filename   : dripcore/tcp_server.h
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __dripcore__tcp_server_h__
#define __dripcore__tcp_server_h__

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/socket.h>

// ----------------------------------------------------------------------------
#include <cstring>

// ----------------------------------------------------------------------------
namespace dripcore
{
  class tcp_server : public task
  {
    using connection_func = std::function<void(socket socket)>;
  public:
    tcp_server(const char* ip, unsigned port, connection_func connection_cb)
      :
      socket_(ipv4::tcp::socket()),
      connection_cb_(connection_cb)
    {
      struct sockaddr_in sock_addr;

      std::memset(&sock_addr, 0, sizeof(sock_addr));

      sock_addr.sin_family = AF_INET;
      sock_addr.sin_port = htons(port);

      auto res = inet_pton(AF_INET, ip, &sock_addr.sin_addr);

      if ( res == 0 ) {
        throw std::runtime_error("invalid ip address");
      }
      else if ( res < 0 ) {
        throw std::system_error(errno, std::system_category());
      }

      socket_.nonblocking(true);
      socket_.reuseaddr(true);
      socket_.bind((struct sockaddr *)&sock_addr, sizeof(sock_addr));
      socket_.listen(5);
    }
  public:
    ~tcp_server()
    {
      detach_eventable(socket_);
    }
  public:
    void main() override
    {
      attach_eventable(socket_);

      do
      {
        auto client = socket_.accept(0, 0);

        if ( client )
        {
          client.nonblocking(true);
          connection_cb_(std::move(client));
        }
        else if ( client.not_ready() )
        {
          wait_readable(socket_);
        }
        else
        {
          throw std::system_error(client.last_error(), std::system_category());
        }
      }
      while ( true );
    }
  private:
    socket socket_;
    connection_func  connection_cb_;
  };
}

// ----------------------------------------------------------------------------
#endif