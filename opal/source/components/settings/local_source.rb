class LocalSourceSettings < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    store.dispatch({ type: :local_source_settings_load })
  end

  def status
    store.state[:local_source_settings][:status]
  end

  def directories
    store.state[:local_source_settings][:directories]
  end

  def store_directory(i, evt)
    directories[i] = evt.target.value
  end

  def update_directory(i, evt)
    store.dispatch({
      type: :local_source_settings_directories,
      data: directories.compact
    })
  end

  def add_directory(evt)
    if evt.target.value
      store.dispatch({
        type: :local_source_settings_directories,
        data: directories.push(evt.target.value)
      })
    end
  end

  def scanning?
    store.state[:local_source_scanning]
  end

  def scan(evt)
    store.dispatch({ type: :local_source_scan })
  end

  def render_header
    h 'h2', "Local Source"
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

  def render_directories
    h 'div.settings-item' do
      [
        (h 'div', "Directories"),
        (
          h 'div' do
            [
              (
                h 'div' do
                  directories.map.with_index do |dir, i|
                    h 'input', {
                      type: 'text', value: dir, key: i,
                      oninput: handler(:store_directory, i),
                      onchange: handler(:update_directory, i)
                    }
                  end
                end
              ),
              (
                h 'div' do
                  h 'input', {
                    type: 'text', value: '', placeholder: "new directory",
                    onchange: handler(:add_directory)
                  }
                end
              ),
              (
                h 'div' do
                  h 'button', { onclick: handler(:scan) } do
                    h 'div' do
                      [
                        (h 'text', "scan"),
                        if scanning?
                          (
                            h 'div.scanning' do
                              h 'div'
                            end
                          )
                        else
                          (h 'div')
                        end
                      ]
                    end
                  end
                end
              )
            ]
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
      render_directories
    ]
  end
end

module ActionDispatchHooks
  def local_source_settings_load
    Browser::HTTP.get("/api/sources/local/directories") do |request|
      request.on :success do |response|
        if directories = response.json
          dispatch({ type: :local_source_settings_success, data: directories })
        end
      end
    end
  end

  def local_source_settings_directories(directories)
    Browser::HTTP.post("/api/sources/local/directories", data=directories.to_json) do |request|
      request.content_type("application/json")
      request.on :success do |response|
        if directories = response.json
          dispatch({ type: :local_source_settings_success, data: directories })
        end
      end
    end
  end

  def local_source_scan
    unless state[:local_source_scanning]
      Browser::HTTP.post("/api/sources/local/scan") do |request|
        request.on :success do |response|
          dispatch({ type: :local_source_scan_success })
        end
      end
    end
  end
end