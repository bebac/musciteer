class SpotifyWebSettings < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    store.dispatch({ type: :spotify_web_settings_load })
  end

  def authorized?
    store.state[:spotify_web][:authorized]
  end

  def status
    if authorized?
      "authorized"
    else
      "not authorized"
    end
  end

  def revoke(evt)
    store.dispatch({ type: :spotify_web_revoke })
  end

  def render_header
    h 'h2', "Spotify Web"
  end

  def render_status
    h 'div.settings-item' do
      [
        (h 'div', "Status"),
        (h 'div', status),
        (h 'div')
      ]
    end
  end

  def render_action
    h 'div.settings-sub-item' do
      [
        (h 'div'),
        (
          h 'div' do
            if authorized?
              h 'button', { onclick: handler(:revoke) } do
                h 'text', "revoke"
              end
            else
              h 'button' do
                h 'a', { href: "/api/spotify/authorize?host=#{$document.location.host.encode_uri_component}" }, "authorize"
              end
            end
          end
        ),
        (h 'div')
      ]
    end
  end

  def render
    [
      render_header,
      render_status,
      render_action
    ]
  end
end

module ActionDispatchHooks
  def spotify_web_settings_load
    Browser::HTTP.get("/api/spotify/settings") do |request|
      request.on :success do |response|
        if settings = response.json
          dispatch({ type: :spotify_web_settings_success, data: settings })
        end
      end
    end
  end

  def spotify_web_revoke
    Browser::HTTP.post("/api/spotify/revoke") do |request|
      request.on :success do |response|
        if settings = response.json
          dispatch({ type: :spotify_web_settings_success, data: settings })
        end
      end
    end
  end
end