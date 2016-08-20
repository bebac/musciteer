class Store
  include Inesita::Store
  include Eventable

  def initialize
    super
    message_channel_start
    $document.on :visibilitychange do
      render! unless hidden?
    end
  end

  def notifications
    @notifications ||= []
  end

  def player_state
    @player_state ||= :stopped
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

  def player_settings_sync
    Browser::HTTP.get("/api/player") do |request|
      request.on :success do |response|
        if output = response.json["output"]
          @audio_device = output["current"]
          @audio_device_list = output["devices"]
        end
        if ctpb = response.json["ctpb"]
          @ctpb_enabled = ctpb["enabled"]
          @ctpb_type = ctpb["type"]
        end
        render!
      end
    end
  end

  def set_audio_output_device(device_name)
    Browser::HTTP.post("/api/player/output", data={"current" => device_name}.to_json) do |request|
      request.content_type("application/json")
      request.on :success do |response|
        if output = response.json
          @audio_device = output["current"]
          @audio_device_list = output["devices"]
        end
        render!
      end
    end
  end

  def audio_device
    @audio_device ||= ""
  end

  def audio_device_list
    @audio_device_list ||= []
  end

  def ctpb_enabled
    @ctpb_enabled
  end

  def update_ctpb_enabled(value)
    Browser::HTTP.post("/api/player/ctpb", data={"enabled" => value}.to_json) do |request|
      request.content_type("application/json")
      request.on :success do |response|
        if ctpb = response.json
          @ctpb_enabled = ctpb["enabled"]
          @ctpb_type = ctpb["type"]
        end
        render!
      end
    end
  end

  def ctpb_type
    @ctpb_type ||= ""
  end

  def source_local_status
    @source_local_status ||= "Ok"
  end

  def directories
    unless @directories
      Browser::HTTP.get("/api/sources/local/directories") do |request|
        request.on :success do |response|
          @directories = response.json
          render!
        end
      end
      []
    else
      @directories
    end
  end

  def directories_set(index, value)
    if value.empty?
      @directories.delete_at(index)
    else
      @directories[index] = value
    end
    post_directories
  end

  def directories_add(value)
    if not value.empty?
      @directories << value
    end
    post_directories
  end

  def post_directories
    Browser::HTTP.post("/api/sources/local/directories", data=@directories.to_json) do |request|
      request.content_type("application/json")
      request.on :success do |response|
        #@directories = response.json
        render!
      end
    end
  end

  def source_spotify_status
    @source_spotify_status ||= "Unknown"
  end

  def spotify_settings
    @spotify_settings ||= SpotifySettings.new
  end

  def spotify_settings_sync
    Browser::HTTP.get("/api/sources/spotify/settings") do |request|
      request.on :success do |response|
        @spotify_settings = SpotifySettings.new(response.json)
        render!
      end
    end
  end

  def spotify_settings_save
    Browser::HTTP.post("/api/sources/spotify/settings", data=@spotify_settings.to_json) do |request|
      request.content_type("application/json")
      request.on :success do |response|
        render!
      end
    end
  end

  def source_local_scanning?
    @scanning ||= false
  end

  def source_local_scan
    unless source_local_scanning?
      Browser::HTTP.post("/api/sources/local/scan") do |request|
        request.on :success do |response|
          puts "scanning done"
          @scanning = false
          render!
        end
      end
      @scanning = true
      render!
    end
  end

  def stream
    @stream
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
        trigger :album_details
      end
    end
  end

  def album_details
    @album
  end

  def album_details_clear
    @album = nil
    render!
  end

  def play(id)
    message_channel_send({ event: :play, data: id })
  end

  def player_play
    message_channel_send({ event: :play, data: nil })
  end

  def player_stop
    message_channel_send({ event: :stop, data: nil })
  end

  def player_skip
    message_channel_send({ event: :skip, data: nil })
  end

  def queue(id)
    message_channel_send({ event: :queue, data: id })
  end

  private

  def hidden?
    `document.hidden`
  end

  def stream_data_sync(stream_id)
    message_channel_send({ :event => :stream_data_sync, :data => stream_id })
  end

  def handle_source_notification(data)
    case data['source_name']
    when 'local'
      @source_local_status = data['message']
    when 'spotify'
      @source_spotify_status = data['message']
    end
  end

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
    when "player_state"
      case message['data']['state']
      when 0
        @player_state = :stopped
      when 1
        @player_state = :playing
      end
      trigger :player_state_changed unless hidden?
    when "queue_update"
      if data = message['data']
        notifications << QueueUpdate.new(data)
        trigger :notification unless hidden?
      end
    when "stream_begin"
      if data = message['data']
        @stream = Stream.new(data)
        stream_data_sync(@stream.id())
        notifications << StreamBegin.new(data)
        trigger :stream_changed unless hidden?
      end
    when "stream_progress"
      if data = message['data']
        if @stream
          @stream.length = data["length"]
          @stream.duration = data['duration']
        else
          @stream = Stream.new(data)
          stream_data_sync(@stream.id())
        end
        trigger :stream_changed unless hidden?
      end
    when "stream_end"
      @stream = nil
      trigger :stream_changed unless hidden?
    when "stream_data"
      if data = message['data']
        @stream.track = Track.new(data['track'])
        trigger :stream_changed unless hidden?
        trigger :notification unless hidden?
      end
    when "source_notification"
      handle_source_notification(message['data'])
      # NOTE: This render! should probably be an event as well.
      render! unless hidden?
    end
  end

  def message_channel_start
    ws_url = "ws://#{$document.location.host}"
    @channel = Browser::Socket.new ws_url do |ws|
    #@channel = Browser::Socket.new "ws://#{$document.location.host}" do # Does not work!!!
      ws.on :open do
        $document.at('#connection-lost-overlay').hide
        message_queue_flush
      end

      ws.on :close do
        $document.at('#connection-lost-overlay').show
        after 5 do
          message_channel_start
        end
      end

      ws.on :message do |e|
        message_channel_recv(JSON.parse(e.data))
      end
    end
  end
end
