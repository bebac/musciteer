class PlayerControl
  include Inesita::Component

  def init
  end

  def render
    div do
      div do
      end
      div id: 'player-toggle' do
        case store.player_state
        when :stopped
          button onclick: -> { store.player_play } do
            img src: 'assets/play.svg'
          end
        when :playing
          button onclick: -> { store.player_stop } do
            img src: 'assets/stop.svg'
          end
        end
      end
      div do
        button onclick: -> { `Android.toggle_fullscreen()` } do
          text "menu"
        end
      end
    end
  end
end
