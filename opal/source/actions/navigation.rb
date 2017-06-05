module Musciteer
  module NavigationActions
    def goto(path)
      @store.dispatch type: :goto, data: path
    end
  end

  module ActionDispatchHooks
    def goto(path)
      case path
      when '/albums'
        unless state[:albums]
          dispatch type: :albums_load, silent: true
        end
      when /\/albums\/(al.{4})/
        dispatch type: :album_details_load, data: $1, silent: true
      when '/tracks'
        unless state[:tracks]
          dispatch type: :tracks_load, silent: true
        end
      else
      end
      $window.history.push(path)
    end
  end
end
