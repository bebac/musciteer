module Musciteer
  class AlbumDetailsTrack
    include Maquette::Component

    attr_reader :track
    attr_reader :store

    def initialize(track, store)
      @track = track
      @store = store
      #@play = PlayButton.new(store, track.id)
    end

    def queue(evt)
      store.dispatch type: :player_queue, data: track.id
    end

    def render
      h 'li', { onclick: handler(:queue) } do
        [
          (
            h 'div' do
              [
                (h 'div.tn', track.tn),
                #@play.render,
                (h 'div.title', track.title)
              ]
            end
          ),
          (
            h 'div' do
              [
                (h 'div.duration', track.duration_formatted)
              ]
            end
          )
        ]
      end
    end
  end
end