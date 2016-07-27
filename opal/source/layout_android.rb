class Layout
  include Inesita::Layout

  def toggle_player_control(evt)
    if evt.target.id == 'player-ctrl-container'
      $document.at('#player-ctrl').toggle
    end
  end

  def render
    div id: 'outlet-container' do
      component outlet
    end

    div id: 'player-ctrl-container', onclick: method(:toggle_player_control) do
      div id: 'player-ctrl' do
        component PlayerControl
      end
    end

    div id: 'connection-lost-overlay' do
      component ConnectionLost
    end
  end
end
