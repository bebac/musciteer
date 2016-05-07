class Settings
  include Inesita::Component

  def init
    p "init"
    store.audio_device_list_sync
  end

  def set_audio_output_device(evt)
    store.set_audio_output_device(evt.target.value)
  end

  def render
    div id: 'settings' do
      select class: 'setting', onchange: method(:set_audio_output_device) do
        store.audio_device_list.each do |value|
          option do
            text value
          end
        end
      end
    end
  end
end
