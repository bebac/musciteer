// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __http__request_h__
#define __http__request_h__

// ----------------------------------------------------------------------------
#include <http/common.h>

// ----------------------------------------------------------------------------
#include <iostream>

// ----------------------------------------------------------------------------
namespace http
{
  class request : public message
  {
  public:
    request();
  public:
    int port() const;
  public:
    void port(int port);
  public:
    http::method method() const;
  public:
    void method(http::method method);
  public:
    const std::string& uri();
  public:
    void uri(const std::string& uri);
    void uri(std::string&& uri);
  public:
    http::version version();
  public:
    void read(std::istream& is);
  public:
    void write(std::ostream& os) const;
  protected:
    int port_;
    http::method method_;
    std::string uri_;
    http::version version_;
  };

  class request_environment : public request
  {
  public:
    request_environment(std::basic_streambuf<char>* sbuf)
      :
      is(sbuf),
      os(sbuf)
    {
      is.exceptions(std::iostream::failbit);
      os.exceptions(std::iostream::failbit);
    }
  public:
    std::istream is;
    std::ostream os;
  };

  inline std::istream& operator>>(std::istream& is, http::request& request)
  {
    request.read(is);
    return is;
  }

  inline std::ostream& operator<<(std::ostream& os, const http::request& request)
  {
    request.write(os);
    return os;
  }
}

// ----------------------------------------------------------------------------
#endif