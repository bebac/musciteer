class Spotify < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
  end

  def importing?
    store.state[:spotify_importing]
  end

  def url
    @url ||= ""
  end

  def store_url(evt)
    @url = evt.target.value
  end

  def make_path_from_url
    case url
    when /https:\/\/play\.spotify\.com\/album\/(.*)/
      "/v1/albums/#{$1}"
    when /https:\/\/play\.spotify\.com\/track\/(.*)/
      "/v1/tracks/#{$1}"
    when /^\/v1\/(albums|tracks)\//
      url
    end
  end

  def import(evt)
    if path = make_path_from_url
      store.dispatch(type: :spotify_import, data: { url: path })
    else
      puts "invalid import path #{path}"
    end
  end

  def render
    h "div#spotify" do
      h 'div' do
        [
          (
            h 'div' do
              [
                (h 'p', "Enter Spotify path to import"),
                (h 'p', "Copy Spotify URL from web player: https://play.spotify.com/album/08ibdX8K0GETv20UEku3sm"),
                (h 'p', "or"),
                (h 'p', "/v1/albums/08ibdX8K0GETv20UEku3sm"),
              ]
            end
          ),
          (
            h 'input', {
              type: 'text', value: url,
              oninput: handler(:store_url)
            }
          ),
          (
            h 'p' do
              h 'button.text', { onclick: handler(:import) } do
                h 'div' do
                  [
                    (h 'text', "import"),
                    if importing?
                      (
                        h 'div.importing' do
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
    end
  end
end

module ActionDispatchHooks
  def spotify_import(data)
    unless state[:spotify_importing]
      Browser::HTTP.post("/api/spotify/import", data.to_json) do |request|
        request.content_type("application/json")
        request.on :success do |response|
          dispatch({ type: :spotify_import_success })
        end
      end
    end
  end
end