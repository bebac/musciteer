class Player < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
  end

  def stream_synced?
    store.state[:stream_sync]
  end

  def stream
    store.state[:stream]
  end

  def track
    stream[:track]
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

  def show_control
    $document.at('#player-control-overlay').show
  end

  def render_idle
    [
      (
        h 'div.stream', onclick: handler(:show_control) do
          h 'div', "Not Playing"
        end
      ),
      (
        h 'div.player-track-image', onclick: handler(:show_control)
      )
    ]

  end

  def render_stream
    [
      (
        h 'div.stream', onclick: handler(:show_control) do
          [
            (
              h 'div.title' do
                h 'div.player-track-title', track.title
              end
            ),
            (
              h 'div.artist-album' do
                h 'div' do
                  [
                    (h 'div.player-track-artists', track.artists),
                    (h 'div.player-track-album', track.album)
                  ]
                end
              end
            ),
            (
              h 'div.time' do
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
      ),
      (
        h 'div.player-track-image', onclick: handler(:show_control) do
          h 'img', { src: track.album_cover_path }
        end
      )
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
end
