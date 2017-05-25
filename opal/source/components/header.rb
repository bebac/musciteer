module Musciteer
  class Header
    include Maquette::Component

    def initialize(store)
      @store = store
      @search_box = SearchBox.new(store)
      @navigation = Navigation.new(store)
    end

    def render_logo
      h 'div.logo' do
        h 'text', "Musciteer"
      end
    end

    def render_search_box
      h 'div.search' do
        @search_box.render
      end
    end

    def render_header_left
      h 'div.header-left' do
        [
          render_logo,
          (h 'div.vbar'),
          render_search_box
        ]
      end
    end

    def render_header_right
      h 'div.header-right' do
        @navigation.render
      end
    end

    def render
      h 'div.header' do
        h 'div' do
          [
            render_header_left,
            render_header_right
          ]
        end
      end
    end
  end
end
