class Album
  attr_accessor :id
  attr_accessor :title
  attr_accessor :cover

  def initialize(attrs)
    attrs.each do |k,v|
      case k
      when 'tracks'
        @tracks = v.map do |track|
          Track.new(track)
        end
      else
        instance_variable_set("@#{k}", v) unless v.nil?
      end
    end
  end

  def artist_id
    @artist['id']
  end

  def artist
    @artist['name']
  end

  def tracks
    @tracks ||= []
  end

  def tracks=(value)
    @tracks = value
  end
end
