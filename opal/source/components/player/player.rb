module Musciteer
  class Player
    include Maquette::Component
    include NavigationActions

    attr_reader :store

    def initialize(store)
      @store = store
      @cache = Maquette::Cache.new
      @player_toggle = PlayerToggle.new(store)
      @player_skip = PlayerSkip.new(store)
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

    def duration_formatted
      if stream? && (duration = stream[:duration])
        secs = duration / 1000
        min = Integer(secs / 60) % 60;
        sec = Integer(secs % 60);
        "%d:%02d" % [ min, sec ]
      else
        "-"
      end
    end

    def length_formatted
      if stream? && (length = stream[:length])
        secs = length / 1000
        min = Integer(secs / 60) % 60;
        sec = Integer(secs % 60);
        "%d:%02d" % [ min, sec ]
      else
        "-"
      end
    end

    def player_hide(evt)
      goto('/albums')
    end

    def render_player_hide_icon
      @cache.result_for(self) do
        h 'svg#player-hide', xmlns: "http://www.w3.org/2000/svg", 'xmlns:xlink': "http://www.w3.org/1999/xlink", viewBox: "0 0 200 200" do
          h 'g' do
            [
              #(h 'circle', { cx: "100", cy: "100", r: "92", fill: "none", 'stroke-width': "5" }),
              (h 'path', d: "m25,75l75,50l75,-50", fill: "none", 'stroke-width': "12"),
              (h 'circle', { cx: "100", cy: "100", r: "100", 'fill-opacity': "0", 'stroke-width': "0", onclick: handler(:player_hide) })
            ]
          end
        end
      end
    end

    def render_player_content_1
      h 'div.container' do
        if stream_sync?
          h 'img', { src: track.album_cover_path }
        end
      end
    end

    def render_player_content_2
      h 'div.container' do
        [
          (
            h 'div.title' do
              if stream_sync?
                h 'div.player-track-title', (track.title.length > 30 ? track.title[0..30]+'...' : track.title)
              end
            end
          ),
          (
            h 'div.artist-album' do
              if stream_sync?
                h 'div' do
                  [
                    (h 'div.player-track-artists', track.artists),
                    (h 'div.player-track-album', track.album)
                  ]
                end
              end
            end
          ),
          (
            h 'div.controls' do
              [
                (
                  h 'div.control-buttons' do
                    [
                      (h 'div'),
                      @player_toggle.render,
                      @player_skip.render
                    ]
                  end
                ),
                (
                  h 'div' do
                    h 'div.player-stream-time' do
                      [
                        (h 'span.stream-duration', duration_formatted),
                        (h 'span.stream-length', " / #{length_formatted}")
                      ]
                    end
                  end
                )
              ]
            end
          )
        ]
      end
    end

    def render
      h 'div#player' do
        [
          (
            h 'div.button#player-hide-button' do
              render_player_hide_icon
            end
          ),
          (
            h 'div#player-content' do
              [
                render_player_content_1,
                render_player_content_2,
              ]
            end
          )
        ]
      end
    end
  end
end
