// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__dm__replay_gain_value_h__
#define __musciteer__dm__replay_gain_value_h__

// ----------------------------------------------------------------------------
#include <msgpack/istream.h>
#include <msgpack/ostream.h>

// ----------------------------------------------------------------------------
#include <string>
#include <limits>
#include <stdexcept>

// ----------------------------------------------------------------------------
namespace musciteer
{
  namespace dm
  {
    class replay_gain_value
    {
    public:
      replay_gain_value() : value_(std::numeric_limits<float>::min())
      {
      }
    public:
      replay_gain_value(float value) : value_(value)
      {
        check_value();
      }
    public:
      replay_gain_value(const std::string& s)
      {
        value_ = std::stof(s);
        check_value();
      }
    public:
      operator bool() const
      {
        return is_valid();
      }
    public:
      float value() const
      {
        if ( is_valid() )
        {
          return value_;
        }
        else
        {
          throw std::runtime_error("replay_gain_value is undefined");
        }
      }
    public:
      void value(float value)
      {
        value_ = value;
        check_value();
      }
    public:
      void value(const std::string& s)
      {
        value_ = std::stof(s);
        check_value();
      }
    public:
      void read(msgpack::istream& is)
      {
        is >> value_;
      }
    public:
      void write(msgpack::ostream& os) const
      {
        os << value_;
      }
    private:
      bool is_valid() const
      {
        return value_ != std::numeric_limits<float>::min();
      }
    private:
      void check_value()
      {
        if ( !is_valid() ) {
          throw std::runtime_error("replay_gain_value is invalid");
        }
      }
    private:
      float value_;
    };

    inline msgpack::istream& operator>>(msgpack::istream& is, replay_gain_value& value)
    {
      value.read(is);
      return is;
    }

    inline msgpack::ostream& operator<<(msgpack::ostream& os, const replay_gain_value& value)
    {
      value.write(os);
      return os;
    }
  }
}

// ----------------------------------------------------------------------------
#endif
