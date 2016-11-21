class AudioSettings < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    store.dispatch({ type: :audio_settings_load })
    store.dispatch({ type: :audio_replaygain_load })
  end

  def current_device
    store.state[:audio_settings][:current]
  end

  def available_devices
    store.state[:audio_settings][:devices]
  end

  def replaygain_enabled?
    store.state[:replaygain_enabled]
  end

  def set_device(evt)
    store.dispatch({ type: :audio_settings_set_device, data: evt.target.value })
  end

  def toggle_replaygain(evt)
    store.dispatch({ type: :audio_replaygain_set, data: replaygain_enabled? ? false : true })
  end

  def render_header
    h 'h2', "Audio"
  end

  def render_device
    h 'dev.settings-item' do
      [
        (h 'div', "Output Device"),
        (
          h 'div' do
            h 'select.setting', { onchange: handler(:set_device) } do
              available_devices.map do |device|
                h 'option', {
                  key: device,
                  selected: current_device == device ? true : false,
                },
                device
              end
            end
          end
        ),
        (h 'div')
      ]
    end
  end

  def render_replaygain
    h 'div.settings-item' do
      [
        (h 'div', "Replaygain"),
        (
          h 'div.togglebox' do
            h 'input', {
              type: 'checkbox',
              checked: replaygain_enabled?,
              onchange: handler(:toggle_replaygain)
            }
          end
        ),
        (h 'div')
      ]
    end
  end

  def render
    [
      render_header,
      render_device,
      render_replaygain
    ]
  end
end

module ActionDispatchHooks
  def audio_settings_load
    Browser::HTTP.get("/api/player/output") do |request|
      request.on :success do |response|
        if output = response.json
          dispatch({ type: :audio_settings_success, data: output })
        end
      end
    end
  end

  def audio_settings_set_device(device_name)
    Browser::HTTP.post("/api/player/output", data={"current" => device_name}.to_json) do |request|
      request.content_type("application/json")
      request.on :success do |response|
        if output = response.json
          dispatch({ type: :audio_settings_success, data: output })
        end
      end
    end
  end

  def audio_replaygain_load
    Browser::HTTP.get("/api/player/replaygain") do |request|
      request.on :success do |response|
        if output = response.json
          dispatch({ type: :audio_replaygain_success, data: output })
        end
      end
    end
  end

  def audio_replaygain_set(value)
    Browser::HTTP.post("/api/player/replaygain", data={"enabled" => value}.to_json) do |request|
      request.content_type("application/json")
      request.on :success do |response|
        if output = response.json
          dispatch({ type: :audio_replaygain_success, data: output })
        end
      end
    end
  end
end
