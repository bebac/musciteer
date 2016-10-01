class Notification < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @timer = every!(5) do
      hide
      @timer.stop
    end
  end

  def show
    $document.at('#notification-overlay').show
  end

  def hide
    $document.at('#notification-overlay').hide
  end

  def restart_timer
    @timer.stop
    @timer.start
  end

  def notification
    store.state[:notification]
  end

  def render_notification(h1, h2, h3)
    [
      (
        h 'div.header' do
          h 'h1', h1
        end
      ),
      (
        h 'div.body' do
          [
            (h 'h2', h2),
            (h 'h3', h3)
          ]
        end
      )
    ]
  end

  def render
    h 'div#notification-container' do
      h 'div#notification' do
        if notification
          # If it is a new notification, show it and start a timer to
          # hide it.
          unless notification.shown
            show
            restart_timer
            notification.shown = true
          end
          # Render notification.
          case notification
          when QueueUpdate
            render_notification(
              notification.queue_size,
              notification.track.title,
              notification.track.artists
            )
          when StreamBegin
            render_notification(
              ">",
              notification.track.title,
              notification.track.artists
            )
          end
        end
      end
    end
  end
end
