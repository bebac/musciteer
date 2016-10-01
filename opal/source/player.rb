module ActionDispatchHooks
  def player_connection_open
    $document.at('#connection-lost-overlay').hide
  end

  def player_connection_close
    $document.at('#connection-lost-overlay').show
  end

  def player_start
    message_channel_send({ event: :play, data: nil })
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
