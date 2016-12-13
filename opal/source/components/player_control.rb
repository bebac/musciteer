class PlayerControl < Maquette::Component
  include PlayerAssets
  include PlayerState
  include PlayerActions

  attr_reader :store

  def initialize(store)
    @store = store
  end

  def albums
    $window.history.push('/albums')
    store.dispatch({ type: :set_path, data: '/albums' })
    $document.at('#player-control-overlay').hide
  end

  def fullscreen
    `document.documentElement.webkitRequestFullscreen()`
  end

  def render_albums_button
    h 'div#player-albums' do
      h 'button', key: 'albums', onclick: handler(:albums) do
        h 'text', "albums"
      end
    end
  end

  def render_spacer
    h 'div'
  end

  def render_player_toggle_button
    h 'div#player-toggle' do
      if playing?
        h 'button', key: 'stop', onclick: handler(:stop), innerHTML: stop_svg
      else
        h 'button', key: 'play', onclick: handler(:play), innerHTML: play_svg
      end
    end
  end

  def render_player_skip_button
    h 'div#player-skip' do
      if playing?
        h 'button', key: 'skip', onclick: handler(:skip), innerHTML: skip_svg
      end
    end
  end

  def render_fullscreen_button
    h 'div#player-fullscreen' do
      h 'button', key: 'fullscreen', onclick: handler(:fullscreen) do
        h 'text', "fullscreen"
      end
    end
  end

  def render
    h 'div#player-control' do
      h 'div' do
        [
          render_albums_button,
          render_spacer,
          render_player_toggle_button,
          render_player_skip_button,
          render_fullscreen_button
        ]
      end
    end
  end
end