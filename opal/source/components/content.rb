module Musciteer
  class Content
    include Maquette::Component

    attr_reader :notification


    def initialize(store)
      @store = store
      @component_ = nil
      @notification = Notification.new(store)
    end

    def component
      @store.state[:component]
    end

    def save_scroll_top(evt)
      if component.respond_to?(:save_scroll_top)
        component.save_scroll_top(evt.target.scroll.y)
      end
    end

    def render
      # Create virtual node.
      node = h 'div.content', onscroll: handler(:save_scroll_top) do
        [
          component.render,
          notification.render
        ]
      end
      # Update scroll position.
      if @component_ && @component_ != component
        if component.respond_to?(:save_scroll_top)
          `node.properties.scrollTop = #{component.scroll_top}`
        end
      end
      # Set rendered componet.
      @component_ = component
      # Return virtual node.
      node
    end
  end
end
