class Top
  include Inesita::Component

  def initialize
    @is_leaf = true
  end

  def init
    store.on(:player_state_changed) { render!(self) }
  end

  def toggle_menu
    puts "toggle menu"
  end

  def toggle_settings(evt)
    store.settings_toggle
  end

  def render
    div class: 'top' do
      div do
        div id: 'menu-toggle', onclick: method(:toggle_menu) do
          img src: 'assets/menu.svg'
        end
        div id: 'player-toggle' do
          case store.player_state
          when :stopped
            button onclick: -> { store.player_play } do
              text 'play'
            end
          when :playing
            button onclick: -> { store.player_stop } do
              text 'stop'
            end
          end
        end
        div id: 'settings-toggle', onclick: method(:toggle_settings) do
          img src: 'assets/settings.svg'
        end
      end
    end
  end
end
