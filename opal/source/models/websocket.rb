class Websocket
  attr_reader :store

  def initialize(store)
    @store = store
    open
  end

  def send(message)
    if @socket.alive?
      @socket.send(message.to_json)
    else
      queue << message
    end
  end

  private

  def queue
    @queue ||= Array.new
  end

  def queue_flush
    while message=queue.pop
      send(message)
    end
  end

  def message_channel_recv(message)
    case message['event']
    when "player_state"
      if state = message['data']['state']
        store.dispatch({ type: (state == 0 ? :player_stopped : :player_playing) })
      else
        fail "received invalid player state message"
      end
    when "queue_update"
      puts "Websocket queue update #{message['data']}"
      if data = message['data']
        store.dispatch({ type: :notification, data: data })
      end
    when "stream_begin"
      if data = message['data']
        store.dispatch({ type: :stream_begin, data: data })
      end
    when "stream_end"
      store.dispatch({ type: :stream_end, data: data })
    when "stream_progress"
      if data = message['data']
        store.dispatch({ type: :stream_progress, data: data })
      end
    when "stream_data"
      if data = message['data']
        store.dispatch({ type: :stream_sync, data: data })
      end
    when "source_notification"
      if data = message['data']
        if source_name = data['source_name']
          case source_name
          when 'spotify'
            store.dispatch({ type: :spotify_source_notify, data: data['message'] })
          end
        end
      end
    end
  end

  def open
    ws_url = "ws://#{$document.location.host}"
    @socket = Browser::Socket.new ws_url do |ws|
    #@channel = Browser::Socket.new "ws://#{$document.location.host}" do # Does not work!!!
      ws.on :open do
        store.dispatch({ type: :player_connection_open })
        queue_flush
      end

      ws.on :close do
        store.dispatch({ type: :player_connection_close })
        after 5 do
          open
        end
      end

      ws.on :message do |e|
        message_channel_recv(JSON.parse(e.data))
      end
    end
  end
end
