class AudioSettings < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    store.dispatch({ type: :audio_settings_load })
  end

  def current_device
    store.state[:audio_settings][:current]
  end

  def available_devices
    store.state[:audio_settings][:devices]
  end

  def set_device(evt)
    store.dispatch({ type: :audio_settings_set_device, data: evt.target.value })
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

  def render
    [
      render_header,
      render_device,
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
end
