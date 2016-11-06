// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include <http/request.h>

// ----------------------------------------------------------------------------
#include <sstream>

// ----------------------------------------------------------------------------
#include "doctest/doctest.h"

// ----------------------------------------------------------------------------
TEST_CASE("Build request")
{
  http::request request;

  SUBCASE("Default constructed")
  {
    CHECK(request.port() == 80);
    CHECK(request.method() == http::method::get);
    CHECK(request.version() == http::version::v1_1);
  }
}

// ----------------------------------------------------------------------------
TEST_CASE("Http environemnt")
{
  std::stringbuf buf;
  http::request_environment env(&buf);
}

// ----------------------------------------------------------------------------
TEST_CASE("Write http request")
{
  http::request request;

  //request.get("http://jsonplaceholder.typicode.com/posts");
  request.uri("/");
  request.set_header("Host", "host.com");

  std::stringstream os;

  os << request;

  CHECK(os.str() == "GET / HTTP/1.1\r\nHost: host.com\r\n\r\n");
}

// ----------------------------------------------------------------------------
TEST_CASE("Read http request")
{
  std::istringstream is(
    "GET /posts HTTP/1.1\r\n"
    "Host: jsonplaceholder.typicode.com\r\n"
    "\r\n"
  );

  http::request request;

  is >> request;

  CHECK(request.version() == http::version::v1_1);
  CHECK(request.method() == http::method::get);
  CHECK(request.uri() == "/posts");

  bool header_found;
  std::string header_value;

  header_found = request.get_header("host", header_value);

  CHECK(header_found);
  CHECK(header_value == "jsonplaceholder.typicode.com");
}
