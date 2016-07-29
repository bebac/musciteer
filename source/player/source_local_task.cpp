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
namespace musicbox
{
  /////
  // FLAC player.

  class flac_decoder : public FLAC::Decoder::File
  {
    using milliseconds = audio_output_alsa::milliseconds;
  public:
    flac_decoder(const std::string& filename, unsigned stream_id, std::shared_ptr<audio_output_alsa> audio_output)
      :
      FLAC::Decoder::File(),
      audio_output_(audio_output),
      stream_id_(stream_id)
    {
      auto res = FLAC::Decoder::File::init(filename.c_str());

      if ( FLAC__STREAM_DECODER_INIT_STATUS_OK != res ) {
        std::cout << "flac decoder error" << std::endl;
      }
    }
  public:
    void stream_buffer_channel_init(size_t num_buffers)
    {
      for ( size_t i=0; i<num_buffers; ++i ) {
        buffer_ch_.send(audio_buffer{}, task_);
      }
    }
  public:
    void play(dripcore::task* task)
    {
      task_ = task;

      stream_buffer_channel_init(10);
      process_until_end_of_stream();
      stream_end();
    }
  private:
    void stream_end()
    {
      message_channel done;
      message m(message::stream_end_id);

      m.stream_end.reply = done;
      audio_output_->send(std::move(m));

      done.recv(task_);
    }
  protected:
    FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], size_t *bytes)
    {
      return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }
  protected:
    FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame* frame, const FLAC__int32* const buffer[])
    {
      auto buf = buffer_ch_.recv(task_);

      buf.clear();
      buf.writen(buffer, frame->header.blocksize, bits_per_sample_);

      audio_output_->send(std::move(buf));

      if ( task_->stopping() || task_->done() ) {
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
      }
      else {
        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
      }
    }
  protected:
    void metadata_callback(const ::FLAC__StreamMetadata *metadata)
    {
      bits_per_sample_ = metadata->data.stream_info.bits_per_sample;

      auto sample_rate = metadata->data.stream_info.sample_rate;
      auto total_samples = metadata->data.stream_info.total_samples;

      std::chrono::duration<float> length{float(total_samples)/float(sample_rate)};

      message m(message::stream_begin_id);

      m.stream_begin.stream_id = stream_id_;
      m.stream_begin.sample_rate = sample_rate;
      m.stream_begin.length = std::chrono::duration_cast<milliseconds>(length);
      m.stream_begin.completed_buffer_ch = buffer_ch_;
      audio_output_->send(std::move(m));
    }
  protected:
    void error_callback (::FLAC__StreamDecoderErrorStatus status)
    {
      std::cout << "decoder error" << std::endl;
    }
  private:
    dripcore::task* task_;
    std::shared_ptr<audio_output_alsa> audio_output_;
  private:
    unsigned stream_id_;
    unsigned bits_per_sample_;
    dripcore::channel<audio_buffer> buffer_ch_;
  };

  /////
  // Flac decode task.

  class flac_decoder_task : public dripcore::task
  {
  public:
    flac_decoder_task(std::shared_ptr<player_session> session) : session_(session)
    {
    }
  private:
    void main() override
    {
      auto track = session_->track();
      assert(track);

      auto source = track->sources_get("local");

      flac_decoder decoder{
        source.uri(),
        session_->id(),
        session_->audio_output()
      };

      decoder.play(this);
    }
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
