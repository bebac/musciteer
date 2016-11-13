// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
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

    if ( is.get() != ' ' ) {
      is.setstate(std::ios_base::failbit);
    }

    std::getline(is, message_, '\r');

    if ( is.get() != '\n' ) {
      is.setstate(std::ios_base::failbit);
    }
  }

  /////
  // response.

  response::response()
  {
  }

  response::~response()
  {
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

  void response::read(std::istream& is)
  {
    is >> version_ >> status_ >> headers_;
  }
}