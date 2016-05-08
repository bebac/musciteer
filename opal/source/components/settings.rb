class Settings
  include Inesita::Component

  def init
    store.audio_device_list_sync
  end

  def set_audio_output_device(evt)
    store.set_audio_output_device(evt.target.value)
  end

  def render
    div id: 'settings' do

      div id: 'settings-header' do
        h1 do
          text 'Settings'
        end
      end

      section do
        div class: 'settings-item' do
          div do
            text "Audio output"
          end
          div do
            select class: 'setting', onchange: method(:set_audio_output_device) do
              store.audio_device_list.each do |value|
                option do
                  text value
                end
              end
            end
          end
          div do
          end
        end
      end
    end
  end
end
