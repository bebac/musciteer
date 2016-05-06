class Layout
  include Inesita::Layout

  def render
    div id: 'outlet-container' do
      component outlet
    end
  end
end
