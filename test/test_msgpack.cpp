// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "../source/time_point.h"

// ----------------------------------------------------------------------------
#include <msgpack/istream.h>
#include <msgpack/ostream.h>
#include <msgpack/map.h>
#include <msgpack/array.h>

// ----------------------------------------------------------------------------
#include <sstream>
#include <vector>
#include <limits>
#include <chrono>

// ----------------------------------------------------------------------------
#include "doctest/doctest.h"

// ----------------------------------------------------------------------------
namespace msgpack
{
  namespace test
  {
    class object_v1
    {
    public:
      object_v1() : member1(), member2(), member3(0)
      {
      }
    public:
      void write(msgpack::ostream& os) const
      {
        msgpack::map map{3};

        os << map
          << 1 << member1
          << 2 << member2
          << 3 << member3
          ;
      }
    public:
      void read(msgpack::istream& is)
      {
        msgpack::map map;

        if ( is >> map )
        {
          for ( size_t i=0; i<map.size(); i++ )
          {
            unsigned key;

            if ( is >> key )
            {
              switch ( key )
              {
                case 1: is >> member1; break;
                case 2: is >> member2; break;
                case 3: is >> member3; break;
                default:
                  is >> msgpack::skip;
                  break;
              }
            }
            else
            {
              throw std::runtime_error("failed to read test object");
            }
          }
        }
        else
        {
          throw std::runtime_error("failed to read test object");
        }
      }
    public:
      std::string member1;
      std::string member2;
      unsigned    member3;
    };

    inline msgpack::istream& operator>>(msgpack::istream& is, object_v1& value)
    {
      value.read(is);
      return is;
    }

    inline msgpack::ostream& operator<<(msgpack::ostream& os, const object_v1& value)
    {
      value.write(os);
      return os;
    }

    class object_v2
    {
    public:
      object_v2() : member1(), member3(0)
      {
      }
    public:
      void write(msgpack::ostream& os) const
      {
        msgpack::map map{2};

        os << map
          << 1 << member1
          << 3 << member3
          ;
      }
    public:
      void read(msgpack::istream& is)
      {
        msgpack::map map;

        if ( is >> map )
        {
          for ( size_t i=0; i<map.size(); i++ )
          {
            unsigned key;

            if ( is >> key )
            {
              switch ( key )
              {
                case 1: is >> member1; break;
                case 3: is >> member3; break;
                default:
                  is >> msgpack::skip;
                  break;
              }
            }
            else
            {
              throw std::runtime_error("failed to read test object");
            }
          }
        }
        else
        {
          throw std::runtime_error("failed to read test object");
        }
      }
    public:
      std::string member1;
      unsigned    member3;
    };

    inline msgpack::istream& operator>>(msgpack::istream& is, object_v2& value)
    {
      value.read(is);
      return is;
    }

    inline msgpack::ostream& operator<<(msgpack::ostream& os, const object_v2& value)
    {
      value.write(os);
      return os;
    }

  }
}

// ----------------------------------------------------------------------------
TEST_CASE("msgpack streaming")
{
  std::stringbuf sbuf;

  msgpack::ostream os(&sbuf);
  msgpack::istream is(&sbuf);

  SUBCASE("positive integers reads as unsigned integers")
  {
    int values[] = { 0, 1, 127, 128, 255, 256, 4095, 4096, std::numeric_limits<int>::max() };

    // All ints fits into an unsigned.
    for ( auto v : values )
    {
      unsigned int x = 0;

      os << v;
      is >> x;

      CHECK(static_cast<int>(x) == v);
    }
  }

  SUBCASE("fails to read negative integer into unsigned")
  {
    unsigned int x = 0;

    os << -1;
    is >> x;

    CHECK(is.fail());
  }

  SUBCASE("it streams int64")
  {
    long long values[] = { std::numeric_limits<long long>::min(), -1, 0, 1, 127, 128, 255, 256, 4095, 4096, std::numeric_limits<long long>::max() };

    for ( auto v : values )
    {
      long long x = 0;

      os << v;
      is >> x;

      CHECK(x == v);
    }
  }

  SUBCASE("it streams uint64")
  {
    unsigned long long values[] = { 0, 1, 127, 128, 255, 256, 4095, 4096, std::numeric_limits<unsigned long long>::max() };

    for ( auto v : values )
    {
      unsigned long long x = 0;

      os << v;
      is >> x;

      CHECK(x == v);
    }
  }

  SUBCASE("it streams bool")
  {
    bool x;

    os << true << false;

    is >> x;
    CHECK(x == true);

    is >> x;
    CHECK(x == false);

    is >> x;
    CHECK(is.eof());
  }

  SUBCASE("it streams time_pint")
  {
    auto now = musciteer::clock::now();
    auto x = musciteer::clock::time_point();

    os << now;
    is >> x;

    CHECK(x == now);
  }

  SUBCASE("it can skip positive fixint")
  {
    unsigned marker = 0xa5a5;

    os << 0x00 << marker;

    unsigned x;

    is >> msgpack::skip >> x;
    CHECK(x == marker);
  }

  SUBCASE("it can skip intX")
  {
    unsigned marker = 0xa5a5;

    os
      << std::numeric_limits<char>::max()
      << std::numeric_limits<short>::max()
      << std::numeric_limits<int>::max()
      << std::numeric_limits<long long>::max()
      << marker;

    unsigned x = 0;

    is >> msgpack::skip >> msgpack::skip >> msgpack::skip >> msgpack::skip >> x;
    CHECK(x == marker);
  }

  SUBCASE("it can skip uintX")
  {
    unsigned marker = 0xa5a5;

    os
      << std::numeric_limits<unsigned char>::max()
      << std::numeric_limits<unsigned short>::max()
      << std::numeric_limits<unsigned int>::max()
      << std::numeric_limits<unsigned long long>::max()
      << marker;

    unsigned x = 0;

    is >> msgpack::skip >> msgpack::skip >> msgpack::skip >> msgpack::skip >> x;
    CHECK(x == marker);
  }

  SUBCASE("it can skip true, false")
  {
    unsigned marker = 0xa5a5;

    os << true << false << marker;

    unsigned x;

    is >> msgpack::skip >> msgpack::skip >> x;
    CHECK(x == marker);
  }

  SUBCASE("it can skip fixstr")
  {
    unsigned marker = 0xa5a5;

    os << "test test test" << marker;

    unsigned x = 0;

    is >> msgpack::skip >> x;
    CHECK(x == marker);

    os << std::string(std::numeric_limits<unsigned char>::max(), 'x') << marker;
  }

  SUBCASE("it can skip strX")
  {
    unsigned marker = 0xa5a5;

    os
      << std::string(std::numeric_limits<unsigned char>::max(), 'x')
      << std::string(std::numeric_limits<unsigned char>::max()+1, 'x')
      << std::string(std::numeric_limits<unsigned short>::max()+1, 'x')
      << marker;

    unsigned x = 0;

    is >> msgpack::skip >> msgpack::skip >> msgpack::skip >> x;
    CHECK(x == marker);
  }

  SUBCASE("it can skip array")
  {
    unsigned marker = 0xa5a5;

    os << std::vector<int>{1, 2 ,3} << marker;

    unsigned x = 0;

    is >> msgpack::skip >> x;
    CHECK(x == marker);
  }

  SUBCASE("it can skip map")
  {
    unsigned marker = 0xa5a5;

    os << std::map<int, int>{ { 1, 1 }, { 2, 2 } } << marker;

    unsigned x = 0;

    is >> msgpack::skip >> x;
    CHECK(x == marker);
  }

  SUBCASE("it can skip a timepoint")
  {
    unsigned marker = 0xa5a5;

    auto now = musciteer::clock::now();
    os << now << marker;

    unsigned x = 0;

    is >> msgpack::skip >> x;
    CHECK(x == marker);
  }

  SUBCASE("it streams custom type")
  {
    msgpack::test::object_v1 o;

    o.member1 = "member 1";
    o.member2 = "member 2";
    o.member3 = 7;

    os << o;

    msgpack::test::object_v1 x;

    is >> x;

    CHECK(x.member1 == "member 1");
    CHECK(x.member2 == "member 2");
    CHECK(x.member3 == 7u);
  }

  SUBCASE("it streams custom type with removed member")
  {
    msgpack::test::object_v1 o;

    o.member1 = "member 1";
    o.member2 = "member 2";
    o.member3 = 7;

    os << o;

    msgpack::test::object_v2 x;

    is >> x;

    CHECK(x.member1 == "member 1");
    CHECK(x.member3 == 7u);
  }
}