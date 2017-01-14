require_relative 'animated_button'

class PlayButton < AnimatedButton
  attr_reader :store
  attr_reader :track_id

  def initialize(store, track_id=nil)
    super(store)
    @track_id = track_id
  end

  def clicked
    store.dispatch({ type: :player_start, data: track_id })
  end

  def render_button
    h 'g', key: self do
      [
        (h 'circle', { cx: "100", cy: "100", r: "92", fill: "none", 'stroke-width': "8" }),
        (h 'path', d: "M90,65v70l35,-35")
      ]
    end
  end
end
