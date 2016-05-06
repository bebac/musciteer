require 'opal'
require 'browser'
require 'browser/socket'
require 'browser/location'
require 'browser/interval'
require 'json'
require 'inesita'

require 'router'
require 'layout'

require_tree './components'

class Store
  include Inesita::Store

  def initialize
    super
    message_channel_start
  end

  def message_channel_start
    ws_url = "ws://#{$document.location.host}"
    @channel = Browser::Socket.new ws_url do |ws|
    #@channel = Browser::Socket.new "ws://#{$document.location.host}" do # Does not work!!!
      ws.on :open do
        puts "message channel open"
        @channel.send("hello")
        @channel.send("world")
      end

      ws.on :message do |e|
        p JSON.parse(e.data)
      end
    end
  end
end

$document.ready do
  Inesita::Application.new(
    router: Router,
    store: Store,
    layout: Layout
  ).mount_to($document.body)
end
