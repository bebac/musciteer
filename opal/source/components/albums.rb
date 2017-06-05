module Musciteer
  class Albums
    include Maquette::Component
    #include AlbumsActions

    attr_reader :store
    attr_reader :scroll_top

    def initialize(store)
      @store = store
      @cache = Maquette::Cache.new
      @scroll_top = 0;
    end

    def save_scroll_top(value)
      @scroll_top = value
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

    def render_loading
      h 'div.loader-container' do
        h 'div.loader'
      end
    end

    def render_albums
      @cache.result_for(albums) do
        h 'ol' do
          albums.map do |album|
            AlbumsThumb.new(album, store).render
          end
        end
      end
    end

    def render_not_loaded
      h 'p', "Albums not loaded!"
    end

    def render
      h 'div#albums' do
        if loading?
          render_loading
        elsif albums
          render_albums
        else
          render_not_loaded
        end
      end
    end
  end
end
