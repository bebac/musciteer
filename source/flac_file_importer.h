// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musicbox__flac_file_importer__
#define __musicbox__flac_file_importer__

// ----------------------------------------------------------------------------
#include "dm/album.h"
#include "dm/album_cover.h"
#include "dm/albums.h"
#include "dm/artist.h"
#include "dm/artists.h"
#include "dm/track.h"
#include "dm/tracks.h"

// ----------------------------------------------------------------------------
#include <FLAC++/metadata.h>

// ----------------------------------------------------------------------------
namespace musicbox
{
  class flac_file_importer
  {
  public:
    flac_file_importer(const std::string& filename)
      :
      filename_(filename),
      tracks_(musicbox::tracks()),
      albums_(musicbox::albums()),
      artists_(musicbox::artists())
    {
      if ( !FLAC::Metadata::get_tags(filename_.c_str(), comments_) )
      {
        throw std::runtime_error("failed to read flac comments metadata");
      }

      if ( !FLAC::Metadata::get_streaminfo(filename_.c_str(), stream_info_) )
      {
        throw std::runtime_error("failed to read flac streaminfo metadata");
      }
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
      std::string alt_id;

      each_comment([&](std::string name, std::string value)
      {
        if ( name == "TITLE" ) {
          tag_title = value;
        }
        else if ( name == "ARTIST" ) {
          tag_artist = value;
        }
        else if ( name == "ALBUM" ) {
          tag_album = value;
        }
        else if ( name == "ALBUM_ARTIST" || name == "ALBUM ARTIST" || name == "ALBUMARTIST" ) {
          tag_album_artist = value;
        }
        else if ( name == "TRACK_NUMBER" || name == "TRACK NUMBER" || name == "TRACKNUMBER" ) {
          tag_tn = std::stoi(value);
        }
        else if ( name == "DISC_NUMBER" || name == "DISC NUMBER" || name == "DISCNUMBER" ) {
          tag_dn = std::stoi(value);
        }
        else if ( name == "DISCID" )
        {
          tag_disc_id = value;
        }
        else
        {
          //std::cout << "unhandled comment name=" << name << ", value=" << value << std::endl;
        }
        //
        // TODO: replaygain.
        //
      });

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

      if ( album_.id().empty() )
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

      if ( album_artist_.id().empty() )
      {
        if ( !tag_album_artist.empty() ) {
          album_artist_ = artists_.create_artist(tag_album_artist);
        }
        else {
          album_artist_ = artists_.create_artist(tag_artist);
        }
      }

      // If album was not found, create new album.

      if ( album_.id().empty() )
      {
        album_ = albums_.create_album();

        album_.title(tag_album);
        album_.artist(album_artist_);

        if ( !tag_disc_id.empty() ) {
          album_.alt_ids_add(tag_disc_id);
        }

        album_artist_.albums_add(album_);

        create_album_cover();
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

      if ( artist_.id().empty() )
      {
        artist_ = artists_.create_artist(tag_artist);
      }

      // Try to find track in album by disc number and track number. If
      // not found create a new track.

      resolve_track_by_disc_and_track_number(tag_dn, tag_tn);

      if ( track_.id().empty() )
      {
        track_ = tracks_.create_track();
      }

      // Update track and album.

      track_.title(tag_title);
      track_.track_number(tag_tn);
      track_.disc_number(tag_dn);
      track_.duration(stream_info_.get_total_samples() / stream_info_.get_sample_rate() * 1000);
      track_.album(album_);
      track_.artists_add(artist_);
      track_.sources_add(musicbox::track_source("local", filename_));

      album_.tracks_add(track_);

      tracks_.save(track_);
      albums_.save(album_);
      artists_.save(album_artist_);
      artists_.save(artist_);
    }
  private:
    void resolve_album_by_disc_id(const std::string& disc_id)
    {
      albums_.each([&](const musicbox::album& rec)
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
      artists_.each([&](const musicbox::artist& rec)
      {
        if ( rec.name() == artist_name )
        {
          album_artist_ = rec;
          return false;
        }
        return true;
      });

      if ( album_artist_.id().empty() )
        return;

      album_artist_.albums_each([&](const musicbox::album& rec)
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
      assert(!album_.id().empty());

      album_.tracks_each([&](const musicbox::track& rec)
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
      artists_.each([&](const musicbox::artist& rec)
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
    void each_comment(std::function<void(std::string name, std::string value)> cb)
    {
      for ( unsigned i=0; i<comments_->get_num_comments(); ++i )
      {
        auto c = comments_->get_comment(i);

        std::string name = c.get_field_name();

        for ( size_t i=0; i<name.length(); ++i ) {
          name[i] = std::toupper(name[i]);
        }

        cb(name, c.get_field_value());
      }
    }
  private:
    void create_album_cover()
    {
      FLAC::Metadata::Picture picture;

      if (
        get_picture(picture, FLAC__STREAM_METADATA_PICTURE_TYPE_FRONT_COVER) ||
        get_picture(picture, FLAC__STREAM_METADATA_PICTURE_TYPE_MEDIA)
      )
      {
        auto cover = musicbox::album_cover();

        cover.mime_type(picture.get_mime_type());
        cover.data(picture.get_data(), picture.get_data_length());

        auto kvstore = musicbox::kvstore();

        kvstore.set(album_.id()+"/cover", cover);
      }
    }
  private:
    bool get_picture(FLAC::Metadata::Picture& picture, FLAC__StreamMetadata_Picture_Type type)
    {
      return FLAC::Metadata::get_picture(
        filename_.c_str(),
        picture,
        type,
        NULL,
        NULL,
        -1,
        -1,
        -1,
        -1
      );
    }
  private:
    std::string filename_;
  private:
    musicbox::tracks tracks_;
    musicbox::albums albums_;
    musicbox::artists artists_;
  private:
    musicbox::track track_;
    musicbox::album album_;
    musicbox::artist album_artist_;
    musicbox::artist artist_;
  private:
    FLAC::Metadata::VorbisComment* comments_;
    FLAC::Metadata::StreamInfo stream_info_;
  };
}

// ----------------------------------------------------------------------------
#endif