require_relative 'animated_button'

class SkipButton < AnimatedButton
  attr_reader :store

  def initialize(store)
    super(store)
  end

  def clicked
    store.dispatch({ type: :player_skip })
  end

  def render_button
    h 'g', key: self, 'stroke-width': "8" do
      [
        (h 'circle', { cx: "100", cy: "100", r: "92", fill: "none" }),
        (h 'path', d: "M 90,60 120,100 90,140", fill: "none")
      ]
    end
  end
end
