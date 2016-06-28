// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musicbox__dm__source_local_h__
#define __musicbox__dm__source_local_h__

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  namespace dm
  {
    class source_local
    {
      using directory_list = std::vector<std::string>;
    public:
      source_local() : kvstore_(musicbox::kvstore())
      {
      }
    public:
      directory_list directories()
      {
        directory_list value;

        if ( kvstore_.get(source_local_directories_key, value) )
        {
          return value;
        }
        else
        {
          return value;
        }
      }
    public:
      void directories(directory_list&& value)
      {
        kvstore_.set(source_local_directories_key, value);
      }
    private:
      kvstore kvstore_;
    private:
      static constexpr const char* source_local_directories_key = "__source_local_directories__";
    };
  }
}

// ----------------------------------------------------------------------------
#endif
