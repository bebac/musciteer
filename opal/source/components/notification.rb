module Musciteer
  class Notification
    include Maquette::Component

    def initialize(store)
      @store = store
      @timer = every!(2.5) do
        hide
        @timer.stop
      end
    end

    def restart_timer
      @timer.stop
      @timer.start
    end

    def show
      $document.at('#notification').style[:display] = :flex
    end

    def hide
      $document.at('#notification').style[:display] = :none
    end

    def queue_update
      @store.state[:queue_update]
    end

    def render_queue_update
      [
        (
          h 'div.header' do
            h 'h1', queue_update.queue_size
          end
        ),
        (
          h 'div.body' do
            [
              (h 'h2', queue_update.track.title),
              (h 'h3', queue_update.track.artists)
            ]
          end
        )
      ]
    end

    def render
      h 'div#notification' do
        if queue_update
          # If it is a new notification, show it and start a timer to
          # hide it.
          unless queue_update.shown
            show
            restart_timer
            queue_update.shown = true
          end

          render_queue_update
        end
      end
    end
  end
end