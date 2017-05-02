// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__migration_h__
#define __musciteer__migration_h__

// ----------------------------------------------------------------------------
#include "storage/kvstore.h"

// ----------------------------------------------------------------------------
#include "dm/albums.h"

// ----------------------------------------------------------------------------
#include "http/api.h"

// ----------------------------------------------------------------------------
#include "spotify_web/api.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/channel.h>

// ----------------------------------------------------------------------------
#include <regex>

// ----------------------------------------------------------------------------
namespace musciteer
{
  namespace migration
  {
    class migration_0_task : public dripcore::task
    {
    private:
      struct spotify_album
      {
        musciteer::dm::album dm;
        std::string          url;
      };
    public:
      migration_0_task() : kvstore_(musciteer::kvstore())
      {
      }
    public:
      void main() override
      {
        auto albums         = musciteer::dm::albums();
        auto spotify_albums = find_spotify_albums(albums);

        update_spotify_albums(spotify_albums, albums);

        // Update kvstore version.
        kvstore_.version(1);
      }
    private:
      std::vector<spotify_album> find_spotify_albums(musciteer::dm::albums& albums)
      {
        std::vector<spotify_album> spotify_albums;

        std::regex  spotify_album_re{"^spotify:album:(.*)"};
        std::smatch spotify_album_match;

        albums.each([&](const musciteer::dm::album& album)
        {
          for ( auto& alt_id : album.alt_ids() )
          {
            if ( std::regex_match(alt_id, spotify_album_match, spotify_album_re) )
            {
              auto id = std::string(spotify_album_match[1]);

              spotify_albums.emplace_back(spotify_album{album, "/v1/albums/" + id});
              break;
            }
          }
          return true;
        });

        return spotify_albums;
      }
    private:
      void update_spotify_albums(std::vector<spotify_album>& spotify_albums, musciteer::dm::albums& albums)
      {
        spotify_web::api spotify(this);

        for ( auto& album : spotify_albums )
        {
          json sp_album;

          spotify.get(album.url, sp_album);

          auto images_json = sp_album["images"];

          album.dm.cover_url(images_json[0]["url"]);
          albums.save(album.dm);

          // Remove the old cover image.
          kvstore_.remove(album.dm.id()+"/cover");
        }
      }
    private:
      musciteer::kvstore kvstore_;
    };

    template<class T>
    void run_migration_task()
    {
      dripcore::loop loop;

      loop.spawn<T>();
      loop.run();
    }

    void run(const std::string& database_filename)
    {
      auto kvstore = musciteer::kvstore();

      while ( kvstore.version() < 1 )
      {
        std::cout << "database " << database_filename << " running migration from version " << kvstore.version() << std::endl;

        switch ( kvstore.version() )
        {
          case 0:
            run_migration_task<migration_0_task>();
            break;
        }
      }

      std::cout << "database " << database_filename << " version is " << kvstore.version() << std::endl;
    }
  }
}

// ----------------------------------------------------------------------------
#endif
