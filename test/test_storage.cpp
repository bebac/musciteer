// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "../source/storage/kvstore.h"

// ----------------------------------------------------------------------------
#include "doctest/doctest.h"

// ----------------------------------------------------------------------------
#include <cstdio>

// ----------------------------------------------------------------------------
class TestStorageFixture
{
public:
  TestStorageFixture()
  {
    musciteer::kvstore::start("testdb");
    kvstore = musciteer::kvstore();
  }
public:
  ~TestStorageFixture()
  {
    musciteer::kvstore::stop();
    std::remove("testdb");
  }
protected:
  musciteer::kvstore kvstore;
};

// ----------------------------------------------------------------------------
TEST_CASE_FIXTURE(TestStorageFixture, "Storage")
{
  SUBCASE("set/get value")
  {
    kvstore.set("1", 1);

    int v = 0;

    CHECK(kvstore.get("1", v) == true);
    CHECK(v == 1);
  }

  SUBCASE("get nonexisting value returns false")
  {
    int v = 0;

    CHECK(kvstore.get("2", v) == false);
  }

  SUBCASE("replace value")
  {
    int v = 0;

    kvstore.set("1", 1);

    CHECK(kvstore.get("1", v) == true);
    CHECK(v == 1);

    kvstore.replace("1", 2);

    CHECK(kvstore.get("1", v) == true);
    CHECK(v == 2);
  }

  SUBCASE("remove value")
  {
    int v = 0;

    kvstore.set("1", 1);

    CHECK(kvstore.get("1", v) == true);
    CHECK(v == 1);

    kvstore.remove("1");

    CHECK(kvstore.get("1", v) == false);
  }

  SUBCASE("each")
  {
    std::map<std::string, int> items {
      { "1", 1 },
      { "2", 2 },
      { "3", 3 }
    };

    for ( auto& item : items ) {
      kvstore.set(item.first, item.second);
    }

    kvstore.each(
      [&](const std::string& key){
        return true;
      },
      [&](msgpack::istream& is)
      {
        int v = 0;

        is >> v;

        CHECK(is);

        switch ( v )
        {
          case 1: items.erase("1"); break;
          case 2: items.erase("2"); break;
          case 3: items.erase("3"); break;
        };

        return true;
      }
    );

    CHECK(items.empty());
  }
}
