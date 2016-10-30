// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2014
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include "kvstore.h"

// ----------------------------------------------------------------------------
namespace musciteer
{
  kvstore::instance_ptr kvstore::instance_;

  void kvstore::start(const std::string& filename)
  {
    assert(!instance_);
    instance_.reset(new kvstore_impl(filename));
  }

  void kvstore::stop()
  {
    assert(instance_);
    instance_.reset();
  }
}