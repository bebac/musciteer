class Main
  include Inesita::Component

  def initialize
  end

  def play
    store.play
  end

  def render
    div id: 'play', onclick: method(:play) do
      text 'Play'
    end
  end
end
