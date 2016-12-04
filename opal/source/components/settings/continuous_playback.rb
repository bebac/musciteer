class ContinuousPlaybackSettings < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    store.dispatch({ type: :ctpb_settings_load })
  end

  def enabled?
    store.state[:ctpb_settings][:enabled]
  end

  def current_type
    store.state[:ctpb_settings][:type]
  end

  def available_types
    [ "random", "less-played", "more-played" ]
  end

  def toggle(evt)
    store.dispatch({ type: :ctpb_settings_enable, data: enabled? ? false : true })
  end

  def set_type(evt)
    store.dispatch({ type: :ctpb_settings_type, data: evt.target.value })
  end

  def render_header
    h 'h2', "Continuous Playback"
  end

  def render_enabled
    h 'div.settings-item' do
      [
        (h 'div', "Enabled"),
        (
          h 'div.togglebox' do
            h 'input', {
              type: 'checkbox',
              checked: enabled?,
              onchange: handler(:toggle)
            }
          end
        ),
        (h 'div')
      ]
    end
  end

  def render_type
    h 'div.settings-item' do
      [
        (h 'div', "Type"),
        (
          h 'div' do
            h 'select.setting', { key: self, onchange: handler(:set_type) } do
              available_types.map do |type|
                h 'option', { key: type, selected: current_type == type ? true : false }, type
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
      render_enabled,
      render_type
    ]
  end
end

module ActionDispatchHooks
  def ctpb_settings_load
    Browser::HTTP.get("/api/player/ctpb") do |request|
      request.on :success do |response|
        if ctpb = response.json
          dispatch({ type: :ctpb_settings_success, data: ctpb})
        end
      end
    end
  end

  def ctpb_settings_enable(value)
    Browser::HTTP.post("/api/player/ctpb", data={"enabled" => value}.to_json) do |request|
      request.content_type("application/json")
      request.on :success do |response|
        if ctpb = response.json
          dispatch({ type: :ctpb_settings_success, data: ctpb})
        end
      end
    end
  end

  def ctpb_settings_type(value)
    Browser::HTTP.post("/api/player/ctpb", data={"type" => value}.to_json) do |request|
      request.content_type("application/json")
      request.on :success do |response|
        if ctpb = response.json
          dispatch({ type: :ctpb_settings_success, data: ctpb})
        end
      end
    end
  end
end
