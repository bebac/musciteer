require_relative 'animated_button'

class CloseButton < AnimatedButton
  attr_reader :store

  def initialize(store, &blk)
    super(store)
    @action = blk
  end

  def clicked
    @action.call
  end

  def render_button
    h 'g', { "stroke-width" => "24" } do
      [
        (h 'path', d: "M50,50l100,100"),
        (h 'path', d: "M50,150l100,-100")
      ]
    end
  end
end