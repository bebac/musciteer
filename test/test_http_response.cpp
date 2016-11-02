// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include <http/response.h>

// ----------------------------------------------------------------------------
#include <sstream>

// ----------------------------------------------------------------------------
#include "doctest/doctest.h"

// ----------------------------------------------------------------------------
TEST_CASE("Parse http status")
{
  std::istringstream is("200 OK\r\n");

  http::status status;

  is >> status;

  CHECK(!is.bad());
  CHECK(!is.eof());
  CHECK(status.code() == 200u);
  CHECK(status.message() == "OK");
}

// ----------------------------------------------------------------------------
TEST_CASE("Write http response")
{
  std::stringstream os;

  os << "HTTP/1.1 200 OK\r\n";

  CHECK(os.str() == "HTTP/1.1 200 OK\r\n");
}

// ----------------------------------------------------------------------------
TEST_CASE("Read http response (ok)")
{
  std::istringstream is(
    "HTTP/1.1 200 OK\r\n"
    "Server: nginx\r\n"
    "Content-Length: 0\r\n"
    "\r\n"
  );

  http::response response;

  is >> response;

  CHECK(response.version() == http::version::v1_1);
  CHECK(response.status_code() == 200u);
  CHECK(response.ok());
  CHECK(response.status_message() == "OK");

  bool header_found;
  std::string header_value;

  header_found = response.get_header("server", header_value);

  CHECK(header_found);
  CHECK(header_value == "nginx");

  header_found = response.get_header("content-length", header_value);

  CHECK(header_found);
  CHECK(header_value == "0");
}

// ----------------------------------------------------------------------------
TEST_CASE("Read http response (bad request)")
{
  std::istringstream is(
    "HTTP/1.1 400 Bad Request\r\n"
    "\r\n"
  );

  http::response response;

  is >> response;

  CHECK(response.version() == http::version::v1_1);
  CHECK(response.status_code() == 400u);
  CHECK(!response.ok());
  CHECK(response.status_message() == "Bad Request");
}