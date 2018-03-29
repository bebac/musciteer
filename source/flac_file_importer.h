// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__flac_file_importer__
#define __musciteer__flac_file_importer__

// ----------------------------------------------------------------------------
#include "dm/album.h"
#include "dm/album_cover.h"
#include "dm/albums.h"
#include "dm/artist.h"
#include "dm/artists.h"
#include "dm/track.h"
#include "dm/tracks.h"

// ----------------------------------------------------------------------------
#include "flac/metadata.h"

// ----------------------------------------------------------------------------
namespace musciteer
{
  class flac_file_importer
  {
  public:
    flac_file_importer(const std::string& filename)
      :
      filename_(filename),
      tracks_(musciteer::dm::tracks()),
      albums_(musciteer::dm::albums()),
      artists_(musciteer::dm::artists())
    {
    }
  public:
    void run()
    {
      int         tag_tn = 1;
      int         tag_dn = 1;
      std::string tag_title;
      std::string tag_artist;
      std::string tag_album;
      std::string tag_album_artist;
      std::string tag_disc_id;
      std::string tag_rg_ref_loudness;
      std::string tag_rg_track_gain;
      std::string tag_rg_track_peak;
      std::string tag_rg_album_gain;
      std::string tag_rg_album_peak;
      std::string alt_id;
      size_t      total_samples;
      size_t      sample_rate;

      flac::metadata_chain metadata;

      metadata.on_stream_info(
        [&](flac::stream_info& stream_info)
        {
          total_samples = stream_info.total_samples;
          sample_rate   = stream_info.sample_rate;
        }
      );

      auto set_tag_vars = [&](std::string& key, std::string& value)
      {
        if ( key == "TITLE" ) {
          tag_title = std::move(value);
        }
        else if ( key == "ARTIST" ) {
          tag_artist = std::move(value);
        }
        else if ( key == "ALBUM" ) {
          tag_album = std::move(value);
        }
        else if ( key == "ALBUM_ARTIST" || key == "ALBUM ARTIST" || key == "ALBUMARTIST" ) {
          tag_album_artist = std::move(value);
        }
        else if ( key == "TRACK_NUMBER" || key == "TRACK NUMBER" || key == "TRACKNUMBER" ) {
          tag_tn = std::stoi(value);
        }
        else if ( key == "DISC_NUMBER" || key == "DISC NUMBER" || key == "DISCNUMBER" ) {
          tag_dn = std::stoi(value);
        }
        else if ( key == "DISCID" ) {
          tag_disc_id = std::move(value);
        }
        else if ( key == "REPLAYGAIN_REFERENCE_LOUDNESS" ) {
          tag_rg_ref_loudness = std::move(value);
        }
        else if ( key == "REPLAYGAIN_TRACK_GAIN" ) {
          tag_rg_track_gain = std::move(value);
        }
        else if ( key == "REPLAYGAIN_TRACK_PEAK" ) {
          tag_rg_track_peak = std::move(value);
        }
        else if ( key == "REPLAYGAIN_ALBUM_GAIN" ) {
          tag_rg_album_gain = std::move(value);
        }
        else if ( key == "REPLAYGAIN_ALBUM_PEAK" ) {
          tag_rg_album_peak = std::move(value);
        }
        else
        {
          //std::cout << "unhandled comment name=" << name << ", value=" << value << std::endl;
        }
      };

      metadata.on_vorbis_comment([&](flac::vorbis_comment& vorbis_comment) {
        flac::vorbis_comment_each_kv(vorbis_comment, set_tag_vars);
      });

      metadata.read(filename_);

#if 0
      std::cout
        << "title        : " << tag_title << std::endl
        << "artist       : " << tag_artist << std::endl
        << "album        : " << tag_album << std::endl
        << "album_artist : " << tag_album_artist << std::endl
        << "tn           : " << tag_tn << std::endl
        << "dn           : " << tag_dn << std::endl
        << "disc_id      : " << tag_disc_id << std::endl;
#endif

      if ( tag_title.empty() ) {
        throw std::runtime_error("title tag is empty!");
      }

      if ( tag_album.empty() ) {
        throw std::runtime_error("album tag is empty!");
      }

      if ( tag_artist.empty() ) {
        throw std::runtime_error("artist tag is empty!");
      }

      // Try to find album by disc id.

      if ( !tag_disc_id.empty() )
      {
        resolve_album_by_disc_id(tag_disc_id);
      }

      // If not found by disc id, try by album artist and album title or
      // artist and album title if there was no album artist tag.

      if ( album_.id_is_null() )
      {
        if ( !tag_album_artist.empty() )
        {
          resolve_album_by_artist_name_and_album_title(tag_album_artist, tag_album);
        }
        else
        {
          resolve_album_by_artist_name_and_album_title(tag_artist, tag_album);
        }
      }

      // If album artist was not found, create new artist.

      if ( album_artist_.id_is_null() )
      {
        if ( !tag_album_artist.empty() ) {
          album_artist_ = artists_.create_artist(tag_album_artist);
        }
        else {
          album_artist_ = artists_.create_artist(tag_artist);
        }
      }

      // If album was not found, create new album.

      if ( album_.id_is_null() )
      {
        album_ = albums_.create_album();

        album_.title(tag_album);
        album_.artist(album_artist_);

        if ( !tag_disc_id.empty() ) {
          album_.alt_ids_add(tag_disc_id);
        }

        album_artist_.albums_add(album_);
      }

      if ( album_artist_.name() == tag_artist )
      {
        artist_ = album_artist_;
      }
      else
      {
        resolve_artist_by_name(tag_artist);
      }

      // If artist was not found, create new artist.

      if ( artist_.id_is_null() )
      {
        artist_ = artists_.create_artist(tag_artist);
      }

      // Try to find track in album by disc number and track number. If
      // not found create a new track.

      resolve_track_by_disc_and_track_number(tag_dn, tag_tn);

      if ( track_.id_is_null() )
      {
        track_ = tracks_.create_track();
      }

      // Update track and album.

      musciteer::dm::track_source source("local", filename_);

      if ( !tag_rg_ref_loudness.empty() ) {
        source.rg_ref_loudness(tag_rg_ref_loudness);
      }

      if ( !tag_rg_track_gain.empty() ) {
        source.rg_track_gain(tag_rg_track_gain);
      }

      if ( !tag_rg_track_peak.empty() ) {
        source.rg_track_peak(tag_rg_track_peak);
      }

      track_.title(tag_title);
      track_.track_number(tag_tn);
      track_.disc_number(tag_dn);
      track_.duration(total_samples / sample_rate * 1000);
      track_.album(album_);
      track_.artists_add(artist_);
      track_.sources_add(source);

      album_.tracks_add(track_);

      tracks_.save(track_);
      albums_.save(album_);
      artists_.save(album_artist_);
      artists_.save(artist_);
    }
  private:
    void resolve_album_by_disc_id(const std::string& disc_id)
    {
      albums_.each([&](const musciteer::dm::album& rec)
      {
        if ( rec.alt_ids_find(disc_id) )
        {
          album_ = rec;
          album_artist_ = rec.artist();
          return false;
        }
        return true;
      });
    }
  private:
    void resolve_album_by_artist_name_and_album_title(const std::string& artist_name, const std::string& album_title)
    {
      artists_.each([&](const musciteer::dm::artist& rec)
      {
        if ( rec.name() == artist_name )
        {
          album_artist_ = rec;
          return false;
        }
        return true;
      });

      if ( album_artist_.id_is_null() )
        return;

      album_artist_.albums_each([&](const musciteer::dm::album& rec)
      {
        if ( rec.title() == album_title )
        {
          album_ = rec;
          return false;
        }
        return true;
      });
    }
  private:
    void resolve_track_by_disc_and_track_number(unsigned dn, unsigned tn)
    {
      assert(!album_.id_is_null());

      album_.tracks_each([&](const musciteer::dm::track& rec)
      {
        if ( rec.disc_number() == dn && rec.track_number() == tn )
        {
          track_ = rec;
          return false;
        }
        return true;
      });
    }
  private:
    void resolve_artist_by_name(const std::string& artist_name)
    {
      artists_.each([&](const musciteer::dm::artist& rec)
      {
        if ( rec.name() == artist_name )
        {
          artist_ = rec;
          return false;
        }
        return true;
      });
    }
  private:
    std::string filename_;
  private:
    musciteer::dm::tracks tracks_;
    musciteer::dm::albums albums_;
    musciteer::dm::artists artists_;
  private:
    musciteer::dm::track track_;
    musciteer::dm::album album_;
    musciteer::dm::artist album_artist_;
    musciteer::dm::artist artist_;
  };
}

// ----------------------------------------------------------------------------
#endif