class PlayerControl < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
  end

  def playing?
    store.state[:player_state] == :playing
  end

  def stop
    store.dispatch({ type: :player_stop })
  end

  def play
    store.dispatch({ type: :player_start })
  end

  def skip
    store.dispatch({ type: :player_skip })
  end

  def albums
    $window.history.push('/albums')
    store.dispatch({ type: :set_path, data: '/albums' })
    $document.at('#player-control-overlay').hide
  end

  def fullscreen
    `document.documentElement.webkitRequestFullscreen()`
  end

  def render
    h 'div#player-control' do
      h 'div' do
        [
          (
            h 'div#player-albums' do
              h 'button', key: 'albums', onclick: handler(:albums) do
                h 'text', "albums"
              end
            end
          ),
          (h 'div'),
          (
            h 'div#player-toggle' do
              if playing?
                h 'button', key: 'stop', onclick: handler(:stop) do
                  h 'img', { src: 'assets/stop.svg' }
                end
              else
                h 'button', key: 'play', onclick: handler(:play) do
                  h 'img', { src: 'assets/play.svg' }
                end
              end
            end
          ),
          (
            h 'div#player-skip' do
              if playing?
                h 'button', key: 'skip', onclick: handler(:skip) do
                  h 'img', { src: 'assets/skip.svg' }
                end
              end
            end
          ),
          (
            h 'div#player-fullscreen' do
              h 'button', key: 'fullscreen', onclick: handler(:fullscreen) do
                h 'text', "fullscreen"
              end
            end
          )
        ]
      end
    end
  end
end