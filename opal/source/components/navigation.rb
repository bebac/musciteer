module Musciteer
  class Navigation
    include Maquette::Component
    include NavigationActions

    def initialize(store)
      @store = store
    end

    def active?(page)
      $document.location.path == page ? '.active' : ''
    end

    def clicked(path, evt)
      goto(path)
      evt.stop!
    end

    def a(path, label)
      h "a#{active?(path)}", { href: path, onclick: handler(:clicked, path) }, label
    end

    def render
      h 'nav' do
        [
          a('/albums', "Albums"),
          a('/tracks', "Tracks"),
          a('/playlists', "Playlists"),
          a('/settings', "Settings")
        ]
      end
    end
  end
end
