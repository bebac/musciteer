// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include "source_spotify_task.h"
#include "message.h"
#include "audio_buffer.h"
#include "audio_output.h"
#include "player.h"

// ----------------------------------------------------------------------------
#include "../dm/source_spotify.h"

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
  public:
    enum class atom
    {
      quit,
      play_session,
      end_session,
      process_events,
      logged_in,
      connection_error,
      metadata_updated,
      track_loaded
    };
  public:
    class message
    {
    public:
      message(atom id) : id(id)
      {
      }
      message(atom id, sp_error error) : id(id), error(error)
      {
      }
      message(atom id, std::shared_ptr<player_session> session)
        : id(id), session(session)
      {
      }
    public:
      atom id;
      sp_error error;
      std::shared_ptr<player_session> session;
    };
  public:
    spotify_session(dripcore::channel<message> ch, done_channel done_ch);
  private:
    void main() final;
  private:
    void init_audio_buffer_channel();
  private:
    void play_session(std::shared_ptr<player_session>);
    void end_session();
    void process_events();
    void logged_in(sp_error error);
    void connection_error(sp_error error);
    void metadata_updated();
    void track_loaded();
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
    dripcore::channel<message> ch_;
    done_channel done_ch_;
  private:
    sp_track* track_;
    bool track_loaded_;
  private:
    std::shared_ptr<player_session> player_session_;
    dripcore::channel<audio_buffer> buffer_ch_;
  private:
    static constexpr const char* name = "spotify";
  };

  /////
  // spotify_session implementation.

  spotify_session::spotify_session(dripcore::channel<message> ch, done_channel done_ch)
    : session_(nullptr), ch_(ch), done_ch_(done_ch), track_(nullptr), track_loaded_(false)
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
      0, //&start_playback_cb,
      0, //&stop_playback_cb,
      0, //&get_audio_buffer_stats_cb,
      0, //&offline_status_updated_cb,
      0, //&offline_error_cb,
      0, //&credentials_blob_updated_cb
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

    musicbox::dm::source_spotify settings{};

    auto& username = settings.username();
    auto& password = settings.password();

    sp_session_login(session_, username.c_str(), password.c_str(), 0, 0);
  }

  void spotify_session::main()
  {
    init_audio_buffer_channel();

    while ( true )
    {
      auto msg = ch_.recv(this);

      if ( msg.id == atom::quit )
      {
        // NOTE: segfaults sometimes!
        // http://stackoverflow.com/questions/14350355/libspotify-destruction-procedure
        sp_session_release(session_);
        break;
      }

      switch ( msg.id )
      {
        case atom::quit:
          break;
        case atom::play_session:
          play_session(msg.session);
          break;
        case atom::end_session:
          end_session();
          break;
        case atom::process_events:
          process_events();
          break;
        case atom::logged_in:
          logged_in(msg.error);
          break;
        case atom::connection_error:
          connection_error(msg.error);
          break;
        case atom::metadata_updated:
          metadata_updated();
          break;
        case atom::track_loaded:
          if ( !track_loaded_ )
          {
            track_loaded();
            track_loaded_ = true;
          }
          break;
      }
    }

    done_ch_.send(true);
  }

  void spotify_session::init_audio_buffer_channel()
  {
    for ( size_t i=0; i<10; ++i )
    {
      buffer_ch_.send(audio_buffer{}, this);
    }
  }

  void spotify_session::play_session(std::shared_ptr<player_session> ps)
  {
    assert(!player_session_);

    player_session_ = ps;

    auto track = player_session_->track();
    assert(track);

    auto source = track->sources_get("spotify");

    spotify_link link(source.uri());

    if ( link.is_track() )
    {
      sp_track_add_ref(track_ = link.as_track());

      if (sp_track_error(track_) == SP_ERROR_OK) {
        ch_.send(message{atom::track_loaded});
      }
    }
    else
    {
      // TODO: Unsupported uri.
    }
  }

  void spotify_session::end_session()
  {
    if ( player_session_ )
    {
      sp_session_player_unload(session_);

      auto audio_output = player_session_->audio_output();
      assert(audio_output);

      message_channel done;
      ::message m(::message::stream_end_id);

      m.stream_end.reply = done;
      audio_output->send(std::move(m));

      done.recv(this);

      if ( track_ )
      {
        sp_track_release(track_);
        track_ = 0;
        track_loaded_ = false;
      }

      player_session_.reset();
    }
    else
    {
      std::cerr << "spotify session - player_session_ is null" << std::endl;
    }
  }

  void spotify_session::process_events()
  {
    int next_timeout;
    do
    {
      sp_session_process_events(session_, &next_timeout);
    }
    while (next_timeout == 0);
  }

  void spotify_session::logged_in(sp_error error)
  {
    auto player = musicbox::player();

    player.source_notification(
      source_notification::id::status,
      name,
      sp_error_message(error)
    );
  }

  void spotify_session::connection_error(sp_error error)
  {
    auto player = musicbox::player();

    player.source_notification(
      source_notification::id::error,
      name,
      sp_error_message(error)
    );
  }

  void spotify_session::metadata_updated()
  {
    if (sp_track_error(track_) == SP_ERROR_OK) {
      ch_.send(message{atom::track_loaded});
    }
  }

  void spotify_session::track_loaded()
  {
    sp_availability avail = sp_track_get_availability(session_, track_);

    if ( avail != SP_TRACK_AVAILABILITY_AVAILABLE )
    {
      std::cerr << "spotify session - track not available" << std::endl;
    }
    else
    {
      sp_error err;

      if ( (err=sp_session_player_load(session_, track_)) != SP_ERROR_OK ) {
        std::cerr << "sp_session_player_load error " << err << std::endl;
      }

      auto track = player_session_->track();
      assert(track);

      auto audio_output = player_session_->audio_output();
      assert(audio_output);

      ::message m(::message::stream_begin_id);

      m.stream_begin.stream_id = player_session_->id();
      m.stream_begin.sample_rate = 44100;
      m.stream_begin.length = std::chrono::milliseconds(track->duration());
      m.stream_begin.completed_buffer_ch = buffer_ch_;
      audio_output->send(std::move(m));

      if ( (err=sp_session_player_play(session_, 1)) != SP_ERROR_OK ) {
        std::cerr << "sp_session_player_play error " << err << std::endl;
      }
    }
  }

  /////
  // spotify session callbacks.

  void spotify_session::logged_in_cb(sp_session *session, sp_error error)
  {
    auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    self->ch_.send(message{atom::logged_in, error});
  }

  void spotify_session::logged_out_cb(sp_session *session)
  {
    std::cerr << "spotify session logged_out_cb" << std::endl;
  }

  void spotify_session::metadata_updated_cb(sp_session *session)
  {
    auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    self->ch_.send(message{atom::metadata_updated});
  }

  void spotify_session::connection_error_cb(sp_session *session, sp_error error)
  {
    auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    self->ch_.send(message{atom::connection_error, error});
  }

  void spotify_session::message_to_user_cb(sp_session *session, const char* message)
  {
    std::cerr << "spotify session message to user " << message << std::endl;
  }

  void spotify_session::notify_main_thread_cb(sp_session *session)
  {
    auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    self->ch_.send(message{atom::process_events});
  }

  int spotify_session::music_delivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames)
  {
    auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));

    if ( self->player_session_ )
    {
      auto audio_output = self->player_session_->audio_output();
      assert(audio_output);

      // There is a risk that we can block here which is not so good according to
      // docs. Hope it will go if we have enough buffers in the pipe.
      auto buf = self->buffer_ch_.recv();

      buf.clear();
      buf.write_s16_le_i(frames, num_frames);

      audio_output->send(std::move(buf));
    }

    //std::cerr << "spotify_session::music_delivery num_frames=" << num_frames << std::endl;
    return num_frames;
  }

  void spotify_session::play_token_lost_cb(sp_session *session)
  {
    std::cerr << "spotify session token lost" << std::endl;
  }

  void spotify_session::log_message_cb(sp_session *session, const char* data)
  {
    //std::cout << "spotify session log " << data << std::endl;
  }

  void spotify_session::end_of_track_cb(sp_session *session)
  {
    auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    self->ch_.send(message{atom::end_session});
  }

  void spotify_session::stream_error_cb(sp_session *session, sp_error error)
  {
    std::cerr << "spotify session stream error!" << sp_error_message(error) << std::endl;
  }

  void spotify_session::user_info_updated_cb(sp_session *session)
  {
  }

  void spotify_session::start_playback_cb(sp_session *session)
  {
    // NOTE: Doesn't work without implementing get_audio_buffer_stats_cb
    //auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    //self->ch_.send(atom::start_playback);
  }

  void spotify_session::stop_playback_cb(sp_session *session)
  {
    // NOTE: Doesn't work without implementing get_audio_buffer_stats_cb
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

  source_spotify_task::source_spotify_task(session_channel channel, done_channel done_ch)
    : ch_(channel), done_ch_(done_ch)
  {
  }

  void source_spotify_task::main()
  {
    dripcore::channel<spotify_session::message> spotify_session_ch;
    done_channel spotify_session_done_ch;

    auto spotify_session_task = spawn<spotify_session>(spotify_session_ch, spotify_session_done_ch);

    while ( true )
    {
      auto session = ch_.recv(this);

      if ( !session )
      {
        spotify_session_ch.send(spotify_session::message{spotify_session::atom::quit}, this);

        if ( !spotify_session_task.expired() )  {
          spotify_session_done_ch.recv(this);
        }
        break;
      }

      // Request spotify session to play session.
      spotify_session_ch.send(spotify_session::message{spotify_session::atom::play_session, session}, this);

      // Process session control messages until session done.
      auto ctrl = player_session::control::undefined;
      do
      {
        ctrl = session->recv(this);

        if ( ctrl == player_session::control::stop )
        {
          spotify_session_ch.send(spotify_session::message{spotify_session::atom::end_session}, this);
        }
      }
      while( ctrl != player_session::control::done );
    }

    done_ch_.send(true);
  }
}
