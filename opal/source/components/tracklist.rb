class TrackListItem < Maquette::Component
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
    h "tr", key: track.id, onclick: handler(:queue) do
      [
        (h 'td.col1', "#{track.title}"),
        (h 'td.col2', "#{track.artists}"),
        (h 'td.col3', "#{track.album}"),
        (h 'td.col4', "#{track.play_count}"),
        (h 'td.col5', "#{track.skip_count}")
      ]
    end
  end
end

class TrackList < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @cache = Maquette::Cache.new
  end

  def loading?
    store.state[:tracks_loading]
  end

  def tracks
    store.state[:tracks]
  end

  def render_loading
    h 'p', "loading..."
  end

  def render_header
    h 'tr' do
      [
        (h 'th.col1', "Title"),
        (h 'th.col2', "Artist"),
        (h 'th.col3', "Album"),
        (h 'th.col4', "Plays"),
        (h 'th.col5', "Skips")
      ]
    end
  end

  def render_tracks
    h 'table' do
      [
        render_header,
        (
          tracks.map do |track|
            TrackListItem.new(track, store).render
          end
        )
      ]
    end
  end

  def render
    @cache.result_for(tracks) do
      h 'div#tracks' do
        if loading?
          render_loading
        elsif tracks
          render_tracks
        else
          h 'p', "Tracks not loaded!"
        end
      end
    end
  end
end

module ActionDispatchHooks
  def tracks_load
    Browser::HTTP.get("/api/tracks") do |req|
      req.on :success do |res|
        dispatch({
          type: :tracks_load_success,
          data: res.json.map { |t| Track.new(t) }.sort { |x, y| x.play_count <=> y.play_count }.reverse
        })
      end

      req.on :error do |err|
        p err
      end
    end
  end
end