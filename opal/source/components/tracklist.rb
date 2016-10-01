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
    h "li", key: self, onclick: handler(:queue) do
      [
        (h 'div.col1', "#{track.title}"),
        (h 'div.col2', "#{track.artists}"),
        (h 'div.col3', "#{track.album}"),
        (h 'div.col4', "#{track.play_count}"),
        (h 'div.col5', "#{track.skip_count}")
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
    h 'div.header' do
      h 'ol' do
        h 'li' do
          [
            (h 'div.col1', "Title"),
            (h 'div.col2', "Artist"),
            (h 'div.col3', "Album"),
            (h 'div.col4', "Plays"),
            (h 'div.col4', "Skips")
          ]
        end
      end
    end
  end

  def render_tracks
    h 'div.body' do
      h 'ol' do
        tracks.map do |track|
          TrackListItem.new(track, store).render
        end
      end
    end
  end

  def render
    @cache.result_for(tracks) do
      h 'div#tracks' do
        if loading?
          render_loading
        elsif tracks
          [
            render_header,
            render_tracks
          ]
        else
          h 'p', "Tracks not loaded!"
        end
      end
    end
  end

  # def load
  #   Browser::HTTP.get("/api/tracks") do |req|
  #     req.on :success do |res|
  #       store.dispatch(
  #         {
  #           type: :tracks_load_success,
  #           data: res.json.map { |t| Track.new(t) }
  #         })
  #     end

  #     req.on :error do |err|
  #       p err
  #     end
  #   end
  #   store.dispatch({ type: :tracks_load })
  # end
end

module ActionDispatchHooks
  def tracks_load
    puts "load tracks"
    Browser::HTTP.get("/api/tracks") do |req|
      req.on :success do |res|
        dispatch(
          {
            type: :tracks_load_success,
            data: res.json.map { |t| Track.new(t) }
          })
      end

      req.on :error do |err|
        p err
      end
    end
  end
end