// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "../dm/album.h"
#include "../dm/albums.h"
#include "../dm/artist.h"
#include "../dm/artists.h"
#include "../dm/track.h"
#include "../dm/tracks.h"

// ----------------------------------------------------------------------------
#include "../storage/base62.h"
#include "../storage/kvstore.h"

// ----------------------------------------------------------------------------
#include "../file_system.h"
#include "../flac_file_importer.h"

// ----------------------------------------------------------------------------
void print_tracks()
{
  auto tracks = musicbox::dm::tracks();

  tracks.each([](musicbox::dm::track& track)
  {
    std::cout << "---" << std::endl;
    std::cout << "track id       : " << track.id() << std::endl;
    std::cout << "track title    : " << track.title() << std::endl;
    std::cout << "track duration : " << track.duration() << std::endl;
    std::cout << "track artists  : " << track.artists().size() << std::endl;
    for ( auto artist : track.artists() )
    {
      std::cout << "  artist name : " << artist.name() << std::endl;
    }
    std::cout << "track album    : " << track.album().title() << std::endl;
    std::cout << "track sources  : " << track.sources().size() << std::endl;
    for ( auto source : track.sources() )
    {
      std::cout << "  source name : " << source.name() << std::endl;
      std::cout << "  source uri  : " << source.uri() << std::endl;
    }

    return true;
  });
}

// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  //musicbox::kvstore::start("testdb");
  musicbox::kvstore::start(".mboxd");

  auto dir = file_system::directory{argv[1]};

  dir.each_file([&](const std::string& filename)
  {
    if ( file_system::extension(filename) == "flac" )
    {
      std::cout << filename << std::endl;
      musicbox::flac_file_importer importer(filename);

      importer.run();
    }
  });

  return 0;
}
