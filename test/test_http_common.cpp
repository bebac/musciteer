// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include <http/common.h>

// ----------------------------------------------------------------------------
#include <sstream>

// ----------------------------------------------------------------------------
#include "doctest/doctest.h"

// ----------------------------------------------------------------------------
TEST_CASE("Parse http version 1.0")
{
  std::istringstream is("HTTP/1.0");

  http::version version;

  is >> version;

  CHECK(!is.bad());
  CHECK(is.eof());
  CHECK(version == http::version::v1_0);
}

// ----------------------------------------------------------------------------
TEST_CASE("Parse http version 1.1")
{
  std::istringstream is("HTTP/1.1");

  http::version version;

  is >> version;

  CHECK(!is.bad());
  CHECK(is.eof());
  CHECK(version == http::version::v1_1);
}

// ----------------------------------------------------------------------------
TEST_CASE("Parse all supported http methods")
{
  std::istringstream is("OPTIONS GET HEAD POST PUT DELETE TRACE CONNECT");

  http::method method;

  is >> method;

  CHECK(!is.bad());
  CHECK(method == http::method::options);

  is >> method;

  CHECK(!is.bad());
  CHECK(method == http::method::get);

  is >> method;

  CHECK(!is.bad());
  CHECK(method == http::method::head);

  is >> method;

  CHECK(!is.bad());
  CHECK(method == http::method::post);

  is >> method;

  CHECK(!is.bad());
  CHECK(method == http::method::put);

  is >> method;

  CHECK(!is.bad());
  CHECK(method == http::method::delete_);

  is >> method;

  CHECK(!is.bad());
  CHECK(method == http::method::trace);

  is >> method;

  CHECK(!is.bad());
  CHECK(method == http::method::connect);
}

// ----------------------------------------------------------------------------
TEST_CASE("Parse unsupported http method")
{
  std::istringstream is("ZZZ");

  http::method method;

  is >> method;

  CHECK(!is.bad());
  CHECK(method == http::method::not_implemented);
}