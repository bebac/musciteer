// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include "player.h"

// ----------------------------------------------------------------------------
#include "../dm/tracks.h"

// ----------------------------------------------------------------------------
#include "../storage/kvstore.h"

// ----------------------------------------------------------------------------
#include <FLAC++/decoder.h>

// ----------------------------------------------------------------------------
class flac_decoder : public FLAC::Decoder::File
{
  using milliseconds = audio_output_alsa::milliseconds;
public:
  flac_decoder(const std::string& filename, unsigned stream_id, audio_output_alsa& audio_output)
    : FLAC::Decoder::File(), stream_id_(stream_id), audio_output_(audio_output)
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

    stream_buffer_channel_init(5);
    process_until_end_of_stream();
    stream_end();
  }
private:
  void stream_end()
  {
    message_channel done;
    message m(message::stream_end_id);

    m.stream_end.reply = done;
    audio_output_.send(std::move(m));

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

    message m(message::stream_buffer_id);

    m.stream_buffer.buffer = std::move(buf);
    audio_output_.send(std::move(m));

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
    audio_output_.send(std::move(m));
  }
protected:
  void error_callback (::FLAC__StreamDecoderErrorStatus status)
  {
    std::cout << "decoder error" << std::endl;
  }
private:
  dripcore::task* task_;
  audio_output_alsa& audio_output_;
private:
  unsigned stream_id_;
  unsigned bits_per_sample_;
  dripcore::channel<audio_buffer> buffer_ch_;
};

// ----------------------------------------------------------------------------
class flac_player_task : public dripcore::task
{
  using milliseconds = audio_output_alsa::milliseconds;
public:
  flac_player_task(const play_request& request, audio_output_alsa& audio_output)
    :
    decoder_(request.uri, request.stream_id, audio_output)
  {
  }
private:
  void main()
  {
    decoder_.play(this);
  }
private:
  flac_decoder decoder_;
};

// ----------------------------------------------------------------------------
namespace musicbox
{
  /////
  // Instantiate the one and only player.

  player::instance_ptr player::instance_;

  /////
  // Player implementation.

  player_impl::player_impl(dripcore::loop* loop)
    : loop_(loop), output_device_()
  {
    auto kvstore = musicbox::kvstore();

    kvstore.get(output_device_key, output_device_);
  }

  void player_impl::subscribe(message_channel ch)
  {
    message req(message::subscribe_id, 0);

    req.subscribe.channel = ch;

    audio_output_.send(std::move(req));
  }

  void player_impl::unsubscribe(message_channel ch)
  {
    message req(message::unsubscribe_id, 0);

    req.subscribe.channel = ch;

    audio_output_.send(std::move(req));
  }

  void player_impl::audio_device_list(message_channel reply_ch)
  {
    message req(message::device_list_req_id, 0);

    req.device_list_req.current = output_device_;
    req.device_list_req.reply = reply_ch;

    audio_output_.send(std::move(req));
  }

  void player_impl::audio_device(const std::string& device_name)
  {
    auto kvstore = musicbox::kvstore();

    output_device_ = device_name;

    kvstore.set(output_device_key, output_device_);
  }

  void player_impl::play(const std::string& id)
  {
    {
      message req(message::open_req_id, 0);

      req.open_req.device_name = output_device_;
      //m.open_req.reply = output_channel_;
      audio_output_.send(std::move(req));
    }

    {
      message req(message::play_req_id, 0);

      auto tracks = musicbox::tracks();

      auto track = tracks.find_by_id(id);
      auto sources = track.sources();

      if ( sources.size() > 0 )
      {
        auto filename = sources[0].uri();

        req.play_req.stream_id = 1;
        req.play_req.uri = filename;

        loop_->spawn<flac_player_task>(req.play_req, audio_output_);
      }
      else
      {
        std::cout << "no source!" << std::endl;
      }
    }
  }
}
