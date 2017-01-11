class Bottom < Maquette::Component
  include PlayerState

  attr_reader :store

  def initialize(store)
    @store = store
    @play = PlayButton.new(store)
    @stop = StopButton.new(store)
    @skip = SkipButton.new(store)
  end

  def goto_player
    store.dispatch({ type: :set_path, data: "/player" })
  end

  def render_player_cover
    h 'div#player-cover' do
      if stream_synced?
        h 'img', { src: track.album_cover_path, onclick: handler(:goto_player) }
      end
    end
  end

  def render_player_info
    h 'div#player-info' do
      if stream_synced?
        [
          (h 'span.title', track.title),
          (h 'span.by', " - "),
          (h 'span.artists', track.artists)
        ]
      elsif !playing?
        #h 'text', "[ stopped ]"
      end
    end
  end

  def render_button(button)
    h 'div' do
      button.render
    end
  end

  def render_player_ctrl
    h 'div#player-ctrl' do
      if playing?
        [
          render_button(@stop),
          render_button(@skip)
        ]
      else
        [
          render_button(@play),
          render_button(@skip)
        ]
      end
    end
  end

  def render
    h 'div#bottom' do
      [
        render_player_cover,
        render_player_info,
        render_player_ctrl
      ]
    end
  end
end
