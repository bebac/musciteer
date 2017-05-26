module Musciteer
  class AlbumDetails
    include Maquette::Component

    attr_reader :store

    def initialize(store)
      @store = store
    end

    def loading?
      store.state[:album_details_loading]
    end

    def album
      store.state[:album_details]
    end

    def tracks
      album.tracks.sort! { |x, y| x.index <=> y.index }
    end

    def tracks_with_disc_headers
      res = []
      dn = 1
      tracks.each do |track|
        if track.dn > dn
          dn = track.dn
          res << AlbumDetailsDiscHeader.new(dn)
        end
        res << AlbumDetailsTrack.new(track, store)
      end
      res
    end

    def render_loading
      h 'text', "loading..."
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
          tracks_with_disc_headers.map { |x| x.render }
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