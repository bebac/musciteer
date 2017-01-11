require_relative 'animated_button'

class PlayButton < AnimatedButton
  attr_reader :store

  def initialize(store)
    super(store)
  end

  def clicked
    store.dispatch({ type: :player_start })
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
