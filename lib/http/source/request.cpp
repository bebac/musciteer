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
  request::request(std::basic_streambuf<char>* sbuf)
    : ios_(sbuf)
  {
    ios_.exceptions(std::iostream::failbit);
  }

  http::method request::method()
  {
    return method_;
  }

  const std::string& request::uri()
  {
    return uri_;
  }

  http::version request::version()
  {
    return version_;
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

  void request::read(std::istream& is)
  {
    is >> method_ >> uri_ >> version_ >> headers_;
  }

  void request::write(std::ostream& os)
  {
    std::cout << "method " << int(method_) << ", path " << uri_ << ", version " << int(version_) << std::endl;
    for ( auto& hdr : headers_ )
    {
      std::cout << hdr.first << ": " << hdr.second << "\r\n";
    }
  }

  std::basic_streambuf<char>* request::rdbuf()
  {
    return ios_.rdbuf();
  }
}