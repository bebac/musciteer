class AlbumDetails
  include Inesita::Component

  def init
    puts "init AlbumDetails"
  end

  def album
    store.album_details
  end

  def render
    div do
      if album
        h1 do; text album.title; end
        ul do
          album.tracks.each do |track|
            li do; text track.title; end
          end
        end
      end
    end
  end
end
