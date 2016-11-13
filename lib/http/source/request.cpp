// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include <http/request.h>

// ----------------------------------------------------------------------------
namespace http
{
  request::request()
    :
    port_(80),
    method_(http::method::get),
    version_(http::version::v1_1)
  {
  }

  int request::port() const
  {
    return port_;
  }

  void request::port(int port)
  {
    port_ = port;
  }

  http::method request::method() const
  {
    return method_;
  }

  void request::method(http::method method)
  {
    method_ = method;
  }

  const std::string& request::uri()
  {
    return uri_;
  }

  void request::uri(const std::string& uri)
  {
    uri_ = uri;
  }

  void request::uri(std::string&& uri)
  {
    uri_ = std::move(uri);
  }

  http::version request::version()
  {
    return version_;
  }

  bool request::get_content_length(size_t& value)
  {
    std::string s;

    if ( !get_header("Content-Length", s) ) {
      return false;
    }

    auto pos = std::size_t{0};
    value = std::stoul(s, &pos);

    return true;
  }

  bool request::get_header(const std::string& key, std::string& value)
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

  void request::set_header(const std::string& key, const std::string& value)
  {
    headers_.emplace(key, value);
  }

  void request::set_header(const std::string& key, std::string&& value)
  {
    headers_.emplace(key, std::move(value));
  }

  void request::read(std::istream& is)
  {
    is >> method_ >> uri_ >> version_ >> headers_;
  }

  void request::write(std::ostream& os) const
  {
    switch ( method_ )
    {
      case http::method::options:
        os << "OPTIONS ";
        break;
      case http::method::get:
        os << "GET ";
        break;
      case http::method::head:
        os << "HEAD ";
        break;
      case http::method::post:
        os << "POST ";
        break;
      case http::method::put:
        os << "PUT ";
        break;
      case http::method::delete_:
        os << "DELETE ";
        break;
      case http::method::trace:
        os << "TRACE ";
        break;
      case http::method::connect:
        os << "CONNECT ";
        break;
      case http::method::not_implemented:
        // ERROR!
        break;
    };

    os << uri_ << " ";

    switch ( version_ )
    {
      case http::version::v1_0:
        os << "HTTP/1.0";
        break;
      case http::version::v1_1:
        os << "HTTP/1.1";
        break;
      default:
        // ERROR!
        break;
    };

    os << "\r\n";

    for ( auto& hdr : headers_ )
    {
      os << hdr.first << ": " << hdr.second << "\r\n";
    }

    os << "\r\n";
  }
}