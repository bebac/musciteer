// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "../source/player/player_ctpb_provider.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>

// ----------------------------------------------------------------------------
#include "doctest/doctest.h"

// ----------------------------------------------------------------------------
class TestContinuousPlaybackFixture
{
public:
  TestContinuousPlaybackFixture() : loop()
  {
    musciteer::kvstore::start(".musciteerdb");
  }
public:
  ~TestContinuousPlaybackFixture()
  {
    musciteer::kvstore::stop();
  }
protected:
  dripcore::loop loop;
};

// ----------------------------------------------------------------------------
TEST_CASE_FIXTURE(TestContinuousPlaybackFixture, "continuous-playback")
{
  SUBCASE("construct")
  {
    class test_task : public dripcore::task
    {
    private:
      void main() final
      {
        musciteer::player_ctpb_provider ctpb_provider(this);
        CHECK(ctpb_provider.is_loaded() == false);
      }
    };

    loop.spawn<test_task>();
    loop.run();
  }

  SUBCASE("load")
  {
    class test_task : public dripcore::task
    {
    private:
      void main() final
      {
        musciteer::player_ctpb_provider ctpb_provider(this);

        ctpb_provider.load("random");

        CHECK(ctpb_provider.is_loaded() == true);

        for ( int i=0; i<5; ++i )
        {
          auto start = std::chrono::steady_clock::now();
          auto track = ctpb_provider.get_track();
          auto end   = std::chrono::steady_clock::now();

          CHECK(track.id_is_null() == false);

#if 1
          std::cout
            << "test_task - get_track id='" << track.id() << "' took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms"
            << std::endl;
#endif

          // Really need a sleep to simulate player task.
          //for ( int i=0; i<1000000; ++i ) {
          //  yield(true);
          //}
        }

        ctpb_provider.unload();
      }
    };

    loop.spawn<test_task>();
    loop.run();
  }
}