class Top < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
  end

  def toggle_menu(evt)
    $document.at('#menu').toggle
  end

  def toggle_settings
    $document.at('#settings-overlay').toggle
  end

  def toggle_player
    if playing?
      store.dispatch({ type: :player_stop })
    else
      store.dispatch({ type: :player_start })
    end
  end

  def playing?
    store.state[:player_state] == :playing
  end

  def render
    [
      (
        h 'button.icon', { onclick: handler(:toggle_menu) } do
          h 'img', { src: 'assets/menu.svg' }
        end
      ),
      (
        h 'button.text', { onclick: handler(:toggle_player) }, playing? ? "stop" : "play"
      ),
      (
        h 'button.icon', { onclick: handler(:toggle_settings) } do
          h 'img', { src: 'assets/settings.svg' }
        end
      ),
    ]
  end
end
