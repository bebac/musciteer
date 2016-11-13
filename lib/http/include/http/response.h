// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
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

  class response : public message
  {
  public:
    response();
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
    void read(std::istream& is);
  protected:
    http::version version_;
    http::status status_;
  };

  class response_environment : public response
  {
  public:
    response_environment(std::basic_streambuf<char>* sbuf)
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

  inline std::istream& operator>>(std::istream& is, http::response& response)
  {
    response.read(is);
    return is;
  }

#if 0
  inline std::ostream& operator<<(std::ostream& os, http::response& response)
  {
    response.write(os);
    return os;
  }
#endif
}

// ----------------------------------------------------------------------------
#endif