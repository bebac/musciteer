module Musciteer
  class Footer
    include Maquette::Component

    def initialize(store)
      @store = store
      @player_toggle = PlayerToggle.new(store)
    end

    def player_show(evt)
      puts "show fullscreen player"
    end

    def render_play_icon
      h 'svg#player-show', xmlns: "http://www.w3.org/2000/svg", 'xmlns:xlink': "http://www.w3.org/1999/xlink", viewBox: "0 0 200 200" do
        h 'g' do
          [
            #(h 'circle', { cx: "100", cy: "100", r: "92", fill: "none", 'stroke-width': "5" }),
            (h 'path', d: "m25,125l75,-50l75,50", fill: "none", 'stroke-width': "12"),
            (h 'circle', { cx: "100", cy: "100", r: "100", 'fill-opacity': "0", 'stroke-width': "0", onclick: handler(:player_show) })
          ]
        end
      end
    end

    def render
      h 'div.footer'  do
        [
          (
            h  'div#footer-left' do
              h 'div.button#player-show-button' do
                render_play_icon
              end
            end
          ),
          @player_toggle.render,
          (
            h 'div#footer-right'
          )
        ]
      end
    end
  end
end