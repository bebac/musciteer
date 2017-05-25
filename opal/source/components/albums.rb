module Musciteer
  class Albums
    include Maquette::Component
    include AlbumsActions

    attr_reader :store

    def initialize(store)
      @store = store
      unless @store.state[:albums]
        albums_load
      end
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
      h 'p', "loading..."
    end

    def render_albums
      h 'ol' do
        albums.map do |album|
          AlbumsThumb.new(album, store).render
        end
      end
    end

    def render_not_loaded
      h 'p', "Albums not loaded!"
    end

    def render
      #h 'div#albums', { onscroll: handler(:save_scroll_top) } do
      h 'div#albums' do
        if loading?
          render_loading
        elsif albums
          render_albums
        else
          render_not_loaded
        end
      end

      # Update scroll position.
      #if @scroll_top > 0
      #  `node.properties.scrollTop = #{@scroll_top}`
      #end

      #node
    end
  end
end
