module Musciteer
  class Playlists
    include Maquette::Component

    def initialize(store)
      @store = store
    end

    def render
      h 'div#playlists' do
        h 'text', "Not implemented yet"
      end
    end
  end
end