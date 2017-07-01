// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __http__api_h__
#define __http__api_h__

// ----------------------------------------------------------------------------
#include "../dm/track.h"

// ----------------------------------------------------------------------------
#include <json.h>

// ----------------------------------------------------------------------------
namespace musciteer
{
  inline json to_json(const musciteer::dm::track_source& track_source)
  {
    json j = {
      { "name", track_source.name() },
      { "uri",  track_source.uri() }
    };

    json replay_gain;

    if ( auto& rg = track_source.rg_ref_loudness() ) {
      replay_gain["ref_loudness"] = static_cast<double>(rg.value()*100)/100;
    }

    if ( auto& rg = track_source.rg_track_gain() ) {
      replay_gain["track_gain"] = static_cast<double>(rg.value()*100)/100;
    }

    if ( auto& rg = track_source.rg_track_peak() ) {
      replay_gain["track_peak"] = rg.value();
    }

    if ( !replay_gain.is_null() ) {
      j["replay_gain"] = replay_gain;
    }

    return j;
  }

  inline json to_json(const musciteer::dm::track& track, bool brief = false)
  {
    json jtrack;
    json jartists;
    json jalbum;

    for ( auto& artist : track.artists() )
    {
      json j = {
        { "id",   artist.id() },
        { "name", artist.name() }
      };
      jartists.push_back(j);
    }

    const auto& album = track.album();

    jalbum = {
      { "id",    album.id() },
      { "title", album.title() }
    };

    if ( !brief )
    {
      auto cover_url = album.cover_url();

      if ( cover_url.empty() ) {
        cover_url = "/api/albums/"+album.id()+"/cover";
      }

      jalbum["cover"] = cover_url;
    }

    jtrack = {
      { "id", track.id() },
      { "title", track.title() },
      { "tn", track.track_number() },
      { "dn", track.disc_number() },
      { "duration", track.duration() },
      { "artists", jartists },
      { "album", jalbum },
      { "play_count", track.play_count() },
      { "skip_count", track.skip_count() }
    };

    if ( !brief )
    {
      json jsources;

      track.sources_each([&](const dm::track_source& source) {
        jsources.push_back(to_json(source));
      });

      jtrack["sources"] = jsources;
    }

    return jtrack;
  }

  inline json to_json(const musciteer::dm::album& album, bool include_artist = true, bool include_tracks = false)
  {
    json jalbum;

    auto artist    = album.artist();
    auto cover_url = album.cover_url();

    if ( cover_url.empty() ) {
      cover_url = "/api/albums/"+album.id()+"/cover";
    }

    jalbum = {
      { "id", album.id() },
      { "title", album.title() },
      //{ "tracks", album.track_ids() },
      { "cover", cover_url },
      { "alt_ids", album.alt_ids() }
    };

    if ( include_artist )
    {
      jalbum["artist"] = json{
        { "id", artist.id() },
        { "name", artist.name() }
      };
    }

    if ( include_tracks )
    {
      json jtracks;

      album.tracks_each([&](const musciteer::dm::track& track)
      {
        if ( !track.id_is_null() ) {
          jtracks.push_back(musciteer::to_json(track));
        }
      });

      jalbum["tracks"] = jtracks;
    }

    return jalbum;
  }

  inline json to_json(const musciteer::dm::artist& artist)
  {
    json jartist;
    json jalbums;

    artist.albums_each([&](const dm::album& album) {
      // Hmm there shouldn't really be any albums with no id.
      if ( album.id_is_null() ) {
        return;
      }
      // Add album.
      jalbums.push_back(to_json(album, false));
    });

    jartist = {
      { "id", artist.id() },
      { "name", artist.name() },
      { "albums", jalbums }
    };

    return jartist;
  }

}

// ----------------------------------------------------------------------------
#endif