class Albums
  include Inesita::Component

  def init
    puts "init Albums"
    store.load_albums
  end

  def show_album(album_id)
    store.load_album_details(album_id)
    $document.at('#album-details-container').show
  end

  def render
    div id: 'albums-container' do
      div id: 'album-thumbs' do
        store.albums.each do |album|
          div class: 'album-thumb', onclick: -> { show_album(album.id) } do
            div do
              text album.title
            end
            div do
              img src: album.cover
            end
            div do
              text album.artist
            end
          end
        end
      end
    end
  end
end
