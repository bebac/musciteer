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
  class request
  {
  public:
    request(std::basic_streambuf<char>* sbuf);
  public:
    http::method method();
  public:
    const std::string& uri();
  public:
    http::version version();
  public:
    bool get_header(const std::string& key, std::string& value);
  public:
    void read(std::istream& is);
  public:
    void write(std::ostream& os);
  public:
    template<typename T>
    std::istream& operator>>(T& value)
    {
      ios_ >> value;
      return ios_;
    }
  public:
    template<typename T>
    std::ostream& operator<<(T value)
    {
      ios_ << std::forward<T>(value);
      return ios_;
    }
  public:
    std::basic_streambuf<char>* rdbuf();
  protected:
    http::method method_;
    std::string uri_;
    http::version version_;
    http::headers headers_;
  private:
    std::iostream ios_;
  };

  inline std::istream& operator>>(std::istream& is, http::request& request)
  {
    request.read(is);
    return is;
  }
}

// ----------------------------------------------------------------------------
#endif