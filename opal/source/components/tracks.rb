module Musciteer
  class Tracks
    include Maquette::Component

    def initialize(store)
      @store = store
    end

    def render
      h 'div#albums' do
        h 'h1', "Show Tracks"
      end
    end
  end
end