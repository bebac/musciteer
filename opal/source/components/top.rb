class Top
  include Inesita::Component

  def toggle_menu
    puts "toggle menu"
  end

  def toggle_settings(evt)
    puts "toggle settings #{evt.target.id}"
    $document.at('#settings-overlay').show
  end

  def render
    div class: 'top' do
      div do
        div id: 'menu-toggle', onclick: method(:toggle_menu) do
          text 'menu'
        end
        div id: 'settings-toggle', onclick: method(:toggle_settings) do
          text 'settings'
        end
      end
    end
  end
end
