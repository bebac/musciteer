module Musciteer
  class SearchBox
    include Maquette::Component

    def initialize(store)
      @store = store
    end

    def input_hidden?
      @input.style['visibility'] == 'hidden'
    end

    def show_input
      @input.style['visibility'] = 'visible'
      @input.style['margin-left'] = "1em"
      @input.width = '50%'
      @input.focus
    end

    def hide_input
      @input.style['visibility'] = 'hidden'
      @input.style['margin-left'] = "0"
      @input.width = '0'
    end

    def created(element)
      @input = element.at('input')
      hide_input
    end

    def toggle(evt)
      puts "toggle search"
      if input_hidden?
        show_input
      else
        hide_input
      end
    end

    def render_seach_icon
      h 'svg', xmlns: "http://www.w3.org/2000/svg", 'xmlns:xlink': "http://www.w3.org/1999/xlink", viewBox: "0 0 200 200", onclick: handler(:toggle) do
        h 'g', 'stroke-width': "8" do
          [
            (h 'circle', cx: "100", cy: "100", r: "30", fill: "none"),
            (h 'path', d: "M125,125l25,25"),
          ]
        end
      end
    end

    def render_search_input
      h 'input', type: :text, placeholder: "start typing here"
    end

    def render
      h 'div', afterCreate: handler(:created) do
        [
          render_search_input,
          render_seach_icon
        ]
      end
    end
  end
end
