// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __spotify_web__track_importer_h__
#define __spotify_web__track_importer_h__

// ----------------------------------------------------------------------------
#include "../dm/track.h"
#include "../dm/tracks.h"
#include "../dm/artist.h"
#include "../dm/artists.h"
#include "../dm/album.h"
#include "../dm/albums.h"
#include "../dm/album_cover.h"

// ----------------------------------------------------------------------------
#include "http_client.h"

// ----------------------------------------------------------------------------
namespace spotify_web
{
  class track_importer
  {
  public:
    track_importer(http_client& http_client) : http_client_(http_client)
    {
    }
  public:
    std::string import_track(const std::string& url)
    {
      auto track_json = http_client_.get_json(url);

      if ( !track_json.is_object() ) {
        throw std::runtime_error("track json not an object");
      }

      return import_track(track_json);
    }
  public:
    std::string import_track(json& track_json)
    {
      if ( !track_json.is_object() ) {
        throw std::runtime_error("track json not an object");
      }

      //std::cout << "track title=" << track_json["name"] << std::endl;
      //std::cout << track_json << std::endl;

      auto album_json = track_json["album"];
      auto artists_json = track_json["artists"];
      auto album_artist = resolve_album_artist(album_json);

      if ( album_artist.id_is_null() ) {
        throw std::runtime_error("album artist id is empty");
      }

      //std::cout << album_json << std::endl;
      //std::cout << "album_artist id='" << album_artist.id() << "', name='" << album_artist.name() << "'" << std::endl;

      auto album = resolve_album(album_artist, album_json);

      //std::cout << "album id='" << album.id() << "', title='" << album.title() << "'" << std::endl;

      if ( album.id_is_null() ) {
        throw std::runtime_error("album id is empty");
      }

      auto track = musciteer::dm::track{};
      auto track_source = musciteer::dm::track_source{"spotify", track_json["uri"]};

      std::string track_title = track_json["name"];
      unsigned track_tn = track_json["track_number"];
      unsigned track_dn = track_json["disc_number"];
      unsigned track_duration = track_json["duration_ms"];

      album.tracks_each([&](const musciteer::dm::track& rec)
      {
        if ( rec.disc_number() == track_dn && rec.track_number() == track_tn )
        {
          track = rec;
          return false;
        }
        else
        {
          return true;
        }
      });

      if ( track.id_is_null() )
      {
        track = tracks_.create_track();

        track.title(track_title);
        track.track_number(track_tn);
        track.disc_number(track_dn);
        track.duration(track_duration);
        track.album(album);

        for ( auto artist_json : artists_json )
        {
          auto track_artist = musciteer::dm::artist{};

          std::string track_artist_name = artist_json["name"];

          artists_.each([&](const musciteer::dm::artist& rec)
          {
            if ( rec.name() == track_artist_name )
            {
              track_artist = rec;
              return false;
            }
            else
            {
              return true;
            }
          });

          if ( track_artist.id_is_null() )
          {
            //std::cout << "create artist (track) name=" << track_artist_name << std::endl;
            track_artist = artists_.create_artist(track_artist_name);
            artists_.save(track_artist);
          }

          track.artists_add(track_artist);
        }

        album.tracks_add(track);
      }
      else
      {
        std::cout << "track found" << std::endl;
      }

      track.sources_add(track_source);

      artists_.save(album_artist);
      albums_.save(album);
      tracks_.save(track);

      return track.id();
    }
  private:
    musciteer::dm::artist resolve_album_artist(const json& album_json)
    {
      auto album_artist = musciteer::dm::artist{};

      if ( !album_json.is_object() ) {
        throw std::runtime_error("album json not an object");
      }

      auto details_json = http_client_.get_json(album_json["href"]);

      if ( !details_json["artists"].is_array() ) {
        throw std::runtime_error("album artists json not an array");
      }

      auto artist_json = details_json["artists"][0];

      if ( !artist_json.is_object() ) {
        throw std::runtime_error("artist json not an object");
      }

      std::string artist_name = artist_json["name"];

      artists_.each([&](const musciteer::dm::artist& rec)
      {
        if ( rec.name() == artist_name )
        {
          album_artist = rec;
          return false;
        }
        else
        {
          return true;
        }
      });

      if ( album_artist.id_is_null() )
      {
        //std::cout << "create artist (album) name=" << artist_name << std::endl;
        album_artist = artists_.create_artist(artist_name);
        artists_.save(album_artist);
      }

      return album_artist;
    }
  private:
    musciteer::dm::album resolve_album(musciteer::dm::artist& artist, const json& album_json)
    {
      auto album = musciteer::dm::album{};

      std::string album_title = album_json["name"];
      std::string album_alt_id = album_json["uri"];

      artist.albums_each([&](const musciteer::dm::album& rec)
      {
        if ( rec.title() == album_title )
        {
          album = rec;
          return false;
        }
        else
        {
          return true;
        }
      });

      if ( album.id_is_null() )
      {
        album = albums_.create_album();

        album.title(album_title);
        album.artist(artist);
        album.alt_ids_add(album_alt_id);

        auto images_json = album_json["images"];

        if ( !images_json.is_array() ) {
          throw std::runtime_error("album images json not an array");
        }

        auto album_cover = http_get_image(images_json[0]["url"]);
        auto kvstore = musciteer::kvstore();

        kvstore.set(album.id()+"/cover", album_cover);

        artist.albums_add(album);
      }

      return album;
    }
  private:
    musciteer::dm::album_cover http_get_image(const std::string& url)
    {
      auto cover = musciteer::dm::album_cover();

      http_client_.get(url, [&](http::response_environment& response)
      {
        std::string content_type;
        std::string content_length_s;
        std::string content;

        if ( !response.get_header("content-type", content_type) ) {
          throw std::runtime_error("no content-type header");
        }

        if ( !response.get_header("content-length", content_length_s) ) {
          throw std::runtime_error("no content-length header");
        }

        auto pos = std::size_t{0};
        auto len = std::stoul(content_length_s, &pos);

        for ( size_t i=0; i<len; ++i) {
          content += response.is.get();
        }

        cover.mime_type(content_type);
        cover.data(reinterpret_cast<const unsigned char*>(content.data()), content.length());
      });

      return cover;
    }
  private:
    http_client& http_client_;
  private:
    musciteer::dm::artists artists_;
    musciteer::dm::albums albums_;
    musciteer::dm::tracks tracks_;
  };
}

// ----------------------------------------------------------------------------
#endif