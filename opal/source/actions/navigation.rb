module Musciteer
  module NavigationActions
    def goto(path)
      @store.dispatch type: :goto, data: path
    end
  end

  module ActionDispatchHooks
    def goto(path)
      puts "action goto path=#{path}"
      $window.history.push(path)
    end
  end
end
