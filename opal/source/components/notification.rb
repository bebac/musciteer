class Notification
  include Inesita::Component

  def init
    puts "init Notification"
    @timer = every!(5)
  end

  def duration(secs)
    @timer = every!(secs) do
      store.notifications.shift
      if store.notifications.empty?
        $document.at('#notification-overlay').hide
        @timer.stop
      else
        @timer.start
      end
    end
    @timer.start
  end

  def render
    div id: 'notification-container' do
      div id: 'notification' do
        if not store.notifications.empty?
          n = store.notifications.last
          case n
          when QueueUpdate
            render_queue_update(n)
            duration(5) if @timer.stopped?
          when StreamBegin
            if stream_data = store.stream_data(n.stream_id)
              render_stream_begin(stream_data)
            else
              div do; end
            end
            duration(5) if @timer.stopped?
          end
        end
      end
    end
  end

  def render_queue_update(notification)
    div do
      h1 do; text notification.queue_size; end
    end
    div do
      h2 do; text notification.track.title; end
      h3 do; text notification.track.artists; end
    end
  end

  def render_stream_begin(notification)
    div do
      h1 do; text '>'; end
    end
    div do
      h2 do; text notification.title; end
      h3 do; text notification.artists; end
    end
  end
end
