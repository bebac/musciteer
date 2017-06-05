module Musciteer
  class Footer
    include Maquette::Component
    include NavigationActions

    attr_reader :store

    def initialize(store)
      @store = store
      @cache = Maquette::Cache.new
      @player_toggle = PlayerToggle.new(store)
    end

    def stream_sync?
      store.state[:stream_sync]
    end

    def stream?
      !store.state[:stream].empty?
    end

    def stream
      store.state[:stream]
    end

    def track
      stream[:track]
    end

    def player_show(evt)
      goto('/player')
    end

    def render_player_show_icon
      @cache.result_for(self) do
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
    end

    def render_player
      if stream_sync?
        [
          (
            h 'div.cover' do
              h 'img', src: track.album_cover_path
            end
          ),
          (
            h 'div.label' do
              [
                (h 'h1', track.title),
                (h 'h2', track.artists),
              ]
            end
          )
        ]
      end
    end

    def render
      h 'div#footer'  do
        [
          (
            h  'div#footer-left' do
              [
                (
                  h 'div.button#player-show-button' do
                    render_player_show_icon
                  end
                ),
                (
                  h 'div#footer-player' do
                    render_player
                  end
                )
              ]
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