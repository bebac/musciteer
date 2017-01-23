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
    h 'g', { "stroke-width" => "16" } do
      [
        (h 'path', d: "M70,70l70,70"),
        (h 'path', d: "M70,140l70,-70")
      ]
    end
  end
end