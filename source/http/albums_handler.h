// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http_server__albums_handler_h__
#define __http_server__albums_handler_h__

// ----------------------------------------------------------------------------
#include "api.h"
#include "api_handler_base.h"

// ----------------------------------------------------------------------------
#include "../flac/metadata.h"

// ----------------------------------------------------------------------------
#include "../dm/albums.h"
#include "../dm/album_cover.h"

// ----------------------------------------------------------------------------
class albums_handler : public api_handler_base
{
public:
  using api_handler_base::api_handler_base;
public:
  void call(const std::string& path)
  {
    auto method = env.method();

    std::smatch match;

    if ( std::regex_match(path, match, route_re_) )
    {
      if ( match[0].length() == 0 )
      {
        if ( method == http::method::get ) {
          get_albums(path);
        }
        else {
          method_not_allowed();
        }
      }
      else if ( match[0] == "/" && match[1].length() == 0 )
      {
        if ( method == http::method::get ) {
          get_albums(path);
        }
        else {
          method_not_allowed();
        }
      }
      else if ( match[0].length() > 0 && match[1].length() > 0 )
      {
        if ( match[2].length() == 0 )
        {
          if ( method == http::method::get ) {
            get_album(match[1]);
          }
          else if ( method == http::method::delete_ ) {
            delete_album(match[1]);
          }
          else {
            method_not_allowed();
          }
        }
        else if ( match[2] == "tracks" )
        {
          if ( method == http::method::get ) {
            get_album_tracks(match[1]);
          }
          else {
            method_not_allowed();
          }
        }
        else if ( match[2] == "cover" )
        {
          if ( method == http::method::get ) {
            get_album_cover(match[1]);
          }
          else {
            method_not_allowed();
          }
        }
        else {
          not_found();
        }
      }
      else {
        not_found();
      }
    }
    else {
      not_found();
    }
  }
protected:
  void get_albums(const std::string& path)
  {
    auto albums = musciteer::dm::albums();

    json j = json::array();

    albums.each([&](musciteer::dm::album& album) {
      j.push_back(musciteer::to_json(album));
      return true;
    });

    ok(j);
  }
private:
  void get_album(const std::string& id)
  {
    auto albums = musciteer::dm::albums();
    auto album = albums.find_by_id(id);

    if ( !album.id_is_null() )
    {
      ok(musciteer::to_json(album, true, true));
    }
    else
    {
      not_found();
    }
  }
private:
  void delete_album(const std::string& id)
  {
    if ( id.length() != 6 )
    {
      unprocessable_entity();
      return;
    }

    if ( !(id[0] == 'a' && id[1] == 'l') )
    {
      unprocessable_entity();
      return;
    }

    auto albums = musciteer::dm::albums();
    auto album = albums.find_by_id(id);

    if ( !album.id_is_null() )
    {
      auto tracks = musciteer::dm::tracks();

      album.tracks_each([&](const musciteer::dm::track& t) {
        tracks.remove(t);
      });

      albums.remove(album);

      ok();
    }
    else
    {
      not_found();
    }
  }
private:
  void get_album_tracks(const std::string& id)
  {
    auto albums = musciteer::dm::albums();
    auto album = albums.find_by_id(id);

    if ( !album.id().empty() )
    {
      json j;

      album.tracks_each([&](const musciteer::dm::track& track)
      {
        if ( !track.id_is_null() ) {
          j.push_back(musciteer::to_json(track));
        }
      });

      ok(j);
    }
    else
    {
      not_found();
    }
  }
private:
  void get_album_cover(const std::string& id)
  {
    auto albums = musciteer::dm::albums();

    auto album = albums.find_by_id(id);
    auto found = false;

    if ( !album.id().empty() )
    {
      album.tracks_each([&](const musciteer::dm::track& track)
      {
        if ( !found )
        {
          try
          {
            auto track_source = track.sources_get("local");
            auto filename = track_source.uri();

            musciteer::flac::metadata_chain metadata;
            musciteer::flac::picture* picture_ptr = nullptr;

            metadata.on_picture(
              [&](musciteer::flac::picture& picture)
              {
                if ( picture.type == FLAC__STREAM_METADATA_PICTURE_TYPE_FRONT_COVER ) {
                  picture_ptr = &picture;
                }
                else if ( !picture_ptr && picture.type == FLAC__STREAM_METADATA_PICTURE_TYPE_MEDIA ) {
                  picture_ptr = &picture;
                }
              }
            );

            metadata.read(filename);

            if ( picture_ptr )
            {
              env.os << "HTTP/1.1 200 OK" << crlf
                << "Content-Type: " << picture_ptr->mime_type << crlf
                << "Content-Length: " << picture_ptr->data_length << crlf
                << "Cache-Control: max-age=86400" << crlf
                << crlf;

              env.os.write(reinterpret_cast<const char*>(picture_ptr->data), picture_ptr->data_length);

              found = true;
            }
          }
          catch(...)
          {
            std::cerr << "track id " << track.id() << " has no local source" << std::endl;
          }
        }
      });
    }

    if ( !found )
    {
      not_found();
    }
  }
};

// ----------------------------------------------------------------------------
#endif