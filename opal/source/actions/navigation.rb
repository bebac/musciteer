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
          dispatch type: :albums_load
        end
      when /\/albums\/(al.{4})/
        dispatch type: :album_details_load, data: $1
      else
      end
      $window.history.push(path)
    end
  end
end
