module Musciteer
  class PlayerSkip
    include Maquette::Component

    def initialize(store)
      @store = store
      @cache = Maquette::Cache.new
    end

    def player_skip
      @store.dispatch type: :player_skip
    end

    def player_skip_create(element)
      @player_skip = element
    end

    def animation_start
      @player_skip.add_class('button-click')
      @player_skip.on :animationend do
        animation_end
      end
    end

    def animation_end(evt)
      @player_skip.remove_class('button-click')
    end

    def skip(evt)
      animation_start
      player_skip
    end

    def render_skip_icon
      h 'svg#skip', xmlns: "http://www.w3.org/2000/svg", 'xmlns:xlink': "http://www.w3.org/1999/xlink", viewBox: "0 0 200 200" do
        h 'g' do
          [
            #(h 'circle', { cx: "100", cy: "100", r: "92", fill: "none", 'stroke-width': "5" }),
            (h 'path', d: "M 90,60 120,100 90,140", fill: "none", 'stroke-width': "5"),
            (h 'circle', { cx: "100", cy: "100", r: "100", 'fill-opacity': "0", 'stroke-width': "0", onclick: handler(:skip) })
          ]
        end
      end
    end

    def render
      @cache.result_for(self) do
        h 'div.button', afterCreate: handler(:player_skip_create) do
          render_skip_icon
        end
      end
    end
  end
end