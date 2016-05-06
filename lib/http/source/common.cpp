// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include <http/common.h>

// ----------------------------------------------------------------------------
#include <istream>
#include <ostream>

// ----------------------------------------------------------------------------
namespace http
{
  bool is_token_char(char c)
  {
    switch(c)
    {
    case  0:
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 127:
    case '(':
    case ')':
    case '<':
    case '>':
    case '@':
    case ',':
    case ';':
    case ':':
    case '\\':
    case '/':
    case '[':
    case ']':
    case '?':
    case '=':
    case '{':
    case '}':
    case ' ':
      return false;
    default:
      return true;
    }
  }

  std::istream& operator>>(std::istream& is, http::version& version)
  {
    version = http::version::unsupported;

    // Ignore SP after uri.

    if ( is.peek() == ' ' ) {
      is.get();
    }

    // HTTP-Version = "HTTP" "/" 1*DIGIT "." 1*DIGIT

    if ( !(
      is.get() == 'H' && is.get() == 'T' &&
      is.get() == 'T' && is.get() == 'P' ) )
    {
      std::cout << "syntax error, expected HTTP next=" << is.peek() << std::endl;
    }

    if ( is.get() == '/' )
    {
      switch ( is.get() )
      {
        case '1':
          if ( is.get() == '.' )
          {
            auto c = is.get();

            if ( c == '0' ) {
              version = http::version::v1_0;
            }
            else if ( c == '1' ) {
              version = http::version::v1_1;
            }
          }
          break;
        case '2':
          version = http::version::v2;
          break;
      }
    }

    // Skip CRLF after version.

    if ( !(is.get() == '\r' && is.get() == '\n') ) {
      // TODO: error.
    }

    return is;
  }

  std::istream& operator>>(std::istream& is, http::method& method)
  {
    method = http::method::not_implemented;

    switch ( is.get() )
    {
      case 'O':
        if (
          is.get() == 'P' && is.get() == 'T' && is.get() == 'I' &&
          is.get() == 'O' && is.get() == 'N' && is.get() == 'S' )
        {
          method = http::method::options;
        }
        break;
      case 'G':
        if ( is.get() == 'E' && is.get() == 'T' )
        {
          method = http::method::get;
        }
        break;
      case 'H':
        if ( is.get() == 'E' && is.get() == 'A' && is.get() == 'D' )
        {
          method = http::method::head;
        }
        break;
      case 'P':
        switch ( is.get() )
        {
          case 'O':
            if ( is.get() == 'S' && is.get() == 'T' ) {
              method = http::method::post;
            }
            break;
          case 'U':
            if ( is.get() == 'T' ) {
              method = http::method::put;
            }
            break;
        }
        break;
      case 'D':
        if (
          is.get() == 'E' && is.get() == 'L' && is.get() == 'E' &&
          is.get() == 'T' && is.get() == 'E' )
        {
          method = http::method::delete_;
        }
        break;
      case 'T':
        if (
          is.get() == 'R' && is.get() == 'A' && is.get() == 'C' &&
          is.get() == 'E' )
        {
          method = http::method::trace;
        }
      case 'C':
        if (
          is.get() == 'O' && is.get() == 'N' && is.get() == 'N' &&
          is.get() == 'E' && is.get() == 'C' && is.get() == 'T' )
        {
          method = http::method::connect;
        }
        break;
    }

    return is;
  }

  std::istream& operator>>(std::istream& is, http::headers& headers)
  {
    // message-headers must start with token char.
    while ( http::is_token_char(is.peek()) )
    {
      std::pair<std::string, std::string> header;

      // Read key.
      std::getline(is, header.first, ':');

      // Skip LWS.
      while ( is.peek() == ' ' || is.peek() == '\t' ) {
        is.get();
      }

      // Read value.
      std::getline(is, header.second, '\r');

      if ( is.peek() == '\n' ) {
        is.get();
      }

      headers.emplace(header);
    }

    // Skip CRLF after version.
    if ( !(is.get() == '\r' && is.get() == '\n') ) {
      // TODO: error.
      std::cout << "error headers should end with crlf" << std::endl;
    }

    return is;
  }
}
