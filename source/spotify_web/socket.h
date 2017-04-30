// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __spotify_web__socket_h__
#define __spotify_web__socket_h__

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/socket.h>

// ----------------------------------------------------------------------------
#include <cassert>

// ----------------------------------------------------------------------------
#include <openssl/conf.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// ----------------------------------------------------------------------------
#include <netdb.h>

// ----------------------------------------------------------------------------
namespace spotify_web
{
  class socket : public dripcore::io
  {
  public:
    socket(dripcore::task* task)
      :
      task_(task),
      socket_(),
      ctx_(nullptr),
      ssl_(nullptr)
    {
    }
  public:
    ~socket()
    {
      if ( socket_ )
      {
        socket_.task_detach(task_);
      }
      ssl_free();
    }
  public:
    operator bool() { return !!socket_; }
  public:
    void connect(const std::string& host, const std::string& port = "443")
    {
      struct addrinfo hints;
      struct addrinfo *result = nullptr, *rp;

      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = 0;
      hints.ai_protocol = 0;

      getaddrinfo(host.c_str(), port.c_str(), &hints, &result);

      for (rp = result; rp != NULL; rp = rp->ai_next)
      {
        close();

        socket_ = dripcore::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if ( socket_ )
        {
          socket_.task_attach(task_);
          socket_.nonblocking(true);
          socket_.connect(rp->ai_addr, rp->ai_addrlen);
          socket_.wait_writable();

          ssl_init();
          ssl_handshake();

          break;
        }
        else
        {
          std::cerr << "socket construction failed!" << std::endl;
        }
      }

      freeaddrinfo(result);
    }
  public:
    virtual ssize_t recv(void *buf, size_t len, int flags=0) override
    {
      assert(ssl_);
      return SSL_read(ssl_, buf, len);
    }
  public:
    virtual ssize_t send(const void *buf, size_t len, int flags=0) override
    {
      assert(ssl_);
      return SSL_write(ssl_, buf, len);
    }
  public:
    virtual int native_handle() const override
    {
      return socket_.native_handle();
    }
  private:
    void ssl_handshake()
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
            task_->wait_readable(socket_);
            break;
          case SSL_ERROR_WANT_WRITE:
            task_->wait_writable(socket_);
            break;
          default:
            std::cerr << ERR_error_string(SSL_get_error(ssl_, res), 0) << std::endl;
            throw std::runtime_error("ssl connect error");
        }
      }
      while ( res < 0 );
    }
  private:
    void ssl_init()
    {
      ctx_ = SSL_CTX_new(SSLv23_method());

      if ( !ctx_ ) {
        throw std::runtime_error("ssl context error");
      }

      ssl_ = SSL_new(ctx_);

      if ( !ssl_ ) {
        throw std::runtime_error("ssl error");
      }

      if ( !socket_ ) {
        throw std::runtime_error("socket not valid");
      }

      SSL_set_fd(ssl_, socket_.native_handle());
    }
  private:
    void ssl_free()
    {
      if ( ssl_ ) {
        SSL_free(ssl_);
      }
      if ( ctx_ ) {
        SSL_CTX_free(ctx_);
      }
    }
  private:
    void getaddrinfo(
      const char *node,
      const char *service,
      const struct addrinfo *hints,
      struct addrinfo **result
    )
    {
      gaicb gai[1];
      gaicb* pai = &gai[0];

      pai->ar_name = node;
      pai->ar_service = service;
      pai->ar_request = hints;
      pai->ar_result = 0;

      sigevent ev;

      ev.sigev_notify = SIGEV_THREAD;
      ev.sigev_value.sival_ptr = task_;
      ev.sigev_notify_attributes = nullptr;
      ev.sigev_notify_function = [](sigval value) {
        dripcore::task* task_ptr = reinterpret_cast<dripcore::task*>(value.sival_ptr);
        task_ptr->resume();
      };

      int res;

      res = getaddrinfo_a(GAI_NOWAIT, &pai, 1, &ev);

      if ( res ) {
        throw std::runtime_error(gai_strerror(res));
      }

      task_->yield();

      res = gai_error(pai);

      if ( res ==  0 ) {
        *result = pai->ar_result;
      }
      else {
        throw std::runtime_error(gai_strerror(res));
      }
    }
  private:
    void close() override
    {
      io::close();
      ssl_free();
      if ( socket_ )
      {
        socket_.task_detach(task_);
        socket_.close();
      }
    }
  private:
    dripcore::task* task_;
  private:
    dripcore::socket socket_;
    SSL_CTX* ctx_;
    SSL* ssl_;
  };
}

// ----------------------------------------------------------------------------
#endif
