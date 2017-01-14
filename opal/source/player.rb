module PlayerState
  def playing?
    store.state[:player_state] == :playing
  end

  def stopped?
    store.state[:player_state] == :stopped
  end

  def stream_synced?
    store.state[:stream_sync]
  end

  def stream
    store.state[:stream]
  end

  def track
    stream[:track]
  end
end

module ActionDispatchHooks
  def player_connection_open
    $document.at('#connection-lost-overlay').hide
  end

  def player_connection_close
    $document.at('#connection-lost-overlay').show
  end

  def player_start(track_id)
    message_channel_send({ event: :play, data: track_id })
  end

  def player_stop
    message_channel_send({ event: :stop, data: nil })
  end

  def player_skip
    message_channel_send({ event: :skip, data: nil })
  end

  def player_queue(id)
    message_channel_send({ event: :queue, data: id })
  end

  def stream_begin(data)
    message_channel_send({ :event => :stream_data_sync, :data => data[:stream_id] })
  end

  def stream_progress(data)
    unless state[:stream_sync]
      message_channel_send({ :event => :stream_data_sync, :data => data[:stream_id] })
    end
  end
end
