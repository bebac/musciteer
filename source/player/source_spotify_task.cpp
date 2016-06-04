// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "source_spotify_task.h"
#include "message.h"
#include "audio_buffer.h"
#include "audio_output.h"

// ----------------------------------------------------------------------------
#include <libspotify/api.h>

// ----------------------------------------------------------------------------
extern const uint8_t g_appkey[];
extern const size_t  g_appkey_size;

// ----------------------------------------------------------------------------
namespace musicbox
{
  /////
  // Spotify error.

  class spotify_error : public std::runtime_error
  {
  private:
    const sp_error& error;
  public:
    spotify_error(const sp_error& error)
      : std::runtime_error(sp_error_message(error)), error(error)
    {
    }
  };

  /////
  // Spotify link.

  class spotify_link
  {
  public:
    spotify_link(const std::string uri) : link_(nullptr)
    {
      link_ = sp_link_create_from_string(uri.c_str());
    }
  public:
    ~spotify_link()
    {
      if ( link_ )
      {
        sp_link_release(link_);
      }
    }
  public:
    operator bool() const
    {
      return !!link_;
    }
  public:
    bool is_track()
    {
      return sp_link_type(link_) == SP_LINKTYPE_TRACK;
    }
  public:
    sp_track* as_track()
    {
      return sp_link_as_track(link_);
    }
  private:
    sp_link* link_;
  };

  /////
  // Spotify session.

  class spotify_session : public dripcore::task
  {
    enum class atom
    {
      quit,
      process_events,
      logged_in,
      metadata_updated,
      track_loaded
    };
  public:
    spotify_session();
  private:
    void main() final;
  private:
    static void logged_in_cb(sp_session *session, sp_error error);
    static void logged_out_cb(sp_session *session);
    static void metadata_updated_cb(sp_session *session);
    static void connection_error_cb(sp_session *session, sp_error error);
    static void message_to_user_cb(sp_session *session, const char* message);
    static void notify_main_thread_cb(sp_session *session);
    static int music_delivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames);
    static void play_token_lost_cb(sp_session *session);
    static void log_message_cb(sp_session *session, const char* data);
    static void end_of_track_cb(sp_session *session);
    static void stream_error_cb(sp_session *session, sp_error error);
    static void user_info_updated_cb(sp_session *session);
    static void start_playback_cb(sp_session *session);
    static void stop_playback_cb(sp_session *session);
    static void get_audio_buffer_stats_cb(sp_session *session, sp_audio_buffer_stats *stats);
    static void offline_status_updated_cb(sp_session *session);
    static void offline_error_cb(sp_session *session, sp_error error);
    static void credentials_blob_updated_cb(sp_session *session, const char* blob);
  private:
    sp_session* session_;
  private:
    dripcore::channel<atom> ch_;
  };

  /////
  // spotify_session implementation.

  spotify_session::spotify_session()
  {
    sp_session_callbacks callbacks = {
      &logged_in_cb,
      &logged_out_cb,
      &metadata_updated_cb,
      &connection_error_cb,
      &message_to_user_cb,
      &notify_main_thread_cb,
      &music_delivery,
      &play_token_lost_cb,
      &log_message_cb,
      &end_of_track_cb,
      &stream_error_cb,
      0, //&user_info_updated_cb,
      &start_playback_cb,
      &stop_playback_cb,
      &get_audio_buffer_stats_cb,
      0, //&offline_status_updated_cb,
      0, //&offline_error_cb,
      &credentials_blob_updated_cb
    };

    sp_session_config config;

    config.callbacks = &callbacks;
    config.api_version = SPOTIFY_API_VERSION;
    //config.cache_location = m_cache_dir.c_str();
    config.cache_location = "/tmp";
    //config.settings_location = m_cache_dir.c_str();
    config.settings_location = "/tmp";
    config.application_key = g_appkey;
    config.application_key_size = g_appkey_size;
    config.user_agent = "spotd";
    config.userdata = this;
    config.compress_playlists = false;
    config.dont_save_metadata_for_playlists = true;
    config.initially_unload_playlists = false;
    config.device_id = 0;
    config.proxy = "";
    config.proxy_username = 0;
    config.proxy_password = 0;
    config.ca_certs_filename = 0;
    config.tracefile = 0;

    sp_error error = sp_session_create(&config, &session_);
    if ( SP_ERROR_OK != error ) {
      throw spotify_error(error);
    }

    error = sp_session_preferred_bitrate(session_, SP_BITRATE_320k);
    if ( SP_ERROR_OK != error ) {
      throw spotify_error(error);
    }

    error = sp_session_set_volume_normalization(session_, false);
    if ( SP_ERROR_OK != error ) {
      throw spotify_error(error);
    }

    sp_session_login(session_, "xxxxxxxx", "xxxxxxxx", 0, 0);
  }

  void spotify_session::main()
  {
    while ( true )
    {
      auto msg = ch_.recv(this);

      if ( msg == atom::quit ) {
        break;
      }

      switch ( msg )
      {
        case atom::quit:
          break;
        case atom::process_events:
        {
          int next_timeout;
          do
          {
            sp_session_process_events(session_, &next_timeout);
          } while (next_timeout == 0);
          break;
        }
        case atom::logged_in:
        {
          std::cout << "spotify session logged in" << std::endl;
          break;
        }
        case atom::metadata_updated:
        {
          std::cout << "spotify session metadata updated" << std::endl;
          break;
        }
        case atom::track_loaded:
        {
          std::cout << "track loaded" << std::endl;
          break;
        }
      }
    }
  }

  /////
  // spotify session callbacks.

  void spotify_session::logged_in_cb(sp_session *session, sp_error error)
  {
    auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    self->ch_.send(atom::logged_in);
  }

  void spotify_session::logged_out_cb(sp_session *session)
  {
  }

  void spotify_session::metadata_updated_cb(sp_session *session)
  {
    auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    self->ch_.send(atom::metadata_updated);
  }

  void spotify_session::connection_error_cb(sp_session *session, sp_error error)
  {
    std::cout << "spotify session connection error!" << sp_error_message(error) << std::endl;
  }

  void spotify_session::message_to_user_cb(sp_session *session, const char* message)
  {
    std::cout << "spotify session message to user " << message << std::endl;
  }

  void spotify_session::notify_main_thread_cb(sp_session *session)
  {
    auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    self->ch_.send(atom::process_events);
  }

  int spotify_session::music_delivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames)
  {
    //std::cerr << "spotify_session::music_delivery num_frames=" << num_frames << std::endl;
    return num_frames;
  }

  void spotify_session::play_token_lost_cb(sp_session *session)
  {
    std::cout << "spotify session token lost" << std::endl;
  }

  void spotify_session::log_message_cb(sp_session *session, const char* data)
  {
    std::cout << "spotify session log " << data << std::endl;
  }

  void spotify_session::end_of_track_cb(sp_session *session)
  {
    std::cout << "spotify session end-of-track" << std::endl;
  }

  void spotify_session::stream_error_cb(sp_session *session, sp_error error)
  {
    std::cout << "spotify session stream error!" << sp_error_message(error) << std::endl;
  }

  void spotify_session::user_info_updated_cb(sp_session *session)
  {
  }

  void spotify_session::start_playback_cb(sp_session *session)
  {
    // NOTE: Doesn't work without implmenting get_audio_buffer_stats_cb
    //auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    //self->ch_.send(atom::start_playback);
  }

  void spotify_session::stop_playback_cb(sp_session *session)
  {
    // NOTE: Doesn't work without implmenting get_audio_buffer_stats_cb
    //auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    //self->ch_.send(atom::stop_playback);
  }

  void spotify_session::get_audio_buffer_stats_cb(sp_session *session, sp_audio_buffer_stats *stats)
  {
  }

  void spotify_session::offline_status_updated_cb(sp_session *session)
  {
  }

  void spotify_session::offline_error_cb(sp_session *session, sp_error error)
  {
  }

  void spotify_session::credentials_blob_updated_cb(sp_session *session, const char* blob)
  {
  }

  /////
  // source_spotify_task implementation.

  source_spotify_task::source_spotify_task(session_channel channel) : ch_(channel)
  {
  }

  void source_spotify_task::main()
  {
    spawn<spotify_session>();

    while ( true )
    {
      auto session = ch_.recv(this);

      if ( !session ) {
        break;
      }

      auto track = session->track();
      assert(track);

      auto sources = track->sources();
    }
  }
}
