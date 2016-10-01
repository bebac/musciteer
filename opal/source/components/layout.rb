class Layout < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @top = Top.new(store)
    @router = Router.new(store)
    @album_details = AlbumDetails.new(store)
    @settings = Settings.new(store)
    @notification = Notification.new(store)
    @player_control = PlayerControl.new(store)
  end

  def hide_album_details(evt)
    if evt.target.id == 'album-details-overlay'
      $document.at('#album-details-overlay').hide
    end
  end

  def hide_settings(evt)
    if evt.target.id == 'settings-overlay'
      $document.at('#settings-overlay').hide
    end
  end

  def hide_player_control(evt)
    if evt.target.id == 'player-control-overlay'
      $document.at('#player-control-overlay').hide
    end
  end

  def render_top
    h 'div#top' do
      @top.render
    end
  end

  def render_bottom
    h 'div#bottom' do
      [
        (
          h 'div#menu' do
            @router.render_navigation
          end
        ),
        (
          h 'div#content' do
            @router.render_path
          end
        )
      ]
    end
  end

  def render_album_details_overlay
    h 'div#album-details-overlay', onclick: handler(:hide_album_details) do
      @album_details.render
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

  def render_player_control_overlay
    h 'div#player-control-overlay', onclick: handler(:hide_player_control) do
      @player_control.render
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
        render_top,
        render_bottom,
        render_album_details_overlay,
        render_settings_overlay,
        render_notification_overlay,
        render_player_control_overlay,
        render_connection_lost_overlay
      ]
    end
  end
end
