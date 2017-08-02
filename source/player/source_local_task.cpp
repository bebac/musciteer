// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include "source_local_task.h"
#include "message.h"
#include "audio_buffer.h"
#include "audio_output.h"

// ----------------------------------------------------------------------------
#include <FLAC++/decoder.h>

// ----------------------------------------------------------------------------
namespace musciteer
{
  /////
  // Flac file decoder.
  //

  template<typename T>
  class flac_decoder_file : public FLAC::Decoder::File
  {
  public:
    flac_decoder_file(T& handler) : handler_(handler)
    {
    }
  public:
    void init_track_source(const std::string& filename)
    {
      auto res = init(filename.c_str());

      if ( FLAC__STREAM_DECODER_INIT_STATUS_OK != res ) {
        throw std::runtime_error("flac decoder file intit error!");
      }
    }
  protected:
    FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], size_t *bytes)
    {
      return handler_.read_callback(buffer, bytes);
    }
  protected:
    FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame* frame, const FLAC__int32* const buffer[])
    {
      return handler_.write_callback(frame, buffer);
    }
  protected:
    void metadata_callback(const ::FLAC__StreamMetadata *metadata)
    {
      handler_.metadata_callback(metadata);
    }
  protected:
    void error_callback (::FLAC__StreamDecoderErrorStatus status)
    {
      handler_.error_callback(status);
    }
  private:
    T& handler_;
  };

  /////
  // Flac decoder task.

  class flac_decoder_task : public dripcore::task
  {
    friend class flac_decoder_file<flac_decoder_task>;
  public:
    using milliseconds = audio_output::milliseconds;
  public:
    flac_decoder_task(std::shared_ptr<player_session> session)
      :
      decoder_(*this),
      session_(session)
    {
    }
  private:
    void init() override
    {
      for ( size_t i=0; i<10; ++i ) {
        buffer_ch_.send(audio_buffer{}, this);
      }
    }
  private:
    void main() override
    {
      auto track = session_->track();
      assert(track);

      source_ = track->sources_get("local");

      decoder_.init_track_source(source_.uri());
      decoder_.process_until_end_of_stream();

      stream_end();
    }
  private:
    void stream_end()
    {
      message_channel done;
      message m(message::stream_end_id);

      m.stream_end.reply = done;

      send(*session_->get_audio_output(), std::move(m));

      done.recv(this);
    }
  private:
    void metadata_callback(const ::FLAC__StreamMetadata *metadata)
    {
      bits_per_sample_ = metadata->data.stream_info.bits_per_sample;

      auto sample_rate = metadata->data.stream_info.sample_rate;
      auto total_samples = metadata->data.stream_info.total_samples;
      auto replaygain = source_.rg_track_gain();
      auto replaygain_peak = source_.rg_track_peak();

      std::chrono::duration<float> length{float(total_samples)/float(sample_rate)};

      message m(message::stream_begin_id);

      m.stream_begin.stream_id = session_->id();
      m.stream_begin.sample_rate = sample_rate;
      m.stream_begin.length = std::chrono::duration_cast<milliseconds>(length);
      m.stream_begin.replaygain = replaygain ? replaygain.value() : 0;
      m.stream_begin.replaygain_peak = replaygain_peak ? replaygain_peak.value() : 1;
      m.stream_begin.completed_buffer_ch = buffer_ch_;

      send(*session_->get_audio_output(), std::move(m));
    }
  private:
    FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], size_t *bytes)
    {
      return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }
  private:
    FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame* frame, const FLAC__int32* const buffer[])
    {
      auto buf = buffer_ch_.recv(this);

      buf.clear();
      buf.writen(buffer, frame->header.blocksize, bits_per_sample_);

      send(*session_->get_audio_output(), std::move(buf));

      if ( stopping() || done() ) {
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
      }
      else {
        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
      }
    }
  private:
    void error_callback (::FLAC__StreamDecoderErrorStatus status)
    {
      std::cout << "decoder error" << std::endl;
    }
  private:
    template<typename Receiver, typename Message>
    void send(Receiver& receiver, Message m)
    {
      receiver.send(std::forward<Message>(m));
    }
  private:
    flac_decoder_file<flac_decoder_task> decoder_;
  private:
    unsigned bits_per_sample_;
    dripcore::channel<audio_buffer> buffer_ch_;
  private:
    dm::track_source source_;
  private:
    std::shared_ptr<player_session> session_;
  };

  /////
  // source_local_task implementation.

  source_local_task::source_local_task(session_channel channel) : ch_(channel)
  {
  }

  void source_local_task::main()
  {
    while ( true )
    {
      auto session = ch_.recv(this);

      if ( !session ) {
        break;
      }

      // Start decoder task.
      auto decoder_task = spawn<flac_decoder_task>(session);

      // Process session control messages until session done.
      auto ctrl = player_session::control::undefined;
      do
      {
        ctrl = session->recv(this);

        if ( ctrl == player_session::control::stop )
        {
          std::shared_ptr<task> task_ptr = decoder_task.lock();

          if ( task_ptr ) {
            task_ptr->stop();
          }
        }
      }
      while( ctrl != player_session::control::done );
    }
  }
}
