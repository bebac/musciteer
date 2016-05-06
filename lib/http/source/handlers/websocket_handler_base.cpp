// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include <http/handlers/websocket_handler_base.h>

// ----------------------------------------------------------------------------
#include <crypto++/sha.h>

// ----------------------------------------------------------------------------
namespace http
{
  websocket_handler_base::websocket_handler_base(http::request& request,
    http::response& response) : request(request), response(response)
  {
  }

  websocket_handler_base::~websocket_handler_base()
  {
  }

  void websocket_handler_base::call(const std::string& path)
  {
    std::string sec_websocket_key;

    if ( request.get_header("Sec-WebSocket-Key", sec_websocket_key) )
    {
      std::string sec_websocket_accept = sec_websocket_key+guid;

      byte digest[CryptoPP::SHA1::DIGESTSIZE];

      CryptoPP::SHA1().CalculateDigest(
        digest,
        reinterpret_cast<const unsigned char*>(sec_websocket_accept.data()),
        sec_websocket_accept.length()
      );

      response << "HTTP/1.1 101 Switching Protocols" << crlf
        << "Upgrade: websocket" << crlf
        << "Connection: Upgrade" << crlf
        << "Sec-WebSocket-Accept: " << base64::encode(digest, sizeof(digest)) << crlf
        << crlf
        << std::flush;

      //
      // We are done with the request object. Pass the stream buffer
      // to switch_protocol.
      //
      switch_protocol(request.rdbuf());
    }
    else
    {
      // ERROR.
    }
  }

  void websocket_handler_base::switch_protocol(std::basic_streambuf<char>* sbuf)
  {
    std::istream is(sbuf);

    on_connect();

    while ( is.good() )
    {
      websocket::header header;

      if ( is >> header )
      {
        if ( header.rsv() != 0 )
        {
          // ERROR!
        }

#if 0
        std::cout << "websocket:" << std::endl
          << "  fin : " << header.fin() << std::endl
          << "  rsv : " << header.rsv() << std::endl
          << "  opc : " << header.opcode() << std::endl
          << "  msk : " << header.mask() << std::endl
          << "  len : " << header.payload_length() << std::endl;
#endif

        dispatch(header, is);
      }
      else
      {
        std::cout << "websocket streaming error!" << std::endl;
      }
    }
  }

  void websocket_handler_base::dispatch(http::websocket::header& header, std::istream& payload)
  {
    switch ( header.opcode() )
    {
      case 0:
        // Not implemented yet.
        break;
      case 1:
      {
        std::string message;

        // TODO: Check fin.
        // TODO: Check masking

        message.reserve(header.payload_length());
        for ( auto i=0; i<header.payload_length(); ++i )
        {
          message.push_back(payload.get() ^ header.masking_key(i));
        }
        on_message(message);
        break;
      }
      case 2:
        // Not implemented yet.
        break;
      case 8:
        on_close();
        payload.setstate(std::ios_base::eofbit);
        return;
    }
  }

  void websocket_handler_base::send_message(const std::string& message)
  {
    http::websocket::header header;

    header.fin(true);
    header.rsv(0);
    header.opcode(1);
    header.mask(false);
    header.payload_length(message.length());

    response << header << message << std::flush;
  }
}