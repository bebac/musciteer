// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include <http/response.h>

// ----------------------------------------------------------------------------
namespace http
{
  /////
  // status.

  status::status()
  {
  }

  unsigned status::code()
  {
    return code_;
  }

  const std::string& status::message()
  {
    return message_;
  }

  void status::read(std::istream& is)
  {
    is >> code_;

    if ( is.peek() != ' ' ) {
      is.setstate(std::ios_base::failbit);
    }

    is >> message_;

    if ( !(is.get() == '\r' && is.get() == '\n') ) {
      is.setstate(std::ios_base::failbit);
    }
  }

  /////
  // response.

  response::response(std::basic_streambuf<char>* sbuf)
    :
    ios_(sbuf)
  {
    ios_.exceptions(std::iostream::failbit);
  }

  response::~response()
  {
    ios_.flush();
  }

  http::version response::version()
  {
    return version_;
  }

  unsigned response::status_code()
  {
    return status_.code();
  }

  const std::string& response::status_message()
  {
    return status_.message();
  }

  bool response::ok()
  {
    return status_.code() == 200u;
  }

  bool response::get_header(const std::string& key, std::string& value)
  {
    auto it = headers_.find(key);

    if ( it != headers_.end() )
    {
      value = it->second;
      return true;
    }
    else
    {
      return false;
    }
  }

  void response::read(std::istream& is)
  {
    is >> version_ >> status_ >> headers_;
  }
}