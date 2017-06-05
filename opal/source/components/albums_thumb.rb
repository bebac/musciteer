module Musciteer
  class AlbumsThumb
    include Maquette::Component
    include NavigationActions

    attr_reader :album
    attr_reader :store

    def initialize(album, store)
      @album = album
      @store = store
    end

    def show_album_details(evt)
      goto("/albums/#{album.id}")
      evt.stop!
    end

    def show_artist(evt)
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
            h 'div.label' do
              [
                (
                  h 'div.title', "#{album.title}"
                ),
                (
                  h 'div.artist', { onclick: handler(:show_artist) }, "#{album.artist}"
                )
              ]
            end
          )
        ]
      end
    end
  end
end
