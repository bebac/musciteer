class Settings
  include Inesita::Component

  def init
    store.player_settings_sync
    store.spotify_settings_sync
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

      # Player.
      section do
        h2 do
          text "Continuous Playback"
        end
        div class: 'settings-item' do
          div do
            text "Enabled"
          end
          div class: 'togglebox' do
            input type: 'checkbox', checked: store.ctpb_enabled, onchange: -> (e) { store.update_ctpb_enabled(e.target.checked) }
          end
          div do
          end
        end
        div class: 'settings-item' do
          div do
            text "Type"
          end
          div do
            select class: 'setting', onchange: -> (e) {} do
              [ "random" ].each do |value|
                option selected: (value == store.ctpb_type ? "selected" : "") do
                  text value
                end
              end
            end
          end
          div do
          end
        end
      end

      # Audio ouput.
      section do
        h2 do
          text "Audio"
        end
        div class: 'settings-item' do
          div do
            text "Output Device"
          end
          div do
            select class: 'setting', onchange: -> (e) { store.set_audio_output_device(e.target.value) } do
              store.audio_device_list.each do |value|
                option selected: (value == store.audio_device ? "selected" : "") do
                  text value
                end
              end
            end
          end
          div do
          end
        end
      end

      # Local source.
      section do
        h2 do
          text "Local Source"
        end
        div class: 'settings-item' do
          div do
            text "Status"
          end
          div do
            p do; text store.source_local_status; end
          end
          div do
          end
        end
        div class: 'settings-item' do
          div do
            text "Directories"
          end
          div do
            store.directories.each_with_index do |directory, i|
              div do
                input type: 'text', value: directory, onchange: -> (e) { store.directories_set(i, e.target.value) }
              end
            end
            div do
              input type: 'text', value: "", onchange: -> (e) { store.directories_add(e.target.value) }
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

      # Spotify source.
      section do
        h2 do
          text "Spotify Source"
        end
        div class: 'settings-item' do
          div do
            text "Status"
          end
          div do
            p do; text store.source_spotify_status; end
          end
          div do
          end
        end
        div class: 'settings-sub-item' do
          div do
            text "Username"
          end
          div do
            input type: 'text', value: store.spotify_settings.username, onchange: -> (e) { store.spotify_settings.username = e.target.value }
          end
          div do
          end
        end
        div class: 'settings-sub-item' do
          div do
            text "Password"
          end
          div do
            input type: 'password', value: "", onchange: -> (e) { store.spotify_settings.password = e.target.value }
            div do
              div do
                button onclick: -> { store.spotify_settings_save } do
                  div do
                    text "activate"
                    #if store.?
                    div class: '' do; div; end
                    #end
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
