class Router < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @paths = {
      '/albums'  => Albums.new(store),
      '/tracks'  => TrackList.new(store),
      '/player'  => Player.new(store),
      '/spotify' => Spotify.new(store)
    }
    $window.on 'pop:state' do |evt|
      set_path($document.location.path)
    end
    if $document.location.path == "/"
      $window.history.replace('/albums')
    end
    set_path($document.location.path)
  end

  def path_active?(path)
    path == $document.location.path
  end

  def set_path(path)
    store.dispatch({ type: :set_path, data: path })
  end

  def goto(path, evt)
    set_path(path)
    $window.history.push(path)
    evt.stop!
  end

  def link(path, text)
    h path_active?(path) ? 'a.active' : 'a', { href: path, onclick: handler(:goto, path) }, text
  end

  def render_navigation
    h 'div#navbar' do
      h 'ul' do
        [
          (h 'li', { key: 1 }, link('/albums', 'Albums')),
          (h 'li', { key: 2 }, link('/tracks', 'Tracks')),
          (h 'li', { key: 3 }, link('/player', 'Player')),
          (h 'li', { key: 4 }, link('/spotify', 'Spotify'))
        ]
      end
    end
  end

  def render_path
    if component = @paths[store.state[:path]]
      component.render
    else
      h 'p', "nothing here"
    end
  end
end

module ActionDispatchHooks
  def set_path(path)
    if path == '/tracks'
      dispatch({ type: :tracks_load }) unless state[:tracks]
    elsif path == '/albums'
      dispatch({ type: :albums_load }) unless state[:albums]
    end
  end
end