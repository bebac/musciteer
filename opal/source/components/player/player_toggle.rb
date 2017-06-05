module Musciteer
  class PlayerToggle
    include Maquette::Component

    def initialize(store)
      @store = store
      @cache = Maquette::Cache.new
    end

    def player_state
      @store.state[:player_state]
    end

    def playing?
      player_state == :playing
    end

    def player_play
      @store.dispatch type: :player_play, data: nil
    end

    def player_stop
      @store.dispatch type: :player_stop
    end

    def player_toggle_create(element)
      @player_toggle = element
      # Not very Maquette, but Maquette doesn't handle animation events.
      @player_toggle.on :animationend do
        animation_end
      end
    end

    def animation_start
      @player_toggle.add_class('button-click')
    end

    def animation_end(evt)
      @player_toggle.remove_class('button-click')
    end

    def play(evt)
      animation_start
      player_play
    end

    def stop(evt)
      animation_start
      player_stop
    end

    def render_play_icon
      h 'svg#play', xmlns: "http://www.w3.org/2000/svg", 'xmlns:xlink': "http://www.w3.org/1999/xlink", viewBox: "0 0 200 200" do
        h 'g' do
          [
            (h 'circle', { cx: "100", cy: "100", r: "92", fill: "none", 'stroke-width': "5" }),
            (h 'path', d: "M90,65v70l35,-35"),
            (h 'circle', { cx: "100", cy: "100", r: "100", 'fill-opacity': "0", 'stroke-width': "0", onclick: handler(:play) })
          ]
        end
      end
    end

    def render_stop_icon
      h 'svg#stop', xmlns: "http://www.w3.org/2000/svg", 'xmlns:xlink': "http://www.w3.org/1999/xlink", viewBox: "0 0 200 200" do
        h 'g' do
          [
            (h 'circle', { cx: "100", cy: "100", r: "92", fill: "none", "stroke-width" => "5" }),
            (h 'path', d: "M75,75h50v50h-50"),
            (h 'circle', { cx: "100", cy: "100", r: "100", 'fill-opacity': "0", 'stroke-width': "0", onclick: handler(:stop) })
          ]
        end
      end
    end

    def render
      h 'div.button', afterCreate: handler(:player_toggle_create) do
        @cache.result_for(player_state) do
          if playing?
            render_stop_icon
          else
            render_play_icon
          end
        end
      end
    end
  end
end