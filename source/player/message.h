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
#include <dripcore/channel.h>
#include <dripcore/task.h>

// ----------------------------------------------------------------------------
//#include <msgpack/istream.h>
//#include <msgpack/ostream.h>
//#include <msgpack/array.h>
//#include <msgpack/map.h>

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
#if 0
public:
  void write(msgpack::ostream& os)
  {
    msgpack::map map{2};

    os << map
      << "error_code"  << error_code
      << "error_message" << error_message;
  }
#endif
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
    channel = std::move(other.channel);
  }
public:
  message_channel channel;
};

// ----------------------------------------------------------------------------
class audio_output_subscribe : public audio_output_notification_message
{
};

// ----------------------------------------------------------------------------
class audio_output_unsubscribe : public audio_output_notification_message
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
    reply = std::move(other.reply);
  }
#if 0
public:
  void read(msgpack::istream& is)
  {
  }
#endif
public:
  message_channel reply;
};

#if 0
// ----------------------------------------------------------------------------
inline msgpack::istream& operator>>(msgpack::istream& is, audio_output_device_list_request& value)
{
  value.read(is);
  return is;
}
#endif

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
  }
#if 0
public:
  void write(msgpack::ostream& os)
  {
    msgpack::map map{1};

    os << map
      << "devices" << device_names;
  }
#endif
public:
  std::vector<std::string> device_names;
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
#if 0
public:
  void read(msgpack::istream& is)
  {
    msgpack::map map;

    if ( is >> map )
    {
      for ( size_t i=0; i<map.size(); i++ )
      {
        std::string key;

        if ( is >> key )
        {
          if ( key == "device_name") {
            is >> device_name;
          }
          else {
            // ignore unknown keys.
          }
        }
        else
        {
          break;
        }
      }
    }
  }
#endif
public:
  std::string device_name;
  message_channel reply;
};

#if 0
// ----------------------------------------------------------------------------
inline msgpack::istream& operator>>(msgpack::istream& is, audio_output_open_request& value)
{
  value.read(is);
  return is;
}
#endif

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
#if 0
public:
  void read(msgpack::istream& is)
  {
  }
#endif
public:
  message_channel reply;
};

#if 0
// ----------------------------------------------------------------------------
inline msgpack::istream& operator>>(msgpack::istream& is, audio_output_close_request& value)
{
  value.read(is);
  return is;
}
#endif

// ----------------------------------------------------------------------------
using audio_output_close_response = audio_output_response;

// ----------------------------------------------------------------------------
class play_request
{
public:
  play_request() : uri(), stream_id(0)
  {
  }
  play_request(play_request&& other)
  {
    uri = std::move(other.uri);
    stream_id = std::move(other.stream_id);
    reply = std::move(other.reply);
  }
#if 0
public:
  void read(msgpack::istream& is)
  {
    msgpack::map map;

    if ( is >> map )
    {
      for ( size_t i=0; i<map.size(); i++ )
      {
        std::string key;

        if ( is >> key )
        {
          if ( key == "uri") {
            is >> uri;
          }
          else if ( key == "stream_id" )
          {
            is >> stream_id;
          }
          else {
            // ignore unknown keys.
          }
        }
        else
        {
          break;
        }
      }
    }
  }
#endif
public:
  std::string uri;
  unsigned stream_id;
  message_channel reply;
};

#if 0
// ----------------------------------------------------------------------------
inline msgpack::istream& operator>>(msgpack::istream& is, play_request& value)
{
  value.read(is);
  return is;
}
#endif

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
#if 0
public:
  void write(msgpack::ostream& os)
  {
    msgpack::map map{1};

    os << map
      << "stream_id" << stream_id;
  }
#endif
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
#if 0
public:
  void write(msgpack::ostream& os)
  {
    msgpack::map map{1};

    os << map
      << "stream_id" << stream_id;
  }
#endif
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
#if 0
public:
  void write(msgpack::ostream& os)
  {
    msgpack::map map{3};

    os << map
      << "stream_id" << stream_id
      << "duration" << duration
      << "length" << length;
  }
#endif
public:
  unsigned stream_id;
  unsigned duration;
  unsigned length;
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
    open_req_id = 3,
    open_res_id = 4,
    close_req_id = 5,
    close_res_id = 6,
    play_req_id = 7,
    stream_begin_id = 8,
    stream_end_id = 9,
    stream_buffer_id = 10,
    stream_begin_notify_id = 11,
    stream_end_notify_id = 12,
    stream_progress_notify_id = 13,
    subscribe_id = 14,
    unsubscribe_id = 15,
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
        new (&subscribe) audio_output_subscribe();
        break;
      case unsubscribe_id:
        new (&unsubscribe) audio_output_unsubscribe();
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
        new (&subscribe) audio_output_subscribe(std::move(other.subscribe));
        break;
      case unsubscribe_id:
        new (&unsubscribe) audio_output_unsubscribe(std::move(other.unsubscribe));
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
        subscribe.~audio_output_subscribe();
        break;
      case unsubscribe_id:
        unsubscribe.~audio_output_unsubscribe();
        break;
    }
  }
public:
  unsigned type;
  union
  {
    audio_output_device_list_request device_list_req;
    audio_output_device_list_response device_list_res;
    audio_output_open_request open_req;
    audio_output_open_response open_res;
    audio_output_close_request close_req;
    audio_output_close_response close_res;
    play_request play_req;
    audio_output_stream_begin stream_begin;
    audio_output_stream_end stream_end;
    audio_output_stream_buffer stream_buffer;
    audio_output_stream_begin_notification stream_begin_notify;
    audio_output_stream_end_notification stream_end_notify;
    audio_output_stream_progress_notification stream_progress_notify;
    audio_output_subscribe subscribe;
    audio_output_unsubscribe unsubscribe;
  };
  //long long ref;
  unsigned ref;
};

// ----------------------------------------------------------------------------
#endif
