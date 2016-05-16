class Layout
  include Inesita::Layout

  def toggle_settings(evt)
    if evt.target.id == 'settings-overlay'
      $document.at('#settings-overlay').hide
    end
  end

  def toggle_album_details(evt)
    if evt.target.id == 'album-details-container'
      $document.at('#album-details-container').hide
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

    div id: 'album-details-container', onclick: method(:toggle_album_details) do
      div id: 'album-details' do
        component AlbumDetails
      end
    end
  end
end
