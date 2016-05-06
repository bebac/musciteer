// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __http__websocket_h__
#define __http__websocket_h__

// ----------------------------------------------------------------------------
#include <istream>
#include <ostream>

// ----------------------------------------------------------------------------
namespace http
{
  namespace websocket
  {
    class header
    {
    private:
      struct frame
      {
        unsigned fin : 1;
        unsigned rsv : 3;
        unsigned opc : 4;
        unsigned msk : 1;
        unsigned len : 7;
      };
    public:
      header()
      {
      }
    public:
      bool fin()
      {
        return frame_.fin == 1;
      }
      void fin(bool value)
      {
        frame_.fin = value ? 1 : 0;
      }
    public:
      unsigned rsv()
      {
        return frame_.rsv;
      }
      void rsv(unsigned value)
      {
        frame_.rsv = value;
      }
    public:
      unsigned opcode()
      {
        return frame_.opc;
      }
      void opcode(unsigned value)
      {
        frame_.opc = value;
      }
    public:
      bool mask()
      {
        return frame_.msk == 1;
      }
      void mask(bool value)
      {
        frame_.msk = value ? 1 : 0;
      }
    public:
      unsigned payload_length()
      {
        return frame_.len;
      }
      void payload_length(unsigned value)
      {
        frame_.len = value;
      }
    public:
      uint8_t masking_key(uint64_t i) const
      {
        return masking_key_[i % 4];
      }
    public:
      void read(std::istream& is)
      {
        auto b1 = is.get();

        frame_.fin = (b1 & 0x80) > 0 ? 1 : 0;
        frame_.rsv = (b1 & 0x70) >> 4;
        frame_.opc = (b1 & 0xf);

        auto b2 = is.get();

        frame_.msk = (b2 & 0x80) > 0 ? 1 : 0;
        frame_.len = (b2 & 0x7f);

        if ( frame_.len > 125 )
        {
        }

        if ( frame_.msk == 1 )
        {
          is.read(reinterpret_cast<char *>(masking_key_), sizeof(masking_key_));
        }
      }
    public:
      void write(std::ostream& os) const
      {
        os.put(frame_.fin << 7 | frame_.rsv << 4 | frame_.opc);
        os.put(frame_.msk << 7 | frame_.len);
      }
    private:
      frame    frame_;
      uint64_t payload_len_;
      uint8_t  masking_key_[4];
    };

    inline std::istream& operator>>(std::istream& is, websocket::header& header)
    {
      header.read(is);
      return is;
    }

    inline std::ostream& operator<<(std::ostream& os, const websocket::header& header)
    {
      header.write(os);
      return os;
    }
  }
}

// ----------------------------------------------------------------------------
#endif
