module Musciteer
  class AlbumDetailsDiscHeader
    include Maquette::Component

    attr_reader :disc_number

    def initialize(disc_number)
      @disc_number = disc_number
    end

    def render
      h 'li.disc-header', "Disc #{disc_number}"
    end
  end
end