class Settings
  include Inesita::Component

  def init
    store.audio_device_list_sync
  end

  def render
    if store.settings_shown?
      render_shown
    else
      nil
    end
  end

  def render_shown
    div id: 'settings' do

      div id: 'settings-header' do
        h1 do
          text 'Settings'
        end
      end

      # Audio ouput.
      section do
        div class: 'settings-item' do
          div do
            text "Audio output"
          end
          div do
            select class: 'setting', onchange: -> (e) { store.set_audio_output_device(e.target.value) } do
              option do; text store.audio_device; end
              store.audio_device_list.each do |value|
                option do; text value end
              end
            end
          end
          div do
          end
        end
      end

      # Directories.
      section do
        div class: 'settings-item' do
          div do
            text "Directories"
          end
          div do
            store.directories.each_with_index do |directory, i|
              div do
                input value: directory, onchange: -> (e) { store.directories_set(i, e.target.value) }
              end
            end
            div do
              input value: "", onchange: -> (e) { store.directories_add(e.target.value) }
            end
            div do
              div do
                button onclick: -> { store.source_local_scan } do
                  div do
                    text "scan"
                    if store.source_local_scanning?
                      div class: 'scanning' do; div; end
                    end
                  end
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
