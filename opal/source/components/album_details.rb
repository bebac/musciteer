class AlbumDetailsTrack < Maquette::Component
  attr_reader :track
  attr_reader :store

  def initialize(track, store)
    @track = track
    @store = store
  end

  def queue
    store.dispatch({ type: :player_queue, data: track.id })
  end

  def render
    h 'li', { key: self, onclick: handler(:queue) } do
      [
        (h 'div.tn', track.tn),
        (h 'div.title', track.title),
        (h 'div.duration', track.duration_formatted)
      ]
    end
  end
end

class AlbumDetailsDiscHeader < Maquette::Component
  attr_reader :disc_number

  def initialize(disc_number)
    @disc_number = disc_number
  end

  def render
    h 'li', "Disc #{disc_number}"
  end
end

class AlbumDetails < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @cache = Maquette::Cache.new
  end

  def loading?
    store.state[:album_details_loading]
  end

  def album
    store.state[:album_details]
  end

  def tracks
    album.tracks.sort! { |x, y| x.index <=> y.index }
  end

  def tracks_with_disc_headers
    res = []
    dn = 1
    tracks.each do |track|
      if track.dn > dn
        dn = track.dn
        res << AlbumDetailsDiscHeader.new(dn)
      end
      res << AlbumDetailsTrack.new(track, store)
    end
    res
  end

  def render
    h "div#album-details" do
      if loading?
        h 'p', "loading..."
      elsif album
        @cache.result_for(album) do
          [
            (
              h 'div.cover' do
                h 'img', src: album.cover
              end
            ),
            (h 'h1', album.title),
            (h 'h2', album.artist),
            (h 'hr'),
            (
              h 'ol.tracks' do
                tracks_with_disc_headers.map { |x| x.render }
              end
            )
          ]
        end
      else
        # Empty.
      end
    end
  end
end