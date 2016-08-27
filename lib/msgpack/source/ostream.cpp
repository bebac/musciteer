#include <msgpack/ostream.h>

// ----------------------------------------------------------------------------
namespace msgpack
{
  ostream::ostream(std::streambuf* buf) : std::ostream(buf)
  {
  }

  void ostream::write_i16(short value)
  {
    if ( value >= -0x20  && value < 0x80 )
    {
      put(value);
    }
    else if ( value >= -0x80 && value < -0x20 )
    {
      put(0xd0, static_cast<char>(value));
    }
    else
    {
      put(0xd1, static_cast<short>(value));
    }
  }

  void ostream::write_u16(unsigned short value)
  {
    if ( value < 0x80 )
    {
      put(value);
    }
    else if ( value < 0x100 )
    {
      put(0xcc, static_cast<unsigned char>(value));
    }
    else
    {
      put(0xcd, static_cast<unsigned short>(value));
    }
  }

  void ostream::write_i32(int value)
  {
    if ( value >= -0x8000 && value <= 0x7fff )
    {
      write_i16(value);
    }
    else
    {
      put(0xd2, value);
    }
  }

  void ostream::write_u32(unsigned int value)
  {
    if ( value < 0x10000 )
    {
      write_u16(value);
    }
    else
    {
      put(0xce, value);
    }
  }

  void ostream::write_i64(long long value)
  {
    if ( value >= -0x80000000 && value <= 0x7fffffff )
    {
      write_i32(value);
    }
    else
    {
      put(0xd3, value);
    }
  }

  void ostream::write_u64(unsigned long long value)
  {
    if ( value < 0x100000000)
    {
      write_i32(value);
    }
    else
    {
      put(0xcf, value);
    }
  }

  void ostream::write_bool(bool value)
  {
    if ( value )
    {
      put(0xc3);
    }
    else
    {
      put(0xc2);
    }
  }

  void ostream::write_float(float value)
  {
    union
    {
      float f;
      uint32_t i;
    } m;

    m.f = value;

    put(0xca, m.i);
  }

  void ostream::write_string(const char* value, size_t len)
  {
    if ( len < 0x20 )
    {
      put(0xa0 | len);
    }
    else if ( len < 0x100 )
    {
      put(0xd9, static_cast<unsigned char>(len));
    }
    else if ( len < 0x10000)
    {
      put(0xda, static_cast<unsigned short>(len));
    }
    else
    {
      put(0xdb, static_cast<unsigned int>(len));
    }
    write(value, len);
  }
}
