module Musciteer
  class Tracks
    include Maquette::Component

    def initialize(store)
      @store = store
    end

    def render
      h 'div#tracks' do
        h 'text', "Not implemented yet"
      end
    end
  end
end