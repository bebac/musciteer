require 'opal'
require 'browser'
require 'browser/socket'
require 'browser/location'
require 'browser/interval'
require 'browser/effects'
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

  def audio_device_list_sync
    message_channel_send({ :event => :audio_device_list_sync })
  end

  def set_audio_output_device(device_name)
  end

  def audio_device_list
    @audio_device_list ||= []
  end

  private

  def message_queue
    @mq ||= Array.new
  end

  def message_queue_flush
    while message=message_queue.pop
      message_channel_send(message)
    end
  end

  def message_channel_send(message)
    if @channel.alive?
      @channel.send(message.to_json)
    else
      message_queue << message
    end
  end

  def message_channel_recv(message)
    p "Received #{message}"
    case message['event']
    when "audio_device_list"
      @audio_device_list = message['data']
      render!
    end
  end

  def message_channel_start
    ws_url = "ws://#{$document.location.host}"
    @channel = Browser::Socket.new ws_url do |ws|
    #@channel = Browser::Socket.new "ws://#{$document.location.host}" do # Does not work!!!
      ws.on :open do
        message_queue_flush
      end

      ws.on :message do |e|
        message_channel_recv(JSON.parse(e.data))
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
