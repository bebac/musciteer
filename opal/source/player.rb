module PlayerActions
  def stop
    store.dispatch({ type: :player_stop })
  end

  def play
    store.dispatch({ type: :player_start })
  end

  def skip
    store.dispatch({ type: :player_skip })
  end
end

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

module PlayerAssets
  def play_svg
    <<-svg
<svg viewBox="0 0 200 200">
  <g>
    <circle cx="100" cy="100" r="92" fill="none" stroke-width="8"/>
    <path d="M85,50v100l50,-50"/>
  </g>
</svg>
    svg
  end

  def stop_svg
    <<-svg
<svg viewBox="0 0 200 200">
  <g>
    <circle cx="100" cy="100" r="92" fill="none" stroke-width="8"/>
    <path d="M75,75h50v50h-50"/>
  </g>
</svg>
    svg
  end

  def skip_svg
    <<-svg
<svg viewBox="0 0 200 200">
  <g  stroke-width="8">
    <circle cx="100" cy="100" r="92" fill="none"/>
    <path d="M 90,60 120,100 90,140" fill="none"/>
  </g>
</svg>
    svg
  end
end

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
