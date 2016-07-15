class Layout
  include Inesita::Layout

  def render
    div id: 'outlet-container' do
      component outlet
    end

    div id: 'connection-lost-overlay' do
      component ConnectionLost
    end
  end
end
