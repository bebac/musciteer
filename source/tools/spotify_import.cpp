// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "../dm/track.h"
#include "../dm/tracks.h"
#include "../dm/artist.h"
#include "../dm/artists.h"
#include "../dm/album.h"
#include "../dm/albums.h"
#include "../dm/album_cover.h"

// ----------------------------------------------------------------------------
#include "../spotify_web/http_client.h"
#include "../spotify_web/track_importer.h"

// ----------------------------------------------------------------------------
#include <dripcore/loop.h>
#include <dripcore/task.h>
#include <dripcore/channel.h>

// ----------------------------------------------------------------------------
#include <json.h>

// ----------------------------------------------------------------------------
#include <openssl/conf.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// spotify_import is sort of a play ground to get started on interacting with
// the spotify web api. It will quickly get quite complicated as the spotify
// web api only support https and oauth for authentication. Luckily we don't
// need to authenticate to get started.

// ----------------------------------------------------------------------------
void print_track(const std::string& track_id)
{
  musciteer::dm::tracks tracks;

  auto track = tracks.find_by_id(track_id);

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
}

// ----------------------------------------------------------------------------
class spotify_importer : public dripcore::task
{
public:
  spotify_importer(std::vector<std::string> urls) : http_client_(this), urls_(urls)
  {
  }
private:
  void main() override
  {
    spotify_web::track_importer importer(http_client_);

    for ( const auto& url : urls_ )
    {
      auto j = http_client_.get_json(url);

      if ( j["type"] == "track" )
      {
        auto track_id = importer.import_track(j);
        print_track(track_id);
      }
      else if ( j["type"] == "album" )
      {
        for ( auto item : j["tracks"]["items"] )
        {
          std::string track_url = item["href"];
          auto track_id = importer.import_track(track_url);
          print_track(track_id);
        }
      }
      else
      {
        throw std::runtime_error("don't know how to import that");
      }
    }
  }
private:
  spotify_web::http_client http_client_;
private:
  std::vector<std::string> urls_;
};

// ----------------------------------------------------------------------------
void init_openssl_library(void)
{
  SSL_library_init();

  SSL_load_error_strings();

  /* ERR_load_crypto_strings(); */

  OPENSSL_config(NULL);

  /* Include <openssl/opensslconf.h> to get this define */
#if defined (OPENSSL_THREADS)
  fprintf(stdout, "Warning: thread locking is not implemented\n");
#endif
}

// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  dripcore::loop loop;

  init_openssl_library();

  musciteer::kvstore::start(".musciteerdb");

  // Import spotify track or album.
  //
  // Examples:
  // spotify_import /v1/albums/51ogMPThFURWcqZGxVA1uT
  // spotify_import /v1/tracks/6dben05JiKn09x31h1JgOW
  // spotify_import -f urls.txt

  std::vector<std::string> urls;

  if ( std::string(argv[1]) == "-f" )
  {
    std::ifstream file(argv[2]);

    while ( !file.eof() )
    {
      std::string path;
      std::getline(file, path);
      urls.push_back("https://api.spotify.com" + path);
    }
  }
  else
  {
    urls.push_back("https://api.spotify.com" + std::string(argv[1]));
  }

  loop.spawn<spotify_importer>(std::move(urls));
  loop.run();

  return 0;
}
