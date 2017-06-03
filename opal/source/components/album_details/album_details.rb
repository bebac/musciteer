module Musciteer
  class AlbumDetails
    include Maquette::Component

    attr_reader :store

    class ItemCache
      attr_reader   :store
      attr_accessor :album_id
      attr_accessor :data

      def initialize(store)
        @store = store
        @album_id = nil
        @data = nil
      end

      def tracks_with_disc_header(album)
        if @album_id && @album_id == album.id
          @data ||= create(album)
        else
          @album_id = album.id
          @data = create(album)
        end
      end

      def create(album)
        puts "create album details items #{album.id}"
        res = []
        dn = 1
        album_tracks_by_index(album).each do |track|
          if track.dn > dn
            dn = track.dn
            res << AlbumDetailsDiscHeader.new(dn)
          end
          res << AlbumDetailsTrack.new(track, store)
        end
        res
      end

      def album_tracks_by_index(album)
        album.tracks.sort! { |x, y| x.index <=> y.index }
      end
    end

    def initialize(store)
      @store = store
      @items = ItemCache.new(store)
    end

    def loading?
      store.state[:album_details_loading]
    end

    def album
      store.state[:album_details]
    end

    def render_loading
      h 'div.loader-container' do
        h 'div.loader'
      end
    end

    def render_header
      h 'div.album-details-header' do
        [
          (
            h 'div' do
              h 'img', src: album.cover
            end
          ),
          (
            h 'div' do
              [
                (
                  h 'div.title' do
                    [
                      (h 'h1', album.title),
                      (h 'h2', album.artist)
                    ]
                  end
                ),
                (
                  h 'div.actions' do
                  end
                )
              ]
            end
          )
        ]
      end
    end

    def render_tracks
      h 'div.album-details-tracks' do
        h 'ol' do
          @items.tracks_with_disc_header(album).map { |x| x.render }
        end
      end
    end

    def render
      h "div.album-details" do
        if loading?
          render_loading
        elsif album
          [
            render_header,
            render_tracks
          ]
        else
          # ERROR!
          h 'text', "error loading album details"
        end
      end
    end
  end
end