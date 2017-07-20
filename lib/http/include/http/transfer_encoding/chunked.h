// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2017
//
// ----------------------------------------------------------------------------
#ifndef __http__transfer_encoding__chunked_h__
#define __http__transfer_encoding__chunked_h__

// ----------------------------------------------------------------------------
namespace http
{
  /////
  // Chunking output stream buffer.

  class chunked_body_obuffer : public std::basic_streambuf<char>
  {
  public:
    chunked_body_obuffer(std::ostream& os) : os_(os)
    {
      init_put_area();
    }
  public:
    ~chunked_body_obuffer()
    {
      os_ << std::hex << 0 << "\r\n";
    }
  protected:
    virtual int_type underflow() override
    {
      throw std::runtime_error("cannot read into a chunked_body_obuffer");
    }
  protected:
    virtual int_type overflow(int_type ch = traits_type::eof()) override
    {
      sync();

      if ( ch != traits_type::eof() ) {
        sputc(ch);
      }

      return traits_type::not_eof(ch);
    }
  protected:
    virtual int sync() override
    {
      send();
      init_put_area();

      return 0;
    }
  private:
    ssize_t send()
    {
      auto ptr = pbase();
      auto len = pptr()-ptr;

      os_ << std::hex << len << "\r\n";
      os_.write(ptr, len);
      os_ << "\r\n";

      return len;
    }
  private:
    void init_put_area()
    {
      setp(obuf_.data(), obuf_.data()+obuf_.size());
    }
  private:
    std::array<char, 4096> obuf_;
  private:
    std::ostream& os_;
  };

  /////
  // Chunking output stream.

  class chunked_body_ostream : public std::ostream
  {
  public:
    chunked_body_ostream(std::ostream& os) : std::ostream(&obuf_), obuf_(os)
    {
    }
  private:
    chunked_body_obuffer obuf_;
  };

  /////
  // Chunking stream operator<<.
  //
  // env.os
  //   << "HTTP/1.1 200 OK" << crlf
  //   << "Content-Type: " << "application/json" << crlf
  //   << "Transfer-Encoding: chunked" << crlf
  //   << crlf
  //   << http::chunk << payload
  //   << crlf;
  //

  class chunked_body_ostream_adapter
  {
  public:
    chunked_body_ostream_adapter(std::ostream& os) : os_(os)
    {
    }
  private:
    template<typename T>
    friend std::ostream& operator<<(const chunked_body_ostream_adapter& a, const T& v);
  private:
    std::ostream& os_;
  };

  class chunked_body_type {};

  chunked_body_ostream_adapter operator << (std::ostream& os, const chunked_body_type&)
  {
    return chunked_body_ostream_adapter(os);
  }

  template<typename T>
  std::ostream& operator << (const chunked_body_ostream_adapter& a, const T& v)
  {
    chunked_body_ostream body(a.os_);

    body << v << std::flush;

    return a.os_;
  }

  static constexpr chunked_body_type chunk;
}

// ----------------------------------------------------------------------------
#endif
