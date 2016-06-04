// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __msgpack__message_h__
#define __msgpack__message_h__

// ----------------------------------------------------------------------------
#include "audio_buffer.h"

// ----------------------------------------------------------------------------
#include "../dm/track.h"

// ----------------------------------------------------------------------------
#include <dripcore/channel.h>
#include <dripcore/task.h>

// ----------------------------------------------------------------------------
class message;

// ----------------------------------------------------------------------------
using message_channel = dripcore::channel<message>;

// ----------------------------------------------------------------------------
class audio_output_response
{
public:
  audio_output_response() : error_code(0)
  {
  }
  audio_output_response(audio_output_response&& other)
  {
    error_code = std::move(other.error_code);
    error_message = std::move(other.error_message);
  }
public:
  int error_code;
  std::string error_message;
};

// ----------------------------------------------------------------------------
class audio_output_notification_message
{
public:
  audio_output_notification_message()
  {
  }
  audio_output_notification_message(audio_output_notification_message&& other)
  {
    *this = std::move(other);
  }
  audio_output_notification_message& operator=(audio_output_notification_message&& rhs)
  {
    channel = std::move(rhs.channel);
    return *this;
  }
public:
  message_channel channel;
};

// ----------------------------------------------------------------------------
class audio_output_subscribe_message : public audio_output_notification_message
{
};

// ----------------------------------------------------------------------------
class audio_output_unsubscribe_message : public audio_output_notification_message
{
};

// ----------------------------------------------------------------------------
class audio_output_device_list_request
{
public:
  audio_output_device_list_request()
  {
  }
  audio_output_device_list_request(audio_output_device_list_request&& other)
  {
    *this = std::move(other);
  }
  audio_output_device_list_request& operator=(audio_output_device_list_request&& rhs)
  {
    current = std::move(rhs.current);
    reply = std::move(rhs.reply);
    return *this;
  }
public:
  std::string current;
  message_channel reply;
};

// ----------------------------------------------------------------------------
class audio_output_device_list_response
{
public:
  audio_output_device_list_response()
  {
  }
  audio_output_device_list_response(audio_output_device_list_response&& other)
  {
    device_names = std::move(other.device_names);
    current = std::move(other.current);
  }
public:
  std::vector<std::string> device_names;
  std::string current;
};

// ----------------------------------------------------------------------------
class audio_output_device
{
public:
  audio_output_device() : device_name()
  {
  }
  audio_output_device(audio_output_device&& other)
  {
    device_name = std::move(other.device_name);
  }
public:
  std::string device_name;
};

// ----------------------------------------------------------------------------
class audio_output_open_request
{
public:
  audio_output_open_request() : device_name()
  {
  }
  audio_output_open_request(audio_output_open_request&& other)
  {
    device_name = std::move(other.device_name);
    reply = std::move(other.reply);
  }
public:
  std::string device_name;
  message_channel reply;
};

// ----------------------------------------------------------------------------
using audio_output_open_response = audio_output_response;

// ----------------------------------------------------------------------------
class audio_output_close_request
{
public:
  audio_output_close_request()
  {
  }
  audio_output_close_request(audio_output_close_request&& other)
  {
    reply = std::move(other.reply);
  }
public:
  message_channel reply;
};

// ----------------------------------------------------------------------------
using audio_output_close_response = audio_output_response;

// ----------------------------------------------------------------------------
class play_request
{
public:
  play_request() : id()
  {
  }
  play_request(play_request&& other)
  {
    id = std::move(other.id);
  }
public:
  std::string id;
};

// ----------------------------------------------------------------------------
class queue_request
{
public:
  queue_request() : id()
  {
  }
  queue_request(queue_request&& other)
  {
    id = std::move(other.id);
  }
public:
  std::string id;
};

// ----------------------------------------------------------------------------
class audio_output_stream_begin
{
public:
  audio_output_stream_begin() : stream_id(0)
  {
  }
  audio_output_stream_begin(audio_output_stream_begin&& other)
  {
    stream_id = std::move(other.stream_id);
    sample_rate = std::move(other.sample_rate);
    length = std::move(other.length);
    completed_buffer_ch = std::move(other.completed_buffer_ch);
  }
public:
  unsigned stream_id;
  unsigned sample_rate;
  std::chrono::milliseconds length;
  dripcore::channel<audio_buffer> completed_buffer_ch;
};

// ----------------------------------------------------------------------------
class audio_output_stream_end
{
public:
  audio_output_stream_end()
  {
  }
  audio_output_stream_end(audio_output_stream_end&& other)
  {
    reply = std::move(other.reply);
  }
public:
  message_channel reply;
};

// ----------------------------------------------------------------------------
class audio_output_stream_buffer
{
public:
  audio_output_stream_buffer() : buffer()
  {
  }
  audio_output_stream_buffer(audio_output_stream_buffer&& other)
  {
    buffer = std::move(other.buffer);
  }
public:
  audio_buffer buffer;
};

// ----------------------------------------------------------------------------
class audio_output_stream_begin_notification
{
public:
  audio_output_stream_begin_notification()
  {
  }
  audio_output_stream_begin_notification(audio_output_stream_begin_notification&& other)
  {
    stream_id = std::move(other.stream_id);
  }
public:
  unsigned stream_id;
};

// ----------------------------------------------------------------------------
class audio_output_stream_end_notification
{
public:
  audio_output_stream_end_notification()
  {
  }
  audio_output_stream_end_notification(audio_output_stream_end_notification&& other)
  {
    stream_id = std::move(other.stream_id);
  }
public:
  unsigned stream_id;
};

// ----------------------------------------------------------------------------
class audio_output_stream_progress_notification
{
public:
  audio_output_stream_progress_notification()
  {
  }
  audio_output_stream_progress_notification(audio_output_stream_progress_notification&& other)
  {
    stream_id = std::move(other.stream_id);
    duration = std::move(other.duration);
    length = std::move(other.length);
  }
public:
  unsigned stream_id;
  unsigned duration;
  unsigned length;
};

// ----------------------------------------------------------------------------
class queue_update_notification
{
public:
  queue_update_notification()
  {
  }
  queue_update_notification(queue_update_notification&& other)
  {
    queue_size = std::move(other.queue_size);
    track = std::move(other.track);
  }
public:
  unsigned queue_size;
  std::shared_ptr<musicbox::track> track;
};

// ----------------------------------------------------------------------------
class stream_data_request
{
public:
  stream_data_request() : stream_id()
  {
  }
  stream_data_request(stream_data_request&& other)
  {
    stream_id = std::move(other.stream_id);
    reply = std::move(other.reply);
  }
public:
  unsigned stream_id;
  message_channel reply;
};

// ----------------------------------------------------------------------------
class stream_data_response
{
public:
  stream_data_response() : stream_id()
  {
  }
  stream_data_response(stream_data_response&& other)
  {
    stream_id = std::move(other.stream_id);
    track = std::move(other.track);
  }
public:
  unsigned stream_id;
  std::shared_ptr<musicbox::track> track;
};

// ----------------------------------------------------------------------------
class message
{
public:
  enum id
  {
    undefined_id = 0,
    device_list_req_id = 1,
    device_list_res_id = 2,
    device_id = 3,
    open_req_id = 4,
    open_res_id = 5,
    close_req_id = 6,
    close_res_id = 7,
    play_req_id = 8,
    queue_req_id = 9,
    stream_begin_id = 10,
    stream_end_id = 11,
    stream_buffer_id = 12,
    stream_begin_notify_id = 13,
    stream_end_notify_id = 14,
    stream_progress_notify_id = 15,
    subscribe_id = 16,
    unsubscribe_id = 17,
    queue_update_id = 18,
    stream_data_req_id = 19,
    stream_data_res_id = 20,
  };
public:
  message() : type(undefined_id), ref(0)
  {
  }
  message(unsigned type, long long ref=0) : type(type), ref(ref)
  {
    switch ( type )
    {
      case device_list_req_id:
        new (&device_list_req) audio_output_device_list_request();
        break;
      case device_list_res_id:
        new (&device_list_res) audio_output_device_list_response();
        break;
      case device_id:
        new (&device) audio_output_device();
        break;
      case open_req_id:
        new (&open_req) audio_output_open_request();
        break;
      case open_res_id:
        new (&open_res) audio_output_open_response();
        break;
      case close_req_id:
        new (&close_req) audio_output_close_request();
        break;
      case close_res_id:
        new (&close_res) audio_output_close_response();
        break;
      case play_req_id:
        new (&play_req) play_request();
        break;
      case queue_req_id:
        new (&queue_req) queue_request();
        break;
      case stream_begin_id:
        new (&stream_begin) audio_output_stream_begin();
        break;
      case stream_end_id:
        new (&stream_end) audio_output_stream_end();
        break;
      case stream_buffer_id:
        new (&stream_buffer) audio_output_stream_buffer();
        break;
      case stream_begin_notify_id:
        new (&stream_begin_notify) audio_output_stream_begin_notification();
        break;
      case stream_end_notify_id:
        new (&stream_end_notify) audio_output_stream_end_notification();
        break;
      case stream_progress_notify_id:
        new (&stream_progress_notify) audio_output_stream_progress_notification();
        break;
      case subscribe_id:
        new (&subscribe) audio_output_subscribe_message();
        break;
      case unsubscribe_id:
        new (&unsubscribe) audio_output_unsubscribe_message();
        break;
      case queue_update_id:
        new (&queue_update) queue_update_notification();
        break;
      case stream_data_req_id:
        new (&stream_data_req) stream_data_request();
        break;
      case stream_data_res_id:
        new (&stream_data_res) stream_data_response();
        break;
    }
  }
public:
  message(message&& other)
  {
    switch( other.type )
    {
      case device_list_req_id:
        new (&device_list_req) audio_output_device_list_request(std::move(other.device_list_req));
        break;
      case device_list_res_id:
        new (&device_list_res) audio_output_device_list_response(std::move(other.device_list_res));
        break;
      case device_id:
        new (&device) audio_output_device(std::move(other.device));
        break;
      case open_req_id:
        new (&open_req) audio_output_open_request(std::move(other.open_req));
        break;
      case open_res_id:
        new (&open_res) audio_output_open_response(std::move(other.open_res));
        break;
      case close_req_id:
        new (&close_req) audio_output_close_request(std::move(other.close_req));
        break;
      case close_res_id:
        new (&close_res) audio_output_close_response(std::move(other.close_res));
        break;
      case play_req_id:
        new (&play_req) play_request(std::move(other.play_req));
        break;
      case queue_req_id:
        new (&queue_req) queue_request(std::move(other.queue_req));
        break;
      case stream_begin_id:
        new (&stream_begin) audio_output_stream_begin(std::move(other.stream_begin));
        break;
      case stream_end_id:
        new (&stream_end) audio_output_stream_end(std::move(other.stream_end));
        break;
      case stream_buffer_id:
        new (&stream_buffer) audio_output_stream_buffer(std::move(other.stream_buffer));
        break;
      case stream_begin_notify_id:
        new (&stream_begin_notify) audio_output_stream_begin_notification(std::move(other.stream_begin_notify));
        break;
      case stream_end_notify_id:
        new (&stream_end_notify) audio_output_stream_end_notification(std::move(other.stream_end_notify));
        break;
      case stream_progress_notify_id:
        new (&stream_progress_notify) audio_output_stream_progress_notification(std::move(other.stream_progress_notify));
        break;
      case subscribe_id:
        new (&subscribe) audio_output_subscribe_message(std::move(other.subscribe));
        break;
      case unsubscribe_id:
        new (&unsubscribe) audio_output_unsubscribe_message(std::move(other.unsubscribe));
        break;
      case queue_update_id:
        new (&queue_update) queue_update_notification(std::move(other.queue_update));
        break;
      case stream_data_req_id:
        new (&stream_data_req) stream_data_request(std::move(other.stream_data_req));
        break;
      case stream_data_res_id:
        new (&stream_data_res) stream_data_response(std::move(other.stream_data_res));
        break;
    }
    type = other.type;
    ref = other.ref;
  }
public:
  ~message()
  {
    switch ( type )
    {
      case device_list_req_id:
        device_list_req.~audio_output_device_list_request();
        break;
      case device_list_res_id:
        device_list_res.~audio_output_device_list_response();
        break;
      case device_id:
        device.~audio_output_device();
        break;
      case open_req_id:
        open_req.~audio_output_open_request();
        break;
      case open_res_id:
        open_res.~audio_output_open_response();
        break;
      case close_req_id:
        close_req.~audio_output_close_request();
        break;
      case close_res_id:
        close_res.~audio_output_close_response();
        break;
      case play_req_id:
        play_req.~play_request();
        break;
      case queue_req_id:
        queue_req.~queue_request();
        break;
      case stream_begin_id:
        stream_begin.~audio_output_stream_begin();
        break;
      case stream_end_id:
        stream_end.~audio_output_stream_end();
        break;
      case stream_buffer_id:
        stream_buffer.~audio_output_stream_buffer();
        break;
      case stream_begin_notify_id:
        stream_begin_notify.~audio_output_stream_begin_notification();
        break;
      case stream_end_notify_id:
        stream_end_notify.~audio_output_stream_end_notification();
        break;
      case stream_progress_notify_id:
        stream_progress_notify.~audio_output_stream_progress_notification();
        break;
      case subscribe_id:
        subscribe.~audio_output_subscribe_message();
        break;
      case unsubscribe_id:
        unsubscribe.~audio_output_unsubscribe_message();
        break;
      case queue_update_id:
        queue_update.~queue_update_notification();
        break;
      case stream_data_req_id:
        stream_data_req.~stream_data_request();
        break;
      case stream_data_res_id:
        stream_data_res.~stream_data_response();
        break;
    }
  }
public:
  unsigned type;
  union
  {
    audio_output_device_list_request device_list_req;
    audio_output_device_list_response device_list_res;
    audio_output_device device;
    audio_output_open_request open_req;
    audio_output_open_response open_res;
    audio_output_close_request close_req;
    audio_output_close_response close_res;
    play_request play_req;
    queue_request queue_req;
    audio_output_stream_begin stream_begin;
    audio_output_stream_end stream_end;
    audio_output_stream_buffer stream_buffer;
    audio_output_stream_begin_notification stream_begin_notify;
    audio_output_stream_end_notification stream_end_notify;
    audio_output_stream_progress_notification stream_progress_notify;
    audio_output_subscribe_message subscribe;
    audio_output_unsubscribe_message unsubscribe;
    queue_update_notification queue_update;
    stream_data_request stream_data_req;
    stream_data_response stream_data_res;
  };
  //long long ref;
  unsigned ref;
};

// ----------------------------------------------------------------------------
#endif
