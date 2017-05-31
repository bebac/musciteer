module Musciteer
  class Settings
    include Maquette::Component

    def initialize(store)
      @store = store
    end

    def render
      h 'div#settings' do
        h 'text', "Not implemented yet"
      end
    end
  end
end
