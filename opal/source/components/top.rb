class Top
  include Inesita::Component

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
        div id: 'settings-toggle', onclick: method(:toggle_settings) do
          img src: 'assets/settings.svg'
        end
      end
    end
  end
end
