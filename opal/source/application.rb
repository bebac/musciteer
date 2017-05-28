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
        albums_loading:        false,
        albums:                nil,
        album_details_loading: false,
        album_details:         nil
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
        [
          @header.render,
          @content.render,
          @footer.render
        ]
      end
    end

    def component_for(path)
      puts "component_for path=#{path}"
      case path
      when '/albums'
        @albums ||= Albums.new(@store)
      when '/tracks'
        @tracks ||= Tracks.new(@store)
      when '/playlists'
        @playlists ||= Playlists.new(@store)
      # when '/player'
      #   @player ||= Player.new(@store)
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
