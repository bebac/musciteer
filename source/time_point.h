// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include <msgpack/istream.h>
#include <msgpack/ostream.h>

// ----------------------------------------------------------------------------
#include <chrono>

// ----------------------------------------------------------------------------
namespace musciteer
{
  using clock = std::chrono::system_clock;

  namespace
  {
    const unsigned char msgpack_time_point = 1;
  }
}

inline msgpack::istream& operator>>(msgpack::istream& is, musciteer::clock::time_point& value)
{
  auto c = is.get();

  if ( c == 0xd7 )
  {
    c = is.get();

    if ( c == musciteer::msgpack_time_point )
    {
      value = musciteer::clock::time_point(std::chrono::nanoseconds(is.read<long long>()));
    }
    else
    {
      is.setstate(std::ios_base::failbit);
    }
  }
  else
  {
    is.setstate(std::ios_base::failbit);
  }

  return is;
}

inline msgpack::ostream& operator<<(msgpack::ostream& os, musciteer::clock::time_point& value)
{
  auto d = std::chrono::duration_cast<std::chrono::nanoseconds>(value.time_since_epoch());

  os.put(0xd7);
  os.put(musciteer::msgpack_time_point, d.count());

  return os;
}


