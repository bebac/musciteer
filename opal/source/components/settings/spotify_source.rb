class SpotifySourceSettings < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    store.dispatch({ type: :spotify_source_settings_load })
  end

  def status
    store.state[:spotify_source_settings][:status]
  end

  def username
    store.state[:spotify_source_settings][:username]
  end

  def password
    store.state[:spotify_source_settings][:password]
  end

  def store_username(evt)
    store.state[:spotify_source_settings][:username] = evt.target.value
  end

  def store_password(evt)
    store.state[:spotify_source_settings][:password] = evt.target.value
  end

  def activate(evt)
    store.dispatch({
      type: :spotify_source_settings,
      data: { username: username, password: password }
    })
  end

  def render_header
    h 'h2', "Spotify Source"
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

  def render_username
    h 'div.settings-sub-item' do
      [
        (h 'div', "Username"),
        (
          h 'div' do
            h 'input', {
              type: 'text',
              value: username,
              oninput: handler(:store_username)
            }
          end
        ),
        (h 'div')
      ]
    end
  end

  def render_password
    h 'div.settings-sub-item' do
      [
        (h 'div', "Password"),
        (
          h 'div' do
            h 'input', {
              type: 'password',
              oninput: handler(:store_password)
            }
          end
        ),
        (h 'div')
      ]
    end
  end

  def render_activate
    h 'div.settings-sub-item' do
      [
        (h 'div'),
        (
          h 'div' do
            h 'button', { onclick: handler(:activate) } do
              h 'text', "activate"
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
      render_username,
      render_password,
      render_activate
    ]
  end
end

module ActionDispatchHooks
  def spotify_source_settings_load
    Browser::HTTP.get("/api/sources/spotify/settings") do |request|
      request.on :success do |response|
        if settings = response.json
          dispatch({ type: :spotify_source_settings_success, data: settings })
        end
      end
    end
  end

  def spotify_source_settings(data)
    puts "post spotify settings data=#{data}"
    Browser::HTTP.post("/api/sources/spotify/settings", data.to_json) do |request|
      request.content_type("application/json")
      request.on :success do |response|
        if settings = response.json
          dispatch({ type: :spotify_source_settings_success, data: settings })
        end
      end
    end
  end
end
