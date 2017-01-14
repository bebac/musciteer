class AnimatedButton < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @cache = Maquette::Cache.new
    @state = :armed
  end

  def clicked
    raise "click handler not implemented"
  end

  def enter_animation(node)
    if @state == :clicked
      circle = node.css('.animated').first

      circle['r'] = "110"
      circle['fill-opacity'] = "0.2"

      animate(circle)
    end
  end

  def exit_animation(node)
    @state = :armed
    node.remove
  end

  def animate(circle)
    animation_frame do
      r = circle["r"].to_i

      if r < 200
        r += 10
      end

      circle['r'] = r

      opacity = circle['fill-opacity'].to_f

      opacity -= 0.02

      if opacity < 0
        opacity = 0
      end

      circle['fill-opacity'] = opacity

      if opacity > 0
        animate(circle)
      else
        @state = :armed
        store.dispatch({ type: :render })
      end
    end
  end

  def click(evt)
    clicked
    @state = :clicked
    enter_animation(evt.target.at('../..'))
    evt.stop!
  end

  def render_animated_circle
    h 'g', { "stroke-width" => "0" } do
      (h 'circle.animated', { cx: "100", cy: "100", r: "0", 'fill-opacity': "0", fill: "#000" })
    end
  end

  def render_button
    raise "render_button not implemented"
  end

  def render_click_area
    h 'g', { "stroke-width" => "0" } do
      (h 'circle', { key: self, cx: "100", cy: "100", r: "100", 'fill-opacity': "0", onclick: handler(:click) })
    end
  end

  def render
    @cache.result_for([ self, @state ]) do
      h 'svg.button', {
        key: [ self, @state ],
        enterAnimation: handler(:enter_animation),
        exitAnimation: handler(:exit_animation),
        viewBox: "0 0 200 200"
      } do
        [
          render_animated_circle,
          render_button,
          render_click_area
        ]
      end
    end
  end
end
