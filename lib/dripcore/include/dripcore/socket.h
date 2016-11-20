// ----------------------------------------------------------------------------
//
//     Filename   : dripcore/socket.h
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __dripcore__socket_h__
#define __dripcore__socket_h__

// ----------------------------------------------------------------------------
#include "io.h"
#include "socket_error.h"

// ----------------------------------------------------------------------------
#include <memory>

// ----------------------------------------------------------------------------
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>

// ----------------------------------------------------------------------------
namespace dripcore
{
  class socket : public io
  {
  public:
    socket() : fd_(-1)
    {
    }
    socket(int fd) : fd_(fd)
    {
    }
    socket(int domain, int type, int protocol) : fd_(-1)
    {
      if ( (fd_ = ::socket(domain, type, protocol)) < 0 ) {
        throw socket_error(errno);
      }
    }
    socket(socket&& other)
    {
      *this = std::move(other);
    }
  public:
    virtual ~socket()
    {
      close();
    }
  private:
    // No copying.
    socket(const socket& other) = delete;
    socket& operator= (const socket& rhs) = delete;
  public:
    socket& operator= (socket&& rhs)
    {
      fd_ = rhs.fd_;
      rhs.fd_ = -1;
      return *this;
    }
  public:
    operator bool() { return fd_ != -1; }
  public:
    void connect(const struct sockaddr *addr, socklen_t addrlen)
    {
      if ( ::connect(fd_, (struct sockaddr *)addr, addrlen) < 0 ) {
        if ( last_error() != EINPROGRESS ) {
          throw socket_error(errno);
        }
      }
    }
  public:
    void bind(const struct sockaddr *addr, socklen_t addrlen)
    {
      if( ::bind(fd_, (struct sockaddr *)addr, addrlen) < 0 ) {
        throw socket_error(errno);
      }
    }
  public:
    void listen(int backlog)
    {
      if( ::listen(fd_, backlog) < 0 ) {
        throw socket_error(errno);
      }
    }
  public:
    socket accept(struct sockaddr *cliaddr, socklen_t *addrlen)
    {
      socket cli(::accept(fd_, NULL, NULL));
      return cli;
    }
  public:
    ssize_t recv(void *buf, size_t len, int flags=0) override
    {
      return ::recv(fd_, buf, len, flags|MSG_NOSIGNAL);
    }
  public:
    ssize_t send(const void *buf, size_t len, int flags=0) override
    {
      return ::send(fd_, buf, len, flags|MSG_NOSIGNAL);
    }
  public:
    void nonblocking(bool value)
    {
      int blocking = ( value ? 1 : 0 );
      if ( ::ioctl(fd_, FIONBIO, (char*)&blocking) < 0 ) {
        throw socket_error(errno);
      }
    }
  public:
    void reuseaddr(bool value)
    {
      int v = ( value ? 1 : 0 );
      if ( setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(int)) == -1 ) {
        throw socket_error(errno);
      }
    }
  public:
    void keepalive(bool value = true)
    {
      int v = ( value ? 1 : 0 );
      if ( setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &v, sizeof(int)) == -1 ) {
        throw socket_error(errno);
      }
    }
  public:
    void close() override
    {
      io::close();
      if ( fd_ != -1 ) {
        ::close(fd_); fd_ = -1;
      }
    }
  public:
    bool ok() { return fd_ != -1; }
  public:
    bool not_ready()
    {
      return last_error() == EWOULDBLOCK || last_error() == EAGAIN;
    }
  public:
    int last_error() const { return errno; }
  public:
    int native_handle() const override
    {
      return fd_;
    }
  private:
    int fd_;
  };

  // ----------------------------------------------------------------------------
  namespace ipv4
  {
    namespace tcp
    {
      inline dripcore::socket socket()
      {
        return dripcore::socket{AF_INET, SOCK_STREAM, IPPROTO_TCP};
      }
    }
  }
}

// ----------------------------------------------------------------------------
#endif
