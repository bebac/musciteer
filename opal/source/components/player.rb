class Player < Maquette::Component
  include PlayerAssets
  include PlayerState
  include PlayerActions

  attr_reader :store

  def initialize(store)
    @store = store
  end

  def duration_formatted
    if duration = stream[:duration]
      secs = duration / 1000
      min = Integer(secs / 60) % 60;
      sec = Integer(secs % 60);
      "%d:%02d" % [ min, sec ]
    else
      "-"
    end
  end

  def length_formatted
    if length = stream[:length]
      secs = length / 1000
      min = Integer(secs / 60) % 60;
      sec = Integer(secs % 60);
      "%d:%02d" % [ min, sec ]
    else
      "-"
    end
  end

  def pop_state(evt)
    #$window.history.back
    store.dispatch({ type: :set_path, data: "/albums" })
  end

  def fullscreen
    `document.documentElement.webkitRequestFullscreen()`
  end

  def render_player_stream
    h 'div.player-stream' do
      [
        (
          h 'div.player-cover' do
            if stream_synced?
              h 'img', { src: track.album_cover_path }
            end
          end
        ),
        (
          h 'div' do
            [
              (
                h 'div.title' do
                  if stream_synced?
                    h 'div.player-track-title', track.title
                  end
                end
              ),
              (
                h 'div.artist-album' do
                  if stream_synced?
                    [
                      (h 'div.player-track-artists', track.artists),
                      (h 'div.player-track-album', track.album)
                    ]
                  end
                end
              ),
              (
                h 'div.controls' do
                  [
                    (
                      h 'div' do
                        [
                          (
                            h 'div'
                          ),
                          (
                            h 'div.player-toggle' do
                              if playing?
                                h 'button', key: 'stop', onclick: handler(:stop), innerHTML: stop_svg
                              else
                                h 'button', key: 'play', onclick: handler(:play), innerHTML: play_svg
                              end
                            end
                          ),
                          (
                            h 'div.player-skip' do
                              if playing?
                                h 'button', key: 'skip', onclick: handler(:skip), innerHTML: skip_svg
                              end
                            end
                          )
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
                    ),
                  ]
                end
              )
            ]
          end
        )
      ]
    end
  end

  def render_player_top
    h 'div.player-top' do
      h 'button.icon', key: 'close.1', onclick: handler(:pop_state), innerHTML: close_svg
    end
  end

  def render_player_bottom
    h 'div.player-bottom' do
      h 'button.icon', key: 'fullscreen.1', onclick: handler(:fullscreen), innerHTML: fullscreen_svg
    end
  end

  def render_player_background_idle
    h 'div.player-background' do
      h 'img.idle', { src: 'assets/player-backgroud.jpg' }
    end
  end

  def render_idle
    [
      render_player_top,
      render_player_background_idle,
      render_player_stream,
      render_player_bottom
    ]
  end

  def render_player_background_playing
    h 'div.player-background' do
      h 'img.stream', { src: track.album_cover_path }
    end
  end

  def render_stream
    [
      render_player_top,
      render_player_background_playing,
      render_player_stream,
      render_player_bottom
    ]
  end

  def render
    h 'div#player' do
      if stream_synced?
        render_stream
      else
        render_idle
      end
    end
  end

  def close_svg
    <<-svg
<svg viewBox="0 0 200 200">
  <g stroke-width="24">
    <path d="M50,50l100,100"/>
    <path d="M50,150l100,-100"/>
  </g>
</svg>
    svg
  end

  def fullscreen_svg
    <<-svg
<svg viewBox="0 0 200 200">
  <g stroke-width="24">
    <path d="M110,90l100,-100M200,0v30l-40,-40"/>
    <path d="M90,110l-100,100M0,200v-30l40,40"/>
  </g>
</svg>
    svg
  end
end
