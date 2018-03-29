// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2018
//
// ----------------------------------------------------------------------------
#ifndef __musciteer__flac__metadata_h__
#define __musciteer__flac__metadata_h__

// ----------------------------------------------------------------------------
#include <FLAC/metadata.h>

// ----------------------------------------------------------------------------
#include <functional>
#include <stdexcept>

// ----------------------------------------------------------------------------
namespace musciteer { namespace flac
{
  // Shorthands for FLAC__StreamMetadata union members.
  using stream_info    = FLAC__StreamMetadata_StreamInfo;
  using padding        = FLAC__StreamMetadata_Padding;
  using application    = FLAC__StreamMetadata_Application;
  using seek_table     = FLAC__StreamMetadata_SeekTable;
  using vorbis_comment = FLAC__StreamMetadata_VorbisComment;
  using cue_sheet      = FLAC__StreamMetadata_CueSheet;
  using picture        = FLAC__StreamMetadata_Picture;
  using unknown        = FLAC__StreamMetadata_Unknown;

  // Shorthand for vorbis comment entry.
  using vorbis_comment_entry = FLAC__StreamMetadata_VorbisComment_Entry;

  // Wrapper for a raw FLAC__StreamMetadata pointer.
  class metadata_block
  {
  public:
    metadata_block(FLAC__StreamMetadata* ptr) : instance_(ptr)
    {
    }
  public:
    FLAC__MetadataType type() const
    {
      return instance_->type;
    }
  public:
    stream_info& get_stream_info()
    {
      return instance_->data.stream_info;
    }
    vorbis_comment& get_vorbis_comment()
    {
      return instance_->data.vorbis_comment;
    }
    picture& get_picture()
    {
      return instance_->data.picture;
    }
  private:
    FLAC__StreamMetadata* instance_;
  };

  // Manage a FLAC__Metadata_Iterator pointer.
  class metadata_iterator
  {
  public:
    metadata_iterator(FLAC__Metadata_Chain* matadata_chain_ptr)
    {
      if ( (instance_ = FLAC__metadata_iterator_new()) != nullptr ) {
        FLAC__metadata_iterator_init(instance_, matadata_chain_ptr);
      }
      else {
        throw std::runtime_error("failed to constuct metadata_iterator");
      }
    }
    ~metadata_iterator()
    {
      FLAC__metadata_iterator_delete(instance_);
    }
  public:
    bool next()
    {
      return FLAC__metadata_iterator_next(instance_);
    }
  public:
    metadata_block get_block()
    {
      return metadata_block(FLAC__metadata_iterator_get_block(instance_));
    }
  private:
    FLAC__Metadata_Iterator* instance_;
  };

  // Manage a FLAC__Metadata_Chain pointer.
  class metadata_chain
  {
    using stream_info_func = std::function<void(stream_info& block)>;
    using vorbis_comment_func = std::function<void(vorbis_comment& block)>;
    using picture_func = std::function<void(picture& block)>;
  public:
    metadata_chain()
    {
      if ( (instance_ = FLAC__metadata_chain_new()) == nullptr ) {
        throw std::runtime_error("failed to constuct metadata_chain");
      }
    }
    ~metadata_chain()
    {
      FLAC__metadata_chain_delete(instance_);
    }
  public:
    void on_stream_info(stream_info_func func)
    {
      on_stream_info_ = func;
    }
    void on_vorbis_comment(vorbis_comment_func func)
    {
      on_vorbis_comment_ = func;
    }
    void on_picture(picture_func func)
    {
      on_picture_ = func;
    }
  public:
    bool read(const std::string& filename)
    {
      if ( FLAC__metadata_chain_read(instance_, filename.c_str()) )
      {
        iterate();
        return true;
      }
      else
      {
        return false;
      }
    }
  private:
    void iterate()
    {
      metadata_iterator it(instance_);
      do
      {
        call_block_handler(it.get_block());
      }
      while ( it.next() );
    }
  private:
    void call_block_handler(metadata_block&& block)
    {
      switch ( block.type() )
      {
        case FLAC__METADATA_TYPE_STREAMINFO:
          if ( on_stream_info_ ) {
            on_stream_info_(block.get_stream_info());
          }
          break;
        case FLAC__METADATA_TYPE_PADDING:
          break;
        case FLAC__METADATA_TYPE_APPLICATION:
          break;
        case FLAC__METADATA_TYPE_SEEKTABLE:
          // TODO
          break;
        case FLAC__METADATA_TYPE_VORBIS_COMMENT:
          if ( on_vorbis_comment_ ) {
            on_vorbis_comment_(block.get_vorbis_comment());
          }
          break;
        case FLAC__METADATA_TYPE_CUESHEET:
          // TODO
          break;
        case FLAC__METADATA_TYPE_PICTURE:
          if ( on_picture_ ) {
            on_picture_(block.get_picture());
          }
          break;
        case FLAC__METADATA_TYPE_UNDEFINED:
          // TODO
          break;
        default:
          break;
      };
    }
  private:
    stream_info_func    on_stream_info_;
    vorbis_comment_func on_vorbis_comment_;
    picture_func        on_picture_;
  private:
    FLAC__Metadata_Chain* instance_;
  };

  // Iterate a vorbis comment block as key/value pairs
  void vorbis_comment_each_kv(musciteer::flac::vorbis_comment& comments,
    std::function<void(std::string&, std::string&)> cb)
  {
    for ( size_t i = 0; i < comments.num_comments; ++i )
    {
      musciteer::flac::vorbis_comment_entry* c = &comments.comments[i];

      char* key_b = reinterpret_cast<char*>(c->entry);
      char* key_e = key_b;
      char* val_b = nullptr;
      char* val_e = key_b + c->length;

      while ( key_e < val_e )
      {
        if ( *key_e == '=')
        {
          val_b = key_e + 1;
          break;
        }
        else
        {
          key_e++;
        }
      }

      if ( val_b )
      {
        std::string key(key_b, key_e-key_b);
        std::string val(val_b, val_e-val_b);

        // Convert key to all upper case.
        for ( size_t i=0; i<key.length(); ++i ) {
          key[i] = std::toupper(key[i]);
        }

        cb(key, val);
      }
      else
      {
        // TODO: Not a key/value pair???
      }
    }
  }
}}

// ----------------------------------------------------------------------------
#endif