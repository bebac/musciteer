class Top < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @close = CloseButton.new(store) { pop_state }
  end

  def path
    store.state[:path]
  end

  def toggle_menu(evt)
    $document.at('#menu').toggle
  end

  def toggle_settings
    $document.at('#settings-overlay').toggle
  end

  def pop_state(evt)
    $window.history.back
  end

  def render_menu_button
    h 'div#top-menu' do
      h 'button.icon', onclick: handler(:toggle_menu), innerHTML: menu_svg
    end
  end

  def render_search_box
    h 'div#top-search' do
      #h 'input', {
      #  type: 'text', value: '', placeholder: "search"
      #}
    end
  end

  def render_close_button
    h 'div#top-close' do
      unless path =~ /^\/albums/
        @close.render
      else
        h 'button.icon', { key: 'settings.1', onclick: handler(:toggle_settings) } do
          h 'img', { src: 'assets/settings.svg' }
        end
      end
    end
  end

  # def render_settings_button
  #   h 'div#top-settings' do
  #     h 'button.icon', { onclick: handler(:toggle_settings) } do
  #       h 'img', { src: 'assets/settings.svg' }
  #     end
  #   end
  # end

  def render
    [
      render_menu_button,
      #render_search_box,
      #render_settings_button
      render_close_button
    ]
  end

  def menu_svg
    <<-svg
<svg viewBox="0 0 200 200">
  <g stroke-width="16" stroke-linecap="round">
    <path d="M40,60h110"/>
    <path d="M40,100h110"/>
    <path d="M40,140h110"/>
  </g>
</svg>
    svg
  end
end
