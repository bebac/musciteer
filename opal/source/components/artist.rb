class ArtistViewAlbumThumb < Maquette::Component
  attr_reader :store
  attr_reader :album

  def initialize(store, album)
    @store = store
    @album = album
  end

  def show_album_details(evt)
    store.dispatch({ type: :set_path, data: "/albums/#{album.id}" })
    store.dispatch({ type: :album_details_load, data: album })
    evt.stop!
  end

  def render
    h "li", key: self, onclick: handler(:show_album_details) do
      [
        (
          h 'div.cover' do
            h 'img', { src: album.cover }
          end
        ),
        (
          h 'div.title', "#{album.title}"
        )
        # (
        #   h 'div.artist', { onclick: handler(:show_artist) }, "#{album.artist}"
        # )
      ]
    end
  end
end

class ArtistView < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @cache = Maquette::Cache.new
  end

  def loading?
    store.state[:artist_loading]
  end

  def artist
    store.state[:artist]
  end

  def albums
    artist.albums.sort! do |x, y|
      (x.title <=> y.title)
    end
  end

  def render_loading
    h 'p', "loading..."
  end

  def render_header
    h 'div#artist-header' do
      [
        (
          h 'div' do
            [
              (h 'h1', artist.name)
            ]
          end
        )
      ]
    end

  end

  def render_albums
    h 'ol' do
      albums.map do |album|
        ArtistViewAlbumThumb.new(store, album).render
      end
    end
  end

  def render
    h "div#artist" do
      if loading?
        render_loading
      elsif artist
        @cache.result_for(artist) do
          [
            render_header,
            render_albums
          ]
        end
      else
        h 'p', 'hmm, something is wrong!'
      end
    end
  end
end
