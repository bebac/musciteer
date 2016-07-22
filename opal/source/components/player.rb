class Player
  include Inesita::Component

  def init
    store.on(:stream_changed) { render! }
    store.on(:player_state_changed) { render! }
  end

  def render
    div id: "player" do
      if stream = store.stream
        div do
          if track = stream.track
            div do
              div class: 'player-track-title' do
                text "#{track.title}"
              end
            end
            div do
              div do
                div class: 'player-track-artists' do
                  text track.artists
                end
                div class: 'player-track-album' do
                  text track.album
                end
              end
            end
            div do
              div class: 'player-stream-time' do
                span class: 'stream-duration' do
                  text stream.duration_formatted
                end
                span class: 'stream-length' do
                  text " / #{stream.length_formatted}"
                end
              end
            end
          end
        end
        div class: 'player-track-image' do
          if track = stream.track
            img src: track.album_cover_path
          end
        end
      else
        text "Not playing"
        text `navigator.userAgent`
      end
    end
  end
end
