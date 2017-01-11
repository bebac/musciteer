require_relative 'animated_button'

class FullscreenButton < AnimatedButton
  attr_reader :store

  def initialize(store)
    super(store)
  end

  def clicked
    `document.documentElement.webkitRequestFullscreen()`
  end

  def render_button
    h 'g' do
      [
        (h 'path', d: "M110,90l50,-50", 'stroke-width': "24"),
        (h 'path', d: "M90,110l-50,50", 'stroke-width': "24"),
        (h 'path', d: "M180,20v40l-40,-40M20,180v-40l40,40")
      ]
    end
  end
end