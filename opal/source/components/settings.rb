class Settings < Maquette::Component
  attr_reader :store

  def initialize(store)
    @store = store
    @ctpb = ContinuousPlaybackSettings.new(store)
    @audio = AudioSettings.new(store)
    @local_source = LocalSourceSettings.new(store)
    @spotify_source = SpotifySourceSettings.new(store)
  end

  def render_header
    h 'div#settings-header' do
      h 'h1', "Settings"
    end
  end

  def render_section(section)
    h 'section' do
      section.render
    end
  end

  def render
    h "div#settings" do
      [
        render_header,
        render_section(@ctpb),
        render_section(@audio),
        render_section(@local_source),
        render_section(@spotify_source)
      ]
    end
  end
end