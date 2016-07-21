class Notification
  include Inesita::Component

  def initialize
    @is_leaf = true
  end

  def init
    @timer = every!(5) do
      $document.at('#notification-overlay').hide
      @timer.stop
      store.notifications.clear
      render!(self)
    end
    store.on(:notification) { render!(self) }
  end

  def show_overlay
    $document.at('#notification-overlay').show
  end

  def restart_timer
    @timer.stop
    @timer.start
  end

  def show(notification)
    show_overlay
    restart_timer
    notification.shown = true
  end

  def render
    div id: 'notification-container' do
      div id: 'notification' do
        if not store.notifications.empty?
          n = store.notifications.last
          case n
          when QueueUpdate
            render_queue_update(n)
            show(n) unless n.shown
          when StreamBegin
            if stream = store.stream
              if track = stream.track
                render_stream_data(track)
                show(n) unless n.shown
              end
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

  def render_stream_data(notification)
    div do
      h1 do; text '>'; end
    end
    div do
      h2 do; text notification.title; end
      h3 do; text notification.artists; end
    end
  end
end
