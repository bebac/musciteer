class AlbumDetails
  include Inesita::Component

  def init
    puts "init AlbumDetails"
    store.on(:album_details) { render! }
  end

  def album
    store.album_details
  end

  def queue_track(track_id)
    store.queue(track_id)
  end

  def render
    div do
      if album
        div class: 'cover' do
          img src: "#{album.cover}"
        end
        h1 do; text album.title; end
        h2 do; text album.artist; end
        hr
        ol class: 'track-list' do
          dn = 1
          album.tracks.each do |track|
            if track.dn > dn
              dn = track.dn
              li class: 'disc-header' do; text "Disc #{dn}"; end
            end
            li onclick: -> { queue_track(track.id) } do;
              div class: 'tn' do
                text "%02d" % [ track.tn ]
              end
              div class: 'title' do
                div do; text track.title; end
              end
              div class: 'duration' do
                text track.duration_formatted
              end
            end
          end
        end
      end
    end
  end
end
