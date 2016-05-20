require 'opal'
require 'browser'
require 'browser/socket'
require 'browser/http'
require 'browser/location'
require 'browser/interval'
require 'browser/effects'
require 'json'
require 'inesita'

require 'router'
require 'layout'

require_tree './components'
require_tree './models'

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
    message_channel_send({ :event => :audio_device, :data => device_name })
  end

  def audio_device
    @audio_device ||= ""
  end

  def audio_device_list
    @audio_device_list ||= []
  end

  def load_tracks
    Browser::HTTP.get("/api/tracks") do |request|
      request.on :success do |response|
        response.json.each do |attrs|
          tracks << Track.new(attrs)
        end
        render!
      end
    end
  end

  def tracks
    @tracks ||= []
  end

  #def tracks
  #  [
  #    Track.new(:id => 1, :title => "I Told You So", :artists => "Carrie Underwood", :album => "I Told You So"),
  #    Track.new(:id => 2, :title => "Somebody Like You", :artists => "Keith Urban", :album => "Days Go By")
  #  ]
  #end

  def load_albums
    Browser::HTTP.get("/api/albums") do |request|
      request.on :success do |response|
        response.json.each do |attrs|
          albums << Album.new(attrs)
        end
        albums.sort! do |x, y|
          (x.artist <=> y.artist).nonzero? ||
          (x.title <=> y.title)
        end
        render!
      end
    end
  end

  def albums
    @albums ||= []
  end

  def load_album_details(album_id)
    @album = @albums.find { |x| x.id == album_id }
    Browser::HTTP.get("/api/albums/#{album_id}/tracks") do |request|
      request.on :success do |response|
        @album.tracks.clear
        response.json.each do |attrs|
          @album.tracks << Track.new(attrs)
        end
        @album.tracks.sort! { |x, y| x.index <=> y.index }
        render!
      end
    end
  end

  def album_details
    @album
  end

  def play(id)
    message_channel_send({ event: :play, data: id })
  end

  def queue(id)
    message_channel_send({ event: :queue, data: id })
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
    #p "Received #{message}"
    case message['event']
    when "audio_device_list"
      @audio_device = message["data"][0]
      @audio_device_list = message['data'][1]
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
