module Musciteer
  module AlbumDetailsActions
    def album_details_load(album_id)
      store.dispatch type: :album_details_load, data: album_id
    end
  end

  module ActionDispatchHooks
    def album_details_load(album_id)
      Browser::HTTP.get("/api/albums/#{album_id}") do |req|
        req.on :success do |res|
          dispatch({
            type: :album_details_load_success,
            data: Album.new(res.json)
          })
        end

        req.on :error do |err|
          p err
        end
      end
    end
  end
end