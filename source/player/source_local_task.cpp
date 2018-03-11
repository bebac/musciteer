// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include "player_notification_sender.h"
#include "source_local_task.h"
#include "message.h"

// ----------------------------------------------------------------------------
#include <dripcore/task.h>
#include <dripcore/timer.h>

// ----------------------------------------------------------------------------
#include <FLAC/stream_decoder.h>

// ----------------------------------------------------------------------------
namespace musciteer
{
  /////
  // Flac file decoder.
  //
  class stream_decoder : public dripcore::task
  {
    using notification_sender = musciteer::player_notification_sender;
    using write_status = FLAC__StreamDecoderWriteStatus;
    using decoder_ptr = FLAC__StreamDecoder*;
  public:
    stream_decoder(unsigned id, const std::string& filename, audio_output_alsa output, message_channel ch)
      :
      filename_(filename),
      output_(output),
      notifier_(id, ch)
    {
      if( (decoder_ = FLAC__stream_decoder_new()) == nullptr ) {
        throw std::runtime_error("failed to allocate new flac stream decoder!");
      }
    }
    ~stream_decoder()
    {
      FLAC__stream_decoder_delete(decoder_);
    }
  public:
    void init() override
    {
      set_md5_checking(true);
      init_file(filename_);
    }
    void main() override
    {
      output_.set_error_handler([&](int error_code) {
        std::cerr << "flac stream decoder audio output error! " << error_code << std::endl;
        stop();
      });

      if ( decode() )
      {
        output_.set_error_handler([&](int error_code) {
          if ( error_code != -EAGAIN ) {
            throw audio_output_error(error_code);
          }
        });

        output_.drain();

        while ( output_.is_draining()  ) {
          sleep_for(std::chrono::duration<double, std::milli>(100));
        }

        notifier_.stream_end();
      }
      else
      {
        output_.drop();
        notifier_.stream_end(true);
      }

      output_.clr_error_handler();
    }
  private:
    void set_md5_checking(bool value)
    {
      FLAC__stream_decoder_set_md5_checking(decoder_, value);
    }
  private:
    void init_file(const std::string& filename)
    {
      auto res = FLAC__stream_decoder_init_file(
        decoder_,
        filename.c_str(),
        // write callback
        [](const FLAC__StreamDecoder*, const FLAC__Frame* frame, const FLAC__int32* const buffer[], void* instance_ptr) {
          return static_cast<stream_decoder*>(instance_ptr)->write_callback(frame, buffer);
        },
        // metadata callback
        [](const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *instance_ptr) {
          return static_cast<stream_decoder*>(instance_ptr)->metadata_callback(metadata);
        },
        // error callback
        [](const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *instance_ptr) {
          throw std::runtime_error(FLAC__StreamDecoderErrorStatusString[status]);
        },
        this
      );

      if( res != FLAC__STREAM_DECODER_INIT_STATUS_OK ) {
        throw std::runtime_error(FLAC__StreamDecoderInitStatusString[res]);
      }
    }
  private:
    bool decode()
    {
      return FLAC__stream_decoder_process_until_end_of_stream(decoder_);
    }
  private:
    virtual write_status write_callback(const FLAC__Frame* frame, const FLAC__int32* const buffer[])
    {
      auto bps   = frame->header.bits_per_sample;
      auto scale = (1 << (32 - bps)) * output_.get_replaygain_scale();
      auto obuf  = reinterpret_cast<const s32_le_n_frame*>(buffer);
      auto size  = frame->header.blocksize;

      if ( ! write_samples(obuf, size, scale) ) {
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
      }

      if ( output_.is_prepared() )
      {
        if ( output_.start() )
        {
          notifier_.stream_begin(output_);
        }
      }
      else
      {
        notifier_.stream_progress();
      }

      if ( stopping() || done() ) {
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
      }
      else {
        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
      }
    }
  private:
    void metadata_callback(const FLAC__StreamMetadata *metadata)
    {
      using std::chrono::duration;
      using std::chrono::duration_cast;
      using std::chrono::milliseconds;

      auto channels      = metadata->data.stream_info.channels;
      auto sample_rate   = metadata->data.stream_info.sample_rate;
      auto total_samples = metadata->data.stream_info.total_samples;
      auto length        = duration<float>(float(total_samples)/float(sample_rate));

      notifier_.set_stream_length(duration_cast<milliseconds>(length));

      if ( channels != 2 ) {
        throw std::runtime_error("flac::stream_decoder only supports 2 channels");
      }

      output_.set_params(channels, sample_rate);
      output_.prepare();
    }
  public:
    bool write_samples(const s32_le_n_frame* const samples, std::size_t len, float scale)
    {
      auto i = size_t{0};

      while ( len > 0 )
      {
        auto avail = output_.avail_update();

        if ( avail < 0 )
        {
          if ( output_.recover(avail, 1) < 0 ) {
            return false;
          }
        }

        if ( avail > output_.hw_period_samples() )
        {
          const snd_pcm_channel_area_t* areas;
          snd_pcm_uframes_t offset;
          snd_pcm_uframes_t frames = len;

          output_.mmap_begin(&areas, &offset, &frames);

          auto obuf = reinterpret_cast<s32_le_i_frame*>(areas[0].addr) + offset;
          for( size_t x = 0; x < frames; x++, i++ )
          {
            obuf[x].l = static_cast<int32_t>(samples->l[i]) * scale;
            obuf[x].r = static_cast<int32_t>(samples->r[i]) * scale;
          }
          auto committed = output_.mmap_commit(offset, frames);

          len -= committed;
        }
        else
        {
          sleep_for(std::chrono::duration<double, std::milli>(50));
        }
      }

      return true;
    }
  public:
    template<class Rep, class Period>
    void sleep_for(const std::chrono::duration<Rep, Period>& duration)
    {
      timer_.set(duration, [&](){ resume(); });
      yield();
    }
  private:
    decoder_ptr decoder_;
    dripcore::timer timer_;
    std::string filename_;
    audio_output_alsa output_;
    notification_sender notifier_;
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

      auto track = session->track();
      assert(track);

      auto source = track->sources_get("local");
      auto output = session->get_audio_output();
      auto replaygain = source.rg_track_gain();
      auto replaygain_peak = source.rg_track_peak();

      output.set_replaygain((replaygain ? replaygain.value() : 0), (replaygain_peak ? replaygain_peak.value() : 1));

      auto decoder_task = spawn<stream_decoder>(
        session->id(),
        source.uri(),
        output,
        session->get_notification_channel()
      );

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
