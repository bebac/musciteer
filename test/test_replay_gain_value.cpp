// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "../source/dm/replay_gain_value.h"

// ----------------------------------------------------------------------------
#include "doctest/doctest.h"

// ----------------------------------------------------------------------------
TEST_CASE("Default constructed replay_gain_value is undefined")
{
  musciteer::dm::replay_gain_value rg;

  CHECK(!rg);

  CHECK_THROWS(
    rg.value()
  );
}

// ----------------------------------------------------------------------------
TEST_CASE("Constructing a replay_gain_value with float min throws")
{
  CHECK_THROWS(
    musciteer::dm::replay_gain_value rg(std::numeric_limits<float>::min())
  );
}

// ----------------------------------------------------------------------------
TEST_CASE("When constructed with a float replay_gain_value is defined and\
           contains value")
{
  musciteer::dm::replay_gain_value rg(0.0);

  CHECK(!!rg);
  CHECK(rg.value() == 0.0f);
}

// ----------------------------------------------------------------------------
TEST_CASE("When constructed with a string replay_gain_value is defined and\
           contains value")
{
  musciteer::dm::replay_gain_value rg1("+2.96 db");

  CHECK(!!rg1);
  CHECK(rg1.value() == 2.96f);

  musciteer::dm::replay_gain_value rg2("-0.96 db");

  CHECK(!!rg2);
  CHECK(rg2.value() == -0.96f);

  CHECK_THROWS(
    musciteer::dm::replay_gain_value rg("zzz");
  );
}

// ----------------------------------------------------------------------------
TEST_CASE("set replay_gain_value value")
{
  musciteer::dm::replay_gain_value rg;

  rg.value(1.0);
  CHECK(rg.value() == 1.0f);

  rg.value("-1.0");
  CHECK(rg.value() == -1.0f);

  CHECK_THROWS(
    rg.value("zzz");
  );

  CHECK_THROWS(
    rg.value(std::numeric_limits<float>::min());
  );
}
