class MessageChannel
  attr_reader :store

  def initialize(store)
    @store = store
    channel_start
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
      if data = message['data']
        store.dispatch({ type: :notification, data: QueueUpdate.new(data) })
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
        store.dispatch({ type: :stream_sync, data: StreamBegin.new(data) })
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

  def channel_start
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
          channel_start
        end
      end

      ws.on :message do |e|
        message_channel_recv(JSON.parse(e.data))
      end
    end
  end
end

module ActionDispatchHooks
end

class Store
  attr_reader :state

  include ActionDispatchHooks

  def initialize(initial_state = nil, &reducer)
    @state = initial_state
    @reducer = reducer || fail("reducer is required")
    @subscribers = []
    @message_channel = MessageChannel.new(self)
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

  def message_channel_send(message)
    @message_channel.send(message)
  end
end
