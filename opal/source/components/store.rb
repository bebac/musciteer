module Musciteer
  module ActionDispatchHooks
  end

  class Store
    attr_reader :state

    include ActionDispatchHooks

    def initialize(initial_state = nil, &reducer)
      @state = initial_state
      @reducer = reducer || fail("reducer is required")
      @subscribers = []
      dispatch
    end

    def dispatch(action = {})
      if type = action[:type]
        send(type, action[:data]) if respond_to?(type)
      end
      @new_state = @reducer.call(@state, action)
      unless @new_state == @state
        @state = @new_state
        @subscribers.each { |s| s.call }
      end
    end

    def subscribe(&blk)
      @subscribers << blk
      -> { @subscribers.delete(blk) }
    end
  end
end
