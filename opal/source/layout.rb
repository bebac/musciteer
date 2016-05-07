class Layout
  include Inesita::Layout

  def toggle_settings(evt)
    if evt.target.id == 'settings-overlay'
      $document.at('#settings-overlay').hide
    end
  end

  def render
    div id: 'top-container' do
      component Top
    end

    div id: 'outlet-container' do
      component outlet
    end

    div id: 'settings-overlay', onclick: method(:toggle_settings) do
      component Settings
    end
  end
end
