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

  inline json to_json(const musciteer::dm::track& track)
  {
    json artists;

    for ( auto& artist : track.artists() )
    {
      json j = {
        { "id",   artist.id() },
        { "name", artist.name() }
      };
      artists.push_back(j);
    }

    const auto& album = track.album();

    json jalbum = {
      { "id",    album.id() },
      { "title", album.title() },
    };

    json sources;

    track.sources_each([&](const dm::track_source& source) {
      sources.push_back(to_json(source));
    });

    json t = {
      { "id", track.id() },
      { "title", track.title() },
      { "tn", track.track_number() },
      { "dn", track.disc_number() },
      { "duration", track.duration() },
      { "artists", artists },
      { "album", jalbum },
      { "play_count", track.play_count() },
      { "skip_count", track.skip_count() },
      { "sources", sources }
    };

    return t;
  }

  inline json to_json(const musciteer::dm::album& album)
  {
    auto artist = album.artist();

    json jalbum = {
      { "id", album.id() },
      { "title", album.title() },
      //{ "tracks", album.track_ids() },
      { "artist", {
        { "id", artist.id() },
        { "name", artist.name() } }
      },
      { "cover", "/api/albums/"+album.id()+"/cover" }
    };

    return jalbum;
  }
}

// ----------------------------------------------------------------------------
#endif