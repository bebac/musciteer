class Top < Maquette::Component
  include PlayerAssets
  include PlayerState
  include PlayerActions

  attr_reader :store

  def initialize(store)
    @store = store
  end

  def toggle_menu(evt)
    $document.at('#menu').toggle
  end

  def toggle_settings
    $document.at('#settings-overlay').toggle
  end

  def render_menu_button
    h 'div#top-menu' do
      h 'button.icon', { onclick: handler(:toggle_menu) } do
        h 'img', { src: 'assets/menu.svg' }
      end
    end
  end

  def render_search_box
    h 'div#top-search' do
      #h 'input', {
      #  type: 'text', value: '', placeholder: "search"
      #}
    end
  end

  def render_player_info
    h 'div#top-player-info' do
      if stream_synced?
        title = track.title.length > 25 ? track.title[0..25] + "..." : track.title
        [
          (h 'span.title', title),
          (h 'span.by', " by "),
          (h 'span.artists', track.artists)
        ]
      elsif !playing?
        h 'text', "[ stopped ]"
      end
    end
  end

  def render_player_ctrl
    h 'div#top-player-ctrl' do
      if playing?
        [
          (h 'button', key: 'stop.1', onclick: handler(:stop), innerHTML: stop_svg),
          (h 'button', key: 'skip.1', onclick: handler(:skip), innerHTML: skip_svg)
        ]
      else
        [
          (h 'button', key: 'play.1', onclick: handler(:play), innerHTML: play_svg),
          (h 'button', key: 'skip.0',                          innerHTML: skip_svg)
        ]
      end
    end
  end

  def render_settings_button
    h 'div#top-settings' do
      h 'button.icon', { onclick: handler(:toggle_settings) } do
        h 'img', { src: 'assets/settings.svg' }
      end
    end
  end

  def render
    [
      render_menu_button,
      render_search_box,
      render_player_info,
      render_player_ctrl,
      render_settings_button
    ]
  end
end
