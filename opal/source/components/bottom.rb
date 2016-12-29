class Bottom < Maquette::Component
  include PlayerAssets
  include PlayerState
  include PlayerActions

  attr_reader :store

  def initialize(store)
    @store = store
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

  def render_player_ctrl
    h 'div#player-ctrl' do
      if playing?
        [
          (h 'button', key: 'stop.1', onclick: handler(:stop), innerHTML: stop_svg),
          (h 'button', key: 'skip.1', onclick: handler(:skip), innerHTML: skip_svg)
        ]
      else
        [
          (h 'button', key: 'play.1', onclick: handler(:play), innerHTML: play_svg),
          (h 'button', key: 'skip.0',                          innerHTML: skip_svg)
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
