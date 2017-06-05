require 'opal'
require 'browser'
require 'browser/socket'
require 'browser/http'
require 'browser/location'
require 'browser/history'
require 'browser/effects'
require 'browser/animation_frame'
require 'browser/interval'
require 'browser/delay'
require 'maquette'

require_tree './models'
require_tree './actions'
require_tree './components'

module Musciteer
  class Application
    include Maquette::Component
    include NavigationActions

    def initialize
      @store = Store.new { |state, action|
        unless state
          init
        else
          reduce(state, action)
        end
      }
      @projector = Maquette::Projector.new
      @header = Header.new(@store)
      @content = Content.new(@store)
      @footer = Footer.new(@store)
      @socket = Websocket.new(@store)
    end

    def init
      {
        component:             nil,
        player_state:          :stopped,
        stream_sync:           false,
        stream:                {},
        albums_loading:        false,
        albums:                nil,
        album_details_loading: false,
        album_details:         nil,
        tracks_loading:        false,
        tracks:                nil
      }
    end

    def reduce(state, action)
      case action[:type]
      when :goto
        state.merge({
          component: component_for(action[:data])
        })
      when :player_play
        socket.send event: :play, data: action[:data]
        state
      when :player_queue
        socket.send event: :queue, data: action[:data]
        state
      when :player_skip
        socket.send event: :skip, data: nil
        state
      when :player_stop
        socket.send event: :stop, data: nil
        state
      when :player_playing
        state.merge({
          player_state: :playing
        })
      when :player_stopped
        state.merge({
          player_state: :stopped
        })
      when :stream_begin
        if data = action[:data]
          socket.send event: :stream_data_sync, data: data[:stream_id]
        end
        state
      when :stream_sync
        stream = state[:stream] || fail("stream is nil on stream_sync")
        data   = action[:data]  || fail("action data is nil on stream_sync")
        state.merge({
          stream_sync: true,
          stream: stream.merge({ stream_id: data[:stream_id], track: Track.new(data[:track]) })
        })
      when :stream_progress
        unless state[:stream_sync]
          if data = action[:data]
            socket.send event: :stream_data_sync, data: data[:stream_id]
          end
          state
        else
          stream = state[:stream] || fail("stream is nil on stream_progress")
          state.merge({
            stream: stream.merge(action[:data])
          })
        end
      when :stream_end
        state.merge({
          stream_sync: false, stream: {}
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
        state.merge({
          album_details_loading: false,
          album_details: action[:data]
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
      when :render
        @projector.render
        state
      else
        puts "unhandled action #{action[:type]}, data #{action[:data]}"
        state
      end
    end

    def socket
      @socket
    end

    def render
      h 'div.main-container' do
        if @store.state[:component] == @player
          @content.render
        else
          [
            @header.render,
            @content.render,
            @footer.render
          ]
        end
      end
    end

    def component_for(path)
      case path
      when '/albums'
        @albums ||= Albums.new(@store)
      when '/tracks'
        @tracks ||= Tracks.new(@store)
      when '/playlists'
        @playlists ||= Playlists.new(@store)
      when '/settings'
        @settings ||= Settings.new(@store)
      when '/player'
        @player ||= Player.new(@store)
      # when '/spotify'
      #   @spotify ||= Spotify.new(@store)
      when /\/albums\/al.{4}/
        @album_details ||= AlbumDetails.new(@store)
      # when /\/artists\/ar.{4}/
      #   @artist_view ||= ArtistView.new(@store)
      else
        nil
      end
    end

    def run
      # Redirect to /albums.
      if $document.location.path == "/"
        $window.history.replace('/albums')
      end

      # Create component for path.
      goto($document.location.path)

      # Attach virtual dom to body.
      @projector.append($document.body) do
        render
      end

      # Render DOM on state changes.
      @store.subscribe do
        @projector.render
      end

      # Handle browser history state change.
      $window.on 'pop:state' do |evt|
        goto($document.location.path)
      end
    end
  end
end

$document.ready do
  Musciteer::Application.new.run
end
