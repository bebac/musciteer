// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "../dm/album.h"
#include "../dm/albums.h"
#include "../dm/track.h"

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"

// ----------------------------------------------------------------------------
std::string shell_escape(const std::string& s)
{
  std::string result;

  result.reserve(s.size());

  for ( auto c : s )
  {
    switch ( c )
    {
      case ' ':  result += "\\ "; break;
      case '$':  result += "\\$"; break;
      case '!':  result += "\\!"; break;
      case '(':  result += "\\("; break;
      case ')':  result += "\\)"; break;
      case '&':  result += "\\&"; break;
      case '\'': result += "\\'"; break;
      default:
        result += c;
    }
  }

  return result;
}

// ----------------------------------------------------------------------------
void do_album(const musciteer::dm::album& album)
{
  std::vector<std::string> filenames;

  album.tracks_each([&](const musciteer::dm::track& track)
  {
    for ( auto& source : track.sources() )
    {
      if ( source.name() == "local" ) {
        filenames.push_back(source.uri());
      }
    }
  });

  std::cout << album.artist().name() << " - " << album.title() << std::endl;

  if ( filenames.size() > 0 )
  {
    std::stringstream sh;

    sh << "metaflac --add-replay-gain";

    for ( auto& filename : filenames )
    {
      sh << " " << shell_escape(filename);
    }

    auto cmd = sh.str();
    auto res = system(cmd.c_str());

    std::cout << " -> " << res << std::endl;
  }
  else
  {
    std::cout << " -> no local sources" << std::endl;
  }
}

// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  musciteer::kvstore::start(".musciteerdb");

  auto albums = musciteer::dm::albums();

  albums.each([&](const musciteer::dm::album& album)
  {
    do_album(album);
    return true;
  });

  return 0;
}
