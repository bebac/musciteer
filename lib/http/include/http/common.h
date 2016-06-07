// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __http__common_h__
#define __http__common_h__

// ----------------------------------------------------------------------------
#include <cstring>
#include <iostream>
#include <map>

// ----------------------------------------------------------------------------
namespace http
{
  enum class version
  {
    v1_0,
    v1_1,
    v2,
    unsupported
  };

  enum class method
  {
    options,
    get,
    head,
    post,
    put,
    delete_,
    trace,
    connect,
    not_implemented
  };

  class case_insensitive_compare {
  public:
    bool operator()(const std::string& rhs, const std::string& lhs) const {
      return strcasecmp(lhs.c_str(), rhs.c_str()) < 0 ? 1 : 0;
    }
  };

  using headers = std::map<std::string, std::string, case_insensitive_compare>;

  std::istream& operator>>(std::istream& is, http::version& version);
  std::istream& operator>>(std::istream& is, http::method& method);
  std::istream& operator>>(std::istream& is, http::headers& headers);
}

// ----------------------------------------------------------------------------
#endif