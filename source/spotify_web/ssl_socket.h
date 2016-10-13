// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __spotify_web__ssl_socket_h__
#define __spotify_web__ssl_socket_h__

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/socket.h>

// ----------------------------------------------------------------------------
#include <openssl/conf.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// ----------------------------------------------------------------------------
namespace spotify_web
{
  class ssl_socket : public dripcore::io
  {
  public:
    ssl_socket(dripcore::socket&& socket) : socket_(std::move(socket))
    {
      ctx_ = SSL_CTX_new(SSLv23_method());

      if ( !ctx_ ) {
        throw std::runtime_error("ssl context error");
      }

      ssl_ = SSL_new(ctx_);

      if ( !ssl_ ) {
        throw std::runtime_error("ssl error");
      }

      SSL_set_fd(ssl_, socket_.native_handle());
    }
  public:
    ~ssl_socket()
    {
      SSL_free(ssl_);
      SSL_CTX_free(ctx_);
    }
  public:
    void connect(dripcore::task* task)
    {
      int res;
      do
      {
        res = SSL_connect(ssl_);

        switch (SSL_get_error(ssl_, res))
        {
          case SSL_ERROR_NONE:
            break;
          case SSL_ERROR_WANT_READ:
            task->wait_readable(socket_);
            break;
          case SSL_ERROR_WANT_WRITE:
            task->wait_writable(socket_);
            break;
          default:
            std::cerr << ERR_error_string(SSL_get_error(ssl_, res), 0) << std::endl;
            throw std::runtime_error("ssl connect error");
        }
      }
      while ( res < 0 );
    }
  public:
    virtual ssize_t recv(void *buf, size_t len, int flags=0) override
    {
      return SSL_read(ssl_, buf, len);
    }
  public:
    virtual ssize_t send(const void *buf, size_t len, int flags=0) override
    {
      return SSL_write(ssl_, buf, len);
    }
  public:
    virtual int native_handle() const override
    {
      return socket_.native_handle();
    }
  private:
    dripcore::socket socket_;
    SSL_CTX* ctx_;
    SSL* ssl_;
  };
}

// ----------------------------------------------------------------------------
#endif