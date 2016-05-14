// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2014
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __musicbox__kvstore_h__
#define __musicbox__kvstore_h__

// ----------------------------------------------------------------------------
#include <kchashdb.h>

// ----------------------------------------------------------------------------
#include <msgpack/istream.h>
#include <msgpack/ostream.h>

// ----------------------------------------------------------------------------
namespace musicbox
{
  class kvstore_impl
  {
  public:
    kvstore_impl(const std::string& filename)
    {
      using namespace kyotocabinet;

      if (!db_.open(filename, HashDB::OWRITER | HashDB::OCREATE)) {
        throw std::runtime_error("kvstore open error");
      }
    }
  public:
    int64_t increment(const std::string& key, int64_t num, int64_t orig)
    {
      return db_.increment(key, num, orig);
    }
  public:
    template<typename T>
    bool get(const std::string& key, T& value)
    {
      std::string sbuf;

      if ( db_.get(key, &sbuf) )
      {
        std::stringbuf buf(sbuf);
        msgpack::istream is(&buf);

        if ( is >> value )
        {
          return true;
        }
        else
        {
          // TODO: error.
          std::cout << "kvstore get error" << std::endl;
        }
      }
      else
      {
        return false;
      }
    }
  public:
    template<typename T>
    bool set(const std::string& key, const T& value)
    {
      std::stringbuf buf;
      msgpack::ostream os(&buf);

      os << value;

      return db_.set(key, buf.str());
    }
  public:
    void each(
      std::function<bool(const std::string&)> key_match,
      std::function<bool(msgpack::istream&)> value_cb
    )
    {
      kyotocabinet::DB::Cursor* cur = db_.cursor();

      cur->jump();

      std::string key;

      while ( cur->get_key(&key, false) )
      {
        if ( key_match(key) )
        {
          std::string value;

          if ( cur->get_value(&value) )
          {
            std::stringbuf buf(value);
            msgpack::istream is(&buf);

            bool continue_ = value_cb(is);

            if ( !continue_ ) {
              break;
            }
          }
        }
        cur->step();
      }

      delete cur;
    }
  private:
    kyotocabinet::HashDB db_;
  };

  class kvstore
  {
  public:
    using instance_ptr = std::shared_ptr<kvstore_impl>;
  public:
    kvstore()
    {
      assert(instance_);
    }
  public:
    int64_t increment(const std::string& key, int64_t num, int64_t orig=0)
    {
      assert(instance_);
      return instance_->increment(key, num, orig);
    }
  public:
    template<typename T>
    bool get(const std::string& key, T& value)
    {
      assert(instance_);
      return instance_->get<T>(key, value);
    }
  public:
    template<typename T>
    bool set(const std::string& key, T value)
    {
      assert(instance_);
      instance_->set(key, std::forward<T>(value));
    }
  public:
    void each(
      std::function<bool(const std::string&)> key_match,
      std::function<bool(msgpack::istream&)> value_cb
    )
    {
      assert(instance_);
      instance_->each(key_match, value_cb);
    }
  public:

  public:
    static void start(const std::string& filename);
  private:
    static instance_ptr instance_;
  };
}

// ----------------------------------------------------------------------------
#endif