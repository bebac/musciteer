class TrackListItem < Maquette::Component
  attr_reader :track
  attr_reader :store

  def initialize(track, store)
    @track = track
    @store = store
    @cache = Maquette::Cache.new
  end

  def queue
    store.dispatch({ type: :player_queue, data: track.id })
  end

  def render
    @cache.result_for(track) do
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
end

class TrackList < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @cache = Maquette::Cache.new
    @table_height = 0
    @hdr_height = 0
    @item_height = 0
    @offset = 0
  end

  def loading?
    store.state[:tracks_loading]
  end

  def tracks_uncached
    store.state[:tracks]
  end

  def tracks
    @cache.result_for(tracks_uncached) do
      tracks_uncached.map { |track| TrackListItem.new(track, store) }
    end
  end

  def batch_size
    100
  end

  def redraw
    store.dispatch({ type: :render })
  end

  def calc_offset
    [ ((@element.scroll.y - @hdr_height) / @item_height).to_i, 0 ].max
  end

  def calc_top
    @offset > 0 ? (@offset * @item_height) + @hdr_height : 0
  end

  def calc_height
    (tracks.length * @item_height) + @hdr_height
  end

  def created(element)
    @element = element
  end

  def created_table(element)
    @table_height = element.height
    @hdr_height = element.at('tr').height
    @item_height = (@table_height-@hdr_height)/batch_size
    @offset = 0
    redraw
  end

  def scroll(evt)
    @offset = [ calc_offset, tracks.length ].min
    redraw
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

  def styles
    top    = calc_top
    height = calc_height - top;
    {
      height: "#{height}px",
      position: "relative",
      top: "#{top}px"
    }
  end

  def render_tracks
    h 'div', styles: styles do
      h 'table', afterCreate: handler(:created_table) do
        [
          (
            render_header if @offset == 0
          ),
          (
            tracks[@offset..@offset+100].map do |track|
              track.render
            end
          )
        ]
      end
    end
  end

  def render
    h 'div#tracks', afterCreate: handler(:created), onscroll: handler(:scroll) do
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