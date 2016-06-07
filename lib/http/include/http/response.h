// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __http__response_h__
#define __http__response_h__

// ----------------------------------------------------------------------------
#include <http/common.h>

// ----------------------------------------------------------------------------
#include <iostream>

// ----------------------------------------------------------------------------
namespace http
{
  class status
  {
  public:
    status();
  public:
    unsigned code();
  public:
    const std::string& message();
  public:
    void read(std::istream& is);
  private:
    unsigned code_;
    std::string message_;
  };

  inline std::istream& operator>>(std::istream& is, http::status& status)
  {
    status.read(is);
    return is;
  }

  class response
  {
  public:
    response(std::basic_streambuf<char>* sbuf);
  public:
    virtual ~response();
  public:
    http::version version();
  public:
    unsigned status_code();
  public:
    const std::string& status_message();
  public:
    bool ok();
  public:
    bool get_header(const std::string& key, std::string& value);
  public:
    void read(std::istream& is);
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
    http::version version_;
    http::status status_;
    http::headers headers_;
  private:
    std::iostream ios_;
  };

  inline std::istream& operator>>(std::istream& is, http::response& response)
  {
    response.read(is);
    return is;
  }
}

// ----------------------------------------------------------------------------
#endif