#include <msgpack/istream.h>
#include <limits>

// ----------------------------------------------------------------------------
namespace msgpack
{
  istream::istream(std::streambuf* buf) : std::istream(buf)
  {
  }

  int istream::read_i32()
  {
    int value;

    auto c = get();

    if ( c >= 0xe0  && c <= 0xff )
    {
      value = char(c);
    }
    else if ( c >= 0 && c <= 0x7f )
    {
      value = c;
    }
    else if ( c == 0xcc )
    {
      value = read<unsigned char>();
    }
    else if ( c == 0xd1 )
    {
      value = read<short>();
    }
    else if ( c == 0xcd )
    {
      value = read<unsigned short>();
    }
    else if ( c == 0xd2 )
    {
      value = read<int>();
    }
    else if ( c == 0xce )
    {
      auto v = read<unsigned int>();
      if ( v < std::numeric_limits<int>::max() )
      {
        value = v;
      }
      else
      {
        std::cerr << "read_i32 failed c=" << c << std::endl;
        setstate(std::ios_base::failbit);
      }
    }
    else
    {
      std::cerr << "read_i32 failed c=" << c << std::endl;
      setstate(std::ios_base::failbit);
    }

    return value;
  }

  unsigned int istream::read_u32()
  {
    unsigned int value;

    auto c = get();

    if ( c >= 0xe0  && c <= 0xff )
    {
      std::cerr << "read_u32 failed c=" << c << std::endl;
      setstate(std::ios_base::failbit);
    }
    else if ( c >= 0 && c <= 0x7f )
    {
      value = c;
    }
    else if ( c == 0xcc )
    {
      value = read<unsigned char>();
    }
    else if ( c == 0xd1 )
    {
      auto v = read<short>();
      if ( v > 0 )
      {
        value = v;
      }
      else
      {
        std::cerr << "read_u32 failed c=" << c << std::endl;
        setstate(std::ios_base::failbit);
      }
    }
    else if ( c == 0xcd )
    {
      value = read<unsigned short>();
    }
    else if ( c == 0xd2 )
    {
      auto v = read<int>();
      if ( v > 0 )
      {
        value = v;
      }
      else
      {
        std::cerr << "read_u32 failed c=" << c << std::endl;
        setstate(std::ios_base::failbit);
      }
    }
    else if ( c == 0xce )
    {
      value = read<unsigned int>();
    }
    else
    {
      std::cerr << "read_u32 failed c=" << c << std::endl;
      setstate(std::ios_base::failbit);
    }

    return value;
  }

  bool istream::read_bool()
  {
    bool value;
    
    auto c = get();

    if ( c == 0xc2 )
    {
      value = false;
    }  
    else if ( c == 0xc3 )
    {
      value = true;
    }
    else
    {
      setstate(std::ios_base::failbit);
    }

    return value;
  }

  void istream::read_string(std::string& value)
  {
    auto c = get();

    if ( c >= 0xa0 && c <= 0xbf )
    {
      read_string(value, c & 0x1f);
    }
    else if ( c == 0xd9 )
    {
      read_string(value, read<unsigned char>());
    }
    else if ( c == 0xda )
    {
      read_string(value, read<unsigned short>());
    }
    else if ( c == 0xdb )
    {
      read_string(value, read<unsigned int>());
    }
    else
    {
      setstate(std::ios_base::failbit);
    }
  }

  void istream::read_string(std::string& value, std::size_t len)
  {
    value.resize(len);
    std::istream::read(&value[0], len);
  }
}
