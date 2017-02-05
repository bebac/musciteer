require 'opal'
require 'browser'
require 'browser/socket'
require 'browser/http'
require 'browser/location'
require 'browser/history'
require 'browser/interval'
require 'browser/delay'
require 'browser/effects'
require 'browser/animation_frame'
require 'json'

require_relative 'maquette'
require_relative 'store'
require_relative 'player'

require_tree './models'
require_tree './components'

class Application
  def initialize
    @store = Store.new { |state, action|
      unless state
        {
          path:                    nil,
          player_state:            :stopped,
          audio_settings:          { current: nil, devices: [] },
          ctpb_settings:           { enabled: nil, type: [] },
          replaygain_enabled:      { enabled: nil },
          local_source_settings:   { status: "ok", directories: [] },
          spotify_source_settings: { status: "unknown", username: "", password: "" },
          local_source_scanning:   false,
          tracks_loading:          false,
          tracks:                  false,
          albums_loading:          false,
          albums:                  false,
          album_details_loading:   false,
          album_details:           false,
          artist_loading:          false,
          artist:                  false,
          notification:            nil,
          stream_sync:             false,
          stream:                  {},
          spotify_importing:       false,
          spotify_web:             { authorized: false }
        }
      else
        case action[:type]
        when :set_path
          state.merge({
            path: action[:data]
          })
        when :player_playing
          state.merge({
            player_state: :playing
          })
        when :player_stopped
          state.merge({
            player_state: :stopped, stream_sync: false, stream: {}
          })
        when :tracks_load
          state.merge({
            tracks_loading: true
          })
        when :tracks_load_success
          state.merge({
            tracks_loading: false,
            tracks: action[:data]
          })
        when :albums_load
          state.merge({
            albums_loading: true
          })
        when :albums_load_success
          state.merge({
            albums_loading: false,
            albums: action[:data]
          })
        when :album_details_load
          state.merge({
            album_details_loading: true,
            album_details: action[:data]
          })
        when :album_details_load_success
          case data = action[:data]
          when Album
            # No state change.
            state
          when Array
            # Oops, we are mutating the data! The important thing is that we
            # create a new state instance
            if album = state[:album_details]
              album.tracks = data
            else
              fail("state album_details is not an album!")
            end
            state.merge({
              album_details_loading: false,
              album_details: album
            })
          end
        when :artist_load_success
          state.merge({
            artist_loading: false,
            artist: action[:data]
          })
        when :audio_settings_success
          state.merge({
            audio_settings: action[:data]
          })
        when :ctpb_settings_success
          state.merge({
            ctpb_settings: action[:data]
          })
        when :audio_replaygain_success
          state.merge({
            replaygain_enabled: action[:data][:enabled]
          })
        when :local_source_settings_success
          state.merge({
            local_source_settings: { status: "ok", directories: action[:data] }
          })
        when :local_source_scan
          state.merge({
            local_source_scanning: true
          })
        when :local_source_scan_success
          state.merge({
            local_source_scanning: false
          })
        when :spotify_source_settings_success
          settings = state[:spotify_source_settings]
          state.merge({
            spotify_source_settings: settings.merge(action[:data])
          })
        when :spotify_source_notify
          settings = state[:spotify_source_settings]
          state.merge({
            spotify_source_settings: settings.merge({ status: action[:data] })
          })
        when :spotify_web_settings_success
          state.merge({
            spotify_web: action[:data]
          })
        when :stream_begin
          state.merge({
            stream: action[:data]
          })
        when :stream_sync
          stream = state[:stream] || fail("stream is nil on stream_sync")
          data   = action[:data]  || fail("action data is nil on stream_sync")
          state.merge({
            stream_sync: true,
            notification: data,
            stream: stream.merge({ stream_id: data.stream_id, track: data.track })
          })
        when :stream_progress
          stream = state[:stream] || fail("stream is nil on stream_progress")
          state.merge({
            stream: stream.merge(action[:data])
          })
        when :stream_end
          # Clear notification if it's a stream begin notification.
          notification = state[:notification]
          if notification
            notification = nil if notification.instance_of?(StreamBegin)
          end
          # Update state.
          state.merge({
            stream_sync: false, notification: notification
          })
        when :notification
          state.merge({
            notification: action[:data]
          })
        when :spotify_import
          state.merge({
            spotify_importing: true
          })
        when :spotify_import_success
          state.merge({
            spotify_importing: false
          })
        when :render
          @projector.render
          state
        else
          puts "unhandled action #{action[:type]}, data #{action[:data]}"
          state
        end
      end
    }
    @layout = Layout.new(@store)
  end

  def run
    @projector = Maquette::Projector.new
    @projector.append($document.body, -> { @layout.render })
    @store.subscribe { @projector.render }
  end
end

$document.ready do
  Application.new.run
end
