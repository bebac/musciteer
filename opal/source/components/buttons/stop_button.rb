require_relative 'animated_button'

class StopButton < AnimatedButton
  attr_reader :store

  def initialize(store)
    super(store)
  end

  def clicked
    store.dispatch({ type: :player_stop })
  end

  def render_button
    h 'g' do
      [
        (h 'circle', { cx: "100", cy: "100", r: "92", fill: "none", "stroke-width" => "8" }),
        (h 'path', d: "M75,75h50v50h-50")
      ]
    end
  end
end
