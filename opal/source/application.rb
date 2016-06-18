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

  def settings_shown?
    @settings_shown
  end

  def settings_toggle
    if @settings_shown
      $document.at('#settings-overlay').hide
      @settings_shown = false
    else
      $document.at('#settings-overlay').show
      @settings_shown = true
    end
    render!
  end

  def notifications
    @notifications ||= []
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

  def directories
    Browser::HTTP.get("/api/sources/local/directories") do |request|
      request.synchronous!
      request.on :success do |response|
        @directories = response.json
      end
    end
    @directories
  end

  def directories_set(index, value)
    if value.empty?
      @directories.delete_at(index)
    else
      @directories[index] = value
    end
    post_directories
    render!
  end

  def directories_add(value)
    if not value.empty?
      @directories << value
    end
    post_directories
    render!
  end

  def post_directories
    Browser::HTTP.post("/api/sources/local/directories", data=@directories.to_json) do |request|
      request.synchronous!
      request.content_type("application/json")
      request.on :success do |response|
        #@directories = response.json
      end
    end
  end

  def stream_data(stream_id)
    if @stream_id and @stream_id == stream_id
      @stream_data
    else
      message_channel_send({ :event => :stream_data_sync, :data => stream_id })
      nil
    end
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
    case message['event']
    when "audio_device_list"
      @audio_device = message["data"][0]
      @audio_device_list = message['data'][1]
      render!
    when "queue_update"
      notifications << QueueUpdate.new(message['data'])
      $document.at('#notification-overlay').show
      render!
    when "stream_begin"
      notifications << StreamBegin.new(message['data'])
      $document.at('#notification-overlay').show
      render!
    when "stream_data"
      @stream_id = message['data']['stream_id']
      @stream_data = Track.new(message['data']['track'])
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
