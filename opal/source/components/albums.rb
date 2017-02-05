class AlbumThumb < Maquette::Component
  attr_reader :album
  attr_reader :store

  def initialize(album, store)
    @album = album
    @store = store
  end

  def show_album_details(evt)
    store.dispatch({ type: :set_path, data: "/albums/#{album.id}" })
    store.dispatch({ type: :album_details_load, data: album })
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
        ),
        (
          h 'div.artist', "#{album.artist}"
        )
      ]
    end
  end
end

class Albums < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @cache = Maquette::Cache.new
    @scroll_top = 0;
  end

  def loading?
    store.state[:albums_loading]
  end

  def albums
    albums = store.state[:albums]
    if albums
      albums.sort! do |x, y|
        (x.artist <=> y.artist).nonzero? || (x.title <=> y.title)
      end
    end
    albums
  end

  def save_scroll_top(evt)
    @scroll_top = evt.target.scroll.y
  end

  def render_loading
    h 'p', "loading..."
  end

  def render_albums
    h 'ol' do
      albums.map do |album|
        AlbumThumb.new(album, store).render
      end
    end
  end

  def render_not_loaded
    h 'p', "Albums not loaded!"
  end

  def render
    node = @cache.result_for(albums) do
      h 'div#albums', { onscroll: handler(:save_scroll_top) } do
        if loading?
          render_loading
        elsif albums
          render_albums
        else
          render_not_loaded
        end
      end
    end

    # Update scroll position.
    if @scroll_top > 0
      `node.properties.scrollTop = #{@scroll_top}`
    end

    node
  end
end

module ActionDispatchHooks
  def albums_load
    Browser::HTTP.get("/api/albums") do |req|
      req.on :success do |res|
        dispatch({
          type: :albums_load_success,
          data: res.json.map { |t| Album.new(t) }
        })
      end

      req.on :error do |err|
        p err
      end
    end
  end

  def album_details_load(album)
    case album
    when String
      Browser::HTTP.get("/api/albums/#{album}") do |req|
        req.on :success do |res|
          # Create album.
          album = Album.new(res.json)
          # Set album details album.
          dispatch({
            type: :album_details_load_success,
            data: album
          })
          # Load tracks.
          dispatch({
            type: :album_details_load,
            data: album
          })
        end

        req.on :error do |err|
          p err
        end
      end
    when Album
      Browser::HTTP.get("/api/albums/#{album.id}/tracks") do |req|
        req.on :success do |res|
          dispatch({
            type: :album_details_load_success,
            data: res.json.map { |t| Track.new(t) }
          })
        end

        req.on :error do |err|
          p err
        end
      end
    end
  end
end