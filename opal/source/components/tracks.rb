module Musciteer
  class Tracks
    include Maquette::Component

    attr_reader :store
    attr_reader :scroll_top

    def initialize(store)
      @store = store
      @cache = Maquette::Cache.new
      @item_height = 0
      @offset = 0
      @scroll_top = 0
    end

    def loading?
      store.state[:tracks_loading]
    end

    def tracks_uncached
      store.state[:tracks]
    end

    def tracks
      @cache.result_for(tracks_uncached) do
        tracks_uncached.sort { |x, y| x.play_count <=> y.play_count }.reverse
      end
    end

    def queue(track)
      store.dispatch type: :player_queue, data: track.id
    end

    def redraw
      store.dispatch type: :render
    end

    def adjust_table_header(element)
      thead = element.at_css('thead')
      tbody = element.at_css('tbody')
      # Add the width of the scrollbar as padding to the last th.
      element.at_css("th:last-child").style padding: "0 #{thead.scroll.width-tbody.scroll.width}px 0 0"
    end

    def created(element)
      $window.on(:resize) do |evt|
        adjust_table_header(element)
      end
      animation_frame do
        adjust_table_header(element)
      end
    end

    def batch_size
      40
    end

    def created_tbody(element)
      @tbody = element
      if @item_height == 0
        animation_frame do
          @item_height = @tbody.scroll.height / batch_size
          redraw
        end
      else
        # Should really set scroll top, but I can't figure out how.
        animation_frame do
          @offset = 0
          redraw
        end
      end
    end

    def calc_offset
      [ (@tbody.scroll.y / @item_height).to_i, 0 ].max
    end

    def calc_top
      @offset > 0 ? (@offset * @item_height) : 0
    end


    def height_total
      tracks.length * @item_height
    end

    def height_top
      @offset * @item_height
    end

    def height_bottom
      height_total - ((batch_size + @offset) * @item_height)
    end

    def scroll(evt)
      @offset = [ calc_offset, tracks.length ].min
      @scroll_top = evt.target.scroll.y
      #redraw
    end

    def render_loading
      h 'div.loader-container' do
        h 'div.loader'
      end
    end

    def render_thead
      h 'thead' do
        h 'tr' do
          [
            (h 'th', "Track"),
            (h 'th', "Artist"),
            (h 'th', "Album"),
            (h 'th', "Plays"),
            (h 'th', "Skips")
          ]
        end
      end
    end

    def render_tbody
      node = h 'tbody', afterCreate: handler(:created_tbody), onscroll: handler(:scroll) do
      #node = h 'tbody', afterCreate: handler(:created_tbody) do
        [
          h 'tr.top', styles: { height: "#{height_top}px" }
        ].concat(
          tracks[@offset..@offset+batch_size].map do |track|
            h "tr##{track.id}", onclick: handler(:queue, track) do
              [
                (h 'td', track.title),
                (h 'td', track.artists),
                (h 'td', track.album),
                (h 'td', track.play_count),
                (h 'td', track.skip_count)
              ]
            end
          end
        ).concat (
          [
            h 'tr.bottom', styles: { height: "#{height_bottom}px" }
          ]
        )
      end

      if scroll_top > 0
        #`node.properties.scrollTop = #{scroll_top}`
      end
      node
    end

    def render_tracks
      h 'table', afterCreate: handler(:created) do
        [
          render_thead,
          render_tbody
        ]
      end
    end

    def render
      h 'div#tracks' do
        if loading?
          render_loading
        elsif tracks
          render_tracks
        else
          h 'text', "error loading tracks"
        end
      end
    end
  end
end