module Maquette
  class Projector
    include Native

    def initialize
      @native = `maquette.createProjector()`
    end

    alias_native :append
    alias_native :render, :scheduleRender
  end

  class Cache
    def initialize
      @input = nil
      @result = nil
    end

    def result_for(input, &blk)
      if @input != input
        @result = blk.call
        @input = input
      end
      @result
    end
  end

  module Hyperscript
    def h(selector, props = nil, children = nil, &block)
      Maquette.make_vnode(selector, props.to_n, block_given? ? block.call : children)
    end
  end

  class Component
    include Hyperscript

    def handlers
      @handlers ||= {}
    end

    def handler(m, *args)
      handlers[[ m, args ]] ||= lambda do |evt|
        method(m).call(*args, Browser::Event.new(evt))
      end
    end
  end

  extend self

  def make_vnode(selector, props, children)
    `maquette.h(selector, props, children)`
  end
end
