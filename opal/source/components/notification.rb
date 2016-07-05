class Notification
  include Inesita::Component

  def init
    @timer = every!(5) do
      if store.notifications.empty?
        $document.at('#notification-overlay').hide
        @timer.stop
      end
    end
  end

  def show_overlay
    $document.at('#notification-overlay').show
  end

  def restart_timer
    @timer.stop
    @timer.start
  end

  def show
    show_overlay
    restart_timer
    store.notifications.shift
  end

  def render
    div id: 'notification-container' do
      div id: 'notification' do
        if not store.notifications.empty?
          n = store.notifications.last
          case n
          when QueueUpdate
            render_queue_update(n)
            show
          when StreamBegin
            if stream_data = store.stream_data(n.stream_id)
              render_stream_begin(stream_data)
              show
            else
              div
            end
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
