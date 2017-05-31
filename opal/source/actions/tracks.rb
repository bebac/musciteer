module Musciteer
  module ActionDispatchHooks
    def tracks_load
      Browser::HTTP.get("/api/tracks") do |req|
        req.on :success do |res|
          dispatch({
            type: :tracks_load_success,
            data: res.json.map { |t| Track.new(t) }
          })
        end

        req.on :error do |err|
          p err
        end
      end
    end
  end
end