module Musciteer
  module ActionDispatchHooks
    #def stream_begin(data)
    #  message_channel_send({ :event => :stream_data_sync, :data => data[:stream_id] })
    #  @store.dispatch type: :stream_sync
    #end

    #def stream_progress(data)
    #  unless state[:stream_sync]
    #    message_channel_send({ :event => :stream_data_sync, :data => data[:stream_id] })
    #  end
    #end
  end
end