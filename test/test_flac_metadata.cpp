// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2018
//
// ----------------------------------------------------------------------------
#include "../source/flac/metadata.h"

// ----------------------------------------------------------------------------
#include "doctest/doctest.h"

// ----------------------------------------------------------------------------
#include <iostream>

// ----------------------------------------------------------------------------
TEST_CASE("Read flac meta data")
{
  using musciteer::flac::metadata_chain;
  using musciteer::flac::vorbis_comment;
  using musciteer::flac::stream_info;
  using musciteer::flac::vorbis_comment_each_kv;

  metadata_chain metadata;

  bool stream_info_called = false;

  metadata.on_stream_info(
    [&](stream_info& stream_info)
    {
      CHECK(stream_info.bits_per_sample == 16u);
      stream_info_called = true;
    }
  );

  bool vorbis_comment_called = false;

  auto print_kv = [](std::string& key, std::string& value) {
    std::cout << "key='" << key << "', value = '" << value << "'" << std::endl;
  };

  metadata.on_vorbis_comment(
    [&](vorbis_comment& vorbis_comment)
    {
      vorbis_comment_each_kv(vorbis_comment, print_kv);
      vorbis_comment_called = true;
    }
  );

  CHECK(metadata.read("test/flac/tone16bit.flac"));
  CHECK(stream_info_called);
  CHECK(vorbis_comment_called);
}
