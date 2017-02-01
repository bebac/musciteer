class Player < Maquette::Component
  include PlayerState

  attr_reader :store

  def initialize(store)
    @store = store
    @close = CloseButton.new(store) { pop_state }
    @fullscreen = FullscreenButton.new(store)
    @play = PlayButton.new(store)
    @stop = StopButton.new(store)
    @skip = SkipButton.new(store)
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
                    h 'div.player-track-title', (track.title.length > 30 ? track.title[0..30]+'...' : track.title)
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
                                @stop.render
                              else
                                @play.render
                              end
                            end
                          ),
                          (
                            h 'div.player-skip' do
                              if playing?
                                @skip.render
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
      h 'div' do
        @close.render
      end
    end
  end

  def render_player_bottom
    h 'div.player-bottom' do
      h 'button', onclick: handler(:fullscreen) do
        @fullscreen.render
      end
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
      if stream_synced?
        h 'img.stream', { src: track.album_cover_path }
      end
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
      if playing? && stream?
        render_stream
      else
        render_idle
      end
    end
  end
end
