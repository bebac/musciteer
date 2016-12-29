class Layout < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @top = Top.new(store)
    @bottom = Bottom.new(store)
    @router = Router.new(store)
    @settings = Settings.new(store)
    @notification = Notification.new(store)
  end

  def hide_settings(evt)
    if evt.target.id == 'settings-overlay'
      $document.at('#settings-overlay').hide
    end
  end

  def render_main
    h 'div#main' do
      [
        (
          h 'div#menu' do
            @router.render_navigation
          end
        ),
        (
          h 'div#top' do
            @top.render
          end
        ),
        (
          h 'div#content' do
            @router.render_path
          end
        ),
        (
          h 'div#bottom-container' do
            @bottom.render
          end
        )
      ]
    end
  end

  def render_settings_overlay
    h 'div#settings-overlay', onclick: handler(:hide_settings) do
      @settings.render
    end
  end

  def render_notification_overlay
    h 'div#notification-overlay' do
      @notification.render
    end
  end

  def render_connection_lost_overlay
    h 'div#connection-lost-overlay' do
      h 'div', "Connection Lost"
    end
  end

  def render
    h 'div' do
      [
        render_main,
        render_settings_overlay,
        render_notification_overlay,
        render_connection_lost_overlay
      ]
    end
  end

  def fold_up_svg
    <<-svg
<svg viewBox="0 0 200 200">
  <g>
    <path d="m25,75l75,50l75,-50" fill="none"/>
  </g>
</svg>
    svg
  end
end
