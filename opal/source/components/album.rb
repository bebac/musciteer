class AlbumViewTrack < Maquette::Component
  attr_reader :track
  attr_reader :store

  def initialize(track, store)
    @track = track
    @store = store
    @play = PlayButton.new(store, track.id)
  end

  def queue
    store.dispatch({ type: :player_queue, data: track.id })
  end

  def render
    h 'li', { key: self, onclick: handler(:queue) } do
      [
        (
          h 'div.flex-left' do
            [
              (h 'div.tn', track.tn),
              @play.render,
              (h 'div.title', track.title)
            ]
          end
        ),
        (
          h 'div.flex-right' do
            [
              (h 'div.duration', track.duration_formatted)
            ]
          end
        )
      ]
    end
  end
end

class AlbumViewDiscHeader < Maquette::Component
  attr_reader :disc_number

  def initialize(disc_number)
    @disc_number = disc_number
  end

  def render
    h 'li.disc-header', "Disc #{disc_number}"
  end
end

class AlbumView < Maquette::Component
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
        res << AlbumViewDiscHeader.new(dn)
      end
      res << AlbumViewTrack.new(track, store)
    end
    res
  end

  def render_header
    h 'div#album-header' do
      [
        (
          h 'div' do
            h 'img', src: album.cover
          end
        ),
        (
          h 'div' do
            h 'img', src: album.cover
          end
        ),
        (
          h 'div' do
            [
              (h 'h1', album.title),
              (h 'h2', album.artist)
            ]
          end
        )
      ]
    end
  end

  def render_track_list
    h 'div#album-tracks' do
      h 'ol.tracks' do
        tracks_with_disc_headers.map { |x| x.render }
      end
    end
  end

  def render
    h "div#album" do
      if loading?
        h 'p', "loading..."
      elsif album
        @cache.result_for(album) do
          [
            render_header,
            render_track_list
          ]
        end
      else
        # Empty.
      end
    end
  end
end