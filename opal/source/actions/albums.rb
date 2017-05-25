module Musciteer
  module AlbumsActions
    def albums_load
      @store.dispatch type: :albums_load
    end
  end

  module ActionDispatchHooks
    def albums_load
      Browser::HTTP.get("/api/albums") do |req|
        req.on :success do |res|
          dispatch({
            type: :albums_load_success,
            data: res.json.map { |t| Album.new(t) }
          })
        end

        req.on :error do |err|
          p err
        end
      end
    end
  end
end