// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#include "player_notification_sender.h"
#include "source_spotify_task.h"
#include "message.h"
#include "player.h"

// ----------------------------------------------------------------------------
#include "../dm/source_spotify.h"

// ----------------------------------------------------------------------------
#include <libspotify/api.h>

// ----------------------------------------------------------------------------
extern const uint8_t g_appkey[];
extern const size_t  g_appkey_size;

// ----------------------------------------------------------------------------
namespace musciteer
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
  // Spotify session message id.

  enum class spotify_session_message_id
  {
    quit,
    play_session,
    begin_session,
    progress_session,
    end_session,
    process_events,
    logged_in,
    connection_error,
    metadata_updated,
    track_loaded,
    token_lost,
    stream_error
  };

  /////
  // Spotify session.

  class spotify_session : public dripcore::task
  {
    using message_id = spotify_session_message_id;
    using notification_sender = musciteer::player_notification_sender;
  public:
    class message
    {
    public:
      message(message_id id) : id(id), error(SP_ERROR_OK)
      {
      }
      message(message_id id, sp_error error) : id(id), error(error)
      {
      }
      message(message_id id, std::shared_ptr<player_session> session)
        : id(id), session(session)
      {
      }
    public:
      ~message() = default;
    public:
      message_id id;
      sp_error error;
      std::shared_ptr<player_session> session;
    };
  public:
    spotify_session(dripcore::channel<message> ch, done_ochannel done_ch);
  private:
    void main() final;
  private:
    void play_session(std::shared_ptr<player_session>);
    void begin_session();
    void progress_session();
    void end_session();
    void process_events();
    void logged_in(sp_error error);
    void connection_error(sp_error error);
    void metadata_updated();
    void track_loaded();
    void release_track();
    void session_error(const char* message);
  private:
    template<typename... M_args>
    static void notify(sp_session* session, M_args... message_args)
    {
      auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
      self->ch_.send(message{std::forward<M_args>(message_args)...});
    }
  private:
    template<typename... M_args>
    void send(M_args... message_args)
    {
      ch_.send(message{std::forward<M_args>(message_args)...});
    }
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
    done_ochannel done_ch_;
  private:
    sp_track* track_;
    bool track_loaded_;
  private:
    std::shared_ptr<player_session> player_session_;
    dm::track_source source_;
    std::unique_ptr<notification_sender> notifier_;
  private:
    static constexpr const char* name = "spotify";
  };

  /////
  // spotify_session implementation.

  spotify_session::spotify_session(dripcore::channel<message> ch, done_ochannel done_ch)
    : session_(nullptr), ch_(ch), done_ch_(done_ch, this), track_(nullptr), track_loaded_(false)
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
      &get_audio_buffer_stats_cb,
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

    musciteer::dm::source_spotify settings{};

    auto& username = settings.username();
    auto& password = settings.password();

    sp_session_login(session_, username.c_str(), password.c_str(), 0, 0);
  }

  void spotify_session::main()
  {
    while ( true )
    {
      auto msg = ch_.recv(this);

      if ( msg.id == message_id::quit )
      {
        // NOTE: segfaults sometimes!
        // http://stackoverflow.com/questions/14350355/libspotify-destruction-procedure
        sp_session_release(session_);
        break;
      }

      switch ( msg.id )
      {
        case message_id::quit:
          break;
        case message_id::play_session:
          play_session(msg.session);
          break;
        case message_id::begin_session:
          begin_session();
          break;
        case message_id::progress_session:
          progress_session();
          break;
        case message_id::end_session:
          end_session();
          break;
        case message_id::process_events:
          process_events();
          break;
        case message_id::logged_in:
          logged_in(msg.error);
          break;
        case message_id::connection_error:
          connection_error(msg.error);
          break;
        case message_id::metadata_updated:
          metadata_updated();
          break;
        case message_id::track_loaded:
          if ( !track_loaded_ )
          {
            track_loaded_ = true;
            track_loaded();
          }
          break;
        case message_id::token_lost:
          session_error("token_lost");
          break;
        case message_id::stream_error:
          session_error(sp_error_message(msg.error));
          break;
      }
    }

    done_ch_.send(true);
  }

  void spotify_session::play_session(std::shared_ptr<player_session> ps)
  {
    assert(!player_session_);

    player_session_ = ps;

    auto track = player_session_->track();
    assert(track);

    notifier_.reset(new notification_sender(player_session_->id(), player_session_->get_notification_channel()));

    source_ = track->sources_get("spotify");

    spotify_link link(source_.uri());

    if ( link.is_track() )
    {
      sp_track_add_ref(track_ = link.as_track());

      if (sp_track_error(track_) == SP_ERROR_OK) {
        send(message_id::track_loaded);
      }
    }
    else
    {
      // TODO: Unsupported uri.
    }
  }

  void spotify_session::begin_session()
  {
    if ( notifier_ )
    {
      notifier_->stream_begin(player_session_->get_audio_output());
    }
    else {
      std::cerr << "spotify session - begin_session - notifier_ is null" << std::endl;
    }
  }

  void spotify_session::progress_session()
  {
    if ( notifier_ ) {
      notifier_->stream_progress();
    }
    else {
      std::cerr << "spotify session - progress_session - notifier_ is null" << std::endl;
    }
  }

  void spotify_session::end_session()
  {
    if ( player_session_ )
    {
      sp_session_player_unload(session_);

      auto output = player_session_->get_audio_output();
      output.drain();

      release_track();
      player_session_.reset();
    }
    else
    {
      std::cerr << "spotify session - player_session_ is null" << std::endl;
    }

    if ( notifier_ )
    {
      notifier_->stream_end();
      notifier_.reset();
    }
    else {
      std::cerr << "spotify session - end_session - notifier_ is null" << std::endl;
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
    auto player = musciteer::player();

    player.source_notification(
      source_notification::id::status,
      name,
      sp_error_message(error)
    );
  }

  void spotify_session::connection_error(sp_error error)
  {
    auto player = musciteer::player();

    player.source_notification(
      source_notification::id::error,
      name,
      sp_error_message(error)
    );
  }

  void spotify_session::metadata_updated()
  {
    if (sp_track_error(track_) == SP_ERROR_OK) {
      send(message_id::track_loaded);
    }
  }

  void spotify_session::track_loaded()
  {
    sp_availability avail = sp_track_get_availability(session_, track_);

    if ( avail != SP_TRACK_AVAILABILITY_AVAILABLE )
    {
      release_track();
      player_session_.reset();

      auto player = musciteer::player();

      player.source_notification(
        source_notification::id::track_unavailable,
        name,
        "track unavailable"
      );
    }
    else
    {
      sp_error err;

      if ( (err = sp_session_player_load(session_, track_)) != SP_ERROR_OK ) {
        session_error(sp_error_message(err));
        return;
      }

      auto track = player_session_->track();

      assert(track);
      assert(notifier_);

      notifier_->set_stream_length(std::chrono::milliseconds(track->duration()));

      auto output = player_session_->get_audio_output();
      auto replaygain = source_.rg_track_gain();
      auto replaygain_peak = source_.rg_track_peak();

      output.set_replaygain((replaygain ? replaygain.value() : 0), (replaygain_peak ? replaygain_peak.value() : 1));
      output.set_params(2, 44100);
      output.prepare();

      if ( (err = sp_session_player_play(session_, 1)) != SP_ERROR_OK ) {
        session_error(sp_error_message(err));
      }
    }
  }

  void spotify_session::release_track()
  {
    if ( track_ )
    {
      sp_track_release(track_);
      track_ = 0;
      track_loaded_ = false;
    }
  }

  void spotify_session::session_error(const char* message)
  {
    auto player = musciteer::player();

    player.source_notification(
      source_notification::id::session_error,
      name,
      message
    );
  }

  /////
  // spotify session callbacks.

  void spotify_session::logged_in_cb(sp_session *session, sp_error error)
  {
    notify(session, message_id::logged_in, error);
  }

  void spotify_session::logged_out_cb(sp_session *session)
  {
    std::cerr << "spotify session logged_out_cb" << std::endl;
  }

  void spotify_session::metadata_updated_cb(sp_session *session)
  {
    notify(session, message_id::metadata_updated);
  }

  void spotify_session::connection_error_cb(sp_session *session, sp_error error)
  {
    notify(session, message_id::connection_error, error);
  }

  void spotify_session::message_to_user_cb(sp_session *session, const char* message)
  {
    std::cerr << "spotify session message to user " << message << std::endl;
  }

  void spotify_session::notify_main_thread_cb(sp_session *session)
  {
    notify(session, message_id::process_events);
  }

  int spotify_session::music_delivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames)
  {
    auto self = reinterpret_cast<spotify_session*>(sp_session_userdata(session));
    auto player_session = self->player_session_;

    if ( player_session )
    {
      auto samples = reinterpret_cast<const s16_le_i_frame*>(frames);
      auto i = 0;
      auto output = player_session->get_audio_output();
      auto avail = output.avail_update();
      auto len = std::min(num_frames, static_cast<int>(avail));
      auto remaining = len;
      auto scale = (1<<16) * output.get_replaygain_scale();

      if ( avail < 0 ) {
        std::cerr << "spotify_session::music_delivery error avail=" << avail << std::endl;
      }

      while ( remaining > 0 )
      {
        const snd_pcm_channel_area_t* areas;
        snd_pcm_uframes_t offset;
        snd_pcm_uframes_t frames = remaining;

        output.mmap_begin(&areas, &offset, &frames);

        auto obuf = reinterpret_cast<s32_le_i_frame*>(areas[0].addr) + offset;

        for( size_t x = 0; x < frames; x++, i++ )
        {
          obuf[x].l = static_cast<int32_t>(samples[i].l) * scale;
          obuf[x].r = static_cast<int32_t>(samples[i].r) * scale;
        }

        auto committed = output.mmap_commit(offset, frames);

        remaining -= committed;
      }

      if ( output.is_prepared() )
      {
        output.start();
        notify(session, message_id::begin_session);
      }
      else {
        notify(session, message_id::progress_session);
      }

      return len;
    }
    else {
      return 0;
    }
  }

  void spotify_session::play_token_lost_cb(sp_session *session)
  {
    notify(session, message_id::token_lost);
  }

  void spotify_session::log_message_cb(sp_session *session, const char* data)
  {
    //std::cout << "spotify session log " << data << std::endl;
  }

  void spotify_session::end_of_track_cb(sp_session *session)
  {
    notify(session, message_id::end_session);
  }

  void spotify_session::stream_error_cb(sp_session *session, sp_error error)
  {
    notify(session, message_id::stream_error, error);
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
    stats->samples = 1024;
    stats->stutter = 0;
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

  source_spotify_task::source_spotify_task(session_channel channel, done_ochannel done_ch)
    : ch_(channel), done_ch_(done_ch, this)
  {
  }

  void source_spotify_task::main()
  {
    using message = spotify_session::message;

    dripcore::channel<message> spotify_session_ch;
    done_ichannel spotify_session_done_ch{done_channel{}, this};

    auto spotify_session_task = spawn<spotify_session>(spotify_session_ch, spotify_session_done_ch);

    while ( true )
    {
      auto session = ch_.recv(this);

      if ( !session )
      {
        spotify_session_ch.send(message{spotify_session_message_id::quit}, this);

        if ( !spotify_session_task.expired() )  {
          spotify_session_done_ch.recv();
        }
        break;
      }

      // Request spotify session to play session.
      spotify_session_ch.send(message{spotify_session_message_id::play_session, session}, this);

      // Process session control messages until session done.
      auto ctrl = player_session::control::undefined;
      do
      {
        ctrl = session->recv(this);

        if ( ctrl == player_session::control::stop )
        {
          spotify_session_ch.send(message{spotify_session_message_id::end_session}, this);
        }
      }
      while( ctrl != player_session::control::done );
    }

    done_ch_.send(true);
  }
}
