module Musciteer
  class Content
    include Maquette::Component

    def initialize(store)
      @store = store
    end

    def render
      h 'div.content' do
        @store.state[:component].render
      end
    end
  end
end
