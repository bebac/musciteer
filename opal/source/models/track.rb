class Track
  attr_accessor :id
  attr_accessor :title
  attr_accessor :tn
  attr_accessor :dn
  attr_accessor :duration
  attr_accessor :play_count
  attr_accessor :skip_count

  def initialize(attrs)
    attrs.each do |k,v|
      instance_variable_set("@#{k}", v) unless v.nil?
    end
  end

  ###
  # Combine disc number and track number into one number used
  # to sort tracks.

  def index
    (dn << 8) + tn
  end

  def artists
    names = Array(@artists).collect { |artist| artist['name'] }
    names.join(", ")
  end

  def album
    @album["title"]
  end

  def album_cover_path
    @album["cover"]
  end

  def duration_formatted
    secs = duration / 1000
    min = Integer(secs / 60) % 60;
    sec = Integer(secs % 60);
    "%d:%02d" % [ min, sec ]
  end
end
